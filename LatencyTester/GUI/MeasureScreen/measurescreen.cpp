#include "measurescreen.h"
#include "ui_measurescreen.h"
#include "appsettings.h"
#include "qcustomplot.h"
#include <QEvent>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

MeasureScreen::MeasureScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MeasureScreen)
    , mMeasure{}
{
    ui->setupUi(this);

    // Initialize slider labels with current values and units
    ui->timeFactorLabel->setText(tr("Intérvalo entre mediciones: %1 ms").arg(ui->TimeFactorSlider->value()));
    ui->DurationLabel->setText(tr("Duración: %1 s").arg(ui->DurationSlider->value()));
}

MeasureScreen::~MeasureScreen()
{
    delete ui;
}

void MeasureScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        // Refresh slider labels with translated text
        ui->timeFactorLabel->setText(tr("Intérvalo entre mediciones: %1 ms").arg(ui->TimeFactorSlider->value()));
        ui->DurationLabel->setText(tr("Duración: %1 s").arg(ui->DurationSlider->value()));
    }
    QWidget::changeEvent(event);
}

void MeasureScreen::on_backButton_released()
{
    emit backRequested();
}

void MeasureScreen::on_startMeasureButton_released()
{
    // Clear previous measurement data
    mMeasure.lantencies.clear();
    mMeasure.meanLatency = 0;
    mMeasure.date = QDateTime::currentDateTime();

    // Step 1: Disable UI, keep stop enabled
    setMeasureWidgetsEnabled(false);
    ui->stopMeasureButton->setEnabled(true);

    // Step 2: Start indefinite flash on start button (durationMs = 0)
    flashWidget(ui->startMeasureButton, {QColor(Qt::green), QColor(Qt::darkGreen)}, 0, 500);

    // Step 3: Run measurement in background thread
    auto future = QtConcurrent::run([this]() -> bool {
        return mSensorOperator.takeMeasure(mMeasure);
    });

    // When operation finishes: stop activity flash, then start result flash
    auto* watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
        ui->stopMeasureButton->setEnabled(false);
        bool success = watcher->result();

        // Stop the activity flash (no callback yet — we chain manually)
        mFlashOnFinished = nullptr;
        stopFlash();

        // Step 4: Result flash, then re-enable UI and plot when done
        QList<QColor> resultColors = success
            ? QList<QColor>{QColor(Qt::green), QColor(Qt::transparent)}
            : QList<QColor>{QColor(Qt::red), QColor(Qt::transparent)};

        flashWidget(ui->startMeasureButton, resultColors, 3000, 500, [this, success]() {
            setMeasureWidgetsEnabled(true);
            if (success)
            {
                plotMeasure();
            }
        });
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}

void MeasureScreen::on_stopMeasureButton_released()
{
    // Signal the sensor operator to stop any ongoing operation
    mSensorOperator.stopMeasure();

    // Stop any ongoing flash and restore UI immediately
    mFlashOnFinished = nullptr;
    stopFlash();
    setMeasureWidgetsEnabled(true);
    ui->stopMeasureButton->setEnabled(false);
}

void MeasureScreen::on_calibrateButton_released()
{
    // Step 1: Disable UI, keep stop enabled
    setMeasureWidgetsEnabled(false);
    ui->stopMeasureButton->setEnabled(true);

    // Step 2: Start indefinite flash on calibrate button (durationMs = 0)
    flashWidget(ui->calibrateButton, {QColor(Qt::yellow), QColor(Qt::darkYellow)}, 0, 500);

    // Step 3: Run calibration in background thread
    auto future = QtConcurrent::run([this]() -> bool {
        return mSensorOperator.calibrateSensor();
    });

    // When operation finishes: stop activity flash, then start result flash
    auto* watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
        bool success = watcher->result();

        // Stop the activity flash (no callback yet — we chain manually)
        mFlashOnFinished = nullptr;
        stopFlash();

        // Step 4: Result flash (green/red), then re-enable UI when done
        QList<QColor> resultColors = success
            ? QList<QColor>{QColor(Qt::green), QColor(Qt::transparent)}
            : QList<QColor>{QColor(Qt::red), QColor(Qt::transparent)};

        flashWidget(ui->calibrateButton, resultColors, 3000, 500, [this]() {
            setMeasureWidgetsEnabled(true);
        });
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}

void MeasureScreen::on_TimeFactorSlider_valueChanged(int value)
{
    mMeasure.timeFactor = value;
    ui->timeFactorLabel->setText(tr("Intérvalo entre mediciones: %1 ms").arg(value));
}

void MeasureScreen::on_DurationSlider_valueChanged(int value)
{
    mMeasure.duration = value * 1000; // Slider in seconds, stored in ms
    ui->DurationLabel->setText(tr("Duración: %1 s").arg(value));
}

void MeasureScreen::plotMeasure()
{
    // Guard: don't plot if there's no valid data
    if (mMeasure.lantencies.isEmpty())
    {
        return;
    }

    // Filter out failed measurements (-1) and zero values for plotting
    // Use parallel filtering for large datasets
    const int dataSize = mMeasure.lantencies.size();

    // Pre-allocate with max possible size to avoid reallocations
    QVector<double> validLatencies;
    QVector<double> x_axis;
    validLatencies.reserve(dataSize);
    x_axis.reserve(dataSize);

    const double timeStep = mMeasure.timeFactor / 1000.0;
    const auto& latencies = mMeasure.lantencies;

    if (dataSize > 100)
    {
        // Parallel: partition data across threads and merge results
        const int threadCount = QThread::idealThreadCount();
        const int chunkSize = (dataSize + threadCount - 1) / threadCount;

        QVector<QVector<double>> threadLatencies(threadCount);
        QVector<QVector<double>> threadXAxis(threadCount);

        QVector<QFuture<void>> futures;
        futures.reserve(threadCount);

        for (int t = 0; t < threadCount; ++t)
        {
            const int start = t * chunkSize;
            const int end = qMin(start + chunkSize, dataSize);
            threadLatencies[t].reserve(end - start);
            threadXAxis[t].reserve(end - start);

            futures.append(QtConcurrent::run([&latencies, &threadLatencies, &threadXAxis, start, end, t, timeStep]() {
                for (int i = start; i < end; ++i)
                {
                    if (latencies[i] > 0)
                    {
                        threadLatencies[t].append(latencies[i]);
                        threadXAxis[t].append(i * timeStep);
                    }
                }
            }));
        }

        // Wait for all threads and merge
        for (auto& future : futures)
        {
            future.waitForFinished();
        }

        for (int t = 0; t < threadCount; ++t)
        {
            validLatencies.append(threadLatencies[t]);
            x_axis.append(threadXAxis[t]);
        }
    }
    else
    {
        // Sequential for small datasets (threading overhead not worth it)
        for (int i = 0; i < dataSize; ++i)
        {
            if (latencies[i] > 0)
            {
                validLatencies.append(latencies[i]);
                x_axis.append(i * timeStep);
            }
        }
    }

    // Guard: need at least one valid point to plot
    if (validLatencies.isEmpty())
    {
        return;
    }

    // Ensure graph exists
    if (ui->plotMeasures->graphCount() > 0)
    {
        ui->plotMeasures->removeGraph(0);
    }
    ui->plotMeasures->addGraph();
    ui->plotMeasures->graph(0)->setData(x_axis, validLatencies);

    // Axis labels
    ui->plotMeasures->yAxis->setLabel("Latencias (ms)");
    ui->plotMeasures->xAxis->setLabel("Timeline (s)");

    // X range: total duration in seconds
    double xMax = mMeasure.duration / 1000.0;
    if (xMax <= 0)
    {
        xMax = 10;
    }
    ui->plotMeasures->xAxis->setRange(0, xMax);

    // Y range: based on actual data, with minimum range of 10ms
    double yMax = mMeasure.meanLatency * 2;
    if (yMax < 10)
    {
        yMax = 100;
    }
    ui->plotMeasures->yAxis->setRange(0, yMax);

    ui->plotMeasures->graph(0)->setLineStyle(QCPGraph::LineStyle::lsStepRight);

    QCPScatterStyle scatter;
    scatter.setShape(QCPScatterStyle::ssCircle);
    scatter.setPen(QPen(Qt::white));
    scatter.setSize(AppSettings::instance().fontSize());

    if (AppSettings::instance().daltonicMode())
    {
        scatter.setBrush(Qt::red);
        ui->plotMeasures->graph(0)->setPen(QPen(Qt::darkGreen));
    }
    else
    {
        scatter.setBrush(Qt::yellow);
        ui->plotMeasures->graph(0)->setPen(QPen(Qt::blue));
    }

    ui->plotMeasures->graph(0)->setScatterStyle(scatter);
    ui->plotMeasures->replot();
}

void MeasureScreen::flashWidget(QWidget* widget, const QList<QColor>& colors, int durationMs, int intervalMs, std::function<void()> onFinished)
{
    if (!widget || colors.isEmpty())
    {
        if (onFinished) onFinished();
        return;
    }

    // Stop any previous flash
    stopFlash();

    // Save state
    mFlashWidget = widget;
    mFlashOriginalStyle = widget->styleSheet();
    mFlashOnFinished = onFinished;

    int* colorIndex = new int(0);
    QList<QColor> colorsCopy = colors;

    mFlashTimer = new QTimer(this);
    connect(mFlashTimer, &QTimer::timeout, this, [this, colorIndex, colorsCopy]() {
        const QColor& color = colorsCopy[*colorIndex % colorsCopy.size()];
        if (mFlashWidget)
        {
            mFlashWidget->setStyleSheet(QStringLiteral("background-color: %1;").arg(color.name()));
        }
        ++(*colorIndex);
    });

    // If durationMs > 0, auto-stop after that time; if 0, flash indefinitely until stopFlash()
    if (durationMs > 0)
    {
        QTimer::singleShot(durationMs, this, [this, colorIndex]() {
            delete colorIndex;
            stopFlash();
        });
    }

    mFlashTimer->start(intervalMs);
}

void MeasureScreen::stopFlash()
{
    if (mFlashTimer && mFlashTimer->isActive())
    {
        mFlashTimer->stop();
    }
    if (mFlashTimer)
    {
        mFlashTimer->deleteLater();
        mFlashTimer = nullptr;
    }
    if (mFlashWidget)
    {
        mFlashWidget->setStyleSheet(mFlashOriginalStyle);
        mFlashWidget = nullptr;
    }
    if (mFlashOnFinished)
    {
        auto callback = mFlashOnFinished;
        mFlashOnFinished = nullptr;
        callback();
    }
}

void MeasureScreen::setMeasureWidgetsEnabled(bool enabled)
{
    ui->startMeasureButton->setEnabled(enabled);
    ui->DurationSlider->setEnabled(enabled);
    ui->TimeFactorSlider->setEnabled(enabled);
    ui->calibrateButton->setEnabled(enabled);
    ui->backButton->setEnabled(enabled);
}

#include "registrydisplayscreen.h"
#include "ui_registrydisplayscreen.h"
#include "appsettings.h"
#include "qcustomplot.h"
#include <QEvent>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

RegistryDisplayScreen::RegistryDisplayScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistryDisplayScreen)
{
    ui->setupUi(this);
}

RegistryDisplayScreen::~RegistryDisplayScreen()
{
    delete ui;
}

void RegistryDisplayScreen::displayMeasure(const Measures& measure)
{
    ui->nameText->setText(measure.name);
    ui->dateTimeEdit->setDateTime(measure.date);
    ui->latencyText->setText(QString::number(measure.meanLatency, 'f', 1));
    ui->timeText->setText(QString::number(measure.timeFactor));
    plotMeasure(measure);
}

void RegistryDisplayScreen::plotMeasure(const Measures& measure)
{
    // Guard: don't plot if there's no valid data
    if (measure.lantencies.isEmpty())
    {
        return;
    }

    const int dataSize = measure.lantencies.size();
    QVector<double> validLatencies;
    QVector<double> x_axis;
    validLatencies.reserve(dataSize);
    x_axis.reserve(dataSize);

    const double timeStep = measure.timeFactor / 1000.0;
    const auto& latencies = measure.lantencies;

    if (dataSize > 100)
    {
        // Parallel filtering for large datasets
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
        // Sequential for small datasets
        for (int i = 0; i < dataSize; ++i)
        {
            if (latencies[i] > 0)
            {
                validLatencies.append(latencies[i]);
                x_axis.append(i * timeStep);
            }
        }
    }

    // Guard: need at least one valid point
    if (validLatencies.isEmpty())
    {
        return;
    }

    // Setup graph
    if (ui->plotMeasures->graphCount() > 0)
    {
        ui->plotMeasures->removeGraph(0);
    }
    ui->plotMeasures->addGraph();
    ui->plotMeasures->graph(0)->setData(x_axis, validLatencies);

    ui->plotMeasures->yAxis->setLabel("Latencias (ms)");
    ui->plotMeasures->xAxis->setLabel("Timeline (s)");

    double xMax = measure.duration / 1000.0;
    if (xMax <= 0) xMax = 10;
    ui->plotMeasures->xAxis->setRange(0, xMax);

    double yMax = measure.meanLatency * 2;
    if (yMax < 10) yMax = 100;
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

void RegistryDisplayScreen::on_backButton_released()
{
    emit backRequested();
}

void RegistryDisplayScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

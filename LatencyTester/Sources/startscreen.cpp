#include "startscreen.h"
#include "ui_startscreen.h"
#include "extensionfiledelegate.h"
#include <QApplication>
#include <QList>
#include <QStyle>
#include <QCommonStyle>
#include <QSettings>
#include <QFileSystemModel>
#include <QDebug>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QThread>

const QString BACKBUTTONSTR{"Back_Button"};
const QString MEASURES{"/Measures"};
const QString MS{"Latencias (ms)"};
const QString TIMELINE{"Timeline (s)"};
const QString ENGLISH{":/i18n/LatencyTester_en_EN.qm"};
const QString POLSKI{":/i18n/LatencyTester_pl_PL.qm"};
const QString APLICAR{"Aplicar"};
const QString CANCELAR{"Cancelar"};
const QString APPLY{"Apply"};
const QString CANCEL{"Cancel"};
const QString STOSOWAC{"Stosować"};
const QString ANULOWAC{"Anulować"};

StartScreen::StartScreen(QWidget *parent)
    : QMainWindow{parent},
      ui{new Ui::StartScreen},
      mCurrentScreen{MenuScreen::START_SCREEN},
      mRenameWindow{new RenamePopUp{this}},
      mDialog{new Dialog{this}},
      mBackTimer{new QTimer{this}},
      mMeasure{}
{
    init();
}

StartScreen::~StartScreen()
{
    delete ui;
}

void StartScreen::on_helpButton_released()
{
    transitionScreen(MenuScreen::HELP_SCREEN);
}

void StartScreen::on_generalInfoButton_released()
{
    transitionScreen(MenuScreen::GENERAL_INFO_SCREEN);
}

void StartScreen::on_historicOfMeasuresButton_released()
{
    transitionScreen(MenuScreen::MEASURES_REGISTRY_SCREEN);
}

void StartScreen::on_backButton_released()
{
    if(mBackTimer->isActive())
    {
        mBackTimer->stop();
    }

    mCurrentScreen = mBackButtonOutputs.value(mCurrentScreen);

    if(MenuScreen::START_SCREEN == mCurrentScreen)
    {
        ui->backButton->setVisible(false);
        ui->backButton->setEnabled(false);
    }

    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(mCurrentScreen)};
    setUpNextScreen(mCurrentScreenWidgets, nextWidgets);
    mCurrentScreenWidgets = nextWidgets;
}

void StartScreen::on_backButton_pressed()
{
    mBackTimer->start();
}

void StartScreen::on_usersManualButton_released()
{

}

void StartScreen::on_settingsButton_released()
{
    transitionScreen(MenuScreen::SETTINGS_SCREEN);
    ui->settingsButtonBox->setEnabled(false);
}

void StartScreen::on_languagesComboBox_currentIndexChanged(int index)
{
    mNextSettings.language = Languages(index);
    ui->settingsButtonBox->setEnabled(true);
}

void StartScreen::on_fontSizeSlider_valueChanged(int value)
{
    mNextSettings.fontSize = value;
    ui->fontSizeLabel->setText(tr("Tamaño de la fuente: %1 pt").arg(value));
    ui->settingsButtonBox->setEnabled(true);
}

void StartScreen::on_daltonicCheckbox_stateChanged(int arg1)
{
    mNextSettings.daltonicMode = arg1;
    ui->settingsButtonBox->setEnabled(true);
}

void StartScreen::on_darkModeCheckbox_stateChanged(int arg1)
{
    mNextSettings.darkMode = arg1;
    ui->settingsButtonBox->setEnabled(true);
}

void StartScreen::on_settingsButtonBox_clicked(QAbstractButton *button)
{
    if(QDialogButtonBox::ButtonRole::ApplyRole == ui->settingsButtonBox->buttonRole(button))
    {
        setSettings();
        ui->settingsButtonBox->setEnabled(false);
    }
    else
    {
        ui->languagesComboBox->setCurrentIndex(static_cast<int>(mCurrentSettings.language));
        ui->fontSizeSlider->setValue(mCurrentSettings.fontSize);
        ui->daltonicCheckbox->setChecked(mCurrentSettings.daltonicMode);
        ui->darkModeCheckbox->setChecked(mCurrentSettings.darkMode);
        mNextSettings = mCurrentSettings;
        ui->settingsButtonBox->setEnabled(false);
    }
}

void StartScreen::on_checkRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = (QFileSystemModel*)ui->registryTreeView->model();

    if(ui->registryTreeView->currentIndex().isValid())
    {
        if(mJsonOperator.loadFileFromDisk(model->filePath(ui->registryTreeView->currentIndex())))
        {
            mJsonOperator.parseJsonToStruct(mMeasure);
            ui->nameText->setText(mMeasure.name);
            ui->dateTimeEdit->setDateTime(mMeasure.date);
            ui->latencyText->setText(QString::number(mMeasure.meanLatency));
            ui->timeText->setText(QString::number(mMeasure.timeFactor));
            plotMeasure();
            transitionScreen(MenuScreen::REGISTRY_DISPLAYER_SCREEN);
        }
        else
        {
            mDialog->setFileNameText(model->fileName(ui->registryTreeView->currentIndex()));
            mDialog->show();
        }
    }
}

void StartScreen::on_deleteRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = (QFileSystemModel*)ui->registryTreeView->model();
    model->remove(ui->registryTreeView->currentIndex());
}

void StartScreen::on_renameRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = (QFileSystemModel*)ui->registryTreeView->model();

    if(ui->registryTreeView->currentIndex().isValid())
    {
        ui->renameRegistryEntryButton->setDisabled(true);
        ui->checkRegistryEntryButton->setDisabled(true);
        ui->deleteRegistryEntryButton->setDisabled(true);
        QString nameWithExtension = model->fileName(ui->registryTreeView->currentIndex());
        QStringList nameList = nameWithExtension.split(".", Qt::SkipEmptyParts, Qt::CaseInsensitive);
        QString name = nameList.first();
        mRenameWindow->setName(name);
        mRenameWindow->show();
    }
}

void StartScreen::backToStartScreen()
{
    // Emit signal to MainWindow to switch back to HomeScreen
    emit backToHome();
}

void StartScreen::navigateTo(MenuScreen screen)
{
    mCurrentScreen = screen;
    ui->backButton->setVisible(true);
    ui->backButton->setEnabled(true);

    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(screen)};
    setUpNextScreen(mCurrentScreenWidgets, nextWidgets);
    mCurrentScreenWidgets = nextWidgets;
}

void StartScreen::on_startMeasureButton_released()
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

void StartScreen::on_stopMeasureButton_released()
{
    // Signal the sensor operator to stop any ongoing operation
    mSensorOperator.stopMeasure();

    // Stop any ongoing flash and restore UI immediately
    mFlashOnFinished = nullptr;
    stopFlash();
    setMeasureWidgetsEnabled(true);
    ui->stopMeasureButton->setEnabled(false);
}

void StartScreen::on_startMeasuringButton_released()
{
    transitionScreen(MenuScreen::START_MEASURE_SCREEN);
}

void StartScreen::on_TimeFactorSlider_valueChanged(int value)
{
    mMeasure.timeFactor = value;
    ui->timeFactorLabel->setText(tr("Intérvalo entre mediciones: %1 ms").arg(value));
}

void StartScreen::on_DurationSlider_valueChanged(int value)
{
    mMeasure.duration = value * 1000; // Slider in seconds, stored in ms
    ui->DurationLabel->setText(tr("Duración: %1 s").arg(value));
}

void StartScreen::on_calibrateButton_released()
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
        mFlashOnFinished = nullptr; // Don't trigger callback on stop
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

void StartScreen::changedName(const QString& name)
{
    QPointer<QFileSystemModel> model = (QFileSystemModel*)ui->registryTreeView->model();
    QString currentNameWithExtension = model->fileName(ui->registryTreeView->currentIndex());
    QString path = model->rootPath();
    QDir file(path);
    if(!file.rename(currentNameWithExtension, name))
    {
        qDebug() << path << file.currentPath() << currentNameWithExtension << name;
    }
    reEnableRegistryButtons();
}

void StartScreen::reEnableRegistryButtons()
{
    ui->deleteRegistryEntryButton->setEnabled(true);
    ui->checkRegistryEntryButton->setEnabled(true);
    ui->renameRegistryEntryButton->setEnabled(true);
}

void StartScreen::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void StartScreen::plotMeasure()
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
    ui->plotMeasures->yAxis->setLabel(MS);
    ui->plotMeasures->xAxis->setLabel(TIMELINE);

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
    scatter.setSize(mCurrentSettings.fontSize);

    if (mCurrentSettings.daltonicMode)
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

void StartScreen::flashWidget(QWidget* widget, const QList<QColor>& colors, int durationMs, int intervalMs, std::function<void()> onFinished)
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

void StartScreen::stopFlash()
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

void StartScreen::setMeasureWidgetsEnabled(bool enabled)
{
    ui->startMeasureButton->setEnabled(enabled);
    ui->DurationSlider->setEnabled(enabled);
    ui->TimeFactorSlider->setEnabled(enabled);
    ui->calibrateButton->setEnabled(enabled);
    ui->backButton->setEnabled(enabled);
}

void StartScreen::init()
{
    widgetsMapInit();
    loadSettings();
    loadRegistry();
    connect(mBackTimer, &QTimer::timeout, this,  &StartScreen::backToStartScreen);
    mBackTimer->setInterval(3000);
    mBackTimer->setSingleShot(true);
}

void StartScreen::widgetsMapInit()
{
    ui->setupUi(this);

    QCommonStyle style;
    ui->backButton->setIcon(style.standardIcon(QStyle::SP_ArrowBack));
    ui->backButton->setAccessibleName(BACKBUTTONSTR);

    // Initialize slider labels with current values and units
    ui->fontSizeLabel->setText(tr("Tamaño de la fuente: %1 pt").arg(ui->fontSizeSlider->value()));
    ui->timeFactorLabel->setText(tr("Intérvalo entre mediciones: %1 ms").arg(ui->TimeFactorSlider->value()));
    ui->DurationLabel->setText(tr("Duración: %1 s").arg(ui->DurationSlider->value()));

    QAbstractButton* applyButton{new QPushButton(tr("Aplicar"), ui->settingsButtonBox)};
    QAbstractButton* cancelButton{new QPushButton(tr("Cancelar"), ui->settingsButtonBox)};

    ui->settingsButtonBox->addButton(applyButton, QDialogButtonBox::ButtonRole::ApplyRole);
    ui->settingsButtonBox->addButton(cancelButton, QDialogButtonBox::ButtonRole::RejectRole);

    mWidgets = {{MenuScreen::START_SCREEN, ui->startFrame},
                {MenuScreen::START_SCREEN, ui->helpButton},
                {MenuScreen::START_SCREEN, ui->settingsButton},
                {MenuScreen::START_SCREEN, ui->startMeasuringButton},
                {MenuScreen::START_SCREEN, ui->historicOfMeasuresButton},
                {MenuScreen::HELP_SCREEN, ui->helpFrame},
                {MenuScreen::HELP_SCREEN, ui->usersManualButton},
                {MenuScreen::HELP_SCREEN, ui->generalInfoButton},
                {MenuScreen::HELP_SCREEN, ui->backButton},
                {MenuScreen::GENERAL_INFO_SCREEN, ui->generalInfoFrame},
                {MenuScreen::GENERAL_INFO_SCREEN, ui->generalInfoText},
                {MenuScreen::GENERAL_INFO_SCREEN, ui->backButton},
                {MenuScreen::SETTINGS_SCREEN, ui->settingsFrame},
                {MenuScreen::SETTINGS_SCREEN, ui->languageLabel},
                {MenuScreen::SETTINGS_SCREEN, ui->languagesComboBox},
                {MenuScreen::SETTINGS_SCREEN, ui->backButton},
                {MenuScreen::SETTINGS_SCREEN, ui->fontSizeLabel},
                {MenuScreen::SETTINGS_SCREEN, ui->fontSizeSlider},
                {MenuScreen::SETTINGS_SCREEN, ui->settingsButtonBox},
                {MenuScreen::SETTINGS_SCREEN, ui->daltonicCheckbox},
                {MenuScreen::SETTINGS_SCREEN, ui->darkModeCheckbox},
                {MenuScreen::MEASURES_REGISTRY_SCREEN, ui->registryFrame},
                {MenuScreen::MEASURES_REGISTRY_SCREEN, ui->registryButtonsBox},
                {MenuScreen::MEASURES_REGISTRY_SCREEN, ui->renameRegistryEntryButton},
                {MenuScreen::MEASURES_REGISTRY_SCREEN, ui->deleteRegistryEntryButton},
                {MenuScreen::MEASURES_REGISTRY_SCREEN, ui->checkRegistryEntryButton},
                {MenuScreen::MEASURES_REGISTRY_SCREEN, ui->backButton},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->registryDisplayerFrame},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->latencyLabel},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->latencyText},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->timeLabel},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->timeText},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->nameLabel},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->nameText},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->dateLabel},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->dateTimeEdit},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->backButton},
                {MenuScreen::REGISTRY_DISPLAYER_SCREEN, ui->plotMeasures},
                {MenuScreen::START_MEASURE_SCREEN, ui->startMeasureFrame},
                {MenuScreen::START_MEASURE_SCREEN, ui->DurationLabel},
                {MenuScreen::START_MEASURE_SCREEN, ui->timeFactorLabel},
                {MenuScreen::START_MEASURE_SCREEN, ui->backButton},
                {MenuScreen::START_MEASURE_SCREEN, ui->DurationSlider},
                {MenuScreen::START_MEASURE_SCREEN, ui->TimeFactorSlider},
                {MenuScreen::START_MEASURE_SCREEN, ui->startMeasureButton},
                {MenuScreen::START_MEASURE_SCREEN, ui->plotMeasures},
                {MenuScreen::START_MEASURE_SCREEN, ui->calibrateButton}
               };

    auto it{mWidgets.begin()};
    while (it != mWidgets.end() )
    {
        if(MenuScreen::START_SCREEN != it.key())
        {
            it.value()->setVisible(false);
            it.value()->setEnabled(false);
        }
        ++it;
    }

    mBackButtonOutputs = {{MenuScreen::HELP_SCREEN, MenuScreen::START_SCREEN},
                          {MenuScreen::USERS_MANUAL_SCREEN, MenuScreen::START_MEASURE_SCREEN},
                          {MenuScreen::SETTINGS_SCREEN, MenuScreen::START_SCREEN},
                          {MenuScreen::GENERAL_INFO_SCREEN, MenuScreen::START_SCREEN},
                          {MenuScreen::START_MEASURE_SCREEN, MenuScreen::START_SCREEN},
                          {MenuScreen::MEASURES_REGISTRY_SCREEN, MenuScreen::START_SCREEN},
                          {MenuScreen::GENERAL_INFO_SCREEN, MenuScreen::HELP_SCREEN},
                          {MenuScreen::REGISTRY_DISPLAYER_SCREEN, MenuScreen::MEASURES_REGISTRY_SCREEN}
                         };
}

void StartScreen::setUpNextScreen(QList<QPointer<QWidget> > &toHide, QList<QPointer<QWidget> > &toShow)
{
    for(auto widget : toHide)
    {
        widget->setVisible(false);
        widget->setEnabled(false);
    }

    for(auto widget : toShow)
    {
        widget->setVisible(true);
        widget->setEnabled(true);
    }
}

void StartScreen::transitionScreen(MenuScreen nextScreen)
{
    mCurrentScreenWidgets = mWidgets.values(mCurrentScreen);
    mCurrentScreen = nextScreen;
    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(mCurrentScreen)};
    setUpNextScreen(mCurrentScreenWidgets, nextWidgets);
    mCurrentScreenWidgets = nextWidgets;
}

void StartScreen::setFontSize()
{
    QFont font;
    for (auto widget : std::as_const(mWidgets))
    {
        font = widget->font();
        font.setPointSize(mNextSettings.fontSize);
        widget->setFont(font);
        if (widget->accessibleName() != BACKBUTTONSTR)
        {
            widget->adjustSize();
        }
    }

    // Also update the stop button font (not in mWidgets)
    QFont stopFont = ui->stopMeasureButton->font();
    stopFont.setPointSize(mNextSettings.fontSize);
    ui->stopMeasureButton->setFont(stopFont);
    ui->stopMeasureButton->adjustSize();

    ui->fontSizeSlider->setSliderPosition(mNextSettings.fontSize);

    // Force all layouts to recalculate
    if (ui->startMeasureFrame->layout())
    {
        ui->startMeasureFrame->layout()->activate();
    }
    ui->startMeasureFrame->updateGeometry();
    this->centralWidget()->updateGeometry();
}

void StartScreen::setTranslation()
{
    // Always install the correct translator (needed at startup and on change)
    QList<QAbstractButton*> buttons = ui->settingsButtonBox->buttons();

    switch (mNextSettings.language)
    {
        case Languages::SPANISH:
        qApp->removeTranslator(&mTranslator);
        buttons[1]->setText(APLICAR);
        buttons[0]->setText(CANCELAR);
        break;
        case Languages::ENGLISH:
        qApp->removeTranslator(&mTranslator);
        (void)mTranslator.load(ENGLISH);
        qApp->installTranslator(&mTranslator);
        buttons[1]->setText(APPLY);
        buttons[0]->setText(CANCEL);
        break;
        case Languages::POLISH:
        qApp->removeTranslator(&mTranslator);
        (void)mTranslator.load(POLSKI);
        qApp->installTranslator(&mTranslator);
        buttons[1]->setText(STOSOWAC);
        buttons[0]->setText(ANULOWAC);
        break;
    }

    ui->languagesComboBox->setCurrentIndex(static_cast<int>(mNextSettings.language));

    // Refresh slider labels with translated text
    ui->fontSizeLabel->setText(tr("Tamaño de la fuente: %1 pt").arg(ui->fontSizeSlider->value()));
    ui->timeFactorLabel->setText(tr("Intérvalo entre mediciones: %1 ms").arg(ui->TimeFactorSlider->value()));
    ui->DurationLabel->setText(tr("Duración: %1 s").arg(ui->DurationSlider->value()));
}

void StartScreen::setDaltonicMode()
{
    // Always re-apply daltonic colors when active (dark mode may have overwritten them)
    if (mNextSettings.daltonicMode)
    {
        ui->startWidget->setStyleSheet(mColorMap.value(true).widgets);

        for (auto widget : std::as_const(mWidgets))
        {
            if (widget->accessibleName() == BACKBUTTONSTR)
            {
                widget->setStyleSheet(mColorMap.value(true).backButton);
            }
            else
            {
                widget->setStyleSheet(mColorMap.value(true).widgets);
            }
        }
    }
    else if (mNextSettings.daltonicMode != mCurrentSettings.daltonicMode)
    {
        // Daltonic was just deactivated: clear per-widget styles so global stylesheet takes over
        ui->startWidget->setStyleSheet("");

        for (auto widget : std::as_const(mWidgets))
        {
            widget->setStyleSheet("");
        }
    }
}

void StartScreen::setDarkMode()
{
    // Apply stylesheet to the top-level window so all screens (HomeScreen included) get themed
    if (window())
    {
        window()->setStyleSheet(mThemeStylesheets.value(mNextSettings.darkMode));
    }
    else
    {
        this->setStyleSheet(mThemeStylesheets.value(mNextSettings.darkMode));
    }
}

void StartScreen::loadSettings()
{
    QSettings settings{"TFG Guillermo Giron Garcia", "Latency Tester"};

    if(settings.value("Language") != 0)
    {
        mNextSettings.language = settings.value("Language").value<Languages>();
    }

    if(settings.value("FontSize") != 0)
    {
        mNextSettings.fontSize = settings.value("FontSize").value<quint8>();
    }

    if(0 == mNextSettings.fontSize)
    {
        mNextSettings.fontSize = 9;
    }

    if(settings.value("DaltonicMode") != 0)
    {
        mNextSettings.daltonicMode = settings.value("DaltonicMode").value<bool>();
    }

    mNextSettings.darkMode = settings.value("DarkMode").value<bool>();

    Colors daltonicColors;
    Colors colors;

    daltonicColors.backButton = "background-color: rgb(157, 2, 8)";
    daltonicColors.widgets = "background-color: rgb(82, 183, 136)";
    mColorMap.insert(true, daltonicColors);

    colors.backButton = "background-color: rgb(255, 214, 10)";
    colors.widgets = "background-color: rgb(0, 180, 216)";
    mColorMap.insert(false, colors);

    // Theme stylesheets: light and dark
    mThemeStylesheets.insert(false,
        "QPushButton {"
        "  border: 2px solid rgba(255, 255, 255, 180);"
        "  border-radius: 8px;"
        "  padding: 6px 16px;"
        "  font-weight: bold;"
        "  color: white;"
        "  background-color: rgba(0, 120, 180, 200);"
        "}"
        "QPushButton:hover { background-color: rgba(0, 150, 220, 220); border-color: rgba(255, 255, 255, 240); }"
        "QPushButton:pressed { background-color: rgba(0, 90, 140, 240); }"
        "QPushButton:disabled {"
        "  background-color: rgba(100, 100, 100, 150);"
        "  border-color: rgba(150, 150, 150, 100);"
        "  color: rgba(200, 200, 200, 150);"
        "}"
        "QSlider::groove:horizontal {"
        "  border: 1px solid rgb(160, 160, 180);"
        "  height: 8px;"
        "  background: rgb(200, 210, 230);"
        "  border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: rgb(0, 140, 200);"
        "  border: 1px solid rgb(0, 110, 170);"
        "  width: 18px;"
        "  margin: -6px 0;"
        "  border-radius: 9px;"
        "}"
    );

    mThemeStylesheets.insert(true,
        "* { background-color: rgb(35, 35, 50); color: rgb(210, 210, 220); }"
        "QPushButton {"
        "  border: 2px solid rgba(180, 180, 200, 150);"
        "  border-radius: 8px;"
        "  padding: 6px 16px;"
        "  font-weight: bold;"
        "  color: white;"
        "  background-color: rgba(40, 80, 120, 220);"
        "}"
        "QPushButton:hover { background-color: rgba(50, 100, 150, 230); }"
        "QPushButton:pressed { background-color: rgba(30, 60, 90, 240); }"
        "QPushButton:disabled {"
        "  background-color: rgba(60, 60, 60, 150);"
        "  color: rgba(150, 150, 150, 150);"
        "}"
        "QSlider::groove:horizontal {"
        "  border: 1px solid rgb(80, 80, 100);"
        "  height: 8px;"
        "  background: rgb(60, 60, 80);"
        "  border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: rgb(100, 160, 220);"
        "  border: 1px solid rgb(70, 130, 190);"
        "  width: 18px;"
        "  margin: -6px 0;"
        "  border-radius: 9px;"
        "}"
        "QComboBox {"
        "  background-color: rgb(50, 50, 65);"
        "  border: 1px solid rgb(80, 80, 100);"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "  color: rgb(210, 210, 220);"
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background-color: rgb(50, 50, 65); color: rgb(210, 210, 220); }"
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width: 18px; height: 18px; }"
        "QCheckBox::indicator:unchecked { border: 2px solid rgb(120, 120, 140); border-radius: 3px; background: rgb(50, 50, 65); }"
        "QCheckBox::indicator:checked { border: 2px solid rgb(100, 160, 220); border-radius: 3px; background: rgb(100, 160, 220); }"
        "QGroupBox { border: 1px solid rgb(80, 80, 100); border-radius: 6px; margin-top: 8px; }"
        "QGroupBox::title { color: rgb(180, 180, 200); }"
        "QTreeView { background-color: rgb(40, 40, 55); color: rgb(200, 200, 210); border: 1px solid rgb(70, 70, 90); }"
        "QLabel { background-color: transparent; }"
    );

    if(mNextSettings.daltonicMode)
    {
        ui->daltonicCheckbox->setCheckState(Qt::CheckState::Checked);
    }

    if(mNextSettings.darkMode)
    {
        ui->darkModeCheckbox->setCheckState(Qt::CheckState::Checked);
    }

    setSettings();
}

void StartScreen::saveSettings()
{
    QSettings settings{"TFG Guillermo Giron Garcia", "Latency Tester"};
    settings.setValue("Language", static_cast<quint8>(mCurrentSettings.language));
    settings.setValue("FontSize", mCurrentSettings.fontSize);
    settings.setValue("DaltonicMode", mCurrentSettings.daltonicMode);
    settings.setValue("DarkMode", mCurrentSettings.darkMode);
}

void StartScreen::setSettings()
{
    setTranslation();
    setDarkMode();       // Apply dark mode first (base theme)
    setFontSize();       // Font size after theme (theme may reset inherited fonts)
    setDaltonicMode();   // Daltonic overrides widget colors on top

    // Ensure stop button stays disabled unless an operation is in progress
    if (!mSensorOperator.isTakingMeasure())
    {
        ui->stopMeasureButton->setEnabled(false);
    }

    mCurrentSettings = mNextSettings;
}

void StartScreen::loadRegistry()
{
    QPointer<QFileSystemModel> model = new QFileSystemModel;
    QPointer<NoIconOrExtensionFileDelegate> delegate = new NoIconOrExtensionFileDelegate;

    // On Raspberry Pi: ~/Measures. On Desktop: use working directory (configured in Qt Creator)
#ifdef RASPBERRY_PI
    QString registryPath = QDir::homePath() + MEASURES;
#else
    QString registryPath = QDir::currentPath() + MEASURES;
#endif

    QDir registryFolder{registryPath};

    // If registry directory doesn't exist, create it
    if (!registryFolder.exists())
    {
        registryFolder.mkpath(registryPath);
    }

    model->setRootPath(registryPath);
    ui->registryTreeView->setModel(model);
    ui->registryTreeView->setItemDelegate(delegate);
    ui->registryTreeView->setRootIndex(model->index(registryPath));
    // Hide size and type colums
    ui->registryTreeView->hideColumn(1);
    ui->registryTreeView->hideColumn(2);
    ui->registryTreeView->setColumnWidth(0, 450);
    ui->registryTreeView->setSelectionBehavior (QAbstractItemView::SelectRows);
    // While it's not possible to translate header labels they'll be hidden
    ui->registryTreeView->setHeaderHidden(true);

    mJsonOperator.setPath(registryPath);
}

void StartScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        // Re-apply slider labels with values (retranslateUi resets them to .ui defaults without %1)
        ui->fontSizeLabel->setText(tr("Tamaño de la fuente: %1 pt").arg(ui->fontSizeSlider->value()));
        ui->timeFactorLabel->setText(tr("Intérvalo entre mediciones: %1 ms").arg(ui->TimeFactorSlider->value()));
        ui->DurationLabel->setText(tr("Duración: %1 s").arg(ui->DurationSlider->value()));
    }

    QWidget::changeEvent(event);
}

#include "startscreen.h"
#include "ui_startscreen.h"
#include "extensionfiledelegate.h"
#include <QList>
#include <QStyle>
#include <QCommonStyle>
#include <QSettings>
#include <QFileSystemModel>
#include <QDebug>
#include <QTimer>
#include <QtDBus/QDBusConnection>

const QString BACKBUTTONSTR{"Back_Button"};
const QString MEASURES{"/Measures"};
const QString MS{"Latencias (ms)"};
const QString TIMELINE{"Timeline (s)"};
const QString ENGLISH{"/home/pi/LatencyTester/bin/LatencyTester_en_EN.qm"};
const QString POLSKI{"/home/pi/LatencyTester/bin/LatencyTester_pl_PL.qm"};
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
      mKeyboard{new QProcess(this)}
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
    ui->settingsButtonBox->setEnabled(true);
}

void StartScreen::on_daltonicCheckbox_stateChanged(int arg1)
{
    mNextSettings.daltonicMode = arg1;
    ui->settingsButtonBox->setEnabled(true);
}

void StartScreen::on_settingsButtonBox_clicked(QAbstractButton *button)
{
    if(QDialogButtonBox::ButtonRole::ApplyRole == ui->settingsButtonBox->buttonRole(button))
    {
        setSettings();
    }
    else
    {
        ui->languagesComboBox->setCurrentIndex(static_cast<int>(mCurrentSettings.language));
        ui->fontSizeSlider->setValue(mCurrentSettings.fontSize);
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
        mKeyboard->start("xvkbd", {}, QIODevice::OpenModeFlag::ReadWrite);
    }
}

void StartScreen::backToStartScreen()
{
    mCurrentScreen = MenuScreen::START_SCREEN;

    if(MenuScreen::START_SCREEN == mCurrentScreen)
    {
        ui->backButton->setVisible(false);
        ui->backButton->setEnabled(false);
    }

    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(mCurrentScreen)};
    setUpNextScreen(mCurrentScreenWidgets, nextWidgets);
    mCurrentScreenWidgets = nextWidgets;
}

void StartScreen::on_startMeasureButton_released()
{
    mSensorOperator.takeMeasure(mMeasure);
}

void StartScreen::on_stopMeasureButton_released()
{
    mSensorOperator.stopMeasure();
}

void StartScreen::on_startMeasuringButton_released()
{
    transitionScreen(MenuScreen::START_MEASURE_SCREEN);
}

void StartScreen::on_TimeFactorSlider_valueChanged(int value)
{
    mMeasure.timeFactor = value;
}

void StartScreen::on_DurationSlider_valueChanged(int value)
{
    mMeasure.duration = value;
}

void StartScreen::on_CalibrateButton_released()
{

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
    mKeyboard->close();
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
    QVector<double> x_axis{};
    x_axis.insert(0, 0.0);

    for(int index = 1; index < mMeasure.lantencies.size(); ++index)
    {
        x_axis.insert(index, x_axis[index-1] + (mMeasure.timeFactor / 1000.0));
    }

    ui->plotMeasures->removeGraph(0);
    ui->plotMeasures->addGraph();
    ui->plotMeasures->graph(0)->setData(x_axis, mMeasure.lantencies);
    ui->plotMeasures->yAxis->setLabel(MS);
    ui->plotMeasures->xAxis->setLabel(TIMELINE);
    ui->plotMeasures->xAxis->setRange(0, mMeasure.duration / 1000);
    ui->plotMeasures->yAxis->setRange(0, mMeasure.meanLatency * 2);
    ui->plotMeasures->graph(0)->setLineStyle(QCPGraph::LineStyle::lsStepRight);

    QCPScatterStyle scatter;
    scatter.setShape(QCPScatterStyle::ssCircle);
    scatter.setPen(QPen(Qt::white));
    scatter.setSize(mCurrentSettings.fontSize);

    if(mCurrentSettings.daltonicMode)
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
                {MenuScreen::START_MEASURE_SCREEN, ui->stopMeasureButton},
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
    if(mNextSettings.fontSize != mCurrentSettings.fontSize)
    {
        QFont font;
        for(auto widget : mWidgets)
        {
            font = widget->font();
            font.setPointSize(mNextSettings.fontSize);
            widget->setFont(font);
            if(widget->accessibleName() != BACKBUTTONSTR)
            {
                widget->adjustSize();
            }
        }

        ui->fontSizeSlider->setSliderPosition(mNextSettings.fontSize);
    }
}

void StartScreen::setTranslation()
{
    if(mNextSettings.language != mCurrentSettings.language)
    {
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
            mTranslator.load(ENGLISH);
            qApp->installTranslator(&mTranslator);
            buttons[1]->setText(APPLY);
            buttons[0]->setText(CANCEL);
            break;
            case Languages::POLISH:
            qApp->removeTranslator(&mTranslator);
            mTranslator.load(POLSKI);
            qApp->installTranslator(&mTranslator);
            buttons[1]->setText(STOSOWAC);
            buttons[0]->setText(ANULOWAC);
            break;
        }

        ui->languagesComboBox->setCurrentIndex(static_cast<int>(mNextSettings.language));
    }
}

void StartScreen::setDaltonicMode()
{
    if(mNextSettings.daltonicMode != mCurrentSettings.daltonicMode)
    {
        ui->startWidget->setStyleSheet(mColorMap.value(mNextSettings.daltonicMode).widgets);

        for(auto widget : mWidgets)
        {
            if(widget->accessibleName() == BACKBUTTONSTR)
            {
                widget->setStyleSheet(mColorMap.value(mNextSettings.daltonicMode).backButton);
            }
            else
            {
                widget->setStyleSheet(mColorMap.value(mNextSettings.daltonicMode).widgets);
            }
        }
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
        mNextSettings.fontSize = 10;
    }

    if(settings.value("DaltonicMode") != 0)
    {
        mNextSettings.daltonicMode = settings.value("DaltonicMode").value<bool>();
    }

    Colors daltonicColors;
    Colors colors;

    daltonicColors.backButton = "background-color: rgb(157, 2, 8)";
    daltonicColors.widgets = "background-color: rgb(82, 183, 136)";
    mColorMap.insert(true, daltonicColors);

    colors.backButton = "background-color: rgb(255, 214, 10)";
    colors.widgets = "background-color: rgb(0, 180, 216)";
    mColorMap.insert(false, colors);

    if(mNextSettings.daltonicMode)
    {
        ui->daltonicCheckbox->setCheckState(Qt::CheckState::Checked);
    }

    setSettings();
}

void StartScreen::saveSettings()
{
    QSettings settings{"TFG Guillermo Giron Garcia", "Latency Tester"};
    settings.setValue("Language", static_cast<quint8>(mCurrentSettings.language));
    settings.setValue("FontSize", mCurrentSettings.fontSize);
    settings.setValue("DaltonicMode", mCurrentSettings.daltonicMode);
}

void StartScreen::setSettings()
{
    setTranslation();
    setFontSize();
    setDaltonicMode();
    mCurrentSettings = mNextSettings;
}

void StartScreen::loadRegistry()
{
    QPointer<QFileSystemModel> model = new QFileSystemModel;
    QPointer<NoIconOrExtensionFileDelegate> delegate = new NoIconOrExtensionFileDelegate;
    QString home = QDir::homePath();
    QDir registryFolder{home + MEASURES};

    // If registry directory doesn't exist, is created
    if(!registryFolder.exists(home + MEASURES))
    {
        registryFolder.mkdir(home + MEASURES);
    }

    model->setRootPath(home + MEASURES);
    ui->registryTreeView->setModel(model);
    ui->registryTreeView->setItemDelegate(delegate);
    ui->registryTreeView->setRootIndex(model->index(home + MEASURES));
    // Hide size and type colums
    ui->registryTreeView->hideColumn(1);
    ui->registryTreeView->hideColumn(2);
    ui->registryTreeView->setColumnWidth(0, 450);
    ui->registryTreeView->setSelectionBehavior (QAbstractItemView::SelectRows);

    mJsonOperator.setPath(home + MEASURES);
}

void StartScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

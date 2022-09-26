#include "startscreen.h"
#include "ui_startscreen.h"
#include "extensionfiledelegate.h"
#include <QList>
#include <QStyle>
#include <QCommonStyle>
#include <QSettings>
#include <QFileSystemModel>
#include <QDebug>

const static QString backButtonStr{"Back_Button"};

StartScreen::StartScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StartScreen), mCurrentScreen(MenuScreen::START_SCREEN), mRenameWindow(new RenamePopUp(this))
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
    mCurrentScreen = mBackButtonOutputs.value(mCurrentScreen);
    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(mCurrentScreen)};
    setUpNextScreen(mCurrentScreenWidgets, nextWidgets);
    mCurrentScreenWidgets = nextWidgets;

    if(MenuScreen::START_SCREEN == mCurrentScreen)
    {
        ui->backButton->setVisible(false);
        ui->backButton->setEnabled(false);
    }
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
    if(ui->settingsButtonBox->Apply == ui->settingsButtonBox->standardButton(button))
    {
        setSettings();
    }
    else if(ui->settingsButtonBox->Reset == ui->settingsButtonBox->standardButton(button))
    {
        ui->languagesComboBox->setCurrentIndex(static_cast<int>(mCurrentSettings.language));
        ui->fontSizeSlider->setValue(mCurrentSettings.fontSize);
        ui->settingsButtonBox->setEnabled(false);
    }
}

void StartScreen::on_checkRegistryEntryButton_released()
{

}

void StartScreen::on_deleteRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = (QFileSystemModel*)ui->registryTreeView->model();
    model->remove(ui->registryTreeView->currentIndex());
}

void StartScreen::on_renameRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = (QFileSystemModel*)ui->registryTreeView->model();
    QString nameWithExtension = model->fileName(ui->registryTreeView->currentIndex());
    QStringList nameList = nameWithExtension.split(".", Qt::SkipEmptyParts, Qt::CaseInsensitive);
    QString name = nameList.first();
    mRenameWindow->setName(name);
    mRenameWindow->show();
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
}

void StartScreen::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void StartScreen::init()
{
    widgetsMapInit();
    loadSettings();
    loadRegistry();
}

void StartScreen::widgetsMapInit()
{
    ui->setupUi(this);

    QCommonStyle style;
    ui->backButton->setIcon(style.standardIcon(QStyle::SP_ArrowBack));
    ui->backButton->setAccessibleName(backButtonStr);

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
                          {MenuScreen::GENERAL_INFO_SCREEN, MenuScreen::HELP_SCREEN}
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
            if(widget->accessibleName() != backButtonStr)
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
        switch (mNextSettings.language)
        {
            case Languages::SPANISH:
            break;
            case Languages::ENGLISH:
            break;
            case Languages::POLISH:
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
            if(widget->accessibleName() == backButtonStr)
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
    QDir registryFolder{QDir::current()};

    // If registry directory doesn't exist, is created
    if(!registryFolder.exists(registryFolder.currentPath() + "/Measures"))
    {
        registryFolder.mkdir("Measures");
    }

    model->setRootPath(registryFolder.currentPath() + "/Measures");
    ui->registryTreeView->setModel(model);
    ui->registryTreeView->setItemDelegate(delegate);
    ui->registryTreeView->setRootIndex(model->index(registryFolder.currentPath() + "/Measures"));
    // Hide size and type colums
    ui->registryTreeView->hideColumn(1);
    ui->registryTreeView->hideColumn(2);
    ui->registryTreeView->setColumnWidth(0, 450);
    ui->registryTreeView->setSelectionBehavior (QAbstractItemView::SelectRows);
}



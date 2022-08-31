#include "startscreen.h"
#include "ui_startscreen.h"
#include <QList>
#include <QStyle>
#include <QCommonStyle>
#include <QSettings>

StartScreen::StartScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StartScreen), mCurrentScreen(MenuScreen::START_SCREEN)
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

void StartScreen::on_settingsButtonBox_accepted()
{
    setTranslation();
    setFontSize();
    mCurrentSettings = mNextSettings;
}

void StartScreen::on_settingsButtonBox_rejected()
{
    ui->languagesComboBox->setCurrentIndex(static_cast<int>(mCurrentSettings.language));
    ui->fontSizeSlider->setValue(mCurrentSettings.fontSize);
    ui->settingsButtonBox->setEnabled(false);
}

void StartScreen::init()
{
    widgetsMapInit();
    loadSettings();
}

void StartScreen::widgetsMapInit()
{
    ui->setupUi(this);

    QCommonStyle style;
    ui->backButton->setIcon(style.standardIcon(QStyle::SP_ArrowBack));

    mWidgets = {{MenuScreen::START_SCREEN, ui->helpButton},
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
                {MenuScreen::SETTINGS_SCREEN, ui->languageText},
                {MenuScreen::SETTINGS_SCREEN, ui->languagesComboBox},
                {MenuScreen::SETTINGS_SCREEN, ui->backButton},
                {MenuScreen::SETTINGS_SCREEN, ui->fontSizeText},
                {MenuScreen::SETTINGS_SCREEN, ui->fontSizeSlider},
                {MenuScreen::SETTINGS_SCREEN, ui->settingsButtonBox}
               };

    auto it{mWidgets.find(MenuScreen::SETTINGS_SCREEN)};
    while (it != mWidgets.end() )
    {
        it.value()->setVisible(false);
        it.value()->setEnabled(false);
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
        }
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
    }
}

void StartScreen::loadSettings()
{
    QSettings settings{"TFG Guillermo Giron Garcia", "Latency Tester"};

    if(settings.value("Language") != 0)
    {
        mCurrentSettings.language = settings.value("Language").value<Languages>();
    }

    if(settings.value("FontSize") != 0)
    {
        mCurrentSettings.fontSize = settings.value("FontSize").value<quint8>();
    }
}

void StartScreen::saveSettings()
{
    QSettings settings{"TFG Guillermo Giron Garcia", "Latency Tester"};
    settings.setValue("Language", static_cast<quint8>(mCurrentSettings.language));
    settings.setValue("FontSize", mCurrentSettings.fontSize);
}

void StartScreen::on_startWidget_destroyed()
{
    saveSettings();
}


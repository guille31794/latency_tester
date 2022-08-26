#include "startscreen.h"
#include "ui_startscreen.h"
#include <QList>
#include <QStyle>
#include <QCommonStyle>

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
    mCurrentScreenWidgets = mWidgets.values(mCurrentScreen);
    mCurrentScreen = MenuScreen::HELP_SCREEN;
    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(mCurrentScreen)};
    setUpNextScreen(mCurrentScreenWidgets, nextWidgets);
    mCurrentScreenWidgets = nextWidgets;
}

void StartScreen::on_generalInfoButton_released()
{
    mCurrentScreenWidgets = mWidgets.values(mCurrentScreen);
    mCurrentScreen = MenuScreen::GENERAL_INFO_SCREEN;
    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(mCurrentScreen)};
    setUpNextScreen(mCurrentScreenWidgets, nextWidgets);
    mCurrentScreenWidgets = nextWidgets;
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

void StartScreen::init()
{
    ui->setupUi(this);
    QCommonStyle style;
    ui->backButton->setIcon(style.standardIcon(QStyle::SP_ArrowBack));
    ui->backButton->setVisible(false);
    ui->backButton->setEnabled(false);
    widgetsMapInit();
}

void StartScreen::widgetsMapInit()
{
    mWidgets = {{MenuScreen::START_SCREEN, ui->helpButton},
                {MenuScreen::START_SCREEN, ui->settingsButton},
                {MenuScreen::START_SCREEN, ui->startMeasuringButton},
                {MenuScreen::START_SCREEN, ui->historicOfMeasuresButton},
                {MenuScreen::HELP_SCREEN, ui->helpFrame},
                {MenuScreen::HELP_SCREEN, ui->userManualButton},
                {MenuScreen::HELP_SCREEN, ui->generalInfoButton},
                {MenuScreen::HELP_SCREEN, ui->backButton},
                {MenuScreen::GENERAL_INFO_SCREEN, ui->generalInfoFrame},
                {MenuScreen::GENERAL_INFO_SCREEN, ui->generalInfoText},
                {MenuScreen::GENERAL_INFO_SCREEN, ui->backButton},
               };

    auto it{mWidgets.find(MenuScreen::HELP_SCREEN)};
    while (it != mWidgets.end() )
    {
        it.value()->setVisible(false);
        it.value()->setEnabled(false);
        ++it;
    }

    mBackButtonOutputs = {{MenuScreen::HELP_SCREEN, MenuScreen::START_SCREEN},
                          {MenuScreen::USER_MANUAL_SCREEN, MenuScreen::START_MEASURE_SCREEN},
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


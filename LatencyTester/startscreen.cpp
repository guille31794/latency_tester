#include "startscreen.h"
#include "ui_startscreen.h"
#include <QList>

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
    QList<QPointer<QWidget>> currentWidgets{mWidgets.values(mCurrentScreen)};
    mCurrentScreen = MenuScreen::HELP_SCREEN;
    QList<QPointer<QWidget>> nextWidgets{mWidgets.values(mCurrentScreen)};
    ui->helpLayout->setEnabled(true);
    setUpNextScreen(currentWidgets, nextWidgets);
}

void StartScreen::init()
{
    ui->setupUi(this);
    widgetsMapInit();
}

void StartScreen::widgetsMapInit()
{
    mWidgets.insert(MenuScreen::START_SCREEN, ui->startWidget);
    mWidgets.insert(MenuScreen::START_SCREEN, ui->helpButton);
    mWidgets.insert(MenuScreen::START_SCREEN, ui->settingsButton);
    mWidgets.insert(MenuScreen::START_SCREEN, ui->startMeasuringButton);
    mWidgets.insert(MenuScreen::START_SCREEN, ui->historicOfMeasuresButton);

    mWidgets.insert(MenuScreen::HELP_SCREEN, ui->helpFrame);
    mWidgets.insert(MenuScreen::HELP_SCREEN, ui->userManualButton);
    mWidgets.insert(MenuScreen::HELP_SCREEN, ui->generalInfoButton);

    auto it{mWidgets.find(MenuScreen::HELP_SCREEN)};
    while (it != mWidgets.end() )
    {
            it.value()->setVisible(false);
            it.value()->setDisabled(true);
            ++it;
    }
}

void StartScreen::setUpNextScreen(QList<QPointer<QWidget> > &toHide, QList<QPointer<QWidget> > &toShow)
{
    for(auto widget : toHide)
    {
        widget->setVisible(false);
        widget->setDisabled(true);
    }

    for(auto widget : toShow)
    {
        widget->setVisible(true);
        widget->setEnabled(true);
    }
}


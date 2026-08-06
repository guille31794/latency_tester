#include "homescreen.h"
#include "ui_homescreen.h"
#include <QEvent>

HomeScreen::HomeScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeScreen)
{
    ui->setupUi(this);
}

HomeScreen::~HomeScreen()
{
    delete ui;
}

void HomeScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void HomeScreen::on_startMeasuringButton_released()
{
    emit startMeasuringRequested();
}

void HomeScreen::on_historicOfMeasuresButton_released()
{
    emit historyRequested();
}

void HomeScreen::on_settingsButton_released()
{
    emit settingsRequested();
}

void HomeScreen::on_helpButton_released()
{
    emit helpRequested();
}

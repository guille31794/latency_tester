#include "helpscreen.h"
#include "ui_helpscreen.h"
#include <QEvent>

HelpScreen::HelpScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpScreen)
{
    ui->setupUi(this);
}

HelpScreen::~HelpScreen()
{
    delete ui;
}

void HelpScreen::on_backButton_released()
{
    emit backRequested();
}

void HelpScreen::on_generalInfoButton_released()
{
    emit generalInfoRequested();
}

void HelpScreen::on_usersManualButton_released()
{
    emit usersManualRequested();
}

void HelpScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

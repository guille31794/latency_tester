#include "helpscreen.h"
#include "ui_helpscreen.h"
#include <QEvent>

HelpScreen::HelpScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpScreen)
{
    ui->setupUi(this);
    // Show help menu by default, hide info panel
    ui->infoPanel->setVisible(false);
}

HelpScreen::~HelpScreen()
{
    delete ui;
}

void HelpScreen::on_backButton_released()
{
    if (ui->infoPanel->isVisible())
    {
        // Go back to help menu
        ui->infoPanel->setVisible(false);
        ui->helpMenu->setVisible(true);
    }
    else
    {
        emit backRequested();
    }
}

void HelpScreen::on_generalInfoButton_released()
{
    ui->helpMenu->setVisible(false);
    ui->infoPanel->setVisible(true);
}

void HelpScreen::on_usersManualButton_released()
{
    // TODO: Show user manual content
}

void HelpScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

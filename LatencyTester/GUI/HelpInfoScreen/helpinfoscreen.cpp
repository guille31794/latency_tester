#include "helpinfoscreen.h"
#include "ui_helpinfoscreen.h"
#include <QEvent>

HelpInfoScreen::HelpInfoScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpInfoScreen)
{
    ui->setupUi(this);
}

HelpInfoScreen::~HelpInfoScreen()
{
    delete ui;
}

void HelpInfoScreen::setContent(const QString& text)
{
    ui->contentText->setPlainText(text);
}

void HelpInfoScreen::on_backButton_released()
{
    emit backRequested();
}

void HelpInfoScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

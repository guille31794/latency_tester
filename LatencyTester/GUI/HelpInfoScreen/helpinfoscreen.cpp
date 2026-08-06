#include "helpinfoscreen.h"
#include "ui_helpinfoscreen.h"
#include <QEvent>
#include <QFile>

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
    ui->contentBrowser->setPlainText(text);
}

void HelpInfoScreen::setHtmlResource(const QString& resourcePath)
{
    QFile file(resourcePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->contentBrowser->setHtml(QString::fromUtf8(file.readAll()));
        file.close();
    }
    else
    {
        ui->contentBrowser->setPlainText("Error: could not load " + resourcePath);
    }
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

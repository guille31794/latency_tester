#include "helpscreen.h"
#include "ui_helpscreen.h"
#include "appsettings.h"
#include <QEvent>
#include <QFile>

HelpScreen::HelpScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpScreen)
{
    ui->setupUi(this);
    updateManualButtonState();
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

void HelpScreen::showEvent(QShowEvent *event)
{
    updateManualButtonState();
    QWidget::showEvent(event);
}

void HelpScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        updateManualButtonState();
    }
    QWidget::changeEvent(event);
}

void HelpScreen::updateManualButtonState()
{
    QString lang;
    switch (AppSettings::instance().language())
    {
        case Languages::ENGLISH: lang = "en"; break;
        case Languages::POLISH:  lang = "pl"; break;
        default:                 lang = "es"; break;
    }

    QString resourcePath = QStringLiteral(":/help/help/manual_%1.html").arg(lang);
    ui->usersManualButton->setEnabled(QFile::exists(resourcePath));
}

#include "settingsscreen.h"
#include "ui_settingsscreen.h"
#include "appsettings.h"
#include <QApplication>
#include <QPushButton>

const QString SPANISH_PATH{":/i18n/LatencyTester_es_ES.qm"};
const QString ENGLISH_PATH{":/i18n/LatencyTester_en_EN.qm"};
const QString POLSKI_PATH{":/i18n/LatencyTester_pl_PL.qm"};

SettingsScreen::SettingsScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsScreen)
{
    ui->setupUi(this);

    // Back button: fixed large font, not affected by font size settings
    QFont backFont = ui->backButton->font();
    backFont.setPointSize(24);
    backFont.setBold(true);
    ui->backButton->setFont(backFont);
    // Add Apply/Cancel buttons
    QAbstractButton* applyButton = new QPushButton(tr("Aplicar"), ui->settingsButtonBox);
    QAbstractButton* cancelButton = new QPushButton(tr("Cancelar"), ui->settingsButtonBox);
    ui->settingsButtonBox->addButton(applyButton, QDialogButtonBox::ButtonRole::ApplyRole);
    ui->settingsButtonBox->addButton(cancelButton, QDialogButtonBox::ButtonRole::RejectRole);

    loadFromAppSettings();
    applyTranslation();
}

SettingsScreen::~SettingsScreen()
{
    delete ui;
}

void SettingsScreen::loadFromAppSettings()
{
    mCurrentSettings = AppSettings::instance().current();
    mNextSettings = mCurrentSettings;

    // Block signals to prevent settingsButtonBox from enabling during load
    const bool langBlocked = ui->languagesComboBox->signalsBlocked();
    const bool sliderBlocked = ui->fontSizeSlider->signalsBlocked();
    const bool dalBlocked = ui->daltonicCheckbox->signalsBlocked();
    const bool darkBlocked = ui->darkModeCheckbox->signalsBlocked();

    ui->languagesComboBox->blockSignals(true);
    ui->fontSizeSlider->blockSignals(true);
    ui->daltonicCheckbox->blockSignals(true);
    ui->darkModeCheckbox->blockSignals(true);

    ui->languagesComboBox->setCurrentIndex(static_cast<int>(mCurrentSettings.language));
    ui->fontSizeSlider->setValue(mCurrentSettings.fontSize);
    ui->daltonicCheckbox->setChecked(mCurrentSettings.daltonicMode);
    ui->darkModeCheckbox->setChecked(mCurrentSettings.darkMode);
    ui->fontSizeLabel->setText(tr("Tamaño de la fuente: %1 pt").arg(mCurrentSettings.fontSize));

    ui->languagesComboBox->blockSignals(langBlocked);
    ui->fontSizeSlider->blockSignals(sliderBlocked);
    ui->daltonicCheckbox->blockSignals(dalBlocked);
    ui->darkModeCheckbox->blockSignals(darkBlocked);

    ui->settingsButtonBox->setEnabled(false);
}

void SettingsScreen::applyTranslation()
{
    qApp->removeTranslator(&mTranslator);

    switch (mNextSettings.language)
    {
    case Languages::SPANISH:
        (void)mTranslator.load(SPANISH_PATH);
        break;
    case Languages::ENGLISH:
        (void)mTranslator.load(ENGLISH_PATH);
        break;
    case Languages::POLISH:
        (void)mTranslator.load(POLSKI_PATH);
        break;
    }

    qApp->installTranslator(&mTranslator);
}

void SettingsScreen::on_fontSizeSlider_valueChanged(int value)
{
    mNextSettings.fontSize = value;
    ui->fontSizeLabel->setText(tr("Tamaño de la fuente: %1 pt").arg(value));
    ui->settingsButtonBox->setEnabled(true);
}

void SettingsScreen::on_languagesComboBox_currentIndexChanged(int index)
{
    mNextSettings.language = static_cast<Languages>(index);
    ui->settingsButtonBox->setEnabled(true);
}

void SettingsScreen::on_daltonicCheckbox_stateChanged(int arg1)
{
    mNextSettings.daltonicMode = arg1;
    ui->settingsButtonBox->setEnabled(true);
}

void SettingsScreen::on_darkModeCheckbox_stateChanged(int arg1)
{
    mNextSettings.darkMode = arg1;
    ui->settingsButtonBox->setEnabled(true);
}

void SettingsScreen::on_settingsButtonBox_clicked(QAbstractButton *button)
{
    if (QDialogButtonBox::ButtonRole::ApplyRole == ui->settingsButtonBox->buttonRole(button))
    {
        // Save settings FIRST so changeEvent reads correct values
        mCurrentSettings = mNextSettings;
        AppSettings::instance().applySettings(mNextSettings);

        // Then apply translation (triggers LanguageChange → changeEvent → loadFromAppSettings with correct data)
        applyTranslation();

        // Update button texts by role (order in buttons() list is not deterministic)
        const auto buttons = ui->settingsButtonBox->buttons();
        for (QAbstractButton* btn : buttons)
        {
            if (ui->settingsButtonBox->buttonRole(btn) == QDialogButtonBox::ApplyRole)
                btn->setText(tr("Aplicar"));
            else if (ui->settingsButtonBox->buttonRole(btn) == QDialogButtonBox::RejectRole)
                btn->setText(tr("Cancelar"));
        }

        ui->settingsButtonBox->setEnabled(false);
    }
    else
    {
        // Cancel: revert to current
        mNextSettings = mCurrentSettings;
        ui->languagesComboBox->setCurrentIndex(static_cast<int>(mCurrentSettings.language));
        ui->fontSizeSlider->setValue(mCurrentSettings.fontSize);
        ui->daltonicCheckbox->setChecked(mCurrentSettings.daltonicMode);
        ui->darkModeCheckbox->setChecked(mCurrentSettings.darkMode);
        ui->settingsButtonBox->setEnabled(false);
    }
}

void SettingsScreen::on_backButton_released()
{
    emit backRequested();
}

void SettingsScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        // Restore widget values after retranslateUi resets them to .ui defaults
        loadFromAppSettings();
        // Update programmatic button texts
        const auto buttons = ui->settingsButtonBox->buttons();
        for (QAbstractButton* btn : buttons)
        {
            if (ui->settingsButtonBox->buttonRole(btn) == QDialogButtonBox::ApplyRole)
                btn->setText(tr("Aplicar"));
            else if (ui->settingsButtonBox->buttonRole(btn) == QDialogButtonBox::RejectRole)
                btn->setText(tr("Cancelar"));
        }
    }
    QWidget::changeEvent(event);
}

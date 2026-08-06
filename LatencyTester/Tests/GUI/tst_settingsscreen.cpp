#include "tst_settingsscreen.h"
#include "settingsscreen.h"
#include "appsettings.h"
#include <QtTest>
#include <QSignalSpy>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QSettings>

void TestSettingsScreen::init()
{
    // Clear stored settings and reload defaults
    QSettings settings("TFG Guillermo Giron Garcia", "Latency Tester");
    settings.clear();
    settings.sync();
    AppSettings::instance().load();
}

// --- Instantiation ---

void TestSettingsScreen::test_canInstantiate()
{
    SettingsScreen screen;
    QVERIFY(screen.findChild<QSlider*>("fontSizeSlider"));
}

// --- Back button ---

void TestSettingsScreen::test_backButton_emitsSignal()
{
    SettingsScreen screen;
    QSignalSpy spy(&screen, &SettingsScreen::backRequested);
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QTest::mouseClick(btn, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
}

// --- Widgets exist ---

void TestSettingsScreen::test_languageComboBox_exists()
{
    SettingsScreen screen;
    QComboBox* combo = screen.findChild<QComboBox*>("languagesComboBox");
    QVERIFY(combo);
    QCOMPARE(combo->count(), 3);  // Spanish, English, Polish
}

void TestSettingsScreen::test_fontSizeSlider_exists()
{
    SettingsScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("fontSizeSlider");
    QVERIFY(slider);
    QCOMPARE(slider->minimum(), 9);
    QCOMPARE(slider->maximum(), 15);
}

void TestSettingsScreen::test_daltonicCheckbox_exists()
{
    SettingsScreen screen;
    QCheckBox* cb = screen.findChild<QCheckBox*>("daltonicCheckbox");
    QVERIFY(cb);
}

void TestSettingsScreen::test_darkModeCheckbox_exists()
{
    SettingsScreen screen;
    QCheckBox* cb = screen.findChild<QCheckBox*>("darkModeCheckbox");
    QVERIFY(cb);
}

void TestSettingsScreen::test_settingsButtonBox_exists()
{
    SettingsScreen screen;
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(box);
    QVERIFY(box->buttons().size() >= 2);
}

// --- Initial state from AppSettings (defaults) ---

void TestSettingsScreen::test_loadFromSettings_languageComboCorrect()
{
    SettingsScreen screen;
    QComboBox* combo = screen.findChild<QComboBox*>("languagesComboBox");
    QVERIFY(combo);
    QCOMPARE(combo->currentIndex(), static_cast<int>(Languages::SPANISH));
}

void TestSettingsScreen::test_loadFromSettings_fontSliderCorrect()
{
    SettingsScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("fontSizeSlider");
    QVERIFY(slider);
    QCOMPARE(slider->value(), 9);  // Default font size
}

void TestSettingsScreen::test_loadFromSettings_daltonicCorrect()
{
    SettingsScreen screen;
    QCheckBox* cb = screen.findChild<QCheckBox*>("daltonicCheckbox");
    QVERIFY(cb);
    QCOMPARE(cb->isChecked(), false);
}

void TestSettingsScreen::test_loadFromSettings_darkModeCorrect()
{
    SettingsScreen screen;
    QCheckBox* cb = screen.findChild<QCheckBox*>("darkModeCheckbox");
    QVERIFY(cb);
    QCOMPARE(cb->isChecked(), false);
}

void TestSettingsScreen::test_buttonBox_disabledInitially()
{
    SettingsScreen screen;
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(box);
    QCOMPARE(box->isEnabled(), false);
}

// --- Widget changes enable buttonBox ---

void TestSettingsScreen::test_fontSliderChange_enablesButtonBox()
{
    SettingsScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("fontSizeSlider");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(slider);
    QVERIFY(box);

    slider->setValue(12);
    QCOMPARE(box->isEnabled(), true);
}

void TestSettingsScreen::test_languageChange_enablesButtonBox()
{
    SettingsScreen screen;
    QComboBox* combo = screen.findChild<QComboBox*>("languagesComboBox");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(combo);
    QVERIFY(box);

    combo->setCurrentIndex(1);  // English
    QCOMPARE(box->isEnabled(), true);
}

void TestSettingsScreen::test_daltonicChange_enablesButtonBox()
{
    SettingsScreen screen;
    QCheckBox* cb = screen.findChild<QCheckBox*>("daltonicCheckbox");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(cb);
    QVERIFY(box);

    cb->setChecked(true);
    QCOMPARE(box->isEnabled(), true);
}

void TestSettingsScreen::test_darkModeChange_enablesButtonBox()
{
    SettingsScreen screen;
    QCheckBox* cb = screen.findChild<QCheckBox*>("darkModeCheckbox");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(cb);
    QVERIFY(box);

    cb->setChecked(true);
    QCOMPARE(box->isEnabled(), true);
}

// --- Apply ---

void TestSettingsScreen::test_apply_savesToAppSettings()
{
    SettingsScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("fontSizeSlider");
    QCheckBox* darkCb = screen.findChild<QCheckBox*>("darkModeCheckbox");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(slider);
    QVERIFY(darkCb);
    QVERIFY(box);

    // Change settings
    slider->setValue(14);
    darkCb->setChecked(true);

    // Find and click the Apply button
    QAbstractButton* applyBtn = nullptr;
    for (QAbstractButton* btn : box->buttons())
    {
        if (box->buttonRole(btn) == QDialogButtonBox::ApplyRole)
        {
            applyBtn = btn;
            break;
        }
    }
    QVERIFY(applyBtn);
    QTest::mouseClick(applyBtn, Qt::LeftButton);

    // Verify AppSettings updated
    QCOMPARE(AppSettings::instance().fontSize(), quint8(14));
    QCOMPARE(AppSettings::instance().darkMode(), true);
}

void TestSettingsScreen::test_apply_disablesButtonBox()
{
    SettingsScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("fontSizeSlider");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(slider);
    QVERIFY(box);

    slider->setValue(11);
    QCOMPARE(box->isEnabled(), true);

    // Click Apply
    QAbstractButton* applyBtn = nullptr;
    for (QAbstractButton* btn : box->buttons())
    {
        if (box->buttonRole(btn) == QDialogButtonBox::ApplyRole)
        {
            applyBtn = btn;
            break;
        }
    }
    QVERIFY(applyBtn);
    QTest::mouseClick(applyBtn, Qt::LeftButton);

    QCOMPARE(box->isEnabled(), false);
}

// --- Cancel ---

void TestSettingsScreen::test_cancel_revertsWidgets()
{
    SettingsScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("fontSizeSlider");
    QCheckBox* dalCb = screen.findChild<QCheckBox*>("daltonicCheckbox");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(slider);
    QVERIFY(dalCb);
    QVERIFY(box);

    // Change values
    slider->setValue(13);
    dalCb->setChecked(true);

    // Click Cancel
    QAbstractButton* cancelBtn = nullptr;
    for (QAbstractButton* btn : box->buttons())
    {
        if (box->buttonRole(btn) == QDialogButtonBox::RejectRole)
        {
            cancelBtn = btn;
            break;
        }
    }
    QVERIFY(cancelBtn);
    QTest::mouseClick(cancelBtn, Qt::LeftButton);

    // Widgets should revert to defaults (9, unchecked)
    QCOMPARE(slider->value(), 9);
    QCOMPARE(dalCb->isChecked(), false);
}

void TestSettingsScreen::test_cancel_disablesButtonBox()
{
    SettingsScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("fontSizeSlider");
    QDialogButtonBox* box = screen.findChild<QDialogButtonBox*>("settingsButtonBox");
    QVERIFY(slider);
    QVERIFY(box);

    slider->setValue(12);
    QCOMPARE(box->isEnabled(), true);

    QAbstractButton* cancelBtn = nullptr;
    for (QAbstractButton* btn : box->buttons())
    {
        if (box->buttonRole(btn) == QDialogButtonBox::RejectRole)
        {
            cancelBtn = btn;
            break;
        }
    }
    QVERIFY(cancelBtn);
    QTest::mouseClick(cancelBtn, Qt::LeftButton);

    QCOMPARE(box->isEnabled(), false);
}

#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include "dataModel.hpp"
#include <QWidget>
#include <QAbstractButton>
#include <QTranslator>

namespace Ui {
class SettingsScreen;
}

/**
 * @brief The SettingsScreen class manages user preferences:
 * language, font size, daltonic mode, and dark mode.
 * Changes are applied via AppSettings.
 */
class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(QWidget *parent = nullptr);
    ~SettingsScreen();
    void changeEvent(QEvent *event) override;

signals:
    void backRequested();

private slots:
    void on_fontSizeSlider_valueChanged(int value);
    void on_languagesComboBox_currentIndexChanged(int index);
    void on_daltonicCheckbox_stateChanged(int arg1);
    void on_darkModeCheckbox_stateChanged(int arg1);
    void on_settingsButtonBox_clicked(QAbstractButton *button);
    void on_backButton_released();

private:
    void loadFromAppSettings();
    void applyTranslation();

    Ui::SettingsScreen *ui;
    GeneralConfigSettings mNextSettings;
    GeneralConfigSettings mCurrentSettings;
    QTranslator mTranslator;
};

#endif // SETTINGSSCREEN_H

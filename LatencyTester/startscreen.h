#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "dataModel.hpp"
#include <QMainWindow>
#include <QMultiMap>
#include <QPointer>
#include <QSharedPointer>
#include <QCloseEvent>
#include <QItemSelectionModel>

QT_BEGIN_NAMESPACE
namespace Ui { class StartScreen; }
QT_END_NAMESPACE

/**
 * @brief The StartScreen class controls navigation between screens and as it names says, first screen
 * shown by the program.
 */
class StartScreen : public QMainWindow
{
    Q_OBJECT

public:
    StartScreen(QWidget *parent = nullptr);
    ~StartScreen();

private slots:
    void on_helpButton_released();
    void on_generalInfoButton_released();
    void on_backButton_released();
    void on_usersManualButton_released();
    void on_settingsButton_released();
    void on_historicOfMeasuresButton_released();

    void on_fontSizeSlider_valueChanged(int value);
    void on_languagesComboBox_currentIndexChanged(int index);
    void on_settingsButtonBox_accepted();
    void on_settingsButtonBox_rejected();

    void on_checkRegistryEntryButton_released();
    void on_deleteRegistryEntryButton_released();
    void on_renameRegistryEntryButton_released();

private:
    /**
     * @brief init Initialize all widgets and connecttions
     */
    void init();
    /**
     * @brief widgetsMapInit Initialize all navigations settings
     */
    void widgetsMapInit();
    /**
     * @brief setUpNextScreen generic method called when a button is pushed to go from current screen
     * to next one
     * @param toHide current screen to change
     * @param toShow next screen asked
     */
    void setUpNextScreen(QList<QPointer<QWidget>> &toHide, QList<QPointer<QWidget>> &toShow);
    /**
     * @brief Used by the buttons to handle transition to next screen
     * @param nextScreen to show
     */
    void transitionScreen(MenuScreen nextScreen);
    /**
     * @brief Change the font size of all texts
     */
    void setFontSize();
    /**
     * @brief Translate the texts to the selected language
     */
    void setTranslation();
    /**
     * @brief load initial configuration
     */
    void loadSettings();
    /**
     * @brief save current settings for next app launch
     */
    void saveSettings();
    /**
     * @brief loadRegistry loads all saved measures entry
     */
    void loadRegistry();
    /**
     * @brief closeEvent Triggers ctions unchained when application closes
     * @param event close event
     */
    void closeEvent(QCloseEvent *event) override;

    Ui::StartScreen *ui;
    MenuScreen mCurrentScreen;
    QList<QPointer<QWidget>> mCurrentScreenWidgets;
    QMultiMap<MenuScreen, QPointer<QWidget>> mWidgets;
    QMap<MenuScreen, MenuScreen> mBackButtonOutputs;
    GeneralConfigSettings mNextSettings;
    GeneralConfigSettings mCurrentSettings;
};
#endif // STARTSCREEN_H

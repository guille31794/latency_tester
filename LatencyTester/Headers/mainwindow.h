#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "homescreen.h"
#include "helpscreen.h"
#include "settingsscreen.h"
#include "startscreen.h"

/**
 * @brief The MainWindow class is the top-level window that manages navigation
 * between screens using a QStackedWidget.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void showMeasureScreen();
    void showHistoryScreen();
    void showSettingsScreen();
    void showHelpScreen();
    void showHomeScreen();

private:
    QStackedWidget *mStackedWidget;
    HomeScreen *mHomeScreen;
    SettingsScreen *mSettingsScreen;
    HelpScreen *mHelpScreen;
    StartScreen *mStartScreen;

    enum ScreenIndex { HOME = 0, SETTINGS = 1, HELP = 2, LEGACY = 3 };
};

#endif // MAINWINDOW_H

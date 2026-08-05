#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>

namespace Ui {
class HomeScreen;
}

/**
 * @brief The HomeScreen class represents the main menu of the application.
 * It provides navigation buttons to access measurement, registry, settings and help screens.
 */
class HomeScreen : public QWidget
{
    Q_OBJECT

public:
    explicit HomeScreen(QWidget *parent = nullptr);
    ~HomeScreen();
    void changeEvent(QEvent *event) override;

signals:
    void startMeasuringRequested();
    void historyRequested();
    void settingsRequested();
    void helpRequested();

private slots:
    void on_startMeasuringButton_released();
    void on_historicOfMeasuresButton_released();
    void on_settingsButton_released();
    void on_helpButton_released();

private:
    Ui::HomeScreen *ui;
};

#endif // HOMESCREEN_H

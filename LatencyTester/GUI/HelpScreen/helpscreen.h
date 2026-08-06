#ifndef HELPSCREEN_H
#define HELPSCREEN_H

#include <QWidget>

namespace Ui {
class HelpScreen;
}

/**
 * @brief The HelpScreen class provides the help menu with navigation
 * to user manual and general info screens.
 */
class HelpScreen : public QWidget
{
    Q_OBJECT

public:
    explicit HelpScreen(QWidget *parent = nullptr);
    ~HelpScreen();
    void changeEvent(QEvent *event) override;

signals:
    void backRequested();
    void generalInfoRequested();
    void usersManualRequested();

private slots:
    void on_backButton_released();
    void on_generalInfoButton_released();
    void on_usersManualButton_released();

private:
    Ui::HelpScreen *ui;
};

#endif // HELPSCREEN_H

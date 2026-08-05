#ifndef HELPSCREEN_H
#define HELPSCREEN_H

#include <QWidget>

namespace Ui {
class HelpScreen;
}

/**
 * @brief The HelpScreen class provides help and general information about the application.
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

private slots:
    void on_backButton_released();
    void on_generalInfoButton_released();
    void on_usersManualButton_released();

private:
    Ui::HelpScreen *ui;
};

#endif // HELPSCREEN_H

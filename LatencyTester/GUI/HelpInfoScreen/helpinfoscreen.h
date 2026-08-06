#ifndef HELPINFOSCREEN_H
#define HELPINFOSCREEN_H

#include <QWidget>

namespace Ui {
class HelpInfoScreen;
}

/**
 * @brief The HelpInfoScreen class displays read-only information
 * (general info or user manual content).
 */
class HelpInfoScreen : public QWidget
{
    Q_OBJECT

public:
    explicit HelpInfoScreen(QWidget *parent = nullptr);
    ~HelpInfoScreen();
    void changeEvent(QEvent *event) override;

    void setContent(const QString& text);

signals:
    void backRequested();

private slots:
    void on_backButton_released();

private:
    Ui::HelpInfoScreen *ui;
};

#endif // HELPINFOSCREEN_H

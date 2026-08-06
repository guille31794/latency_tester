#ifndef REGISTRYDISPLAYSCREEN_H
#define REGISTRYDISPLAYSCREEN_H

#include "dataModel.hpp"
#include <QWidget>

class QCustomPlot;

namespace Ui {
class RegistryDisplayScreen;
}

/**
 * @brief The RegistryDisplayScreen class displays a saved measurement:
 * name, date, latency average, time factor, and the latency graph.
 */
class RegistryDisplayScreen : public QWidget
{
    Q_OBJECT

public:
    explicit RegistryDisplayScreen(QWidget *parent = nullptr);
    ~RegistryDisplayScreen();
    void changeEvent(QEvent *event) override;

    /**
     * @brief displayMeasure shows the given measurement data and plots it.
     */
    void displayMeasure(const Measures& measure);

signals:
    void backRequested();

private slots:
    void on_backButton_released();

private:
    void plotMeasure(const Measures& measure);

    Ui::RegistryDisplayScreen *ui;
};

#endif // REGISTRYDISPLAYSCREEN_H

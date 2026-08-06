#ifndef MEASURESCREEN_H
#define MEASURESCREEN_H

#include "dataModel.hpp"
#include "sensoroperator.h"
#include <QWidget>
#include <QPointer>
#include <QTimer>
#include <QColor>
#include <functional>

namespace Ui {
class MeasureScreen;
}

/**
 * @brief The MeasureScreen class handles the measurement taking process:
 * calibration, start/stop measuring, plotting results.
 */
class MeasureScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MeasureScreen(QWidget *parent = nullptr);
    ~MeasureScreen();
    void changeEvent(QEvent *event) override;

signals:
    void backRequested();

private slots:
    void on_backButton_released();
    void on_startMeasureButton_released();
    void on_stopMeasureButton_released();
    void on_calibrateButton_released();
    void on_TimeFactorSlider_valueChanged(int value);
    void on_DurationSlider_valueChanged(int value);

private:
    /**
     * @brief plotMeasure draws the latency graph from current measurement data.
     * Uses parallel filtering for datasets > 100 points.
     */
    void plotMeasure();
    /**
     * @brief flashWidget makes a widget blink through a list of colors for visual feedback.
     * @param widget the widget to flash (typically a QPushButton)
     * @param colors list of colors to cycle through
     * @param durationMs total duration of the flashing (0 = indefinite until stopFlash is called)
     * @param intervalMs time between color changes in milliseconds (default 250)
     * @param onFinished optional callback invoked when flashing completes
     */
    void flashWidget(QWidget* widget, const QList<QColor>& colors, int durationMs, int intervalMs = 250, std::function<void()> onFinished = nullptr);
    /**
     * @brief stopFlash stops any ongoing flash on the given widget immediately.
     * Triggers the onFinished callback if one was provided.
     */
    void stopFlash();
    /**
     * @brief setMeasureWidgetsEnabled enables or disables all interactive widgets
     * in the measurement screen. Used during blocking operations.
     * @param enabled true to enable, false to disable
     */
    void setMeasureWidgetsEnabled(bool enabled);

    Ui::MeasureScreen *ui;
    Measures mMeasure;
    SensorOperator mSensorOperator;

    // Flash state tracking
    QPointer<QTimer> mFlashTimer;
    QWidget* mFlashWidget{nullptr};
    QString mFlashOriginalStyle;
    std::function<void()> mFlashOnFinished;
};

#endif // MEASURESCREEN_H

#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "dataModel.hpp"
#include <QMainWindow>
#include <QMultiMap>
#include <QPointer>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE
namespace Ui { class StartScreen; }
QT_END_NAMESPACE

class StartScreen : public QMainWindow
{
    Q_OBJECT

public:
    StartScreen(QWidget *parent = nullptr);
    ~StartScreen();

private slots:
    void on_helpButton_released();

private:
    void init();
    void widgetsMapInit();
    void setUpNextScreen(QList<QPointer<QWidget>> &toHide, QList<QPointer<QWidget>> &toShow);
    Ui::StartScreen *ui;
    MenuScreen mCurrentScreen;
    QMultiMap<MenuScreen, QPointer<QWidget>> mWidgets;
};
#endif // STARTSCREEN_H

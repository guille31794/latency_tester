#ifndef REGISTRYSCREEN_H
#define REGISTRYSCREEN_H

#include "dataModel.hpp"
#include "jsonoperator.h"
#include <QWidget>
#include <QPointer>
#include <QFileSystemModel>

namespace Ui {
class RegistryScreen;
}

class RenamePopUp;

/**
 * @brief The RegistryScreen class displays the list of saved measurements
 * and allows the user to check, delete or rename them.
 */
class RegistryScreen : public QWidget
{
    Q_OBJECT

public:
    explicit RegistryScreen(QWidget *parent = nullptr);
    ~RegistryScreen();
    void changeEvent(QEvent *event) override;

signals:
    void backRequested();
    void measureSelected(const Measures& measure);

private slots:
    void on_backButton_released();
    void on_checkRegistryEntryButton_released();
    void on_deleteRegistryEntryButton_released();
    void on_renameRegistryEntryButton_released();

public slots:
    void changedName(const QString& name);
    void reEnableRegistryButtons();

private:
    void loadRegistry();

    Ui::RegistryScreen *ui;
    JsonOperator mJsonOperator;
    Measures mMeasure;
    QPointer<RenamePopUp> mRenameWindow;
};

#endif // REGISTRYSCREEN_H

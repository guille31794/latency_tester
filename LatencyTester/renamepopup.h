#ifndef RENAMEPOPUP_H
#define RENAMEPOPUP_H

#include "startscreen.h"
#include <QDialog>
#include <QAbstractButton>
#include <QProcess>
#include <QPointer>

namespace Ui {
class RenamePopUp;
}

/**
 * @brief The RenamePopUp class allows to rename a registry measure file in a new window
 */
class RenamePopUp : public QDialog
{
    Q_OBJECT

public:
    explicit RenamePopUp(QWidget *parent = nullptr);
    ~RenamePopUp();
    void setName(QString name);

signals:
    void nameSaved(const QString& name);
    void reEnableRegistryButtons();

private slots:
    void on_renameButtonBox_accepted();
    void on_renameButtonBox_clicked(QAbstractButton *button);

private:
    Ui::RenamePopUp *ui;
};

#endif // RENAMEPOPUP_H

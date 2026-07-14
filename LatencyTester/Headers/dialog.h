#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void setFileNameText(const QString& name);

private slots:
    void on_okButton_released();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H

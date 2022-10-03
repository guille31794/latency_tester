#include "dialog.h"
#include "ui_dialog.h"

const QString openFileWarning{"Warning! Unable to open: "};
const QString openFileWarningEnd{" . Retry again."};

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    // Hide close button
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setFileNameText(const QString &name)
{
    ui->warningLabel->setText(openFileWarning + name + openFileWarningEnd);
}

void Dialog::on_okButton_released()
{
    close();
}


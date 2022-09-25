#include "renamepopup.h"
#include "ui_renamepopup.h"

RenamePopUp::RenamePopUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenamePopUp)
{
    ui->setupUi(this);
}

RenamePopUp::~RenamePopUp()
{
    delete ui;
}

void RenamePopUp::setName(const QString &name)
{
    ui->renameText->setText(name);
}

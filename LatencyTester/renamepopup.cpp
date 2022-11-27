#include "renamepopup.h"
#include "ui_renamepopup.h"

const QString VIRTUAL_KEYBOARD{"onboard"};

RenamePopUp::RenamePopUp(QWidget *parent) :
    QDialog(parent),
    ui{new Ui::RenamePopUp}
{
    // Hide close button
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    connect(this, &RenamePopUp::nameSaved, (StartScreen*)this->parentWidget(), &StartScreen::changedName);
    connect(this, &RenamePopUp::reEnableRegistryButtons, (StartScreen*)this->parentWidget(), &StartScreen::reEnableRegistryButtons);
    ui->setupUi(this);
    ui->renameText->setAttribute(Qt::WA_AcceptTouchEvents);
}

RenamePopUp::~RenamePopUp()
{
    delete ui;
}

void RenamePopUp::setName( QString name)
{
    ui->renameText->setText(name);
}

void RenamePopUp::on_renameButtonBox_accepted()
{
    QString name = ui->renameText->toPlainText() + ".json";
    emit nameSaved(name);
}

void RenamePopUp::on_renameButtonBox_clicked(QAbstractButton *button)
{
    if(ui->renameButtonBox->Discard == ui->renameButtonBox->standardButton(button))
    {
        emit reEnableRegistryButtons();
        close();
    }
}


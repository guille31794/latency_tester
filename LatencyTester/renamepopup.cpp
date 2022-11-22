#include "renamepopup.h"
#include "ui_renamepopup.h"

const QString VIRTUAL_KEYBOARD{"matchbox-keyboard"};

RenamePopUp::RenamePopUp(QWidget *parent) :
    QDialog(parent),
    ui{new Ui::RenamePopUp}, mVirtualKeyboard{new QProcess()}
{
    // Hide close button
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    connect(this, &RenamePopUp::nameSaved, (StartScreen*)this->parentWidget(), &StartScreen::changedName);
    ui->setupUi(this);
    ui->renameText->setAttribute(Qt::WA_AcceptTouchEvents);
    mVirtualKeyboard->start(VIRTUAL_KEYBOARD, {}, QIODevice::OpenModeFlag::ReadWrite);
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
    mVirtualKeyboard->close();
}

void RenamePopUp::on_renameButtonBox_clicked(QAbstractButton *button)
{
    if(ui->renameButtonBox->Discard == ui->renameButtonBox->standardButton(button))
    {
        close();
        mVirtualKeyboard->close();
    }
}


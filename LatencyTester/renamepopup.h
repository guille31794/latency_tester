#ifndef RENAMEPOPUP_H
#define RENAMEPOPUP_H

#include <QDialog>

namespace Ui {
class RenamePopUp;
}

class RenamePopUp : public QDialog
{
    Q_OBJECT

public:
    explicit RenamePopUp(QWidget *parent = nullptr);
    ~RenamePopUp();
    void setName(const QString& name);
private:
    Ui::RenamePopUp *ui;
};

#endif // RENAMEPOPUP_H

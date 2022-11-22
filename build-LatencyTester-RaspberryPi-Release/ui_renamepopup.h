/********************************************************************************
** Form generated from reading UI file 'renamepopup.ui'
**
** Created by: Qt User Interface Compiler version 5.15.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENAMEPOPUP_H
#define UI_RENAMEPOPUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_RenamePopUp
{
public:
    QDialogButtonBox *renameButtonBox;
    QLabel *renameLabel;
    QTextEdit *renameText;

    void setupUi(QDialog *RenamePopUp)
    {
        if (RenamePopUp->objectName().isEmpty())
            RenamePopUp->setObjectName(QString::fromUtf8("RenamePopUp"));
        RenamePopUp->resize(280, 181);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RenamePopUp->sizePolicy().hasHeightForWidth());
        RenamePopUp->setSizePolicy(sizePolicy);
        RenamePopUp->setMinimumSize(QSize(280, 181));
        RenamePopUp->setMaximumSize(QSize(280, 181));
        renameButtonBox = new QDialogButtonBox(RenamePopUp);
        renameButtonBox->setObjectName(QString::fromUtf8("renameButtonBox"));
        renameButtonBox->setGeometry(QRect(10, 130, 251, 41));
        renameButtonBox->setOrientation(Qt::Horizontal);
        renameButtonBox->setStandardButtons(QDialogButtonBox::Discard|QDialogButtonBox::Save);
        renameButtonBox->setCenterButtons(false);
        renameLabel = new QLabel(RenamePopUp);
        renameLabel->setObjectName(QString::fromUtf8("renameLabel"));
        renameLabel->setGeometry(QRect(21, 10, 231, 20));
        renameLabel->setAlignment(Qt::AlignCenter);
        renameLabel->setTextInteractionFlags(Qt::NoTextInteraction);
        renameText = new QTextEdit(RenamePopUp);
        renameText->setObjectName(QString::fromUtf8("renameText"));
        renameText->setGeometry(QRect(20, 60, 231, 31));

        retranslateUi(RenamePopUp);
        QObject::connect(renameButtonBox, SIGNAL(accepted()), RenamePopUp, SLOT(accept()));
        QObject::connect(renameButtonBox, SIGNAL(rejected()), RenamePopUp, SLOT(reject()));

        QMetaObject::connectSlotsByName(RenamePopUp);
    } // setupUi

    void retranslateUi(QDialog *RenamePopUp)
    {
        RenamePopUp->setWindowTitle(QCoreApplication::translate("RenamePopUp", "Renombrar", nullptr));
        renameLabel->setText(QCoreApplication::translate("RenamePopUp", "Inserte nuevo nombre", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RenamePopUp: public Ui_RenamePopUp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENAMEPOPUP_H

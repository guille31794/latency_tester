#include "registryscreen.h"
#include "ui_registryscreen.h"
#include "renamepopup.h"
#include "extensionfiledelegate.h"
#include <QDir>
#include <QEvent>
#include <QApplication>
#include <QDebug>

const QString MEASURES_DIR{"/Measures"};

RegistryScreen::RegistryScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistryScreen)
    , mRenameWindow(new RenamePopUp(this))
{
    ui->setupUi(this);
    loadRegistry();

    connect(mRenameWindow, &RenamePopUp::nameSaved, this, &RegistryScreen::changedName);
    connect(mRenameWindow, &RenamePopUp::reEnableRegistryButtons, this, &RegistryScreen::reEnableRegistryButtons);
}

RegistryScreen::~RegistryScreen()
{
    delete ui;
}

void RegistryScreen::loadRegistry()
{
    QPointer<QFileSystemModel> model = new QFileSystemModel(this);
    QPointer<NoIconOrExtensionFileDelegate> delegate = new NoIconOrExtensionFileDelegate(this);

#ifdef RASPBERRY_PI
    QString registryPath = QDir::homePath() + MEASURES_DIR;
#else
    QString registryPath = QDir::currentPath() + MEASURES_DIR;
#endif

    QDir registryFolder{registryPath};
    if (!registryFolder.exists())
    {
        registryFolder.mkpath(registryPath);
    }

    model->setRootPath(registryPath);
    ui->registryTreeView->setModel(model);
    ui->registryTreeView->setItemDelegate(delegate);
    ui->registryTreeView->setRootIndex(model->index(registryPath));
    ui->registryTreeView->hideColumn(1);
    ui->registryTreeView->hideColumn(2);
    ui->registryTreeView->hideColumn(3);
    ui->registryTreeView->setColumnWidth(0, 450);
    ui->registryTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->registryTreeView->setHeaderHidden(true);

    mJsonOperator.setPath(registryPath);
}

void RegistryScreen::on_backButton_released()
{
    emit backRequested();
}

void RegistryScreen::on_checkRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = qobject_cast<QFileSystemModel*>(ui->registryTreeView->model());
    if (!model || !ui->registryTreeView->currentIndex().isValid())
        return;

    if (mJsonOperator.loadFileFromDisk(model->filePath(ui->registryTreeView->currentIndex())))
    {
        mJsonOperator.parseJsonToStruct(mMeasure);
        emit measureSelected(mMeasure);
    }
}

void RegistryScreen::on_deleteRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = qobject_cast<QFileSystemModel*>(ui->registryTreeView->model());
    if (model && ui->registryTreeView->currentIndex().isValid())
    {
        model->remove(ui->registryTreeView->currentIndex());
    }
}

void RegistryScreen::on_renameRegistryEntryButton_released()
{
    QPointer<QFileSystemModel> model = qobject_cast<QFileSystemModel*>(ui->registryTreeView->model());
    if (!model || !ui->registryTreeView->currentIndex().isValid())
        return;

    ui->renameRegistryEntryButton->setDisabled(true);
    ui->checkRegistryEntryButton->setDisabled(true);
    ui->deleteRegistryEntryButton->setDisabled(true);

    QString nameWithExtension = model->fileName(ui->registryTreeView->currentIndex());
    QString name = nameWithExtension.split(".", Qt::SkipEmptyParts, Qt::CaseInsensitive).first();
    mRenameWindow->setName(name);
    mRenameWindow->show();
}

void RegistryScreen::changedName(const QString& name)
{
    QPointer<QFileSystemModel> model = qobject_cast<QFileSystemModel*>(ui->registryTreeView->model());
    if (!model) return;

    QString currentNameWithExtension = model->fileName(ui->registryTreeView->currentIndex());
    QString path = model->rootPath();
    QFile::rename(path + "/" + currentNameWithExtension, path + "/" + name + ".json");
    reEnableRegistryButtons();
}

void RegistryScreen::reEnableRegistryButtons()
{
    ui->deleteRegistryEntryButton->setEnabled(true);
    ui->checkRegistryEntryButton->setEnabled(true);
    ui->renameRegistryEntryButton->setEnabled(true);
}

void RegistryScreen::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

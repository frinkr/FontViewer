#include <QFileDialog>
#include <QListWidgetItem>

#include "QXApplication.h"
#include "QXFontManager.h"
#include "QXPreferencesDialog.h"

#include "ui_QXPreferencesDialog.h"

QXPreferencesDialog::QXPreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::QXPreferencesDialog) {
    ui_->setupUi(this);
    ui_->addButton->setIcon(qxApp->loadIcon(":/images/plus.png"));
    ui_->removeButton->setIcon(qxApp->loadIcon(":/images/minus.png"));
    ui_->dirListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    for (const QString & folder: QXFontManager::get().systemFontFolders()) {
        QListWidgetItem * item = new QListWidgetItem(folder, ui_->dirListWidget, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    for (const QString & folder: QXFontManager::get().userFontFolders()) 
        new QListWidgetItem(folder, ui_->dirListWidget, 1);

    connect(ui_->addButton, &QToolButton::clicked, this, &QXPreferencesDialog::onAddButtonClicked);
    connect(ui_->removeButton, &QToolButton::clicked, this, &QXPreferencesDialog::onRemoveButtonClicked);
}

QXPreferencesDialog::~QXPreferencesDialog() {
    delete ui_;
}

void
QXPreferencesDialog::showPreferences() {
    QXPreferencesDialog dialog;
    dialog.exec();
}

void
QXPreferencesDialog::onAddButtonClicked() {
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Add Font Directory"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        // check exists
        if(ui_->dirListWidget->findItems(dir, Qt::MatchFixedString).isEmpty())
            new QListWidgetItem(dir, ui_->dirListWidget, 1);
    }
}

void
QXPreferencesDialog::onRemoveButtonClicked() {
    int row = ui_->dirListWidget->currentRow();
    if (row != -1)
        ui_->dirListWidget->takeItem(row);
}

void
QXPreferencesDialog::accept() {
    QDialog::accept();
}

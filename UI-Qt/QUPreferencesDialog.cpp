#include <QFileDialog>
#include <QListWidgetItem>

#include "QUApplication.h"
#include "QUFontManager.h"
#include "QUPreferencesDialog.h"

#include "ui_QUPreferencesDialog.h"

QUPreferencesDialog::QUPreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::QUPreferencesDialog) {
    ui_->setupUi(this);
    ui_->addButton->setIcon(quApp->loadIcon(":/images/plus.png"));
    ui_->removeButton->setIcon(quApp->loadIcon(":/images/minus.png"));
    ui_->dirListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    for (const QString & folder: QUFontManager::get().systemFontFolders()) {
        QListWidgetItem * item = new QListWidgetItem(folder, ui_->dirListWidget, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    for (const QString & folder: QUFontManager::get().userFontFolders()) 
        new QListWidgetItem(folder, ui_->dirListWidget, 1);

    connect(ui_->addButton, &QToolButton::clicked, this, &QUPreferencesDialog::onAddButtonClicked);
    connect(ui_->removeButton, &QToolButton::clicked, this, &QUPreferencesDialog::onRemoveButtonClicked);
}

QUPreferencesDialog::~QUPreferencesDialog() {
    delete ui_;
}

void
QUPreferencesDialog::showPreferences() {
    QUPreferencesDialog dialog;
    dialog.exec();
}

void
QUPreferencesDialog::onAddButtonClicked() {
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
QUPreferencesDialog::onRemoveButtonClicked() {
    int row = ui_->dirListWidget->currentRow();
    if (row != -1)
        ui_->dirListWidget->takeItem(row);
}

void
QUPreferencesDialog::accept() {
    QDialog::accept();
}

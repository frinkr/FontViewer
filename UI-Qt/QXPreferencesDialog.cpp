#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>

#include "QXApplication.h"
#include "QXFontManager.h"
#include "QXPreferencesDialog.h"

#include "ui_QXPreferencesDialog.h"

namespace {
    constexpr int kSystemFontFolder = 0;
    constexpr int kUserFontFolder = 1;
}

QXPreferencesDialog::QXPreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::QXPreferencesDialog) {
    ui_->setupUi(this);
    ui_->addButton->setIcon(qApp->loadIcon(":/images/plus.png"));
    ui_->removeButton->setIcon(qApp->loadIcon(":/images/minus.png"));
    ui_->dirListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    for (const QString & folder: QXFontManager::instance().systemFontFolders()) {
        QListWidgetItem * item = new QListWidgetItem(folder, ui_->dirListWidget, kSystemFontFolder);
        item->setForeground(palette().color(QPalette::Disabled, QPalette::WindowText));
    }

    for (const QString & folder: QXFontManager::instance().userFontFolders())
        new QListWidgetItem(folder, ui_->dirListWidget, kUserFontFolder);

    connect(ui_->dirListWidget, &QListWidget::currentItemChanged, this, &QXPreferencesDialog::onCurrentDirItemChanged);
    connect(ui_->addButton, &QToolButton::clicked, this, &QXPreferencesDialog::onAddButtonClicked);
    connect(ui_->removeButton, &QToolButton::clicked, this, &QXPreferencesDialog::onRemoveButtonClicked);
}

QXPreferencesDialog::~QXPreferencesDialog() {
    delete ui_;
}

QStringList
QXPreferencesDialog::userFontFolders() const {
    QStringList list;
    for (int i = 0; i < ui_->dirListWidget->count(); ++ i) {
        auto item = ui_->dirListWidget->item(i);
        if (item->type() == kUserFontFolder)
            list.append(item->text());
    }
    return list;
}

void
QXPreferencesDialog::showPreferences() {
    QXPreferencesDialog dialog;
    dialog.exec();
}

void
QXPreferencesDialog::onCurrentDirItemChanged(QListWidgetItem * current, QListWidgetItem * previous) {
    int type = 0;
    if (current) 
        type = current->type();

    ui_->removeButton->setEnabled(type == kUserFontFolder);
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
    auto item = ui_->dirListWidget->currentItem();
    if (row != -1 && item && item->type() == kUserFontFolder) 
        ui_->dirListWidget->takeItem(row);
}

void
QXPreferencesDialog::accept() {
    auto newUserFolders = userFontFolders();
    auto oldUserFolders = QXFontManager::instance().userFontFolders();
    if (newUserFolders != oldUserFolders) {
        QMessageBox::information(this, tr("Restart Required"), tr("The font folders change will take effect after restart."));
        QXFontManager::instance().setUserFontFolders(newUserFolders);
    }
    
    QDialog::accept();
}

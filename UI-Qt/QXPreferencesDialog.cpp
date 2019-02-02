#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>

#include "QXApplication.h"
#include "QXFontManager.h"
#include "QXPreferences.h"
#include "QXPreferencesDialog.h"
#include "QXTheme.h"

#include "ui_QXPreferencesDialog.h"

namespace {
    constexpr int kSystemFontFolder = 0;
    constexpr int kUserFontFolder = 1;
}

QXPreferencesDialog::QXPreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::QXPreferencesDialog) {
    ui_->setupUi(this);

    // Font Folder list widget
    ui_->addFontFolderButton->setIcon(qApp->loadIcon(":/images/plus.png"));
    ui_->removeFontFolderButton->setIcon(qApp->loadIcon(":/images/minus.png"));
    ui_->fontFolderListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    for (const QString & folder: QXFontManager::instance().systemFontFolders()) {
        QListWidgetItem * item = new QListWidgetItem(folder, ui_->fontFolderListWidget, kSystemFontFolder);
        item->setForeground(palette().color(QPalette::Disabled, QPalette::WindowText));
    }
    for (const QString & folder: QXFontManager::instance().userFontFolders())
        new QListWidgetItem(folder, ui_->fontFolderListWidget, kUserFontFolder);

    connect(ui_->fontFolderListWidget, &QListWidget::currentItemChanged, this, &QXPreferencesDialog::onCurrentFontFoldertemChanged);
    connect(ui_->addFontFolderButton, &QToolButton::clicked, this, &QXPreferencesDialog::onAddFontFolderButtonClicked);
    connect(ui_->removeFontFolderButton, &QToolButton::clicked, this, &QXPreferencesDialog::onRemoveFontFolderButtonClicked);

    // Theme combobox
    ui_->themeCombobox->addItems(QXTheme::availableThemes());
    ui_->themeCombobox->setCurrentText(QXPreferences::theme());
}

QXPreferencesDialog::~QXPreferencesDialog() {
    delete ui_;
}

QStringList
QXPreferencesDialog::userFontFolders() const {
    QStringList list;
    for (int i = 0; i < ui_->fontFolderListWidget->count(); ++ i) {
        auto item = ui_->fontFolderListWidget->item(i);
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
QXPreferencesDialog::onCurrentFontFoldertemChanged(QListWidgetItem * current, QListWidgetItem * previous) {
    Q_UNUSED(previous);

    int type = 0;
    if (current) 
        type = current->type();

    ui_->removeFontFolderButton->setEnabled(type == kUserFontFolder);
}

void
QXPreferencesDialog::onAddFontFolderButtonClicked() {
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Add Font Directory"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        // check exists
        if(ui_->fontFolderListWidget->findItems(dir, Qt::MatchFixedString).isEmpty())
            new QListWidgetItem(dir, ui_->fontFolderListWidget, 1);
    }
}

void
QXPreferencesDialog::onRemoveFontFolderButtonClicked() {
    int row = ui_->fontFolderListWidget->currentRow();
    auto item = ui_->fontFolderListWidget->currentItem();
    if (row != -1 && item && item->type() == kUserFontFolder) 
        ui_->fontFolderListWidget->takeItem(row);
}

void
QXPreferencesDialog::accept() {
    auto newUserFolders = userFontFolders();
    auto oldUserFolders = QXFontManager::instance().userFontFolders();
    if (newUserFolders != oldUserFolders) {
        QMessageBox::information(this, tr("Restart Required"), tr("The font folders change will take effect after restart."));
        QXFontManager::instance().setUserFontFolders(newUserFolders);
    }

    QXPreferences::setTheme(ui_->themeCombobox->currentText());
    QXTheme::setCurrent(ui_->themeCombobox->currentText());

    QDialog::accept();
}

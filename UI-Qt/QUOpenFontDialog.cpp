#include <QFileDialog>
#include "QUFontManager.h"
#include "QUOpenFontDialog.h"
#include "ui_QUOpenFontDialog.h"

QUOpenFontDialog::QUOpenFontDialog(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::QUOpenFontDialog) {
    ui_->setupUi(this);
    
    connect(ui_->browseButton, &QPushButton::clicked,
            this, &QUOpenFontDialog::slotBrowseFile);
    connect(ui_->fontComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QUOpenFontDialog::slotFontSelected);
    
    QUFontManager::get();
}

QUOpenFontDialog::~QUOpenFontDialog() {
    delete ui_;
}

QUFontURI
QUOpenFontDialog::selectedFont() {
    return ui_->fontComboBox->selectedFont();
}

void
QUOpenFontDialog::slotBrowseFile() {
    QString file = QFileDialog::getOpenFileName(
        this,
        tr("Open Font"),
        QString(),
        tr("Fonts (*.otf *.ttf);;All Files (*)"));

}

void
QUOpenFontDialog::slotFontSelected(int index) {
}

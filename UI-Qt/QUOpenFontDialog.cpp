#include <QFileDialog>
#include "QUOpenFontDialog.h"
#include "ui_QUOpenFontDialog.h"

QUOpenFontDialog::QUOpenFontDialog(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::QUOpenFontDialog) {
    ui_->setupUi(this);
    
    connect(ui_->browseButton, &QPushButton::clicked,
            this, &QUOpenFontDialog::slotBrowseFile);
        
    ui_->textEdit->setText("/Data/git/github/FontViewer/Resources/Fonts/MyriadPro-Regular.otf");
}

QUOpenFontDialog::~QUOpenFontDialog() {
    delete ui_;
}

QUFontURI
QUOpenFontDialog::selectedFont() {
    QUFontURI uri{ui_->textEdit->toPlainText(), 0};
    return uri;
}

void
QUOpenFontDialog::slotBrowseFile() {
    QString file = QFileDialog::getOpenFileName(
        this,
        tr("Open Font"),
        QString(),
        tr("Fonts (*.otf *.ttf);;All Files (*)"));

    if (!file.isEmpty())
        ui_->textEdit->setText(file);
}

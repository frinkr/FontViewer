#include <QListWidgetItem>

#include "FontX/FXFace.h"

#include "QXConv.h"
#include "QXDocument.h"
#include "QXFontCollectionDialog.h"
#include "ui_QXFontCollectionDialog.h"

QXFontCollectionDialog::QXFontCollectionDialog(const QString & filePath, QWidget * parent) 
    : QDialog(parent)
    , ui_(new Ui::QXFontCollectionDialog)
    , filePath_(filePath) {
    ui_->setupUi(this);

    auto initFace = FXFace::createFace(toStdString(filePath), 0);
    size_t faceCount = initFace? initFace->faceCount(): 0;
    for (size_t i = 0; i < faceCount; ++ i) {
        auto face = initFace->openFace(i);
        if (face) 
            ui_->listWidget->addItem(QXDocument::faceDisplayName(face));
        else
            ui_->listWidget->addItem(tr("<INVALID FACE>"));
    }
    ui_->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->listWidget->setCurrentRow(0);
}

QXFontCollectionDialog::~QXFontCollectionDialog() {
    delete ui_;
}

int
QXFontCollectionDialog::selectedIndex() const {
    return ui_->listWidget->currentRow();
}

int
QXFontCollectionDialog::selectFontIndex(const QString & filePath) {
    QXFontCollectionDialog dialog(filePath);
    if (dialog.exec() == QDialog::Accepted)
        return dialog.selectedIndex();
    return -1;
}

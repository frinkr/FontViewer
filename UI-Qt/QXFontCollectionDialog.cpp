#include <QListWidgetItem>

#include "FontX/FXFace.h"
#include "FontX/FXPDF.h"

#include "QXConv.h"
#include "QXDocument.h"
#include "QXFontCollectionDialog.h"
#include "ui_QXFontCollectionDialog.h"

QXFontCollectionDialog::QXFontCollectionDialog(const QString & filePath, FXPtr<FXFace> initFace, QWidget * parent)
    : QDialog(parent)
    , ui_(new Ui::QXFontCollectionDialog)
    , filePath_(filePath) {
    ui_->setupUi(this);
    if (!initFace)
       initFace = FXFace::createFace(toStdString(filePath), 0);
    size_t faceCount = initFace? initFace->faceCount(): 0;
    for (size_t i = 0; i < faceCount; ++ i) {
        auto face = initFace->openFace(i);
        if (face)
            (new QListWidgetItem(QXDocument::faceDisplayName(face), ui_->listWidget))->setData(Qt::UserRole, int(i));
        else
            ui_->listWidget->addItem(tr("<INVALID FACE>"));
    }

    ui_->listWidget->sortItems();
    ui_->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->listWidget->setCurrentRow(0);
}

QXFontCollectionDialog::~QXFontCollectionDialog() {
    delete ui_;
}

int
QXFontCollectionDialog::selectedIndex() const {
    return ui_->listWidget->currentItem()->data(Qt::UserRole).toInt();
}

int
QXFontCollectionDialog::selectFontIndex(const QString & filePath, FXPtr<FXFace> openFace) {
    QXFontCollectionDialog dialog(filePath, openFace);
    if (dialog.exec() == QDialog::Accepted)
        return dialog.selectedIndex();
    return -1;
}

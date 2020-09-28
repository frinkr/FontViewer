#include <QListWidgetItem>

#include "FontX/FXFace.h"
#include "FontX/FXPDF.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocument.h"
#include "QXFontCollectionDialog.h"
#include "QXPreferences.h"
#include "ui_QXFontCollectionDialog.h"


namespace {
    void
    loadFaceList(QListWidget * listWidget, FXPtr<FXFace> currFace) {
        listWidget->clear();
        
        if (!currFace)
            return;
        
        size_t faceCount = currFace->faceCount();
        for (size_t i = 0; i < faceCount; ++ i) {
            auto face = currFace->openFace(i);
            if (face) {
                auto item = new QListWidgetItem(QXDocument::faceDisplayName(face, QXPreferences::fontDisplayLanguage()), listWidget);
                item->setData(Qt::UserRole, int(i));
                
                if (i == currFace->index())
                    listWidget->setCurrentItem(item);
                
                if (!face->hasValidFaceData()) {
                    item->setData(Qt::DecorationRole, qApp->loadIcon(":/images/warning.png"));
                    item->setToolTip("No font data.");
                }
            }
            else
                listWidget->addItem(QXFontCollectionDialog::tr("<INVALID FACE>"));
        }

        listWidget->sortItems();
        listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    }
}

void
QXFontCollectionWidget::setDocument(QXDocument * document) {
    document_ = document;
    reload();
}

void
QXFontCollectionWidget::setCurrentFace(int index) {
    for (int i = 0; i < list_->count(); ++ i) {
        auto item = list_->item(i);
        if (itemData(item) == index) {
            list_->setCurrentItem(item);
            break;
        }
    }
}

int
QXFontCollectionWidget::currentFace() const {
    return itemData(list_->currentItem());
}

void
QXFontCollectionWidget::reload() {
    if (!list_) {
        list_ = new QListWidget(this);
        connect(list_, &QListWidget::itemDoubleClicked, this, &QXFontCollectionWidget::onListItemDoubleClicked);
        
        QVBoxLayout * layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->addWidget(list_);
        setLayout(layout);
    }

    auto currFace = document_->face();
    loadFaceList(list_, currFace);
}

void
QXFontCollectionWidget::onListItemDoubleClicked(QListWidgetItem * item) {
    if (auto index = itemData(item); index != -1)
        emit fontDoubleClicked(index);
}

int
QXFontCollectionWidget::itemData(QListWidgetItem * item) const {
    bool ok = false;
    auto index = item->data(Qt::UserRole).toInt(&ok);
    return ok? index: -1;
}

QXFontCollectionDialog::QXFontCollectionDialog(const QString & filePath, FXPtr<FXFace> initFace, QWidget * parent)
    : QDialog(parent)
    , ui_(new Ui::QXFontCollectionDialog)
    , filePath_(filePath) {
    ui_->setupUi(this);
    if (!initFace)
       initFace = FXFace::createFace(toStdString(filePath), 0);
    loadFaceList(ui_->listWidget, initFace);
    connect(ui_->listWidget, &QListWidget::itemDoubleClicked, this, &QXFontCollectionDialog::accept);
    
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

#include <QListWidgetItem>

#include "FontX/FXFace.h"
#include "FontX/FXPDF.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocument.h"
#include "QXRelatedFontsWidget.h"
#include "QXPreferences.h"


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
                    item->setData(Qt::DecorationRole, qApp->loadIcon(":/images/error.png"));
                    item->setData(Qt::ToolTipRole, "No font data.");
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
QXRelatedFontsWidget::setDocument(QXDocument * document) {
    document_ = document;
    reload();
}

void
QXRelatedFontsWidget::setCurrentFace(int index) {
    for (int i = 0; i < list_->count(); ++ i) {
        auto item = list_->item(i);
        if (itemData(item) == index) {
            list_->setCurrentItem(item);
            break;
        }
    }
}

int
QXRelatedFontsWidget::currentFace() const {
    return itemData(list_->currentItem());
}

void
QXRelatedFontsWidget::reload() {
    if (!list_) {
        list_ = new QListWidget(this);
        connect(list_, &QListWidget::itemDoubleClicked, this, &QXRelatedFontsWidget::onListItemDoubleClicked);
        
        QVBoxLayout * layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->addWidget(list_);
        setLayout(layout);
    }

    auto currFace = document_->face();
    loadFaceList(list_, currFace);
}

void
QXRelatedFontsWidget::onListItemDoubleClicked(QListWidgetItem * item) {
    if (auto index = itemData(item); index != -1)
        emit fontDoubleClicked(index);
}

int
QXRelatedFontsWidget::itemData(QListWidgetItem * item) const {
    bool ok = false;
    auto index = item->data(Qt::UserRole).toInt(&ok);
    return ok? index: -1;
}

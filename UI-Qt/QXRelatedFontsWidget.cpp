#include <QListWidgetItem>
#include <QVBoxLayout>
#include "FontX/FXFace.h"
#include "FontX/FXPDF.h"

#include "QXApplication.h"
#include "QXFontManager.h"
#include "QXDocument.h"
#include "QXRelatedFontsWidget.h"
#include "QXPreferences.h"

namespace {
    void
    loadFontCollection(QListWidget * listWidget, FXPtr<FXFace> currFace) {
        listWidget->clear();
        
        if (!currFace)
            return;
        
        size_t faceCount = currFace->faceCount();
        if (faceCount < 2)
            return;
        
        for (size_t i = 0; i < faceCount; ++ i) {
            auto face = currFace->openFace(i);
            if (face) {
                auto item = new QListWidgetItem(QXDocument::faceDisplayName(face, QXPreferences::fontDisplayLanguage()), listWidget);
                
                QXFontURI uri = QXFontURI::fromDesc(face->desc());
                
                item->setData(Qt::UserRole, QVariant::fromValue(uri));
                
                if (i == currFace->index())
                    listWidget->setCurrentItem(item);
                
                if (!face->hasValidFaceData()) {
                    item->setData(Qt::DecorationRole, qApp->loadIcon(":/images/error.png"));
                    item->setData(Qt::ToolTipRole, "No font data.");
                }
            }
            else
                listWidget->addItem(QXRelatedFontsWidget::tr("<INVALID FACE>"));
        }
    }

    void
    loadFontFamily(QListWidget * listWidget, FXPtr<FXFace> currFace) {
        auto db = QXFontManager::instance().db();
        auto family = db->findFamily(currFace->postscriptName());

        for (auto & i: family) {
            auto item = new QListWidgetItem(QXDocument::faceDisplayName(db->faceAttributes(i), QXPreferences::fontDisplayLanguage()), listWidget);
            QXFontURI uri = QXFontURI::fromDesc(db->faceDescriptor(i));
            item->setData(Qt::UserRole, QVariant::fromValue(uri));
        }
    }
}

void
QXRelatedFontsWidget::setDocument(QXDocument * document) {
    document_ = document;
    reload();
}

void
QXRelatedFontsWidget::selectCurrentFace() {
    const auto & uri = document_->uri();
    for (int i = 0; i < list_->count(); ++ i) {
       auto item = list_->item(i);
       if (itemData(item) == uri) {
           list_->setCurrentItem(item);
           break;
       }
   }
}

void
QXRelatedFontsWidget::reload() {
    if (!list_) {
        list_ = new QListWidget(this);
        list_->setSelectionMode(QAbstractItemView::SingleSelection);
        
        connect(list_, &QListWidget::itemDoubleClicked, this, &QXRelatedFontsWidget::onListItemDoubleClicked);
        
        QVBoxLayout * layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(list_);
        setLayout(layout);
    }

    auto currFace = document_->face();
    loadFontCollection(list_, currFace);
    list_->sortItems();
    loadFontFamily(list_, currFace);

    selectCurrentFace();
}


void
QXRelatedFontsWidget::onListItemDoubleClicked(QListWidgetItem * item) {
    if (auto desc = itemData(item); desc.filePath.size())
        emit fontDoubleClicked(desc);
}

QXFontURI
QXRelatedFontsWidget::itemData(QListWidgetItem * item) const {
    return item->data(Qt::UserRole).value<QXFontURI>();
}

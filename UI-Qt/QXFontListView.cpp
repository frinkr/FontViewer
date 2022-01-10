#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTextBrowser>
#include <QTextDocument>

#include "FontX/FXCache.h"
#include "QXApplication.h"
#include "QXConv.h"
#include "QXFontListModel.h"
#include "QXFontListView.h"
#include "QXHtmlTemplate.h"
#include "QXImageHelpers.h"
#include "QXPopoverWindow.h"
#include "QXPreferences.h"
#include "QXResources.h"

namespace {
    class QXFontListViewItemDelegate : public QStyledItemDelegate {
    private:
        qreal    infoIconMargin_{ 10 };
        qreal    infoIconSize_{ 20 };
        QXFontListViewPreview preview_{QXFontListViewPreview::defaultPreview()};
        
        mutable FXCache<FXFaceDescriptor, FXPtr<FXFace>> faceCache_{50}; // cache 50 faces
        mutable QRect    infoIconRect_{};
    public:
        QXFontListViewItemDelegate(QWidget * parent = 0)
            : QStyledItemDelegate(parent) {
        }

        ~QXFontListViewItemDelegate() {
            faceCache_.clear();   
        }

        void
        setPreview(const QXFontListViewPreview & preview) {
            preview_ = preview;
        }
        
        QRect
        infoIconRect(const QStyleOptionViewItem & option) const {
            qreal right = option.rect.right() - infoIconMargin_;
            qreal left = right - infoIconSize_;
            qreal top = option.rect.top() + (option.rect.height() - infoIconSize_) / 2;
            return QRect(left, top, infoIconSize_, infoIconSize_);
        }

        bool 
        editorEvent(QEvent * event, QAbstractItemModel*, const QStyleOptionViewItem & option, const QModelIndex & index) override {
            if (event->type() == QEvent::MouseButtonRelease) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

                QRect rect = infoIconRect(option);
                if (rect.contains(mouseEvent->pos())) {
                    auto view = qobject_cast<const QXFontListView *>(option.widget);
                    if (view) {
                        QRect globalRect(view->mapToGlobal(rect.topLeft()), view->mapToGlobal(rect.bottomRight()));
                        view->showFontInfoPopover(index, globalRect);
                    }
                }
            }
            return false;
        }

        void
        paint(QPainter * painter,
              const QStyleOptionViewItem & option,
              const QModelIndex & proxyIndex) const override {
            painter->save();
            
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            const bool selected = (option.state & QStyle::State_Selected);
            if (selected)
                painter->fillRect(option.rect, option.palette.brush(QPalette::Active, QPalette::Highlight));
            else if (option.state & QStyle::State_MouseOver)
                painter->fillRect(option.rect, option.palette.alternateBase());
            
            const QXSortFilterFontListModel * proxyModel = qobject_cast<const QXSortFilterFontListModel*>(proxyIndex.model());
            const QXFontListModel * sourceModel = qobject_cast<const QXFontListModel *>(proxyModel->sourceModel());
            const QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);

            // Draw font type icon
            const int fontTypeIconSize = painter->fontInfo().pixelSize() * 1.2;
            QVariant iconVariant = proxyModel->data(proxyIndex, Qt::DecorationRole);
            QIcon icon;
            if (iconVariant.canConvert<QIcon>())
                icon = iconVariant.value<QIcon>();
            if (!icon.isNull()) {
                QPixmap pixmap = icon.pixmap(painter->fontInfo().pixelSize() * 2);
                painter->drawPixmap(QRect(option.rect.left(), option.rect.top(), fontTypeIconSize, fontTypeIconSize), pixmap);
            }
            
            int x = option.rect.left() + 5 + fontTypeIconSize;

            // Draw font name
            QVariant nameVariant = proxyModel->data(proxyIndex, Qt::DisplayRole);
            QString displayName;
            if (nameVariant.canConvert<QString>())
                displayName = nameVariant.value<QString>();
            if (selected)
                painter->setPen(option.palette.highlightedText().color());
            else
                painter->setPen(option.palette.text().color());
            if (!displayName.isEmpty())
                painter->drawText(QRect(x, option.rect.top(), option.rect.width(), option.rect.height()), displayName);

            const FXFaceDescriptor & desc = sourceModel->db()->faceDescriptor(sourceIndex.row());

            // Draw font path
            x += 8 + painter->fontMetrics().horizontalAdvance(displayName);;
            if (!selected)
                painter->setPen(option.palette.color(QPalette::Disabled, QPalette::Text));
            else 
                painter->setPen(option.palette.color(QPalette::HighlightedText));
            painter->drawText(QRect(x, option.rect.top(), option.rect.width() + 99999, option.rect.height()),
                              QString("(%1)").arg(toQString(desc.filePath)));

            // Draw sample text
            FXPtr<FXFace> face;
            if (faceCache_.has(desc))
                face = faceCache_.get(desc);
            else {
                face = FXFace::createFace(desc);
                faceCache_.put(desc, face);
            }
            
            if (face) {
                if (!face->isScalable())
                    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

                auto sample = preview_.sampleText.toStdU32String();

                const qreal sampleHeight = option.rect.height() - fontTypeIconSize;
                const qreal sampleFontSizePt = preview_.fontSize;
                const qreal sampleFontSizePx = pt2px(sampleFontSizePt);
                const qreal sampleFontScale = sampleFontSizePt / face->fontSize();

                QPointF pen(option.rect.left(), option.rect.top()
                            + fontTypeIconSize
                            + (sampleHeight - sampleFontSizePx) / 2
                            + 0.75 * sampleFontSizePx);
                
                for (FXGChar c : sample) {
                    FXGlyph g = face->glyph(c);

                    FXVec2d<int> bmOffset;
                    double bmScale = face->bmScale();
                    FXGlyphImage gi = tintGlyphImageWithColor(face->glyphImage(g.gid), option.palette.color(selected? QPalette::HighlightedText: QPalette::Text));
                    qreal scaledBmHeight = sampleFontScale * gi.pixmap.height * bmScale;
                    qreal scaledBmWidth = sampleFontScale * gi.pixmap.width * bmScale;
                    if (!face->isScalable()) {
                        if (scaledBmHeight >= 0.95 * sampleFontSizePx) {
                            qreal heightFittingScale = 0.95 * sampleFontSizePx / scaledBmHeight;
                            scaledBmWidth *= heightFittingScale;
                            scaledBmHeight = 0.95 * sampleFontSizePx;
                            bmScale *= heightFittingScale;
                        }

                        pen.setY(option.rect.top() + fontTypeIconSize + (sampleFontSizePx + scaledBmHeight) / 2);
                    }

                    if (!gi.pixmap.empty()) {
                        auto img = toQImage(gi);
                        img = img.scaledToWidth(scaledBmWidth * qApp->devicePixelRatio(),
                                                face->isScalable()? Qt::SmoothTransformation: Qt::FastTransformation);

                        const qreal left   = pen.x() + gi.offset.x * sampleFontScale;
                        const qreal bottom = pen.y() - gi.offset.y * sampleFontScale;
                        const qreal right  = left + gi.pixmap.width * bmScale * sampleFontScale;
                        const qreal top    = bottom - gi.pixmap.height * bmScale * sampleFontScale;

                        painter->drawImage(QRect(QPoint(left, top), QPoint(right, bottom)),
                                           img);
                    }
                    qreal advPx = pt2px(face->fontSize()) *  sampleFontScale * g.metrics.horiAdvance / face->upem();
                    if (advPx == 0 || face->upem() == 0)
                        advPx = scaledBmWidth;

                    pen += QPoint(advPx, 0);
                    if (pen.x() > option.rect.right())
                        break;
                }
                
            }

            // Draw info icon
            QXFontListView * window = qobject_cast<QXFontListView*>(option.widget->window());
            bool popoverVisible = false;
            if (window) popoverVisible = window->isFontInfoPopoverVisible();
            if ((selected && popoverVisible) || (option.state & QStyle::State_MouseOver)) {
                QRect iconRect = infoIconRect(option);
                int x1 = option.rect.right() + 3;
                int x0 = iconRect.left() - infoIconMargin_ * 5;
                QLinearGradient gradient(QPoint(x0, option.rect.top()), QPoint(option.rect.right(), option.rect.top())); // diagonal gradient from top-left to bottom-right

                QColor c0 = option.palette.base().color();
                if (selected)
                    c0 = option.palette.brush(QPalette::Active, QPalette::Highlight).color();
                QColor c1 = c0;
                c0.setAlpha(0);
                c1.setAlpha(255);
                gradient.setColorAt(0, c0);
                gradient.setColorAt(1, c1);

                painter->fillRect(QRect(QPoint(x0, option.rect.top()), QSize(x1 - x0, option.rect.height())), gradient);
                QIcon infoIcon = qApp->loadIcon(":/images/info.png");
                
                painter->drawPixmap(iconRect, infoIcon.pixmap(infoIconSize_, infoIconSize_));
            }
            painter->restore();
        }

        QSize
        sizeHint(const QStyleOptionViewItem & option,
                 const QModelIndex & index) const override {
            return QSize(10, pt2px(preview_.fontSize) + 30);
        }
    };

    QMap<QString, QVariant>
    templateValues(const FXFaceDescriptor & desc, const FXFaceAttributes & atts) {
        QMap<QString, QVariant> map;

        map["FULL_NAME"] = QXDocument::faceDisplayName(atts, QXPreferences::fontDisplayLanguage());
        map["PS_NAME"] = toQString(atts.names.postscriptName());
        map["VENDOR"] = toQString(atts.names.vendor());
        map["VERSION"] = toQString(atts.names.version());
        map["FILE"] = toQString(desc.filePath);
        map["INDEX"] = quint32(desc.index);

        map["GLYPH_COUNT"] = quint32(atts.glyphCount);
        map["UPEM"] = quint32(atts.upem);
        map["FORMAT"] = toQString(atts.format);
        map["IS_CID"] = atts.isCID;
        map["CID"] = toQString(atts.cid);
        map["IS_OT_VARIANT"] = atts.isOpenTypeVariable;
        map["IS_MM"] = atts.isMultipleMaster;
        return map;
    }
   
}

const QXFontListViewPreview &
QXFontListViewPreview::defaultPreview() {
    static QXFontListViewPreview dflt {"The quick fox jumps over the lazy dog", 30};
    return dflt;
}

QXFontListView::QXFontListView(QWidget * parent)
    : QListView(parent) {
    
    // List view
    QSortFilterProxyModel * proxy = new QXSortFilterFontListModel(this);
    proxy->setSourceModel(new QXFontListModel(this));
    proxy->sort(0);
    setModel(proxy);
    setItemDelegate(new QXFontListViewItemDelegate(this));
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QListView::customContextMenuRequested, this, &QXFontListView::onFontContextMenuRequested, Qt::QueuedConnection);

}

int
QXFontListView::selectedFontIndex() const {
    return currentSourceIndex().row();
}

QXFontURI
QXFontListView::selectedFont() const {
    int row = selectedFontIndex();
    if (row == -1)
        return QXFontURI{};
    
    auto desc = db()->faceDescriptor(row);
    auto atts = db()->faceAttributes(row);
    QXFontURI uri{toQString(desc.filePath), desc.index};
    return uri;
}

int
QXFontListView::selectFont(int index) {
    QModelIndex proxyIndex = proxyModel()->mapFromSource(sourceModel()->index(index));
    setCurrentIndex(proxyIndex);
    scrollToCurrentIndex();
    return selectedFontIndex();
}

int
QXFontListView::selectFont(const QXFontURI & fontURI) {
    clearFilter();
    int index = sourceModel()->db()->faceIndex({toStdString(fontURI.filePath), fontURI.faceIndex});
    return selectFont(index);
}

void
QXFontListView::setFilter(const QXFontListFilter & filter) {
    proxyModel()->setFilter(filter);
    if (!filter.fontName.isEmpty() || selectedFontIndex() == -1)
        setCurrentIndex(proxyModel()->index(0, 0));
    
    scrollToCurrentIndex();
}
    
void
QXFontListView::clearFilter() {
    proxyModel()->clearFilter();
}

void
QXFontListView::setPreview(const QXFontListViewPreview & preview) {
    QXFontListViewItemDelegate * delegate = dynamic_cast<QXFontListViewItemDelegate *>(itemDelegate());
    if (delegate) {
        delegate->setPreview(preview);
        model()->layoutChanged();
        scrollTo(currentIndex(), QAbstractItemView::PositionAtTop);
    }
}

void
QXFontListView::setDb(FXPtr<FXFaceDatabase> db) {
    sourceModel()->setDb(db);
}

FXPtr<FXFaceDatabase>
QXFontListView::db() const {
    return sourceModel()->db();
}
    
void
QXFontListView::showFontInfoPopover(const QModelIndex & index, const QRect & globalRect) const{
    if (popover_ == nullptr) {
        popover_ = new QXPopoverWindow(const_cast<QXFontListView*>(this));
        popoverWidget_ = new QTextBrowser(const_cast<QXFontListView*>(this));
        popoverWidget_->setFixedWidth(250);
        popoverWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        popoverWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        popoverWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        popoverWidget_->setStyleSheet("background-color: palette(window);border: none;");
        popover_->setWidget(popoverWidget_);
    }
    
    int row = proxyModel()->mapToSource(index).row();
    auto & desc = db()->faceDescriptor(row);
    auto & atts = db()->faceAttributes(row);
    
    QXHtmlTemplate * html = QXHtmlTemplate::createFromFile(QXResources::path("Html/FontInfoTemplate.html"));
    popoverWidget_->setHtml(html->instantialize(templateValues(desc, atts)));
    html->deleteLater();
    
    qreal docHeight = popoverWidget_->document()->documentLayout()->documentSize().height();
    if (docHeight)
        popoverWidget_->setFixedHeight(docHeight + 2);
    
    popover_->showRelativeTo(globalRect, QXPopoverAnyEdge);
}

bool
QXFontListView::isFontInfoPopoverVisible() const {
    return popover_ && popover_->isVisible();
}

QXSortFilterFontListModel *
QXFontListView::proxyModel() const {
    return qobject_cast<QXSortFilterFontListModel *>(this->model());
}

QXFontListModel *
QXFontListView::sourceModel() const {
    return qobject_cast<QXFontListModel *>(proxyModel()->sourceModel());
}

QModelIndex
QXFontListView::currentProxyIndex() const {
    return currentIndex();
}

QModelIndex
QXFontListView::currentSourceIndex() const {
    return proxyModel()->mapToSource(currentProxyIndex());
}

void
QXFontListView::scrollToCurrentIndex() {
    scrollTo(currentIndex(), QAbstractItemView::PositionAtTop);
}

void
QXFontListView::onFontContextMenuRequested(const QPoint & pos) {
    auto index = currentSourceIndex();
    if (index.row() < 0)
        return;
    
    auto & desc = db()->faceDescriptor(index.row());
    
    QPoint globalPos = this->mapToGlobal(pos);
    
    QMenu menu;
    menu.addAction(tr("Copy File Path"), [desc](){
        qApp->copyTextToClipBoard(toQString(desc.filePath));
    });
    
    menu.addAction(
#if defined(Q_OS_WIN)
        tr("Show in Explorer"),
#elif defined(Q_OS_MACOS)
        tr("Reveal in Finder"),
#else
        tr("Show in Finder"),
#endif
        [this, desc]() {
            qApp->showInGraphicalShell(this, toQString(desc.filePath));
        });
    
    menu.exec(globalPos);
}

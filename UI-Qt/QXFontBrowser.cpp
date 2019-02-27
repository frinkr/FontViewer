#include <QPainter>
#include <QStyledItemDelegate>
#include "QXApplication.h"
#include "QXConv.h"
#include "QXFontBrowser.h"
#include "QXFontManager.h"
#include "QXFontComboBox.h"
#include "ui_QXFontBrowser.h"

namespace {
    class QXFontBrowserItemDelegate : public QStyledItemDelegate {
    public:
        QXFontBrowserItemDelegate(QWidget * parent = 0) : QStyledItemDelegate(parent) {}

        void
        paint(QPainter * painter,
              const QStyleOptionViewItem & option,
              const QModelIndex & proxyIndex) const override {
            painter->save();
            
            painter->setRenderHint(QPainter::HighQualityAntialiasing);
            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            const bool selected = (option.state & QStyle::State_Selected);
            if (selected)
                painter->fillRect(option.rect, option.palette.highlight());
            else
                painter->fillRect(option.rect, option.palette.base());
            
            const QXSortFilterFontListModel * proxyModel = qobject_cast<const QXSortFilterFontListModel*>(proxyIndex.model());
            const QXFontListModel * sourceModel = qobject_cast<const QXFontListModel *>(proxyModel->sourceModel());
            const QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);

            // Draw font type icon
            const int iconSize = painter->fontInfo().pixelSize() * 1.2;
            QVariant iconVariant = proxyModel->data(proxyIndex, Qt::DecorationRole);
            QIcon icon;
            if (iconVariant.canConvert<QIcon>())
                icon = iconVariant.value<QIcon>();
            if (!icon.isNull()) {
                QPixmap pixmap = icon.pixmap(painter->fontInfo().pixelSize() * 2);
                painter->drawPixmap(QRect(option.rect.left(), option.rect.top(), iconSize, iconSize), pixmap);
            }
            
            // Draw font name
            QVariant nameVariant = proxyModel->data(proxyIndex, Qt::DisplayRole);
            QString displayName;
            if (nameVariant.canConvert<QString>())
                displayName = nameVariant.value<QString>();
            if (!displayName.isEmpty())
                painter->drawText(option.rect.adjusted(iconSize + 2, 0, 0, 0), displayName);
            
            // Draw sample text
            const FXFaceDescriptor & desc = sourceModel->db()->faceDescriptor(sourceIndex.row());
            FXPtr<FXFace> face = FXFace::createFace(desc);
            if (face) {
                FXVector<FXChar> sample = {'a', 'b', 'f', 'g', 'x', 'y', 'r'};

                const qreal sampleHeight = option.rect.height() - iconSize;
                const qreal sampleFontSizePx = 0.9 * sampleHeight;
                const qreal sampleFontSizePt = px2pt(sampleFontSizePx);
                const qreal sampleFontScale = sampleFontSizePt / face->fontSize();

                QPointF pen(option.rect.left(), option.rect.top()
                            + iconSize
                            + (sampleHeight - sampleFontSizePx) / 2
                            + 0.75 * sampleFontSizePx);
                
                for (FXGChar c : sample) {
                    FXGlyph g = face->glyph(c);

                    FXVec2d<int> bmOffset;
                    const double bmScale = face->bmScale();
                    FXPixmapARGB bm = face->pixmap(g.gid, &bmOffset);
                    if (bm.empty())
                        continue;
                   
                    auto img = toQImage(bm);
                    qreal newWidth = sampleFontScale * img.width();
                    img = img.scaledToWidth(newWidth, Qt::SmoothTransformation);
                    if ((face->isScalable() && selected) || qApp->darkMode())
                        img.invertPixels();

                    const qreal left   = pen.x() + bmOffset.x * sampleFontScale;
                    const qreal bottom = pen.y() - bmOffset.y * sampleFontScale;
                    const qreal right  = left + bm.width * bmScale * sampleFontScale;
                    const qreal top    = bottom - bm.height * bmScale * sampleFontScale;

                    painter->drawImage(QRect(QPoint(left, top), QPoint(right, bottom)),
                                       img);
                                        
                    qreal advPx = 1.0 * img.width() / g.metrics.width * g.metrics.horiAdvance;
                    pen += QPoint(advPx, 0);
                }
                
            }
            painter->restore();
        }

        QSize
        sizeHint(const QStyleOptionViewItem & option,
                 const QModelIndex & index) const override {
            return QSize(10, 100);
        }
    };
}

QXFontBrowser::QXFontBrowser(QWidget * parent)
    : QXThemedWindow<QDialog>(parent)
    , ui_(new Ui::QXFontBrowser) {
    ui_->setupUi(this);

    QSortFilterProxyModel * proxy = new QXSortFilterFontListModel(this);
    proxy->setSourceModel(new QXFontListModel(this));
    ui_->fontListView->setModel(proxy);
    ui_->fontListView->setItemDelegate(new QXFontBrowserItemDelegate(this));
    proxy->sort(0);

    connect(ui_->fontListView, &QListView::doubleClicked,
            this, &QXFontBrowser::onFontDoubleClicked);
}

QXFontBrowser::~QXFontBrowser() {
    delete ui_;
}

int
QXFontBrowser::selectedFontIndex() const {
    return currentSourceIndex().row();
}

QXFontURI
QXFontBrowser::selectedFont() const {
    int row = selectedFontIndex();
    if (row == -1)
        return QXFontURI{};
    
    auto desc = QXFontManager::instance().db()->faceDescriptor(row);
    auto atts = QXFontManager::instance().db()->faceAttributes(row);
    atts.names.familyName();
    QXFontURI uri{toQString(desc.filePath), desc.index};
    return uri;
}

QXSortFilterFontListModel *
QXFontBrowser::proxyModel() const {
    return qobject_cast<QXSortFilterFontListModel *>(ui_->fontListView->model());
}

QXFontListModel *
QXFontBrowser::sourceModel() const {
    return qobject_cast<QXFontListModel *>(proxyModel()->sourceModel());
}

void
QXFontBrowser::onFontDoubleClicked(const QModelIndex & index) {
    accept();
    //const QModelIndex sourceIndex = proxyModel()->mapToSource(proxyIndex);
    //    auto db = 
}

QModelIndex
QXFontBrowser::currentProxyIndex() const {
    return ui_->fontListView->currentIndex();
    
}

QModelIndex
QXFontBrowser::currentSourceIndex() const {
    return proxyModel()->mapToSource(currentProxyIndex());
}

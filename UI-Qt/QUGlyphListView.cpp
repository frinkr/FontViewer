#include <QApplication>
#include <QIcon>
#include <QPainter>

#include "QUConv.h"
#include "QUGlyphListView.h"
#include "QUDocument.h"
#include "QUEncoding.h"

constexpr int QUGlyphRole= Qt::UserRole + 1;

void
QUGlyphItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QUGlyphListModel * model = (QUGlyphListModel*)index.model();

    QVariant v = index.data(QUGlyphRole);
    if (!v.canConvert<QUGlyph>())
        return QStyledItemDelegate::paint(painter, option, index);

    const FXGlyph g = qvariant_cast<QUGlyph>(v).g();
    
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    painter->save();

    
    const QWidget * widget = opt.widget;
    QStyle * style = widget ? widget->style() : QApplication::style();
    
    QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    
    QSize emSize = glyphEmSize();
    QRect emRect(0, 0, emSize.width(), emSize.height());
        
    // draw the background
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

    // draw the icon
    if (true) {
        QIcon::Mode mode = QIcon::Normal;
        QImage image;
        if (g.id || !model->charMode() || !model->currentCMap().isUnicode())
            image = placeImage(toQImage(g.bitmap), emSize);
        else
            image = charImage(g.character, emSize);
    
        QIcon icon(QPixmap::fromImage(image));
        if (opt.state & QStyle::State_Selected)
            mode = QIcon::Selected;
        icon.paint(painter, iconRect, opt.decorationAlignment, mode, QIcon::On);
    }

    // draw the text
    if (true) {
        QString charCode = QUEncoding::charHexNotation(g.character, model->currentCMap().isUnicode());

        QString text;
        switch (model->glyphLabel()) {
            case QUGlyphLabel::CharacterCode:
                text = charCode;
                break;
            case QUGlyphLabel::GlyphID:
                text = QString("%1").arg(g.id);
                break;
            case QUGlyphLabel::GlyphName:
                text = toQString(g.name);
                if (!g.id && model->charMode())
                    text = charCode;
                break;

        }

        if (!text.isEmpty()) {
            if (opt.state & QStyle::State_Selected)
                painter->setPen(opt.palette.color(QPalette::Normal, QPalette::HighlightedText));
            else
                painter->setPen(opt.palette.color(QPalette::Normal, QPalette::Text));
            style->drawItemText(painter, textRect, Qt::AlignHCenter, opt.palette, true, text);
        }
    }
    
    painter->restore();
}

QUGlyphListModel::QUGlyphListModel(FXPtr<FXFace> face, QObject * parent)
    : QAbstractListModel(parent)
    , face_(face)
    , blockIndex_(0)
    , dummyImage_(glyphEmSize(), QImage::Format_ARGB32)
    , charMode_(true)
    , glyphLabel_(QUGlyphLabel::GlyphName) {
    dummyImage_.fill(Qt::black);
    dummyImage_.setDevicePixelRatio(2);
}

FXCMap
QUGlyphListModel::currentCMap() const {
    return face_->currentCMap();
}
    
FXPtr<FXCharBlock>
QUGlyphListModel::currentBlock() const {
    return currentCMap().blocks()[blockIndex_];
}

int
QUGlyphListModel::rowCount(const QModelIndex & index) const {
    if (charMode_)
        return int(currentBlock()->size());
    else
        return int(face_->attributes().glyphCount);
}
    
QVariant
QUGlyphListModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= face_->glyphCount())
        return QVariant();

    if (role == QUGlyphRole) {
        FXGlyph g;
        if (charMode_)
            g = face_->glyph(currentBlock()->get(index.row()), false);
        else
            g = face_->glyph(index.row(), true);
        QVariant v;
        v.setValue(QUGlyph(g));
        return v;
    }
    else if (role == Qt::DisplayRole) 
        return QString("Dummy");
    else if (role == Qt::DecorationRole)
        return dummyImage_;
    else
        return QVariant();
}

void
QUGlyphListModel::selectBlock(size_t index) {
    beginResetModel();
    blockIndex_ = index;
    endResetModel();
}

QSize
QUGlyphListModel::iconSize() const {
    return QSize(100, 100);
}

bool
QUGlyphListModel::charMode() const {
    return charMode_;
}

void
QUGlyphListModel::setCharMode(bool state) {
    beginResetModel();
    charMode_ = state;
    endResetModel();
}

QUGlyphLabel
QUGlyphListModel::glyphLabel() const {
    return glyphLabel_;
}

void
QUGlyphListModel::setGlyphLabel(QUGlyphLabel label) {
    beginResetModel();
    glyphLabel_ = label;
    endResetModel();
}

QUGlyphListView::QUGlyphListView(QWidget * parent)
    : QListView(parent){
    setLayoutMode(QListView::Batched);
    setBatchSize(100);
    setUniformItemSizes(true);
    setMovement(QListView::Static);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

#include <QApplication>
#include <QIcon>
#include <QPainter>

#include "QUConv.h"
#include "QUGlyphListView.h"

void
QUGlyphItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    painter->save();
    painter->setClipRect(opt.rect);
    
    const QWidget * widget = opt.widget;
    QStyle * style = widget ? widget->style() : QApplication::style();
    
    QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    
    // draw the background
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);
    
    // draw the icon
    QIcon::Mode mode = QIcon::Normal;
    if (opt.state & QStyle::State_Selected)
        mode = QIcon::Selected;
    opt.icon.paint(painter, iconRect, opt.decorationAlignment, mode, QIcon::On);
    
    // draw the text
    if (!opt.text.isEmpty()) {
        if (opt.state & QStyle::State_Selected)
            painter->setPen(opt.palette.color(QPalette::Normal, QPalette::HighlightedText));
        else
            painter->setPen(opt.palette.color(QPalette::Normal, QPalette::Text));
        style->drawItemText(painter, textRect, Qt::AlignHCenter, opt.palette, true, opt.text);
    }
    
    painter->restore();
}

QUGlyphListModel::QUGlyphListModel(FXPtr<FXFace> face, QObject * parent)
    : QAbstractListModel(parent)
    , face_(face)
    , blockIndex_(0) {}

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
    return currentBlock()->size();
}
    
QVariant
QUGlyphListModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= face_->glyphCount())
        return QVariant();

    FXGlyph g = face_->glyph(currentBlock()->get(index.row()), false);
    
    if (role == Qt::DisplayRole) {
        if (g.name.empty())
            return QString("%1").arg(g.id);
        return toQString(g.name);
    }
    else if (role == Qt::DecorationRole) {
        return placeImage(toQImage(g.bitmap), glyphEmSize());
    }
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

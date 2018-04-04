#include <QApplication>
#include <QIcon>
#include <QPainter>

#include "QUConv.h"
#include "QUDocument.h"

void
QUGlyphItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QUDocument * document = (QUDocument*)index.model();
    
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


QUDocument *
QUDocument::openFromURI(const QUFontURI & uri, QObject * parent) {
    QUDocument * document = new QUDocument(uri, parent);
    if (!document->load()) {
        document->deleteLater();
        return nullptr;
    }
    return document;
}

QUDocument *
QUDocument::openFromFile(const QString & filePath, size_t faceIndex, QObject * parent) {
    QUFontURI uri{filePath, faceIndex};
    return openFromURI(uri, parent);
}

QUDocument::QUDocument(const QUFontURI & uri, QObject * parent)
    : QAbstractListModel(parent)
    , uri_(uri)
    , blockIndex_(0) {}

FXPtr<FXFace>
QUDocument::face() const {
    return face_;
}

QSize
QUDocument::iconSize() const {
    return QSize(100, 100);
}

bool
QUDocument::load() {
    face_ = FXFace::createFace(toStdString(uri_.filePath), uri_.faceIndex);
    return true;
}

FXPtr<FXCharBlock>
QUDocument::currentBlock() const {
    return face_->currentCMap().blocks()[blockIndex_];
}

int
QUDocument::rowCount(const QModelIndex & index) const {
    return currentBlock()->size();
}
    
QVariant
QUDocument::data(const QModelIndex & index, int role) const {
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
QUDocument::selectCMap(int index) {
    if (face_->selectCMap(index)) {
        selectBlock(0);
        emit cmapActivated(index);
    }
}

void
QUDocument::selectBlock(int index) {
    beginResetModel();
    blockIndex_ = index;
    endResetModel();
}

#include <QApplication>
#include <QIcon>
#include <QPainter>

#include "QUConv.h"
#include "QUGlyphListModel.h"
#include "QUDocument.h"
#include "QUEncoding.h"


QUGlyphListModel::QUGlyphListModel(FXPtr<FXFace> face, QObject * parent)
    : QAbstractListModel(parent)
    , face_(face)
    , fullGlyphsBlock_(new FXCharRangeBlock(0, FXChar(face->glyphCount()), FXGCharTypeGlyphID, "All Glyphs"))
    , blockIndex_(0)
    , dummyImage_(glyphEmSize(), QImage::Format_ARGB32)
    , charMode_(true)
    , glyphLabel_(QUGlyphLabel::GlyphName) {
    dummyImage_.fill(Qt::black);
    dummyImage_.setDevicePixelRatio(2);
}

const FXCMap &
QUGlyphListModel::currentCMap() const {
    return face_->currentCMap();
}
    
FXPtr<FXGCharBlock>
QUGlyphListModel::currentBlock() const {
    if (charMode_)
        return currentCMap().blocks()[blockIndex_];
    else
        return fullGlyphsBlock_;
}

size_t
QUGlyphListModel::currentBlockIndex() const {
    if (charMode_)
        return blockIndex_;
    else
        return 0;
}
    
int
QUGlyphListModel::rowCount(const QModelIndex & index) const {
    return int(currentBlock()->size());
}
    
QVariant
QUGlyphListModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role == QUGlyphRole) {
        FXGlyph g = face_->glyph(currentBlock()->get(index.row()));
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
    blockIndex_ = charMode_? index : 0;
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

FXGChar
QUGlyphListModel::charAt(const QModelIndex & index) const {
    return currentBlock()->get(index.row());
}

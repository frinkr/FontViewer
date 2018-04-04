#include <QIcon>

#include "QUConv.h"
#include "QUDocument.h"

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
    else if (role == Qt::DecorationRole)
        return placeImage(toQImage(g.bitmap), glyphEmSize());
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

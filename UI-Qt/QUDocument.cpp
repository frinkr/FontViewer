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
    , uri_(uri) {}

bool
QUDocument::load() {
    face_ = FXFace::createFace(toStdString(uri_.filePath), uri_.faceIndex);
    return true;
}

int
QUDocument::rowCount(const QModelIndex & index) const {
    return face_->glyphCount();
}
    
QVariant
QUDocument::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= face_->glyphCount())
        return QVariant();

    FXGlyph g = face_->glyph(index.row(), true);
    
    if (role == Qt::DisplayRole)
        return toQString(g.name);
    else if (role == Qt::DecorationRole)
        return placeImage(toQImage(g.bitmap), glyphEmSize());
    else
        return QVariant();

}


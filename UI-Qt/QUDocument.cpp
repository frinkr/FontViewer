#include "QUGlyphListView.h"
#include "QUConv.h"
#include "QUDocument.h"

///////////////////////////////////////////////////////////////////////////////////////////

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
    : QObject(parent)
    , uri_(uri)
{}

FXPtr<FXFace>
QUDocument::face() const {
    return face_;
}

QUGlyphListModel *
QUDocument::model() const {
    return model_.data();
}


QUGlyphItemDelegate *
QUDocument::delegate() const {
    return delegate_.data();
}

bool
QUDocument::load() {
    face_ = FXFace::createFace(toStdString(uri_.filePath), uri_.faceIndex);
    model_.reset(new QUGlyphListModel(face_, this));
    delegate_.reset(new QUGlyphItemDelegate(this));
    return true;
}

void
QUDocument::selectCMap(size_t index) {
    if (face_->selectCMap(index)) {
        model_->selectBlock(0);
        emit cmapActivated(index);
    }
}

void
QUDocument::selectBlock(size_t index) {
    model_->selectBlock(index);
}

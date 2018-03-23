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
    : QObject(parent)
    , uri_(uri) {}

bool
QUDocument::load() {
    return true;
}

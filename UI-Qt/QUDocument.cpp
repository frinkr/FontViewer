#include "QUConv.h"
#include "QUGlyphSearchEngine.h"
#include "QUDocument.h"

QUGlyph::QUGlyph(const FXGlyph & glyph, QObject * parent)
    : QObject(parent)
    , g_(glyph) {}

QUGlyph::QUGlyph(QObject * parent)
    : QObject(parent) {}

QUGlyph::QUGlyph(const QUGlyph & other)
    : g_(other.g()){
}

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

FXPtr<FXFace>
QUDocument::face() const {
    return face_;
}

void
QUDocument::selectCMap(size_t index) {
    if (face_->selectCMap(index)) {
        selectBlock(0);
        emit cmapActivated(int(index));
    }
}

void
QUDocument::selectBlock(size_t index) {
    beginResetModel();
    blockIndex_ = charMode_? index : 0;
    endResetModel();
}

void
QUDocument::search(const QString & text) {
    QUGlyphSearchEngine * s = new QUGlyphSearchEngine(this);
    QUGlyphSearchResult result = s->search(text);
    if (result.found)
        emit searchFound(result, text);
    else
        emit searchNotFound(text);
}

const FXCMap &
QUDocument::currentCMap() const {
    return face_->currentCMap();
}
    
FXPtr<FXGCharBlock>
QUDocument::currentBlock() const {
    if (charMode_)
        return currentCMap().blocks()[blockIndex_];
    else
        return fullGlyphsBlock_;
}

size_t
QUDocument::currentBlockIndex() const {
    if (charMode_)
        return blockIndex_;
    else
        return 0;
}

bool
QUDocument::charMode() const {
    return charMode_;
}

void
QUDocument::setCharMode(bool state) {
    beginResetModel();
    charMode_ = state;
    endResetModel();
}

QUGlyphLabel
QUDocument::glyphLabel() const {
    return glyphLabel_;
}

void
QUDocument::setGlyphLabel(QUGlyphLabel label) {
    beginResetModel();
    glyphLabel_ = label;
    endResetModel();
}

FXGChar
QUDocument::charAt(const QModelIndex & index) const {
    return currentBlock()->get(index.row());
}

int
QUDocument::rowCount(const QModelIndex & index) const {
    return int(currentBlock()->size());
}
    
QVariant
QUDocument::data(const QModelIndex & index, int role) const {
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

QUDocument::QUDocument(const QUFontURI & uri, QObject * parent)
    : QAbstractListModel(parent)
    , uri_(uri)
    , blockIndex_(0)
    , charMode_(true)
    , glyphLabel_(QUGlyphLabel::GlyphName)
{}

bool
QUDocument::load() {
    face_ = FXFace::createFace(toStdString(uri_.filePath), uri_.faceIndex);
    fullGlyphsBlock_.reset(new FXCharRangeBlock(0, FXChar(face_->glyphCount()), FXGCharTypeGlyphID, "All Glyphs"));
    dummyImage_ = QImage(glyphEmSize(), QImage::Format_ARGB32);
    dummyImage_.setDevicePixelRatio(2);
    return true;
}

    

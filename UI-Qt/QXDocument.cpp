#include <QFileInfo>
#include <QRegularExpression>

#include "QXConv.h"
#include "QXSearchEngine.h"
#include "QXDocument.h"

QString
QXFontURI::toString() const {
    return QString("%1@%2").arg(faceIndex).arg(filePath);
}

const QXFontURI
QXFontURI::fromString(const QString & string) {
    QXFontURI uri;

    QRegularExpression re(R"(^(?<index>\d*)@(?<path>.*$))");
    QRegularExpressionMatch match = re.match(string);
    if (match.hasMatch()) {
        uri.faceIndex = match.captured("index").toInt();
        uri.filePath = match.captured("path");
    }
    else {
        uri.faceIndex = -1;
        uri.filePath = string;
    }
    return uri;
}

QXGlyph::QXGlyph(const FXGlyph & glyph, QObject * parent)
    : QObject(parent)
    , g_(glyph) {}

QXGlyph::QXGlyph(QObject * parent)
    : QObject(parent) {}

QXGlyph::QXGlyph(const QXGlyph & other)
    : g_(other.g()){
}

///////////////////////////////////////////////////////////////////////////////////////////

QXDocument *
QXDocument::openFromURI(const QXFontURI & uri, QObject * parent) {
    QXDocument * document = new QXDocument(uri, parent);
    if (!document->load()) {
        document->deleteLater();
        return nullptr;
    }
    return document;
}

QXDocument *
QXDocument::openFromFile(const QString & filePath, size_t faceIndex, QObject * parent) {
    QXFontURI uri{filePath, faceIndex};
    return openFromURI(uri, parent);
}

QString
QXDocument::faceDisplayName(const FXPtr<FXFace> & face) {
    return faceDisplayName(face->attributes());
}

QString
QXDocument::faceDisplayName(const FXFaceAttributes & atts) {
    QString familyName = toQString(atts.names.familyName());
    QString styleName = toQString(atts.names.styleName());

    QString fullName;
    if (!familyName.isEmpty())
        fullName = QString("%1 - %2").arg(familyName, styleName);
    else
        fullName = QString("%1 - %2").arg(QFileInfo(toQString(atts.desc.filePath)).fileName(), atts.desc.index);
    return fullName;
}

FXPtr<FXFace>
QXDocument::face() const {
    return face_;
}

QString
QXDocument::displayName() const {
    return faceDisplayName(face_->attributes());
}

bool
QXDocument::selectCMap(size_t index) {
    if (face_->selectCMap(index)) {
        selectBlock(0);
        emit cmapActivated(int(index));
        return true;
    }
    return false;
}

void
QXDocument::selectBlock(size_t index) {
    if (blockIndex_ == index)
        return;
    
    beginResetModel();
    blockIndex_ = charMode_? index : 0;
    endResetModel();

    emit blockSelected(blockIndex_);
}

void
QXDocument::search(const QXSearch & s) {
    QUSearchEngine * se = new QUSearchEngine(this);
    QXSearchResult result = se->search(s);
    emit searchDone(result, "");
}

void
QXDocument::search(const QString & text) {
    QUSearchEngine * se = new QUSearchEngine(this);
    QXSearchResult result = se->search(text);
    emit searchDone(result, text);
}

const FXCMap &
QXDocument::currentCMap() const {
    return face_->currentCMap();
}
    
FXPtr<FXGCharBlock>
QXDocument::currentBlock() const {
    if (charMode_)
        return currentCMap().blocks()[blockIndex_];
    else
        return fullGlyphsBlock_;
}

size_t
QXDocument::currentBlockIndex() const {
    if (charMode_)
        return blockIndex_;
    else
        return 0;
}

bool
QXDocument::charMode() const {
    return charMode_;
}

void
QXDocument::setCharMode(bool state) {
    if (charMode_ == state)
        return;
    
    beginResetModel();
    charMode_ = state;
    endResetModel();

    emit charModeChanged(charMode_);
}

QXGlyphLabel
QXDocument::glyphLabel() const {
    return glyphLabel_;
}

void
QXDocument::setGlyphLabel(QXGlyphLabel label) {
    beginResetModel();
    glyphLabel_ = label;
    endResetModel();
}

FXGChar
QXDocument::charAt(const QModelIndex & index) const {
    return currentBlock()->get(index.row());
}

int
QXDocument::rowCount(const QModelIndex & index) const {
    return int(currentBlock()->size());
}
    
QVariant
QXDocument::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role == QXGlyphRole) {
        FXGlyph g = face_->glyph(currentBlock()->get(index.row()));
        QVariant v;
        v.setValue(QXGlyph(g));
        return v;
    }
    else if (role == Qt::DisplayRole) 
        return QString("Dummy");
    else if (role == Qt::DecorationRole)
        return dummyImage_;
    else
        return QVariant();
}

QXDocument::QXDocument(const QXFontURI & uri, QObject * parent)
    : QAbstractListModel(parent)
    , uri_(uri)
    , blockIndex_(0)
    , charMode_(true)
    , glyphLabel_(QXGlyphLabel::GlyphName) {

    connect(this, &QXDocument::variableCoordinatesChanged, [this]() {       
        dataChanged(index(0), index(rowCount(), 0));
    });
}

bool
QXDocument::load() {
    face_ = FXFace::createFace(toStdString(uri_.filePath), uri_.faceIndex);
    if (!face_)
        return false;

    fullGlyphsBlock_.reset(new FXCharRangeBlock(0, FXChar(face_->glyphCount()), FXGCharTypeGlyphID, "All Glyphs"));
    dummyImage_ = QImage(glyphEmSize(), QImage::Format_ARGB32);
    dummyImage_.setDevicePixelRatio(2);
    return true;
}

    

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

QXGCharBook::QXGCharBook(Scope scope, const QString & name)
    : scope_(scope),
      name_(name) {
}

const FXVector<FXPtr<FXGCharBlock>> &
QXGCharBook::blocks() const {
    return blocks_;
}

void
QXGCharBook::addBlock(FXPtr<FXGCharBlock> block) {
    blocks_.push_back(block);
}

QXGCharBook::Scope
QXGCharBook::scope() const {
    return scope_;
}

const QString &
QXGCharBook::name() const {
    return name_;
}

void
QXGCharBook::setName(const QString & name) {
    name_ = name;
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
        loadBooks();
        emit cmapActivated(int(index));
        initCurrentBook();
        return true;
    }
    return false;
}

void
QXDocument::selectBook(int index) {
    beginResetModel();
    bookIndex_ = index;
    endResetModel();

    emit bookSelected(bookIndex_);
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

const QXGCharBooks &
QXDocument::books() const {
    return books_;
}

const QXGCharBook &
QXDocument::currentBook() const {
    return books_[bookIndex_];
}

int
QXDocument::currentBookIndex() const {
    return bookIndex_;
}

bool
QXDocument::charMode() const {
    return charMode_;
}

void
QXDocument::setCharMode(bool state) {
    if (charMode_ == state)
        return;
    
    charMode_ = state;

    if (!charMode_) {
        prevBookIndex_ = bookIndex_;
        for (auto i = 0; i < books_.size(); ++i) {
            if (books_[i].scope() == QXGCharBook::GlyphList) {
                selectBook(i);
                break;
            }
        }
    }
    else {
        selectBook(prevBookIndex_);
    }

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
QXDocument::charAt(const QXCollectionModelIndex & index) const {
    return currentCMap().blocks()[index.section]->get(index.item);
}

int
QXDocument::sectionCount() const {
    return currentBook().blocks().size();
} 

int
QXDocument::itemCount(int section) const {
    return currentBook().blocks()[section]->size();
}

QVariant
QXDocument::data(const QXCollectionModelIndex & index, int role) const {
    if (role == QXGlyphRole) {
        auto & block = currentBook().blocks()[index.section];
        FXGlyph g = face_->glyph(block->get(index.item));
        QVariant v;
        v.setValue(QXGlyph(g));
        return v;
    }
    return QString("G %1,%2").arg(index.section).arg(index.item);
}

QVariant
QXDocument::data(int section) const {
    return toQString(currentBook().blocks()[section]->name());
}

QXDocument::QXDocument(const QXFontURI & uri, QObject * parent)
    : QXCollectionModel(parent)
    , uri_(uri)
    , charMode_(true)
    , glyphLabel_(QXGlyphLabel::GlyphName) {

    connect(this, &QXDocument::variableCoordinatesChanged, [this]() {
        beginResetModel();
        endResetModel();
    });
}

bool
QXDocument::load() {
    face_ = FXFace::createFace(toStdString(uri_.filePath), uri_.faceIndex);
    if (!face_)
        return false;

    return loadBooks() && initCurrentBook();
}

bool
QXDocument::loadBooks() {
    auto & cmap = currentCMap();
    books_.clear();

    // All glyphs blocks at 0
    QXGCharBook allGlyphBook(QXGCharBook::GlyphList, "Full Glyphs");
    allGlyphBook.addBlock(std::make_shared<FXCharRangeBlock>(0, FXChar(face_->glyphCount()), FXGCharTypeGlyphID, "All Glyphs"));
    books_.push_back(allGlyphBook);

    if (cmap.isUnicode()) {
        // Add Full Unicode blocks
        QXGCharBook fullUnicodeBook(QXGCharBook::FullUnicode, "Full Unicode");
        for (auto & block : cmap.unicodeBlocks())
            fullUnicodeBook.addBlock(block);
        books_.push_back(fullUnicodeBook);
    }

    // Add all blocks to single book
    if (cmap.blocks().size() > 1) {
        QXGCharBook allBlockBook(QXGCharBook::CMap, cmap.isUnicode() ? tr("Unicode Compact") : tr("All Blocks"));
        for (auto & block : cmap.blocks())
            allBlockBook.addBlock(block);
        books_.push_back(allBlockBook);
    }

    // Each block as a book
    for (auto & block : cmap.blocks()) {
        QXGCharBook book(QXGCharBook::Single, toQString(block->name()));
        book.addBlock(block);
        books_.push_back(book);
    }

    return true;
}

bool
QXDocument::initCurrentBook() {
    auto & cmap = currentCMap();

    // Init current book
    int bookIndex = 0;
    if(cmap.isUnicode())
        bookIndex = 2;    
    else if (!cmap.blocks().empty())
        bookIndex = 1;
    prevBookIndex_ = bookIndex;
    selectBook(bookIndex);

    return true;
}

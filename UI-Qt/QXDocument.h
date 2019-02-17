#pragma once
#include <QAbstractListModel>
#include <QImage>

#include "FontX/FXFace.h"
#include "QXCollectionModel.h"

struct QXSearchResult;
struct QXSearch;

struct QXFontURI
{
    QString filePath;
    size_t  faceIndex;

    QString
    toString() const;

    static const QXFontURI
    fromString(const QString & string);

    bool
    operator==(const QXFontURI & other) const {
        return filePath == other.filePath && faceIndex == other.faceIndex;
    }
    
    bool
    operator!=(const QXFontURI & other) const {
        return !(*this == other);
    }
};
Q_DECLARE_METATYPE(QXFontURI);

struct QXRecentFontItem : public QXFontURI {
    QString fullName;
};
Q_DECLARE_METATYPE(QXRecentFontItem);


class QXGlyph : public QObject {
    Q_OBJECT
public:
    QXGlyph(const FXGlyph & glyph, QObject * parent = nullptr);
    QXGlyph(QObject * parent = nullptr);
    QXGlyph(const QXGlyph & other);

    const FXGlyph &
    g() const {
        return g_;
    }

protected:
    FXGlyph  g_;
};

constexpr int QXGlyphRole= Qt::UserRole + 1;

enum class QXGlyphLabel {
    CharCode,
    GlyphID,
    GlyphName
};

Q_DECLARE_METATYPE(QXGlyph);

class QXGCharBook {
public:
    enum Type {
        One,       // The book contains only one block
        CMap,         // The book contains all blocks from current CMap
        GlyphList,    // The book contains the block of all glyphs in font
        FullUnicode,  // The book contains all blocks of Unicode
    };
public:
    explicit QXGCharBook(Type type, const QString & name = QString());

    const FXVector<FXPtr<FXGCharBlock>> &
    blocks() const;

    void
    addBlock(FXPtr<FXGCharBlock> block);

    Type
    type() const;

    const QString &
    name() const;

    void
    setName(const QString & name);

private:
    QString  name_;
    Type     type_;
    FXVector<FXPtr<FXGCharBlock>> blocks_;
};

using QXGCharBooks = FXVector<QXGCharBook>;

class QXDocument : public QXCollectionModel
{
    Q_OBJECT
public:
    /**
     * file loading
     */
    static QXDocument *
    openFromURI(const QXFontURI & uri, QObject * parent = nullptr);
    
    static QXDocument *
    openFromFile(const QString & filePath, size_t faceIndex, QObject * parent = nullptr);

    static QString
    faceDisplayName(const FXPtr<FXFace> & face);

    static QString
    faceDisplayName(const FXFaceAttributes & atts);

public:
    const QXFontURI &
    uri() const { return uri_; }

    FXPtr<FXFace>
    face() const;

    QString
    displayName() const;
public slots:
    bool
    selectCMap(size_t index);
    
    void
    selectBook(int index);

    void
    search(const QXSearch & s);
    
    void
    search(const QString & text);

public:    
    const FXCMap &
    currentCMap() const;

    const QXGCharBooks &
    books() const;

    const QXGCharBook &
    currentBook() const;

    int
    currentBookIndex() const;

    bool
    charMode() const;

    void
    setCharMode(bool state);
    
    QXGlyphLabel
    glyphLabel() const;

    void 
    setGlyphLabel(QXGlyphLabel label);

    FXGChar
    charAt(const QXCollectionModelIndex & index) const;

public:   // IMPL: QXCollectionViewModel
    int
    sectionCount() const override;

    int
    itemCount(int section) const override;

    QVariant
    data(const QXCollectionModelIndex & index, int role) const override;

    QVariant
    data(int section) const override;

signals:
    void
    cmapActivated(int index);

    void
    bookSelected(int index);

    void
    charModeChanged(bool charMode);
    
    void
    searchDone(const QXSearchResult & result, const QString & text);

    void
    variableCoordinatesChanged();

protected:
    QXDocument(const QXFontURI & uri, QObject * parent);

    bool
    load();

    bool
    loadBooks();

    bool
    initCurrentBook();
protected:
    QXFontURI        uri_;
    FXPtr<FXFace>    face_;

    QXGCharBooks     books_;
    size_t           bookIndex_;
    size_t           prevBookIndex_;

    bool             charMode_;
    QXGlyphLabel     glyphLabel_;
    
};

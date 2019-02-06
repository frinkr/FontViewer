#pragma once
#include <QAbstractListModel>
#include <QImage>

#include "FontX/FXFace.h"

struct QXSearchResult;
struct QXSearch;

struct QXFontURI
{
    QString filePath;
    size_t  faceIndex;

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

class QXDocument : public QAbstractListModel
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
    selectBlock(size_t index);

    void
    search(const QXSearch & s);
    
    void
    search(const QString & text);

public:    
    const FXCMap &
    currentCMap() const;

    FXPtr<FXGCharBlock>
    currentBlock() const;

    size_t
    currentBlockIndex() const;
    
    bool
    charMode() const;

    void
    setCharMode(bool state);
    
    QXGlyphLabel
    glyphLabel() const;

    void 
    setGlyphLabel(QXGlyphLabel label);

    FXGChar
    charAt(const QModelIndex & index) const;

    int
    rowCount(const QModelIndex & parent = QModelIndex()) const;
    
    QVariant
    data(const QModelIndex &, int) const;
    
signals:
    void
    cmapActivated(int index);

    void
    blockSelected(int index);

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

protected:
    QXFontURI        uri_;
    FXPtr<FXFace>    face_;

    FXPtr<FXGCharBlock> fullGlyphsBlock_;
    size_t           blockIndex_;
    QImage           dummyImage_;
    bool             charMode_;
    QXGlyphLabel     glyphLabel_;
    
};

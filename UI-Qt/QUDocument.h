#pragma once
#include <QAbstractListModel>
#include <QImage>

#include "FontX/FXFace.h"

struct QUSearchResult;
struct QUSearch;

struct QUFontURI
{
    QString filePath;
    size_t  faceIndex;

    bool
    operator==(const QUFontURI & other) const {
        return filePath == other.filePath &&
            faceIndex == other.faceIndex;
    }
    
    bool
    operator!=(const QUFontURI & other) const {
        return !(*this == other);
    }

    friend QDataStream &
    operator << (QDataStream & arch, const QUFontURI & uri)
    {
        arch << uri.filePath;
        arch << static_cast<int>(uri.faceIndex);
        return arch;
    }

    friend QDataStream &
    operator >> (QDataStream & arch, QUFontURI & uri)
    {
        int index = 0;
        arch >> uri.filePath;
        arch >> index;
        uri.faceIndex = static_cast<size_t>(index);
        return arch;
    }
};

Q_DECLARE_METATYPE(QUFontURI);

class QUGlyph : public QObject {
    Q_OBJECT
public:
    QUGlyph(const FXGlyph & glyph, QObject * parent = nullptr);
    QUGlyph(QObject * parent = nullptr);
    QUGlyph(const QUGlyph & other);

    const FXGlyph &
    g() const {
        return g_;
    }

protected:
    FXGlyph  g_;
};

constexpr int QUGlyphRole= Qt::UserRole + 1;

enum class QUGlyphLabel {
    CharCode,
    GlyphID,
    GlyphName
};

Q_DECLARE_METATYPE(QUGlyph);

class QUDocument : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * file loading
     */
    static QUDocument *
    openFromURI(const QUFontURI & uri, QObject * parent = nullptr);
    
    static QUDocument *
    openFromFile(const QString & filePath, size_t faceIndex, QObject * parent = nullptr);

    static QString
    faceGUIName(const FXFaceAttributes & atts);

public:
    const QUFontURI &
    uri() const { return uri_; }

    FXPtr<FXFace>
    face() const;

public slots:
    bool
    selectCMap(size_t index);
    
    void
    selectBlock(size_t index);

    void
    search(const QUSearch & s);
    
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
    
    QUGlyphLabel
    glyphLabel() const;

    void 
    setGlyphLabel(QUGlyphLabel label);

    FXGChar
    charAt(const QModelIndex & index) const;

    int
    rowCount(const QModelIndex &) const;
    
    QVariant
    data(const QModelIndex &, int) const;
    
signals:
    void
    cmapActivated(int index);

    void
    blockSelected(int index);

    void
    charModeActivated(bool state);
    
    void
    searchDone(const QUSearchResult & result, const QString & text);

protected:
    QUDocument(const QUFontURI & uri, QObject * parent);

    bool
    load();

protected:
    QUFontURI        uri_;
    FXPtr<FXFace>    face_;

    FXPtr<FXGCharBlock> fullGlyphsBlock_;
    size_t           blockIndex_;
    QImage           dummyImage_;
    bool             charMode_;
    QUGlyphLabel     glyphLabel_;
    
};

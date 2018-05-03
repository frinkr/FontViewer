#pragma once
#include "QUGlyphListModel.h"
    
struct QUGlyphSearchResult {
    bool    found    {false};
    bool    charMode {false};
    size_t  block    {0};  
    size_t  index    {0};  // index in block
};

/**
 * A search is either char/gid or name searching
 */
struct QUGlyphSearch {
    FXGChar      gchar;
    QString      name;
};

class QUGlyphSearchExpressionParser : public QObject {
    Q_OBJECT
public:
    explicit QUGlyphSearchExpressionParser(QObject * parent = nullptr);

    QUGlyphSearch
    parse(const QString & text);
};

class QUGlyphSearchEngine : public QObject {
    Q_OBJECT
public:
    QUGlyphSearchEngine(QUGlyphListModel * model, QObject * parent = nullptr);

    QUGlyphSearchResult
    search(const QString & expression) const;

    QUGlyphSearchResult
    search(const QUGlyphSearch & expression) const;

protected:
    QUGlyphSearchResult
    searchChar(FXGChar c) const;

    QUGlyphSearchResult
    searchGlyph(FXGlyphID g) const;

    QUGlyphSearchResult
    searchName(const QString & name) const;

protected:
    QUGlyphListModel * model_;
};

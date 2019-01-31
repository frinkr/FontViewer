#pragma once
#include "QXDocument.h"
    
struct QXSearchResult {
    bool    found    {false};
    bool    charMode {false};
    size_t  block    {0};  
    size_t  index    {0};  // index in block
};

/**
 * A search is either char/gid or name searching
 */
struct QXSearch {
    FXGChar      gchar;
    QString      name;
};

class QXSearchExpressionParser : public QObject {
    Q_OBJECT
public:
    explicit QXSearchExpressionParser(QObject * parent = nullptr);

    QXSearch
    parse(const QString & text);
};

class QUSearchEngine : public QObject {
    Q_OBJECT
public:
    QUSearchEngine(QXDocument * document);

    QXSearchResult
    search(const QString & expression) const;

    QXSearchResult
    search(const QXSearch & expression) const;

protected:
    QXSearchResult
    searchChar(FXGChar c) const;

    QXSearchResult
    searchGlyph(FXGlyphID g) const;

    QXSearchResult
    searchName(const QString & name) const;

protected:
    QXDocument * document_;
};

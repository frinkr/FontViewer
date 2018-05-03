#pragma once
#include "QUDocument.h"
    
struct QUSearchResult {
    bool    found    {false};
    bool    charMode {false};
    size_t  block    {0};  
    size_t  index    {0};  // index in block
};

/**
 * A search is either char/gid or name searching
 */
struct QUSearch {
    FXGChar      gchar;
    QString      name;
};

class QUSearchExpressionParser : public QObject {
    Q_OBJECT
public:
    explicit QUSearchExpressionParser(QObject * parent = nullptr);

    QUSearch
    parse(const QString & text);
};

class QUSearchEngine : public QObject {
    Q_OBJECT
public:
    QUSearchEngine(QUDocument * document);

    QUSearchResult
    search(const QString & expression) const;

    QUSearchResult
    search(const QUSearch & expression) const;

protected:
    QUSearchResult
    searchChar(FXGChar c) const;

    QUSearchResult
    searchGlyph(FXGlyphID g) const;

    QUSearchResult
    searchName(const QString & name) const;

protected:
    QUDocument * document_;
};

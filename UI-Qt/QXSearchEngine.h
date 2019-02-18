#pragma once
#include "QXDocument.h"
    
struct QXSearchResult {
    bool  found    {false};
    int   book     {0};  // which book
    int   block    {0};  // block in book
    int   index    {0};  // index in block
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

class QXSearchEngine : public QObject {
    Q_OBJECT
public:
    QXSearchEngine(QXDocument * document);

    QXSearchResult
    search(const QString & expression) const;

    QXSearchResult
    search(const QXSearch & expression) const;

    QXSearchResult
    search(const FXGChar & c) const;

protected:
    QXSearchResult
    searchName(const QString & name) const;

    QXSearchResult
    searchGlyphID(FXGlyphID g) const;

    QXSearchResult
    searchInBook(const FXGChar & c, int bookIndex) const;

protected:
    QXDocument * document_;
};

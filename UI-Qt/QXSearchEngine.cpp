#include "QXEncoding.h"
#include "QXSearchEngine.h"

QXSearchExpressionParser::QXSearchExpressionParser(QObject * parent)
    : QObject(parent) {}

QXSearch
QXSearchExpressionParser::parse(const QString & text) {
    const FXGChar gchar = QXEncoding::charFromHexNotation(text);

    QXSearch search;
    
    // check codepoint notation
    if (gchar.isValid() && gchar.isChar()) {
        search.gchar = gchar;
        return search;
    }
    bool ok = false;
    
    // check glyph id
    QString code = text;
    if (text.indexOf("\\g") == 0) {
        code.remove(0, 2);
        FXGlyphID gid = code.toUInt(&ok);
        if (ok) {
            search.gchar = FXGChar(gid, FXGCharTypeGlyphID);
            return search;
        }
    }

    // check single char
    code = text;
    const QVector<uint> ucs4 = code.toUcs4();
    if (ucs4.size() == 1) {
        search.gchar = FXGChar(ucs4[0], FXGCharTypeUnicode);
        return search;
    }

    // lastly glyph name
    search.name = text;
    return search;
}

QUSearchEngine::QUSearchEngine(QXDocument * document_)
    : QObject(document_)
    , document_(document_) {}

QXSearchResult
QUSearchEngine::search(const QString & expression) const {
    QXSearchExpressionParser parser;
    return search(parser.parse(expression));
}

QXSearchResult
QUSearchEngine::search(const QXSearch & expression) const {
    if (expression.gchar.isValid()) {
        if (expression.gchar.isChar()) 
            return searchChar(expression.gchar);
        else
            return searchGlyph(expression.gchar.value);
    }
    else 
        return searchName(expression.name);
}

QXSearchResult
QUSearchEngine::searchChar(FXGChar c) const {
    assert(!c.isGlyphID());
    
    QXSearchResult result;

    auto & books = document_->books();
    FXVector<size_t> indexes;
    const QXGCharBook::Scope bookOrder[] = { QXGCharBook::CMap, QXGCharBook::Single, QXGCharBook::GlyphList, QXGCharBook::FullUnicode };
    indexes.push_back(document_->currentBookIndex());
    for (auto scope : bookOrder) {
        for (size_t i = 0; i < books.size(); ++i) {
            if (i == indexes.front()) continue;
            if (books[i].scope() == scope)
                indexes.push_back(i);
        }
    }
    
    for (auto bookIndex: indexes) {
        auto & book = books[bookIndex];
        if (book.scope() != QXGCharBook::GlyphList) {
            auto & blocks = book.blocks();
            for (size_t blockIndex = 0; blockIndex < blocks.size(); ++ blockIndex) {
                auto & block = blocks[blockIndex];
                auto charIndex = block->index(c);
                if (charIndex != -1) {
                    result.found = true;
                    result.book  = bookIndex;
                    result.block = blockIndex;
                    result.index = charIndex;
                    result.charMode = true;
                }
            }
        }
    }

    // let's convert char to glyph and search in glyph mode
    if (!result.found) {
        FXGlyphID gid = document_->currentCMap().glyphForChar(c.value);
        if (gid)
            return searchGlyphID(gid);
    }
    return result;
}

QXSearchResult
QUSearchEngine::searchGlyph(FXGlyphID g) const {
    if (g < document_->face()->glyphCount()) {
        const auto & cm = document_->face()->currentCMap();
        const FXVector<FXChar> chs = cm.charsForGlyph(g);
        if (document_->charMode() && chs.size())
            return searchChar(FXGChar(chs[0], cm.isUnicode() ? FXGCharTypeUnicode : FXGCharTypeOther));
    }
    return searchGlyphID(g);
}

QXSearchResult
QUSearchEngine::searchGlyphID(FXGlyphID g) const {
    QXSearchResult result;

    FXGChar c(g, FXGCharTypeGlyphID);
    if (g < document_->face()->glyphCount()) {
        auto & books = document_->books();
        for (auto bookIndex = 0; bookIndex < books.size(); ++ bookIndex) {
            auto & book = books[bookIndex];
            if (book.scope() != QXGCharBook::GlyphList)
                continue;

            auto & blocks = book.blocks();
            for (size_t blockIndex = 0; blockIndex < blocks.size(); ++ blockIndex) {
                auto & block = blocks[blockIndex];
                auto charIndex = block->index(c);
                if (charIndex != -1) {
                    result.found = true;
                    result.book  = bookIndex;
                    result.block = blockIndex;
                    result.index = charIndex;
                    result.charMode = false;
                }
            }            
        }
    }
    return result;
}

QXSearchResult
QUSearchEngine::searchName(const QString & name) const {
    QXSearchResult result;
    return result;
}

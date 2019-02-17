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

QXSearchEngine::QXSearchEngine(QXDocument * document_)
    : QObject(document_)
    , document_(document_) {}

QXSearchResult
QXSearchEngine::search(const QString & expression) const {
    QXSearchExpressionParser parser;
    return search(parser.parse(expression));
}

QXSearchResult
QXSearchEngine::search(const QXSearch & expression) const {
    if (expression.gchar.isValid()) 
        return search(expression.gchar);
    else 
        return searchName(expression.name);
}

QXSearchResult
QXSearchEngine::search(const FXGChar & c) const {
    QXSearchResult result = searchInBook(c, document_->currentBookIndex());
    if (result.found) 
        return result;

    auto & books = document_->books();
    FXVector<QXGCharBook::Type> bookOrder{QXGCharBook::CMap, QXGCharBook::One, QXGCharBook::GlyphList};
    for (auto type : bookOrder) {
        for (size_t i = 0; i < books.size(); ++i) {
            if (i == document_->currentBookIndex())
                continue;

            if (books[i].type() == type) {
                result = searchInBook(c, i);
                if (result.found) 
                    return result;
            }
        }
    }
    
    return result;
}

QXSearchResult
QXSearchEngine::searchGlyphID(FXGlyphID g) const {
    QXSearchResult result;

    FXGChar c(g, FXGCharTypeGlyphID);
    if (g < document_->face()->glyphCount()) {
        auto & books = document_->books();
        for (auto bookIndex = 0; bookIndex < books.size(); ++ bookIndex) {
            auto & book = books[bookIndex];
            if (book.type() != QXGCharBook::GlyphList)
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
                }
            }
        }
    }
    return result;
}

QXSearchResult
QXSearchEngine::searchName(const QString & name) const {
    QXSearchResult result;
    return result;
}

QXSearchResult
QXSearchEngine::searchInBook(const FXGChar & c, int bookIndex) const {
    FXGChar c2;
    if (c.isChar())
        c2 = FXGChar(document_->currentCMap().glyphForChar(c.value), FXGCharTypeGlyphID);
    else {
        auto chars = document_->currentCMap().charsForGlyph(c.value);
        if (chars.size())
            c2 = FXGChar(chars[0], (document_->currentCMap().isUnicode()?FXGCharTypeUnicode: FXGCharTypeOther));
    }

    auto & book = document_->books()[bookIndex];
    auto & blocks = book.blocks();
    for (size_t i = 0; i < blocks.size(); ++i) {
        auto & block = blocks[i];
        auto index = block->index(c);
        if (index == -1 && c2.isValid())
            index = block->index(c2);

        if (index != -1) {
            QXSearchResult result;
            result.found = true;
            result.book  = bookIndex;
            result.block = i;
            result.index = index;
            return result;
        }
    }
    return QXSearchResult();
    
}

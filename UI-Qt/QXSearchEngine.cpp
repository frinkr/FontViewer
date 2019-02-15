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

    size_t blockIndex = -1;
    size_t charIndex = -1;
            
    auto currentBlock = document_->currentBlock();
    charIndex = currentBlock->index(c);
    if (charIndex != -1)
        blockIndex = document_->currentBlockIndex();

    if (blockIndex == -1) {
        const FXCMap & cmap = document_->currentCMap();
        auto blocks = cmap.blocks();
        for (size_t i = 0; i < blocks.size(); ++ i) {
            charIndex = blocks[i]->index(c);
            if (charIndex != -1) {
                blockIndex = i;
                break;
            }
        }
    }
    if (blockIndex != -1) {
        result.found    = true;
        result.charMode = true;
        result.block    = blockIndex;
        result.index    = charIndex;
    }
    else {
        // let's convert char to glyph and search in glyph mode
        FXGlyphID gid = document_->currentCMap().glyphForChar(c.value);
        if (gid)
            return searchGlyph(gid);
    }
    return result;
}

QXSearchResult
QUSearchEngine::searchGlyph(FXGlyphID g) const {
    QXSearchResult result;

    const auto & cm = document_->face()->currentCMap();
    const FXVector<FXChar> chs = cm.charsForGlyph(g);

    if (document_->charMode() && chs.size()) {
        return searchChar(FXGChar(chs[0], cm.isUnicode()?FXGCharTypeUnicode:FXGCharTypeOther));
    }
    else if (g < document_->face()->glyphCount()) {
        result.found    = true;
        result.charMode = false;
        result.index    = g;
    }
    return result;
}

QXSearchResult
QUSearchEngine::searchName(const QString & name) const {
    QXSearchResult result;
    return result;
}

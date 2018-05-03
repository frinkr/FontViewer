#include "QUEncoding.h"
#include "QUGlyphSearchEngine.h"

QUGlyphSearchExpressionParser::QUGlyphSearchExpressionParser(QObject * parent)
    : QObject(parent) {}

QUGlyphSearch
QUGlyphSearchExpressionParser::parse(const QString & text) {
    const FXGChar gchar = QUEncoding::charFromHexNotation(text);

    QUGlyphSearch search;
    
    // check codepoint notation
    if (gchar.isChar()) {
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
            search.gchar = FXGChar(FXGCharTypeGlyphID, gid);
            return search;
        }
    }

    // check single char
    code = text;
    const QVector<uint> ucs4 = code.toUcs4();
    if (ucs4.size() == 1) {
        search.gchar = FXGChar(FXGCharTypeUnicode, ucs4[0]);
        return search;
    }

    // lastly glyph name
    search.name = text;
    return search;
}

QUGlyphSearchEngine::QUGlyphSearchEngine(QUDocument * document_)
    : QObject(document_)
    , document_(document_) {}

QUGlyphSearchResult
QUGlyphSearchEngine::search(const QString & expression) const {
    QUGlyphSearchExpressionParser parser;
    return search(parser.parse(expression));
}

QUGlyphSearchResult
QUGlyphSearchEngine::search(const QUGlyphSearch & expression) const {
    if (expression.gchar.isValid()) {
        if (expression.gchar.isChar()) 
            return searchChar(expression.gchar);
        else
            return searchGlyph(expression.gchar.value);
    }
    else 
        return searchName(expression.name);
}

QUGlyphSearchResult
QUGlyphSearchEngine::searchChar(FXGChar c) const {
    QUGlyphSearchResult result;
    if (document_->charMode()) {
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
    }
    else {
        // let's convert char to glyph and search in glyph mode
        FXGlyphID gid = document_->currentCMap().glyphForChar(c.value);
        if (gid)
            return searchGlyph(gid);
    }
    return result;
}

QUGlyphSearchResult
QUGlyphSearchEngine::searchGlyph(FXGlyphID g) const {
    QUGlyphSearchResult result;
    if (g < document_->face()->glyphCount()) {
        result.found    = true;
        result.charMode = false;
        result.index    = g;
    }
    return result;
}

QUGlyphSearchResult
QUGlyphSearchEngine::searchName(const QString & name) const {
    QUGlyphSearchResult result;
    return result;
}

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

QUGlyphSearchEngine::QUGlyphSearchEngine(QUGlyphListModel * model, QObject * parent)
    : QObject(parent)
    , model_(model) {}

QUGlyphSearchResult
QUGlyphSearchEngine::search(const QString & expression) {
    QUGlyphSearchExpressionParser parser;
    return search(parser.parse(expression));
}

QUGlyphSearchResult
QUGlyphSearchEngine::search(const QUGlyphSearch & expression) {
    if (expression.gchar.isValid()) {
        if (expression.gchar.isChar() && model_->charMode()) {
            size_t blockIndex = -1;
            size_t charIndex = -1;
            
            auto currentBlock = model_->currentBlock();
            charIndex = currentBlock->index(expression.gchar);
            if (charIndex != -1)
                blockIndex = model_->currentBlockIndex();

            if (blockIndex == -1) {
                const FXCMap & cmap = model_->currentCMap();
                auto blocks = cmap.blocks();
                for (size_t i = 0; i < blocks.size(); ++ i) {
                    charIndex = blocks[i]->index(expression.gchar);
                    if (charIndex != -1) {
                        blockIndex = i;
                        break;
                    }
                }
            }
            if (blockIndex != -1) {
                QUGlyphSearchResult result;
                result.found    = true;
                result.charMode = true;
                result.block    = blockIndex;
                result.index    = charIndex;
            }
            
        }
    }
    else {
        // search by name
    }
    return QUGlyphSearchResult();
}

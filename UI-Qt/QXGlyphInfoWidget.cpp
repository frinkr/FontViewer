#include "FontX/FXUnicode.h"

#include "QXApplication.h"
#include "QXGlyphInfoWidget.h"
#include "QXConv.h"
#include "QXHtmlTemplate.h"
#include "QXEncoding.h"
#include "QXImageHelpers.h"
#include "QXResources.h"

#include <QImage>
#include <QTextDocument>
#include <QDebug>

namespace {

    QString toQString2(FXEastAsianWidth ea) {
        switch(ea) {
        default:
        case FXEastAsianWidth::neutral: return "neutral";
        case FXEastAsianWidth::ambiguous: return "ambiguous";
        case FXEastAsianWidth::halfWidth: return "half width";
        case FXEastAsianWidth::fullWidth: return "full width";
        case FXEastAsianWidth::narrow: return "narrow";
        case FXEastAsianWidth::wide: return "wide";            
        }
    }

    QString toQString3(FXBidiClass c) {
        static const char * map [] = {
            "L",
            "R",
            "EN",
            "ES",
            "ET",
            "AN",
            "CS",
            "B",
            "S",
            "WS",
            "ON",
            "LRE",
            "LRO",
            "AL",
            "RLE",
            "RLO",
            "PDF",
            "NSM",
            "BN",
            "FSI",
            "LRI",
            "RLI",
            "PDI",
        };
        return map[c];
    };
    
    QMap<QString, QVariant>
    templateValues(const FXGlyph & glyph) {
        const FXChar c = glyph.character.value;
        
        QMap<QString, QVariant> map;
        map["NAME"] = toQString(glyph.name);
        if (glyph.character.isUnicode()) 
            map["CHAR"] = QXEncoding::htmlLinkElement(QXEncoding::externalUnicodeHexLink(c), QXEncoding::charHexNotation(glyph.character));
        else
            map["CHAR"] = QXEncoding::charHexNotation(glyph.character);

        map["ID"] = glyph.gid;

        // Metrics
        map["WIDTH"] = glyph.metrics.width;
        map["HEIGHT"] = glyph.metrics.height;
        map["HORI_ADVANCE"] = glyph.metrics.horiAdvance;
        map["HORI_BEARING_X"] = glyph.metrics.horiBearingX;
        map["HORI_BEARING_Y"] = glyph.metrics.horiBearingY;
        map["VERT_ADVANCE"] = glyph.metrics.vertAdvance;
        map["VERT_BEARING_X"] = glyph.metrics.vertBearingX;
        map["VERT_BEARING_Y"] = glyph.metrics.vertBearingY;

        // Unicode
        bool isDefined = glyph.character.isUnicode() && FXUnicode::defined(c);
        map["UNICODE_NAME"] = isDefined? toQString(FXUnicode::name(c)): QString();
        map["UNICODE_BLOCK"] = isDefined? toQString(FXUnicode::block(c).name): QString();
        map["UNICODE_SCRIPT"] = isDefined? toQString(FXUnicode::script(c)) : QString();
        map["UNICODE_AGE"] = isDefined? toQString(FXUnicode::age(c)) : QString();
        map["UNICODE_GENERAL_CATEGORY"] = isDefined? toQString(FXUnicode::category(c).fullName): QString();
        map["UNICODE_EAST_ASIAN_WIDTH"] = isDefined? toQString2(FXUnicode::eastAsianWidth(c)): QString();

        auto decompose = [](FXChar c, bool nfkd) {
            QStringList decompositionLinks;
            for (FXChar d : FXUnicode::decomposition(c, nfkd)) {
                decompositionLinks << QXEncoding::htmlLinkElement(
                    QXEncoding::charHexLink({d, FXGCharTypeUnicode}).toDisplayString(),
                    QXEncoding::charHexNotation({d, FXGCharTypeUnicode}));
            }
            return decompositionLinks.join(", ");
        };
        
        map["UNICODE_DECOMPOSITION_NFD"] = isDefined? decompose(c, false): QString();
        map["UNICODE_DECOMPOSITION_NFKD"] = isDefined? decompose(c, true): QString();
        map["BIDI_CLASS"] = isDefined? toQString3(FXUnicode::bidiClass(c)): QString();
        
        // Encoding
        QStringList utf8;
        for (auto b : FXUnicode::utf8(c))
            utf8 << QString("%1").arg(b, 2, 16, QChar('0')).toUpper();
        
        QStringList utf16;
        for (auto s : FXUnicode::utf16(c))
            utf16 << QString("%1").arg(s, 4, 16, QChar('0')).toUpper();

        map["UTF8"] = isDefined? utf8.join(" "): QString();
        map["UTF16"] = isDefined? utf16.join(" "): QString();
        return map;
    };
}

QXGlyphInfoWidget::QXGlyphInfoWidget(QWidget *parent)
    : QTextBrowser(parent)
    , document_(nullptr)
    , char_(FXGCharInvalid) {
    connect(this, &QTextBrowser::anchorClicked,
            this, &QXGlyphInfoWidget::onLinkClicked);
    setOpenLinks(true);
    setOpenExternalLinks(true);
}

QXDocument *
QXGlyphInfoWidget::document() const {
    return document_;
}

void
QXGlyphInfoWidget::setDocument(QXDocument * document) {
    document_ = document;
}

void
QXGlyphInfoWidget::setChar(FXGChar c) {
    char_ = c;
    loadGlyph();
}

void
QXGlyphInfoWidget::loadGlyph() {
    
    FXGlyph glyph = document_->face()->glyph(char_);
    
    FXGlyphImage bm = fillGlyphImageWithColor(glyph.glyphImage(), palette().color(QPalette::Text));
    QImage image = drawGlyphImageInEmBox(bm);
    
    QTextDocument * qdoc = new QTextDocument;
    qdoc->addResource(QTextDocument::ImageResource,
                      QUrl("fv://glyph.png"), QVariant(image));
    
    QTextBrowser::setDocument(qdoc);
    
    QXHtmlTemplate * html = QXHtmlTemplate::createFromFile(QXResources::path("Html/GlyphInfoTemplate.html"));
    setHtml(html->instantialize(templateValues(glyph)));
    html->deleteLater();
}
    
void
QXGlyphInfoWidget::onLinkClicked(const QUrl & link) {
    FXGChar c = QXEncoding::charFromLink(link);
    if (c != FXGCharInvalid)
        emit charLinkClicked(c);
}

#include "FontX/FXUnicode.h"

#include "QUGlyphInfoWidget.h"
#include "QUConv.h"
#include "QUHtmlTemplate.h"
#include "QUEncoding.h"
#include "QUResource.h"

#include <QImage>
#include <QTextDocument>
#include <QDebug>

namespace {
    QMap<QString, QVariant>
    templateValues(const FXGlyph & glyph) {
        QMap<QString, QVariant> map;
        map["NAME"] = toQString(glyph.name);
        map["CHAR"] = QUEncoding::charHexNotation(glyph.character, true);
        map["ID"] = glyph.id;

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
        FXChar c = glyph.character;
        bool isDefined = FXUnicode::defined(c);
        map["UNICODE_NAME"] = isDefined? toQString(FXUnicode::name(c)): QString();
        map["UNICODE_BLOCK"] = isDefined? toQString(FXUnicode::block(c).name): QString();
        map["UNICODE_SCRIPT"] = isDefined? toQString(FXUnicode::script(c)) : QString();
        map["UNICODE_AGE"] = isDefined? toQString(FXUnicode::age(c)) : QString();
        map["UNICODE_GENERAL_CATEGORY"] = toQString(FXUnicode::category(c).fullName);

        QStringList decompositionLinks;
        for (FXChar d : FXUnicode::decomposition(c)) {
            decompositionLinks << QString("<a href=\"%1\">%2</a>")
                .arg(QUEncoding::charHexLink(d).toDisplayString())
                .arg(QUEncoding::charHexNotation(d));
        }
        
        map["UNICODE_DECOMPOSITION"] = decompositionLinks.join(", ");

        // Encoding
        QStringList utf8;
        for (auto b : FXUnicode::utf8(c))
            utf8 << QString("%1").arg(b, 2, 16, QChar('0')).toUpper();
        
        QStringList utf16;
        for (auto s : FXUnicode::utf16(c))
            utf16 << QString("%1").arg(s, 4, 16, QChar('0')).toUpper();

        map["UTF8"] = utf8.join(" ");
        map["UTF16"] = utf16.join(" ");
        return map;
    };
}

QUGlyphInfoWidget::QUGlyphInfoWidget(QWidget *parent)
    : QTextBrowser(parent)
    , document_(nullptr)
    , gid_(0)
    , char_(0) {

    connect(this, &QTextBrowser::anchorClicked,
            this, &QUGlyphInfoWidget::onLinkClicked);
    setOpenLinks(false);
}

QUDocument *
QUGlyphInfoWidget::document() const {
    return document_;
}

void
QUGlyphInfoWidget::setQUDocument(QUDocument * document) {
    document_ = document;
}

void
QUGlyphInfoWidget::setGlyph(FXGlyphID gid) {
    gid_ = gid;
    char_ = FXCharInvalid;
    
    loadGlyph();
}

void
QUGlyphInfoWidget::setChar(FXChar c) {
    char_ = c;
    gid_ = NotDef;
    loadGlyph();
}

FXGlyphID
QUGlyphInfoWidget::glyph() const {
    return gid_;
}

void
QUGlyphInfoWidget::loadGlyph() {
    
    FXGlyph glyph;
    if (char_ != FXCharInvalid)
        glyph = document_->face()->glyph(char_, false);
    else
        glyph = document_->face()->glyph(gid_, true);
    
    QTextDocument * qdoc = new QTextDocument;
    qdoc->addResource(QTextDocument::ImageResource,
                      QUrl("fv://glyph.png"), QVariant(placeImage(toQImage(glyph.bitmap), glyphEmSize())));
    
    setDocument(qdoc);
    
    QUHtmlTemplate html(QUResource::path("/Html/GlyphInfoTemplate.html"));
    setHtml(html.instantialize(templateValues(glyph)));
}
    
void
QUGlyphInfoWidget::onLinkClicked(const QUrl & link) {
    FXChar c = QUEncoding::charFromLink(link);
    if (c != FXCharInvalid)
        emit charLinkClicked(c);
}

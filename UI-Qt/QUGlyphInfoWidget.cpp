#include "QUGlyphInfoWidget.h"
#include "QUConv.h"
#include "QUTemplateInstantializer.h"
#include "QUEncoding.h"
#include "FontX/FXUnicode.h"
#include <QImage>
#include <QTextDocument>
#include <QFile>
#include <QTextStream>

namespace {
    QString loadTemplate() {
        QString file = FX_RESOURCES_DIR "/Html/GlyphInfoTemplate.html";
        QFile f(file);
        if (!f.open(QFile::ReadOnly | QFile::Text))
            return QString();
        QTextStream in(&f);
        return in.readAll();
    }

    QString instTemplate(const QString & temp, const FXGlyph & glyph) {
        QMap<QString, QVariant> map;
        map["NAME"] = toQString(glyph.name);
        map["CHAR"] = QUEncoding::charHexNotation(glyph.character, true);
        map["ID"] = glyph.id;

        // metrics
        map["WIDTH"] = glyph.metrics.width;
        map["HEIGHT"] = glyph.metrics.height;
        map["HORI_ADVANCE"] = glyph.metrics.horiAdvance;
        map["HORI_BEARING_X"] = glyph.metrics.horiBearingX;
        map["HORI_BEARING_Y"] = glyph.metrics.horiBearingY;
        map["VERT_ADVANCE"] = glyph.metrics.vertAdvance;
        map["VERT_BEARING_X"] = glyph.metrics.vertBearingX;
        map["VERT_BEARING_Y"] = glyph.metrics.vertBearingY;

        // unicode
        map["UNICODE_NAME"] = toQString(FXUnicode::name(glyph.character));
        map["UNICODE_BLOCK"] = toQString(FXUnicode::block(glyph.character).name);
        map["UNICODE_SCRIPT"] = toQString(FXUnicode::script(glyph.character));
        
        return instantializeTemplate(temp, map);
    };
}

QUGlyphInfoWidget::QUGlyphInfoWidget(QWidget *parent)
    : QTextBrowser(parent)
    , document_(nullptr)
    , gid_(0)
    , char_(0) {}

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
    setHtml(instTemplate(loadTemplate(), glyph));
}
    

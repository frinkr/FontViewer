#pragma once
#include <QTextBrowser>
#include "QUDocument.h"

class QUGlyphInfoWidget : public QTextBrowser {
public:
    explicit QUGlyphInfoWidget(QWidget *parent = nullptr);

    QUDocument *
    document() const;

    void
    setQUDocument(QUDocument * document);

    void
    setGlyph(FXGlyphID gid);

    void
    setChar(FXChar c);
    
    FXGlyphID
    glyph() const;
    
protected:
    void
    loadGlyph();
    
protected:
    QUDocument *   document_;
    FXGlyphID      gid_;
    FXChar         char_;
};


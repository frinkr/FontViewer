#pragma once
#include <QTextBrowser>
#include "QUDocument.h"

class QUGlyphInfoWidget : public QTextBrowser {
    Q_OBJECT
    
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

signals:
    void
    charLinkClicked(FXChar c);
    
protected:
    void
    loadGlyph();

protected slots:
    void
    onLinkClicked(const QUrl & link);
    
protected:
    QUDocument *   document_;
    FXGlyphID      gid_;
    FXChar         char_;
};


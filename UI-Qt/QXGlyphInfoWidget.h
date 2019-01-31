#pragma once
#include <QTextBrowser>
#include "QXDocument.h"

class QXGlyphInfoWidget : public QTextBrowser {
    Q_OBJECT
    
public:
    explicit QXGlyphInfoWidget(QWidget *parent = nullptr);

    QXDocument *
    document() const;

    void
    setQUDocument(QXDocument * document);

    void
    setChar(FXGChar c);
    
signals:
    void
    charLinkClicked(FXGChar c);
    
protected:
    void
    loadGlyph();

protected slots:
    void
    onLinkClicked(const QUrl & link);
    
protected:
    QXDocument *   document_;
    FXGChar        char_;
};


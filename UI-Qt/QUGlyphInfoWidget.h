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
    QUDocument *   document_;
    FXGChar        char_;
};


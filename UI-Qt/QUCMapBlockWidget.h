#pragma once

#include "QUPopoverWindow.h"

namespace Ui {
    class QUCMapBlockWidget;
}

class QUDocument;

class QUCMapBlockWidget : public QWidget {
    Q_OBJECT

public:
    explicit QUCMapBlockWidget(QWidget *parent = 0);
    ~QUCMapBlockWidget();

    QUDocument *
    document() const;

    void
    setDocument(QUDocument * document);

private slots:
    void
    reloadCMaps();

    void
    reloadBlocks();

    void
    onGlyphMode(bool state);

    void
    onCharMode(bool state);
    
private:
    Ui::QUCMapBlockWidget * ui_;
    QUDocument * document_;
};



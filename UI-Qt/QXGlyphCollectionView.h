#pragma once

#include "QXCollectionView.h"
#include "QXDocument.h"

class QXGlyphCollectionView: public QXCollectionView {
  Q_OBJECT
public:
    explicit QXGlyphCollectionView(QWidget * parent = nullptr);

    QXDocument *
    document() const;

    void
    setDocument(QXDocument * document);

private:
    QXDocument * document_;
};

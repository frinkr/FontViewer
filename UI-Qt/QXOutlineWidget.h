#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "FontX/FXGlyph.h"
class QXOutlineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QXOutlineWidget(QWidget *parent = 0);
    ~QXOutlineWidget();

    void
    paintEvent(QPaintEvent * event) override;

    QSize
    sizeHint() const override;

    QSize
    minimumSizeHint() const override;
    
    void
    setOutline(const FXGlyphOutline & outline);

private:
    void
        buildScene();

private:
    FXGlyphOutline outline_ {};

    QGraphicsView * view_{};
    QGraphicsScene* scene_{};
};



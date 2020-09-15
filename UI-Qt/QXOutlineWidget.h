#pragma once

#include <QWidget>
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
    FXGlyphOutline outline_ {};
};



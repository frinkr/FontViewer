#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainterPath>

#include "FontX/FXGlyph.h"
class QXOutlineWidget : public QWidget
{
    Q_OBJECT
    
public:
    enum Component {
        kContours       = 1,
        kPoints         = 2,
        kSketch         = 4,
        kAxises         = 8,

    };

    Q_DECLARE_FLAGS(Components, Component);

public:
    explicit QXOutlineWidget(QWidget *parent = 0);
    ~QXOutlineWidget();

    QSize
    sizeHint() const override;

    QSize
    minimumSizeHint() const override;
    
    void
    setOutline(const FXGlyphOutline & outline);

    Components
    components() const;

    void
    setComponents(Components components);

private:
    void
    buildScene();

    QPainterPath
    contourToPainterPath(const FXGlyphOutline::Contour & contour,
                         const QSizeF & emSize,
                         const QPointF & origin) const;

    QPointF
    transformPoint(const FXVec2d<int> & p, const QSizeF & emSize, const QPointF & origin) const;
    
private:
    FXGlyphOutline      outline_ {};
    QGraphicsView     * view_ {};
    QGraphicsScene    * scene_ {};

    QPen axisPen_{};
    QPen contourPen_ {};
    QPen sketchPen_ {};
    QPen onPointPen_ {};
    QPen offPointPen_ {};

    Components components_{};
};


Q_DECLARE_OPERATORS_FOR_FLAGS(QXOutlineWidget::Components);

#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include "QXOutlineWidget.h"

QXOutlineWidget::QXOutlineWidget(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);
    scene_ = new QGraphicsScene(this);
    view_ = new QGraphicsView(scene_);
    view_->setDragMode(QGraphicsView::ScrollHandDrag);
    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setRenderHint(QPainter::Antialiasing);

    layout->setMargin(0);
    layout->addWidget(view_);

    axisPen_ = QPen(Qt::gray);
    contourPen_ = QPen(Qt::yellow);
    sketchPen_ = QPen(Qt::red);
    onPointPen_ = QPen(Qt::red);
    offPointPen_ = QPen(Qt::green);

    components_ = kContours | kPoints | kSketch | kAxises;
}

QXOutlineWidget::~QXOutlineWidget() {
}

QSize
QXOutlineWidget::sizeHint() const {
    return minimumSizeHint();
}

QSize
QXOutlineWidget::minimumSizeHint() const {
    return {500, 500};
}

void
QXOutlineWidget::setOutline(const FXGlyphOutline & outline) {
    outline_ = outline;
    buildScene();
    update();
}

QXOutlineWidget::Components
QXOutlineWidget::components() const {
    return components_;
}

void
QXOutlineWidget::setComponents(Components components) {
    components_ = components;
    buildScene();
    update();
}


void 
QXOutlineWidget::buildScene() {
    //scene_ = new QGraphicsScene(this);
    scene_->clear();


    const QPointF origin(0, 0);
    const QSizeF emSize(1000, 1000);

    scene_->setSceneRect(QRectF(-emSize.width() * 9.5, -emSize.height() * 9.5, emSize.width() * 20, emSize.height() * 20));

    
    // Axises
    if (components_ & kAxises) {
        scene_->addLine(origin.x(), origin.y(),
            origin.x(), origin.y() - emSize.height(),
            axisPen_);

        scene_->addLine(origin.x(), origin.y(),
            origin.x() + emSize.width(), origin.y(),
            axisPen_);
    }

    auto xf = [origin, emSize, this](const FXVec2d<int>& p) {
        return transformPoint(p, emSize, origin);
    };


    // Connected lines of on-path points
    if (components_ & kSketch) {
        for (auto& cnt : outline_.contours) {
            FXOpt<size_t> lastOnIndex;
            FXOpt<size_t> firstOnIndex;
            for (size_t i = 0; i < cnt.points.size(); ++i) {
                auto& p = cnt.points[i];
                if (p.on && lastOnIndex)
                    scene_->addLine(QLineF(xf(p.pos), xf(cnt.points[*lastOnIndex].pos)), sketchPen_);

                if (p.on && !firstOnIndex)
                    firstOnIndex = i;
                if (p.on)
                    lastOnIndex = i;

            }
            if (lastOnIndex && firstOnIndex) {
                auto p0 = xf(cnt.points[*lastOnIndex].pos);
                auto p1 = xf(cnt.points[*firstOnIndex].pos);
                if (p0 != p1) {
                    QPen dotPen(sketchPen_);
                    dotPen.setStyle(Qt::DotLine);
                    scene_->addLine(QLineF(p0, p1), dotPen);
                }
            }
        }
    }


    // Bezier
    if (components_ & kContours) {
        for (auto& cnt : outline_.contours) {
            QPainterPath path = contourToPainterPath(cnt, emSize, origin);
            scene_->addPath(path, contourPen_);
        }
    }



    // Points
    if (components_ & kPoints) {
        for (auto& cnt : outline_.contours) {
            for (size_t i = 0; i < cnt.points.size(); ++i) {
                auto& p = cnt.points[i];
                scene_->addEllipse(xf(p.pos).x(), xf(p.pos).y(), 3, 3, p.on ? onPointPen_ : offPointPen_);
                auto textItem = scene_->addText(QString("%1").arg(i));
                textItem->setPos(xf(p.pos) + QPointF(6, 6));

                QColor color = p.on ? onPointPen_.color() : offPointPen_.color();
                textItem->setDefaultTextColor(color);
            }
        }
    }

}


QPainterPath
QXOutlineWidget::contourToPainterPath(const FXGlyphOutline::Contour & cnt,
                                      const QSizeF & emSize,
                                      const QPointF & origin) const {



    auto xf = [origin, emSize, this](const FXVec2d<int>& p) {
        return transformPoint(p, emSize, origin);
    };
    
    QPainterPath path;

    FXOpt<size_t> lastOnIndex;
    FXOpt<size_t> firstOnIndex;
    for (size_t i = 0; i < cnt.points.size(); ++i) {
        auto & p = cnt.points[i];

        if (p.on) {
            if (lastOnIndex)
            {
                FXVector<FXVec2d<int>> controlPoints;
                for (size_t j = *lastOnIndex + 1; j < i; ++j) {
                    assert(!cnt.points[j].on);
                    controlPoints.push_back(cnt.points[j].pos);
                }
                if (controlPoints.size() == 0) {
                    path.lineTo(xf(p.pos));
                }
                else if (controlPoints.size() == 1) {
                    path.quadTo(xf(controlPoints[0]), xf(p.pos));
                }
                else {
                    path.cubicTo(xf(controlPoints[0]), xf(controlPoints[1]), xf(p.pos));
                }

            }
            else {
                path.moveTo(xf(p.pos));
            }
        }

        if (p.on && !firstOnIndex)
            firstOnIndex = i;
        if (p.on)
            lastOnIndex = i;

        if (i + 1 == cnt.points.size()) {
            // last point

            auto firstPoint = xf(cnt.points[*firstOnIndex].pos);

            if (p.on) {
                auto p0 = xf(p.pos);
                if (p0 != firstPoint)
                    path.lineTo(firstPoint);
            }
            else if (lastOnIndex) {
                FXVector<FXVec2d<int>> controlPoints;
                for (size_t j = *lastOnIndex + 1; j <= i; ++j) {
                    assert(!cnt.points[j].on);
                    controlPoints.push_back(cnt.points[j].pos);
                }

                if (controlPoints.size() == 1) {
                    path.quadTo(xf(controlPoints[0]), firstPoint);
                }
                else {
                    path.cubicTo(xf(controlPoints[0]), xf(controlPoints[1]), firstPoint);
                }
            }
        }
    }

    return path;
        
}
    

QPointF
QXOutlineWidget::transformPoint(const FXVec2d<int> & p, const QSizeF & emSize, const QPointF & origin) const {
    auto x = origin.x() + p.x * 1.0 / outline_.upem * emSize.width();
    auto y = origin.y() - p.y * 1.0 / outline_.upem * emSize.height();
    return QPointF(x, y);
}
    

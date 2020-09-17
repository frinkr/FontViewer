#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QTimer>
#include <qmath.h>
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
    view_->viewport()->installEventFilter(this);
    view_->setMouseTracking(true);

    layout->setMargin(0);
    layout->addWidget(view_);

    int defaultPenWidth = 1;
    auto defaultCap = Qt::FlatCap;
    auto defaultJoin = Qt::MiterJoin;

    axisPen_ = QPen(Qt::gray, defaultPenWidth, Qt::SolidLine, defaultCap, defaultJoin);
    axisAuxPen_ = QPen(Qt::gray, 0.2, Qt::DotLine);
    contourPen_ = QPen(Qt::yellow, defaultPenWidth, Qt::SolidLine, defaultCap, defaultJoin);
    sketchPen_ = QPen(Qt::red, defaultPenWidth, Qt::SolidLine, defaultCap, defaultJoin);
    onPointPen_ = QPen(Qt::red, defaultPenWidth, Qt::SolidLine, defaultCap, defaultJoin);
    offPointPen_ = QPen(Qt::green, defaultPenWidth, Qt::SolidLine, defaultCap, defaultJoin);

    components_ = kContours | kFillContours | kPoints | kSketch | kGrids | kEmSquare;
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


bool
QXOutlineWidget::eventFilter(QObject * object, QEvent * event) {
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF delta = targetViewportPos_ - mouseEvent->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            targetViewportPos_ = mouseEvent->pos();
            targetScenePos_ = view_->mapToScene(targetViewportPos_);
        }
    }
    else if (event->type() == QEvent::Wheel){
        QWheelEvent * wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->orientation() == Qt::Vertical) {
            double angle = wheelEvent->angleDelta().y();
            double factor = qPow(1.0015, angle);

            view_->scale(factor, factor);
            view_->centerOn(targetScenePos_);
            QPointF deltaPos = targetViewportPos_ - 
                QPointF(view_->viewport()->width() / 2.0,
                        view_->viewport()->height() / 2.0);
            
            QPointF viewportCenter = view_->mapFromScene(targetScenePos_) - deltaPos;
            view_->centerOn(view_->mapToScene(viewportCenter.toPoint()));

            return true;
        }
    }

    Q_UNUSED(object);
    return false;
}


void 
QXOutlineWidget::buildScene() {
    //scene_ = new QGraphicsScene(this);
    scene_->clear();

    constexpr float sceneScale = 20; // How many EMs in a row
    const QPointF origin(0, 0);
    const QSizeF emSize(500, 500);
    const QRectF sceneRect(-emSize.width() * sceneScale / 2, -emSize.height() * sceneScale / 2, emSize.width() * sceneScale, emSize.height() * sceneScale);

    scene_->setSceneRect(sceneRect);

    
    auto xf = [origin, emSize, this](const FXVec2d<int>& p) {
        return transformPoint(p, emSize, origin);
    };

    
    // Grid
    if (components_ & kGrids) {
        constexpr int step = 100;
        const int gridMin = -qRound(sceneScale * outline_.upem / step) * step;
        const int gridMax = -gridMin;

        for (int y = gridMin; y <= gridMax; y += step)
            scene_->addLine(QLineF(xf({gridMin, y}), xf({gridMax, y})), axisAuxPen_);
        for (int x = gridMin; x <= gridMax; x += step)
            scene_->addLine(QLineF(xf({x, gridMin}), xf({x, gridMax})), axisAuxPen_);

    }

    // EM square
    if (components_ & kEmSquare) {
        QRectF rect(xf({ 0, outline_.upem }), xf({ outline_.upem, 0 }));
        scene_->addRect(rect, axisPen_);
    }

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
        QPainterPath paths;
        for (auto & cnt : outline_.contours) {
            QPainterPath path = contourToPainterPath(cnt, emSize, origin);
            paths.addPath(path);
        }
        paths.setFillRule(Qt::OddEvenFill);
        scene_->addPath(paths, contourPen_, (components_ & kFillContours)? QBrush(contourPen_.color()) : QBrush());
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
    

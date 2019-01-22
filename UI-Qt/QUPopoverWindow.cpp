#include <QBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QGUIApplication>
#include "QUPopoverWindow.h"
#include <QGraphicsDropShadowEffect>

namespace {
    constexpr int  POPOVER_ARROW_SIZE = 10;
    bool
    isHorizontal(QUPopoverEdge edge) {
        return edge == QUPopoverLeft || edge == QUPopoverRight;
    }
}

QUPopoverWindow::QUPopoverWindow(QWidget * parent)
    : QWidget(parent, Qt::Popup)
    , edge_(QUPopoverBottom)      
    , widget_(nullptr)
    , layout_(nullptr) {
    setAttribute(Qt::WA_TranslucentBackground);
}

void
QUPopoverWindow::setWidget(QWidget * widget) {
    widget_ = widget;
    setFocusProxy(widget_);
}

QWidget *
QUPopoverWindow::widget() const {
    return widget_;
}

QUPopoverEdge
QUPopoverWindow::edge() const {
    return edge_;
}

void
QUPopoverWindow::showRelativeTo(const QRect & rect, QUPopoverEdges preferedEgdes) {
    setEdge(edgeRelativeTo(rect, preferedEgdes));
    setFocus(Qt::ActiveWindowFocusReason);
    show();
    move(geometryRelativeTo(rect, edge_).topLeft());
}

void
QUPopoverWindow::showRelativeTo(QWidget * widget, QUPopoverEdges preferedEdges) {
    QPoint leftTop = widget->mapToGlobal(QPoint(0, 0));
    QPoint rightBottom = widget->mapToGlobal(QPoint(widget->frameGeometry().width(), widget->frameGeometry().height()));

    return showRelativeTo(QRect(leftTop, rightBottom), preferedEdges);
}

QSize
QUPopoverWindow::sizeHint() const {
    if (!widget_)
        return QWidget::sizeHint();
    return widget_->sizeHint() + QSize(
        isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0,
        !isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0);
}

QSize
QUPopoverWindow::minimumSizeHint() const {
    if (!widget_)
        return QWidget::minimumSizeHint();
    return widget_->minimumSizeHint() + QSize(
        isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0,
        !isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0);
}

void
QUPopoverWindow::resizeEvent(QResizeEvent * event) {
    //setMask(localRegion());
    QWidget::resizeEvent(event);
}

void
QUPopoverWindow::paintEvent(QPaintEvent * event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    p.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing);

    auto poly = localPolygon();
    auto color = palette().color(QPalette::Normal, QPalette::Window);
    p.setBrush(color);
    p.setPen(color);
    p.drawConvexPolygon(poly);
}

void
QUPopoverWindow::showEvent(QShowEvent * event) {
    QWidget::showEvent(event);

    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(200);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void
QUPopoverWindow::setEdge(QUPopoverEdge edge) {
    edge_ = edge;
    if (!widget_)
        return;

    if (layout()) 
        delete layout();

    layout_ = new QBoxLayout(isHorizontal(edge)? QBoxLayout::LeftToRight: QBoxLayout::TopToBottom);
    bool addSpacingBefore = (edge == QUPopoverRight) || (edge == QUPopoverBottom);
    if (addSpacingBefore)
        layout_->addSpacing(POPOVER_ARROW_SIZE);
    layout_->addWidget(widget_);
    if (!addSpacingBefore)
        layout_->addSpacing(POPOVER_ARROW_SIZE);

    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);
}

QUPopoverEdge
QUPopoverWindow::edgeRelativeTo(const QRect & rect, QUPopoverEdges preferedEgdes) {
    QUPopoverEdges otherEdges = ~preferedEgdes;
    
    QVector<QUPopoverEdge> edges;
    if (preferedEgdes & QUPopoverBottom) edges.append(QUPopoverBottom);
    if (preferedEgdes & QUPopoverRight) edges.append(QUPopoverRight);
    if (preferedEgdes & QUPopoverLeft) edges.append(QUPopoverLeft);
    if (preferedEgdes & QUPopoverTop) edges.append(QUPopoverTop);
    if (otherEdges & QUPopoverBottom) edges.append(QUPopoverBottom);
    if (otherEdges & QUPopoverRight) edges.append(QUPopoverRight);
    if (otherEdges & QUPopoverLeft) edges.append(QUPopoverLeft);
    if (otherEdges & QUPopoverTop) edges.append(QUPopoverTop);
    
    if (edges.count() == 1)
        return edges[0];

    
    QList<QRect> geometries;
    foreach(QUPopoverEdge edge, edges) 
        geometries.append(geometryRelativeTo(rect, edge));

    QScreen * screen = qApp->screenAt(rect.center());
    if (!screen) screen = qApp->primaryScreen();
    if (screen) {
        auto screenRect = screen->availableGeometry();
        for (size_t i = 0; i < edges.size(); ++ i) {
            if (screenRect.contains(geometries[i]))
                return edges[i];
        }
    }

    return edges[0];
}
    

QRect
QUPopoverWindow::geometryRelativeTo(const QRect & rect, QUPopoverEdge edge) {
    const int width = frameSize().width();
    const int height = frameSize().height();
    QPoint center;
    switch (edge) {
    case QUPopoverLeft:
        center = QPoint(rect.left() - width / 2,
                        rect.center().y());
        break;
    case QUPopoverRight:
        center = QPoint(rect.right() + width / 2,
                        rect.center().y());
        break;
        
    case QUPopoverTop:
        center = QPoint(rect.center().x(),
                        rect.top() - height /2);
        break;
        
    case QUPopoverBottom:
        center = QPoint(rect.center().x(),
                        rect.bottom() + height /2);

        break;
    default:
        center = rect.center();
    }
    
    QRect box = QRect(center - QPoint(width / 2, height / 2),
                      center + QPoint(width / 2, height / 2));
    return box;
}

QRegion
QUPopoverWindow::localRegion() {
    return localPolygon().toPolygon();
}

QPolygonF
QUPopoverWindow::localPolygon() {
    QVector<QPointF> points;
    const qreal width = size().width() - (isHorizontal(edge_)? POPOVER_ARROW_SIZE : 0);
    const qreal height = size().height() - (isHorizontal(edge_)? 0: POPOVER_ARROW_SIZE);
    
    const qreal arrowHeight = POPOVER_ARROW_SIZE;
    const qreal arrowWidth  = POPOVER_ARROW_SIZE * 2.5;

    const QPointF topLeft(-width/2, -height/2);
    const QPointF topRight(width/2, -height/2);
    const QPointF bottomLeft(-width/2, height/2);
    const QPointF bottomRight(width/2, height/2);

    QVector<QPointF> arrowPoints;
    arrowPoints.append(QPointF(-arrowWidth/2, 0));
    arrowPoints.append(QPointF(0, - arrowHeight));
    arrowPoints.append(QPointF(arrowWidth/2, 0));

    auto rotate = [](const QVector<QPointF> & points, const QPointF & translate, qreal rotate) {
        QTransform r, t, xform;
        r.rotate(rotate);
        t.translate(translate.x(), translate.y());
        xform = r * t;
        
        QVector<QPointF> ret;
        for(QPointF p : points)
            ret.append(xform.map(p));
        return ret;
    };

    QPointF center;
    switch (edge_) {
    case QUPopoverLeft:
        center = QPointF(width / 2, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(rotate(arrowPoints, QPointF(width / 2, 0), 90));
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    case QUPopoverRight:
        center = QPointF(width / 2 + POPOVER_ARROW_SIZE, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(rotate(arrowPoints, QPointF(- width / 2, 0), -90));        
        points.append(topLeft);
        break;
    case QUPopoverTop:
        center = QPointF(width / 2, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(bottomRight);
        points.append(rotate(arrowPoints, QPointF(0, height / 2), 180));
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    case QUPopoverBottom:
    default:
        center = QPointF(width/ 2, height / 2 + POPOVER_ARROW_SIZE);
        points.append(topLeft);
        points.append(rotate(arrowPoints, QPointF(0, - height / 2), 0));
        points.append(topRight);
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    }

    QPolygonF poly(points);
    poly.translate(center);
    return poly;
}

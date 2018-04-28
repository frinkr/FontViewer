#include <QBoxLayout>
#include "QUPopoverWindow.h"

namespace {
    constexpr int  POPOVER_DISTANCE = 10;
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
}

void
QUPopoverWindow::setWidget(QWidget * widget) {
    widget_ = widget;
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
    move(geometryRelativeTo(rect, edge_).topLeft());
    show();
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
        isHorizontal(edge_)? POPOVER_DISTANCE: 0,
        !isHorizontal(edge_)? POPOVER_DISTANCE: 0);
}

QSize
QUPopoverWindow::minimumSizeHint() const {
    if (!widget_)
        return QWidget::minimumSizeHint();
    return widget_->minimumSizeHint() + QSize(
        isHorizontal(edge_)? POPOVER_DISTANCE: 0,
        !isHorizontal(edge_)? POPOVER_DISTANCE: 0);
}

void
QUPopoverWindow::resizeEvent(QResizeEvent * event) {
    setMask(localRegion());
    QWidget::resizeEvent(event);
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
        layout_->addSpacing(POPOVER_DISTANCE);
    layout_->addWidget(widget_);
    if (!addSpacingBefore)
        layout_->addSpacing(POPOVER_DISTANCE);

    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);
}

QUPopoverEdge
QUPopoverWindow::edgeRelativeTo(const QRect & rect, QUPopoverEdges preferedEgdes) {
    QVector<QUPopoverEdge> edges;
    if (preferedEgdes & QUPopoverBottom) edges.append(QUPopoverBottom); // prefer bottom
    if (preferedEgdes & QUPopoverRight) edges.append(QUPopoverRight);
    if (preferedEgdes & QUPopoverLeft) edges.append(QUPopoverLeft);
    if (preferedEgdes & QUPopoverTop) edges.append(QUPopoverTop);

    if (edges.count() == 1)
        return edges[0];

    // FIXME: find the edge
    QList<QRect> geometries;
    foreach(QUPopoverEdge edge, edges) 
        geometries.append(geometryRelativeTo(rect, edge));
    return QUPopoverBottom;
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
    QVector<QPoint> points;
    const int width = size().width() - (isHorizontal(edge_)? POPOVER_DISTANCE : 0);
    const int height = size().height() - (isHorizontal(edge_)? 0: POPOVER_DISTANCE);
    
    const int dirHeight = POPOVER_DISTANCE;
    const int dirWidth =  POPOVER_DISTANCE * 2.5;

    const QPoint topLeft(-width/2, -height/2);
    const QPoint topRight(width/2, -height/2);
    const QPoint bottomLeft(-width/2, height/2);
    const QPoint bottomRight(width/2, height/2);

    QVector<QPoint> dirPoints;
    dirPoints.append(QPoint(-dirWidth/2, 0));
    dirPoints.append(QPoint(0, - dirHeight));
    dirPoints.append(QPoint(dirWidth / 2, 0));

    auto rotate = [](const QVector<QPoint> & points, const QPoint & translate, qreal rotate) {
        QTransform r, t, xform;
        r.rotate(rotate);
        t.translate(translate.x(), translate.y());
        xform = r * t;
        
        QVector<QPoint> ret;
        for(QPoint p : points)
            ret.append(xform.map(p));
        return ret;
    };

    QPoint center;
    switch (edge_) {
    case QUPopoverLeft:
        center = QPoint(width / 2, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(rotate(dirPoints, QPoint(width / 2, 0), 90));
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    case QUPopoverRight:
        center = QPoint(width / 2 + POPOVER_DISTANCE, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(rotate(dirPoints, QPoint(- width / 2, 0), -90));        
        points.append(topLeft);
        break;
    case QUPopoverTop:
        center = QPoint(width/ 2, height / 2 - POPOVER_DISTANCE);
        points.append(topLeft);
        points.append(topRight);
        points.append(bottomRight);
        points.append(rotate(dirPoints, QPoint(0, height / 2), 180));
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    case QUPopoverBottom:
    default:
        center = QPoint(width/ 2, height / 2 + POPOVER_DISTANCE);
        points.append(topLeft);
        points.append(rotate(dirPoints, QPoint(0, - height / 2), 0));
        points.append(topRight);
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    }

    QPolygon poly(points);
    poly.translate(center);
    return QRegion(poly);
}

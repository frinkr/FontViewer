#include <QBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QImage>
#include <QBitmap>
#include <QPixmap>
#include <QGuiApplication>
#include "QXPopoverWindow.h"
#include <QGraphicsDropShadowEffect>

namespace {
#ifdef Q_OS_MACOS
    constexpr qreal BORDER = 0.5;
#else
    constexpr qreal BORDER = 1;
#endif
    constexpr qreal POPOVER_ARROW_SIZE = 10;
    bool
    isHorizontal(QXPopoverEdge edge) {
        return edge == QXPopoverLeft || edge == QXPopoverRight;
    }
}

QXPopoverWindow::QXPopoverWindow(QWidget * parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
    , edge_(QXPopoverBottom)      
    , widget_(nullptr)
    , layout_(nullptr) {
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

void
QXPopoverWindow::setWidget(QWidget * widget) {
    widget_ = widget;
    widget_->setParent(this);
    setFocusProxy(widget_);
}

QWidget *
QXPopoverWindow::widget() const {
    return widget_;
}

QXPopoverEdge
QXPopoverWindow::edge() const {
    return edge_;
}

void
QXPopoverWindow::showRelativeTo(const QRect & rect, QXPopoverEdges preferedEgdes) {
    setEdge(edgeRelativeTo(rect, preferedEgdes));
    setFocus(Qt::ActiveWindowFocusReason);
    show();
    move(geometryRelativeTo(rect, edge_).topLeft());
}

void
QXPopoverWindow::showRelativeTo(QWidget * widget, QXPopoverEdges preferedEdges) {
    QPoint leftTop = widget->mapToGlobal(QPoint(0, 0));
    QPoint rightBottom = widget->mapToGlobal(QPoint(widget->frameGeometry().width(), widget->frameGeometry().height()));

    return showRelativeTo(QRect(leftTop, rightBottom), preferedEdges);
}

QSize
QXPopoverWindow::sizeHint() const {
    if (!widget_)
        return QWidget::sizeHint();
    return widget_->sizeHint() + QSize(
        isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0,
        !isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0);
}

QSize
QXPopoverWindow::minimumSizeHint() const {
    if (!widget_)
        return QWidget::minimumSizeHint();
    return widget_->minimumSizeHint() + QSize(
        isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0,
        !isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0);
}

void
QXPopoverWindow::resizeEvent(QResizeEvent * event) {
#ifdef Q_OS_WIN
	setMask(localRegion(0));
#endif
    QWidget::resizeEvent(event);
}

void
QXPopoverWindow::paintEvent(QPaintEvent * event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    p.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing);
	
    auto poly = localPolygon(BORDER);
    auto color = palette().color(QPalette::Normal, QPalette::Window);
    QPen pen(palette().color(QPalette::Normal, QPalette::Mid), BORDER, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setBrush(color);
    p.setPen(pen);
    p.drawConvexPolygon(poly);
}

void
QXPopoverWindow::showEvent(QShowEvent * event) {
    QWidget::showEvent(event);

    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(200);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void
QXPopoverWindow::setEdge(QXPopoverEdge edge) {
    edge_ = edge;
    if (!widget_)
        return;

    if (layout()) 
        delete layout();

    layout_ = new QBoxLayout(isHorizontal(edge)? QBoxLayout::LeftToRight: QBoxLayout::TopToBottom);
    bool addSpacingBefore = (edge == QXPopoverRight) || (edge == QXPopoverBottom);
    if (addSpacingBefore)
        layout_->addSpacing(POPOVER_ARROW_SIZE);
    layout_->addWidget(widget_);
    if (!addSpacingBefore)
        layout_->addSpacing(POPOVER_ARROW_SIZE);

    layout_->setSpacing(0);
    layout_->setContentsMargins(BORDER, BORDER, BORDER, BORDER);
    setLayout(layout_);
}

QXPopoverEdge
QXPopoverWindow::edgeRelativeTo(const QRect & rect, QXPopoverEdges preferedEgdes) {
    QXPopoverEdges otherEdges = ~preferedEgdes;
    
    QVector<QXPopoverEdge> edges;
    if (preferedEgdes & QXPopoverBottom) edges.append(QXPopoverBottom);
    if (preferedEgdes & QXPopoverRight) edges.append(QXPopoverRight);
    if (preferedEgdes & QXPopoverLeft) edges.append(QXPopoverLeft);
    if (preferedEgdes & QXPopoverTop) edges.append(QXPopoverTop);
    if (otherEdges & QXPopoverBottom) edges.append(QXPopoverBottom);
    if (otherEdges & QXPopoverRight) edges.append(QXPopoverRight);
    if (otherEdges & QXPopoverLeft) edges.append(QXPopoverLeft);
    if (otherEdges & QXPopoverTop) edges.append(QXPopoverTop);
    
    if (edges.count() == 1)
        return edges[0];

    
    QList<QRect> geometries;
    foreach(QXPopoverEdge edge, edges) 
        geometries.append(geometryRelativeTo(rect, edge));
    
#if QT_VERSION_MAJOR >= 5 && QT_VERSION_MINOR >= 10
    QScreen * screen = qApp->screenAt(rect.center());
    if (!screen) screen = qApp->primaryScreen();
#else
    QScreen * screen = qApp->primaryScreen();
#endif
    
    if (screen) {
        auto screenRect = screen->availableGeometry();
        for (int i = 0; i < edges.size(); ++ i) {
            if (screenRect.contains(geometries[i]))
                return edges[i];
        }
    }
    return edges[0];
}
    

QRect
QXPopoverWindow::geometryRelativeTo(const QRect & rect, QXPopoverEdge edge) {
    const int width = frameSize().width();
    const int height = frameSize().height();
    QPoint center;
    switch (edge) {
    case QXPopoverLeft:
        center = QPoint(rect.left() - width / 2,
                        rect.center().y());
        break;
    case QXPopoverRight:
        center = QPoint(rect.right() + width / 2,
                        rect.center().y());
        break;
        
    case QXPopoverTop:
        center = QPoint(rect.center().x(),
                        rect.top() - height /2);
        break;
        
    case QXPopoverBottom:
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
QXPopoverWindow::localRegion(qreal border) {
    return localPolygon(border).toPolygon();
}

QPolygonF
QXPopoverWindow::localPolygon(qreal border) {
    QVector<QPointF> points;
    const qreal width = size().width() - (isHorizontal(edge_)? POPOVER_ARROW_SIZE : 0);
    const qreal height = size().height() - (isHorizontal(edge_)? 0: POPOVER_ARROW_SIZE);
    
    const qreal arrowHeight = POPOVER_ARROW_SIZE - border;
    const qreal arrowWidth  = arrowHeight * 2.5;

    const QPointF topLeft((border-width)/2, (border-height)/2);
    const QPointF topRight((width-border)/2, (border-height)/2);
    const QPointF bottomLeft((border-width)/2, (height-border)/2);
    const QPointF bottomRight((width-border)/2, (height-border)/2);

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
    case QXPopoverLeft:
        center = QPointF(width / 2, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(rotate(arrowPoints, QPointF((width - border) / 2, 0), 90));
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    case QXPopoverRight:
        center = QPointF(width / 2 + POPOVER_ARROW_SIZE, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(bottomRight);
        points.append(bottomLeft);
        points.append(rotate(arrowPoints, QPointF((border - width) / 2, 0), -90));
        points.append(topLeft);
        break;
    case QXPopoverTop:
        center = QPointF(width / 2, height / 2);
        points.append(topLeft);
        points.append(topRight);
        points.append(bottomRight);
        points.append(rotate(arrowPoints, QPointF(0, (height - border)/ 2), 180));
        points.append(bottomLeft);
        points.append(topLeft);
        break;
    case QXPopoverBottom:
    default:
        center = QPointF(width / 2, height / 2 + POPOVER_ARROW_SIZE);
        points.append(topLeft);
        points.append(rotate(arrowPoints, QPointF(0, (border - height) / 2), 0));
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

#include <QBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QImage>
#include <QBitmap>
#include <QPixmap>
#include "QXApplication.h"
#include "QXPopoverWindow.h"
#include <QStyleOptionFrame>
#include <QImage>
#include <QTimer>
#include <cmath>
namespace {
    constexpr qreal DEFAULT_BORDER = 1;
    constexpr qreal DEFAULT_BORDER_RADIUS = 5;
    constexpr qreal POPOVER_ARROW_SIZE = 10;
    constexpr qreal PI = 3.14159265358979323846;

    bool
    isHorizontal(QXPopoverEdge edge) {
        return edge == QXPopoverLeft || edge == QXPopoverRight;
    }

    QRect
    screenRectAtPoint(const QPoint & point) {
#if QT_VERSION_MAJOR >= 5 && QT_VERSION_MINOR >= 10
        QScreen * screen = qApp->screenAt(point);
        if (!screen) screen = qApp->primaryScreen();
#else
        QScreen * screen = qApp->primaryScreen();
#endif
        if (screen)
            return screen->availableGeometry();
        else
            return QRect();
    }

    void
    arcToTopLeft(QPainterPath & path, const QPointF & point, qreal radius) {
        QRectF rect(point, QSizeF(radius * 2, radius * 2));
        path.arcTo(rect, 90, 90);
    }

    void
    arcToTopRight(QPainterPath & path, const QPointF & point, qreal radius) {
        QRectF rect(point - QPointF(2 * radius, 0),
                    QSizeF(radius * 2, radius * 2));
        path.arcTo(rect, 0, 90);
    }

    void
    arcToBottomLeft(QPainterPath & path, const QPointF & point, qreal radius) {
        QRectF rect(point - QPointF(0, 2 * radius),
                    QSizeF(radius * 2, radius * 2));
        
        path.arcTo(rect, 180, 90);
    }

    void
    arcToBottomRight(QPainterPath & path, const QPointF & point, qreal radius) {
        QRectF rect(point - QPointF(2 * radius, 2 * radius),
                    QSizeF(radius * 2, radius * 2));
        path.arcTo(rect, 270, 90);
    }

    

}

QXPopoverWindow::QXPopoverWindow(QWidget * parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
    , edge_(QXPopoverBottom)      
    , widget_(nullptr)
    , layout_(nullptr)
    , border_(DEFAULT_BORDER)
    , borderRadius_(DEFAULT_BORDER_RADIUS) {
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

void
QXPopoverWindow::setWidget(QWidget * widget) {
    widget_ = widget;
    widget_->setParent(this);
    setFocusProxy(widget_);
}

qreal
QXPopoverWindow::boder() const {
    return border_;
}

void
QXPopoverWindow::setBorder(qreal border) {
    border_ = border;
}

qreal
QXPopoverWindow::borderRadius() const {
    return borderRadius_;
}

void
QXPopoverWindow::setBorderRadius(qreal radius) {
    borderRadius_ = radius;
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
    referenceRect_ = rect;
    setEdge(edgeRelativeTo(rect, preferedEgdes));
    setFocus(Qt::ActiveWindowFocusReason);
    show();
    move(windowGeometryRelativeTo(rect, edge_).topLeft());
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
        !isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0) + QSize(border_ + borderRadius_, border_ + borderRadius_);
}

QSize
QXPopoverWindow::minimumSizeHint() const {
    if (!widget_)
        return QWidget::minimumSizeHint();
    return widget_->minimumSizeHint() + QSize(
        isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0,
        !isHorizontal(edge_)? POPOVER_ARROW_SIZE: 0) + QSize(border_ + borderRadius_, border_ + borderRadius_);
}

void
QXPopoverWindow::resizeEvent(QResizeEvent * event) {
    QWidget::resizeEvent(event);
#if defined(Q_OS_WIN)
    QTimer::singleShot(200, [this]() {updateMask();});
#endif
}

void
QXPopoverWindow::paintEvent(QPaintEvent * event) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::Antialiasing);

    auto backgroundColor = palette().window().color();
    auto borderColor = palette().midlight().color();
    
    // Get the frame color
    if (false) {
        QPixmap px(5, 5);
        px.fill();
        QPainter pp(&px);
        QStyleOptionFrame opt;
        opt.initFrom(this);
        opt.rect = QRect(QPoint(0, 0), px.size());
        opt.frameShape = QFrame::StyledPanel;
        opt.lineWidth = 20;
        opt.midLineWidth = 0;
        style()->drawControl(QStyle::CE_ShapedFrame, &opt, &pp, this);

        QImage img = px.toImage();
        borderColor = img.pixel(0, 0);
    }

    p.setBrush(borderRadius_? backgroundColor: borderColor);
    p.setPen(QPen(borderColor, border_));

    QPainterPath roundRectPath = painterPath();
    p.fillPath(roundRectPath, backgroundColor);
    p.drawPath(roundRectPath);
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
    const qreal margin = border_ + borderRadius_;
    layout_->setContentsMargins(margin + 1, margin + 1, margin, margin);
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
        geometries.append(windowGeometryRelativeTo(rect, edge));
    
    auto screenRect = screenRectAtPoint(rect.center());
    for (int i = 0; i < edges.size(); ++ i) {
        if (screenRect.contains(geometries[i]))
            return edges[i];
    }
    
    return edges[0];
}
    

QRect
QXPopoverWindow::windowGeometryRelativeTo(const QRect & rect, QXPopoverEdge edge) {
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

    // Make sure 'box' is not out of screen
    QRectF screenRect = screenRectAtPoint(box.center());
    if (edge == QXPopoverTop || edge == QXPopoverBottom) {
        if (box.left() < screenRect.left())
            box.translate(screenRect.left() - box.left(), 0);
        else if (box.right() > screenRect.right())
            box.translate(screenRect.right() - box.right(), 0);
    }
    if (edge == QXPopoverLeft || edge == QXPopoverRight) {
        if (box.top() < screenRect.top())
            box.translate(0, screenRect.top() - box.top());
        else if (box.bottom() > screenRect.bottom())
            box.translate(0, screenRect.bottom() - box.bottom());
    }
    
    return box;
}

QRectF
QXPopoverWindow::contentRect(qreal border) const {
    QVector<QPointF> points;
    const qreal width = size().width() - (isHorizontal(edge_) ? POPOVER_ARROW_SIZE : 0);
    const qreal height = size().height() - (isHorizontal(edge_) ? 0 : POPOVER_ARROW_SIZE);

    const QPointF topLeft((border - width) / 2, (border - height) / 2);
    const QPointF bottomRight((width - border) / 2, (height - border) / 2);

    QPointF center;
    switch (edge_) {
    case QXPopoverLeft:
        center = QPointF(width / 2, height / 2);
        break;
    case QXPopoverRight:
        center = QPointF(width / 2 + POPOVER_ARROW_SIZE, height / 2);
        break;
    case QXPopoverTop:
        center = QPointF(width / 2, height / 2);
        break;
    case QXPopoverBottom:
    default:
        center = QPointF(width / 2, height / 2 + POPOVER_ARROW_SIZE);
        break;
    }

    QRectF rect(topLeft, bottomRight);
    return rect.translated(center);
}

QPainterPath
QXPopoverWindow::painterPath(bool mask) const {
    QRectF contentRect = this->contentRect(border_);
    if (!mask)
        contentRect.adjust(1, 1, 0, 0);

    const QRect refRect(mapFromGlobal(referenceRect_.topLeft()), mapFromGlobal(referenceRect_.bottomRight()));

    qreal arrowRatio = 1; // = H/W
    QPointF arrowHead, arrowLeft, arrowRight;
    if (edge_ == QXPopoverBottom) {
        arrowHead = QPointF(refRect.center().x(), refRect.bottom() + (mask? 0: 1));
        arrowLeft = QPointF(arrowHead.x() - POPOVER_ARROW_SIZE / arrowRatio, arrowHead.y() + POPOVER_ARROW_SIZE);
        arrowRight = QPointF(arrowHead.x() + POPOVER_ARROW_SIZE / arrowRatio, arrowHead.y() + POPOVER_ARROW_SIZE);
    }
    else if (edge_ == QXPopoverTop) {
        arrowHead = QPointF(refRect.center().x(), refRect.top());
        arrowLeft = QPointF(arrowHead.x() + POPOVER_ARROW_SIZE / arrowRatio, arrowHead.y() - POPOVER_ARROW_SIZE);
        arrowRight = QPointF(arrowHead.x() - POPOVER_ARROW_SIZE / arrowRatio, arrowHead.y() - POPOVER_ARROW_SIZE);
    }
    else if (edge_ == QXPopoverLeft) {
        arrowHead = QPointF(refRect.left(), refRect.center().y());
        arrowLeft = QPointF(arrowHead.x() - POPOVER_ARROW_SIZE, arrowHead.y() - POPOVER_ARROW_SIZE / arrowRatio);
        arrowRight = QPointF(arrowHead.x() - POPOVER_ARROW_SIZE, arrowHead.y() + POPOVER_ARROW_SIZE / arrowRatio);
    }
    else {
        arrowHead = QPointF(refRect.right() + 1, refRect.center().y());
        arrowLeft = QPointF(arrowHead.x() + POPOVER_ARROW_SIZE, arrowHead.y() + POPOVER_ARROW_SIZE / arrowRatio);
        arrowRight = QPointF(arrowHead.x() + POPOVER_ARROW_SIZE, arrowHead.y() - POPOVER_ARROW_SIZE / arrowRatio);
    }
    
    
    qreal arrowRadius = borderRadius_ * 1.8;
    qreal arrowArcDistance = (std::atan(arrowRatio) / 2) * arrowRadius;  // the distance of arrow arc begining to arrowLeft/right
    qreal radianToDegree = 180 / PI;
    int dx = 0, dy = 0;
    if (!isHorizontal(edge_)) {
        if (arrowLeft.x() < contentRect.left() + borderRadius_ + arrowArcDistance)
            dx = contentRect.left() + borderRadius_ + arrowArcDistance - arrowLeft.x();
        else if (arrowRight.x() > contentRect.right() - borderRadius_ - arrowArcDistance)
            dx = contentRect.right() - borderRadius_ - arrowArcDistance - arrowRight.x();
    }
    else {
        if (arrowLeft.y() > contentRect.bottom() - borderRadius_ - arrowArcDistance)
            dy = contentRect.bottom() - borderRadius_ - arrowArcDistance - arrowLeft.y();
        else if (arrowRight.y() < contentRect.top() + borderRadius_ + arrowArcDistance)
            dy = contentRect.top() + borderRadius_ + arrowArcDistance - arrowRight.y();
    }
    
    if (dx || dy) {
        arrowHead += QPoint(dx, dy);
        arrowLeft += QPoint(dx, dy);
        arrowRight += QPoint(dx, dy);
    }
    
    // Add point couter-clock wise
    QPainterPath roundRectPath;
    if (!borderRadius_) {
        if (edge_ == QXPopoverBottom) {
            roundRectPath.moveTo(contentRect.topLeft());
            roundRectPath.lineTo(contentRect.bottomLeft());
            roundRectPath.lineTo(contentRect.bottomRight());
            roundRectPath.lineTo(contentRect.topRight());
            roundRectPath.lineTo(arrowRight);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.lineTo(arrowLeft);
            roundRectPath.closeSubpath();
        }
        else if (edge_ == QXPopoverTop) {
            roundRectPath.moveTo(contentRect.bottomRight());
            roundRectPath.lineTo(contentRect.topRight());
            roundRectPath.lineTo(contentRect.topLeft());
            roundRectPath.lineTo(contentRect.bottomLeft());
            roundRectPath.lineTo(arrowRight);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.lineTo(arrowLeft);
            roundRectPath.closeSubpath();
        }
        else if (edge_ == QXPopoverLeft) {
            roundRectPath.moveTo(contentRect.topRight());
            roundRectPath.lineTo(contentRect.topLeft());
            roundRectPath.lineTo(contentRect.bottomLeft());
            roundRectPath.lineTo(contentRect.bottomRight());
            roundRectPath.lineTo(arrowRight);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.lineTo(arrowLeft);
            roundRectPath.closeSubpath();
        }
        else {
            roundRectPath.moveTo(contentRect.bottomLeft());
            roundRectPath.lineTo(contentRect.bottomRight());
            roundRectPath.lineTo(contentRect.topRight());
            roundRectPath.lineTo(contentRect.topLeft());
            roundRectPath.lineTo(arrowRight);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.lineTo(arrowLeft);
            roundRectPath.closeSubpath();
        }
    }
    else {
        if (edge_ == QXPopoverBottom) {
            roundRectPath.moveTo(arrowLeft - QPointF(arrowArcDistance, 0));
            arcToTopLeft(roundRectPath, contentRect.topLeft(), borderRadius_);
            arcToBottomLeft(roundRectPath, contentRect.bottomLeft(), borderRadius_);
            arcToBottomRight(roundRectPath, contentRect.bottomRight(), borderRadius_);
            arcToTopRight(roundRectPath, contentRect.topRight(), borderRadius_);

            roundRectPath.lineTo(arrowRight + QPointF(arrowArcDistance, 0));
            roundRectPath.arcTo(QRectF(arrowRight + QPointF(arrowArcDistance - arrowRadius, -2 * arrowRadius), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                270,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.arcTo(QRectF(arrowLeft - QPointF(arrowRadius + arrowArcDistance, 2 * arrowRadius), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                270 + std::atan(arrowRatio)*radianToDegree,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.closeSubpath();
        }
        else if (edge_ == QXPopoverTop) {
            roundRectPath.moveTo(arrowLeft + QPointF(arrowArcDistance, 0));
            arcToBottomRight(roundRectPath, contentRect.bottomRight(), borderRadius_);
            arcToTopRight(roundRectPath, contentRect.topRight(), borderRadius_);
            arcToTopLeft(roundRectPath, contentRect.topLeft(), borderRadius_);
            arcToBottomLeft(roundRectPath, contentRect.bottomLeft(), borderRadius_);

            roundRectPath.lineTo(arrowRight - QPointF(arrowArcDistance, 0));
            roundRectPath.arcTo(QRectF(arrowRight - QPointF(arrowArcDistance + arrowRadius, 0), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                90,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.arcTo(QRectF(arrowLeft + QPointF(arrowArcDistance - arrowRadius, 0), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                90 + std::atan(arrowRatio)*radianToDegree,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.closeSubpath();
        }
        else if (edge_ == QXPopoverLeft) {
            roundRectPath.moveTo(arrowLeft + QPointF(0, -arrowArcDistance));
            arcToTopRight(roundRectPath, contentRect.topRight(), borderRadius_);
            arcToTopLeft(roundRectPath, contentRect.topLeft(), borderRadius_);
            arcToBottomLeft(roundRectPath, contentRect.bottomLeft(), borderRadius_);
            arcToBottomRight(roundRectPath, contentRect.bottomRight(), borderRadius_);

            roundRectPath.lineTo(arrowRight + QPointF(0, arrowArcDistance));
            roundRectPath.arcTo(QRectF(arrowRight + QPointF(0, arrowArcDistance - arrowRadius), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                180,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.arcTo(QRectF(arrowLeft - QPointF(0, arrowArcDistance + arrowRadius), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                180 + std::atan(arrowRatio)*radianToDegree,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.closeSubpath();
        }
        else {
            roundRectPath.moveTo(arrowLeft + QPointF(0, arrowArcDistance));
            arcToBottomLeft(roundRectPath, contentRect.bottomLeft(), borderRadius_);
            arcToBottomRight(roundRectPath, contentRect.bottomRight(), borderRadius_);
            arcToTopRight(roundRectPath, contentRect.topRight(), borderRadius_);
            arcToTopLeft(roundRectPath, contentRect.topLeft(), borderRadius_);

            roundRectPath.lineTo(arrowRight - QPointF(0, arrowArcDistance));
            roundRectPath.arcTo(QRectF(arrowRight - QPointF(2 * arrowRadius, arrowArcDistance + arrowRadius), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                0,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.lineTo(arrowHead);
            roundRectPath.arcTo(QRectF(arrowLeft - QPointF(2 * arrowRadius, arrowRadius - arrowArcDistance), QSizeF(2 * arrowRadius, 2 * arrowRadius)),
                90 - std::atan(arrowRatio)*radianToDegree,
                -std::atan(arrowRatio)*radianToDegree);
            roundRectPath.closeSubpath();
        }
    }
    return roundRectPath;
}

#if defined(Q_OS_WIN)
void
QXPopoverWindow::updateMask() {
    QPainterPath roundRectPath = painterPath(true);
    QPolygonF polygon = roundRectPath.toFillPolygon();
    setMask(QRegion(polygon.toPolygon(), Qt::WindingFill));    
}
#endif

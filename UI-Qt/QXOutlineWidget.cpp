#include <QPainter>
#include <QPainterPath>
#include "QXOutlineWidget.h"

QXOutlineWidget::QXOutlineWidget(QWidget *parent)
    : QWidget(parent)
{}

QXOutlineWidget::~QXOutlineWidget() {
}

void
QXOutlineWidget::paintEvent(QPaintEvent * event) {
    QPainter painter(this);
    float left = qMin(width(), height()) / 5;
    const QPointF origin(left, height() - left);

    auto s = qMin(width() - left, height() - left);
    const QSizeF emSize(s, s);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setPen(Qt::gray);
    painter.drawLine(origin, QPointF(origin.x(), origin.y() - emSize.height()));
    painter.drawLine(origin, QPointF(origin.x() + emSize.width(), origin.y()));

    auto xf = [origin, emSize, this](const FXVec2d<int>& p) {
        auto x = origin.x() + p.x * 1.0 / outline_.upem * emSize.width();
        auto y = origin.y() - p.y * 1.0 / outline_.upem * emSize.height();
        return QPointF(x, y);
    };

    

    // 
    for (auto& cnt : outline_.contours) {
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

        painter.setPen(Qt::yellow);
        painter.drawPath(path);
    }
    
    // Sketch mode
    for (auto& cnt : outline_.contours) {
        FXOpt<size_t> lastOnIndex;
        FXOpt<size_t> firstOnIndex;
        for (size_t i = 0; i < cnt.points.size(); ++i) {

            auto& p = cnt.points[i];
            painter.setPen(p.on ? Qt::red : Qt::green);
            painter.drawEllipse(xf(p.pos), 3, 3);
            painter.drawText(xf(p.pos) + QPointF(6, 6), QString("%1").arg(i));

            if (p.on && lastOnIndex)
                ;// painter.drawLine(xf(p.pos), xf(cnt.points[*lastOnIndex].pos));

            if (p.on && !firstOnIndex)
                firstOnIndex = i;
            if (p.on)
                lastOnIndex = i;

        }
        if (lastOnIndex && firstOnIndex) {
            auto p0 = xf(cnt.points[*lastOnIndex].pos);
            auto p1 = xf(cnt.points[*firstOnIndex].pos);
            if (p0 != p1) {
                painter.setPen(QPen(Qt::red, 1, Qt::DotLine));
                ;// painter.drawLine(p0, p1);
            }
        }
    }
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
    update();
}


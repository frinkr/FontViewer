#include <QPainter>
#include <QPaintEvent>
#include "QXIconButton.h"

void
QXIconButton::paintEvent(QPaintEvent * event) {
    QIcon icon = this->icon();
    QPainter painter(this);
    QPixmap px = icon.pixmap(this->iconSize());
    QMargins margins = this->contentsMargins();
    qreal scale = std::min<qreal>(
        qreal(width() - margins.left() - margins.right()) / px.width(),
        qreal(height() - margins.top() - margins.bottom()) / px.height());
    QRect r(QPoint(margins.left() + (width() - margins.left() - margins.right() - px.width() * scale) / 2,
                   margins.top() + (height() - margins.top() - margins.bottom() - px.height() * scale) / 2),
            QSize(px.width() * scale, px.height() * scale));
    painter.drawPixmap(r, px);
}

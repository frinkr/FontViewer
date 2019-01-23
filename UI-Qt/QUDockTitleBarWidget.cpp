#include <QMouseEvent>
#include <QDockWidget>
#include <QPainter>
#include <QStylePainter>
#include "QUDockTitleBarWidget.h"

namespace {
    int TITLE_BAR_HEIGHT = 18;
}
QUDockTitleBarWidget::QUDockTitleBarWidget(QWidget * parent)
    : QWidget(parent) {
        this->setMinimumHeight(20);
}

QUDockTitleBarWidget::~QUDockTitleBarWidget() {
    
}


QSize
QUDockTitleBarWidget::sizeHint() const {
    return minimumSizeHint();
}

QSize
QUDockTitleBarWidget::minimumSizeHint() const {
    return QSize(TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);
}

void
QUDockTitleBarWidget::paintEvent(QPaintEvent * event) {
    QPainter p(this);

    p.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    
    QRect rect(0, 0, width(), height());
    //p.fillRect(rect, palette().color(QPalette::Normal, QPalette::Window));

    QColor color = palette().color(QPalette::Normal, QPalette::Window);
    //color = color.darker();
    QBrush brush(color, Qt::Dense2Pattern);
    p.fillRect(rect, brush);

    // close icon
    QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    p.drawPixmap(closeIconRect(), closeIcon, closeIcon.rect());

    // title
    p.setPen(palette().color(QPalette::Normal, QPalette::Text));
    QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    p.drawText(rect, Qt::AlignCenter, dockWidget->windowTitle());
}

void
QUDockTitleBarWidget::mousePressEvent(QMouseEvent * event) {
    QRectF rect = closeIconRect();
    if (rect.contains(event->localPos())) {
        QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
        dockWidget->hide();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

QRectF
QUDockTitleBarWidget::closeIconRect() {
    QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    qreal h = qMin(closeIcon.height(), height());
    qreal w = h * closeIcon.width() / closeIcon.height();
    return QRectF(5, (height() - h) / 2, w, h);
}

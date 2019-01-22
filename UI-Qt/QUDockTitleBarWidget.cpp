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

    QRect rect(0, 0, width(), height());
    //p.fillRect(rect, palette().color(QPalette::Normal, QPalette::Window));

    QColor color = palette().color(QPalette::Normal, QPalette::Window);
    //color = color.darker();
    QBrush brush(color, Qt::Dense2Pattern);
    p.fillRect(rect, brush);

    // close icon
    QIcon icon = style()->standardIcon(QStyle::SP_TitleBarCloseButton, nullptr, this);
    icon.paint(&p, 0, 0, TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);

    // title
    p.setPen(palette().color(QPalette::Normal, QPalette::Text));
    QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    p.drawText(rect, Qt::AlignCenter, dockWidget->windowTitle());
}

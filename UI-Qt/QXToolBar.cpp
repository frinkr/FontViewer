#include <QMouseEvent>
#include "QXToolBar.h"

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
#  define QX_MOVE_WINDOW_BY_TOOLBAR
#endif
void
QXToolBar::mousePressEvent(QMouseEvent * event) {
#ifdef QX_MOVE_WINDOW_BY_TOOLBAR
    if ((event->buttons() & Qt::LeftButton)) {
        mousePressPos_ = event->pos();
        movingWindow_ = true;
    }
#endif
    QToolBar::mousePressEvent(event);
}

void
QXToolBar::mouseMoveEvent(QMouseEvent * event) {
#ifdef QX_MOVE_WINDOW_BY_TOOLBAR
    if (movingWindow_ && (event->buttons() & Qt::LeftButton)) {
        QPoint diff = event->pos() - mousePressPos_;
        QPoint newpos = this->window()->pos() + diff;
        this->window()->move(newpos);
    }
#endif
    QToolBar::mouseMoveEvent(event);
}

void
QXToolBar::mouseReleaseEvent(QMouseEvent * event) {
#ifdef QX_MOVE_WINDOW_BY_TOOLBAR
    movingWindow_ = false;
#endif
    QToolBar::mouseReleaseEvent(event);
}

void
QXToolBar::mouseDoubleClickEvent(QMouseEvent * event) {
#ifdef Q_OS_MAC
    if (this->window()->windowState().testFlag(Qt::WindowMaximized))
        this->window()->showNormal();
    else
        this->window()->showMaximized();
#endif
    QToolBar::mouseDoubleClickEvent(event);
}

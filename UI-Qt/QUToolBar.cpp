#include <QMouseEvent>
#include "QUToolBar.h"

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
#  define QX_MOVE_WINDOW_BY_TOOLBAR
#endif
void
QUToolBar::mousePressEvent(QMouseEvent * event) {
#ifdef QX_MOVE_WINDOW_BY_TOOLBAR
    if ((event->buttons() & Qt::LeftButton)) {
        mousePressPos_ = event->pos();
        movingWindow_ = true;
    }
#endif
    QToolBar::mousePressEvent(event);
}

void
QUToolBar::mouseMoveEvent(QMouseEvent * event) {
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
QUToolBar::mouseReleaseEvent(QMouseEvent * event) {
#ifdef QX_MOVE_WINDOW_BY_TOOLBAR
    movingWindow_ = false;
#endif
    QToolBar::mouseReleaseEvent(event);
}

void
QUToolBar::mouseDoubleClickEvent(QMouseEvent * event) {
#ifdef QX_MOVE_WINDOW_BY_TOOLBAR
    if (this->window()->windowState().testFlag(Qt::WindowMaximized))
        this->window()->showNormal();
    else
        this->window()->showMaximized();
#endif
    QToolBar::mouseDoubleClickEvent(event);
}

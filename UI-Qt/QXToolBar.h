#pragma once

#include <QToolBar>

class QXToolBar : public QToolBar {
    Q_OBJECT
public:
    using QToolBar::QToolBar;

protected:
    void
    mousePressEvent(QMouseEvent * event);

    void
    mouseMoveEvent(QMouseEvent * event);

    void
    mouseReleaseEvent(QMouseEvent * event);

    void
    mouseDoubleClickEvent(QMouseEvent * event);

private:
    QPoint mousePressPos_;
    bool   movingWindow_{false};
};

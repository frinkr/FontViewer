#pragma once

#include <QToolBar>

class QXToolBar : public QToolBar {
    Q_OBJECT
public:
    using QToolBar::QToolBar;

protected:
    void
    mousePressEvent(QMouseEvent * event) override;

    void
    mouseMoveEvent(QMouseEvent * event) override;

    void
    mouseReleaseEvent(QMouseEvent * event) override;

    void
    mouseDoubleClickEvent(QMouseEvent * event) override;

private:
    QPoint mousePressPos_;
    bool   movingWindow_{false};
};

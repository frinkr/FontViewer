#pragma once
#include <QWidget>

class QDockWidget;

class QPaintEvent;

class QUDockTitleBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit QUDockTitleBarWidget(QWidget * parent = 0);
    ~QUDockTitleBarWidget();

    virtual QSize
    sizeHint() const;

    virtual QSize
    minimumSizeHint() const;

    virtual void
    paintEvent(QPaintEvent * event);

    virtual void
    mousePressEvent(QMouseEvent * event);
private:
    QRectF
    closeIconRect();
};

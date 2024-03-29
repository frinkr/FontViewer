#pragma once
#include <QWidget>

class QDockWidget;

class QPaintEvent;

class QXDockTitleBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit QXDockTitleBarWidget(QWidget * parent = 0);
    ~QXDockTitleBarWidget();

    virtual QSize
    sizeHint() const;

    virtual QSize
    minimumSizeHint() const;

    virtual void
    paintEvent(QPaintEvent * event);

private slots:
    void
    onFocusChanged(QWidget * old, QWidget * now);

private:
    bool haveFocus_{};
};

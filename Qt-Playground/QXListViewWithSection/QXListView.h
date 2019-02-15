#pragma once

#include <QScrollArea>
#include <QWidget>

class QXListViewContentWidget: public QWidget {
    Q_OBJECT
public:
    using QWidget::QWidget;

    QSize
    sizeHint() const override;

    QSize
    minimumSizeHint() const override;

    void
    paintEvent(QPaintEvent * event)  override;
};

class QXListView : public QScrollArea {
    Q_OBJECT
public:
    explicit QXListView(QWidget *parent = nullptr);

signals:

public slots:
};

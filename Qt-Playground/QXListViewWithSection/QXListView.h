#pragma once

#include <QScrollArea>
#include <QWidget>

class QXListViewDataModel;

class QXListViewContentWidget: public QWidget {
    Q_OBJECT
public:
    explicit QXListViewContentWidget(QWidget * parent = nullptr);

    QSize
    sizeHint() const override;

    QSize
    minimumSizeHint() const override;

    void
    paintEvent(QPaintEvent * event)  override;

private:
    /* number of columns at current widget's width */
    int
    columnCount() const;

    /* number of rows for section at current widget's width */
    int
    rowCount(int section) const;

    /* the <section, row> pair at Y position */
    std::tuple<int, int>
    rowAt(int y) const;

    int
    sectionHeight(int section) const;

    int
    rowHeight() const;

    int
    headerHeight() const;

    QRect
    cellRect(int section, int cell) const;

private:
    QXListViewDataModel * model_;
};

class QXListViewDataModel : public QObject{
    Q_OBJECT
public:
    using QObject::QObject;

    virtual int
    sectionCount() const = 0;

    virtual int
    itemCount(int section) const = 0;

    virtual QVariant
    data(int section, int item, int role) const = 0;

    virtual QVariant
    data(int section) const = 0;
};

class QXListView : public QScrollArea {
    Q_OBJECT
public:
    explicit QXListView(QWidget *parent = nullptr);

signals:

public slots:
};

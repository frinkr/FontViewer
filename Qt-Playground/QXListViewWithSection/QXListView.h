#pragma once

#include <QScrollArea>
#include <QWidget>

class QXListViewDataModel;

struct QXListViewDataIndex {
    int section;
    int item;

    bool
    operator==(const QXListViewDataIndex & other) const {
        return section == other.section && item == other.item;
    }
    bool
    operator!=(const QXListViewDataIndex & other) const {
        return !operator==(other);
    }
};


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

    void
    mousePressEvent(QMouseEvent * event) override;

private:
    /* number of columns at current widget's width */
    int
    columnCount() const;

    /* number of rows for section at current widget's width */
    int
    rowCount(int section) const;

    /* Y location of row */
    int
    rowY(int section, int row) const;

    /* the <section, row> pair at Y position */
    std::tuple<int, int>
    rowAt(int y) const;

    /* the <section, item> pair at mouse position.
     * return <-1, -1> for click nothing.
     *        <s, -1> for clicking the section header
     *        <s, i> click section 's' at item 'i'
     */
    QXListViewDataIndex
    cellAt(const QPoint & pos) const;

    int
    sectionHeight(int section) const;

    int
    rowHeight() const;

    int
    headerHeight() const;

private:
    QXListViewDataModel * model_;
    QXListViewDataIndex   selected_;
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
    data(const QXListViewDataIndex & index, int role) const = 0;

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

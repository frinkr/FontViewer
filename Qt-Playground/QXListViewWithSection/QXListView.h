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

    virtual void
    drawCell(QPainter * painter, const QRect & rect, const QXListViewDataIndex & index, bool selected);

    virtual void
    drawHeader(QPainter * painter, const QRect & rect, int section);

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

    /* the <section, row> pair at Y position
     * return <-1, -1> empty model.
     */
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

    
    friend class QXListView;
private:
    QXListViewDataModel * model_;
    QXListViewDataIndex   selected_;

    QSize     cellSize_;
    int       cellSpace_;
    int       headerHeight_;
    int       sectionSpace_;
    int       contentMargin_;
};

class QXListViewDataModel : public QObject {
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

signals:
    void
    reset();
};


class QXListView : public QScrollArea {
    Q_OBJECT
public:
    explicit QXListView(QWidget *parent = nullptr);

    QXListViewDataModel *
    model() const;

    void
    setModel(QXListViewDataModel * model);

    void
    setCellSize(const QSize & size);

    void
    setCellSize(int size);

    const QSize &
    cellSize() const;

    void
    setCellSpace(int space);

    int
    cellSpace() const;

    void
    setSectionSpace(int space);

    int
    sectionSpace() const;

    void
    setHeaderHeight(int height);

    int
    headerHeight() const;

public slots:

private slots:
    void
    onModelReset();

private:
    QXListViewContentWidget * widget_{nullptr};
};

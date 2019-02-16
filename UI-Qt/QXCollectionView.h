#pragma once

#include <QScrollArea>
#include <QWidget>
#include "QXCollectionModel.h"

class QXCollectionViewContentWidget;

class QXCollectionViewDelegate : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void
    drawCell(
        QXCollectionViewContentWidget * view,
        QPainter * painter,
        const QRect & rect,
        const QXCollectionModelIndex & index,
        bool selected) = 0;

    virtual void
    drawHeader(
        QXCollectionViewContentWidget * view,
        QPainter * painter,
        const QRect & rect,
        int section) = 0;
};

class QXCollectionViewContentWidget: public QWidget {
    Q_OBJECT
public:
    explicit QXCollectionViewContentWidget(QWidget * parent = nullptr);

    QSize
    sizeHint() const override;

    QSize
    minimumSizeHint() const override;

    void
    paintEvent(QPaintEvent * event)  override;

    void
    mousePressEvent(QMouseEvent * event) override;

    void
    mouseMoveEvent(QMouseEvent * event);

    void
    mouseDoubleClickEvent(QMouseEvent * event) override;

    virtual void
    drawCell(QPainter * painter, const QRect & rect, const QXCollectionModelIndex & index, bool selected);

    virtual void
    drawHeader(QPainter * painter, const QRect & rect, int section);

signals:
    void
    clicked(const QXCollectionModelIndex & index);

    void
    doubleClicked(const QXCollectionModelIndex & index);

private:
    /* number of columns at current widget's width */
    int
    columnCount() const;

    /* number of rows for section at current widget's width */
    int
    rowCount(int section) const;

    /* Top position of row */
    int
    rowTop(int section, int row) const;

    /* the <section, row> pair at Y position
     * return <-1, -1> empty model.
     */
    std::tuple<int, int>
    rowAt(int y) const;

    /* the <section, row> pari at index */
    std::tuple<int, int>
    rowAt(const QXCollectionModelIndex & index) const;

    /* the <section, item> pair at mouse position.
     * return <-1, -1> for click nothing.
     *        <s, -1> for clicking the section header
     *        <s, i> click section 's' at item 'i'
     */
    QXCollectionModelIndex
    cellAt(const QPoint & pos) const;

    int
    sectionHeight(int section) const;

    int
    rowHeight() const;

    int
    headerHeight() const;

    QRect
    visualRect(const QXCollectionModelIndex & index) const;
    
    friend class QXCollectionView;
private:
    QXCollectionModel * model_ {nullptr};
    QXCollectionViewDelegate  * delegate_ {nullptr};
    QXCollectionModelIndex   selected_ {-1, -1};

    QSize     cellSize_;
    int       cellSpace_;
    int       headerHeight_;
    int       headerSpaceAbove_;
    int       contentMargin_;
};


class QXCollectionView : public QScrollArea {
    Q_OBJECT
public:
    explicit QXCollectionView(QWidget *parent = nullptr);

    QXCollectionModel *
    model() const;

    void
    setModel(QXCollectionModel * model);

    QXCollectionViewDelegate *
    delegate() const;

    void
    setDelegate(QXCollectionViewDelegate * delegate);

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

    QRect
    visualRect(const QXCollectionModelIndex & index) const;

signals:
    void
    clicked(const QXCollectionModelIndex & index);

    void
    doubleClicked(const QXCollectionModelIndex & index);

public slots:

private slots:
    void
    onModelReset();

private:
    QXCollectionViewContentWidget * widget_{nullptr};
};

#pragma once

#include <QScrollArea>
#include <QWidget>
#include "QXCollectionModel.h"

class QXCollectionViewContentWidget;

struct QXCollectionViewDrawItemOption {
    QXCollectionViewContentWidget * widget;
    QRect  rect;
    QXCollectionModelIndex index;
    bool selected;
};

struct QXCollectionViewDrawHeaderOption {
    QXCollectionViewContentWidget * widget;
    QRect  rect;
    int section;
    bool selected;
};

class QXCollectionViewDelegate : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void
    drawItem(QPainter * painter,
             const QXCollectionViewDrawItemOption & option) = 0;

    virtual void
    drawHeader(QPainter * painter,
               const QXCollectionViewDrawHeaderOption & option) = 0;
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
    drawItem(QPainter * painter,
             const QXCollectionViewDrawItemOption & option);

    virtual void
    drawHeader(QPainter * painter,
               const QXCollectionViewDrawHeaderOption & option);

signals:
    void
    clicked(const QXCollectionModelIndex & index);

    void
    doubleClicked(const QXCollectionModelIndex & index);

private:
    struct RowIndex {
        int section;
        int row;
    };
    
    /* number of columns at current widget's width */
    int
    columnCount() const;

    /* number of rows for section at current widget's width */
    int
    rowCount(int section) const;

    /* Top position of row */
    int
    rowTop(const RowIndex & index) const;

    /* the <section, row> pair at Y position
     * return <-1, -1> empty model.
     */
    RowIndex
    rowAt(int y) const;

    /* the <section, row> pari at index */
    RowIndex
    rowAt(const QXCollectionModelIndex & index) const;

    /* the <section, item> pair at mouse position.
     * return <-1, -1> for click nothing.
     *        <s, -1> for clicking the section header
     *        <s, i> click section 's' at item 'i'
     */
    QXCollectionModelIndex
    itemAt(const QPoint & pos) const;

    int
    sectionHeight(int section) const;

    int
    rowHeight() const;

    int
    headerHeight() const;

    QRect
    itemRect(const QXCollectionModelIndex & index) const;
    
    friend class QXCollectionView;
private:
    QXCollectionModel * model_ {nullptr};
    QXCollectionViewDelegate  * delegate_ {nullptr};
    QXCollectionModelIndex   selected_ {-1, -1};

    QSize     itemSize_;
    QSize     itemSpace_;
    int       headerSize_;               // font size of header text
    int       sectionSpace_;            // space fron header text to last row of previous section
    int       sectionInterSpace_;
    QMargins  contentMargins_;
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
    itemRect(const QXCollectionModelIndex & index) const;

signals:
    void
    clicked(const QXCollectionModelIndex & index);

    void
    doubleClicked(const QXCollectionModelIndex & index);

public slots:

private slots:
    void
    onModelReset();

    void
    onBeginResetModel();

    void
    onEndResetModel();
private:
    QXCollectionViewContentWidget * widget_{nullptr};
};

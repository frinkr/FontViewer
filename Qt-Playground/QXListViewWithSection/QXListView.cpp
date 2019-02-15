#include <QScrollArea>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include "QXListView.h"
#include <QRandomGenerator>

namespace {
    constexpr qreal headerSpaceBelow_ = 8;
    constexpr int minimumColumnCount_ = 1;

    class QXDummyModel: public QXListViewDataModel {
    public:
        using QXListViewDataModel::QXListViewDataModel;
        int
        sectionCount() const override {
            return 20;
        }

        int
        itemCount(int section) const override {
            return std::pow(2 + sectionCount() / 2 - std::abs(sectionCount() / 2 - section), 2);
        }

        QVariant
        data(const QXListViewDataIndex & index, int role) const override {
            return QString("%1, %2").arg(index.section).arg(index.item);
        }

        QVariant
        data(int section) const override {
            return QString("Section %1").arg(section);
        }
    };

    class QXListViewEmptyDataModel : public QXListViewDataModel {
        using QXListViewDataModel::QXListViewDataModel;
        int
        sectionCount() const override {
            return 0;
        }

        int
        itemCount(int section) const override {
            return 0;
        }

        QVariant
        data(const QXListViewDataIndex & index, int role) const override {
            return QVariant();
        }

        QVariant
        data(int section) const override {
            return QVariant();
        }
    };
}


QXListViewContentWidget::QXListViewContentWidget(QWidget * parent)
    : QWidget(parent)
    , model_(new QXListViewEmptyDataModel(this))
    , selected_{-1, -1}
    , cellSize_(80, 100)
    , cellSpace_(20)
    , headerHeight_{20}
    , sectionSpace_{40}
    , contentMargin_{5} {
}

int
QXListViewContentWidget::columnCount() const {
    return (rect().width() - 2 * contentMargin_ + cellSpace_) / (cellSize_.width() + cellSpace_);
}

int
QXListViewContentWidget::rowCount(int section) const {
    int count = model_->itemCount(section);
    return count / columnCount() + (count % columnCount()? 1: 0);
}

std::tuple<int, int>
QXListViewContentWidget::rowAt(int y) const {
    int height = contentMargin_;
    for (int s = 0; s < model_->sectionCount(); ++ s) {
        int newHeight = height + sectionHeight(s);
        if (y <= newHeight) {
            int rowY = height + headerHeight();
            for (int r = 0; r < rowCount(s); ++ r) {
                rowY += cellSize_.height() + (r?cellSpace_: 0);
                if (y <= rowY)
                    return std::make_tuple(s, r);
            }
            return std::make_tuple(s, rowCount(s) - 1);
        }
        height = newHeight;
    }

    // Return last row of last section
    return std::make_tuple(model_->sectionCount() - 1,
                           rowCount(model_->sectionCount() - 1) - 1);
}

int
QXListViewContentWidget::rowY(int section, int row) const {
    int height = contentMargin_;
    for (int s = 0; s < section; ++ s)
        height += sectionHeight(s);
    height += headerHeight();
    for (int r = 0; r <= row; ++ r)
        height += cellSize_.height() + (r?cellSpace_: 0);
    return height;
}

QXListViewDataIndex
QXListViewContentWidget::cellAt(const QPoint & pos) const {
    int section, row;
    std::tie(section, row) = rowAt(pos.y());

    int yMax = rowY(section, row);
    int yMin = yMax - rowHeight();

    if (pos.y() < yMin) {
        // header
        return {section, -1};
    }
    else {
        int columns = columnCount();
        int cellEnd = columns;
        if ((row + 1) * columns > model_->itemCount(row))
            cellEnd = model_->itemCount(section) - row * columns;

        int x = contentMargin_;
        for (int c = 0; c < cellEnd; ++ c) {
            if (pos.x() >= x && pos.x() <= (x + cellSize_.width())) {
                int itemIndex = row * columns + c;
                return {section, itemIndex};
            }
            x += (cellSize_.width() + cellSpace_);
        }
    }
    return {-1, -1};
}

int
QXListViewContentWidget::sectionHeight(int section) const {
    return rowCount(section) * rowHeight() - cellSpace_ + headerHeight();
}

int
QXListViewContentWidget::rowHeight() const {
    return cellSize_.height() + cellSpace_;
}

int
QXListViewContentWidget::headerHeight() const {
    return headerHeight_ + sectionSpace_ + headerSpaceBelow_;
}

QSize
QXListViewContentWidget::sizeHint() const {
    return minimumSizeHint();
}

QSize
QXListViewContentWidget::minimumSizeHint() const {
    int height = 0;
    for (int i = 0; i < model_->sectionCount(); ++ i) 
        height += sectionHeight(i);
        
    return QSize(minimumColumnCount_ * (cellSize_.width() + cellSpace_) + 2 * contentMargin_ - cellSpace_,
                 height + 2 * contentMargin_);
}

void
QXListViewContentWidget::paintEvent(QPaintEvent * event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), palette().base());

    int columns = columnCount();
    int beginSection, beginRow, endSection, endRow;

    std::tie(beginSection, beginRow) = rowAt(event->rect().top());
    std::tie(endSection, endRow) = rowAt(event->rect().bottom());

    if (beginSection == -1 || endSection == -1)
        return ;

    int y = contentMargin_;
    for (int s = 0; s < beginSection; ++ s)
        y += sectionHeight(s);
    y += headerHeight();

    Qt::GlobalColor colors[] = {Qt::cyan, Qt::yellow, Qt::magenta, Qt::darkGreen, Qt::darkBlue, Qt::red, Qt::green, Qt::blue, };

    int updatedCellCount = 0;
    for (int s = beginSection; s <= endSection; ++ s) {
        auto bg = QColor(colors[s % (sizeof(colors) / sizeof(colors[0]))]).toRgb();
        auto fg = QColor(255 - bg.red(), 255 - bg.green(), 255 - bg.blue());
        painter.setPen(bg);
        QRect sectionHeaderRect(contentMargin_,
                                y - headerSpaceBelow_ - headerHeight_,
                                width() - 2 * contentMargin_,
                                headerHeight_);

        painter.drawText(sectionHeaderRect, Qt::AlignCenter, model_->data(s).toString());
        painter.setPen(fg);

        int r0 = 0, r1 = rowCount(s) - 1;
        if (s == beginSection) {
            r0 = beginRow;
            if (beginSection == endSection)
                r1 = endRow;
        }
        if (s == endSection) {
            if (beginSection == endSection)
                r0 = beginRow;
            r1 = endRow;
        }

        int rowY = y;
        for (int r = 0; r < r0; ++ r)
            rowY += rowHeight();

        for (int r = r0; r <= r1; ++ r) {
            int cEnd = columns;
            if ((r + 1) * columns > model_->itemCount(s))
                cEnd = model_->itemCount(s) - r * columns;

            for (int c = 0; c < cEnd; ++ c) {
                QPoint leftTop(contentMargin_ + c * (cellSize_.width() + cellSpace_), rowY);
                QRect cellRect(leftTop, cellSize_);

                QXListViewDataIndex dataIndex = {s, r * columns + c};
                int expand = 10;
                if (selected_ == dataIndex)
                    painter.fillRect(cellRect.adjusted(-expand, -expand, expand, expand), bg);
                else
                    painter.fillRect(cellRect, bg);
                painter.drawText(cellRect, Qt::AlignHCenter | Qt::AlignVCenter, model_->data(dataIndex, 0).toString());
                ++ updatedCellCount;
            }
            rowY += rowHeight();
        }

        y += sectionHeight(s);
    }

    qWarning("QXListViewContentWidget::update => rect: (%d, %d, %d, %d), beginSection: %d, %d, endSection: %d, %d, total updated cells: %d",
             event->rect().left(), event->rect().top(), event->rect().right(), event->rect().bottom(),
             beginSection, beginRow, endSection, endRow, updatedCellCount);
}

void
QXListViewContentWidget::mousePressEvent(QMouseEvent * event) {
    selected_ = cellAt(event->pos());
    update();
}


QXListView::QXListView(QWidget *parent)
    : QScrollArea(parent) {
    this->setWidgetResizable(false);

    widget_ = new QXListViewContentWidget(this);
    this->setWidget(widget_);
    setModel(new QXDummyModel(this));
}


QXListViewDataModel *
QXListView::model() const {
    return qobject_cast<QXListViewContentWidget*>(widget())->model_;
}

void
QXListView::setModel(QXListViewDataModel * model) {
    widget_->model_ = model;
    connect(model, &QXListViewDataModel::reset, this, &QXListView::onModelReset);
    emit model->reset();
}

void
QXListView::onModelReset() {
    widget()->update();
}

void
QXListView::setCellSize(const QSize & size) {
    widget_->cellSize_ = size;
}

void
QXListView::setCellSize(int size) {
    widget_->cellSize_ = QSize(size, size);
}

const QSize &
QXListView::cellSize() const {
    return widget_->cellSize_;
}

void
QXListView::setCellSpace(int space) {
    widget_->cellSpace_ = space;
}

int
QXListView::cellSpace() const {
    return widget_->cellSpace_;
}

void
QXListView::setSectionSpace(int space) {
    widget_->sectionSpace_ = space;
}

int
QXListView::sectionSpace() const {
    return widget_->sectionSpace_;
}

void
QXListView::setHeaderHeight(int height) {
    widget_->headerHeight_ = height;
}

int
QXListView::headerHeight() const {
    return widget_->headerHeight_;
}

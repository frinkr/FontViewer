#include <QScrollArea>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include "QXListView.h"
#include <QRandomGenerator>

namespace {
    constexpr qreal cellWidth_ = 80;
    constexpr qreal cellHeight_ = 80;
    constexpr qreal cellSpace_ = 20;
    constexpr qreal headerHeight_ = 20;
    constexpr qreal headerSpaceAbove_ = 12;
    constexpr qreal headerSpaceBelow_ = 8;
    constexpr qreal contentMargin_ = 20;

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
            return std::pow(2 + sectionCount() / 2 - std::abs(sectionCount() / 2 - section), 4);
        }

        QVariant
        data(int section, int item, int role) const override {
            return QString("%1, %2").arg(section).arg(item);
        }

        QVariant
        data(int section) const override {
            return QString("Section %1").arg(section);
        }
    };
}


QXListViewContentWidget::QXListViewContentWidget(QWidget * parent)
    : QWidget(parent) {
    model_ = new QXDummyModel(this);
}

int
QXListViewContentWidget::columnCount() const {
    return (rect().width() - 2 * contentMargin_ + cellSpace_) / (cellWidth_ + cellSpace_);
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
                rowY += cellHeight_ + (r?cellSpace_: 0);
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
        height += cellHeight_ + (r?cellSpace_: 0);
    return height;
}

std::tuple<int, int>
QXListViewContentWidget::cellAt(const QPoint & pos) const {
    int section, row;
    std::tie(section, row) = rowAt(pos.y());

    int yMax = rowY(section, row);
    int yMin = yMax - rowHeight();

    if (pos.y() < yMin) {
        // header
        return std::make_tuple(section, -1);
    }
    else {
        int columns = columnCount();
        int cellEnd = columns;
        if ((row + 1) * columns > model_->itemCount(row))
            cellEnd = model_->itemCount(section) - row * columns;

        int x = contentMargin_;
        for (int c = 0; c < cellEnd; ++ c) {
            if (pos.x() >= x && pos.x() <= (x + cellWidth_)) {
                int itemIndex = row * columns + c;
                return std::make_tuple(section, itemIndex);
            }
            x += (cellWidth_ + cellSpace_);
        }
    }
    return std::make_tuple(-1, -1);
}

int
QXListViewContentWidget::sectionHeight(int section) const {
    return rowCount(section) * rowHeight() - cellSpace_ + headerHeight();
}

int
QXListViewContentWidget::rowHeight() const {
    return cellHeight_ + cellSpace_;
}

int
QXListViewContentWidget::headerHeight() const {
    return headerHeight_ + headerSpaceAbove_ + headerSpaceBelow_;
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
        
    return QSize(minimumColumnCount_ * (cellWidth_ + cellSpace_) + 2 * contentMargin_ - cellSpace_,
                 height + 2 * contentMargin_);
}

void
QXListViewContentWidget::paintEvent(QPaintEvent * event) {
    QPainter painter(this);
    painter.setPen(Qt::white);

    painter.setBrush(Qt::red);

    painter.fillRect(event->rect(), palette().base());

    int columns = columnCount();
    int beginSection, beginRow, endSection, endRow;

    std::tie(beginSection, beginRow) = rowAt(event->rect().top());
    std::tie(endSection, endRow) = rowAt(event->rect().bottom());

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
        painter.drawText(QPoint(contentMargin_, y - headerSpaceBelow_), model_->data(s).toString());
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
                QPoint leftTop(contentMargin_ + c * (cellWidth_ + cellSpace_), rowY);
                QRect cellRect(leftTop, QSize(cellWidth_, cellHeight_));

                int itemIndex = r * columns + c;
                int expand = 10;
                if (selected_.index == itemIndex && selected_.section == s)
                    painter.fillRect(cellRect.adjusted(-expand, -expand, expand, expand), bg);
                else
                    painter.fillRect(cellRect, bg);
                painter.drawText(cellRect, Qt::AlignHCenter | Qt::AlignVCenter, model_->data(s, itemIndex, 0).toString());
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
    auto pos = event->pos();
    int section, item;
    std::tie(section, item) = cellAt(pos);
    selected_.index = item;
    selected_.section = section;
    update();
}


QXListView::QXListView(QWidget *parent)
    : QScrollArea(parent) {

    this->setWidgetResizable(false);

    this->setWidget(new QXListViewContentWidget);
}

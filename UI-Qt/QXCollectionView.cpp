#include <QScrollArea>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include "QXCollectionView.h"
#include <QRandomGenerator>

namespace {
    constexpr qreal headerSpaceBelow_ = 10;
    constexpr int minimumColumnCount_ = 1;

    class QXDummyModel: public QXCollectionViewDataModel {
    public:
        using QXCollectionViewDataModel::QXCollectionViewDataModel;
        int
        sectionCount() const override {
            return 20;
        }

        int
        itemCount(int section) const override {
            return std::pow(2 + sectionCount() / 2 - std::abs(sectionCount() / 2 - section), 2);
        }

        QVariant
        data(const QXCollectionViewDataIndex & index, int role) const override {
            return QString("%1, %2").arg(index.section).arg(index.item);
        }

        QVariant
        data(int section) const override {
            return QString("Section %1").arg(section);
        }
    };

    class QXCollectionViewEmptyDataModel : public QXCollectionViewDataModel {
        using QXCollectionViewDataModel::QXCollectionViewDataModel;
        int
        sectionCount() const override {
            return 0;
        }

        int
        itemCount(int section) const override {
            return 0;
        }

        QVariant
        data(const QXCollectionViewDataIndex & index, int role) const override {
            return QVariant();
        }

        QVariant
        data(int section) const override {
            return QVariant();
        }
    };

    Qt::GlobalColor colors[] = {Qt::cyan, Qt::yellow, Qt::magenta, Qt::darkGreen, Qt::darkBlue, Qt::red, Qt::green, Qt::blue, };
}


QXCollectionViewContentWidget::QXCollectionViewContentWidget(QWidget * parent)
    : QWidget(parent)
    , selected_{-1, -1}
    , cellSize_(80, 80)
    , cellSpace_(20)
    , headerHeight_{25}
    , sectionSpace_{40}
    , contentMargin_{5} {
        model_ = new QXDummyModel(this);
      }

int
QXCollectionViewContentWidget::columnCount() const {
    return (rect().width() - 2 * contentMargin_ + cellSpace_) / (cellSize_.width() + cellSpace_);
}

int
QXCollectionViewContentWidget::rowCount(int section) const {
    int count = model_->itemCount(section);
    return count / columnCount() + (count % columnCount()? 1: 0);
}

std::tuple<int, int>
QXCollectionViewContentWidget::rowAt(int y) const {
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
QXCollectionViewContentWidget::rowY(int section, int row) const {
    int height = contentMargin_;
    for (int s = 0; s < section; ++ s)
        height += sectionHeight(s);
    height += headerHeight();
    for (int r = 0; r <= row; ++ r)
        height += cellSize_.height() + (r?cellSpace_: 0);
    return height;
}

QXCollectionViewDataIndex
QXCollectionViewContentWidget::cellAt(const QPoint & pos) const {
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
QXCollectionViewContentWidget::sectionHeight(int section) const {
    return rowCount(section) * rowHeight() - cellSpace_ + headerHeight();
}

int
QXCollectionViewContentWidget::rowHeight() const {
    return cellSize_.height() + cellSpace_;
}

int
QXCollectionViewContentWidget::headerHeight() const {
    return headerHeight_ + sectionSpace_ + headerSpaceBelow_;
}

QSize
QXCollectionViewContentWidget::sizeHint() const {
    return minimumSizeHint();
}

QSize
QXCollectionViewContentWidget::minimumSizeHint() const {
    int height = 0;
    for (int i = 0; i < model_->sectionCount(); ++ i) 
        height += sectionHeight(i);
        
    return QSize(minimumColumnCount_ * (cellSize_.width() + cellSpace_) + 2 * contentMargin_ - cellSpace_,
                 height + 2 * contentMargin_);
}

void
QXCollectionViewContentWidget::paintEvent(QPaintEvent * event) {
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


    int updatedCellCount = 0;
    for (int s = beginSection; s <= endSection; ++ s) {
        auto bg = QColor(colors[s % (sizeof(colors) / sizeof(colors[0]))]).toRgb();
        auto fg = QColor(255 - bg.red(), 255 - bg.green(), 255 - bg.blue());
        QRect sectionHeaderRect(contentMargin_,
                                y - headerSpaceBelow_ - headerHeight_,
                                width() - 2 * contentMargin_,
                                headerHeight_);
        drawHeader(&painter, sectionHeaderRect, s);
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
                QXCollectionViewDataIndex dataIndex = {s, r * columns + c};
                drawCell(&painter, cellRect, dataIndex, selected_ == dataIndex);

                ++ updatedCellCount;
            }
            rowY += rowHeight();
        }

        y += sectionHeight(s);
    }

    qWarning("QXCollectionViewContentWidget::update => rect: (%d, %d, %d, %d), beginSection: %d, %d, endSection: %d, %d, total updated cells: %d",
             event->rect().left(), event->rect().top(), event->rect().right(), event->rect().bottom(),
             beginSection, beginRow, endSection, endRow, updatedCellCount);
}

void
QXCollectionViewContentWidget::drawCell(QPainter * painter, const QRect & rect, const QXCollectionViewDataIndex & index, bool selected) {
    auto bg = QColor(colors[index.section % (sizeof(colors) / sizeof(colors[0]))]).toRgb();
    int expand = 10;
    if (selected)
        painter->fillRect(rect.adjusted(-expand, -expand, expand, expand), bg);
    else
        painter->fillRect(rect, bg);
    painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, model_->data(index, 0).toString());
}

void
QXCollectionViewContentWidget::drawHeader(QPainter * painter, const QRect & rect, int section) {
    auto bg = QColor(colors[section % (sizeof(colors) / sizeof(colors[0]))]).toRgb();
    auto fg = QColor(255 - bg.red(), 255 - bg.green(), 255 - bg.blue());
    painter->setPen(bg);
    QFont font = painter->font();
    font.setPixelSize(rect.height() - 2);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, model_->data(section).toString());
}

void
QXCollectionViewContentWidget::mousePressEvent(QMouseEvent * event) {
    selected_ = cellAt(event->pos());
    update();
}


QXCollectionView::QXCollectionView(QWidget *parent)
    : QScrollArea(parent) {
    this->setWidgetResizable(false);

    widget_ = new QXCollectionViewContentWidget();
    this->setWidget(widget_);
    //setModel(new QXDummyModel(this));
}


QXCollectionViewDataModel *
QXCollectionView::model() const {
    return qobject_cast<QXCollectionViewContentWidget*>(widget())->model_;
}

void
QXCollectionView::setModel(QXCollectionViewDataModel * model) {
    widget_->model_ = model;
    connect(model, &QXCollectionViewDataModel::reset, this, &QXCollectionView::onModelReset);
    //emit model->reset();
}

void
QXCollectionView::onModelReset() {
    widget()->update();
}

void
QXCollectionView::setCellSize(const QSize & size) {
    widget_->cellSize_ = size;
}

void
QXCollectionView::setCellSize(int size) {
    widget_->cellSize_ = QSize(size, size);
}

const QSize &
QXCollectionView::cellSize() const {
    return widget_->cellSize_;
}

void
QXCollectionView::setCellSpace(int space) {
    widget_->cellSpace_ = space;
}

int
QXCollectionView::cellSpace() const {
    return widget_->cellSpace_;
}

void
QXCollectionView::setSectionSpace(int space) {
    widget_->sectionSpace_ = space;
}

int
QXCollectionView::sectionSpace() const {
    return widget_->sectionSpace_;
}

void
QXCollectionView::setHeaderHeight(int height) {
    widget_->headerHeight_ = height;
}

int
QXCollectionView::headerHeight() const {
    return widget_->headerHeight_;
}

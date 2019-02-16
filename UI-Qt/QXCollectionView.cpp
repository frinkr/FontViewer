#include <QScrollArea>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include "QXCollectionView.h"
#include <QRandomGenerator>

namespace {
    constexpr qreal headerSpaceBelow_ = 10;
    constexpr int minimumColumnCount_ = 1;

    class QXDummyModel: public QXCollectionModel {
    public:
        using QXCollectionModel::QXCollectionModel;
        int
        sectionCount() const override {
            return 20;
        }

        int
        itemCount(int section) const override {
            return std::pow(2 + sectionCount() / 2 - std::abs(sectionCount() / 2 - section), 2);
        }

        QVariant
        data(const QXCollectionModelIndex & index, int role) const override {
            return QString("%1, %2").arg(index.section).arg(index.item);
        }

        QVariant
        data(int section) const override {
            return QString("Section %1").arg(section);
        }
    };

    class QXCollectionViewEmptyDataModel : public QXCollectionModel {
        using QXCollectionModel::QXCollectionModel;
        int
        sectionCount() const override {
            return 0;
        }

        int
        itemCount(int section) const override {
            return 0;
        }

        QVariant
        data(const QXCollectionModelIndex & index, int role) const override {
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
    , headerSpaceAbove_{40}
    , contentMargin_{5}
{
    model_ = new QXDummyModel(this);
    setMouseTracking(false);
}

int
QXCollectionViewContentWidget::columnCount() const {
    return std::max(1, (rect().width() - 2 * contentMargin_ + cellSpace_) / (cellSize_.width() + cellSpace_));
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

std::tuple<int, int>
QXCollectionViewContentWidget::rowAt(const QXCollectionModelIndex & index) const {
    return std::make_tuple(index.section, index.item / columnCount());
}

int
QXCollectionViewContentWidget::rowTop(int section, int row) const {
    int height = contentMargin_;
    for (int s = 0; s < section; ++ s)
        height += sectionHeight(s);
    height += headerHeight();
    for (int r = 0; r < row; ++ r)
        height += rowHeight();
    return height;
}

QXCollectionModelIndex
QXCollectionViewContentWidget::cellAt(const QPoint & pos) const {
    int section, row;
    std::tie(section, row) = rowAt(pos.y());

    int yMin = rowTop(section, row);
    int yMax = yMin + rowHeight();

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

QRect
QXCollectionViewContentWidget::visualRect(const QXCollectionModelIndex & index) const {
    int section, row;
    std::tie(section, row) = rowAt(index);
    int top = rowTop(section, row);
    int left = contentMargin_;
    for (int i = 0; i < index.item % columnCount(); ++ i) 
        left += cellSize_.width() + cellSpace_;
    return QRect(QPoint(left, top), cellSize_);
}

int
QXCollectionViewContentWidget::sectionHeight(int section) const {
    return rowCount(section) * rowHeight() - cellSpace_ + headerHeight() - (section? 0: headerSpaceAbove_);
}

int
QXCollectionViewContentWidget::rowHeight() const {
    return cellSize_.height() + cellSpace_;
}

int
QXCollectionViewContentWidget::headerHeight() const {
    return headerHeight_ + headerSpaceAbove_ + headerSpaceBelow_;
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

    int sectionTop = contentMargin_;
    for (int s = 0; s < beginSection; ++ s)
        sectionTop += sectionHeight(s);

    int updatedCellCount = 0;
    for (int s = beginSection; s <= endSection; ++ s) {
        QRect sectionHeaderRect(contentMargin_,
                                sectionTop + headerSpaceAbove_,
                                width() - 2 * contentMargin_,
                                headerHeight_);
        drawHeader(&painter, sectionHeaderRect, s);

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

        int rowTop = sectionTop + headerHeight();;
        for (int r = 0; r < r0; ++ r)
            rowTop += rowHeight();

        for (int r = r0; r <= r1; ++ r) {
            int cEnd = columns;
            if ((r + 1) * columns > model_->itemCount(s))
                cEnd = model_->itemCount(s) - r * columns;

            for (int c = 0; c < cEnd; ++ c) {
                QPoint leftTop(contentMargin_ + c * (cellSize_.width() + cellSpace_), rowTop);
                QRect cellRect(leftTop, cellSize_);
                QXCollectionModelIndex dataIndex = {s, r * columns + c};
                drawCell(&painter, cellRect, dataIndex, selected_ == dataIndex);

                ++ updatedCellCount;
            }
            rowTop += rowHeight();
        }

        sectionTop += sectionHeight(s); // TODO: or sectionTop = rowTop?
    }

    qWarning("QXCollectionViewContentWidget::update => rect: (%d, %d, %d, %d), beginSection: %d, %d, endSection: %d, %d, total updated cells: %d",
             event->rect().left(), event->rect().top(), event->rect().right(), event->rect().bottom(),
             beginSection, beginRow, endSection, endRow, updatedCellCount);
}

void
QXCollectionViewContentWidget::drawCell(QPainter * painter, const QRect & rect, const QXCollectionModelIndex & index, bool selected) {
    if (delegate_) 
        return delegate_->drawCell(this, painter, rect, index, selected);

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
    if (delegate_)
        return delegate_->drawHeader(this, painter, rect, section);

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
    emit clicked(selected_);
}

void
QXCollectionViewContentWidget::mouseMoveEvent(QMouseEvent * event) {
    selected_ = cellAt(event->pos());
    update();
    emit clicked(selected_);
}

void
QXCollectionViewContentWidget::mouseDoubleClickEvent(QMouseEvent * event) {
    emit doubleClicked(selected_);
}


///////////////////////////////////////////////////////////////////

QXCollectionView::QXCollectionView(QWidget *parent)
    : QScrollArea(parent) {
    this->setWidgetResizable(false);

    widget_ = new QXCollectionViewContentWidget();
    this->setWidget(widget_);

    connect(widget_, &QXCollectionViewContentWidget::clicked,
            this, &QXCollectionView::clicked);

    connect(widget_, &QXCollectionViewContentWidget::doubleClicked,
            this, &QXCollectionView::doubleClicked);
}


QXCollectionModel *
QXCollectionView::model() const {
    return qobject_cast<QXCollectionViewContentWidget*>(widget())->model_;
}

void
QXCollectionView::setModel(QXCollectionModel * model) {
    if (widget_->model_) 
        widget_->model_->disconnect(this);
        
    widget_->model_ = model;
    model->setParent(widget_);
    connect(model, &QXCollectionModel::reset, this, &QXCollectionView::onModelReset);
}

QXCollectionViewDelegate *
QXCollectionView::delegate() const {
    return widget_->delegate_;
}

void
QXCollectionView::setDelegate(QXCollectionViewDelegate * delegate) {
    widget_->delegate_ = delegate;
    delegate->setParent(widget_);
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
    widget_->headerSpaceAbove_ = space;
}

int
QXCollectionView::sectionSpace() const {
    return widget_->headerSpaceAbove_;
}

void
QXCollectionView::setHeaderHeight(int height) {
    widget_->headerHeight_ = height;
}


int
QXCollectionView::headerHeight() const {
    return widget_->headerHeight_;
}

QRect
QXCollectionView::visualRect(const QXCollectionModelIndex & index) const {
    QRect rect = widget_->visualRect(index);
    return QRect(widget_->mapToParent(rect.topLeft()),
                 widget_->mapToParent(rect.bottomRight()));
}

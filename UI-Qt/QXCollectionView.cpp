#include <QCoreApplication>
#include <QScrollArea>
#include <QPaintEvent>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include "QXCollectionView.h"
#include <QRandomGenerator>

namespace {
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
    , itemSize_(80, 80)
    , itemSpace_(20, 20)
    , headerSize_{25}
    , sectionSpace_{40}
    , sectionInterSpace_{10}
    , contentMargins_{15, 0, 15, 15}
{
    model_ = new QXDummyModel(this);
    setMouseTracking(false);
    setCursor(Qt::ArrowCursor);
    setFocusPolicy(Qt::StrongFocus);
}

int
QXCollectionViewContentWidget::columnCount() const {
    return std::max(1, (rect().width() - contentMargins_.left() - contentMargins_.right() + itemSpace_.width()) / (itemSize_.width() + itemSpace_.width()));
}

int
QXCollectionViewContentWidget::rowCount(int section) const {
    int count = model_->itemCount(section);
    return count / columnCount() + (count % columnCount()? 1: 0);
}

QXCollectionViewContentWidget::RowIndex
QXCollectionViewContentWidget::rowAt(int y) const {
    int height = contentMargins_.top();
    for (int s = 0; s < model_->sectionCount(); ++ s) {
        int newHeight = height + sectionHeight(s);
        if (y <= newHeight) {
            int rowBottom = height + headerHeight();
            for (int r = 0; r < rowCount(s); ++ r) {
                rowBottom += rowHeight();
                if (y <= rowBottom)
                    return { s, r };
            }
            return { s, rowCount(s) - 1 };
        }
        height = newHeight;
    }

    // Return last row of last section
    return { model_->sectionCount() - 1,
             rowCount(model_->sectionCount() - 1) - 1 };
}

QXCollectionViewContentWidget::RowIndex
QXCollectionViewContentWidget::rowAt(const QXCollectionModelIndex & index) const {
    return { index.section, index.item / columnCount() };
}

int
QXCollectionViewContentWidget::colAt(int x) const {
    return (x - contentMargins_.left()) / (itemSize_.width() + itemSpace_.width());
}

int
QXCollectionViewContentWidget::rowTop(const RowIndex & index) const {
    int height = contentMargins_.top();
    for (int s = 0; s < index.section; ++ s)
        height += sectionHeight(s);
    height += headerHeight();
    for (int r = 0; r < index.row; ++ r)
        height += rowHeight();
    return height;
}

QXCollectionModelIndex
QXCollectionViewContentWidget::itemAt(const QPoint & pos) const {
    RowIndex rowIndex = rowAt(pos.y());

    int yMin = rowTop(rowIndex);
    int yMax = yMin + rowHeight();

    if (pos.y() < yMin) {
        // header
        return { rowIndex.section, -1};
    }
    else {
        int columns = columnCount();
        int cellEnd = columns;
        if ((rowIndex.row + 1) * columns > model_->itemCount(rowIndex.section))
            cellEnd = model_->itemCount(rowIndex.section) - rowIndex.row * columns;

        int x = contentMargins_.left();
        for (int c = 0; c < cellEnd; ++ c) {
            if (pos.x() >= x && pos.x() <= (x + itemSize_.width())) {
                int itemIndex = rowIndex.row * columns + c;
                return { rowIndex.section, itemIndex};
            }
            x += (itemSize_.width() + itemSpace_.width());
        }
    }
    return {-1, -1};
}

QRect
QXCollectionViewContentWidget::itemRect(const QXCollectionModelIndex & index) const {
    int top = rowTop(rowAt(index));
    int left = contentMargins_.left();
    for (int i = 0; i < index.item % columnCount(); ++ i) 
        left += itemSize_.width() + itemSpace_.width();
    return QRect(QPoint(left, top), itemSize_ + QSize(1, 1));
}

void
QXCollectionViewContentWidget::select(const QXCollectionModelIndex & index) {
    if (selected_.section != -1)
        update(itemRect(selected_));
    selected_ = index;
    update(itemRect(index));
}

int
QXCollectionViewContentWidget::sectionHeight(int section) const {
    return rowCount(section) * rowHeight() + headerHeight();
}

int
QXCollectionViewContentWidget::rowHeight() const {
    return itemSize_.height() + itemSpace_.height();
}

int
QXCollectionViewContentWidget::headerHeight() const {
    return headerSize_ + sectionSpace_ + sectionInterSpace_;
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
        
    return QSize(minimumColumnCount_ * (itemSize_.width() + itemSpace_.width()) + contentMargins_.left() + contentMargins_.right() - itemSpace_.width(),
                 height + contentMargins_.top() + contentMargins_.bottom());
}

void
QXCollectionViewContentWidget::paintEvent(QPaintEvent * event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), palette().base());

    const int columns = columnCount();

    const RowIndex beginRowIndex = rowAt(event->rect().top());
    const RowIndex endRowIndex = rowAt(event->rect().bottom());
    const int beginCol = colAt(event->rect().left());
    const int endCol = colAt(event->rect().right());

    if (beginRowIndex.section == -1 || endRowIndex.section == -1)
        return ;

    int sectionTop = contentMargins_.top();
    for (int s = 0; s < beginRowIndex.section; ++ s)
        sectionTop += sectionHeight(s);

    int updatedCellCount = 0;
    for (int s = beginRowIndex.section; s <= endRowIndex.section; ++ s) {
        QRect sectionHeaderRect(contentMargins_.left(),
                                sectionTop + sectionSpace_,
                                width() - contentMargins_.left() - contentMargins_.right(),
                                headerSize_);

        QXCollectionViewDrawHeaderOption option;
        option.widget = this;
        option.rect = sectionHeaderRect;
        option.section = s;
        option.selected = (selected_.item == -1 && selected_.section == s);

        drawHeader(&painter, option);

        int r0 = 0, r1 = rowCount(s) - 1;
        if (s == beginRowIndex.section) {
            r0 = beginRowIndex.row;
            if (beginRowIndex.section == endRowIndex.section)
                r1 = endRowIndex.row;
        }
        if (s == endRowIndex.section) {
            if (beginRowIndex.section == endRowIndex.section)
                r0 = beginRowIndex.row;
            r1 = endRowIndex.row;
        }

        int rowTop = sectionTop + headerHeight();
        for (int r = 0; r < r0; ++ r)
            rowTop += rowHeight();

        for (int r = r0; r <= r1; ++ r) {
            int cEnd = columns;
            if ((r + 1) * columns > model_->itemCount(s))
                cEnd = model_->itemCount(s) - r * columns;

            for (int c = std::max(0, beginCol); c < std::min(cEnd, endCol + 1); ++ c) {
                QPoint leftTop(contentMargins_.left() + c * (itemSize_.width() + itemSpace_.width()), rowTop);
                QRect cellRect(leftTop, itemSize_);
                QXCollectionModelIndex dataIndex = {s, r * columns + c};

                QXCollectionViewDrawItemOption option;
                option.widget = this;
                option.rect = cellRect;
                option.selected = selected_ == dataIndex;
                option.index = dataIndex;

                drawItem(&painter, option);

                ++ updatedCellCount;
            }
            rowTop += rowHeight();
        }

        sectionTop += sectionHeight(s); // TODO: or sectionTop = rowTop?
        sectionTop = rowTop;
    }

    qWarning("QXCollectionViewContentWidget::update => rect: (%d, %d, %d, %d), beginRow: %d, %d, endRow: %d, %d, total updated cells: %d",
             event->rect().left(), event->rect().top(), event->rect().right(), event->rect().bottom(),
        beginRowIndex.section, beginRowIndex.row, endRowIndex.section, endRowIndex.row, updatedCellCount);
}

void
QXCollectionViewContentWidget::drawItem(QPainter * painter, const QXCollectionViewDrawItemOption & option) {
    if (delegate_) 
        return delegate_->drawItem(painter, option);

    auto bg = QColor(colors[option.index.section % (sizeof(colors) / sizeof(colors[0]))]).toRgb();
    int expand = 10;
    if (option.selected)
        painter->fillRect(option.rect.adjusted(-expand, -expand, expand, expand), bg);
    else
        painter->fillRect(option.rect, bg);
    painter->drawText(option.rect, Qt::AlignHCenter | Qt::AlignVCenter, model_->data(option.index, 0).toString());
}

void
QXCollectionViewContentWidget::drawHeader(QPainter * painter, const QXCollectionViewDrawHeaderOption & option) {
    if (delegate_)
        return delegate_->drawHeader(painter, option);

    auto bg = QColor(colors[option.section % (sizeof(colors) / sizeof(colors[0]))]).toRgb();
    painter->setPen(bg);
    QFont font = painter->font();
    font.setPixelSize(option.rect.height() - 2);
    painter->setFont(font);
    painter->drawText(option.rect, Qt::AlignCenter, model_->data(option.section).toString());
}

void
QXCollectionViewContentWidget::mousePressEvent(QMouseEvent * event) {
    select(itemAt(event->pos()));
    emit clicked(selected_);
}

void
QXCollectionViewContentWidget::mouseMoveEvent(QMouseEvent * event) {
    select(itemAt(event->pos()));
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

    widget_->setAcceptDrops(true);
    this->setAcceptDrops(true);
    
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
    connect(model, &QXCollectionModel::beginResetModel, this, &QXCollectionView::onBeginResetModel);
    connect(model, &QXCollectionModel::endResetModel, this, &QXCollectionView::onEndResetModel);
    setFocusProxy(widget_);
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
QXCollectionView::setItemSize(const QSize & size) {
    widget_->itemSize_ = size;
}

void
QXCollectionView::setItemSize(int size) {
    widget_->itemSize_ = QSize(size, size);
}

const QSize &
QXCollectionView::itemSize() const {
    return widget_->itemSize_;
}

void
QXCollectionView::setItemSpace(int space) {
    widget_->itemSpace_ = QSize(space, space);
}

int
QXCollectionView::itemSpace() const {
    return widget_->itemSpace_.width();
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
    widget_->headerSize_ = height;
}

int
QXCollectionView::headerHeight() const {
    return widget_->headerSize_;
}

QRect
QXCollectionView::itemRect(const QXCollectionModelIndex & index) const {
    QRect rect = widget_->itemRect(index);
    return QRect(widget_->mapToParent(rect.topLeft()),
                 widget_->mapToParent(rect.bottomRight()));
}

void
QXCollectionView::select(const QXCollectionModelIndex & index) {
    widget_->select(index);
}

void
QXCollectionView::scrollTo(const QXCollectionModelIndex & index) {
    QRect rect = itemRect(index);
    this->ensureVisible(rect.center().x(), rect.center().y(), rect.width(), rect.height());
    widget_->update();
}

void
QXCollectionView::onBeginResetModel() {
    
}

void
QXCollectionView::onEndResetModel() {
    widget_->updateGeometry();
    widget_->update();
    ensureVisible(0, 0);
}

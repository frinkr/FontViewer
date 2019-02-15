#include <QScrollArea>
#include <QPaintEvent>
#include <QPainter>
#include "QXListView.h"
#include <QRandomGenerator>

namespace {
    constexpr qreal CELL_SIZE = 100;
    constexpr qreal CELL_SPACE = 20;
    constexpr qreal HEADER_HEIGHT = 20;
    constexpr qreal HEADER_SPACE_ABOVE = 18;
    constexpr qreal HEADER_SPACE_BELOW = 8;
    constexpr qreal CONTENT_MARGIN = 30;

    constexpr int COLUMNS = 5;
}

QSize
QXListViewContentWidget::sizeHint() const {
    return QSize(COLUMNS * (CELL_SIZE + CELL_SPACE) + 2 * CONTENT_MARGIN - CELL_SPACE, 1000);
}

QSize
QXListViewContentWidget::minimumSizeHint() const {
    return QSize(COLUMNS * (CELL_SIZE + CELL_SPACE) + 2 * CONTENT_MARGIN - CELL_SPACE, 1000);
}

void
QXListViewContentWidget::paintEvent(QPaintEvent * event) {
    QPainter painter(this);
    painter.setPen(Qt::red);

    painter.setBrush(Qt::red);

    painter.fillRect(event->rect(), Qt::darkGray);

    int columns = (this->rect().width() - 2 * CONTENT_MARGIN + CELL_SPACE) / (CELL_SIZE + CELL_SPACE);

    int rowsBegin = (event->rect().top() - CONTENT_MARGIN) / (CELL_SIZE + CELL_SPACE);

    int rowsEnd = std::ceil((event->rect().bottom() - CONTENT_MARGIN) / (CELL_SIZE + CELL_SPACE));

    auto getCellRect = [this](int row, int col) {
        QPoint leftTop (CONTENT_MARGIN + col * (CELL_SIZE + CELL_SPACE), CONTENT_MARGIN + row * (CELL_SIZE + CELL_SPACE));
        return QRect(leftTop, QSize(CELL_SIZE, CELL_SIZE));
    };

    for (int col = 0; col < columns; ++ col) {
        for (int row = rowsBegin; row < rowsEnd ; ++ row) {
            QRect cellRect = getCellRect(row, col);
            int rand = QRandomGenerator::global()->bounded(10);
            Qt::GlobalColor colors[] = {Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::yellow, Qt::magenta, Qt::darkGreen, Qt::darkBlue};
            painter.fillRect(cellRect, colors[((col + 1) * (row + 1)) % (sizeof(colors) / sizeof(colors[0]))]);
        }
    }

    qWarning("update rect: %d, %d, %d, %d. %d cells updated",
             event->rect().left(),
             event->rect().top(),
             event->rect().width(),
             event->rect().height(),
             (columns * (rowsEnd - rowsBegin)));
}

QXListView::QXListView(QWidget *parent)
    : QScrollArea(parent) {

    this->setWidgetResizable(false);

    this->setWidget(new QXListViewContentWidget);
}

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDockWidget>
#include <QPainter>
#include <QPushButton>
#include <QStylePainter>
#include <QSpacerItem>
#include "QUDockTitleBarWidget.h"

namespace {
    int TITLE_BAR_HEIGHT = 18;
}
QUDockTitleBarWidget::QUDockTitleBarWidget(QWidget * parent)
    : QWidget(parent) {
    this->setMinimumHeight(20);
    QHBoxLayout * layout = new QHBoxLayout(this);
    QSpacerItem * spacer = new QSpacerItem(10, TITLE_BAR_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Minimum);
    

    QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    QPushButton * closeButton = new QPushButton(closeIcon, QString(), this);
    closeButton->setFixedSize(closeIconRect().size().toSize());
    closeButton->setFlat(true);
    closeButton->setStyleSheet("boder:none;");
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
        dockWidget->hide();
    });

    layout->addSpacing(5);
    layout->addWidget(closeButton);
    layout->addItem(spacer);
    layout->setMargin(0);
}

QUDockTitleBarWidget::~QUDockTitleBarWidget() {
    
}


QSize
QUDockTitleBarWidget::sizeHint() const {
    return minimumSizeHint();
}

QSize
QUDockTitleBarWidget::minimumSizeHint() const {
    return QSize(TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);
}

void
QUDockTitleBarWidget::paintEvent(QPaintEvent * event) {
    //return QWidget::paintEvent(event);
    QPainter p(this);

    p.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    
    QRect rect(0, 0, width(), height());
    //p.fillRect(rect, palette().color(QPalette::Normal, QPalette::Window));

    QColor color = palette().color(QPalette::Normal, QPalette::Window);
    //color = color.darker();
    QBrush brush(color, Qt::Dense2Pattern);
    p.fillRect(rect, brush);

    // close icon
    //QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    //p.drawPixmap(closeIconRect(), closeIcon, closeIcon.rect());

    // title
    p.setPen(palette().color(QPalette::Normal, QPalette::Text));
    QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    p.drawText(rect, Qt::AlignCenter, dockWidget->windowTitle());
}

void
QUDockTitleBarWidget::mousePressEvent(QMouseEvent * event) {
    QWidget::mousePressEvent(event);
}

QRectF
QUDockTitleBarWidget::closeIconRect() {
    QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    qreal h = qMin(closeIcon.height(), height());
    qreal w = h * closeIcon.width() / closeIcon.height();
    return QRectF(5, (height() - h) / 2, w, h);
}

#include <QDockWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QSpacerItem>
#include <QStylePainter>

#include "QXApplication.h"
#include "QXDockTitleBarWidget.h"

namespace {
    int TITLE_BAR_HEIGHT = 18;
}
QXDockTitleBarWidget::QXDockTitleBarWidget(QWidget * parent)
    : QWidget(parent) {
    this->setMinimumHeight(20);
    QHBoxLayout * layout = new QHBoxLayout(this);
    QSpacerItem * spacer = new QSpacerItem(10, TITLE_BAR_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    int iconSize = TITLE_BAR_HEIGHT - 6;
    QIcon closeIcon = qApp->loadIcon(":/images/close.png");
    QPushButton * closeButton = new QPushButton(closeIcon, QString(), this);

    closeButton->setMinimumSize(iconSize, iconSize);
    closeButton->setMaximumSize(iconSize, iconSize);
    closeButton->setIconSize(QSize(iconSize, iconSize));
    closeButton->setFlat(true);
    //closeButton->setStyleSheet("boder:none;");
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
        dockWidget->hide();
    });

    layout->addSpacing(5);
    layout->addWidget(closeButton);
    layout->addItem(spacer);
    layout->setMargin(0);
}

QXDockTitleBarWidget::~QXDockTitleBarWidget() {
    
}


QSize
QXDockTitleBarWidget::sizeHint() const {
    return minimumSizeHint();
}

QSize
QXDockTitleBarWidget::minimumSizeHint() const {
    return QSize(TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);
}

void
QXDockTitleBarWidget::paintEvent(QPaintEvent * event) {
    //return QWidget::paintEvent(event);
    QPainter p(this);

    p.setRenderHints(QPainter::Antialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    
    //QRect rect(0, 0, width(), height());
    //p.fillRect(rect, palette().color(QPalette::Normal, QPalette::Window));

    //QColor color = palette().color(QPalette::Normal, QPalette::Window);
    //color = color.darker();
    //QBrush brush(color, Qt::Dense2Pattern);
    //p.fillRect(rect, brush);

    // close icon
    //QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    //p.drawPixmap(closeIconRect(), closeIcon, closeIcon.rect());

    // title
    p.setPen(palette().color(QPalette::Normal, QPalette::Text));
    QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    p.drawText(rect(), Qt::AlignCenter, dockWidget->windowTitle());
}

void
QXDockTitleBarWidget::mousePressEvent(QMouseEvent * event) {
    QWidget::mousePressEvent(event);
}

QRectF
QXDockTitleBarWidget::closeIconRect() {
    QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    qreal h = qMin(closeIcon.height(), height());
    qreal w = h * closeIcon.width() / closeIcon.height();
    return QRectF(5, (height() - h) / 2, w, h);
}

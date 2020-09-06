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
    int TITLE_BAR_HEIGHT = 23;
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

    connect(qApp, &QApplication::focusChanged, this, &QXDockTitleBarWidget::onFocusChanged);

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

    // Background color
    QColor bgColor;
    if (haveFocus_)
        bgColor = palette().color(QPalette::Active, QPalette::Highlight);
    else
        bgColor = palette().color(QPalette::Normal, QPalette::Window);

    p.fillRect(rect(), bgColor);


    // close icon
    //QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    //p.drawPixmap(closeIconRect(), closeIcon, closeIcon.rect());

    // title
    QColor fgColor;
    if (haveFocus_)
        fgColor = palette().color(QPalette::Active, QPalette::HighlightedText);
    else
        fgColor = palette().color(QPalette::Normal, QPalette::Text);
    p.setPen(fgColor);
    QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    p.drawText(rect(), Qt::AlignCenter, dockWidget->windowTitle());

    
    // Grab bar
    QFontMetrics fm(p.font());
    int titleWidth = fm.horizontalAdvance(dockWidget->windowTitle());

    float kHoriMargin = 5;
    float kVertMargin = TITLE_BAR_HEIGHT / 4;
    float barWidth = (width() - titleWidth) / 2 - 2 * kHoriMargin;
    float barHeight = TITLE_BAR_HEIGHT - 2 * kVertMargin;
    QRect leftRect(kHoriMargin, kVertMargin, barWidth, barHeight);
    QRect rightRect(width() - barWidth - kHoriMargin, kVertMargin, barWidth, barHeight);
    p.fillRect(leftRect, QBrush(qApp->darkMode()? bgColor.lighter(): bgColor.darker(), Qt::Dense7Pattern));
    p.fillRect(rightRect, QBrush(qApp->darkMode()? bgColor.lighter(): bgColor.darker(), Qt::Dense7Pattern));
}

void
QXDockTitleBarWidget::mousePressEvent(QMouseEvent * event) {
    QWidget::mousePressEvent(event);
}

void
QXDockTitleBarWidget::onFocusChanged(QWidget * old, QWidget * now) {
    QDockWidget* dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    QWidget * parent = now;
    while (parent && parent != dockWidget)
        parent = parent->parentWidget();

    if (haveFocus_ != (parent == dockWidget)) {
        haveFocus_ = !haveFocus_;
        update();
    }
}

QRectF
QXDockTitleBarWidget::closeIconRect() {
    QPixmap closeIcon = style()->standardPixmap(QStyle::SP_TitleBarCloseButton, nullptr, this);
    qreal h = qMin(closeIcon.height(), height());
    qreal w = h * closeIcon.width() / closeIcon.height();
    return QRectF(5, (height() - h) / 2, w, h);
}

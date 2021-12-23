#include <QDockWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QToolButton>
#include <QSpacerItem>
#include <QStylePainter>

#include "QXApplication.h"
#include "QXDockTitleBarWidget.h"

namespace {
    int TITLE_BAR_HEIGHT = 21;
}
QXDockTitleBarWidget::QXDockTitleBarWidget(QWidget * parent)
    : QWidget(parent) {
    this->setMinimumHeight(20);
    QHBoxLayout * layout = new QHBoxLayout(this);
    QSpacerItem * spacer = new QSpacerItem(10, TITLE_BAR_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    int iconSize = TITLE_BAR_HEIGHT - 6;
    QIcon closeIcon = qApp->loadIcon(":/images/close.png");
    QToolButton * closeButton = new QToolButton(this);

    closeButton->setFixedSize(iconSize, iconSize);
    closeButton->setIcon(closeIcon);
    connect(closeButton, &QAbstractButton::clicked, this, [this]() {
        QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
        dockWidget->hide();
    });
    
    setFocusPolicy(Qt::StrongFocus);
    connect(qApp, &QApplication::focusChanged, this, &QXDockTitleBarWidget::onFocusChanged);

    layout->addSpacing(5);
    layout->addWidget(closeButton);
    layout->addItem(spacer);
    layout->setContentsMargins(0, 0, 0, 0);
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

    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    
    // Background color
    QColor bgColor, fgColor;
    if (haveFocus_) {
        bgColor = palette().color(QPalette::Active, backgroundRole());
        fgColor = palette().color(QPalette::Active, QPalette::HighlightedText);
        if (qApp->darkMode())
            bgColor = bgColor.lighter(150);
        else
            bgColor = bgColor.darker(120);
    }
    else {
        bgColor = palette().color(QPalette::Inactive, backgroundRole());
        fgColor = palette().color(QPalette::Normal, QPalette::Text);
        
        if (qApp->darkMode())
            bgColor = bgColor.lighter(120);
        else
            bgColor = bgColor.darker(105);
    }
    p.fillRect(rect(), bgColor);

    p.setPen(fgColor);
    QDockWidget * dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    p.drawText(rect(), Qt::AlignCenter, dockWidget->windowTitle());

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

#include "QXDockWidget.h"
#include "QXDockTitleBarWidget.h"

QXDockWidget::QXDockWidget(const QString & title, QWidget * parent)
    : QDockWidget(title, parent) {
    setTitleBarWidget(new QXDockTitleBarWidget(this));

    connect(this, &QDockWidget::topLevelChanged, [this](bool topLevel) {
        if (topLevel)
            setTitleBarWidget(nullptr);
        else
            setTitleBarWidget(new QXDockTitleBarWidget(this));
    });
}

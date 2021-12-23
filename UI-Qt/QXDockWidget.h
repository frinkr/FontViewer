#pragma once
#include <QDockWidget>

class QXDockWidget : public QDockWidget {
public:
    QXDockWidget(const QString & title, QWidget * parent = nullptr);
};

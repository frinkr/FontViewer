#include "QXThemedWindow.h"

#if defined(Q_OS_LINUX)
QXWindowDecorator *
QXWindowDecorator::createInstance(QObject * parent) {
    return new QXWindowDecorator(parent);
}
#endif

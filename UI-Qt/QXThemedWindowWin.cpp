#include "QXThemedWindow.h"

QXWindowDecorator *
QXWindowDecorator::createInstance(QObject * parent) {
    return new QXWindowDecorator(parent);
}
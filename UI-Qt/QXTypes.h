#pragma once
#include <QString>
#include <string>

template <typename T = void>
QString
toQString(const std::string & str) {
    return QString::fromStdString(str);
}

template <typename T = void>
std::string
toStdString(const QString & str) {
    return str.toUtf8().constData();
}

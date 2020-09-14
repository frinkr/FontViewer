#pragma once
#include <QDir>
#include <QString>
#include <string>

QString
toQString(const std::string & str);

std::string
toStdString(const QString & str);

template <typename T>
T pathJoin(const T & v) {
    return v;
}

template<typename... Args>
QString pathJoin(const QString & first, Args... args) {
    return QDir(first).filePath(pathJoin(args...));
}

QString
ftDateTimeToString(int64_t value);

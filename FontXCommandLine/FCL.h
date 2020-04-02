#pragma once
#include <QString>
#include <string>

QString
toQString(const std::string & str);

std::string
toStdString(const QString & str);

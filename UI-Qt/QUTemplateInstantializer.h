#pragma once

#include <QString>
#include <QMap>
#include <QVariant>

QString
instantializeTemplate(const QString & t, const QMap<QString, QVariant> & map);

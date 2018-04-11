#include "QUTemplateInstantializer.h"

QString
instantializeTemplate(const QString & t, const QMap<QString, QVariant> & map) {
    QString s(t);
    for(const auto & key: map.keys()) {
        QVariant value = map.value(key);
        s = s.replace(QString("${%1}").arg(key), value.toString());
    }
    return s;
}

#include "QXConv.h"
#include <QDateTime>
#include "FontX/FXUnicode.h"

QString
toQString(const std::string & str) {
    return QString::fromStdString(str);
}

std::string
toStdString(const QString & str) {
    return str.toUtf8().constData();
}

QString
ftDateTimeToString(int64_t value) {
    static QDateTime epoch(QDate(1904, 01, 01), QTime(0, 0, 0), Qt::UTC);

    return epoch.addSecs(value).toString("yyyy-MM-dd HH:mm:ss t");
}

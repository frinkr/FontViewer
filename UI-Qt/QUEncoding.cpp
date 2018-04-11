#include "QUEncoding.h"

QString
QUEncoding::charHexNotation(FXChar c, bool unicode) {
    if (unicode)
        return QString("U+%1").arg(c, 4, 16, QChar('0')).toUpper();
    else
        return QString("0x%1").arg(c, 4, 16, QChar('0')).toUpper();
}

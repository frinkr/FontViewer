#include <QStringList>
#include "QUEncoding.h"

QString
QUEncoding::charHexNotation(FXChar c, bool unicode) {
    if (unicode)
        return QString("U+%1").arg(c, 4, 16, QChar('0')).toUpper();
    else
        return "0x" + QString("%1").arg(c, 4, 16, QChar('0')).toUpper();
}

FXChar
QUEncoding::charFromHexNotation(const QString & str, bool * unicode) {
    QString code = str;

    if (code.indexOf("U+") == 0 || code.indexOf("0x"))
        code.remove(0, 2);
    
    bool ok = false;
    FXChar c = code.toUInt(&ok, 16);
    if (ok) return c;
    return FXCharInvalid;
}

QUrl
QUEncoding::charHexLink(FXChar c, bool unicode) {
    return QUrl(QString("fv://go/%1").arg(charHexNotation(c, unicode)));
}

bool
QUEncoding::isCharHexLink(const QUrl & link) {
    return charFromLink(link) != FXCharInvalid;
}

FXChar
QUEncoding::charFromLink(const QUrl & link) {
    if (link.scheme() == "fv" && link.host() == "go") {
        QString path = link.path();
        if (path.indexOf('/') == 0)
            path.remove(0, 1);
        return charFromHexNotation(path);
    }
    return FXCharInvalid;
}

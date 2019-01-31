#include <QStringList>
#include "QXEncoding.h"

QString
QXEncoding::charHexNotation(FXGChar c) {
    switch (c.type) {
    case FXGCharTypeUnicode:
        return QString("U+%1").arg(c.value, 4, 16, QChar('0')).toUpper();
    case FXGCharTypeOther:
        return "0x" + QString("%1").arg(c.value, 4, 16, QChar('0')).toUpper();
    case FXGCharTypeGlyphID:
        return "gid" + QString("%1").arg(c.value);
    default:
        return QString();
    }
}

FXGChar
QXEncoding::charFromHexNotation(const QString & str) {
    QString code = str;
    bool ok = false;
    if (code.indexOf("U+") == 0 || code.indexOf("0x") == 0 || code.indexOf("\\u") == 0) {
        code.remove(0, 2);
        FXChar c = code.toUInt(&ok, 16);
        return ok? FXGChar(c, FXGCharTypeUnicode): FXGCharInvalid;
    }
    else if (code.indexOf("gid") == 0){
        code.remove(0, 3);
        FXChar c = code.toUInt(&ok);
        return ok? FXGChar(c, FXGCharTypeGlyphID): FXGCharInvalid;
    }
    return FXGCharInvalid;
}

QUrl
QXEncoding::charHexLink(FXGChar c) {
    return QUrl(QString("fv://go/%1").arg(charHexNotation(c)));
}

bool
QXEncoding::isCharHexLink(const QUrl & link) {
    return charFromLink(link) != FXGCharInvalid;
}

FXGChar
QXEncoding::charFromLink(const QUrl & link) {
    if (link.scheme() == "fv" && link.host() == "go") {
        QString path = link.path();
        if (path.indexOf('/') == 0)
            path.remove(0, 1);
        return charFromHexNotation(path);
    }
    return FXGCharInvalid;
}
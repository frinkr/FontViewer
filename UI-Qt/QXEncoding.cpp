#include <regex>
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

QString
QXEncoding::htmlLinkElement(const QUrl & url, const QString & name) {
    return QString("<a href=\"%1\">%2</a>").arg(url.toString()).arg(name);
}

QUrl
QXEncoding::externalUnicodeHexLink(FXChar unicode) {
    return QString("https://codepoints.net/%1").arg(charHexNotation(FXGChar(unicode, FXGCharTypeUnicode)));
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

QString
QXEncoding::decodeFromHexNotation(const QString & str) {
    std::regex re(R"(([uU]\+|\\[uU]|0x){1}[A-Za-z0-9]{4})");
    std::string s = str.toStdString();
    auto matchBegin = std::sregex_iterator(s.begin(), s.end(), re);
    auto matchEnd = std::sregex_iterator();
    int lastEnd = 0;
    QStringList list;
    for (std::sregex_iterator i = matchBegin; i != matchEnd; ++i) {
        std::smatch match = *i;
        int start = match.position();
        int len = match.length();
        int end = start + len;

        if (lastEnd != start)
            list << QString::fromStdString(s.substr(lastEnd, start - lastEnd));

        uint c = QString::fromStdString(s.substr(start + 2, len - 2)).toUInt(nullptr, 16);
        list << QString::fromUcs4(&c, 1);
        lastEnd = end;
    }
    if (lastEnd != s.length())
        list << QString::fromStdString(s.substr(lastEnd, s.length() - lastEnd));
    return list.join("");
}

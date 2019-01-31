#pragma once
#include <QString>
#include <QUrl>
#include "FontX/FX.h"

struct QXEncoding {
    static QString
    charHexNotation(FXGChar c);

    static FXGChar
    charFromHexNotation(const QString & str);

    static QUrl
    charHexLink(FXGChar c);

    static bool
    isCharHexLink(const QUrl & link);
    
    static FXGChar
    charFromLink(const QUrl & link);

    static FXVector<unsigned char>
    utf8(FXChar c);

    static FXVector<unsigned short>
    utf16(FXChar c);
};

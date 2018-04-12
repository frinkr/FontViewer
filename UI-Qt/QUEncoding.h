#pragma once
#include <QString>
#include <QUrl>
#include "FontX/FX.h"

struct QUEncoding {
    static QString
    charHexNotation(FXChar c, bool unicode = true);

    static FXChar
    charFromHexNotation(const QString & str, bool * unicode = nullptr);
    
    static QUrl
    charHexLink(FXChar c, bool unicode = true);

    static bool
    isCharHexLink(const QUrl & link);
    
    static FXChar
    charFromLink(const QUrl & link);

    static FXVector<unsigned char>
    utf8(FXChar c);

    static FXVector<unsigned short>
    utf16(FXChar c);
};

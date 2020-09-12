#pragma once
#include <QDir>
#include <QImage>
#include <QString>
#include <string>

#include "FontX/FXPixmap.h"
#include "FontX/FXFace.h"
#include "FontX/FXGlyph.h"

QString
toQString(const std::string & str);

std::string
toStdString(const QString & str);

template <typename T>
T pathJoin(const T & v) {
    return v;
}

template<typename... Args>
QString pathJoin(const QString & first, Args... args) {
    return QDir(first).filePath(pathJoin(args...));
}

QImage
toQImage(const FXPixmapARGB & bm, bool copy);

QImage
toQImage(const FXPixmapGray & bm, bool copy);

QImage
toQImage(const FXGlyphImage & im, bool copy);

FXPixmapARGB
convertToWhite(const FXPixmapARGB & bm);

FXPixmapARGB
convertToBlack(const FXPixmapARGB & bm);

FXGlyphImage
autoColorGlyphImage(const FXGlyphImage & img, bool selected);

QImage
drawGlyphImageInEmBox(const FXGlyphImage & img);

QImage
unicodeCharImage(FXChar c, const QSize & emSize);

QString
ftDateTimeToString(int64_t value);

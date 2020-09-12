#pragma once
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

QSize
glyphEmSize();

QImage
drawGlyphImage(const FXGlyphImage & img, const QSize & emSize);

QImage
unicodeCharImage(FXChar c, const QSize & emSize);

QString
ftDateTimeToString(int64_t value);

bool
needInvertImage(const QImage & img, bool selected, bool darkMode);

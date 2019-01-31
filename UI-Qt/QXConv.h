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
toQImage(const FXPixmapARGB & bm);

QSize
glyphEmSize();

QImage
placeGlyphImage(const FXGlyph & g, const QSize & emSize);

QImage
unicodeCharImage(FXChar c, const QSize & emSize);

QString
ftDateTimeToString(int64_t value);

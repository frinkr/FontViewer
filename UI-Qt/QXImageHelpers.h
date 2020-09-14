#pragma once
#include <QImage>

#include "FontX/FXPixmap.h"
#include "FontX/FXFace.h"
#include "FontX/FXGlyph.h"


QImage
toQImage(const FXPixmapARGB & bm);

QImage
toQImage(const FXPixmapGray & bm);

QImage
toQImage(const FXGlyphImage & im);

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


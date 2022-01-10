#pragma once
#include <QImage>

#include "FontX/FXPixmap.h"
#include "FontX/FXFace.h"
#include "FontX/FXGlyph.h"

bool
isDarkColor(const QColor & color);
    
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
tintGlyphImageWithColor(const FXGlyphImage & img, const QColor & color, bool toBlackOrWhite = true);

QImage
drawGlyphImageInEmBox(const FXGlyphImage & img);

QImage
unicodeCharImage(FXChar c, const QSize & emSize);

QRect
calculateTargetRect(const FXGlyphImage & glyphImage, const QRect & emRect);

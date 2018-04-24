#pragma once
#include <QImage>
#include <QString>
#include <string>

#include "FontX/FXBitmap.h"

QString
toQString(const std::string & str);

std::string
toStdString(const QString & str);

QImage
toQImage(const FXBitmapARGB & bm);

QSize
glyphEmSize();

QImage
placeImage(const QImage & image, const QSize & emSize);

QImage
unicodeCharImage(FXChar c, const QSize & emSize);

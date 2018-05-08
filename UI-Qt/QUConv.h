#pragma once
#include <QImage>
#include <QString>
#include <string>

#include "FontX/FXPixmap.h"

QString
toQString(const std::string & str);

std::string
toStdString(const QString & str);

QImage
toQImage(const FXPixmapARGB & bm);

QSize
glyphEmSize();

QImage
placeImage(const QImage & image, const QSize & emSize);

QImage
unicodeCharImage(FXChar c, const QSize & emSize);

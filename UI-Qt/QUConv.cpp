#include <QPainter>
#include "QUConv.h"
#include "FontX/FXFace.h"
#include "FontX/FXUnicode.h"

QString
toQString(const std::string & str) {
    return QString::fromStdString(str);
}

std::string
toStdString(const QString & str) {
    return str.toUtf8().constData();
}

QImage
toQImage(const FXBitmapARGB & bm) {
    QImage image(bm.width, bm.height, QImage::Format_ARGB32);
    image.setDevicePixelRatio(2);
    for (int y = 0; y < bm.height; ++ y) {
        for (int x = 0; x < bm.width; ++ x) {
            FXARGB color = bm.pixel(x, y);
            image.setPixel(x, y, color);
        }
    }
    return image;
}

QSize
glyphEmSize() {
    return QSize(pt2px(FXDefaultFontSize),
                 pt2px(FXDefaultFontSize));
}

QImage
placeImage(const QImage & image, const QSize & emSize) {
    QRect imageRect(0, 0, image.width(), image.height());
    QRect emRect(0, 0, emSize.width(), emSize.height());
    
    QImage out(emSize, image.isNull()?QImage::Format_ARGB32 : image.format());

    QPainter p(&out);
    QPen pen;
    p.fillRect(emRect, Qt::white);

    const double r = 0.8;
    QRect outRect((1 - r) / 2 * emSize.width(),
                  (1 - r) / 2 * emSize.height(),
                  emSize.width() * r,
                  emSize.height() * r);

    if ((outRect.width() > imageRect.width()) || (outRect.height() > imageRect.height()))
        outRect = QRect((emSize.width() - imageRect.width()) / 2,
                        (emSize.height() - imageRect.height()) / 2,
                        imageRect.width(),
                        imageRect.height());
    p.drawImage(outRect, image, imageRect);

    out.setDevicePixelRatio(2);
    return out;
}

QImage
charImage(FXChar c, const QSize & emSize) {
    if (FXUnicode::defined(c)) {
        QRect emRect(0, 0, emSize.width(), emSize.height());
        QImage out(emSize, QImage::Format_ARGB32);
        QPainter p(&out);
        QPen pen;
        pen.setWidth(2);
        pen.setColor(Qt::red);
        p.setPen(pen);
        p.fillRect(emRect, Qt::gray);
        p.drawRect(emRect);

        QFont f = p.font();
        f.setPointSize(100);
        p.setFont(f);
        uint u = c;
        QString text = QString::fromUcs4(&u, 1);
        p.drawText(emRect, Qt::AlignCenter, text);

        out.setDevicePixelRatio(2);
        return out;
    }
    else {
        return QImage(":images/undefined_d.png");
    }
}

#include <QDateTime>
#include <QPainter>

#include "QXConv.h"
#include "FontX/FXFace.h"
#include "FontX/FXUnicode.h"

namespace {
    void
    deleteFXPixmapARGB(void * bm) {
        delete (FXPixmapARGB*)bm;
    }
}
QString
toQString(const std::string & str) {
    return QString::fromStdString(str);
}

std::string
toStdString(const QString & str) {
    return str.toUtf8().constData();
}

QImage
toQImage(const FXPixmapARGB & bm) {
#if QU_IMAGE_NO_OPTIMIZE
    QImage image(bm.width, bm.height, QImage::Format_ARGB32);
    image.setDevicePixelRatio(2);
    for (int y = 0; y < bm.height; ++ y) {
        for (int x = 0; x < bm.width; ++ x) {
            FXARGB color = bm.pixel(x, y);
            image.setPixel(x, y, color);
        }
    }
#else
    FXPixmapARGB * ref = new FXPixmapARGB(bm); // make a new ref
    QImage image((uchar*)bm.buffer,
                 bm.width,
                 bm.height,
                 QImage::Format_ARGB32,
                 deleteFXPixmapARGB,
                 ref);
#endif
    return image;
}

QSize
glyphEmSize() {
    return QSize(pt2px(FXDefaultFontSize),
                 pt2px(FXDefaultFontSize));
}

QImage
placeGlyphImage(const FXGlyph & g, const QSize & emSize) {
    QImage image = toQImage(g.pixmap());

    QRect imageRect(0, 0, image.width(), image.height());
    QRect emRect(0, 0, emSize.width(), emSize.height());
    
    QImage out(emSize, image.isNull()?QImage::Format_ARGB32 : image.format());
    out.fill(qRgba(0, 0, 0, 0));
//    out.fill(Qt::white);
    if (image.isNull())
        return out;
    
    QPainter p(&out);
    if (g.face->isScalable())
        p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    const double r = g.face->isScalable()? 0.8: 0.618;
    double wr = r, hr = r;
    if (image.width() > image.height()) {
        wr = r;
        hr = image.height() * wr / image.width();
    }
    else {
        hr = r;
        wr = image.width() * hr / image.height();
    }
    // proportional scaling no larger than 0.8 EM    
    QRectF outRect((1 - wr) / 2 * emSize.width(),
                  (1 - hr) / 2 * emSize.height(),
                  emSize.width() * wr,
                  emSize.height() * hr);
    if (g.face->isScalable()) {
        // if the image is too small to full-fill outRect, let's keep the orignal size
        if ((outRect.width() > imageRect.width()) && (outRect.height() > imageRect.height()))
            outRect = QRect((emSize.width() - imageRect.width()) / 2,
                            (emSize.height() - imageRect.height()) / 2,
                            imageRect.width(),
                            imageRect.height());
    }
    p.drawImage(outRect, image, imageRect);

    out.setDevicePixelRatio(2);
    return out;
}

QImage
unicodeCharImage(FXChar c, const QSize & emSize) {
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

QString
ftDateTimeToString(int64_t value) {
    static QDateTime epoch(QDate(1904, 01, 01), QTime(0, 0, 0), Qt::UTC);

    return epoch.addSecs(value).toString("yyyy-MM-dd HH:mm:ss t");
}

#include <QDateTime>
#include <QPainter>

#include "QXApplication.h"
#include "QXConv.h"
#include "FontX/FXFace.h"
#include "FontX/FXUnicode.h"

namespace {
    template <typename T> void
    deleteFXPixmap(void * bm) {
        delete (T*)bm;
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
toQImage(const FXPixmapARGB & bm, bool copy) {
    FXPixmapARGB * ref = new FXPixmapARGB(bm); // make a new ref
    QImage image(reinterpret_cast<uchar*>(bm.buffer),
                 bm.width,
                 bm.height,
                 QImage::Format_ARGB32,
                 &deleteFXPixmap<FXPixmapARGB>,
                 ref);
    return copy? image.copy() : image; 
}

QImage
toQImage(const FXPixmapGray & bm, bool copy) {
    FXPixmapGray* ref = new FXPixmapGray(bm); // make a new ref
    QImage image(reinterpret_cast<uchar*>(bm.buffer),
        bm.width,
        bm.height,
        bm.pitch,
        QImage::Format_Alpha8,
        &deleteFXPixmap<FXPixmapGray>,
        ref);
    return copy? image.copy(): image;
}

static FXString bmWhiteForegroundKey = "fg";

FXPixmapARGB
convertToWhite(const FXPixmapARGB & bm) {
    if (bm.properties->has(bmWhiteForegroundKey) &&
        bm.properties->get<bool>(bmWhiteForegroundKey)) {
        return bm; // is white 
    }

    // Convert to white by QImage, (without copying the data)
    QImage image = toQImage(bm, false);
    image.invertPixels();
    const_cast<FXPixmapARGB&>(bm).properties->set(bmWhiteForegroundKey, true);
    return bm;
}

FXPixmapARGB
convertToBlack(const FXPixmapARGB & bm) {
    if (!bm.properties->has(bmWhiteForegroundKey) ||
        !bm.properties->get<bool>(bmWhiteForegroundKey)) {
        return bm; // is black
    }

    // Convert to white by QImage, (without copying the data)
    QImage image = toQImage(bm, false);
    image.invertPixels();
    const_cast<FXPixmapARGB&>(bm).properties->set(bmWhiteForegroundKey, false);
    return bm;
}

FXGlyphImage
autoColorGlyphImage(const FXGlyphImage & img, bool selected) {
    if (img.empty())
        return img;

    auto gi = img;
    if (gi.mode != FXGlyphImage::kColor && (selected || qApp->darkMode()))
        gi.pixmap = convertToWhite(gi.pixmap);
    else
        gi.pixmap = convertToBlack(gi.pixmap);
    return gi;
}

QImage
toQImage(const FXGlyphImage & im, bool copy) {
    return toQImage(im.pixmap, copy);
}

QSize
glyphEmSize() {
    return QSize(pt2px(FXDefaultFontSize),
                 pt2px(FXDefaultFontSize));
}

QImage
drawGlyphImage(const FXGlyphImage & img, const QSize & emSize) {
    QImage image = toQImage(img, true);

    QRect imageRect(0, 0, image.width(), image.height());
    QRect emRect(0, 0, emSize.width(), emSize.height());
    
    QImage out(emSize, image.isNull()?QImage::Format_ARGB32 : image.format());
    out.fill(qRgba(0, 0, 0, 0));
//    out.fill(Qt::white);
    if (image.isNull())
        return out;
    
    QPainter p(&out);
    if (img.isSmoothScalable())
        p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

    const double r = img.isSmoothScalable()? 0.8: 0.618;
    double wr, hr;
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
    if (img.isSmoothScalable()) {
        // if the image is too small to full-fill outRect, let's keep the orignal size
        if ((outRect.width() > imageRect.width()) && (outRect.height() > imageRect.height()))
            outRect = QRect((emSize.width() - imageRect.width()) / 2,
                            (emSize.height() - imageRect.height()) / 2,
                            imageRect.width(),
                            imageRect.height());
    }
    p.drawImage(outRect, image, imageRect);
    p.end();
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

bool
needInvertImage(const QImage & img, bool selected, bool darkMode) {
    if (img.format() == QImage::Format_ARGB32)
        return false;

    return selected || darkMode;
}

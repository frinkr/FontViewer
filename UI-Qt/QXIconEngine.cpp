#include <QDir>
#include <QPainter>
#include <QPixmap>
#include <QFileInfo>
#include "QXApplication.h"
#include "QXIconEngine.h"

bool QXIconEngine::PixmapCacheKey::operator<(const QXIconEngine::PixmapCacheKey & other) const {
    if (darkMode < other.darkMode) return true;
    if (darkMode > other.darkMode) return false;
    if (size.width() < other.size.width()) return true;
    if (size.width() > other.size.width()) return false;
    if (size.height() < other.size.height()) return true;
    if (size.height() > other.size.height()) return false;
    if (mode < other.mode) return true;
    if (mode > other.mode) return false;
    if (state < other.state) return true;
    return false;

}

QXIconEngine::QXIconEngine(const QString & filePath)
    : QIconEngine()
    , filePath_(filePath) {}
    
void
QXIconEngine::paint(QPainter * painter,
                    const QRect & rect,
                    QIcon::Mode mode,
                    QIcon::State state) {
    auto pix = pixmap(rect.size(), mode, state);
    painter->drawPixmap(rect, pix);
}

QPixmap
QXIconEngine::pixmap(const QSize & size,
                     QIcon::Mode mode,
                     QIcon::State state) {
    
    PixmapCacheKey key{ qApp->darkMode(), size, mode, state };
    if (!cache_.contains(key)) {
        QPixmap pixmap;
        if (key.darkMode) {
            if (pm0_.isNull())
                pm0_ = QPixmap(GetDarkIconFile());
            pixmap = pm0_;
        }
        else {
            if (pm1_.isNull())
                pm1_ = QPixmap(filePath_);
            pixmap = pm1_;
        }
        QIcon icon(pixmap);
        pixmap = pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cache_.insert(key, icon.pixmap(size, mode, state));
    }
        
    return cache_.value(key);
}

QString
QXIconEngine::GetDarkIconFile() const {
    QFileInfo fi(filePath_);
    fi = fi.dir().filePath(fi.baseName() + "_d." + fi.completeSuffix());
    if (fi.exists())
        return fi.absoluteFilePath();
    else
        return filePath_;
}
QIconEngine *
QXIconEngine::clone() const {
    return new QXIconEngine(filePath_);
}

#pragma once

#include <QIconEngine>
#include <QPixmap>
#include <QMap>

class QXIconEngine : public QIconEngine {

public:
    explicit QXIconEngine(const QString & filePath);
    
    void
    paint(QPainter * painter,
          const QRect & rect,
          QIcon::Mode mode,
          QIcon::State state) override;
    
    QPixmap
    pixmap(const QSize & size,
           QIcon::Mode mode,
           QIcon::State state) override;

    QIconEngine *
    clone() const override;

private:
    QString
    GetDarkIconFile() const;
    
    struct PixmapCacheKey {
        bool darkMode{};
        QSize size{};
        QIcon::Mode mode{};
        QIcon::State state{};

        bool operator<(const PixmapCacheKey& other) const;
    };


private:
    QString   filePath_;
    QPixmap   pm0_{}, pm1_{};
    QMap<PixmapCacheKey, QPixmap> cache_;
};


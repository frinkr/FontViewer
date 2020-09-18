#pragma once

#include <chrono>
#include <tuple>
#include <QSplashScreen>

class QVariantAnimation;

class QXSplash : public QSplashScreen {
    Q_OBJECT
public:
    explicit QXSplash(QScreen * parent = nullptr);
    
    void
    dismiss(QWidget * widget);

    void
    showProgress(int value, int maximum, const QString & message);

protected:
    void
    drawContents(QPainter * painter) override;

    void
    mousePressEvent(QMouseEvent * event) override;

    void
    mouseMoveEvent(QMouseEvent * event) override;

    void
    mouseReleaseEvent(QMouseEvent * event) override;

protected:
    QPoint mousePressPos_{};
    bool   movingWindow_{false};
    std::tuple<int, int> progress_{};
    QString message_;
    QVariantAnimation * ani_{ nullptr };
    std::chrono::high_resolution_clock::time_point lastUpdate_{};
};

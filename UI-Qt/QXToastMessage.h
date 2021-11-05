#pragma once

#include <QWidget>
class QLabel;
class QTimer;
class QPropertyAnimation;

class QXToastMessage: public QWidget {
public:
    explicit QXToastMessage(QWidget * parent = nullptr);

    void
    showToParent(const QIcon & icon, const QString & message);

protected:
    void
    showEvent(QShowEvent * event) override;

    void
    enterEvent(QEnterEvent * event) override;

    void
    leaveEvent(QEvent * event) override;

private slots:
    void
    fadeIn();

    void
    fadeOut();

    void
    startFadeOutTimer();

    void
    stopFadeOutTimer();

    void
    startCloseTimer();

    void
    stopCloseTimer();
private:
    QLabel        *icon_{nullptr};
    QLabel        *message_{nullptr};
    QPropertyAnimation * fadeOutAnimation_{nullptr};
    QTimer        *fadeOutTimer_{nullptr};
    QTimer        *closeTimer_{nullptr};
    qreal          opacity_{0.9};
};

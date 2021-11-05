#include <QBoxLayout>
#include <QGraphicsBlurEffect>
#include <QFrame>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QTimer>

#include "QXToastMessage.h"

QXToastMessage::QXToastMessage(QWidget * parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint) {

    QFrame * frame = new QFrame(this);
    icon_ = new QLabel("Hello", frame);
    icon_->setFixedHeight(30);
    icon_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    icon_->setAlignment(Qt::AlignCenter);
        
    message_ = new QLabel("world", frame);
    message_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    message_->setWordWrap(true);
    message_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    message_->setTextInteractionFlags(Qt::TextBrowserInteraction);
        
    QBoxLayout * frameLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    frameLayout->addWidget(icon_);
    frameLayout->addWidget(message_);    
    frame->setLayout(frameLayout);
    frame->setFrameStyle(QFrame::Panel | QFrame::Raised);;
    frame->setStyleSheet(".QFrame {border: 2px solid palette(midlight); border-radius: 8px; background-color: palette(window);}");
        
    QBoxLayout * lo = new QBoxLayout(QBoxLayout::TopToBottom);
    lo->addWidget(frame);
    setLayout(lo);
    
    setMinimumSize(300, 100);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
}

void
QXToastMessage::showToParent(const QIcon & icon, const QString & message) {
    icon_->setPixmap(icon.pixmap(std::min(icon_->width(), icon_->height())));
    message_->setText(message);
    
    QWidget * parentWindow = parentWidget()->window();
    move(QPoint(parentWindow->frameGeometry().center().x() - width() / 2,
                parentWindow->frameGeometry().bottom() - height() - 50));
    show();

    startCloseTimer();
    startFadeOutTimer();
}

void
QXToastMessage::showEvent(QShowEvent * event) {
    QWidget::showEvent(event);
    fadeIn();
}

void
QXToastMessage::enterEvent(QEnterEvent * event) {
    QWidget::enterEvent(event);
    stopFadeOutTimer();
    stopCloseTimer();
    setWindowOpacity(opacity_);
}

void
QXToastMessage::leaveEvent(QEvent * event) {
    QWidget::leaveEvent(event);
    startCloseTimer();
    startFadeOutTimer();
}

void
QXToastMessage::fadeIn() {
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(opacity_);
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void
QXToastMessage::fadeOut() {
    fadeOutAnimation_ = new QPropertyAnimation(this, "windowOpacity");
    fadeOutAnimation_->setStartValue(opacity_);
    fadeOutAnimation_->setEndValue(0.0);
    fadeOutAnimation_->setDuration(1000);
    fadeOutAnimation_->start(QAbstractAnimation::DeleteWhenStopped);
}

void
QXToastMessage::startFadeOutTimer() {
    stopFadeOutTimer();
    
    fadeOutTimer_ = new QTimer(this);
    fadeOutTimer_->setSingleShot(true);
    fadeOutTimer_->callOnTimeout(this, &QXToastMessage::fadeOut);
    fadeOutTimer_->start(2000);
}

void
QXToastMessage::stopFadeOutTimer() {
    if (fadeOutTimer_) {
        fadeOutTimer_->stop();
        delete fadeOutTimer_;
        fadeOutTimer_ = nullptr;
    }

    if (fadeOutAnimation_) {
        fadeOutAnimation_->stop();
        delete fadeOutAnimation_;
        fadeOutAnimation_ = nullptr;
    }
}

void
QXToastMessage::startCloseTimer() {
    stopCloseTimer();
    
    closeTimer_ = new QTimer(this);
    closeTimer_->setSingleShot(true);
    closeTimer_->callOnTimeout(this, &QXToastMessage::close);
    closeTimer_->start(3000);
}

void
QXToastMessage::stopCloseTimer() {
    if (closeTimer_) {
        closeTimer_->stop();
        delete closeTimer_;
        closeTimer_ = nullptr;
    }
}

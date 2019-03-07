#include <QBoxLayout>
#include <QFrame>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QTimer>

#include "QXToastMessage.h"

QXToastMessage::QXToastMessage(QWidget * parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint) {

    QFrame * frame = new QFrame(this);
    icon_ = new QLabel("Hello", frame);
    icon_->setFixedHeight(30);
    icon_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    icon_->setAlignment(Qt::AlignCenter);
        
    message_ = new QLabel("world", frame);
    message_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    message_->setWordWrap(true);
    message_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QBoxLayout * frameLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    frameLayout->addWidget(icon_);
    frameLayout->addWidget(message_);    
    frame->setLayout(frameLayout);
    frame->setFrameStyle(QFrame::Panel | QFrame::Raised);;
    frame->setStyleSheet(".QFrame {border-radius: 6px; background-color: palette(window);}");
    
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
    move(parentWindow->frameGeometry().center() - QPoint(this->width() / 2,
                                                         this->height() - parentWindow->frameGeometry().height() / 2));
    show();

    QTimer::singleShot(2500, this, &QXToastMessage::fadeOut);
    QTimer::singleShot(3000, this, &QWidget::close);
}

void
QXToastMessage::showEvent(QShowEvent * event) {
    QWidget::showEvent(event);
    fadeIn();
}

void
QXToastMessage::fadeIn() {
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void
QXToastMessage::fadeOut() {
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

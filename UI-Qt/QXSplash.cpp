#include <QPainter>
#include <QMouseEvent>

#include "QXSplash.h"
#include "QXApplication.h"

QXSplash::QXSplash(QWidget * parent)
    : QSplashScreen(parent) {
    setPixmap(qApp->loadIcon(":/images/splash.png").pixmap(256, 256));
}

void
QXSplash::dismiss(QWidget * widget) {
    if (widget)
        finish(widget);
    else
        close();
}

void
QXSplash::showProgress(int value, int maximum, const QString & message) {
    progress_ = std::make_tuple(value, maximum);
    if (!message.isEmpty())
    showMessage(message, Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
}

void
QXSplash::drawContents(QPainter * painter) {
    QRect r = rect();
    painter->setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->drawPixmap(r.topLeft(), pixmap());

    const qreal progressRadius = rect().width() / 2.0 * 0.75;
    const QRect progressRect(r.center() - QPoint(progressRadius, progressRadius), 
        QSize(progressRadius * 2, progressRadius * 2));

    int val = std::get<0>(progress_);
    int max = std::get<1>(progress_);

    // Draw app icon
    QRect iconRect = progressRect.adjusted(10, 10, -10, -10);
    if (max) {
        int size = progressRect.height() / 3;
        iconRect = QRect(QPoint(progressRect.center().x() - size / 2, progressRect.top() + 20), QSize(size, size));
    }
    QPixmap icon = qApp->loadIcon(":/images/splash-text.png").pixmap(iconRect.size());
    painter->drawPixmap(iconRect, icon);
    

    // Draw progress
    if (max) {
        painter->setPen(QPen(palette().highlight().color(), 5));
        painter->drawArc(progressRect, 0, val / qreal(max) * 360 * 16);
    }

    // Draw message
    QString msg = message();
    if (!msg.isEmpty()) {
        QFont font = painter->font();
        font.setPointSize(15);
        painter->setFont(font);
        painter->setPen(Qt::white);
        QRect msgRect(rect().left(), progressRect.top() + progressRect.height() / 3, rect().width(), progressRect.height() * 2 / 3);
        painter->drawText(rect().adjusted(0, 20, 0, 0), Qt::AlignHCenter | Qt::AlignVCenter, msg);
    }
}

void
QXSplash::mousePressEvent(QMouseEvent * event) {
    if ((event->buttons() & Qt::LeftButton)) {
        mousePressPos_ = event->pos();
        movingWindow_ = true;
    }
}

void
QXSplash::mouseMoveEvent(QMouseEvent * event) {
    if (movingWindow_ && (event->buttons() & Qt::LeftButton)) {
        QPoint diff = event->pos() - mousePressPos_;
        QPoint newpos = this->window()->pos() + diff;
        this->window()->move(newpos);
    }
}

void
QXSplash::mouseReleaseEvent(QMouseEvent * event) {
    movingWindow_ = false;
}

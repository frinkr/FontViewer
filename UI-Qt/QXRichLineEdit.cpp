#include <QKeyEvent>
#include <QStyleOptionFrameV2>
#include <QApplication>
#include "QXRichLineEdit.h"


QXRichLineEdit::QXRichLineEdit(QWidget * parent)
    : QTextEdit(parent) {
    setTabChangesFocus(true);
    setWordWrapMode(QTextOption::NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(sizeHint().height());
}

void
QXRichLineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        event->ignore();
    else 
        QTextEdit::keyPressEvent(event);
}

QSize
QXRichLineEdit::sizeHint() const {
    QFontMetrics fm(font());
    int h = qMax(fm.height(), 14) + 4;
    int w = fm.width(QLatin1Char('x')) * 17 + 4;
    QStyleOptionFrameV2 opt;
    opt.initFrom(this);
    return (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h).
                                      expandedTo(QApplication::globalStrut()), this));
}


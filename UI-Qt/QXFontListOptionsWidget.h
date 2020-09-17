#ifndef QXFONTLISTOPTIONSWIDGET_H
#define QXFONTLISTOPTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class QXFontListOptionsWidget;
}

class QXFontListOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QXFontListOptionsWidget(QWidget *parent = 0);
    ~QXFontListOptionsWidget();

private:
    Ui::QXFontListOptionsWidget *ui;
};

#endif // QXFONTLISTOPTIONSWIDGET_H

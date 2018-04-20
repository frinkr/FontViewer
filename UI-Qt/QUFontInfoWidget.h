#ifndef QUFONTINFOWIDGET_H
#define QUFONTINFOWIDGET_H

#include <QWidget>

namespace Ui {
class QUFontInfoWidget;
}

class QUFontInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QUFontInfoWidget(QWidget *parent = 0);
    ~QUFontInfoWidget();

private:
    Ui::QUFontInfoWidget *ui;
};

#endif // QUFONTINFOWIDGET_H

#pragma once

#include <QWidget>
#include <QWindow>
#include <QMainWindow>

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


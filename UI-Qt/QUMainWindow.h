#pragma once
#include <QMainWindow>

namespace Ui {
class QUMainWindow;
}

class QUMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QUMainWindow(QWidget *parent = 0);
    ~QUMainWindow();

private:
    Ui::QUMainWindow *ui;
};

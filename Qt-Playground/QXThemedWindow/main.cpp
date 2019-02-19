#include "mainwindow.h"
#include <QDialog>
#include <QApplication>
#include "QXThemedWindow.h"
#include "framelesswindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CFramelessWindow w;
    w.show();
    MainWindow m;
    m.show();
    QXThemedWindow<QDialog> d;
    d.exec();
    return a.exec();
}

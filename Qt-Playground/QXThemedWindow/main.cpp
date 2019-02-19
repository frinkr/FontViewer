#include "mainwindow.h"
#include <QApplication>

#include "framelesswindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CFramelessWindow w;
    w.show();

    return a.exec();
}

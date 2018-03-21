#include <QApplication>
#include <QPushButton>
#include "QUApp.h"
#include "QUMainWindow.h"
int QUApp::Run(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QUMainWindow window;
    window.show();

    return a.exec();
}
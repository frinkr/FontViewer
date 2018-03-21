#include <QApplication>
#include <QPushButton>
#include "QUApp.h"

int QUApp::Run(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QWidget window;

    window.resize(250, 150);
    window.setWindowTitle("Simple example");
    window.show();

    return a.exec();
}
#include <QApplication>
#include <QPalette>
#include <QMenuBar>
#include <QStyleFactory>
#include "QXTheme.h"

#ifdef Q_OS_WIN
#  include <Windows.h>
#endif

void
QXTheme::applyDarkFusion() {
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    qApp->setPalette(darkPalette);
}

void
QXTheme::applyDarkFusionOnMenuBar(QMenuBar * menuBar) {
    QString style = QString("background-color:%1;").arg("#353535");
    menuBar->setStyleSheet(style);
    for (QAction * action : menuBar->actions()) {
        if (QMenu * menu = action->menu()) {
            menu->setStyleSheet(style);
        }
    }
}

void
QXTheme::applyDarkFusionOnWindowTitleBar() {
#ifdef Q_OS_WIN
    int aElements[2] = { COLOR_INACTIVECAPTION, COLOR_ACTIVECAPTION };
    DWORD aOldColors[2];
    DWORD aNewColors[2];

    aOldColors[0] = GetSysColor(aElements[0]);
    aOldColors[1] = GetSysColor(aElements[1]);
    aNewColors[0] = RGB(0x80, 0x80, 0x80);  // light gray
    aNewColors[1] = RGB(0x80, 0x00, 0x80);  // dark purple

    SetSysColors(2, aElements, aNewColors);
#endif
}
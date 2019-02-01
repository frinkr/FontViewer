#include <QApplication>
#include <QMenuBar>
#include <QPalette>
#include <QStyleFactory>
#include <QToolBar>

#include "QXTheme.h"

#ifdef Q_OS_WIN
#  include <Windows.h>
#endif

namespace {
    QPalette
    darkFusionPalette() {
        QPalette p;

        // Normal
        p.setColor(QPalette::Window,          QColor(53, 53, 53));
        p.setColor(QPalette::Background,      QColor(53, 53, 53));
        p.setColor(QPalette::WindowText,      Qt::white);
        p.setColor(QPalette::Foreground,      Qt::white);
        p.setColor(QPalette::Base,            QColor(42, 42, 42));
        p.setColor(QPalette::AlternateBase,   QColor(66, 66, 66));
        p.setColor(QPalette::ToolTipBase,     Qt::white);
        p.setColor(QPalette::ToolTipText,     Qt::white);
        p.setColor(QPalette::PlaceholderText, QColor(42, 130, 218));
        p.setColor(QPalette::Text,            Qt::white);
        p.setColor(QPalette::Button,          QColor(53, 53, 53));
        p.setColor(QPalette::ButtonText,      Qt::white);
        p.setColor(QPalette::BrightText,      Qt::white);

        p.setColor(QPalette::Light,           QColor(73, 73, 73));
        p.setColor(QPalette::Midlight,        QColor(63, 63, 63));
        p.setColor(QPalette::Dark,            QColor(35, 35, 35));
        p.setColor(QPalette::Mid,             QColor(44, 44, 44));
        p.setColor(QPalette::Shadow,          QColor(20, 20, 20));

        p.setColor(QPalette::Highlight,       QColor(42, 130, 218));
        p.setColor(QPalette::HighlightedText, Qt::white);

        p.setColor(QPalette::Link,            QColor(42, 130, 218));
        p.setColor(QPalette::LinkVisited,     QColor(42, 130, 218));

        // Disabled
        p.setColor(QPalette::Disabled, QPalette::ButtonText,      QColor(127, 127, 127));
        p.setColor(QPalette::Disabled, QPalette::Highlight,       QColor(80, 80, 80));
        p.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
        p.setColor(QPalette::Disabled, QPalette::Text,            QColor(127, 127, 127));
        p.setColor(QPalette::Disabled, QPalette::WindowText,      QColor(127, 127, 127));

        return p;
    }
}

void
QXTheme::applyDarkFusion() {
    qApp->setStyle(QStyleFactory::create("Fusion"));
    qApp->setPalette(darkFusionPalette());
}

void
QXTheme::applyDarkFusionOnMenuBar(QMenuBar * menuBar) {
    QPalette p = darkFusionPalette();
    QString style = QString("background-color:%1;color:%2")
        .arg(p.color(QPalette::Window).name(QColor::HexRgb))
        .arg(p.color(QPalette::WindowText).name(QColor::HexRgb));
    
    menuBar->setStyleSheet(style);
    for (QAction * action : menuBar->actions()) {
        if (QMenu * menu = action->menu()) {
            menu->setStyleSheet(style);
        }
    }
}

void
QXTheme::applyDarkFusionOnToolBar(QToolBar * toolBar) {
    toolBar->setStyleSheet("QToolBar { border: 0px }");
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


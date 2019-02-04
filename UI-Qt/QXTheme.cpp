#include <QApplication>
#include <QMenuBar>
#include <QPalette>
#include <QStyleFactory>
#include <QToolBar>

#include <memory>
#include "QXTheme.h"

namespace {
    class QXSystemTheme : public QXTheme {
    public:
        static QString
        staticName() {
            return tr("System");
        }

        QString
        name() override {
            return staticName();
        }
    };

    class QXFusionTheme : public QXTheme {
    public:
        static QString
        staticName() {
            return tr("Fusion");
        }

        QString
        name() override {
            return staticName();
        }

        void
        applyToApplication() override {
            qApp->setStyle(QStyleFactory::create("Fusion"));
        }
    };

    class QXFusionColorTheme: public QXTheme {
    public:
        void
        applyToApplication() override {
            qApp->setStyle(QStyleFactory::create("Fusion"));
            qApp->setPalette(palette());
        }

        void
        applyToMenuBar(QMenuBar * menuBar) override {
#if defined(Q_OS_LINUX)
            QPalette p = palette();
            QString menuBarStyle = QString(" \
                QMenuBar { background-color:%1;color:%2; }  \
                QMenuBar::item::selected {background: %3; } \
                ")
                .arg(p.color(QPalette::Window).name(QColor::HexRgb))
                .arg(p.color(QPalette::WindowText).name(QColor::HexRgb))
                .arg(p.color(QPalette::Highlight).name(QColor::HexRgb));
    
            menuBar->setStyleSheet(menuBarStyle);
            return;

            for (QAction * action : menuBar->actions()) {
                if (QMenu * menu = action->menu()) {
                    menu->setStyleSheet(menuBarStyle);
                }
            }
#endif
        }

        void
        applyToToolBar(QToolBar * toolBar) override {
            toolBar->setStyleSheet("QToolBar { border: 0px }");
        }

    protected:
        virtual QPalette
        palette() = 0;
    };

    class QXDarkFusionTheme : public QXFusionColorTheme {
    public:
        static QString
        staticName() {
            return tr("Dark Fusion");
        }

        QString
        name() override {
            return staticName();
        }

    protected:
        QPalette
        palette() override {
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
            //p.setColor(QPalette::PlaceholderText, QColor(42, 130, 218));
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
    };

    class QXBlueFusionTheme : public QXFusionColorTheme {
    public:
        static QString
        staticName() {
            return tr("Blue Fusion");
        }

        QString
        name() override {
            return staticName();
        }

    protected:
        QPalette
        palette() override {
            QPalette p;

            // Normal
            p.setColor(QPalette::Window,          QColor("#1976D2"));
            p.setColor(QPalette::Background,      QColor("#1976D2"));
            p.setColor(QPalette::WindowText,      Qt::white);
            p.setColor(QPalette::Foreground,      Qt::white);
            p.setColor(QPalette::Base,            QColor("#2086E2"));
            p.setColor(QPalette::AlternateBase,   QColor("#BBDEFB"));
            p.setColor(QPalette::ToolTipBase,     Qt::white);
            p.setColor(QPalette::ToolTipText,     Qt::white);
            p.setColor(QPalette::Text,            Qt::white);
            p.setColor(QPalette::Button,          QColor("#2086E2"));
            p.setColor(QPalette::ButtonText,      Qt::white);
            p.setColor(QPalette::BrightText,      Qt::white);
            p.setColor(QPalette::Light,           QColor("#2691f1"));
            p.setColor(QPalette::Midlight,        QColor("#268ae5"));
            p.setColor(QPalette::Dark,            QColor("#2080d6"));
            p.setColor(QPalette::Mid,             QColor("#2286e0"));
            p.setColor(QPalette::Shadow,          QColor("#0066cc"));
            p.setColor(QPalette::Highlight,       QColor("#82c139"));
            p.setColor(QPalette::HighlightedText, Qt::white);// QColor("#212121"));
            p.setColor(QPalette::Link,            QColor("#82c139"));
            p.setColor(QPalette::LinkVisited,     QColor("#72d129"));

            // Disabled
            p.setColor(QPalette::Disabled, QPalette::ButtonText,      QColor(200, 200, 200));
            p.setColor(QPalette::Disabled, QPalette::Highlight,       QColor(168, 168, 168));
            p.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(200, 200, 200));
            p.setColor(QPalette::Disabled, QPalette::Text,            QColor(200, 200, 200));
            p.setColor(QPalette::Disabled, QPalette::WindowText,      QColor(200, 200, 200));

            return p;
        }
    };


    std::unique_ptr<QXTheme> currentTheme_;
}

QXTheme *
QXTheme::current() {
    return currentTheme_.get();   
}

QStringList
QXTheme::availableThemes() {
    QStringList list;
    list << QXSystemTheme::staticName()
         << QXFusionTheme::staticName()
         << QXDarkFusionTheme::staticName()
         << QXBlueFusionTheme::staticName();
    return list;
}

void
QXTheme::setCurrent(const QString & current) {
    currentTheme_.reset(getTheme(current));
    currentTheme_->applyToApplication();
}

QXTheme *
QXTheme::getTheme(const QString & name) {
    if (name == QXSystemTheme::staticName())
        return new QXSystemTheme;
    if (name == QXFusionTheme::staticName())
        return new QXFusionTheme;
    if (name == QXDarkFusionTheme::staticName())
        return new QXDarkFusionTheme;
    if (name == QXBlueFusionTheme::staticName())
        return new QXBlueFusionTheme;
    return new QXSystemTheme;;
}

#include <QApplication>
#include <QMenuBar>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QToolBar>
#include <QSettings>

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

        void
        applyToApplication() override {
            QStyle * style {};
#if defined(Q_OS_WIN)
            style = QStyleFactory::create("windowsvista");
#elif defined(Q_OS_MAC)
            style = QStyleFactory::create("mac");
#endif
            if (style) {
                qApp->setStyle(style);
                qApp->setPalette(style->standardPalette());
            }
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
            auto style = QStyleFactory::create("Fusion");

            qApp->setStyle(style);// QStyleFactory::create("Fusion"));
            qApp->setPalette(style->standardPalette());
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
            p.setColor(QPalette::WindowText,      Qt::white);
            p.setColor(QPalette::Base,            QColor(42, 42, 42));
            p.setColor(QPalette::AlternateBase,   QColor(53, 53, 53));
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

            // Inactive
            p.setColor(QPalette::Inactive, QPalette::Highlight,       QColor(80, 80, 80));

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
            p.setColor(QPalette::WindowText,      Qt::white);
            p.setColor(QPalette::Base,            QColor("#2086E2"));
            p.setColor(QPalette::AlternateBase,   QColor("#368fe7"));
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

            // Inactive
            p.setColor(QPalette::Inactive, QPalette::Highlight,       QColor("#6fa531"));
            return p;
        }
    };


    class QXBlackFusionTheme : public QXFusionColorTheme {
    public:
        static QString
        staticName() {
            return tr("Black Fusion");
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
            p.setColor(QPalette::Window,          gray(0x00));
            p.setColor(QPalette::WindowText,      gray(0xFF));
            p.setColor(QPalette::Base,            gray(0x10));
            p.setColor(QPalette::AlternateBase,   gray(0x20));
            p.setColor(QPalette::ToolTipBase,     gray(0xFF));
            p.setColor(QPalette::ToolTipText,     gray(0xFF));
            p.setColor(QPalette::Text,            gray(0xFF));
            p.setColor(QPalette::Button,          gray(0x20));

            p.setColor(QPalette::ButtonText,      gray(0xFF));
            p.setColor(QPalette::BrightText,      gray(0xFF));
            p.setColor(QPalette::Light,           gray(0x30));
            p.setColor(QPalette::Midlight,        gray(0x28));
            p.setColor(QPalette::Dark,            gray(0x10));
            p.setColor(QPalette::Mid,             gray(0x18));
            p.setColor(QPalette::Shadow,          gray(0x05));
            p.setColor(QPalette::Highlight,       QColor(42, 130, 218));
            p.setColor(QPalette::HighlightedText, gray(0xFF));
            p.setColor(QPalette::Link,            QColor(42, 130, 218));
            p.setColor(QPalette::LinkVisited,     QColor(42, 130, 218));

            // Disabled
            p.setColor(QPalette::Disabled, QPalette::ButtonText,      Qt::gray); 
            p.setColor(QPalette::Disabled, QPalette::Highlight,       Qt::darkGray);
            p.setColor(QPalette::Disabled, QPalette::HighlightedText, Qt::gray);
            p.setColor(QPalette::Disabled, QPalette::Text,            Qt::gray);
            p.setColor(QPalette::Disabled, QPalette::WindowText,      Qt::gray);

            // Inactive
            p.setColor(QPalette::Inactive, QPalette::Highlight,       gray(0x40));
            return p;
        }

        static QColor
        gray(int level) {
            return QColor(level, level,level);
        }
    };

    class QXThemeCreatorInterface {
    public:
        virtual ~QXThemeCreatorInterface() {}

        virtual QString
        themeName() const = 0;
        
        virtual QXTheme *
        createTheme() const = 0;
    };
    
    template <class Theme>
    class QXThemeCreator : public QXThemeCreatorInterface{
    public:

        QString
        themeName() const override {
            return Theme::staticName();
        }
        
        QXTheme *
        createTheme() const override {
            return new Theme;
        }
    };

    const QList<QXThemeCreatorInterface*> &
    themeCreatorList() {
        static QList<QXThemeCreatorInterface*> list;
        if (list.empty()) {
            list.append(new QXThemeCreator<QXSystemTheme>);
            list.append(new QXThemeCreator<QXFusionTheme>);
            list.append(new QXThemeCreator<QXDarkFusionTheme>);
            list.append(new QXThemeCreator<QXBlueFusionTheme>);
            list.append(new QXThemeCreator<QXBlackFusionTheme>);
        }
        return list;
    }
        

    std::unique_ptr<QXTheme> currentTheme_;
}

QXTheme *
QXTheme::current() {
    return currentTheme_.get();   
}

QStringList
QXTheme::availableThemes() {
    QStringList list;
    for (auto themeCreator: themeCreatorList()) 
        list << themeCreator->themeName();

    return list;
}

void
QXTheme::setCurrent(const QString & current) {
    currentTheme_.reset(getTheme(current));
    currentTheme_->applyToApplication();
}

QXTheme *
QXTheme::getTheme(const QString & name) {
    for (auto themeCreator : themeCreatorList()) {
        if (themeCreator->themeName() == name)
            return themeCreator->createTheme();
    }
    return new QXSystemTheme;;
}

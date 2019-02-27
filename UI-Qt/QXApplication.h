#pragma once

#include <QApplication>
#include <QIcon>

class QEvent;

namespace QXEvent {
    extern const QEvent::Type OpenFontDialog;
}

class QXApplication : public QApplication {
public:
    QXApplication(int & argc, char ** argv);
    ~QXApplication();

	bool
	darkMode() const;

	QIcon
	loadIcon(const QString & path);

    bool
    userRequiredToResetAppData() const;

    bool
    question(QWidget * parent, const QString & title, const QString & text) const;

    void
    warning(QWidget * parent, const QString & title, const QString & text) const;

    void
    postCustomEvent(QObject * receiver,
                    QEvent::Type eventType,
                    int priority = Qt::NormalEventPriority,
                    bool removePostedEvents = false);

public slots:
    void
    about() const;

    void
    aboutFonts() const;

    void
    preferences() const;

private:
#ifdef Q_OS_MAC
    bool event(QEvent * event);
#endif

private:
    QMap<QString, QIcon> iconCache_;
};

#if defined(qApp)
#undef qApp
#endif

#define qApp (static_cast<QXApplication *>(QCoreApplication::instance()))


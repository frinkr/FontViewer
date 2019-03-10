#pragma once

#include <QApplication>
#include <QIcon>

class QEvent;
class QXSplash;

class QXApplication : public QApplication
{
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

public slots:
    void
    showSplashScreen();

    void
    dismissSplashScreen(QWidget * widget = nullptr);
    
    void
    splashScreenShowProgress(int value, int maximum, const QString & message);
                                                              
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
    QXSplash           * splash_ {nullptr};
};

#if defined(qApp)
#undef qApp
#endif

#define qApp (static_cast<QXApplication *>(QCoreApplication::instance()))


#pragma once

#include <QApplication>
#include <QIcon>

class QEvent;

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
    userRequiredToResetAppData();

public slots:
    void
    about() const;

    void
    preferences() const;

private:
#ifdef Q_OS_MAC
    bool event(QEvent *);
#endif

private:
    QMap<QString, QIcon> iconCache_;
};

#if defined(qApp)
#undef qApp
#endif

#define qApp (static_cast<QXApplication *>(QCoreApplication::instance()))


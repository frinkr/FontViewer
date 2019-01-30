#pragma once

#include <QGuiApplication>
#include <QIcon>
#include "QUSingleApplication.h"

class QEvent;

class QUApplication : public QUSingleApplication
{
public:
    QUApplication(int & argc, char ** argv);
    ~QUApplication();

	bool
	darkMode() const;

	QIcon
	loadIcon(const QString & path);

public slots:
    void
    about() const;

private:
#ifdef Q_OS_MAC
    bool event(QEvent *);
#endif

private:
    QMap<QString, QIcon> iconCache_;
};

extern QUApplication * quApp;



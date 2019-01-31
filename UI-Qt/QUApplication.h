#pragma once

#include <QApplication>
#include <QIcon>

class QEvent;

class QUApplication : public QApplication
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

    void
    preferences() const;

private:
#ifdef Q_OS_MAC
    bool event(QEvent *);
#endif

private:
    QMap<QString, QIcon> iconCache_;
};

extern QUApplication * quApp;



#pragma once
#include <QObject>

class QMenuBar;
class QToolBar;

class QXTheme : public QObject{
    Q_OBJECT
public:
    static QXTheme *
    current();

    static QStringList
    availableThemes();

    static void
    setCurrent(const QString & current);

private:
    static QXTheme *
    getTheme(const QString & name);

public:
    using QObject::QObject;

    virtual QString
    name() = 0;

    virtual void
    applyToApplication() = 0;

    virtual void
    applyToMenuBar(QMenuBar * menuBar) = 0;

    virtual void
    applyToToolBar(QToolBar * toolBar) = 0;

    virtual void
    applyToWindowTitleBar(QWidget * widget) = 0;
};

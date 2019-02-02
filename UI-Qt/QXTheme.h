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
    applyToApplication() {}

    virtual void
    applyToMenuBar(QMenuBar * menuBar) { Q_UNUSED(menuBar); }

    virtual void
    applyToToolBar(QToolBar * toolBar) { Q_UNUSED(toolBar); }

    virtual void
    applyToWindowTitleBar(QWidget * widget) { Q_UNUSED(widget); }
};

#pragma once
#include <QObject>
#include "QXDocument.h"

class QXPreferences : public QObject {
    Q_OBJECT

public:
    static void
    load();

    static void
    save();
    
    static void
    reset();

    static QStringList
    userFontFolders();

    static void
    setUserFontFolders(const QStringList & folders);

    static QList<QXRecentFontItem>
    recentFonts();

    static void
    setRecentFonts(const QList<QXRecentFontItem> & recentFonts);

    static QString
    theme();

    static void
    setTheme(const QString & theme);

    static std::string
    fontNameLanguage();

private:
    using QObject::QObject;
};

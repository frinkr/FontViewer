#include <QSettings>
#include <QStringList>

#include "QXPreferences.h"

void
QXPreferences::resetPreferences() {
    QSettings settings;
    settings.clear();
}

QStringList
QXPreferences::userFontFolders() {
    QSettings settings;
    QVariant  data = settings.value("userFontFolders");
    if (data.canConvert<QStringList>())
        return data.value<QStringList>();
    return QStringList();
}

void
QXPreferences::setUserFontFolders(const QStringList & folders) {
    QSettings settings;
    settings.setValue("userFontFolders", folders);
}


QList<QXRecentFontItem>
QXPreferences::recentFonts() {
    QList<QXRecentFontItem> recentFonts;
    qRegisterMetaTypeStreamOperators<QXRecentFontItem>("QXRecentFontItem");

    QSettings settings;
    QList<QVariant> variantList = settings.value("recentFonts").toList();
    foreach(QVariant v, variantList) {
        if (v.canConvert<QXRecentFontItem>())
            recentFonts.append(v.value<QXRecentFontItem>());
    }
    return recentFonts;
}

void
QXPreferences::setRecentFonts(const QList<QXRecentFontItem> & recentFonts) {
    qRegisterMetaTypeStreamOperators<QXRecentFontItem>("QXRecentFontItem");

    QSettings settings;
    QList<QVariant> variantList;
    foreach(QXRecentFontItem uri, recentFonts)
        variantList.append(QVariant::fromValue(uri));

    settings.setValue("recentFonts", variantList);
}

QString
QXPreferences::theme() {
    QSettings settings;
    QVariant  data = settings.value("theme");
    if (data.canConvert<QString>())
        return data.value<QString>();
    return QString();
}

void
QXPreferences::setTheme(const QString & theme) {
    QSettings settings;
    settings.setValue("theme", theme);
}

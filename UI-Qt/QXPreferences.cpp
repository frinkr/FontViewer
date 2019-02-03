#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QStandardPaths>
#include <QStringList>
#include <QTextStream>
#include "QXPreferences.h"

namespace {
    QJsonObject json;

    QString
    jsonFilePath() {
        QDir folder(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)));
        if (!folder.exists())
            folder.mkpath(".");
        return folder.filePath("FontViewer.prefs");
    }
}

void
QXPreferences::load() {
    QJsonDocument doc;
    QFile file(jsonFilePath());
    if (file.open(QFile::ReadOnly)) 
        doc = QJsonDocument::fromJson(file.readAll());
    json = doc.object();
}

void
QXPreferences::save() {
    QFile file(jsonFilePath());
    if (file.open(QFile::WriteOnly)) {
        QJsonDocument doc(json);
        file.write(doc.toJson());
    }
}


void
QXPreferences::reset() {
    json = QJsonObject();
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
    qRegisterMetaType<QXFontURI>();
    qRegisterMetaType<QXRecentFontItem>();
    qRegisterMetaTypeStreamOperators<QXRecentFontItem>("QXRecentFontItem");

    QList<QXRecentFontItem> recentFonts;
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

    qRegisterMetaType<QXFontURI>();
    qRegisterMetaType<QXRecentFontItem>();
    qRegisterMetaTypeStreamOperators<QXRecentFontItem>("QXRecentFontItem");

    QSettings settings;
    QList<QVariant> variantList;
    foreach(QXRecentFontItem uri, recentFonts)
        variantList.append(QVariant::fromValue(uri));

    settings.setValue("recentFonts", variantList);
}

QString
QXPreferences::theme() {
    if (json.contains("theme") && json["theme"].isString())
        return json["theme"].toString();
    return QString();
}

void
QXPreferences::setTheme(const QString & theme) {
    json["theme"] = theme;
}

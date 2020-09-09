#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
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

    QJsonObject
    toJsonObject(const QXRecentFontItem & item) {
        QJsonObject obj;
        obj["filePath"]  = item.filePath;
        obj["faceIndex"] = static_cast<double>(item.faceIndex);
        obj["fullName"]  = item.fullName;
        return obj;
    }

    QXRecentFontItem
    toQXRecentFontItem(const QJsonObject & obj) {
        QXRecentFontItem item;
        if (obj["filePath"].isString())
            item.filePath = obj["filePath"].toString();
        if (obj["faceIndex"].isDouble())
            item.faceIndex = static_cast<size_t>(obj["faceIndex"].toDouble());
        if (obj["fullName"].isString())
            item.fullName = obj["fullName"].toString();
        return item;
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
    QStringList list;
    if (json["userFontFolders"].isArray()) {
        QJsonArray array = json["userFontFolders"].toArray();
        for (const auto value: array) {
            if (value.isString())
                list.append(value.toString());
        }
    }
    return list;
}

void
QXPreferences::setUserFontFolders(const QStringList & folders) {
    QJsonArray array;
    for (const auto & folder: folders)
        array.append(folder);
    json["userFontFolders"] = array;
}


QList<QXRecentFontItem>
QXPreferences::recentFonts() {
    QList<QXRecentFontItem> list;
    if (json["recentFonts"].isArray()) {
        QJsonArray array = json["recentFonts"].toArray();
        for (const auto value: array) {
            if (value.isObject()) {
                auto item = toQXRecentFontItem(value.toObject());
                if (!item.fullName.isEmpty())
                    list.append(item);
            }
        }
    }
    return list;
}

void
QXPreferences::setRecentFonts(const QList<QXRecentFontItem> & recentFonts) {
    QJsonArray array;
    for (const auto & item: recentFonts)
        array.append(toJsonObject(item));
    json["recentFonts"] = array;
}

QString
QXPreferences::theme() {
    if (json["theme"].isString())
        return json["theme"].toString();
    return QString();
}

void
QXPreferences::setTheme(const QString & theme) {
    json["theme"] = theme;
}

std::string
QXPreferences::fontNameLanguage() {
    return "zh-cn";
}

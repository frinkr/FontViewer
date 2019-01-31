#include <QFile>
#include <QTextStream>

#include "QXHtmlTemplate.h"

QXHtmlTemplate *
QXHtmlTemplate::createFromFile(const QString & file, QObject * parent) {
    QXHtmlTemplate * t = new QXHtmlTemplate(parent);
    t->file_ = file;
    return t;
}

QXHtmlTemplate *
QXHtmlTemplate::createFromString(const QString & file, QObject * parent) {
    QXHtmlTemplate * t = new QXHtmlTemplate(parent);
    t->content_ = file;
    return t;
}

QXHtmlTemplate::QXHtmlTemplate(QObject * parent)
    : QObject(parent) {
};

QString
QXHtmlTemplate::instantialize(const QMap<QString, QVariant> & variables) const {
    if (content_.isEmpty()) 
        content_ = readTemplate();
    if (content_.isEmpty())
        return QString();

    QString instance(content_);
    for(const auto & key: variables.keys()) {
        QVariant value = variables.value(key);
        instance = instance.replace(QString("${%1}").arg(key), value.toString());
    }
    return instance;
}

QString
QXHtmlTemplate::readTemplate() const {
    QFile f(file_);
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&f);
        return in.readAll();
    }
    return QString();
}

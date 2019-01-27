#include <QFile>
#include <QTextStream>

#include "QUHtmlTemplate.h"

QUHtmlTemplate *
QUHtmlTemplate::createFromFile(const QString & file, QObject * parent) {
    QUHtmlTemplate * t = new QUHtmlTemplate(parent);
    t->file_ = file;
    return t;
}

QUHtmlTemplate *
QUHtmlTemplate::createFromString(const QString & file, QObject * parent) {
    QUHtmlTemplate * t = new QUHtmlTemplate(parent);
    t->content_ = file;
    return t;
}

QUHtmlTemplate::QUHtmlTemplate(QObject * parent)
    : QObject(parent) {
};

QString
QUHtmlTemplate::instantialize(const QMap<QString, QVariant> & variables) const {
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
QUHtmlTemplate::readTemplate() const {
    QFile f(file_);
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&f);
        return in.readAll();
    }
    return QString();
}

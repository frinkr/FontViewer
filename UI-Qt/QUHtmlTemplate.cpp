#include <QFile>
#include <QTextStream>

#include "QUHtmlTemplate.h"

QUHtmlTemplate::QUHtmlTemplate(const QString & file, QObject * parent)
    : QObject(parent)
    , file_(file)
{
};

const QString &
QUHtmlTemplate::file() const {
    return file_;
}

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

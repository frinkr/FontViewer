#pragma once

#include <QString>
#include <QMap>
#include <QVariant>

class QUHtmlTemplate : public QObject {
    Q_OBJECT
public:
    static QUHtmlTemplate *
    createFromFile(const QString & file, QObject * parent = nullptr);

    static QUHtmlTemplate *
    createFromString(const QString & file, QObject * parent = nullptr);

    QString
    instantialize(const QMap<QString, QVariant> & variables) const;

protected:
    QUHtmlTemplate(QObject * parent = nullptr);

protected:
    QString
    readTemplate() const;
protected:
    QString   file_;
    mutable QString   content_;
};

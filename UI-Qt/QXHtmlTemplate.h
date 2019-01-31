#pragma once

#include <QString>
#include <QMap>
#include <QVariant>

class QXHtmlTemplate : public QObject {
    Q_OBJECT
public:
    static QXHtmlTemplate *
    createFromFile(const QString & file, QObject * parent = nullptr);

    static QXHtmlTemplate *
    createFromString(const QString & file, QObject * parent = nullptr);

    QString
    instantialize(const QMap<QString, QVariant> & variables) const;

protected:
    QXHtmlTemplate(QObject * parent = nullptr);

protected:
    QString
    readTemplate() const;
protected:
    QString   file_;
    mutable QString   content_;
};

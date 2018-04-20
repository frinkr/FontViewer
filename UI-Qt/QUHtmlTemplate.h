#pragma once

#include <QString>
#include <QMap>
#include <QVariant>

class QUHtmlTemplate : public QObject {
    Q_OBJECT
public:
    QUHtmlTemplate(const QString & file, QObject * parent = nullptr);

    const QString &
    file() const;

    QString
    instantialize(const QMap<QString, QVariant> & variables) const;

protected:
    QString
    readTemplate() const;
protected:
    QString   file_;
    mutable QString   content_;
};

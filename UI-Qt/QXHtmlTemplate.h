#pragma once

#include <QString>
#include <QMap>
#include <QVariant>

#include "QXConv.h"

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


template <typename T>
struct QXHtmlTableTemplateStringTraits {
    static QString
    toQString(const T & v) {
        return QString("%1").arg(v);
    }
};

template <> struct QXHtmlTableTemplateStringTraits<bool> {
    static QString
    toQString(bool value) {
        return value? "Yes": "No";
    }
};

template <> struct QXHtmlTableTemplateStringTraits<FXString> {
    static QString
    toQString(const FXString & value) {
        return ::toQString(value);
    }
};

class QXHtmlTableTemplate : public QObject {
    Q_OBJECT
public:
    QXHtmlTableTemplate(QObject * parent = nullptr);

    QString
    html(); 

    template <typename T> void
    addDataRow(const QString & name, const T & value) {
        QString h = "                               \
            <tr>                                    \
              <td class=\"key\">%1:</td>            \
              <td>%2</td>                           \
            </tr>                                   \
            \n";

        htmlTableRows_ += h.arg(name).arg(QXHtmlTableTemplateStringTraits<T>::toQString(value));
    }

    void
    addHeadRow(const QString & text);

    void
    addEmptyRow();

    virtual void
    loadTableRows() {}

protected:
    QXHtmlTemplate   * htmlTemplate_;
    QString            htmlTableRows_;
};

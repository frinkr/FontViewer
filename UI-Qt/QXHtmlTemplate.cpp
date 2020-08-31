#include <QFile>
#include <QTextStream>

#include "QXHtmlTemplate.h"
#include "QXResources.h"

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

QXHtmlTableTemplate::QXHtmlTableTemplate(QObject * parent)
    : QObject(parent) {
    htmlTemplate_ = QXHtmlTemplate::createFromFile(QXResources::path("Html/template.html"), this);
}

QString
QXHtmlTableTemplate::html() {
    if (htmlTableRows_.isEmpty())
        loadTableRows();

    QMap<QString, QVariant> vars;
    vars["TABLE_ROWS"] = htmlTableRows_;
    return htmlTemplate_->instantialize(vars);
}

void
QXHtmlTableTemplate::addHeadRow(const QString & text) {
    if (!htmlTableRows_.isEmpty())
        addEmptyRow();
    
    QString h = "                                  \
            <tr>                                   \
              <td class=\"key\">                   \
                 <strong>%1</strong>               \
              </td>                                \
            </tr>                                  \
            \n";
    htmlTableRows_ += h.arg(text);
}

void
QXHtmlTableTemplate::addLongHeadRow(const QString & text) {
    if (!htmlTableRows_.isEmpty())
        addEmptyRow();
    
    QString h = "                                  \
            <tr>                                   \
              <td colspan=\"2\">                   \
                 <strong>%1</strong>               \
              </td>                                \
            </tr>                                  \
            \n";
    htmlTableRows_ += h.arg(text);
}


void
QXHtmlTableTemplate::addEmptyRow() {
    QString h = "                                  \
            <tr>                                   \
            </tr>                                  \
            \n";
    htmlTableRows_ += h;
}


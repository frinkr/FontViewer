#pragma once

#include <QWidget>

class QXDocument;

namespace Ui {
    class QXSearchWidget;
}

class QXSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QXSearchWidget(QWidget *parent = 0);
    ~QXSearchWidget();

    void
    setDocument(QXDocument * document);
                                      
public slots:
    void
    doSearch();
    
protected:
    virtual void
    focusInEvent(QFocusEvent *event);

protected slots:
    void
    onSearchResult(const QXSearchResult & result, const QString & text);
    
    void
    onSearchTextChanged(const QString & text);

protected:
    Ui::QXSearchWidget * ui_;
    QXDocument         * document_;
    QString              labelText_;
};


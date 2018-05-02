#pragma once

#include <QWidget>

class QUDocument;

namespace Ui {
    class QUSearchWidget;
}

class QUSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QUSearchWidget(QWidget *parent = 0);
    ~QUSearchWidget();

    void
    setDocument(QUDocument * document);
                                      
public slots:
    void
    doSearch();
    
protected:
    virtual void
    focusInEvent(QFocusEvent *event);

protected slots:
    void
    onSearchNotFound(const QString & text);
    
    void
    onSearchTextChanged(const QString & text);

protected:
    Ui::QUSearchWidget * ui_;
    QUDocument         * document_;
    QString              labelText_;
};


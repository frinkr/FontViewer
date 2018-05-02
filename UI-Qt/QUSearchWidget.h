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
private:
    Ui::QUSearchWidget * ui_;
    QUDocument         * document_;
};


#pragma once

#include "FontX/FXFace.h"

class QListWidget;
class QListWidgetItem;
class QXDocument;

class QXRelatedFontsWidget : public QWidget {
    Q_OBJECT
    
public:
    using QWidget::QWidget;

    void
    setDocument(QXDocument * document);

    void
    setCurrentFace(int index);

    int
    currentFace() const;
    
signals:
    void
    fontDoubleClicked(int i);

private slots:
    void
    onListItemDoubleClicked(QListWidgetItem * item);

    int
    itemData(QListWidgetItem * item) const;
    
private:
    void
    reload();
    
private:
    QListWidget * list_ {};
    QXDocument * document_ {};
};

#pragma once

#include "FontX/FXFace.h"
#include "QXDocument.h"

class QListWidget;
class QListWidgetItem;

class QXRelatedFontsWidget : public QWidget {
    Q_OBJECT
    
public:
    using QWidget::QWidget;

    void
    setDocument(QXDocument * document);

    void
    selectCurrentFace();
    
signals:
    void
    fontDoubleClicked(const QXFontURI & uri);

private slots:
    void
    onListItemDoubleClicked(QListWidgetItem * item);

    QXFontURI
    itemData(QListWidgetItem * item) const;
    
private:
    void
    reload();

    
private:
    QListWidget * list_ {};
    QXDocument * document_ {};
};

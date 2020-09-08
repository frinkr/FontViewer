#pragma once

#include <QDialog>
#include "FontX/FXFace.h"

namespace Ui {
    class QXFontCollectionDialog;
}

class QListWidget;
class QListWidgetItem;
class QXDocument;

class QXFontCollectionWidget : public QWidget {
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

class QXFontCollectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit QXFontCollectionDialog(const QString & filePath, FXPtr<FXFace> initFace = nullptr, QWidget * parent = nullptr);
    ~QXFontCollectionDialog();

    int
    selectedIndex() const;

    static int
    selectFontIndex(const QString & filePath, FXPtr<FXFace> initFace = nullptr);

private:
    Ui::QXFontCollectionDialog * ui_;
    QString filePath_;
};



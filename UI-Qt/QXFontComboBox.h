#pragma once
#include <QComboBox>
#include "QXDocument.h"

class QXFontListModel;
class QXSortFilterFontListModel;

class QXFontComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit QXFontComboBox(QWidget * parent = nullptr);

    int
    selectedFontIndex() const;

    QXFontURI
    selectedFont() const;

    int
    selectFont(int index);

    int
    selectFont(const QXFontURI & fontURI);

    void
    clearFilter();

signals:
    void
    fontSelected(const QXFontURI & fontURI, size_t index);

protected:
    QXSortFilterFontListModel *
    proxyModel() const;

    QXFontListModel *
    sourceModel() const;

    QModelIndex
    currentProxyIndex() const;
    
    QModelIndex
    currentSourceIndex() const;
    
protected:
    virtual void
    showPopup();
    
    virtual bool
    eventFilter(QObject *watched, QEvent *event);
                                                
private slots:
    void
    onFontSelected(int index);

    void
    onLineEdited(const QString & text);
};


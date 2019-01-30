#pragma once
#include <QComboBox>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include "FontX/FXFaceDatabase.h"
#include "QUDocument.h"

class QUFontListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit QUFontListModel(QObject * parent = nullptr);

    int
    rowCount(const QModelIndex & parent) const;
    
    QVariant
    data(const QModelIndex & index, int role) const;

    bool
    acceptRow(const QString & filter, int row) const;
public:
    FXPtr<FXFaceDatabase>
    db() const;

    const FXFaceAttributes &
    attributes(size_t index) const;

    QString
    displayName(size_t index) const;

    QIcon
    icon(size_t index) const;
};

class QUSortFilterFontListModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    bool
    lessThan(const QModelIndex & left, const QModelIndex & right) const;

    bool
    filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const;

public:
    void
    setFilter(const QString & text);

    void
    clearFilter();

protected:
    QUFontListModel *
    fontListModel() const;
protected:
    QString  filter_;
    
};

class QUFontComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit QUFontComboBox(QWidget * parent = nullptr);

    size_t
    selectedFontIndex() const;

    QUFontURI
    selectedFont() const;

    void
    selectFont(const QUFontURI & fontURI);

signals:
    void
    fontSelected(const QUFontURI & fontURI, size_t index);

protected:
    QUSortFilterFontListModel *
    proxyModel() const;

    QUFontListModel *
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


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
protected:
    FXPtr<FXFaceDatabase>
    db() const;

    const FXFaceAttributes &
    attributes(size_t index) const;

    QString
    displayName(size_t index) const;
};

class QUSortFilterFontListModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    bool
    lessThan(const QModelIndex & left, const QModelIndex & right) const;

    bool
    filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const;

protected:
    QUFontListModel *
    fontListModel() const;
};

class QUFontComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit QUFontComboBox(QWidget * parent = nullptr);

    QUFontURI
    selectedFont() const;
protected:
    QModelIndex
    currentProxyIndex() const;
    
    QModelIndex
    currentSourceIndex() const;

private slots:
    void
    onFontSelected(int index);
};


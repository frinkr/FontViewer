#pragma once
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include "FontX/FXFaceDatabase.h"

class QXFontListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit QXFontListModel(QObject * parent = nullptr);

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

class QXSortFilterFontListModel : public QSortFilterProxyModel {
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
    QXFontListModel *
    fontListModel() const;
protected:
    QString  filter_;
    
};


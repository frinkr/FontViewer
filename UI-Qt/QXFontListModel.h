#pragma once
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include "FontX/FXFaceDatabase.h"

struct QXFontListFilter {
    QString  fontName;
    QString  sampleText;
    bool     converAllSampleCharacters {false};
    
    void clear() {
        fontName.clear();
        sampleText.clear();
        converAllSampleCharacters = false;
    }
    
    bool isEmpty() const {
        return fontName.isEmpty() && (!converAllSampleCharacters && sampleText.isEmpty());
    }
};

class QXFontListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit QXFontListModel(QObject * parent = nullptr);

    int
    rowCount(const QModelIndex & parent) const;
    
    QVariant
    data(const QModelIndex & index, int role) const;

    bool
    acceptRow(const QXFontListFilter & filter, int row) const;
public:
    void
    setDb(FXPtr<FXFaceDatabase> db);

    FXPtr<FXFaceDatabase>
    db() const;

    const FXFaceAttributes &
    attributes(size_t index) const;

    QString
    displayName(size_t index) const;

    QIcon
    icon(size_t index) const;

private:
    FXPtr<FXFaceDatabase> db_ {};
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
    setFilter(const QXFontListFilter & filter);

    const QXFontListFilter &
    filter() const;
    
    void
    clearFilter();

protected:
    QXFontListModel *
    fontListModel() const;
protected:
    QXFontListFilter  filter_;
    
};


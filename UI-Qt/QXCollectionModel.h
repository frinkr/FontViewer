#pragma once

#include <QObject>

class QXCollectionModel;

struct QXCollectionModelIndex {
    int section;
    int item;

    bool
    operator==(const QXCollectionModelIndex & other) const {
        return section == other.section && item == other.item;
    }

    bool
    operator!=(const QXCollectionModelIndex & other) const {
        return !operator==(other);
    }

    bool
    isValid() const {
        return section != -1 && item != -1;
    }
};

class QXCollectionModel : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;

    virtual int
    sectionCount() const = 0;

    virtual int
    itemCount(int section) const = 0;

    virtual QVariant
    data(const QXCollectionModelIndex & index, int role) const = 0;

    virtual QVariant
    data(int section) const = 0;

signals:
    void
    reset();

    void
    beginResetModel();

    void 
    endResetModel();
};


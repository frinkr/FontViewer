#pragma once
#include <QAbstractListModel>

class QUGlyphListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit QUGlyphListModel(QObject * parent = nullptr);
    ~QUGlyphListModel();

    int rowCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int) const;
};


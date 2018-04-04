#pragma once
#include <QAbstractListModel>
#include <QStyledItemDelegate>
#include <QListView>

#include "FontX/FXFace.h"

class QUGlyphItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    QUGlyphItemDelegate(QObject * parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void
    paint(QPainter *painter,
          const QStyleOptionViewItem &option,
          const QModelIndex &index) const;
};


class QUGlyphListModel : public QAbstractListModel {
    
    Q_OBJECT
    
public:
    explicit QUGlyphListModel(FXPtr<FXFace> face,
                              QObject * parent = nullptr);

public:
    FXCMap
    currentCMap() const;
    
    FXPtr<FXCharBlock>
    currentBlock() const;

    QSize
    iconSize() const;

    void
    selectBlock(size_t index);
    
public:
    int
    rowCount(const QModelIndex &) const;
    
    QVariant
    data(const QModelIndex &, int) const;

protected:
    FXPtr<FXFace>    face_;
    size_t           blockIndex_;
};


class QUGlyphListView : public QListView {
    Q_OBJECT
public:
    explicit QUGlyphListView(QWidget * parent = nullptr);
};

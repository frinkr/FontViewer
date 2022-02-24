#pragma once

#include <QComboBox>
#include <QStringListModel>
#include <QStyledItemDelegate>

class QXShapingTextListModel: public QStringListModel {
public:
    using QStringListModel::QStringListModel;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

public:
    void
    clear();
    
public:
    static QStringList
    loadStrings();

    static void
    saveStrings(const QStringList & stringList);
    
    static QXShapingTextListModel &
    instance();
};

class QXShapingTextComboboxItemDelegate : public QStyledItemDelegate {
public:
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;
    using Parent = QStyledItemDelegate;
        
public:
    bool 
    editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index) override;
        
    QSize
    sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

        
    void
    paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

public:
    int
    heightForRow(int row) const;

    void
    fitPopupViewHeightToModel(QAbstractItemView * view, QAbstractItemModel * model) const;
    
    QRect
    closeButtonRect(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    void
    requestDeleteItem(const QModelIndex & index);
};

class QXShapingTextCombobox: public QComboBox {
public:
    Q_OBJECT;
    
public:
    using QComboBox::QComboBox;

public:
    void
    init();
};


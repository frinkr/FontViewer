#pragma once
#include <QListView>
#include <QStyledItemDelegate>

class QXGlyphItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    QXGlyphItemDelegate(QObject * parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void
    paint(QPainter *painter,
          const QStyleOptionViewItem &option,
          const QModelIndex &index) const;
};


class QXGlyphListView : public QListView {
    Q_OBJECT
public:
    explicit QXGlyphListView(QWidget * parent = nullptr);
};

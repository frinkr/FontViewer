#pragma once
#include <QListView>
#include <QStyledItemDelegate>

class QUGlyphItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    QUGlyphItemDelegate(QObject * parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void
    paint(QPainter *painter,
          const QStyleOptionViewItem &option,
          const QModelIndex &index) const;
};


class QUGlyphListView : public QListView {
    Q_OBJECT
public:
    explicit QUGlyphListView(QWidget * parent = nullptr);
};

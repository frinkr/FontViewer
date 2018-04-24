#pragma once
#include <QAbstractListModel>
#include <QImage>
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

enum class QUGlyphLabel {
    CharacterCode,
    GlyphID,
    GlyphName
};

class QUGlyphListModel : public QAbstractListModel {
    
    Q_OBJECT
    
public:
    explicit QUGlyphListModel(FXPtr<FXFace> face,
                              QObject * parent = nullptr);

public:
    const FXCMap &
    currentCMap() const;
    
    FXPtr<FXCharBlock>
    currentBlock() const;

    QSize
    iconSize() const;

    void
    selectBlock(size_t index);
    
    bool
    charMode() const;

    void
    setCharMode(bool state);
    
    QUGlyphLabel
    glyphLabel() const;

    void 
    setGlyphLabel(QUGlyphLabel label);

    FXChar
    charAt(const QModelIndex & index, bool & isGID) const;
public:
    int
    rowCount(const QModelIndex &) const;
    
    QVariant
    data(const QModelIndex &, int) const;

protected:
    FXPtr<FXFace>    face_;
    FXPtr<FXCharBlock> fullGlyphsBlock_;
    size_t           blockIndex_;

    QImage           dummyImage_;

    bool             charMode_;
    QUGlyphLabel     glyphLabel_;
};


class QUGlyphListView : public QListView {
    Q_OBJECT
public:
    explicit QUGlyphListView(QWidget * parent = nullptr);
};

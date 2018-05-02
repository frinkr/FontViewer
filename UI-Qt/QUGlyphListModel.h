#pragma once
#include <QAbstractListModel>
#include <QImage>
#include "FontX/FXFace.h"

constexpr int QUGlyphRole= Qt::UserRole + 1;

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
    
    FXPtr<FXGCharBlock>
    currentBlock() const;

    size_t
    currentBlockIndex() const;
    
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

    FXGChar
    charAt(const QModelIndex & index) const;
public:
    int
    rowCount(const QModelIndex &) const;
    
    QVariant
    data(const QModelIndex &, int) const;

protected:
    FXPtr<FXFace>    face_;
    FXPtr<FXGCharBlock> fullGlyphsBlock_;
    size_t           blockIndex_;

    QImage           dummyImage_;

    bool             charMode_;
    QUGlyphLabel     glyphLabel_;
};

#pragma once

#include <QPair>
#include <QWidget>

#include "FontX/FXTag.h"

namespace Ui {
    class QXShapingWidget;
}

class FXInspector;
class FXShaper;
class QPaintEvent;
class QXDocument;

class QXShapingGlyphView : public QWidget {
    Q_OBJECT

public:
    explicit QXShapingGlyphView(QWidget * parent = nullptr);

    void
    setShaper(FXShaper * shaper);

    void
    setDocument(QXDocument * document);
    
    QSize
    minimumSizeHint() const;
    
    QSize
    sizeHint() const;
    
    void
    paintEvent(QPaintEvent * event);

    void
    mousePressEvent(QMouseEvent *event);

    void
    mouseDoubleClickEvent(QMouseEvent *event);

    void
    setFontSize(double fontSize);
    
signals:
    void
    glyphDoubleClicked(FXGlyphID gid) const;
    
protected:
    
    QPoint
    baseLinePosition() const ;

    int
    gridCellBottom(int row /*start from bottom*/, int col = 0) const;

    int
    gridCellLeft(int row, int col /*start from first glyph*/) const;

    QRect
    gridCellRect(int row, int col) const;

    QRect
    glyphInteractionRect(int index) const;

    int
    glyphAtPoint(const QPoint & point) const;
    
    double
    fu2px(fu f) const;

protected:
    int selectedIndex_;
    double fontSize_;    
    FXShaper * shaper_;
    QXDocument * document_;
};

class QUShapingWidget : public QWidget {
    Q_OBJECT

public:
    explicit QUShapingWidget(QWidget * parent = nullptr);
    ~QUShapingWidget();

    void
    setDocument(QXDocument * document);

public slots:
    void
    reloadScriptList();

    void
    reloadFeatureList();

    void
    doShape();

    void
    gotoGlyph(FXGlyphID gid);
    
private:
    FXPtr<FXInspector>
    inspector();

    FXVector<FXTag>
    onFeatures() const;

    FXVector<FXTag>
    offFeatures() const;
    
private:
    Ui::QXShapingWidget * ui_;
    QXDocument   * document_;
    FXShaper     * shaper_;    
};



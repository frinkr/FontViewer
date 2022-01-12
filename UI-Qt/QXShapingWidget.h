#pragma once

#include <QPair>
#include <QWidget>

#include "FontX/FXTag.h"
#include "FontX/FXShaper.h"

#include "QXShapingOptionsWidget.h"

namespace Ui {
    class QXShapingWidget;
}

class FXInspector;
class FXShaper;
class QPaintEvent;
class QXDocument;
class QXShapingFeaturesWidget;
class QXShapingOptionsWidget;
class QXPopoverWindow;

class QXShapingGlyphView : public QWidget {
    Q_OBJECT
    
public:
    explicit QXShapingGlyphView(QWidget * parent = nullptr);

    void
    setShaper(const FXShaper * shaper);

    void
    setOptions(const QXShapingOptions & options);
    
    void
    setDocument(QXDocument * document);
    
    QSize
    minimumSizeHint() const override;
    
    QSize
    sizeHint() const override;
    
    void
    paintEvent(QPaintEvent * event) override;

    void
    mousePressEvent(QMouseEvent *event) override;
    
    void
    mouseMoveEvent(QMouseEvent *event) override;
    
    void
    mouseDoubleClickEvent(QMouseEvent *event) override;
    
    void
    wheelEvent(QWheelEvent *event) override;
    
signals:
    void
    glyphDoubleClicked(FXGlyphID gid) const;

    void
    fontSizeChanged(double fontSize) const;
    
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

private slots:
    void
    onOptionsChanged();
    
protected:
    int selectedColIndex_;
    int selectedRowIndex_;
    double fontSize_;    
    const FXShaper * shaper_;
    QXShapingOptions options_{};
    QXDocument * document_;
    FXVector<int> cellLeftCache_;
};

class QXShapingWidget : public QWidget {
    Q_OBJECT

public:
    explicit QXShapingWidget(QWidget * parent = nullptr);
    ~QXShapingWidget();

    void
    setDocument(QXDocument * document);

public slots:
    void
    doShape();

    void
    gotoGlyph(FXGlyphID gid);
    
    void
    doCopyAction();

    void
    doCopyHexAction();

    void
    doCopyHexCStyleAction();
    
    void
    focusLineEdit(bool selectAll = true);

    void
    showOptionsPopover();

    void
    showFeaturesPopover();
    
private:

    void
    copyTextToClipboard(const QString & text);
    
    FXPtr<FXInspector>
    inspector();
    
private:
    Ui::QXShapingWidget * ui_;
    QXDocument   * document_;
    FXShaper     * shaper_;
    QAction      * warningAction_;

    QXShapingOptionsWidget * optionsWidget_{};
    QXShapingFeaturesWidget * featuresWidget_{};
    QXPopoverWindow * optionsPopover_{};
    QXPopoverWindow * featuresPopover_{};
};



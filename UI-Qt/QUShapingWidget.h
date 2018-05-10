#pragma once

#include <QPair>
#include <QWidget>

#include "FontX/FXTag.h"

namespace Ui {
    class QUShapingWidget;
}

class QPaintEvent;
class FXInspector;
class FXShaper;
class QUDocument;


class QUShapingGlyphView : public QWidget {
    Q_OBJECT

public:
    explicit QUShapingGlyphView(QWidget * parent = nullptr);

    void
    setShaper(FXShaper * shaper);

    void
    paintEvent(QPaintEvent * event);
    
protected:
    FXShaper * shaper_;
};

class QUShapingWidget : public QWidget {
    Q_OBJECT

public:
    explicit QUShapingWidget(QWidget * parent = nullptr);
    ~QUShapingWidget();

    void
    setDocument(QUDocument * document);

public slots:
    void
    reloadScriptList();

    void
    reloadFeatureList();

    void
    doShape();

private:
    FXPtr<FXInspector>
    inspector();

    FXVector<FXTag>
    onFeatures() const;

    FXVector<FXTag>
    offFeatures() const;
    
private:
    Ui::QUShapingWidget * ui_;
    QUDocument   * document_;
    FXShaper     * shaper_;    
};



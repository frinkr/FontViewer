#pragma once

#include <QWidget>
    
#include "FontX/FXShaper.h"

namespace Ui {
    class QXShapingOptionsWidget;
}

struct QXShapingOptions {
    double fontSize {100};
    bool showGlyphsBoundary {true};
    bool alternativeRowColor {true};
    FXShapingGenralOptions general {};
    FXShapingBidiOptions bidi {};
};
    
class QXShapingOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QXShapingOptionsWidget(QWidget *parent = nullptr);
    ~QXShapingOptionsWidget();

    QXShapingOptions
    options() const;
    
signals:
    void 
    optionsChanged();

    void
    togglePanelButtonClicked();

    void
    copyTextButtonClicked();

    void
    copyHexButtonClicked();

    void
    copyHexCStyleButtonClicked();
    
private:
    Ui::QXShapingOptionsWidget *ui;
};


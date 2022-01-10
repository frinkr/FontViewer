#pragma once

#include <QWidget>
    
#include "FontX/FXShaper.h"

namespace Ui {
    class QXShapingOptionsWidget;
}

struct QXShapingOptions {
    struct Ui {
        double fontSize {100};
        bool   showGlyphsBoundary {true};
        bool   alternativeRowColor {true};
        bool   highlightCombiningMarks {true};
    } ui {};
    
    FXShapingOptions shapingOpts {};
};
    
class QXShapingOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QXShapingOptionsWidget(QWidget *parent = nullptr);
    ~QXShapingOptionsWidget();

    QXShapingOptions
    options() const;

public slots:
    void
    setFontSize(double fontSize);
    
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


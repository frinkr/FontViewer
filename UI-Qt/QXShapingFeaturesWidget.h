#ifndef QXSHAPINGFEATURESWIDGET_H
#define QXSHAPINGFEATURESWIDGET_H

#include <QWidget>
#include "FontX/FXTag.h"
#include "FontX/FXShaper.h"

namespace Ui {
    class QXShapingFeaturesWidget;
}


class FXInspector;
class QXDocument;

class QXShapingFeaturesWidget : public QWidget {
    Q_OBJECT

public:
    explicit QXShapingFeaturesWidget(QWidget *parent = nullptr);
    ~QXShapingFeaturesWidget();

    void
    setDocument(QXDocument * document);

    FXVector<FXTag>
    onFeatures() const;

    FXVector<FXTag>
    offFeatures() const;

    std::tuple<FXTag, FXTag>
    scriptAndLanguage() const;
    
signals:
    void
    featuresChanged();
    
private:
    void
    reloadScriptList();
    
    void
    reloadFeatureList();
    
    FXPtr<FXInspector>
    inspector();

private:
    Ui::QXShapingFeaturesWidget * ui_;
    QXDocument   * document_ {};
};

#endif // QXSHAPINGFEATURESWIDGET_H

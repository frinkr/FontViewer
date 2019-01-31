#pragma once
#include <QWidget>
#include "FontX/FXFace.h"

namespace Ui {
    class QXFontInfoWidget;
}

class QXFontInfoPage : public QObject {
    Q_OBJECT
    
public:
    QXFontInfoPage(const QString & title, FXPtr<FXFace> face, QObject * parent = nullptr);
    virtual ~QXFontInfoPage() {}
    
    const QString &
    title() const;
    
    virtual QString
    html() = 0;

protected:
    const FXFaceAttributes &
    faceAtts() const;
protected:
    QString       title_;
    FXPtr<FXFace> face_;
};

class QXFontInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QXFontInfoWidget(FXPtr<FXFace> face, QWidget *parent = 0);
    ~QXFontInfoWidget();

private slots:
    void
    onCombobox(int index);
private:
    Ui::QXFontInfoWidget * ui;
    QList<QXFontInfoPage*>  pages_;
};


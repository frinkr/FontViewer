#pragma once
#include <QWidget>
#include "FontX/FXFace.h"

namespace Ui {
    class QUFontInfoWidget;
}

class QUFontInfoPage;


class QUFontInfoPage : public QObject {
    Q_OBJECT
    
public:
    QUFontInfoPage(const QString & title, FXPtr<FXFace> face, QObject * parent = nullptr);
    virtual ~QUFontInfoPage() {}
    
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

class QUFontInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QUFontInfoWidget(FXPtr<FXFace> face, QWidget *parent = 0);
    ~QUFontInfoWidget();

private slots:
    void
    onCombobox(int index);
private:
    Ui::QUFontInfoWidget * ui;
    QList<QUFontInfoPage*>  pages_;
};


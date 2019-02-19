#pragma once

#include <QDialog>
#include "QXDocument.h"
#include "QXThemedWindow.h"
namespace Ui {
    class QXOpenFontDialog;
}

class QXOpenFontDialog : public QXThemedWindow<QDialog> {
    Q_OBJECT

public:
    explicit QXOpenFontDialog(QWidget *parent = 0);
    ~QXOpenFontDialog();

    QXFontURI
    selectedFont();

    virtual void
    accept();
            
private slots:
    void
    onOpenFileButtonClicked();

    void
    onFontSelected(const QXFontURI & uri, size_t index);

private:
    void
    showEvent(QShowEvent * event);
    
private:
    Ui::QXOpenFontDialog * ui_;
    QMenu                * recentMenu_; 
};



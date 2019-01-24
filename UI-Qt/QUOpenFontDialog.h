#ifndef QUOPENFONTDIALOG_H
#define QUOPENFONTDIALOG_H

#include <QDialog>
#include "QUDocument.h"

namespace Ui {
    class QUOpenFontDialog;
}

class QUOpenFontDialog : public QDialog {
    Q_OBJECT

public:
    explicit QUOpenFontDialog(QWidget *parent = 0);
    ~QUOpenFontDialog();

    QUFontURI
    selectedFont();

    virtual void
    accept();
            
private slots:
    void
    slotBrowseFile();

    void
    slotFontSelected(const QUFontURI & uri, size_t index);
        
private:
    Ui::QUOpenFontDialog * ui_;
};

#endif

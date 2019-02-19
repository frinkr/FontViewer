#pragma once

#include <QDialog>
#include "QXThemedWindow.h"

namespace Ui {
    class QXAboutFontsDialog;
}

class QXAboutFontsDialog : public QXThemedWindow<QDialog> {
    Q_OBJECT

public:
    explicit QXAboutFontsDialog(QWidget * parent = 0);
    ~QXAboutFontsDialog();

    static void
    showAbout(bool model = false);

private:
    Ui::QXAboutFontsDialog *ui;
};



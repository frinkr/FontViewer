#pragma once

#include <QDialog>

namespace Ui {
    class QXAboutFontsDialog;
}

class QXAboutFontsDialog : public QDialog {
    Q_OBJECT

public:
    explicit QXAboutFontsDialog(QWidget * parent = 0);
    ~QXAboutFontsDialog();

    static void
    showAbout();

private:
    Ui::QXAboutFontsDialog *ui;
};



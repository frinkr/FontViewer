#pragma once

#include <QDialog>
#include "QXThemedWindow.h"

namespace Ui {
    class QXAboutDialog;
}

class QXAboutDialog : public QXThemedWindow<QDialog, false> {
    Q_OBJECT

public:
    explicit QXAboutDialog(QWidget *parent = nullptr);
    ~QXAboutDialog();

    static void
    showAbout();

private:
    Ui::QXAboutDialog *ui;
};

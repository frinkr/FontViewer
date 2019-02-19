#ifndef QUABOUTDIALOG_H
#define QUABOUTDIALOG_H

#include <QDialog>
#include "QXThemedWindow.h"

namespace Ui {
    class QXAboutDialog;
}

class QXAboutDialog : public QXThemedWindow<QDialog> {
    Q_OBJECT

public:
    explicit QXAboutDialog(QWidget *parent = nullptr);
    ~QXAboutDialog();

    static void
    showAbout();

private:
    Ui::QXAboutDialog *ui;
};

#endif // QUABOUTDIALOG_H

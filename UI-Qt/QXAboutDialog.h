#ifndef QUABOUTDIALOG_H
#define QUABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class QXAboutDialog;
}

class QXAboutDialog : public QDialog {
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

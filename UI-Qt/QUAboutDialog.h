#ifndef QUABOUTDIALOG_H
#define QUABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class QUAboutDialog;
}

class QUAboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit QUAboutDialog(QWidget *parent = nullptr);
    ~QUAboutDialog();

    static void
    showAbout();

private:
    Ui::QUAboutDialog *ui;
};

#endif // QUABOUTDIALOG_H

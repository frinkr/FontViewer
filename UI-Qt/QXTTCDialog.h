#ifndef QXTTCDIALOG_H
#define QXTTCDIALOG_H

#include <QDialog>

namespace Ui {
class QXTTCDialog;
}

class QXTTCDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QXTTCDialog(QWidget *parent = nullptr);
    ~QXTTCDialog();

private:
    Ui::QXTTCDialog *ui;
};

#endif // QXTTCDIALOG_H

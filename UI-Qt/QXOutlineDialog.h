#ifndef QXOUTLINEDIALOG_H
#define QXOUTLINEDIALOG_H

#include <QDialog>

namespace Ui {
class QXOutlineDialog;
}

class QXOutlineWidget;

class QXOutlineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QXOutlineDialog(QWidget *parent = 0);
    ~QXOutlineDialog();

    QXOutlineWidget *
    outlineWidget() const;

private slots:
    void
        updateComponents();

private:
    Ui::QXOutlineDialog *ui;
};

#endif // QXOUTLINEDIALOG_H

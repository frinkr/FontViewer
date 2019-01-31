#pragma once

#include <QDialog>

namespace Ui {
    class QXProgressDialog;
}

class QXProgressDialog : public QDialog {
    Q_OBJECT

public:
    explicit QXProgressDialog(QWidget * parent = nullptr);
    ~QXProgressDialog();

    void
    setProgress(int value, int maximum, const QString & message);

private:
    Ui::QXProgressDialog * ui_;
};



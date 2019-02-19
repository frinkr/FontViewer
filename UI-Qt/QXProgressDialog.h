#pragma once

#include <QDialog>
#include "QXThemedWindow.h"

namespace Ui {
    class QXProgressDialog;
}

class QXProgressDialog : public QXThemedWindow<QDialog> {
    Q_OBJECT

public:
    explicit QXProgressDialog(QWidget * parent = nullptr);
    ~QXProgressDialog();

    void
    setProgress(int value, int maximum, const QString & message);

private:
    Ui::QXProgressDialog * ui_;
};



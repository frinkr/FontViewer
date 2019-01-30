#pragma once

#include <QDialog>

namespace Ui {
    class QUProgressDialog;
}

class QUProgressDialog : public QDialog {
    Q_OBJECT

public:
    explicit QUProgressDialog(QWidget * parent = nullptr);
    ~QUProgressDialog();

    void
    setProgress(int value, int maximum, const QString & message);

private:
    Ui::QUProgressDialog * ui_;
};



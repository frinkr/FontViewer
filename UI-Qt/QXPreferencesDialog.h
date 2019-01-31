#pragma once

#include <QDialog>

namespace Ui {
    class QXPreferencesDialog;
}

class QXPreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit QXPreferencesDialog(QWidget * parent = nullptr);
    ~QXPreferencesDialog();

    void
    accept();

    static void
    showPreferences();

private slots:
    void
    onAddButtonClicked();

    void
    onRemoveButtonClicked();
private:
    Ui::QXPreferencesDialog * ui_;
};


#pragma once

#include <QDialog>

namespace Ui {
    class QUPreferencesDialog;
}

class QUPreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit QUPreferencesDialog(QWidget * parent = nullptr);
    ~QUPreferencesDialog();

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
    Ui::QUPreferencesDialog * ui_;
};


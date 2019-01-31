#pragma once

#include <QDialog>

namespace Ui {
    class QXPreferencesDialog;
}

class QListWidgetItem;

class QXPreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit QXPreferencesDialog(QWidget * parent = nullptr);
    ~QXPreferencesDialog();

    void
    accept();

    QStringList
    userFontFolders() const;

    static void
    showPreferences();

private slots:
    void
    onCurrentDirItemChanged(QListWidgetItem * current, QListWidgetItem * previous);

    void
    onAddButtonClicked();

    void
    onRemoveButtonClicked();
private:
    Ui::QXPreferencesDialog * ui_;
};


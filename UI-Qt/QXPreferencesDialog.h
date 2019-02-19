#pragma once

#include <QDialog>
#include "QXThemedWindow.h"

namespace Ui {
    class QXPreferencesDialog;
}

class QListWidgetItem;

class QXPreferencesDialog : public QXThemedWindow<QDialog> {
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
    onCurrentFontFoldertemChanged(QListWidgetItem * current, QListWidgetItem * previous);

    void
    onAddFontFolderButtonClicked();

    void
    onRemoveFontFolderButtonClicked();
private:
    Ui::QXPreferencesDialog * ui_;
};


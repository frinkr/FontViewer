#pragma once

#include <QDialog>

namespace Ui {
    class QXFontCollectionDialog;
}

class QXFontCollectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit QXFontCollectionDialog(const QString & filePath, QWidget * parent = nullptr);
    ~QXFontCollectionDialog();

    int
    selectedIndex() const;

    static int
    selectFontIndex(const QString & filePath);

private:
    Ui::QXFontCollectionDialog * ui_;
    QString filePath_;
};



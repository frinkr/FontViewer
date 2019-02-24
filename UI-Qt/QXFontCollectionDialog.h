#pragma once

#include <QDialog>
#include "FontX/FXFace.h"

namespace Ui {
    class QXFontCollectionDialog;
}

class QXFontCollectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit QXFontCollectionDialog(const QString & filePath, FXPtr<FXFace> initFace = nullptr, QWidget * parent = nullptr);
    ~QXFontCollectionDialog();

    int
    selectedIndex() const;

    static int
    selectFontIndex(const QString & filePath, FXPtr<FXFace> initFace = nullptr);

private:
    Ui::QXFontCollectionDialog * ui_;
    QString filePath_;
};



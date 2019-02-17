#pragma once

#include "QXPopoverWindow.h"

namespace Ui {
    class QXCMapBlockWidget;
}

class QXDocument;

class QXCMapBlockWidget : public QWidget {
    Q_OBJECT

public:
    explicit QXCMapBlockWidget(QWidget *parent = 0);
    ~QXCMapBlockWidget();

    QXDocument *
    document() const;

    void
    setDocument(QXDocument * document);

private slots:
    void
    reloadCMapsCombobox();

    void
    reloadBlocksCombobox();

    void
    onBlockComboBoxChanged(int index);

    void
    onDocumentBookSelected(int book);

    void
    onGlyphCheckBox(bool state);

    void
    onDocumentCharModeChanged(bool state);
    
private:
    Ui::QXCMapBlockWidget * ui_;
    QXDocument * document_;
};



#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QAbstractListModel>
#include <QItemDelegate>
#include <QItemSelection>

class QMenu;
class QAction;
class QFileDialog;
class QMessageBox;
class QStringListModel;

class QUDocumentWindowManager;
class QUDocument;
class QUFontInfoWindow;
class QUCMapBlockWindow;

namespace Ui {
    class QUDocumentWindow;
}

class QUDocumentWindow : public QMainWindow
{
    friend class QUDocumentWindowManager;

    Q_OBJECT

public:
    QUDocumentWindow(QUDocument * document, QWidget *parent = 0);
    ~QUDocumentWindow();

    QUDocument * document() const {
        return document_;
    }

private:
    void
    initUI();

    void
    initWindowTitle();

    void
    initMenu();
    
    void
    initToolBar();

    void
    initListView();

    void
    initGlyphInfoView();
    
    void
    connectSingals();

private slots:
    void
    reloadBlocks();

    void
    showFullGlyphList(bool state);

    void
    onToggleFullScreen(bool state);
    
    void
    onSwitchGlyphLabel();

    void
    onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void
    onCharLinkClicked(FXGChar c);

    void
    onCMapBlockAction();
    
    void
    onFontInfoAction();
    
private:
    Ui::QUDocumentWindow * ui_;
    QDockWidget          * infoDockWidget_;

    QUCMapBlockWindow * cmapBlockWindow_;
    QAction    * cmapBlockAction_;

    
    QComboBox  * cmapCombobox_;
    QAction    * cmapAction_;
    QComboBox  * blockCombobox_;
    QAction    * blockAction_;
    QLineEdit  * searchEdit_;
    QAction    * searchAction_;

    QUDocument * document_;
};

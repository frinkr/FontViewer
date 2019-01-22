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
class QToolButton;

class QUDocumentWindowManager;
class QUDocument;
class QUFontInfoWindow;
class QUPopoverWindow;

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

    QToolButton *
    senderToolButton();
                      
private slots:
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
    onShapingAction();

    void
    onTableAction();
    
    void
    onFontInfoAction();

    void
    onGlyphAction();
    
    void
    onSearchAction();

    void
    onSearchResult(const QUSearchResult & result, const QString & text);

private:
    void
    toggleDockWidget(QDockWidget * dockWidget);
    
private:
    Ui::QUDocumentWindow * ui_;
    QAction         * cmapBlockAction_;
    QUPopoverWindow * cmapBlockWindow_;
    QAction         * shapingAction_;
    QDockWidget     * shapingDockWidget_;
    QAction         * tableAction_;
    QDockWidget     * tableDockWidget_;
    QAction         * infoAction_;
    QDockWidget     * infoDockWidget_;
    QAction         * glyphAction_;
    QDockWidget     * glyphDockWidget_;
    QAction         * searchAction_;
    QUPopoverWindow * searchWindow_;
    
    QUDocument      * document_;
};

#pragma once

#include <QAbstractListModel>
#include <QCheckBox>
#include <QComboBox>
#include <QItemDelegate>
#include <QItemSelection>
#include <QMainWindow>
#include <QPushButton>

#include "QXPopoverWindow.h"
#include "QXThemedWindow.h"

struct FXGChar;

class QAction;
class QFileDialog;
class QLineEdit;
class QMenu;
class QMessageBox;
class QStringListModel;
class QToolButton;

class QXDocument;
class QXDocumentWindowManager;
class QXFontInfoWindow;
class QXGlyphInfoWidget;
class QXMenuBar;
class QXPopoverWindow;

struct QXCollectionModelIndex;
struct QXSearchResult;

namespace Ui {
    class QXDocumentWindow;
}

class QXDocumentWindow : public QXThemedWindow<QMainWindow>
{
    friend class QXDocumentWindowManager;

    Q_OBJECT

public:
    QXDocumentWindow(QXDocument * document, QWidget *parent = 0);
    ~QXDocumentWindow();

    QXDocument * document() const {
        return document_;
    }

    void
    showGlyphPopover(const FXGChar & c,
                     const QRect & rect,
                     QXPopoverEdges preferedEgdes = QXPopoverAnyEdge);

signals:
    void
    aboutToClose(QXDocumentWindow * window);

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
    initSearchField();

    void
    initCollectionView();

    void
    initGlyphInfoView();
    
    void
    connectSingals();

    QToolButton *
    senderToolButton();

    void
    closeEvent(QCloseEvent * event);

    void
    dropEvent(QDropEvent * event);

    bool
    eventFilter(QObject * watched, QEvent * event);

private slots:
    void
    onGlyphClicked(const QXCollectionModelIndex & index);

    void
    onGlyphRightClicked(const QXCollectionModelIndex & index);
    
    void
    onGlyphDoubleClicked(const QXCollectionModelIndex & index);

    void
    onCharLinkClicked(FXGChar c);

    void
    onCopyAction();

    void
    onOpenFontAction();

    void
    onCMapBlockAction();

    void
    onVariableAction();

    void
    onShapingAction();

    void
    onTableAction();
    
    void
    onFontInfoAction();

    void
    onOpenFontInSameFileAction();

    void
    onFontListItemDoubleClicked(int index);
    
    void
    onSearchAction();

    void
    onSearchLineEditReturnPressed();

    void
    onSearchResult(const QXSearchResult & result, const QString & text);

private:
    void
    toggleDockWidget(QDockWidget * dockWidget);

    void
    copyUnicodeAtIndex(const QXCollectionModelIndex & index);
private:
    Ui::QXDocumentWindow * ui_;
    QXMenuBar       * menuBar_;

    QAction         * openFontAction_;
    QAction         * cmapBlockAction_;
    QXPopoverWindow * cmapBlockPopover_;
    QAction         * variableAction_;
    QXPopoverWindow * variablePopover_;
    QAction         * shapingAction_;
    QDockWidget     * shapingDockWidget_;
    QAction         * tableAction_;
    QDockWidget     * tableDockWidget_;
    QAction         * infoAction_;
    QDockWidget     * infoDockWidget_;
    QAction         * openFontInSameFileAction_;
    QXPopoverWindow * openFontInSameFilePopover_;
    QLineEdit       * searchLineEdit_;

    QXPopoverWindow   * glyphPopover_;
    QXGlyphInfoWidget * glyphWidget_;

    QXDocument        * document_;
};

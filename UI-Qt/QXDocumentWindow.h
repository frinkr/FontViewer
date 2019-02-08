#pragma once

#include <QAbstractListModel>
#include <QCheckBox>
#include <QComboBox>
#include <QItemDelegate>
#include <QItemSelection>
#include <QMainWindow>
#include <QPushButton>

class QAction;
class QFileDialog;
class QMenu;
class QMessageBox;
class QStringListModel;
class QToolButton;

class QXDocument;
class QXDocumentWindowManager;
class QUFontInfoWindow;
class QXGlyphInfoWidget;
class QXMenuBar;
class QXPopoverWindow;
struct QXSearchResult;

namespace Ui {
    class QXDocumentWindow;
}

class QXDocumentWindow : public QMainWindow
{
    friend class QXDocumentWindowManager;

    Q_OBJECT

public:
    QXDocumentWindow(QXDocument * document, QWidget *parent = 0);
    ~QXDocumentWindow();

    QXDocument * document() const {
        return document_;
    }

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
    initListView();

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
    onGlyphDoubleClicked(const QModelIndex &index);

    void
    onCharLinkClicked(FXGChar c);

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
    onSearchAction();

    void
    onSearchResult(const QXSearchResult & result, const QString & text);

private:
    void
    toggleDockWidget(QDockWidget * dockWidget);
    
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
    QAction         * searchAction_;
    QXPopoverWindow * searchPopover_;

    QXPopoverWindow   * glyphPopover_;
    QXGlyphInfoWidget * glyphWidget_;

    QXDocument      * document_;
};

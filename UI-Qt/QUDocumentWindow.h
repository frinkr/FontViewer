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

class QUDocument;
class QUDocumentWindowManager;
class QUFontInfoWindow;
class QUGlyphInfoWidget;
class QUMenuBar;
class QUPopoverWindow;
struct QUSearchResult;

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

signals:
    void
    aboutToClose(QUDocumentWindow * window);

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

private slots:

    void
    onGlyphDoubleClicked(const QModelIndex &index);

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
    onSearchAction();

    void
    onSearchResult(const QUSearchResult & result, const QString & text);

private:
    void
    toggleDockWidget(QDockWidget * dockWidget);
    
private:
    Ui::QUDocumentWindow * ui_;
    QUMenuBar       * menuBar_;
    
    QAction         * cmapBlockAction_;
    QUPopoverWindow * cmapBlockWindow_;
    QAction         * shapingAction_;
    QDockWidget     * shapingDockWidget_;
    QAction         * tableAction_;
    QDockWidget     * tableDockWidget_;
    QAction         * infoAction_;
    QDockWidget     * infoDockWidget_;
    QAction         * searchAction_;
    QUPopoverWindow * searchWindow_;

    QUPopoverWindow   * glyphPopover_;
    QUGlyphInfoWidget * glyphWidget_;

    QUDocument      * document_;
};

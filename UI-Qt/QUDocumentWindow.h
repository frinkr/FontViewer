#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QAbstractListModel>
#include <QItemDelegate>


class QMenu;
class QAction;
class QFileDialog;
class QMessageBox;
class QStringListModel;

class QUDocumentWindowManager;
class QUDocument;

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
    initToolBar();

    void
    initListView();

    void
    connectSingals();

private slots:
    void
    reloadBlocks();

    void
    slotSetGlyphMode(bool state);
private:
    Ui::QUDocumentWindow * ui_;

    QComboBox  * cmapCombobox_;
    QComboBox  * blockCombobox_;
    QPushButton * glyphModelToggle_;
    QUDocument * document_;
};

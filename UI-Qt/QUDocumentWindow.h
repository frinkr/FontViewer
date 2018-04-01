#ifndef QUDOCUMENTWINDOW_H
#define QUDOCUMENTWINDOW_H

#include <QMainWindow>
#include <QPainter>
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
    
private:
    Ui::QUDocumentWindow * ui_;

    QUDocument * document_;
    QAbstractItemModel * model_;

};

#endif // QUDOCUMENTWINDOW_H

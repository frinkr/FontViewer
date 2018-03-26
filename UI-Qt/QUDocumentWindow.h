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

class QUGlyphItemDelegate : public QItemDelegate {
    void
    paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
        painter->drawText(option.rect, Qt::AlignLeft, "hell");
    }
};

class QUGlyphListModel : public QAbstractListModel {
    int columnCount(const QModelIndex &parent) const {
        return 5;
    }
    int rowCount(const QModelIndex &parent) const {
        return 1000;
    }

    QVariant data(const QModelIndex &index, int role) const {
           if ( role == Qt::DisplayRole ) 
               return "hello";
           return QVariant();
    }

    QModelIndex index(int row, int column, const QModelIndex &parent) const
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();
        return createIndex(row, column, nullptr);
    }
};

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
    QStringListModel * model_;

};

#endif // QUDOCUMENTWINDOW_H

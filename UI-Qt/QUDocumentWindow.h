#ifndef QUDOCUMENTWINDOW_H
#define QUDOCUMENTWINDOW_H

#include <QMainWindow>

class QMenu;
class QAction;
class QFileDialog;
class QMessageBox;

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
    Ui::QUDocumentWindow * ui_;

    QUDocument * document_;
    QString currFile;
};

#endif // QUDOCUMENTWINDOW_H

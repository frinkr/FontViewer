#pragma once

#include <QWidget>
#include <QAbstractTableModel>

class QXDocument;

namespace Ui {
    class QXGlyphTableWidget;
}

class QXGlyphTableModelColumn;

class QXGlyphTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit QXGlyphTableModel(QXDocument * document, QObject * parent = nullptr);
    int
    rowCount(const QModelIndex & parent) const;

    int
    columnCount(const QModelIndex & parent) const;
    
    QVariant
    data(const QModelIndex & index, int role) const;
    
    QVariant
    headerData(int section, Qt::Orientation orientation, int role) const;

    const QVector<QXGlyphTableModelColumn *> &
    columns() const;

    FXGlyph
    glyphAt(int row) const;
public slots:
    void
    reset();

    bool
    exportToFile(const QString & filePath) const;
    
protected:
    QXDocument     * document_;
    QVector<QXGlyphTableModelColumn *> columns_;
};

class QUGlyphTableWidget : public QWidget {
    Q_OBJECT

public:
    explicit QUGlyphTableWidget(QXDocument * document, QWidget *parent = nullptr);
    ~QUGlyphTableWidget();
                         
public slots:
    void
    exportToFile();

private slots:
    void
    gotoGlyphAtIndex(const QModelIndex & index);
private:
    Ui::QXGlyphTableWidget * ui_;
    QXDocument             * document_;
    QXGlyphTableModel      * model_;
};

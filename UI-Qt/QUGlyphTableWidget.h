#pragma once

#include <QWidget>
#include <QAbstractTableModel>

class QUDocument;

namespace Ui {
    class QUGlyphTableWidget;
}

class QUGlyphTableModelColumn : public QObject {
    Q_OBJECT
public:
    explicit QUGlyphTableModelColumn(const QString & name, QObject * parent = nullptr)
        : QObject(parent)
        , name_(name) {}
        
    virtual QVariant
    header(int role) {
        if (role == Qt::DisplayRole) 
            return name();
        return QVariant();;
    }
        
    virtual QVariant
    data(const FXGlyph & g, int role) {
        if (role == Qt::DisplayRole) 
            return value(g);
        return QVariant();
    }

    virtual QString
    name() const {
        return name_;
    };

    virtual QVariant
    value(const FXGlyph & g) = 0;
            
protected:
    QString      name_;
};

class QUGlyphTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit QUGlyphTableModel(QUDocument * document, QObject * parent = nullptr);
    int
    rowCount(const QModelIndex & parent) const;

    int
    columnCount(const QModelIndex & parent) const;
    
    QVariant
    data(const QModelIndex & index, int role) const;
    
    QVariant
    headerData(int section, Qt::Orientation orientation, int role) const;

    const QVector<QUGlyphTableModelColumn *> &
    columns() const;
    
protected:
    QUDocument     * document_;
    QVector<QUGlyphTableModelColumn *> columns_;
};

class QUGlyphTableWidget : public QWidget {
    Q_OBJECT

public:
    explicit QUGlyphTableWidget(QUDocument * document, QWidget *parent = nullptr);
    ~QUGlyphTableWidget();

private:
    Ui::QUGlyphTableWidget * ui_;
    QUDocument             * document_;
};



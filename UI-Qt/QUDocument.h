#ifndef QUDOCUMENT_H
#define QUDOCUMENT_H
#include <QObject>
#include <QSharedPointer>
#include <QAbstractListModel>
#include <QStyledItemDelegate>

#include "FontX/FXFace.h"

struct QUFontURI
{
    QString filePath;
    size_t  faceIndex;

    bool
    operator==(const QUFontURI & other) const {
        return filePath == other.filePath &&
            faceIndex == other.faceIndex;
    }
    
    bool
    operator!=(const QUFontURI & other) const {
        return !(*this == other);
    }

};

class QUGlyphItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    QUGlyphItemDelegate(QWidget *parent = 0)
        : QStyledItemDelegate(parent) {}

    void
    paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class QUDocument : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * file loading
     */
    static QUDocument *
    openFromURI(const QUFontURI & uri, QObject * parent = nullptr);
    
    static QUDocument *
    openFromFile(const QString & filePath, size_t faceIndex, QObject * parent = nullptr);

public:
    const QUFontURI &
    uri() const { return uri_; }

    FXPtr<FXFace>
    face() const;
    
    QSize
    iconSize() const;
public:
    int
    rowCount(const QModelIndex &) const;
    
    QVariant
    data(const QModelIndex &, int) const;
    
signals:
    void
    cmapActivated(int index);
        
public slots:
    void
    selectCMap(int index);

    void
    selectBlock(int index);
    
private:
    QUDocument(const QUFontURI & uri, QObject * parent);

    bool
    load();

    FXPtr<FXCharBlock>
    currentBlock() const;
    
private:
    QUFontURI        uri_;
    FXPtr<FXFace>    face_;

    size_t           blockIndex_;
};
#endif

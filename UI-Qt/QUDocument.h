#ifndef QUDOCUMENT_H
#define QUDOCUMENT_H
#include <QObject>
#include <QSharedPointer>
#include <QAbstractListModel>

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

public:
    int
    rowCount(const QModelIndex &) const;
    
    QVariant
    data(const QModelIndex &, int) const;

private:
    QUDocument(const QUFontURI & uri, QObject * parent);

    bool
    load();

    
private:
    QUFontURI        uri_;
    FXPtr<FXFace>    face_;
};
#endif

#pragma once
#include <QObject>
#include <QSharedPointer>
#include "FontX/FXFace.h"

class QUGlyphItemDelegate;
class QUGlyphListModel;

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


class QUDocument : public QObject
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

    QUGlyphListModel *
    model() const;

    QUGlyphItemDelegate *
    delegate() const;
signals:
    void
    cmapActivated(int index);
        
public slots:
    void
    selectCMap(size_t index);

    void
    selectBlock(size_t index);
private:
    QUDocument(const QUFontURI & uri, QObject * parent);

    bool
    load();

private:
    QUFontURI        uri_;
    FXPtr<FXFace>    face_;

    QSharedPointer<QUGlyphListModel>    model_;
    QSharedPointer<QUGlyphItemDelegate> delegate_;
};

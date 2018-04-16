#pragma once
#include <QComboBox>
#include <QAbstractListModel>
#include "FontX/FXFaceDatabase.h"
#include "QUDocument.h"

class QUFontListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit QUFontListModel(QObject * parent = nullptr);

    int
    rowCount(const QModelIndex & parent) const;
    
    QVariant
    data(const QModelIndex & index, int role) const;
protected:
    FXPtr<FXFaceDatabase>
    db() const;

    const FXFaceAttributes &
    attributes(size_t index) const;

    QString
    displayName(size_t index) const;
};

class QUFontComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit QUFontComboBox(QWidget * parent = nullptr);

    QUFontURI
    selectedFont() const;
};


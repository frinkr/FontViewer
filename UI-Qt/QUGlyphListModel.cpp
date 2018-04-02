#include "QUGlyphListModel.h"
#include <QIcon>


QUGlyphListModel::QUGlyphListModel(QObject * parent)
    : QAbstractListModel(parent)
{
}


QUGlyphListModel::~QUGlyphListModel()
{
}


int QUGlyphListModel::rowCount(const QModelIndex & index) const {
    return 100000;
}

QVariant QUGlyphListModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= 100000)
        return QVariant();

    if (role == Qt::DisplayRole)
        return QVariant(QString("hell0000000000000000000000000000000000000000000000o %1").arg(index.row()));
    else if (role == Qt::DecorationRole) {
        QStringList list;
        list << "copy" << "cut" << "open" << "paste";
        QString iconPath = ":/images/" + list.at(index.row() % list.size()) + ".png";

        QImage image(iconPath);
        QPixmap pix(iconPath);
        pix = pix.scaled(QSize(100, 100));
        QIcon icon;
        icon.addPixmap(pix);
        //icon.addFile(":/images/" + list.at(index.row() % list.size()) + ".png", QSize(200, 200));
        return icon;
    }
    else
        return QVariant();
}
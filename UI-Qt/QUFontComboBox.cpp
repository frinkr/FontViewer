#include "FontX/FXFaceDatabase.h"
#include "QUFontManager.h"
#include "QUFontComboBox.h"
#include "QUConv.h"

QUFontListModel::QUFontListModel(QObject * parent)
    : QAbstractListModel(parent) {
}

int
QUFontListModel::rowCount(const QModelIndex & parent) const {
    return (int)db()->faceCount();
}
    
QVariant
QUFontListModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();
    if (index.row() >= db()->faceCount())
        return QVariant();
    if (role == Qt::DisplayRole) 
        return displayName(index.row());
    else
        return QVariant();
    
}

const FXFaceAttributes &
QUFontListModel::attributes(size_t index) const {
    return db()->faceAttributes(index);
}

QString
QUFontListModel::displayName(size_t index) const {
    auto const & attrs = attributes(index);
    return QString("%1 - %2")
        .arg(toQString(attrs.names.familyName()),
             toQString(attrs.names.styleName()));
}

FXPtr<FXFaceDatabase>
QUFontListModel::db() const {
    return QUFontManager::get().db();
}

QUFontComboBox::QUFontComboBox(QWidget * parent)
    : QComboBox(parent) {
    setModel(new QUFontListModel(this));
    //setEditable(true);
    //setDuplicatesEnabled(true);
}

QUFontURI
QUFontComboBox::selectedFont() const {
    auto desc = QUFontManager::get().db()->faceDescriptor(currentIndex());
    auto atts = QUFontManager::get().db()->faceAttributes(currentIndex());
    atts.names.familyName();
    QUFontURI uri{toQString(desc.filePath), desc.index};
    return uri;
}

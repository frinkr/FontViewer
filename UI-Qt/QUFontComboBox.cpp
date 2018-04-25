#include <QFileInfo>
#include <QLineEdit>
#include <QSortFilterProxyModel>
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
    QString familyName = toQString(attrs.names.familyName());
    QString styleName = toQString(attrs.names.styleName());
    if (!familyName.isEmpty())
        return QString("%1 - %2").arg(familyName, styleName);
    else
        return QFileInfo(toQString(attrs.desc.filePath)).fileName();
    
}

FXPtr<FXFaceDatabase>
QUFontListModel::db() const {
    return QUFontManager::get().db();
}

bool
QUSortFilterFontListModel::lessThan(const QModelIndex & left, const QModelIndex & right) const {
    QVariant d0 = fontListModel()->data(left, Qt::DisplayRole);
    QVariant d1 = fontListModel()->data(right, Qt::DisplayRole);
    return d0 < d1;
    return true;
}

bool
QUSortFilterFontListModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
    return true;
}

QUFontListModel *
QUSortFilterFontListModel::fontListModel() const {
    return qobject_cast<QUFontListModel *>(sourceModel());
}

QUFontComboBox::QUFontComboBox(QWidget * parent)
    : QComboBox(parent) {
    QSortFilterProxyModel * proxy = new QUSortFilterFontListModel(this);
    proxy->setSourceModel(new QUFontListModel(this));
    setModel(proxy);
    proxy->sort(0);
    setEditable(true);
    connect(this, QOverload<int>::of(&QComboBox::activated),
            this, &QUFontComboBox::onFontSelected);

    //setDuplicatesEnabled(true);
}

QUFontURI
QUFontComboBox::selectedFont() const {
    int row = currentSourceIndex().row();
    auto desc = QUFontManager::get().db()->faceDescriptor(row);
    auto atts = QUFontManager::get().db()->faceAttributes(row);
    atts.names.familyName();
    QUFontURI uri{toQString(desc.filePath), desc.index};
    return uri;
}

QModelIndex
QUFontComboBox::currentProxyIndex() const {
    return model()->index(currentIndex(), modelColumn());   
}
    
QModelIndex
QUFontComboBox::currentSourceIndex() const {
    QSortFilterProxyModel * m = qobject_cast<QSortFilterProxyModel*>(model());
    return m->mapToSource(currentProxyIndex());
}

void
QUFontComboBox::onFontSelected(int ) {
    QModelIndex index = currentProxyIndex();
    QVariant data = model()->data(index, Qt::DisplayRole);
    lineEdit()->setText(data.toString());
}

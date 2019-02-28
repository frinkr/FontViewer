#include "QXFontListModel.h"

#include "FontX/FXFaceDatabase.h"
#include "QXApplication.h"
#include "QXConv.h"
#include "QXFontComboBox.h"
#include "QXFontManager.h"

QXFontListModel::QXFontListModel(QObject * parent)
    : QAbstractListModel(parent) {
}

int
QXFontListModel::rowCount(const QModelIndex & parent) const {
    return (int)db()->faceCount();
}
    
QVariant
QXFontListModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();
    if (index.row() >= db()->faceCount())
        return QVariant();
    if (role == Qt::DisplayRole) 
        return displayName(index.row());
    else if (role == Qt::DecorationRole)
        return icon(index.row());
    else
        return QVariant();
    
}

bool
QXFontListModel::acceptRow(const QString & filter, int row) const {
    if (filter.isEmpty())
        return true;
    
    if (displayName(row).contains(filter, Qt::CaseInsensitive))
        return true;

    auto const & atts = attributes(row);

    auto searchInNames = [](const FXMap<FXString, FXString> & names, const QString & name) {
        for (const auto it : names) {
            if (toQString(it.second).contains(name, Qt::CaseInsensitive))
                return true;
        }
        return false;
    };

    return searchInNames(atts.names.localizedFamilyNames(), filter) ||
        searchInNames(atts.names.localizedStyleNames(), filter) ||
        searchInNames(atts.names.localizedPostscriptNames(), filter);
}

const FXFaceAttributes &
QXFontListModel::attributes(size_t index) const {
    return db()->faceAttributes(index);
}

QString
QXFontListModel::displayName(size_t index) const {
    auto const & attrs = attributes(index);
    return QXDocument::faceDisplayName(attrs);
}

QIcon
QXFontListModel::icon(size_t index) const {
    auto const & attrs = attributes(index);
    if (attrs.format == FXFaceFormatConstant::CFF)
        return qApp->loadIcon(":/images/opentype.png");
    if (attrs.format == FXFaceFormatConstant::Type1)
        return qApp->loadIcon(":/images/postscript.png");
    if (attrs.format == FXFaceFormatConstant::TrueType)
        return qApp->loadIcon(":/images/truetype.png");
    if (attrs.format == FXFaceFormatConstant::WinFNT)
        return qApp->loadIcon(":/images/windowsfnt.png");
    return QIcon();
}

FXPtr<FXFaceDatabase>
QXFontListModel::db() const {
    return QXFontManager::instance().db();
}

bool
QXSortFilterFontListModel::lessThan(const QModelIndex & left, const QModelIndex & right) const {
    QVariant d0 = fontListModel()->data(left, Qt::DisplayRole);
    QVariant d1 = fontListModel()->data(right, Qt::DisplayRole);
    return d0 < d1;
    return true;
}

bool
QXSortFilterFontListModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
    if (filter_.isEmpty())
        return true;
    return fontListModel()->acceptRow(filter_, sourceRow);
}

void
QXSortFilterFontListModel::setFilter(const QString & text) {
    filter_ = text;
    invalidateFilter();
}

void
QXSortFilterFontListModel::clearFilter() {
    setFilter(QString());
}

QXFontListModel *
QXSortFilterFontListModel::fontListModel() const {
    return qobject_cast<QXFontListModel *>(sourceModel());
}

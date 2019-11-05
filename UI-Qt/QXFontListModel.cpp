#include <future>
#include <QFileInfo>
#include "FontX/FXFaceDatabase.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocument.h"
#include "QXFontListModel.h"
#include "QXFontManager.h"

namespace {

    template<typename T, typename U> size_t
    uiLevenshteinDistance(const T & s1, const U & s2) {
        const size_t m(s1.size());
        const size_t n(s2.size());
 
        if( m==0 ) return n;
        if( n==0 ) return m;
 
        size_t *costs = new size_t[n + 1];
 
        for( size_t k=0; k<=n; k++ ) costs[k] = k;
 
        size_t i = 0;
        for (auto it1 = s1.begin(); it1 != s1.end(); ++it1, ++i ) {
            costs[0] = i+1;
            size_t corner = i;
 
            size_t j = 0;
            for (auto it2 = s2.begin(); it2 != s2.end(); ++it2, ++j){
                size_t upper = costs[j+1];
                if( *it1 == *it2 ) {
                    costs[j+1] = corner;
                }
                else {
                    size_t t(upper<corner?upper:corner);
                    costs[j+1] = (costs[j]<t?costs[j]:t)+1;
                }
 
                corner = upper;
            }
        }
 
        size_t result = costs[n];
        delete [] costs;
 
        return result;
    }

    bool
    acceptWithFilter(const QString & string, const QString & filter) {
        QString s = string.toLower();
        QString f = filter.toLower();
        if (s.contains(filter))
            return true;
        if (filter.length() < string.length()) {
            s.resize(f.size());
            size_t ed = uiLevenshteinDistance(s, f);
            return ed < f.length() / 2;
        }
        else {
            size_t ed = uiLevenshteinDistance(s, f);
            return ed < 9;
        }
    }

    /**
     * Return true if left is closer to filter, otherwise false.
     */
    bool
    lessThanWithFilter(const QString & left, const QString & right, const QXFontListFilter & filter) {
        QString f = filter.fontName.toLower();
        QString str0 = left.toLower();
        QString str1 = right.toLower();
        
        bool s0 = str0.startsWith(f);
        bool s1 = str1.startsWith(f);
        bool c0 = str0.contains(f);
        bool c1 = str1.contains(f);

        if (s0 != s1)
            return s0;

        if (c0 != c1)
            return c0;

        //QRegExp sep("\\s+");
        QString sep(" ");
        size_t ed0 = uiLevenshteinDistance(str0.splitRef(sep, QString::SkipEmptyParts), f.splitRef(sep, QString::SkipEmptyParts));
        size_t ed1 = uiLevenshteinDistance(str1.splitRef(sep, QString::SkipEmptyParts), f.splitRef(sep, QString::SkipEmptyParts));
        if (ed0 != ed1)
            return ed0 < ed1;

        ed0 = uiLevenshteinDistance(str0, f);
        ed1 = uiLevenshteinDistance(str1, f);
        if (ed0 == ed1)
            return str0 < str1;
        return ed0 < ed1;
            
    }
}

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
QXFontListModel::acceptRow(const QXFontListFilter & filter, int row) const {
    if (filter.isEmpty())
        return true;
    
    auto searchInNames = [](const FXMap<FXString, FXString> & names, const QString & name) {
        for (const auto & it : names) {
            if (toQString(it.second).contains(name, Qt::CaseInsensitive))
                return true;
        }
        return false;
    };
    
    // Check names
    auto const & atts = attributes(row);
    auto fileName = QFileInfo(QString::fromStdString(atts.desc.filePath)).baseName();
    const bool acceptFontName = filter.fontName.isEmpty() ||
        acceptWithFilter(displayName(row), filter.fontName) ||
        acceptWithFilter(fileName, filter.fontName) ||
        searchInNames(atts.names.localizedFamilyNames(), filter.fontName) ||
        searchInNames(atts.names.localizedStyleNames(), filter.fontName) ||
        searchInNames(atts.names.localizedPostscriptNames(), filter.fontName);
    
    if (!acceptFontName)
        return false;
    
    // Check sample chars
    if (filter.converAllSampleCharacters && !filter.sampleText.isEmpty()) {
        const FXFaceDescriptor & desc = db()->faceDescriptor(row);
        
        FXPtr<FXFastFace> face = FXFastFace::create(desc);
        if (!face)
            return false;
        for (uint ch : filter.sampleText.toUcs4()) {
            if (!face->hasGlyphForChar(ch))
                return false;
        }
    }
    
    return true;
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
    if (!filter_.isEmpty()) 
        return lessThanWithFilter(d0.toString(), d1.toString(), filter_);
    
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
QXSortFilterFontListModel::setFilter(const QXFontListFilter & filter) {
    filter_ = filter;
    invalidate();
}

void
QXSortFilterFontListModel::clearFilter() {
    filter_.clear();
}

QXFontListModel *
QXSortFilterFontListModel::fontListModel() const {
    return qobject_cast<QXFontListModel *>(sourceModel());
}

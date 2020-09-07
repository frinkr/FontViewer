#include <future>
#include <QFileInfo>
#include "FontX/FXFaceDatabase.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocument.h"
#include "QXFontListModel.h"
#include "QXFontManager.h"
#include "score_match.h"

namespace {

    double
    fuzzMatch(const QString& str, const QString& filter) {
        auto s = str.toStdString();
        auto f = filter.toStdString();

        MatchOptions opt;
        opt.case_sensitive = true;
        opt.max_gap = 10;
        opt.smart_case = true;
        return score_match(s.c_str(), nullptr, f.c_str(), nullptr, opt);
    }

    bool
    acceptWithFilter(const QString & string, const QString & filter) {
        QString s = string.toLower();
        QString f = filter.toLower();
        if (s.contains(filter))
            return true;

        return fuzzMatch(s, f) > 0.3;
    }

    /**
     * Return true if left is closer to filter, otherwise false.
     */
    bool
    lessThanWithFilter(const QString & left, const QString & right, const QXFontListFilter & filter) {
        QString f = filter.fontName.toLower();
        QString str0 = left.toLower();
        QString str1 = right.toLower();

        return fuzzMatch(str0, f) < fuzzMatch(str1, f);
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
    auto filePath = QString::fromStdString(atts.desc.filePath);
    auto fileName = QFileInfo(filePath).fileName();
    const bool acceptFontName = filter.fontName.isEmpty() ||
        acceptWithFilter(displayName(row), filter.fontName) ||
        acceptWithFilter(fileName, filter.fontName) ||
        acceptWithFilter(filePath, filter.fontName) ||
        searchInNames(atts.sfntNames.localizedFamilyNames(), filter.fontName) ||
        searchInNames(atts.sfntNames.localizedStyleNames(), filter.fontName) ||
        searchInNames(atts.sfntNames.localizedPostscriptNames(), filter.fontName) 
        ;
    
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

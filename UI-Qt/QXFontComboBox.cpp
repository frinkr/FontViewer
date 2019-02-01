#include <QAbstractItemView>
#include <QApplication>
#include <QEvent>
#include <QFileInfo>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QValidator>

#include "FontX/FXFaceDatabase.h"
#include "QXApplication.h"
#include "QXConv.h"
#include "QXFontComboBox.h"
#include "QXFontManager.h"

namespace {
    class QUFalseValidator : public QValidator {
    public:
        using QValidator::QValidator;
        QValidator::State	
        validate(QString &input, int &pos) const override {
            return QValidator::Intermediate;
        }
    };
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

QXFontComboBox::QXFontComboBox(QWidget * parent)
    : QComboBox(parent) {
    QSortFilterProxyModel * proxy = new QXSortFilterFontListModel(this);
    proxy->setSourceModel(new QXFontListModel(this));
    setModel(proxy);
    proxy->sort(0);
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setValidator(new QUFalseValidator(this));
    setDuplicatesEnabled(true);
    setCompleter(nullptr);
    setMaxVisibleItems(20);
    
    connect(this, QOverload<int>::of(&QComboBox::activated),
            this, &QXFontComboBox::onFontSelected);

    connect(lineEdit(), &QLineEdit::textEdited,
            this, &QXFontComboBox::onLineEdited);

    view()->installEventFilter(this);
}

int
QXFontComboBox::selectedFontIndex() const {
    return currentSourceIndex().row();
}

QXFontURI
QXFontComboBox::selectedFont() const {
    int row = selectedFontIndex();
    if (row == -1)
        return QXFontURI{};
    
    auto desc = QXFontManager::instance().db()->faceDescriptor(row);
    auto atts = QXFontManager::instance().db()->faceAttributes(row);
    atts.names.familyName();
    QXFontURI uri{toQString(desc.filePath), desc.index};
    return uri;
}

int
QXFontComboBox::selectFont(int index) {
    QModelIndex proxyIndex = proxyModel()->mapFromSource(sourceModel()->index(index));
    setCurrentIndex(proxyIndex.row());
    onFontSelected(proxyIndex.row());
    return selectedFontIndex();
}
int
QXFontComboBox::selectFont(const QXFontURI & fontURI) {
    proxyModel()->clearFilter();
    int index = sourceModel()->db()->faceIndex({toStdString(fontURI.filePath), fontURI.faceIndex});
    return selectFont(index);
}

QXFontListModel *
QXFontComboBox::sourceModel() const {
    return qobject_cast<QXFontListModel *>(proxyModel()->sourceModel());
}

QXSortFilterFontListModel *
QXFontComboBox::proxyModel() const {
    return qobject_cast<QXSortFilterFontListModel*>(model());
}

QModelIndex
QXFontComboBox::currentProxyIndex() const {
    QModelIndex index = model()->index(currentIndex(), modelColumn());
    return index;
}
    
QModelIndex
QXFontComboBox::currentSourceIndex() const {
    return proxyModel()->mapToSource(currentProxyIndex());
}

void
QXFontComboBox::showPopup() {
    QComboBox::showPopup();
    //qreal y = mapToGlobal(rect().topLeft()).y();
    //QWidget *popup = this->findChild<QFrame*>(); 
    //popup->move(popup->x(),popup->y() + (popup->y() > y? 8 : -4));
}

bool
QXFontComboBox::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        QKeyEvent* newEvent = new QKeyEvent(keyEvent->type(),
                                            keyEvent->key(),
                                            keyEvent->modifiers(), 
                                            keyEvent->text(),
                                            keyEvent->isAutoRepeat(),
                                            keyEvent->count());

        QFocusEvent* focusEvent = new QFocusEvent(QEvent::FocusIn, Qt::OtherFocusReason);
        QCoreApplication::postEvent(lineEdit(), focusEvent);
        QCoreApplication::postEvent(lineEdit(), newEvent);
    }
    return false;
}

void
QXFontComboBox::onFontSelected(int ) {
    QModelIndex index = currentProxyIndex();
    QVariant data = model()->data(index, Qt::DisplayRole);
    lineEdit()->setText(data.toString());

    emit fontSelected(selectedFont(), selectedFontIndex());
}

void
QXFontComboBox::onLineEdited(const QString & text) {
    proxyModel()->setFilter(text);
    lineEdit()->setText(text);
    showPopup();
}

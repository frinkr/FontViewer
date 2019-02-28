#include <QAbstractItemView>
#include <QApplication>
#include <QEvent>
#include <QFileInfo>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTimer>
#include <QValidator>

#include "QXApplication.h"
#include "QXConv.h"
#include "QXFontComboBox.h"
#include "QXFontListModel.h"
#include "QXFontManager.h"

namespace {
    class QXFalseValidator : public QValidator {
    public:
        using QValidator::QValidator;
        QValidator::State	
        validate(QString &input, int &pos) const override {
            return QValidator::Intermediate;
        }
    };
}

QXFontComboBox::QXFontComboBox(QWidget * parent)
    : QComboBox(parent) {
    QSortFilterProxyModel * proxy = new QXSortFilterFontListModel(this);
    proxy->setSourceModel(new QXFontListModel(this));
    setModel(proxy);
    proxy->sort(0);
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setValidator(new QXFalseValidator(this));
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
    clearFilter();
    int index = sourceModel()->db()->faceIndex({toStdString(fontURI.filePath), fontURI.faceIndex});
    return selectFont(index);
}

void
QXFontComboBox::clearFilter() {
    proxyModel()->clearFilter();
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

#include <QAbstractItemView>
#include <QFile>
#include <QHBoxLayout>
#include <QIODevice>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QTextStream>

#include "QXApplication.h"
#include "QXPreferences.h"
#include "QXShapingTextCombobox.h"

namespace {
    QStringList
    readLinesFromFile(const QString & filePath) {
        QFile inputFile(filePath);
        QStringList lines;
        if (inputFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inputFile);
            while (!in.atEnd()) {
                if (QString line = in.readLine(); !line.isEmpty())
                    lines << line;
            }
            inputFile.close();
        }
        return lines;
    }
    
}

Qt::ItemFlags QXShapingTextListModel::flags(const QModelIndex &index) const {
    if (index.row() == 0)
        return Qt::NoItemFlags;
    else
        return QStringListModel::flags(index);
}

void
QXShapingTextListModel::clear() {
    removeRows(1, rowCount() - 1);
}

QStringList
QXShapingTextListModel::loadStrings() {
    QStringList list;
    list << "";
    list << readLinesFromFile(QXPreferences::filePathInAppData("ShapingHistory.txt"));
            
    if (list.size() == 1)
        list << readLinesFromFile(":/shaping-samples.txt");
    return list;
}

void
QXShapingTextListModel::saveStrings(const QStringList & stringList) {
    QString filePath = QXPreferences::filePathInAppData("ShapingHistory.txt");
    QFile outputFile(filePath);
    if (outputFile.open(QIODevice::WriteOnly)) {
        QTextStream out(&outputFile);
        for (auto & string: stringList)
            if (!string.isEmpty())
                out << string << "\n";
    }
}
        
QXShapingTextListModel & QXShapingTextListModel::instance() {
    static QXShapingTextListModel model = []() {

        return QXShapingTextListModel(loadStrings());
    }();

    static bool registerSaveOnce = []() {
        qApp->connect(qApp, &QXApplication::aboutToQuit, [] () {
            saveStrings(model.stringList());
        });
        return true;
    }();
            
    return model;
}


bool 
QXShapingTextComboboxItemDelegate::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index) {
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
        QRect rect = closeButtonRect(option, index);
        if (rect.contains(mouseEvent->pos())) {
            if (event->type() == QEvent::MouseButtonPress) 
                //model->removeRow(index.row());
                emit requestDeleteItem(index);
                    
        }
        return true; // to repaint the cell
    }
    return false;
}
        
QSize
QXShapingTextComboboxItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = Parent::sizeHint(option, index);            
    size.setHeight(heightForRow(index.row()));
    return size;
}

int
QXShapingTextComboboxItemDelegate::heightForRow(int row) const {
    if (row == 0)
        return 40;
    else
        return 30;
}

void
QXShapingTextComboboxItemDelegate::fitPopupViewHeightToModel(QAbstractItemView * view, QAbstractItemModel * model) const {
    int height = 0;
    for (int row = 0; row < model->rowCount(); ++ row)
        height += heightForRow(row);
    view->window()->setFixedHeight(height);
}
        
void
QXShapingTextComboboxItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();            
    if (index.row() == 0) {
        painter->fillRect(option.rect, option.palette.window());
    }
    else {
        Parent::paint(painter, option, index);
            
        const auto closeRect = closeButtonRect(option, index);
        const auto offset = closeRect.width() * 0.2;

        //auto pen = painter->pen();
        QPen pen(option.palette.color(option.state & QStyle::State_Selected? QPalette::HighlightedText: QPalette::Text));
        pen.setWidthF(1.5);
        
        painter->setPen(pen);
        painter->drawLine(QPointF(closeRect.left() + offset + 1, closeRect.top() + offset + 1), QPointF(closeRect.right() - offset, closeRect.bottom() - offset));
        painter->drawLine(QPointF(closeRect.left() + offset + 1, closeRect.bottom() - offset), QPointF(closeRect.right() - offset, closeRect.top() + offset + 1));

        pen.setWidth(1);
        painter->setPen(pen);
        if (auto widget = qobject_cast<const QListView*>(option.widget)) {
            auto mousePos = widget->viewport()->mapFromGlobal(QCursor::pos() );
            if (closeRect.contains(mousePos))
                painter->drawRect(closeRect);
        }
    }
    painter->restore();
}

QRect
QXShapingTextComboboxItemDelegate::closeButtonRect(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto rect = option.rect;
    rect.setLeft(rect.right() - rect.height());

    auto margin = rect.height() / 4;
    return rect.marginsRemoved(QMargins(margin, margin, margin, margin));
}


void
QXShapingTextCombobox::init() {
    QListView * popupView = new QListView(this);
    auto & model = QXShapingTextListModel::instance();
    popupView->setModel(&model);
    auto delegate = new QXShapingTextComboboxItemDelegate;

    // init the first row
    QWidget * itemWidget = new QWidget();
    itemWidget->setAttribute(Qt::WA_MouseTracking);
    QPushButton * addButton = new QPushButton("Add To History");
    QPushButton * clearButton = new QPushButton("Clear History");
    QPushButton * saveButton = new QPushButton("Save To File...");
    QPushButton * loadButton = new QPushButton("Load From File...");

    QHBoxLayout * itemWidgetLayout = new QHBoxLayout(itemWidget);
    itemWidgetLayout->addStretch();
    itemWidgetLayout->addWidget(addButton);
    itemWidgetLayout->addWidget(clearButton);
    itemWidgetLayout->addWidget(saveButton);
    itemWidgetLayout->addWidget(loadButton);
    itemWidgetLayout->addStretch();
    itemWidgetLayout->setContentsMargins(5, 5, 5, 5);
    popupView->setIndexWidget(model.index(0), itemWidget);

    connect(addButton, &QPushButton::clicked, this, [&model, this, popupView, delegate]() {
        if (auto text = this->currentText(); !text.isEmpty()) {
            model.insertRow(1);
            model.setData(model.index(1), text);
            delegate->fitPopupViewHeightToModel(popupView, &model);                
        }
    });
    connect(clearButton, &QPushButton::clicked, this, [&model, this, popupView, delegate]() {
        auto text = this->currentText();
        model.clear();
        this->setCurrentText(text);
        delegate->fitPopupViewHeightToModel(popupView, &model);
    });

    
    popupView->setItemDelegate(delegate);
    popupView->setAlternatingRowColors(true);
    
    this->setModel(popupView->model());
    this->setView(popupView);

    connect(delegate, &QXShapingTextComboboxItemDelegate::requestDeleteItem, this, [&model](auto index) {
        model.removeRow(index.row());
    });
}

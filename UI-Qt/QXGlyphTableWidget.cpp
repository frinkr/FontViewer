#include <QFile>
#include <QFileDialog>
#include <QItemDelegate>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStandardPaths>
#include <QStyleOptionFocusRect>
#include <QStyledItemDelegate>
#include <QTextStream>

#include "QXConv.h"
#include "QXEncoding.h"
#include "QXDocument.h"
#include "QXGlyphTableWidget.h"
#include "QXSearchEngine.h"
#include "ui_QXGlyphTableWidget.h"

class QXGlyphTableModelColumn : public QObject {
public:
    explicit QXGlyphTableModelColumn(const QString & name, QObject * parent = nullptr)
        : QObject(parent)
        , name_(name)
    {}
        
    virtual QVariant
    header(int role) const {
        if (role == Qt::DisplayRole) 
            return name();
        return QVariant();;
    }
        
    virtual QVariant
    data(const FXGlyph & g, int role) const {
        if (role == Qt::DisplayRole) 
            return value(g);
        return QVariant();
    }

    virtual QString
    name() const {
        return name_;
    }

    virtual QVariant
    value(const FXGlyph & g) const = 0;

protected:
    QString      name_;
};

class QUGlyphTableBitmapDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void 
    paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
        painter->save();
        
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        const QXGlyph ug = qvariant_cast<QXGlyph>(index.data());
        const FXGlyph & g = ug.g();

        QIcon icon(QPixmap::fromImage(placeGlyphImage(g, glyphEmSize())));
                   
        QIcon::Mode mode = QIcon::Normal;
        if (!(opt.state & QStyle::State_Enabled))
            mode = QIcon::Disabled;
        else if (opt.state & QStyle::State_Selected)
            mode = QIcon::Selected;
        else if (opt.state & QStyle::State_MouseOver)
            mode = QIcon::Selected;
        
        QIcon::State state = opt.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
        
        QPixmap bk(opt.rect.size());
        bk.fill(Qt::white);
        QIcon bkIcon(bk);
        bkIcon.paint(painter, opt.rect, Qt::AlignHCenter, mode, state);
        
        icon.paint(painter, opt.rect, Qt::AlignHCenter, mode, state);

        painter->restore();
    }
};

namespace {

    template <typename T>
    QVariant
    toVariant(T value) {
        return value;
    }

    QVariant
    toVariant(const FXString & str) {
        return toQString(str);
    }

    QVariant
    toVariant(const FXGChar & c) {
        if (c.isUnicode() && c.isValid()) {
            char32_t i = c.value;
            return QString::fromUcs4(&i, 1);
        }
        return QVariant();
    }
    
    template <class PointerToMember>
    class QUGlyphBasicColumn : public QXGlyphTableModelColumn {
    public:
        QUGlyphBasicColumn(const QString & name, PointerToMember p, QObject * parent = nullptr)
            : QXGlyphTableModelColumn(name, parent)
            , p_(p)
        {}
        
        virtual QVariant
        value(const FXGlyph & g) const {
            return toVariant(g.*p_);
        }
    protected:
        PointerToMember p_;
    };
    
    template <class PointerToMember>
    class QUGlyphMetricColumn : public QXGlyphTableModelColumn {
    public:
        QUGlyphMetricColumn(const QString & name, PointerToMember p, QObject * parent = nullptr)
            : QXGlyphTableModelColumn(name, parent)
            , p_(p) {}

        virtual QVariant
        value(const FXGlyph & g) const {
            return toVariant(g.metrics.*p_);
        }
    protected:
        PointerToMember p_;
    };

    
    class QUGlyphCodePointColumn : public QXGlyphTableModelColumn {
    public:
        using QXGlyphTableModelColumn::QXGlyphTableModelColumn;
        QVariant
        value(const FXGlyph & g) const {
            if (g.character.isChar() && g.character.isValid())
                return QXEncoding::charHexNotation(g.character);
            return QVariant();
        }
    };

    class QUGlyphImageColumn : public QXGlyphTableModelColumn {
    public:
        using QXGlyphTableModelColumn::QXGlyphTableModelColumn;
        QVariant
        value(const FXGlyph & g) const {
            QVariant v;
            v.setValue(QXGlyph(g));
            return v;
        }
    };

    template <typename PointerToMember>
    QUGlyphBasicColumn<PointerToMember> *
    makeBasicColumn(const QString & name, PointerToMember p, QObject * parent) {
        return new QUGlyphBasicColumn<PointerToMember>(name, p, parent);
    }

    
    template <typename PointerToMember>
    QUGlyphMetricColumn<PointerToMember> *
    makeMetricColumn(const QString & name, PointerToMember p, QObject * parent) {
        return new QUGlyphMetricColumn<PointerToMember>(name, p, parent);
    }

}

QXGlyphTableModel::QXGlyphTableModel(QXDocument * document, QObject * parent)
    : QAbstractTableModel(parent)
    , document_(document) {

    columns_.append(makeBasicColumn(tr("Index"), &FXGlyph::gid, this));
    columns_.append(new QUGlyphImageColumn(tr("Glyph"), this));
    columns_.append(new QUGlyphCodePointColumn(tr("Codepoint"), this));
    columns_.append(makeBasicColumn(tr("Char"), &FXGlyph::character, this));
    columns_.append(makeBasicColumn(tr("Name"), &FXGlyph::name, this));
    columns_.append(makeMetricColumn(tr("Width"), &FXGlyphMetrics::width, this));
    columns_.append(makeMetricColumn(tr("Height"), &FXGlyphMetrics::height, this));
    columns_.append(makeMetricColumn(tr("Hori Bearing X"), &FXGlyphMetrics::horiBearingX, this));
    columns_.append(makeMetricColumn(tr("Hori Bearing Y"), &FXGlyphMetrics::horiBearingY, this));
    columns_.append(makeMetricColumn(tr("Hori Advance"), &FXGlyphMetrics::horiAdvance, this));
    columns_.append(makeMetricColumn(tr("Vert Bearing X"), &FXGlyphMetrics::vertBearingX, this));
    columns_.append(makeMetricColumn(tr("Vert Bearing Y"), &FXGlyphMetrics::vertBearingY, this));
    columns_.append(makeMetricColumn(tr("Vert Advance"), &FXGlyphMetrics::vertAdvance, this));

    connect(document_, &QXDocument::cmapActivated,
            this, &QXGlyphTableModel::reset);
}

int
QXGlyphTableModel::rowCount(const QModelIndex & parent) const {
    return document_->face()->glyphCount();
}

int
QXGlyphTableModel::columnCount(const QModelIndex & parent) const {
    return columns_.size();
}
    
QVariant
QXGlyphTableModel::data(const QModelIndex & index, int role) const {
    return columns_[index.column()]->data(glyphAt(index.row()), role);
}

QVariant
QXGlyphTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) 
        return columns_[section]->header(role);
    return QVariant();
}

const QVector<QXGlyphTableModelColumn *> &
QXGlyphTableModel::columns() const {
    return columns_;
}

FXGlyph
QXGlyphTableModel::glyphAt(int row) const {
    FXGChar gc(row, FXGCharTypeGlyphID);
    return document_->face()->glyph(gc);
}

void
QXGlyphTableModel::reset() {
    beginResetModel();
    endResetModel();
}

bool
QXGlyphTableModel::exportToFile(const QString & filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream ts(&file);
    for (int i = 0; i < columns_.size(); ++ i) {
        if (i == 1) continue; // skip the glyph image column
        ts << columns_[i]->name() << ", ";
    }
    ts << "\n";
    for (size_t i = 0; i < document_->face()->glyphCount(); ++ i) {
        FXGChar gc(i, FXGCharTypeGlyphID);
        FXGlyph g = document_->face()->glyph(gc);
        for (int i = 0; i < columns_.size(); ++ i) {
            if (i == 1) continue;
            QVariant v = columns_[i]->data(g, Qt::DisplayRole);
            ts << v.toString() << ", ";
        }
        ts << "\n";
    }
    return true;
}

QUGlyphTableWidget::QUGlyphTableWidget(QXDocument * document, QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QXGlyphTableWidget)
    , document_(document) {
    ui_->setupUi(this);

    model_ = new QXGlyphTableModel(document, this);
    
    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model_);
    ui_->tableView->setModel(proxyModel);
    
    ui_->tableView->verticalHeader()->hide();
    ui_->tableView->setItemDelegateForColumn(1, new QUGlyphTableBitmapDelegate(this));
    ui_->tableView->setSortingEnabled(true);
    ui_->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableView->sortByColumn(0, Qt::AscendingOrder);
    ui_->tableView->setAlternatingRowColors(true);
    
    connect(ui_->pushButton, &QPushButton::clicked,
            this, &QUGlyphTableWidget::exportToFile);

    connect(ui_->tableView, &QTableView::doubleClicked,
            this, &QUGlyphTableWidget::gotoGlyphAtIndex);
}

QUGlyphTableWidget::~QUGlyphTableWidget() {
    delete ui_;
}

void
QUGlyphTableWidget::exportToFile() {
    QString file = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + toQString(document_->face()->postscriptName());
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export"), file,
        tr("CSV (*.csv);;All Files (*)"));

    if (!fileName.isEmpty())
        model_->exportToFile(fileName);
}

void
QUGlyphTableWidget::gotoGlyphAtIndex(const QModelIndex & index) {
    const QSortFilterProxyModel * proxyModel = qobject_cast<const QSortFilterProxyModel *>(index.model());
    QModelIndex sourceIndex = proxyModel->mapToSource(index);

    FXGlyph g = model_->glyphAt(sourceIndex.row());
    FXGChar gc = g.character;
    if (!gc.isChar() || !gc.isValid())
        gc = FXGChar(g.gid, FXGCharTypeGlyphID);

    QXSearch s;
    s.gchar = gc;

    document_->search(s);
}

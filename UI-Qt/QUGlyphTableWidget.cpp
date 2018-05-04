#include "QUConv.h"
#include "QUEncoding.h"
#include "QUDocument.h"
#include "QUGlyphTableWidget.h"
#include "ui_QUGlyphTableWidget.h"

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
        if (c.value == FXCharInvalid)
            return QString();
        return QUEncoding::charHexNotation(c);
    }
    
    template <class PointerToMember>
    class QUGlyphBasicColumn : public QUGlyphTableModelColumn {
    public:
        QUGlyphBasicColumn(const QString & name, PointerToMember p, QObject * parent = nullptr)
            : QUGlyphTableModelColumn(name, parent)
            , p_(p) {}

        virtual QVariant
        value(const FXGlyph & g) {
            return toVariant(g.*p_);
        };
    protected:
        PointerToMember p_;
    };
    
    template <class PointerToMember>
    class QUGlyphMetricColumn : public QUGlyphTableModelColumn {
    public:
        QUGlyphMetricColumn(const QString & name, PointerToMember p, QObject * parent = nullptr)
            : QUGlyphTableModelColumn(name, parent)
            , p_(p) {}

        virtual QVariant
        value(const FXGlyph & g) {
            return toVariant(g.metrics.*p_);
        };
    protected:
        PointerToMember p_;
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

QUGlyphTableModel::QUGlyphTableModel(QUDocument * document, QObject * parent)
    : QAbstractTableModel(parent)
    , document_(document) {

    columns_.append(makeBasicColumn(tr("Index"), &FXGlyph::gid, this));
    columns_.append(makeBasicColumn(tr("Code"), &FXGlyph::character, this));
    columns_.append(makeBasicColumn(tr("Name"), &FXGlyph::name, this));
    columns_.append(makeMetricColumn(tr("Width"), &FXGlyphMetrics::width, this));
    columns_.append(makeMetricColumn(tr("Height"), &FXGlyphMetrics::height, this));
    columns_.append(makeMetricColumn(tr("Hori Bearing X"), &FXGlyphMetrics::horiBearingX, this));
    columns_.append(makeMetricColumn(tr("Hori Bearing Y"), &FXGlyphMetrics::horiBearingY, this));
    columns_.append(makeMetricColumn(tr("Hori Advance"), &FXGlyphMetrics::horiAdvance, this));
    columns_.append(makeMetricColumn(tr("Vert Bearing X"), &FXGlyphMetrics::vertBearingX, this));
    columns_.append(makeMetricColumn(tr("Vert Bearing Y"), &FXGlyphMetrics::vertBearingY, this));
    columns_.append(makeMetricColumn(tr("Vert Advance"), &FXGlyphMetrics::vertAdvance, this));
    
}

int
QUGlyphTableModel::rowCount(const QModelIndex & parent) const {
    return document_->face()->glyphCount();
}

int
QUGlyphTableModel::columnCount(const QModelIndex & parent) const {
    return columns_.size();
}
    
QVariant
QUGlyphTableModel::data(const QModelIndex & index, int role) const {
    int gid = index.row();
    FXGChar gc(FXGCharTypeGlyphID, gid);
    FXGlyph g = document_->face()->glyph(gc);
    return columns_[index.column()]->data(g, role);
}

QVariant
QUGlyphTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) 
        return columns_[section]->header(role);
    return QVariant();
}

const QVector<QUGlyphTableModelColumn *> &
QUGlyphTableModel::columns() const {
    return columns_;
}


QUGlyphTableWidget::QUGlyphTableWidget(QUDocument * document, QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QUGlyphTableWidget)
    , document_(document) {
    ui_->setupUi(this);

    ui_->tableView->setModel(new QUGlyphTableModel(document, this));
    ui_->tableView->verticalHeader()->hide();
}

QUGlyphTableWidget::~QUGlyphTableWidget() {
    delete ui_;
}

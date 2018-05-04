#include "QUConv.h"
#include "QUDocument.h"
#include "QUGlyphTableWidget.h"
#include "ui_QUGlyphTableWidget.h"

namespace {
    class QUGlyphIndexColumn: public QUGlyphTableModelColumn {
    public:
        using QUGlyphTableModelColumn::QUGlyphTableModelColumn;
        virtual QVariant
        value(const FXGlyph & g) {
            return g.gid;
        };
    };

    class QUGlyphNameColumn: public QUGlyphTableModelColumn {
    public:
        using QUGlyphTableModelColumn::QUGlyphTableModelColumn;
        virtual QVariant
        value(const FXGlyph & g) {
            return toQString(g.name);
        };
    };

    template <class PointerToMember>
    class QUGlyphBasicColumn : public QUGlyphTableModelColumn {
    public:
        QUGlyphBasicColumn(const QString & name, PointerToMember p, QObject * parent = nullptr)
            : QUGlyphTableModelColumn(name, parent)
            , p_(p) {}

        virtual QVariant
        value(const FXGlyph & g) {
            return g.*p_;
        };
    protected:
        PointerToMember p_;
    };

    template <typename PointerToMember>
    QUGlyphBasicColumn<PointerToMember> *
    makeColumn(const QString & name, PointerToMember p, QObject * parent) {
        return new QUGlyphBasicColumn<PointerToMember>(name, p, parent);
    }
}

QUGlyphTableModel::QUGlyphTableModel(QUDocument * document, QObject * parent)
    : QAbstractTableModel(parent)
    , document_(document) {

    columns_.append(makeColumn(tr("Index"), &FXGlyph::gid, this));
    columns_.append(new QUGlyphNameColumn(tr("Name"), this));
    
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

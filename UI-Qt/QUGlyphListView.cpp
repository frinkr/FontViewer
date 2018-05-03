#include <QApplication>
#include <QIcon>
#include <QPainter>

#include "QUConv.h"
#include "QUEncoding.h"
#include "QUDocument.h"
#include "QUGlyphListView.h"

void
QUGlyphItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QUDocument * model = (QUDocument*)index.model();

    QVariant v = index.data(QUGlyphRole);
    if (!v.canConvert<QUGlyph>())
        return QStyledItemDelegate::paint(painter, option, index);

    const FXGlyph g = qvariant_cast<QUGlyph>(v).g();
    
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    painter->save();
    
    const QWidget * widget = opt.widget;
    QStyle * style = widget ? widget->style() : QApplication::style();
    
    QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    
    QSize emSize = glyphEmSize();
    QRect emRect(0, 0, emSize.width(), emSize.height());
        
    // draw the background
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

    // draw the icon
    if (true) {
        QIcon::Mode mode = QIcon::Normal;
        QImage image = placeImage(toQImage(g.bitmap), emSize);
        QIcon icon(QPixmap::fromImage(image));
        if (opt.state & QStyle::State_Selected)
            mode = QIcon::Selected;
        icon.paint(painter, iconRect, opt.decorationAlignment, mode, QIcon::On);
    }

    // draw the text
    if (true) {
        const QString charCode = QUEncoding::charHexNotation(g.character);

        QString text;
        switch (model->glyphLabel()) {
        case QUGlyphLabel::CharCode:
            text = charCode;
            break;
        case QUGlyphLabel::GlyphID:
            text = QString("%1").arg(g.gid);
            break;
        case QUGlyphLabel::GlyphName:
            text = toQString(g.name);
            if (!g.gid && model->charMode())
                text = charCode;
            break;

        }

        if (!text.isEmpty()) {
            if (opt.state & QStyle::State_Selected)
                painter->setPen(opt.palette.color(QPalette::Normal, QPalette::HighlightedText));
            else
                painter->setPen(opt.palette.color(QPalette::Normal, QPalette::Text));
            style->drawItemText(painter, textRect, Qt::AlignHCenter, opt.palette, true, text);
        }
    }
    
    painter->restore();
}

QUGlyphListView::QUGlyphListView(QWidget * parent)
    : QListView(parent){
    setLayoutMode(QListView::Batched);
    setBatchSize(100);
    setUniformItemSizes(true);
    setMovement(QListView::Static);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

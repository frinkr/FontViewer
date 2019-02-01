#include <QApplication>
#include <QIcon>
#include <QBitmap>
#include <QPainter>

#include "QXApplication.h"
#include "QXConv.h"
#include "QXEncoding.h"
#include "QXDocument.h"
#include "QXGlyphListView.h"

void
QXGlyphItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QXDocument * model = (QXDocument*)index.model();

    QVariant v = index.data(QXGlyphRole);
    if (!v.canConvert<QXGlyph>())
        return QStyledItemDelegate::paint(painter, option, index);

    const FXGlyph g = qvariant_cast<QXGlyph>(v).g();
    
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    const QWidget * widget = opt.widget;
    QStyle * style = widget ? widget->style() : QApplication::style();
    
    QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    
    QSize emSize = glyphEmSize();
    QRect emRect(0, 0, emSize.width(), emSize.height());
        
    // draw the background & focus
    if (true) {
#ifdef Q_OS_MACOS
        style->drawPrimitive(QStyle::PE_PanelItemViewRow, &opt, painter, widget);
#else
        if (opt.state & QStyle::State_Selected) {
            QRect bgRect = textRect.united(iconRect);
            painter->fillRect(bgRect, opt.palette.color(QPalette::Active, QPalette::Highlight));
        }
#endif
    }

    // draw the text
    if (true) {
        const QString charCode = g.character == FXCharInvalid? "N/A": QXEncoding::charHexNotation(g.character);

        QString text;
        switch (model->glyphLabel()) {
        case QXGlyphLabel::CharCode:
            text = charCode;
            break;
        case QXGlyphLabel::GlyphID:
            text = QString("%1").arg(g.gid);
            break;
        case QXGlyphLabel::GlyphName:
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
            textRect = QRect(iconRect.left() , textRect.top() - textRect.height(), iconRect.width(), textRect.height() * 2);
            painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWrapAnywhere, text);
        }
    }
    
    if (true) {
        QImage image = placeGlyphImage(g, emSize);
        if ((opt.state & QStyle::State_Selected) || qApp->darkMode())
            image.invertPixels();
        QPixmap pixmap = QPixmap::fromImage(image);
        painter->drawImage(iconRect, image, QRectF(0, 0, image.width(), image.height()));
    }

    
    painter->restore();
}

QXGlyphListView::QXGlyphListView(QWidget * parent)
    : QListView(parent){
    setLayoutMode(QListView::Batched);
    setBatchSize(100);
    setUniformItemSizes(true);
    setMovement(QListView::Static);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

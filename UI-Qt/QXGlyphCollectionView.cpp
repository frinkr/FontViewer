#include <QPainter>

#include "QXApplication.h"
#include "QXConv.h"
#include "QXEncoding.h"
#include "QXGlyphCollectionView.h"

namespace {
    constexpr int GLYPH_IMAGE_SIZE = 70;
    constexpr int GLYPH_NAME_HEIGHT = 10;

    Qt::GlobalColor colors[] = {Qt::cyan, Qt::yellow, Qt::magenta, Qt::darkGreen, Qt::darkBlue, Qt::red, Qt::green, Qt::blue, };

    class QXGlyphCollectionViewDelegate : public QXCollectionViewDelegate {
    public:
        QXGlyphCollectionViewDelegate(QObject * parent = nullptr)
            : QXCollectionViewDelegate(parent)
            , document_(nullptr) {
            
        }
        
        void
        drawItem(QPainter * painter, const QXCollectionViewDrawItemOption & option) override {
            auto & palette = option.widget->palette();
            // draw the background and focus
            bool hasFocus = option.widget->hasFocus();
            if (option.selected) {
                painter->fillRect(option.rect, palette.color(hasFocus?QPalette::Active: QPalette::Inactive, QPalette::Highlight));
            }

            auto data = document_->data(option.index, QXGlyphRole);
            if (!data.canConvert<QXGlyph>())
                return;

            const FXGlyph g = qvariant_cast<QXGlyph>(data).g();

            painter->save();
            painter->setRenderHint(QPainter::HighQualityAntialiasing);
            if (g.face->attributes().format != FXFaceFormatConstant::WinFNT)
                painter->setRenderHint(QPainter::SmoothPixmapTransform);


            
            // draw the text
            if (true) {
                const QString charCode = g.character == FXCharInvalid? "N/A": QXEncoding::charHexNotation(g.character);

                QString text;
                switch (document_->glyphLabel()) {
                case QXGlyphLabel::CharCode:
                    text = charCode;
                    break;
                case QXGlyphLabel::GlyphID:
                    text = QString("%1").arg(g.gid);
                    break;
                case QXGlyphLabel::GlyphName:
                    text = toQString(g.name);
                    if (!g.gid && document_->charMode())
                        text = charCode;
                    break;

                }

                if (!text.isEmpty()) {
                    painter->setPen(palette.color(hasFocus ? QPalette::Active : QPalette::Inactive, option.selected? QPalette::HighlightedText: QPalette::Text));
                    QRect textRect = option.rect.adjusted(0, GLYPH_IMAGE_SIZE, 0, 0);
                    painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWrapAnywhere, text);
                }
            }

            // image
            if (true) {
                QSize emSize = glyphEmSize();
                QRect emRect(0, 0, emSize.width(), emSize.height());
    
                QImage image = placeGlyphImage(g, emSize);
                if (g.face->isScalable() && (option.selected || qApp->darkMode()))
                    image.invertPixels();
                QPixmap pixmap = QPixmap::fromImage(image);
                QRect imageRect = option.rect.adjusted(0, 0, 0, -GLYPH_NAME_HEIGHT);
                painter->drawImage(imageRect, image, QRectF(0, 0, image.width(), image.height()));
            }

            painter->restore();
//            painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, document_->data(index, Qt::DisplayRole).toString());
        }

        void
        drawHeader(QPainter * painter, const QXCollectionViewDrawHeaderOption & option) override {
            auto bg = QColor(colors[option.section % (sizeof(colors) / sizeof(colors[0]))]).toRgb();
            auto fg = QColor(255 - bg.red(), 255 - bg.green(), 255 - bg.blue());
            painter->save();
            painter->setPen(option.widget->palette().color(option.selected? QPalette::HighlightedText: QPalette::Text));
            if (option.selected) {
                //painter->fillRect(option.rect, option.widget->palette().color(QPalette::Highlight));
            }
            QFont font = painter->font();
            font.setPixelSize(option.rect.height() - 2);
            font.setBold(true);
            painter->setFont(font);
            painter->drawText(option.rect, Qt::AlignCenter, document_->data(option.section).toString());
            painter->restore();
        }

        QXDocument * document_;
    };    
}

QXGlyphCollectionView::QXGlyphCollectionView(QWidget * parent)
    : QXCollectionView(parent){
    this->setCellSize(QSize(GLYPH_IMAGE_SIZE, GLYPH_IMAGE_SIZE + GLYPH_NAME_HEIGHT));
    this->setCellSpace(10);
    this->setSectionSpace(40);
    setDelegate(new QXGlyphCollectionViewDelegate(this));
}

QXDocument *
QXGlyphCollectionView::document() const {
    return document_;
}

void
QXGlyphCollectionView::setDocument(QXDocument * document) {
    document_ = document;
    (dynamic_cast<QXGlyphCollectionViewDelegate*>(delegate()))->document_ = document;
    setModel(document_);
}

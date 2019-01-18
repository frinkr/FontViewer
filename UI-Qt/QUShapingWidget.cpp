#include <QPainter>
#include <QMouseEvent>
#include "FontX/FXInspector.h"
#include "FontX/FXShaper.h"

#include "QUConv.h"
#include "QUDocument.h"
#include "QUSearchEngine.h"
#include "QUShapingWidget.h"
#include "ui_QUShapingWidget.h"

namespace {
    QVariant
    langSysToVariant(FXTag script, FXTag language) {
        return (qulonglong(script) << 32) + language;
    }

    bool
    variantToLangSys(const QVariant & v, FXTag & script, FXTag & language) {
        uint64_t i = v.value<uint64_t>();
        script = (i >> 32);
        language = (i & 0xFFFFFFFF);
        return true;
    }

    constexpr int QU_SHAPINGVIEW_MARGIN = 20;
    constexpr int QU_SHAPINGVIEW_GRID_ROW_HEIGHT = 20;
    constexpr int QU_SHAPINGVIEW_GRID_HEAD_WIDTH = 100;

}

QUShapingGlyphView::QUShapingGlyphView(QWidget * parent)
    : QWidget(parent)
    , selectedIndex_(-1)
    , fontSize_(100)
    , shaper_(nullptr) {}

void
QUShapingGlyphView::setShaper(FXShaper * shaper) {
    shaper_ = shaper;
}

QSize
QUShapingGlyphView::minimumSizeHint() const {
    return sizeHint();
}

QSize
QUShapingGlyphView::sizeHint() const {
    if (!shaper_)
        return QWidget::sizeHint();
    FXFace * face = shaper_->face();
    FXFace::AutoFontSize autoFontSize(face, fontSize_);
    
    const int baselineY = baseLinePosition().y();
    const int height = rect().height() - baselineY + fu2px(face->attributes().bbox.height()) + QU_SHAPINGVIEW_MARGIN;
    
    return QSize(gridCellLeft(5, shaper_->glyphCount()) + QU_SHAPINGVIEW_MARGIN, height);
}

void
QUShapingGlyphView::paintEvent(QPaintEvent * event) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    if (!shaper_)
        return ;
    const QPen gridPen(Qt::darkGray, 1, Qt::SolidLine);
    const QPen baseLinePen(Qt::green, 1, Qt::SolidLine);
    const QPen boundaryPen(Qt::red, 1, Qt::DashLine);
    const QPen originPen(Qt::blue, 1, Qt::SolidLine);
    const QPen textPen(Qt::black);

    /*
      |              |        |       |       |
      |              |        |       |       |
      |              |   A    |   B   |   c   |
      |              |        |       |       |
      |  -------------------------------------------------------------> baseline
      |              |
      |              |
      |  -------------------------------------------------------------
      |         gid  |   1    |   2   |  3    | ...
      |  -------------------------------------------------------------
      |  Natural Adv |  160   |  251  | 230   | ...
      |  -------------------------------------------------------------
      |  Shaping Adv |  155   |  240  | 230   | ...
      |  -------------------------------------------------------------
      |     kern         |   -5   |  -11  |   0   |
      |  -------------------------------------------------------------
      | 
    */
    FXFace * face = shaper_->face();
    FXFace::AutoFontSize autoFontSize(face, fontSize_);

    // draw baseline
    const int baseLineY = baseLinePosition().y();
    const int baseLineX = baseLinePosition().x();

    // draw selected glyph background
    {
        int penX = baseLineX;
        for (int i = 0; i < shaper_->glyphCount(); ++ i) {
            const FXVec2d<fu> adv = shaper_->advance(i);

            // draw background for selected
            if (selectedIndex_ == i) {
                QRect rect(penX, 0, fu2px(adv.x), gridCellBottom(1));
                p.fillRect(rect, QColor(179, 216, 253));
            }
            
            penX += fu2px(adv.x);
        }  
    }
    
    // draw glyphs
    {
        const int penY = baseLineY;
        
        int penX = baseLineX;
        for (int i = 0; i < shaper_->glyphCount(); ++ i) {
            const FXGlyphID gid = shaper_->glyph(i);
            const FXVec2d<fu> adv = shaper_->advance(i);
            const FXVec2d<fu> off = shaper_->offset(i);

            const double bmScale = face->bmScale();
            FXVec2d<int> bmOffset;
            
            FXPixmapARGB bm = face->pixmap(gid, &bmOffset);
            QImage img = toQImage(bm);

            const int left = penX + bmOffset.x;
            const int bottom = penY - bmOffset.y;
            const int right = left + bm.width * bmScale + fu2px(off.x);
            const int top = bottom - bm.height * bmScale - fu2px(off.y);

            p.drawImage(QRect(QPoint(left, top), QPoint(right, bottom)),
                        img,
                        QRect(0, 0, bm.width, bm.height),
                        Qt::AutoColor
                );
            
            
            penX += fu2px(adv.x);
        }        
    }

    // draw glyph boundary
    {
        int penX = baseLineX;
        for (int i = 0; i <= shaper_->glyphCount(); ++ i) {

            p.setPen(boundaryPen);
            p.drawLine(penX, 0, penX, gridCellBottom(4, i));
            
            if (i == shaper_->glyphCount())
                break;
            const FXVec2d<fu> adv = shaper_->advance(i);
            penX += fu2px(adv.x);
        }
    }

    // draw baseline
    {
        p.setPen(baseLinePen);
        p.drawLine(gridCellLeft(6, -1), baseLineY, rect().right(), baseLineY);
    }
        
    // draw grid
    {
        p.setPen(gridPen);

        // row lines
        for (int i = 0; i < 5; ++ i) 
            p.drawLine(QU_SHAPINGVIEW_MARGIN, gridCellBottom(i),
                       rect().right(), gridCellBottom(i));
        // column lines
        for (int i = 0; i <= shaper_->glyphCount(); ++ i)  {
            p.drawLine(gridCellLeft(4, i), gridCellBottom(4),
                       gridCellLeft(1, i), gridCellBottom(1));
            p.drawLine(gridCellLeft(0, i), gridCellBottom(1),
                       gridCellLeft(0, i), gridCellBottom(0));
        }
        
        // headers
        p.setPen(textPen);
        p.drawText(gridCellRect(1, -1).translated(0, QU_SHAPINGVIEW_GRID_ROW_HEIGHT), Qt::AlignRight, tr("Kern "));
        p.drawText(gridCellRect(1, -1), Qt::AlignRight, tr("Shaping Adv. "));
        p.drawText(gridCellRect(2, -1), Qt::AlignRight, tr("Natural Adv. "));
        p.drawText(gridCellRect(3, -1), Qt::AlignRight, tr("GID "));

        // Values
        for (int i = 0; i < shaper_->glyphCount(); ++ i) {
            FXGlyphID gid = shaper_->glyph(i);
            FXVec2d<fu> adv = shaper_->advance(i);
            FXGlyph g = face->glyph(FXGChar(gid, FXGCharTypeGlyphID));
            fu kern = adv.x - g.metrics.horiAdvance;
            
            p.drawText(gridCellRect(0, i), Qt::AlignCenter, QString::number(kern));
            p.drawText(gridCellRect(1, i), Qt::AlignCenter, QString::number(adv.x));
            p.drawText(gridCellRect(2, i), Qt::AlignCenter, QString::number(g.metrics.horiAdvance));
            p.drawText(gridCellRect(3, i), Qt::AlignCenter, QString::number(gid));
        }
    }
}

void
QUShapingGlyphView::mousePressEvent(QMouseEvent *event) {
    FXFace * face = shaper_->face();
    FXFace::AutoFontSize autoFontSize(face, fontSize_);

    int index = glyphAtPoint(event->pos());
    if (selectedIndex_ != index) {
        selectedIndex_ = index;
        update();
    }
}

void
QUShapingGlyphView::mouseDoubleClickEvent(QMouseEvent *event) {
    mousePressEvent(event);
    if (selectedIndex_ != -1)
        emit glyphDoubleClicked(shaper_->glyph(selectedIndex_));
}

void
QUShapingGlyphView::setFontSize(double fontSize) {
    fontSize_ = fontSize;
}

QPoint
QUShapingGlyphView::baseLinePosition() const {
    return QPoint(gridCellLeft(5, 0),
                  gridCellBottom(5, 0) + fu2px(shaper_->face()->attributes().descender));
}

int
QUShapingGlyphView::gridCellBottom(int row, int col) const {
    return rect().height() - QU_SHAPINGVIEW_MARGIN - QU_SHAPINGVIEW_GRID_ROW_HEIGHT * row;
}

int
QUShapingGlyphView::gridCellLeft(int row, int col) const {
    // col == -1: header
    // col == shaper_->glyphCount : last glyph right
    if (col < 0)
        return QU_SHAPINGVIEW_MARGIN;
    if (row == 0) 
        return (gridCellLeft(1, col) + gridCellLeft(1, col + 1)) / 2;

    int adv = 0;
    for (int i = 0; i < std::min<int>(col, shaper_->glyphCount()); ++ i) 
        adv += fu2px(shaper_->advance(i).x);

    return QU_SHAPINGVIEW_MARGIN + QU_SHAPINGVIEW_GRID_HEAD_WIDTH + adv;
}

QRect
QUShapingGlyphView::gridCellRect(int row, int col) const {
    int x0 = gridCellLeft(row, col);
    int x1 = gridCellLeft(row, col + 1);
    int y0 = gridCellBottom(row + 1);
    int y1 = gridCellBottom(row);
    return QRect(QPoint(x0, y0), QPoint(x1, y1));
}

QRect
QUShapingGlyphView::glyphInteractionRect(int index) const {
    int x0 = gridCellLeft(1, index);
    int x1 = gridCellLeft(1, index + 1);
    int y0 = 0;
    int y1 = gridCellBottom(1);
    return QRect(QPoint(x0, y0), QPoint(x1, y1));
}

int
QUShapingGlyphView::glyphAtPoint(const QPoint & point) const {
    if (!shaper_) return -1;
    for (int i = 0; i < shaper_->glyphCount(); ++ i) {
        QRect rect = glyphInteractionRect(i);
        if (rect.contains(point))
            return i;
    }
    return -1;
}
    

double
QUShapingGlyphView::fu2px(fu f) const {
    return shaper_->face()->fontSize() * f / shaper_->face()->upem();
}
    
QUShapingWidget::QUShapingWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::QUShapingWidget)
    , document_(nullptr)
    , shaper_(nullptr) {
    ui_->setupUi(this);

    ui_->featureListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    // connect signals
    connect(ui_->langSysComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QUShapingWidget::reloadFeatureList);
    connect(ui_->featureListWidget, &QListWidget::itemSelectionChanged,
            this, &QUShapingWidget::doShape);
    connect(ui_->lineEdit, &QLineEdit::textEdited,
            this, &QUShapingWidget::doShape);
    connect(ui_->fontSizeComboBox, &QComboBox::currentTextChanged,
            this, &QUShapingWidget::doShape);
    connect(ui_->glyphView, &QUShapingGlyphView::glyphDoubleClicked,
            this, &QUShapingWidget::gotoGlyph);
}

QUShapingWidget::~QUShapingWidget() {
    delete shaper_;
    delete ui_;
}

void
QUShapingWidget::setDocument(QUDocument * document) {
    document_ = document;
    delete shaper_;
    shaper_ = new FXShaper(document_->face().get());

    ui_->glyphView->setShaper(shaper_);

    reloadScriptList();
}

void
QUShapingWidget::reloadScriptList() {
    ui_->langSysComboBox->clear();

    const FXVector<FXTag> scripts = inspector()->otScripts();
    for (FXTag script : scripts) {
        const FXVector<FXTag> languages = inspector()->otLanguages(script);
        for (FXTag language : languages) {
            ui_->langSysComboBox->addItem(
                QString("%1 %2 [%3-%4]").arg(
                    toQString(FXOT::scriptName(script)),
                    toQString(FXOT::languageName(language)),
                    toQString(FXTag2Str(script)),
                    toQString(FXTag2Str(language))),
                langSysToVariant(script, language));
        }
    }

    ui_->langSysComboBox->setCurrentIndex(0);
    reloadFeatureList();
}

void
QUShapingWidget::reloadFeatureList() {
    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);
    const FXVector<FXTag> features =inspector()->otFeatures(script, language);

    ui_->featureListWidget->clear();
    for (FXTag feature : features) {
        QListWidgetItem * item = new QListWidgetItem(toQString(FXTag2Str(feature)));
        item->setData(Qt::UserRole, feature);
        item->setData(Qt::ToolTipRole, toQString(FXOT::featureName(feature)));
        ui_->featureListWidget->addItem(item);
        if (FXOT::autoFeatures.find(feature) != FXOT::autoFeatures.end())
            item->setSelected(true);
    }

    doShape();
}

void
QUShapingWidget::doShape() {
    if (!shaper_)
        return;

    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);

    shaper_->shape(toStdString(ui_->lineEdit->text()),
                   script,
                   language,
                   FXShappingLTR,
                   onFeatures(),
                   offFeatures());

    bool ok = false;
    double fontSize = ui_->fontSizeComboBox->currentText().toDouble(&ok);
    if (!ok)
        fontSize = 100;
    
    ui_->glyphView->setFontSize(fontSize);
    ui_->glyphView->updateGeometry();
    ui_->glyphView->update();
}

void
QUShapingWidget::gotoGlyph(FXGlyphID gid) {
    QUSearch s;
    s.gchar = FXGChar(gid, FXGCharTypeGlyphID);
    document_->search(s);
}
    
FXPtr<FXInspector>
QUShapingWidget::inspector() {
    return document_->face()->inspector();
}

FXVector<FXTag>
QUShapingWidget::onFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        if (item->isSelected())
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;
}

FXVector<FXTag>
QUShapingWidget::offFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        if (!item->isSelected())
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;
}

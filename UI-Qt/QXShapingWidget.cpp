#include <QPainter>
#include <QMenu>
#include <QMouseEvent>
#include "FontX/FXInspector.h"
#include "FontX/FXShaper.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocument.h"
#include "QXDocumentWindow.h"
#include "QXDocumentWindowManager.h"
#include "QXEncoding.h"
#include "QXSearchEngine.h"
#include "QXShapingWidget.h"
#include "ui_QXShapingWidget.h"

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

    constexpr int QX_SHAPINGVIEW_MARGIN = 20;
    constexpr int QX_SHAPINGVIEW_GRID_ROW_HEIGHT = 20;
    constexpr int QX_SHAPINGVIEW_GRID_HEAD_WIDTH = 100;

}

QXShapingGlyphView::QXShapingGlyphView(QWidget * parent)
    : QWidget(parent)
    , selectedIndex_(-1)
    , fontSize_(100)
    , shaper_(nullptr) {
    setFocusPolicy(Qt::StrongFocus);
}

void
QXShapingGlyphView::setShaper(FXShaper * shaper) {
    shaper_ = shaper;
}

void
QXShapingGlyphView::setDocument(QXDocument * document) {
    document_ = document;
}

QSize
QXShapingGlyphView::minimumSizeHint() const {
    return sizeHint();
}

QSize
QXShapingGlyphView::sizeHint() const {
    if (!shaper_)
        return QWidget::sizeHint();
    FXFace * face = shaper_->face();
    FXFace::AutoFontSize autoFontSize(face, fontSize_);
    
    const int baselineY = baseLinePosition().y();
    const int height = rect().height() - baselineY + fu2px(face->attributes().bbox.height()) + QX_SHAPINGVIEW_MARGIN;
    
    return QSize(gridCellLeft(5, shaper_->glyphCount()) + QX_SHAPINGVIEW_MARGIN, height);
}

void
QXShapingGlyphView::paintEvent(QPaintEvent * event) {
    const QPalette & p = palette();
    QPainter painter(this);
    painter.fillRect(rect(), p.base());

    if (!shaper_)
        return ;
    const QPen gridPen(Qt::darkGray, 1, Qt::SolidLine);
    const QPen baseLinePen(Qt::green, 1, Qt::SolidLine);
    const QPen boundaryPen(Qt::red, 1, Qt::DashLine);
    const QPen originPen(Qt::blue, 1, Qt::SolidLine);
    const QPen textPen(p.color(QPalette::Text));

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

    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    if (face->attributes().format != FXFaceFormatConstant::WinFNT)
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

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
                painter.fillRect(rect, palette().color(this->hasFocus()? QPalette::Active: QPalette::Inactive, QPalette::Highlight));
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
            if (face->isScalable() && ((i == selectedIndex_) || qApp->darkMode()))
                img.invertPixels();

            const int left = penX + bmOffset.x;
            const int bottom = penY - bmOffset.y;
            const int right = left + bm.width * bmScale + fu2px(off.x);
            const int top = bottom - bm.height * bmScale - fu2px(off.y);

            painter.drawImage(QRect(QPoint(left, top), QPoint(right, bottom)),
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

            painter.setPen(boundaryPen);
            painter.drawLine(penX, 0, penX, gridCellBottom(4, i));
            
            if (i == shaper_->glyphCount())
                break;
            const FXVec2d<fu> adv = shaper_->advance(i);
            penX += fu2px(adv.x);
        }
    }

    // draw baseline
    {
        painter.setPen(baseLinePen);
        painter.drawLine(gridCellLeft(-1, -1), baseLineY, rect().right(), baseLineY);
    }
        
    // draw grid
    {
        painter.setPen(gridPen);

        // row lines
        for (int i = 0; i < 6; ++ i)
            painter.drawLine(QX_SHAPINGVIEW_MARGIN, gridCellBottom(i),
                       rect().right(), gridCellBottom(i));
        // column lines
        for (int i = 0; i <= shaper_->glyphCount(); ++ i)  {
            painter.drawLine(gridCellLeft(4, i), gridCellBottom(4),
                       gridCellLeft(1, i), gridCellBottom(1));
            painter.drawLine(gridCellLeft(0, i), gridCellBottom(1),
                       gridCellLeft(0, i), gridCellBottom(0));
        }
        
        // headers
        painter.setPen(textPen);
        painter.drawText(gridCellRect(1, -1).translated(0, QX_SHAPINGVIEW_GRID_ROW_HEIGHT), Qt::AlignRight, tr("Kern "));
        painter.drawText(gridCellRect(1, -1), Qt::AlignRight, tr("Shaping Adv. "));
        painter.drawText(gridCellRect(2, -1), Qt::AlignRight, tr("Natural Adv. "));
        painter.drawText(gridCellRect(3, -1), Qt::AlignRight, tr("GID "));
        painter.drawText(gridCellRect(4, -1), Qt::AlignRight, tr("Index "));
        
        // Values
        for (int i = 0; i < shaper_->glyphCount(); ++ i) {
            FXGlyphID gid = shaper_->glyph(i);
            FXVec2d<fu> adv = shaper_->advance(i);
            FXGlyph g = face->glyph(FXGChar(gid, FXGCharTypeGlyphID));
            fu kern = adv.x - g.metrics.horiAdvance;
            
            painter.drawText(gridCellRect(0, i), Qt::AlignCenter, QString::number(kern));
            painter.drawText(gridCellRect(1, i), Qt::AlignCenter, QString::number(adv.x));
            painter.drawText(gridCellRect(2, i), Qt::AlignCenter, QString::number(g.metrics.horiAdvance));
            painter.drawText(gridCellRect(3, i), Qt::AlignCenter, QString::number(gid));
            painter.drawText(gridCellRect(4, i), Qt::AlignCenter, QString::number(i));
        }
    }
}

void
QXShapingGlyphView::mousePressEvent(QMouseEvent *event) {
    FXFace * face = shaper_->face();
    FXFace::AutoFontSize autoFontSize(face, fontSize_);

    int index = glyphAtPoint(event->pos());
    if (selectedIndex_ != index) {
        selectedIndex_ = index;
        update();
    }
}


void
QXShapingGlyphView::mouseMoveEvent(QMouseEvent *event) {
    mousePressEvent(event);
}

void
QXShapingGlyphView::mouseDoubleClickEvent(QMouseEvent *event) {
    mousePressEvent(event);
    if (selectedIndex_ != -1) {
        auto gid = shaper_->glyph(selectedIndex_);
        
        QXDocumentWindow * documentWindow = QXDocumentWindowManager::instance()->getDocumentWindow(document_);
        if (documentWindow) {
            QRect rect = glyphInteractionRect(selectedIndex_);
            QRect globalRect(this->mapToGlobal(rect.topLeft()),
                             this->mapToGlobal(rect.bottomRight()));
            documentWindow->showGlyphPopover(FXGChar(gid, FXGCharTypeGlyphID),
                                             globalRect,
                                             QXPopoverTop);
        }
        
        emit glyphDoubleClicked(gid);
    }
}

void
QXShapingGlyphView::setFontSize(double fontSize) {
    fontSize_ = fontSize;
}

QPoint
QXShapingGlyphView::baseLinePosition() const {
    return QPoint(gridCellLeft(6, 0),
                  gridCellBottom(6, 0) + fu2px(shaper_->face()->attributes().descender));
}

int
QXShapingGlyphView::gridCellBottom(int row, int col) const {
    return rect().height() - QX_SHAPINGVIEW_MARGIN - QX_SHAPINGVIEW_GRID_ROW_HEIGHT * row;
}

int
QXShapingGlyphView::gridCellLeft(int row, int col) const {
    // col == -1: header
    // col == shaper_->glyphCount : last glyph right
    if (col < 0)
        return QX_SHAPINGVIEW_MARGIN;
    if (row == 0) 
        return (gridCellLeft(1, col) + gridCellLeft(1, col + 1)) / 2;

    int adv = 0;
    for (int i = 0; i < std::min<int>(col, shaper_->glyphCount()); ++ i) 
        adv += fu2px(shaper_->advance(i).x);

    return QX_SHAPINGVIEW_MARGIN + QX_SHAPINGVIEW_GRID_HEAD_WIDTH + adv;
}

QRect
QXShapingGlyphView::gridCellRect(int row, int col) const {
    int x0 = gridCellLeft(row, col);
    int x1 = gridCellLeft(row, col + 1);
    int y0 = gridCellBottom(row + 1);
    int y1 = gridCellBottom(row);
    return QRect(QPoint(x0, y0), QPoint(x1, y1));
}

QRect
QXShapingGlyphView::glyphInteractionRect(int index) const {
    int x0 = gridCellLeft(1, index);
    int x1 = gridCellLeft(1, index + 1);
    int y0 = 0;
    int y1 = gridCellBottom(1);
    return QRect(QPoint(x0, y0), QPoint(x1, y1));
}

int
QXShapingGlyphView::glyphAtPoint(const QPoint & point) const {
    if (!shaper_) return -1;
    for (int i = 0; i < shaper_->glyphCount(); ++ i) {
        QRect rect = glyphInteractionRect(i);
        if (rect.contains(point))
            return i;
    }
    return -1;
}
    

double
QXShapingGlyphView::fu2px(fu f) const {
    if (!shaper_->face()->upem())
        return shaper_->face()->fontSize();
    else
        return shaper_->face()->fontSize() * f / shaper_->face()->upem();
}
    
QXShapingWidget::QXShapingWidget(QWidget * parent)
    : QWidget(parent)
    , ui_(new Ui::QXShapingWidget)
    , document_(nullptr)
    , shaper_(nullptr) {
    ui_->setupUi(this);
    ui_->menuButton->setIcon(qApp->loadIcon(":/images/menu.png"));
    ui_->featureListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    // connect signals
    connect(ui_->langSysComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QXShapingWidget::reloadFeatureList);
    connect(ui_->featureListWidget, &QListWidget::itemSelectionChanged,
            this, &QXShapingWidget::doShape);
    connect(ui_->lineEdit, &QLineEdit::textEdited,
            this, &QXShapingWidget::doShape);
    connect(ui_->fontSizeComboBox, &QComboBox::currentTextChanged,
            this, &QXShapingWidget::doShape);
    connect(ui_->actionCopyDecodedText, &QAction::triggered,
            this, &QXShapingWidget::doCopyAction);
    connect(ui_->actionToggleOtPanel, &QAction::triggered,
            this, &QXShapingWidget::doTogglePanelAction);
    connect(ui_->glyphView, &QXShapingGlyphView::glyphDoubleClicked,
            this, &QXShapingWidget::gotoGlyph);

    QMenu * menu = new QMenu(this);
    menu->addAction(ui_->actionCopyDecodedText);
    menu->addAction(ui_->actionToggleOtPanel);
    ui_->menuButton->setMenu(menu);

    warningAction_ = new QAction(this);
    warningAction_->setIcon(qApp->loadIcon(":/images/warning.png"));
    warningAction_->setToolTip(tr("OpenType shaping is not available, fallback to basic shaping."));
}

QXShapingWidget::~QXShapingWidget() {
    delete shaper_;
    delete ui_;
}

void
QXShapingWidget::setDocument(QXDocument * document) {
    document_ = document;
    delete shaper_;
    shaper_ = new FXShaper(document_->face().get());

    ui_->glyphView->setShaper(shaper_);
    ui_->glyphView->setDocument(document);
    
    reloadScriptList();
}

void
QXShapingWidget::reloadScriptList() {
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
QXShapingWidget::reloadFeatureList() {
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
QXShapingWidget::doShape() {
    if (!shaper_)
        return;

    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);

    shaper_->shape(toStdString(QXEncoding::decodeFromHexNotation(ui_->lineEdit->text())),
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
    
    if (shaper_->hasFallbackShaping())
        ui_->lineEdit->addAction(warningAction_, QLineEdit::TrailingPosition);
    else 
        ui_->lineEdit->removeAction(warningAction_);
}

void
QXShapingWidget::gotoGlyph(FXGlyphID gid) {
    QXSearch s;
    s.gchar = FXGChar(gid, FXGCharTypeGlyphID);
    document_->search(s);
}

FXPtr<FXInspector>
QXShapingWidget::inspector() {
    return document_->face()->inspector();
}

FXVector<FXTag>
QXShapingWidget::onFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        if (item->isSelected())
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;
}

FXVector<FXTag>
QXShapingWidget::offFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        if (!item->isSelected())
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;
}

void
QXShapingWidget::doCopyAction() {
    QString text = QXEncoding::decodeFromHexNotation(ui_->lineEdit->text());
    qApp->copyTextToClipBoard(text);
    qApp->message(QXDocumentWindowManager::instance()->getDocumentWindow(document_), QString(), text);
}

void
QXShapingWidget::doTogglePanelAction() {
    if (ui_->leftWidget->isHidden())
        ui_->leftWidget->show();
    else
        ui_->leftWidget->hide();
}

void
QXShapingWidget::focusLineEdit(bool selectAll) {
    ui_->lineEdit->setFocus();
    if (selectAll)
        ui_->lineEdit->selectAll();
}

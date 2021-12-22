#include <QFontDatabase>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QTextStream>

#include "FontX/FXInspector.h"
#include "FontX/FXShaper.h"

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocument.h"
#include "QXDocumentWindow.h"
#include "QXDocumentWindowManager.h"
#include "QXEncoding.h"
#include "QXImageHelpers.h"
#include "QXSearchEngine.h"
#include "QXShapingWidget.h"
#include "QXShapingOptionsWidget.h"
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


    QStringList
    loadSamples() {
        QFile inputFile(":/shaping-samples.txt");
        QStringList samples;
        if (inputFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inputFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                samples << line;
            }
            inputFile.close();
        }
        return samples;
    }
    
    constexpr int QX_SHAPINGVIEW_MARGIN = 20;
    constexpr int QX_SHAPINGVIEW_GRID_ROW_HEIGHT = 20;
    constexpr int QX_SHAPINGVIEW_GRID_HEAD_WIDTH = 100;
    constexpr int QX_SHAPINGVIEW_TOTAL_ROW = 7;
}

QXShapingGlyphView::QXShapingGlyphView(QWidget * parent)
    : QWidget(parent)
    , selectedColIndex_(-1)
    , selectedRowIndex_(-1)
    , shaper_(nullptr) {
    setFocusPolicy(Qt::StrongFocus);
}

void
QXShapingGlyphView::setShaper(FXShaper * shaper) {
    shaper_ = shaper;
}

void
QXShapingGlyphView::setOptions(const QXShapingOptions & options) {
    options_ = options;
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
    FXFace::AutoFontSize autoFontSize(face, options_.fontSize);
    
    const int baselineY = baseLinePosition().y();
    const int height = rect().height() - baselineY + fu2px(face->attributes().bbox.height()) + QX_SHAPINGVIEW_MARGIN;
    
    return QSize(gridCellLeft(1, shaper_->glyphCount()) + QX_SHAPINGVIEW_MARGIN, height);
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
    FXFace::AutoFontSize autoFontSize(face, options_.fontSize);

    painter.setRenderHint(QPainter::Antialiasing);
    if (face->attributes().format != FXFaceFormatConstant::WinFNT)
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // alternative color
    if (options_.alternativeRowColor) {
        for (int i = 1; i < QX_SHAPINGVIEW_TOTAL_ROW; i += 2) {
            if (i == selectedRowIndex_)
                continue;
            
            auto rect = gridCellRect(i, shaper_->glyphCount());
            rect.setLeft(gridCellLeft(i, -1));
            painter.fillRect(rect, palette().brush(QPalette::Inactive, QPalette::Highlight));
        }
    }

    // draw baseline
    const int baseLineY = baseLinePosition().y();
    const int baseLineX = baseLinePosition().x();

    // draw selected glyph background
    {

        if (selectedColIndex_ != -1) {
            int penX = baseLineX;
            for (int i = 0; i < shaper_->glyphCount(); ++ i) {
                const FXVec2d<fu> adv = shaper_->advance(i);

                // draw background for selected
                if (selectedColIndex_ == i) {
                    QRect rect(penX, 0, fu2px(adv.x), gridCellBottom(1));
                    painter.fillRect(rect, palette().color(this->hasFocus()? QPalette::Active: QPalette::Inactive, QPalette::Highlight));
                }
            
                penX += fu2px(adv.x);
            }
        }
        else if (selectedRowIndex_ != -1) {
            auto rect = gridCellRect(selectedRowIndex_, shaper_->glyphCount());
            rect.setLeft(gridCellLeft(selectedRowIndex_, -1));
            painter.fillRect(rect, palette().color(this->hasFocus()? QPalette::Active: QPalette::Inactive, QPalette::Highlight));   
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

            QColor textColor = p.color(QPalette::Text);
            if (i == selectedColIndex_)
                textColor = p.color(hasFocus() ? QPalette::Active : QPalette::Inactive, QPalette::HighlightedText);

            FXGlyphImage gi = fillGlyphImageWithColor(face->glyphImage(gid), textColor);
            QImage img = toQImage(gi);

            const int left = penX + gi.offset.x * gi.scale + fu2px(off.x);
            const int bottom = penY - gi.offset.y * gi.scale - fu2px(off.y);
            const int right = left + gi.pixmap.width * gi.scale;
            const int top = bottom - gi.pixmap.height * gi.scale;

            painter.drawImage(QRect(QPoint(left, top), QPoint(right, bottom)),
                        img,
                        QRect(0, 0, gi.pixmap.width, gi.pixmap.height),
                        Qt::AutoColor
                );
            
            
            penX += fu2px(adv.x);
        }        
    }

    // draw glyph boundary
    if (options_.showGlyphsBoundary)
    {
        int penX = baseLineX;
        for (int i = 0; i <= shaper_->glyphCount(); ++ i) {

            painter.setPen(boundaryPen);
            painter.drawLine(penX, 0, penX, gridCellBottom(QX_SHAPINGVIEW_TOTAL_ROW - 1, i));
            
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
        for (int i = 0; i < QX_SHAPINGVIEW_TOTAL_ROW; ++ i)
            painter.drawLine(QX_SHAPINGVIEW_MARGIN, gridCellBottom(i),
                       rect().right(), gridCellBottom(i));
        // column lines
        for (int i = 0; i <= shaper_->glyphCount(); ++ i)  {
            painter.drawLine(gridCellLeft(QX_SHAPINGVIEW_TOTAL_ROW - 1, i), gridCellBottom(QX_SHAPINGVIEW_TOTAL_ROW - 1),
                       gridCellLeft(1, i), gridCellBottom(1));
            painter.drawLine(gridCellLeft(0, i), gridCellBottom(1), // for kern row
                       gridCellLeft(0, i), gridCellBottom(0));
        }
        
        // headers
        painter.setPen(textPen);
        painter.drawText(gridCellRect(1, -1).translated(0, QX_SHAPINGVIEW_GRID_ROW_HEIGHT), Qt::AlignRight, tr("Kern "));
        painter.drawText(gridCellRect(1, -1), Qt::AlignRight, tr("Shaping Adv. "));
        painter.drawText(gridCellRect(2, -1), Qt::AlignRight, tr("Natural Adv. "));
        painter.drawText(gridCellRect(3, -1), Qt::AlignRight, tr("Cluster "));
        painter.drawText(gridCellRect(4, -1), Qt::AlignRight, tr("GID "));
        painter.drawText(gridCellRect(5, -1), Qt::AlignRight, tr("Index "));
        
        // Values
        for (int i = 0; i < shaper_->glyphCount(); ++ i) {
            FXGlyphID gid = shaper_->glyph(i);
            FXVec2d<fu> adv = shaper_->advance(i);
            size_t cluster = shaper_->cluster(i);
            FXGlyph g = face->glyph(FXGChar(gid, FXGCharTypeGlyphID));
            fu kern = adv.x - g.metrics.horiAdvance;
            
            painter.drawText(gridCellRect(0, i), Qt::AlignCenter, QString::number(kern));
            painter.drawText(gridCellRect(1, i), Qt::AlignCenter, QString::number(adv.x));
            painter.drawText(gridCellRect(2, i), Qt::AlignCenter, QString::number(g.metrics.horiAdvance));
            painter.drawText(gridCellRect(3, i), Qt::AlignCenter, QString::number(cluster));
            painter.drawText(gridCellRect(4, i), Qt::AlignCenter, QString::number(gid));
            painter.drawText(gridCellRect(5, i), Qt::AlignCenter, QString::number(i));
        }
    }
}

void
QXShapingGlyphView::mousePressEvent(QMouseEvent *event) {
    if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        selectedColIndex_ = -1;

        int newRowIndex = -1;
        for (int i = 0; (i + 1) < QX_SHAPINGVIEW_TOTAL_ROW; ++ i) {
            auto bottom = gridCellBottom(i);
            auto top = gridCellBottom(i + 1);
            if (event->pos().y() < bottom && event->pos().y() > top) {
                newRowIndex = i;
            }
        }
        if (newRowIndex != -1 && newRowIndex != selectedRowIndex_) {
            selectedRowIndex_ = newRowIndex;
            update();
        }
    }
    else {
        selectedRowIndex_ = -1;
        FXFace * face = shaper_->face();
        FXFace::AutoFontSize autoFontSize(face, options_.fontSize);

        int index = glyphAtPoint(event->pos());
        if (selectedColIndex_ != index) {
            selectedColIndex_ = index;
            update();
        }
    }
}


void
QXShapingGlyphView::mouseMoveEvent(QMouseEvent *event) {
    mousePressEvent(event);
}

void
QXShapingGlyphView::mouseDoubleClickEvent(QMouseEvent *event) {
    mousePressEvent(event);
    if (selectedColIndex_ != -1) {
        auto gid = shaper_->glyph(selectedColIndex_);
        
        QXDocumentWindow * documentWindow = QXDocumentWindowManager::instance()->getDocumentWindow(document_);
        if (documentWindow) {
            QRect rect = glyphInteractionRect(selectedColIndex_);
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

QXShapingGlyphView::wheelEvent(QWheelEvent * event) {
    if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        if (double angle = event->angleDelta().x()) {
            double scale = qPow(1.0015, angle);
            qPow(1.0015, angle);
            
            options_.fontSize *= scale;
            
            updateGeometry();
            update();
            event->accept();

            emit fontSizeChanged(options_.fontSize);
            
            return;
        }
    }
    
    QWidget::wheelEvent(event);
}

QPoint
QXShapingGlyphView::baseLinePosition() const {
    return QPoint(gridCellLeft(QX_SHAPINGVIEW_TOTAL_ROW, 0),
                  gridCellBottom(QX_SHAPINGVIEW_TOTAL_ROW, 0) + fu2px(shaper_->face()->attributes().descender));
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
    //ui_->featureListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui_->textComboBox->addItems(loadSamples());
        
    // connect signals
    connect(ui_->langSysComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QXShapingWidget::reloadFeatureList);
    connect(ui_->featureListWidget, &QListWidget::itemChanged,
            this, &QXShapingWidget::doShape);    
    connect(ui_->textComboBox, &QComboBox::editTextChanged,
            this, &QXShapingWidget::doShape);
    connect(ui_->glyphView, &QXShapingGlyphView::glyphDoubleClicked,
            this, &QXShapingWidget::gotoGlyph);
    connect(ui_->menuButton, &QPushButton::clicked,
            this, &QXShapingWidget::showOptionsPopover);

    warningAction_ = new QAction(this);
    warningAction_->setIcon(qApp->loadIcon(":/images/warning.png"));
    warningAction_->setToolTip(tr("OpenType shaping is not available, fallback to basic shaping."));

    ui_->menuButton->setIcon(qApp->loadIcon(":/images/truetype.png"));

    // Popover
    optionsPopover_ = new QXPopoverWindow(this);
    optionsWidget_ = new QXShapingOptionsWidget(this);
    optionsPopover_->setWidget(optionsWidget_);

    connect(optionsWidget_, &QXShapingOptionsWidget::togglePanelButtonClicked,
            this, &QXShapingWidget::doTogglePanelAction);

    connect(optionsWidget_, &QXShapingOptionsWidget::copyTextButtonClicked,
            this, &QXShapingWidget::doCopyAction);

    connect(optionsWidget_, &QXShapingOptionsWidget::copyHexButtonClicked,
            this, &QXShapingWidget::doCopyHexAction);

    connect(optionsWidget_, &QXShapingOptionsWidget::copyHexCStyleButtonClicked,
            this, &QXShapingWidget::doCopyHexCStyleAction);
    
    connect(optionsWidget_, &QXShapingOptionsWidget::optionsChanged,
            this, &QXShapingWidget::doShape);

    connect(ui_->glyphView, &QXShapingGlyphView::fontSizeChanged,
            optionsWidget_, &QXShapingOptionsWidget::setFontSize);

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

    connect(document, &QXDocument::variableCoordinatesChanged, [this]() {
        delete shaper_;
        shaper_ = new FXShaper(document_->face().get());
        ui_->glyphView->setShaper(shaper_);
        doShape();
    });
        
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

    //QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    //font.setPointSizeF(font.pointSizeF() * 1.5);
    
    ui_->featureListWidget->clear();
    for (FXTag feature : features) {
        QListWidgetItem * item = new QListWidgetItem(QString::fromStdU16String(u"\u3000 ") + toQString(FXTag2Str(feature)));
        item->setData(Qt::UserRole, feature);
        item->setData(Qt::UserRole + 1, 0);
        item->setData(Qt::ToolTipRole, toQString(FXOT::featureName(feature)));
        //item->setFont(font);
        ui_->featureListWidget->addItem(item);
    }

    connect(ui_->featureListWidget, &QListWidget::itemClicked, [this](QListWidgetItem * item) {
            int state = item->data(Qt::UserRole + 1).value<int>();
            state = (state + 2) % 3 - 1;
        
            item->setData(Qt::UserRole + 1, state);
            
            QString text = item->text();
            text[0] = state? (state == 1? u'\uFF0B': u'\uFF0D'): u'\u3000';
            item->setText(text);
            
        });
    
    doShape();
}

void
QXShapingWidget::doShape() {
    if (!shaper_)
        return;

    //auto face = document_->face();
    //auto g = face->glyph(0x1F44B, 0xFE01);
        
    auto options = optionsWidget_->options();
    options.general.onFeatures = onFeatures();
    options.general.offFeatures = offFeatures();
    
    FXTag script, language;
    variantToLangSys(ui_->langSysComboBox->currentData(), script, language);

    shaper_->shape(toStdString(QXEncoding::decodeFromGidNotation(QXEncoding::decodeFromHexNotation(ui_->textComboBox->currentText()))),
                   script,
                   language,
                   options.general,
                   options.bidi);

    ui_->glyphView->setOptions(options);
    ui_->glyphView->updateGeometry();
    ui_->glyphView->update();
    
    if (shaper_->hasFallbackShaping())
        ui_->textComboBox->lineEdit()->addAction(warningAction_, QLineEdit::TrailingPosition);
    else 
        ui_->textComboBox->lineEdit()->removeAction(warningAction_);
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
        int state = item->data(Qt::UserRole + 1).value<int>();
        if (state == 1)
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    features.push_back('locl');
    return features;
}

FXVector<FXTag>
QXShapingWidget::offFeatures() const {
    FXVector<FXTag> features;
    for (int i = 0; i < ui_->featureListWidget->count(); ++ i) {
        QListWidgetItem * item = ui_->featureListWidget->item(i);
        int state = item->data(Qt::UserRole + 1).value<int>();
        if (state == -1)
            features.push_back(item->data(Qt::UserRole).value<FXTag>());
    }
    return features;
}

void
QXShapingWidget::doCopyAction() {
    copyTextToClipboard(QXEncoding::decodeFromHexNotation(ui_->textComboBox->currentText()));
}

void
QXShapingWidget::doCopyHexAction() {
    QString text = QXEncoding::decodeFromHexNotation(ui_->textComboBox->currentText());
    text = QXEncoding::encodeToHexNotation(text, false);
    copyTextToClipboard(text);
}

void
QXShapingWidget::doCopyHexCStyleAction() {
    QString text = QXEncoding::decodeFromHexNotation(ui_->textComboBox->currentText());
    text = QXEncoding::encodeToHexNotation(text, true);
    copyTextToClipboard(text);    
}

void
QXShapingWidget::copyTextToClipboard(const QString & text) {
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
    ui_->textComboBox->setFocus();
    if (selectAll)
        ui_->textComboBox->lineEdit()->selectAll();
}

void 
QXShapingWidget::showOptionsPopover() {
    optionsPopover_->showRelativeTo(ui_->menuButton, QXPopoverTop);
}


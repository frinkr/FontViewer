#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTimer>

#include "FontX/FXCache.h"
#include "FontX/FXBenchmark.h"
#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocumentWindowManager.h"
#include "QXFontListWidget.h"
#include "QXFontListModel.h"
#include "QXFontManager.h"
#include "QXHtmlTemplate.h"
#include "QXPopoverWindow.h"
#include "QXImageHelpers.h"
#include "QXResources.h"
#include "ui_QXFontListWidget.h"

namespace {
    constexpr qreal MIN_PREVIEW_FONT_SIZE = 20;
    constexpr qreal MAX_PREVIEW_FONT_SIZE = 200;
    constexpr qreal DEFAULT_PREVIEW_FONT_SIZE = 30;
    const QString DEFAULT_PRVIEW_TEXT = "The quick fox jumps over the lazy dog";

    
    
    class QXFontBrowserItemDelegate : public QStyledItemDelegate {
    private:
        qreal    infoIconMargin_{ 10 };
        qreal    infoIconSize_{ 20 };
        qreal    fontSize_ {DEFAULT_PREVIEW_FONT_SIZE};
        QString  previewText_;
        mutable FXCache<FXFaceDescriptor, FXPtr<FXFace>> faceCache_{50}; // cache 50 faces
        mutable QRect    infoIconRect_{};
    public:
        QXFontBrowserItemDelegate(QWidget * parent = 0)
            : QStyledItemDelegate(parent)
            , previewText_(DEFAULT_PRVIEW_TEXT){
        }

        ~QXFontBrowserItemDelegate() {
            faceCache_.clear();   
        }

        void
        setFontSize(qreal fontSize) {
            fontSize_ = fontSize;
        }

        void
        setPreviewText(const QString & previewText) {
            if (previewText.isEmpty())
                previewText_ = DEFAULT_PRVIEW_TEXT;
            else
                previewText_ = previewText;
        }

        QRect
        infoIconRect(const QStyleOptionViewItem & option) const {
            qreal right = option.rect.right() - infoIconMargin_;
            qreal left = right - infoIconSize_;
            qreal top = option.rect.top() + (option.rect.height() - infoIconSize_) / 2;
            return QRect(left, top, infoIconSize_, infoIconSize_);
        }

        bool 
        editorEvent(QEvent * event, QAbstractItemModel*, const QStyleOptionViewItem & option, const QModelIndex & index) override {
            if (event->type() == QEvent::MouseButtonRelease) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

                QRect rect = infoIconRect(option);
                if (rect.contains(mouseEvent->pos())) {
                    QXFontListWidget * window = qobject_cast<QXFontListWidget*>(option.widget->window());
                    if (window) {
                        QRect globalRect(option.widget->mapToGlobal(rect.topLeft()), option.widget->mapToGlobal(rect.bottomRight()));
                        window->showFontInfoPopover(index, globalRect);
                    }
                }
            }
            return false;
        }

        void
        paint(QPainter * painter,
              const QStyleOptionViewItem & option,
              const QModelIndex & proxyIndex) const override {
            painter->save();
            
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            const bool selected = (option.state & QStyle::State_Selected);
            if (selected)
                painter->fillRect(option.rect, option.palette.brush(QPalette::Active, QPalette::Highlight));
            else if (option.state & QStyle::State_MouseOver)
                painter->fillRect(option.rect, option.palette.alternateBase());
            
            const QXSortFilterFontListModel * proxyModel = qobject_cast<const QXSortFilterFontListModel*>(proxyIndex.model());
            const QXFontListModel * sourceModel = qobject_cast<const QXFontListModel *>(proxyModel->sourceModel());
            const QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);

            // Draw font type icon
            const int fontTypeIconSize = painter->fontInfo().pixelSize() * 1.2;
            QVariant iconVariant = proxyModel->data(proxyIndex, Qt::DecorationRole);
            QIcon icon;
            if (iconVariant.canConvert<QIcon>())
                icon = iconVariant.value<QIcon>();
            if (!icon.isNull()) {
                QPixmap pixmap = icon.pixmap(painter->fontInfo().pixelSize() * 2);
                painter->drawPixmap(QRect(option.rect.left(), option.rect.top(), fontTypeIconSize, fontTypeIconSize), pixmap);
            }
            
            int x = option.rect.left() + 5 + fontTypeIconSize;

            // Draw font name
            QVariant nameVariant = proxyModel->data(proxyIndex, Qt::DisplayRole);
            QString displayName;
            if (nameVariant.canConvert<QString>())
                displayName = nameVariant.value<QString>();
            if (selected)
                painter->setPen(option.palette.highlightedText().color());
            else
                painter->setPen(option.palette.text().color());
            if (!displayName.isEmpty())
                painter->drawText(QRect(x, option.rect.top(), option.rect.width(), option.rect.height()), displayName);

            const FXFaceDescriptor & desc = sourceModel->db()->faceDescriptor(sourceIndex.row());

            // Draw font path
            x += 8 + painter->fontMetrics().horizontalAdvance(displayName);;
            if (!selected)
                painter->setPen(option.palette.color(QPalette::Disabled, QPalette::Text));
            else 
                painter->setPen(option.palette.color(QPalette::HighlightedText));
            painter->drawText(QRect(x, option.rect.top(), option.rect.width() + 99999, option.rect.height()),
                              QString("(%1)").arg(toQString(desc.filePath)));

            // Draw sample text
            FXPtr<FXFace> face;
            if (faceCache_.has(desc))
                face = faceCache_.get(desc);
            else {
                face = FXFace::createFace(desc);
                faceCache_.put(desc, face);
            }
            
            if (face) {
                if (!face->isScalable())
                    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

                auto sample = previewText_.toStdU32String();

                const qreal sampleHeight = option.rect.height() - fontTypeIconSize;
                const qreal sampleFontSizePt = fontSize_;
                const qreal sampleFontSizePx = pt2px(sampleFontSizePt);
                const qreal sampleFontScale = sampleFontSizePt / face->fontSize();

                QPointF pen(option.rect.left(), option.rect.top()
                            + fontTypeIconSize
                            + (sampleHeight - sampleFontSizePx) / 2
                            + 0.75 * sampleFontSizePx);
                
                for (FXGChar c : sample) {
                    FXGlyph g = face->glyph(c);

                    FXVec2d<int> bmOffset;
                    double bmScale = face->bmScale();
                    FXGlyphImage gi = autoColorGlyphImage(face->glyphImage(g.gid), selected);
                    qreal scaledBmHeight = sampleFontScale * gi.pixmap.height * bmScale;
                    qreal scaledBmWidth = sampleFontScale * gi.pixmap.width * bmScale;
                    if (!face->isScalable()) {
                        if (scaledBmHeight >= 0.95 * sampleFontSizePx) {
                            qreal heightFittingScale = 0.95 * sampleFontSizePx / scaledBmHeight;
                            scaledBmWidth *= heightFittingScale;
                            scaledBmHeight = 0.95 * sampleFontSizePx;
                            bmScale *= heightFittingScale;
                        }

                        pen.setY(option.rect.top() + fontTypeIconSize + (sampleFontSizePx + scaledBmHeight) / 2);
                    }

                    if (!gi.pixmap.empty()) {
                        auto img = toQImage(gi);
                        img = img.scaledToWidth(scaledBmWidth * qApp->devicePixelRatio(),
                                                face->isScalable()? Qt::SmoothTransformation: Qt::FastTransformation);

                        const qreal left   = pen.x() + gi.offset.x * sampleFontScale;
                        const qreal bottom = pen.y() - gi.offset.y * sampleFontScale;
                        const qreal right  = left + gi.pixmap.width * bmScale * sampleFontScale;
                        const qreal top    = bottom - gi.pixmap.height * bmScale * sampleFontScale;

                        painter->drawImage(QRect(QPoint(left, top), QPoint(right, bottom)),
                                           img);
                    }
                    qreal advPx = pt2px(face->fontSize()) *  sampleFontScale * g.metrics.horiAdvance / face->upem();
                    if (advPx == 0 || face->upem() == 0)
                        advPx = scaledBmWidth;

                    pen += QPoint(advPx, 0);
                    if (pen.x() > option.rect.right())
                        break;
                }
                
            }

            // Draw info icon
            QXFontListWidget * window = qobject_cast<QXFontListWidget*>(option.widget->window());
            bool popoverVisible = false;
            if (window) popoverVisible = window->isFontInfoPopoverVisible();
            if ((selected && popoverVisible) || (option.state & QStyle::State_MouseOver)) {
                QRect iconRect = infoIconRect(option);
                int x1 = option.rect.right() + 3;
                int x0 = iconRect.left() - infoIconMargin_ * 5;
                QLinearGradient gradient(QPoint(x0, option.rect.top()), QPoint(option.rect.right(), option.rect.top())); // diagonal gradient from top-left to bottom-right

                QColor c0 = option.palette.base().color();
                if (selected)
                    c0 = option.palette.brush(QPalette::Active, QPalette::Highlight).color();
                QColor c1 = c0;
                c0.setAlpha(0);
                c1.setAlpha(255);
                gradient.setColorAt(0, c0);
                gradient.setColorAt(1, c1);

                painter->fillRect(QRect(QPoint(x0, option.rect.top()), QSize(x1 - x0, option.rect.height())), gradient);
                QIcon infoIcon = qApp->loadIcon(":/images/info.png");
                
                painter->drawPixmap(iconRect, infoIcon.pixmap(infoIconSize_, infoIconSize_));
            }
            painter->restore();
        }

        QSize
        sizeHint(const QStyleOptionViewItem & option,
                 const QModelIndex & index) const override {
            return QSize(10, pt2px(fontSize_) + 30);
        }
    };

    QMap<QString, QVariant>
    templateValues(const FXFaceDescriptor & desc, const FXFaceAttributes & atts) {
        QMap<QString, QVariant> map;

        map["FULL_NAME"] = QXDocument::faceDisplayName(atts, QXPreferences::fontDisplayLanguage());
        map["PS_NAME"] = toQString(atts.names.postscriptName());
        map["VENDOR"] = toQString(atts.names.vendor());
        map["VERSION"] = toQString(atts.names.version());
        map["FILE"] = toQString(desc.filePath);
        map["INDEX"] = quint32(desc.index);

        map["GLYPH_COUNT"] = quint32(atts.glyphCount);
        map["UPEM"] = quint32(atts.upem);
        map["FORMAT"] = toQString(atts.format);
        map["IS_CID"] = atts.isCID;
        map["CID"] = toQString(atts.cid);
        map["IS_OT_VARIANT"] = atts.isOpenTypeVariable;
        map["IS_MM"] = atts.isMultipleMaster;
        return map;
    }
   
}

QXFontListWidget::QXFontListWidget(QWidget * parent, Qt::WindowFlags flags)
    : QXThemedWindow<QWidget>(parent, flags)
    , ui_(new Ui::QXFontListWidget) {
    ui_->setupUi(this);
    ui_->menuButton->setIcon(qApp->loadIcon(":/images/menu.png"));

    // List view
    QSortFilterProxyModel * proxy = new QXSortFilterFontListModel(this);
    proxy->setSourceModel(new QXFontListModel(this));
    proxy->sort(0);
    ui_->fontListView->setModel(proxy);
    ui_->fontListView->setItemDelegate(new QXFontBrowserItemDelegate(this));
    ui_->fontListView->setMouseTracking(true);
    ui_->fontListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_->fontListView, &QListView::doubleClicked, this, &QXFontListWidget::onFontDoubleClicked, Qt::QueuedConnection);
    connect(ui_->fontListView, &QListView::customContextMenuRequested, this, &QXFontListWidget::onFontContextMenuRequested, Qt::QueuedConnection);

    QXFontManager::instance();
    QXDocumentWindowManager * mgr = QXDocumentWindowManager::instance();
    if (mgr->recentFonts().size()) {
        for (const auto & uri: mgr->recentFonts()) {
            if (-1 != selectFont(uri))
                break;
        }
    }
    if (-1 == selectedFontIndex())
        selectFont(0);
    
    // Search Edit
    ui_->searchLineEdit->setStyleSheet(
        QString("QLineEdit { "                                          \
                "  border: 2px solid; "                                 \
                "  border-radius: 14px; "                               \
                "  background-color: palette(base);}"                   \
                "QLineEdit:focus { "                                    \
                "  border: 2px solid; "                                 \
                "  border-radius: 14px; "                               \
                "  border-color: palette(highlight);}"));
    ui_->searchLineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui_->searchLineEdit->setMinimumHeight(28);
    ui_->searchLineEdit->setMinimumWidth(200);
    ui_->searchLineEdit->setPlaceholderText(tr("Search..."));
    ui_->searchLineEdit->setClearButtonEnabled(true);
    ui_->searchLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui_->searchLineEdit->installEventFilter(this);
    
    QAction * searchIconAction = new QAction(this);
    searchIconAction->setIcon(qApp->loadIcon(":/images/search.png"));
    ui_->searchLineEdit->addAction(searchIconAction, QLineEdit::LeadingPosition);
    connect(ui_->searchLineEdit, &QLineEdit::returnPressed, this, &QXFontListWidget::onSearchLineEditReturnPressed);
    connect(ui_->searchLineEdit, &QLineEdit::textEdited, this, &QXFontListWidget::onFilterChanged);

    // Menu button
    QMenu * menu = new QMenu(ui_->menuButton);
    ui_->menuButton->setMenu(menu);

    menu->addAction(qApp->loadIcon(":/images/preview.png"), tr("Preview Options"), [this]() {
            if (ui_->previewSettingsGoupBox->isVisible())
                ui_->previewSettingsGoupBox->hide();
            else
                ui_->previewSettingsGoupBox->show();
        });
    menu->addAction(qApp->loadIcon(":/images/open.png"), tr("Open Font File"), [this]() {
            onOpenFileButtonClicked();
        });
    menu->addSeparator();

    recentMenu_ = menu->addMenu(qApp->loadIcon(":/images/history.png"), tr("Recents"));
    connect(recentMenu_, &QMenu::aboutToShow, [this]() {
        QXDocumentWindowManager::instance()->aboutToShowRecentMenu(recentMenu_);
        foreach (QAction * action, recentMenu_->actions()) {
            if (!action->isSeparator() && !action->menu()) {
                connect(action, &QAction::triggered, [this, action]() {
                    QVariant data = action->data();
                    if (data.canConvert<QXFontURI>()) {
                        QXFontURI uri = data.value<QXFontURI>();
                        if (-1 == selectFont(uri)) {
                            QTimer::singleShot(0, [this, uri]() {
                                QXDocumentWindowManager::instance()->openFontURI(uri);
                                this->reject();                         
                            });
                        }
                    }
                });
            }
        }
    });

    // Preview settings
    ui_->previewFontSizeSlider->setMinimum(MIN_PREVIEW_FONT_SIZE);
    ui_->previewFontSizeSlider->setMaximum(MAX_PREVIEW_FONT_SIZE);
    ui_->previewFontSizeSlider->setValue(DEFAULT_PREVIEW_FONT_SIZE);
    connect(ui_->previewFontSizeSlider, &QSlider::valueChanged, this, &QXFontListWidget::updatePreviewText);
    connect(ui_->previewTextEdit, &QLineEdit::textEdited, this, &QXFontListWidget::onPreviewTextChanged);
    connect(ui_->previewCoverAllCharsCheckBox, &QCheckBox::stateChanged, this, &QXFontListWidget::onFilterChanged);
        
    ui_->previewSettingsGoupBox->hide();

    // Add actions
    QAction * searchAction = new QAction(this);
    connect(searchAction, &QAction::triggered, this, &QXFontListWidget::onSearchAction);
    searchAction->setShortcuts(QKeySequence::Find);
    addAction(searchAction);

    qApp->dismissSplashScreen(this);
    
    setWindowTitle(QString("%1 (bm %2s/%3s)")
        .arg(windowTitle())
        .arg(QXFontManager::instance().dbInitSeconds(), 0, 'f', 2)
        .arg(qApp->benchmark().time().count(), 0, 'f', 2));
    //setWindowTitle(QString("%1 (scaning fonts in %2s)").arg(windowTitle()).arg(qApp->benchmark().time().count()));
        
    // Drops
    this->setAcceptDrops(true);
    ui_->fontListView->setAcceptDrops(true);
    ui_->fontListView->installEventFilter(this);
}

QXFontListWidget::~QXFontListWidget() {
    delete ui_;
}

void
QXFontListWidget::accept() {
    hide();
    emit accepted();
}

void
QXFontListWidget::reject() {
    hide();
    emit rejected();
}

int
QXFontListWidget::selectedFontIndex() const {
    return currentSourceIndex().row();
}

QXFontURI
QXFontListWidget::selectedFont() const {
    int row = selectedFontIndex();
    if (row == -1)
        return QXFontURI{};
    
    auto desc = QXFontManager::instance().db()->faceDescriptor(row);
    auto atts = QXFontManager::instance().db()->faceAttributes(row);
    QXFontURI uri{toQString(desc.filePath), desc.index};
    return uri;
}

int
QXFontListWidget::selectFont(int index) {
    QModelIndex proxyIndex = proxyModel()->mapFromSource(sourceModel()->index(index));
    ui_->fontListView->setCurrentIndex(proxyIndex);
    scrollToCurrentIndex();
    return selectedFontIndex();
}

int
QXFontListWidget::selectFont(const QXFontURI & fontURI) {
    clearFilter();
    int index = sourceModel()->db()->faceIndex({toStdString(fontURI.filePath), fontURI.faceIndex});
    return selectFont(index);
}

void
QXFontListWidget::clearFilter() {
    proxyModel()->clearFilter();
    ui_->searchLineEdit->clear();
}

void
QXFontListWidget::showFontInfoPopover(const QModelIndex & index, const QRect & globalRect) {
    if (popover_ == nullptr) {
        popover_ = new QXPopoverWindow(this);
        popoverWidget_ = new QTextBrowser(this);
        popoverWidget_->setFixedWidth(250);
        popoverWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        popoverWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        popoverWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        popoverWidget_->setStyleSheet("background-color: palette(window);border: none;");
        popover_->setWidget(popoverWidget_);
    }
    
    int row = proxyModel()->mapToSource(index).row();
    auto & desc = QXFontManager::instance().db()->faceDescriptor(row);
    auto & atts = QXFontManager::instance().db()->faceAttributes(row);
    
    QXHtmlTemplate * html = QXHtmlTemplate::createFromFile(QXResources::path("Html/FontInfoTemplate.html"));
    popoverWidget_->setHtml(html->instantialize(templateValues(desc, atts)));
    html->deleteLater();
    
    qreal docHeight = popoverWidget_->document()->documentLayout()->documentSize().height();
    if (docHeight)
        popoverWidget_->setFixedHeight(docHeight + 2);
    
    popover_->showRelativeTo(globalRect, QXPopoverAnyEdge);
}

bool
QXFontListWidget::isFontInfoPopoverVisible() const {
    return popover_ && popover_->isVisible();
}

bool
QXFontListWidget::eventFilter(QObject * obj, QEvent * event) {
    if (obj == ui_->searchLineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent * keyEvent = (QKeyEvent*)event;

        if ((keyEvent->modifiers() & ~Qt::KeypadModifier) == Qt::NoModifier &&
            (keyEvent->key() == Qt::Key_Down ||
             keyEvent->key() == Qt::Key_PageDown ||
             keyEvent->key() == Qt::Key_Up ||
             keyEvent->key() == Qt::Key_PageUp ||
             keyEvent->key() == Qt::Key_Home ||
             keyEvent->key() == Qt::Key_End)) {

            QKeyEvent * eventCopy = new QKeyEvent(QEvent::KeyPress, keyEvent->key(), Qt::NoModifier);
            qApp->postEvent(ui_->fontListView, eventCopy);
        }
    }
    else if (event->type() == QEvent::DragEnter) {
         if (obj == ui_->fontListView || obj == ui_->searchLineEdit) {
             auto dragEnterEvent = ((QDragEnterEvent *)event);
             if (dragEnterEvent->mimeData()->hasUrls()) {
                 dragEnterEvent->acceptProposedAction();
                 return true;
             }
         }
    }
    else if (event->type() == QEvent::Drop) {
        if (obj == ui_->fontListView || obj == ui_->searchLineEdit) {
            if (QXDocumentWindowManager::instance()->handleDropEvent((QDropEvent*)event)) {
                event->accept();
                hide();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void
QXFontListWidget::closeEvent(QCloseEvent* event) {
    reject();
}

QXSortFilterFontListModel *
QXFontListWidget::proxyModel() const {
    return qobject_cast<QXSortFilterFontListModel *>(ui_->fontListView->model());
}

QXFontListModel *
QXFontListWidget::sourceModel() const {
    return qobject_cast<QXFontListModel *>(proxyModel()->sourceModel());
}

QModelIndex
QXFontListWidget::currentProxyIndex() const {
    return ui_->fontListView->currentIndex();
}

QModelIndex
QXFontListWidget::currentSourceIndex() const {
    return proxyModel()->mapToSource(currentProxyIndex());
}

QXFontListFilter
QXFontListWidget::fontListFilter() const {
    QXFontListFilter filter;
    filter.fontName = ui_->searchLineEdit->text().trimmed();
    filter.sampleText = ui_->previewTextEdit->text();
    filter.converAllSampleCharacters = ui_->previewCoverAllCharsCheckBox->checkState() == Qt::Checked;
    return filter;
}

void
QXFontListWidget::scrollToCurrentIndex() {
    ui_->fontListView->scrollTo(ui_->fontListView->currentIndex(), QAbstractItemView::PositionAtTop);    
}

void
QXFontListWidget::onFontDoubleClicked(const QModelIndex & index) {
    accept();
}

void
QXFontListWidget::onFontContextMenuRequested(const QPoint & pos) {
    auto index = currentSourceIndex();
    if (index.row() < 0)
        return;
    
    auto & desc = QXFontManager::instance().db()->faceDescriptor(index.row());
    
    QPoint globalPos = ui_->fontListView->mapToGlobal(pos);
    
    QMenu menu;
    menu.addAction(tr("Copy File Path"), [desc](){
        qApp->copyTextToClipBoard(toQString(desc.filePath));
    });
    
    menu.addAction(
#if defined(Q_OS_WIN)
                   tr("Show in Explorer"),
#elif defined(Q_OS_MACOS)
                   tr("Reveal in Finder"),
#else
                   tr("Show in Finder"),
#endif
                   [this, desc]() {
        qApp->showInGraphicalShell(this, toQString(desc.filePath));
    });
    
    menu.exec(globalPos);
}

void
QXFontListWidget::onSearchLineEditReturnPressed() {
}

void
QXFontListWidget::onFilterChanged() {
    auto filter = fontListFilter();
    proxyModel()->setFilter(fontListFilter());
    if (!filter.fontName.isEmpty() || selectedFontIndex() == -1)
        ui_->fontListView->setCurrentIndex(proxyModel()->index(0, 0));
    scrollToCurrentIndex();
}

void
QXFontListWidget::onSearchAction() {
    ui_->searchLineEdit->setFocus();
    ui_->searchLineEdit->selectAll();
}

void
QXFontListWidget::onOpenFileButtonClicked() {
    if (QXDocumentWindowManager::instance()->doNativeOpenFileDialog())
        reject();    
}

void
QXFontListWidget::onPreviewTextChanged() {
    updatePreviewText();
    
    if (ui_->previewCoverAllCharsCheckBox->checkState() == Qt::Checked)
        onFilterChanged();
}

void
QXFontListWidget::updatePreviewText() {
    QXFontBrowserItemDelegate * delegate = dynamic_cast<QXFontBrowserItemDelegate *>(ui_->fontListView->itemDelegate());
    if (delegate) {
        delegate->setFontSize(ui_->previewFontSizeSlider->value());
        delegate->setPreviewText(ui_->previewTextEdit->text());
        ui_->fontListView->model()->layoutChanged();
        scrollToCurrentIndex();
    }
}

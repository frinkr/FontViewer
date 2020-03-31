#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTimer>

#include "FontX/FXCache.h"
#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocumentWindowManager.h"
#include "QXFontBrowser.h"
#include "QXFontListModel.h"
#include "QXFontManager.h"
#include "QXHtmlTemplate.h"
#include "QXPopoverWindow.h"
#include "QXResource.h"
#include "ui_QXFontBrowser.h"

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
                    QXFontBrowser * window = qobject_cast<QXFontBrowser*>(option.widget->window());
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
            
            painter->setRenderHint(QPainter::HighQualityAntialiasing);
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
                    FXPixmapARGB bm = face->pixmap(g.gid, &bmOffset);
                    qreal scaledBmHeight = sampleFontScale * bm.height * bmScale;
                    qreal scaledBmWidth = sampleFontScale * bm.width * bmScale;
                    if (!face->isScalable()) {
                        if (scaledBmHeight >= 0.95 * sampleFontSizePx) {
                            qreal heightFittingScale = 0.95 * sampleFontSizePx / scaledBmHeight;
                            scaledBmWidth *= heightFittingScale;
                            scaledBmHeight = 0.95 * sampleFontSizePx;
                            bmScale *= heightFittingScale;
                        }

                        pen.setY(option.rect.top() + fontTypeIconSize + (sampleFontSizePx + scaledBmHeight) / 2);
                    }

                    if (!bm.empty()) {
                        auto img = toQImage(bm);
                        img = img.scaledToWidth(scaledBmWidth * qApp->devicePixelRatio(),
                                                face->isScalable()? Qt::SmoothTransformation: Qt::FastTransformation);
                        if (face->isScalable() && (selected || qApp->darkMode()))
                            img.invertPixels();

                        const qreal left   = pen.x() + bmOffset.x * sampleFontScale;
                        const qreal bottom = pen.y() - bmOffset.y * sampleFontScale;
                        const qreal right  = left + bm.width * bmScale * sampleFontScale;
                        const qreal top    = bottom - bm.height * bmScale * sampleFontScale;

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
            QXFontBrowser * window = qobject_cast<QXFontBrowser*>(option.widget->window());
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
            
            faceCache_.gc();
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

        map["FULL_NAME"] = QXDocument::faceDisplayName(atts);
        map["PS_NAME"] = toQString(atts.sfntNames.postscriptName());
        map["VENDOR"] = toQString(atts.sfntNames.vendor());
        map["VERSION"] = toQString(atts.sfntNames.version());
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

QXFontBrowser::QXFontBrowser(QWidget * parent)
    : QXThemedWindow<QDialog>(parent)
    , ui_(new Ui::QXFontBrowser) {
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
    connect(ui_->fontListView, &QListView::doubleClicked, this, &QXFontBrowser::onFontDoubleClicked, Qt::QueuedConnection);
    connect(ui_->fontListView, &QListView::customContextMenuRequested, this, &QXFontBrowser::onFontContextMenuRequested, Qt::QueuedConnection);

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
    connect(ui_->searchLineEdit, &QLineEdit::returnPressed, this, &QXFontBrowser::onSearchLineEditReturnPressed);
    connect(ui_->searchLineEdit, &QLineEdit::textEdited, this, &QXFontBrowser::onFilterChanged);

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
    ui_->previewTextEdit->setText(DEFAULT_PRVIEW_TEXT);
    ui_->previewFontSizeSlider->setMinimum(MIN_PREVIEW_FONT_SIZE);
    ui_->previewFontSizeSlider->setMaximum(MAX_PREVIEW_FONT_SIZE);
    ui_->previewFontSizeSlider->setValue(DEFAULT_PREVIEW_FONT_SIZE);
    connect(ui_->previewFontSizeSlider, &QSlider::valueChanged, this, &QXFontBrowser::updatePreviewText);
    connect(ui_->previewTextEdit, &QLineEdit::textEdited, this, &QXFontBrowser::onPreviewTextChanged);
    connect(ui_->previewCoverAllCharsCheckBox, &QCheckBox::stateChanged, this, &QXFontBrowser::onFilterChanged);
        
    ui_->previewSettingsGoupBox->hide();

    // Add actions
    QAction * searchAction = new QAction(this);
    connect(searchAction, &QAction::triggered, this, &QXFontBrowser::onSearchAction);
    searchAction->setShortcuts(QKeySequence::Find);
    addAction(searchAction);

    QAction * closeAction = new QAction(this);
    connect(closeAction, &QAction::triggered, this, &QDialog::close);
    closeAction->setShortcuts(QKeySequence::Close);
    addAction(closeAction);

    QAction * quitAction = new QAction(this);
    connect(quitAction, &QAction::triggered, this, &QXFontBrowser::quitApplication);
    quitAction->setShortcuts(QKeySequence::Quit);
    addAction(quitAction);

    qApp->dismissSplashScreen(this);
}

QXFontBrowser::~QXFontBrowser() {
    delete ui_;
}

int
QXFontBrowser::selectedFontIndex() const {
    return currentSourceIndex().row();
}

QXFontURI
QXFontBrowser::selectedFont() const {
    int row = selectedFontIndex();
    if (row == -1)
        return QXFontURI{};
    
    auto desc = QXFontManager::instance().db()->faceDescriptor(row);
    auto atts = QXFontManager::instance().db()->faceAttributes(row);
    atts.sfntNames.familyName();
    QXFontURI uri{toQString(desc.filePath), desc.index};
    return uri;
}

int
QXFontBrowser::selectFont(int index) {
    QModelIndex proxyIndex = proxyModel()->mapFromSource(sourceModel()->index(index));
    ui_->fontListView->setCurrentIndex(proxyIndex);
    scrollToCurrentIndex();
    return selectedFontIndex();
}

int
QXFontBrowser::selectFont(const QXFontURI & fontURI) {
    clearFilter();
    int index = sourceModel()->db()->faceIndex({toStdString(fontURI.filePath), fontURI.faceIndex});
    return selectFont(index);
}

void
QXFontBrowser::clearFilter() {
    proxyModel()->clearFilter();
    ui_->searchLineEdit->clear();
}

void
QXFontBrowser::showFontInfoPopover(const QModelIndex & index, const QRect & globalRect) {
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
    
    QXHtmlTemplate * html = QXHtmlTemplate::createFromFile(QXResource::path("/Html/FontInfoTemplate.html"));
    popoverWidget_->setHtml(html->instantialize(templateValues(desc, atts)));
    html->deleteLater();
    
    qreal docHeight = popoverWidget_->document()->documentLayout()->documentSize().height();
    if (docHeight)
        popoverWidget_->setFixedHeight(docHeight + 2);
    
    popover_->showRelativeTo(globalRect, QXPopoverAnyEdge);
}

bool
QXFontBrowser::isFontInfoPopoverVisible() const {
    return popover_ && popover_->isVisible();
}

bool
QXFontBrowser::eventFilter(QObject * obj, QEvent * event) {
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
        else if (keyEvent == QKeySequence::Quit){
            quitApplication();
            return true;
        }
    }

    return QDialog::eventFilter(obj, event);
}

QXSortFilterFontListModel *
QXFontBrowser::proxyModel() const {
    return qobject_cast<QXSortFilterFontListModel *>(ui_->fontListView->model());
}

QXFontListModel *
QXFontBrowser::sourceModel() const {
    return qobject_cast<QXFontListModel *>(proxyModel()->sourceModel());
}

QModelIndex
QXFontBrowser::currentProxyIndex() const {
    return ui_->fontListView->currentIndex();
}

QModelIndex
QXFontBrowser::currentSourceIndex() const {
    return proxyModel()->mapToSource(currentProxyIndex());
}

QXFontListFilter
QXFontBrowser::fontListFilter() const {
    QXFontListFilter filter;
    filter.fontName = ui_->searchLineEdit->text().trimmed();
    filter.sampleText = ui_->previewTextEdit->text();
    filter.converAllSampleCharacters = ui_->previewCoverAllCharsCheckBox->checkState() == Qt::Checked;
    return filter;
}

void
QXFontBrowser::scrollToCurrentIndex() {
    ui_->fontListView->scrollTo(ui_->fontListView->currentIndex(), QAbstractItemView::PositionAtTop);    
}

void
QXFontBrowser::onFontDoubleClicked(const QModelIndex & index) {
    accept();
}

void
QXFontBrowser::onFontContextMenuRequested(const QPoint & pos) {
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
QXFontBrowser::onSearchLineEditReturnPressed() {
}

void
QXFontBrowser::onFilterChanged() {
    auto filter = fontListFilter();
    proxyModel()->setFilter(fontListFilter());
    if (!filter.fontName.isEmpty() || selectedFontIndex() == -1)
        ui_->fontListView->setCurrentIndex(proxyModel()->index(0, 0));
    scrollToCurrentIndex();
}

void
QXFontBrowser::onSearchAction() {
    ui_->searchLineEdit->setFocus();
    ui_->searchLineEdit->selectAll();
}

void
QXFontBrowser::onOpenFileButtonClicked() {
    if (QXDocumentWindowManager::instance()->doOpenFontFromFile())
        reject();    
}

void
QXFontBrowser::onPreviewTextChanged() {
    updatePreviewText();
    
    if (ui_->previewCoverAllCharsCheckBox->checkState() == Qt::Checked)
        onFilterChanged();
}

void
QXFontBrowser::updatePreviewText() {
    QXFontBrowserItemDelegate * delegate = dynamic_cast<QXFontBrowserItemDelegate *>(ui_->fontListView->itemDelegate());
    if (delegate) {
        delegate->setFontSize(ui_->previewFontSizeSlider->value());
        delegate->setPreviewText(ui_->previewTextEdit->text());
        ui_->fontListView->model()->layoutChanged();
        scrollToCurrentIndex();
    }
}

void
QXFontBrowser::quitApplication() {
    this->close();
    QXDocumentWindowManager::instance()->closeAllDocumentsAndQuit();    
}

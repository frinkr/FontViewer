#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTimer>
#include "FontX/FXCache.h"
#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocumentWindowManager.h"
#include "QXFontBrowser.h"
#include "QXFontManager.h"
#include "QXFontComboBox.h"
#include "QXTheme.h"
#include "ui_QXFontBrowser.h"

namespace {
    constexpr qreal MIN_PREVIEW_FONT_SIZE = 20;
    constexpr qreal MAX_PREVIEW_FONT_SIZE = 200;
    constexpr qreal DEFAULT_PREVIEW_FONT_SIZE = 50;
    const QString DEFAULT_PRVIEW_TEXT = "The quick fox jumps over the lazy dog";

    FXCache<FXFaceDescriptor, FXPtr<FXFace>> faceCache_(50); // cache 50 faces
    
    class QXFontBrowserItemDelegate : public QStyledItemDelegate {
    private:
        qreal    fontSize_ {DEFAULT_PREVIEW_FONT_SIZE};
        QString  previewText_;
    public:
        QXFontBrowserItemDelegate(QWidget * parent = 0)
            : QStyledItemDelegate(parent)
            , previewText_(DEFAULT_PRVIEW_TEXT){
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

        void
        paint(QPainter * painter,
              const QStyleOptionViewItem & option,
              const QModelIndex & proxyIndex) const override {
            painter->save();
            
            painter->setRenderHint(QPainter::HighQualityAntialiasing);

            const bool selected = (option.state & QStyle::State_Selected);
            const bool active = (option.state & QStyle::State_Active);
            if (selected)
                painter->fillRect(option.rect, option.palette.brush(QPalette::Active, QPalette::Highlight));
            else
                painter->fillRect(option.rect, proxyIndex.row() % 2? option.palette.base(): option.palette.alternateBase());
            
            const QXSortFilterFontListModel * proxyModel = qobject_cast<const QXSortFilterFontListModel*>(proxyIndex.model());
            const QXFontListModel * sourceModel = qobject_cast<const QXFontListModel *>(proxyModel->sourceModel());
            const QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);

            // Draw font type icon
            const int iconSize = painter->fontInfo().pixelSize() * 1.2;
            QVariant iconVariant = proxyModel->data(proxyIndex, Qt::DecorationRole);
            QIcon icon;
            if (iconVariant.canConvert<QIcon>())
                icon = iconVariant.value<QIcon>();
            if (!icon.isNull()) {
                QPixmap pixmap = icon.pixmap(painter->fontInfo().pixelSize() * 2);
                painter->drawPixmap(QRect(option.rect.left(), option.rect.top(), iconSize, iconSize), pixmap);
            }
            
            // Draw font name
            QVariant nameVariant = proxyModel->data(proxyIndex, Qt::DisplayRole);
            QString displayName;
            if (nameVariant.canConvert<QString>())
                displayName = nameVariant.value<QString>();
            if (!displayName.isEmpty())
                painter->drawText(option.rect.adjusted(iconSize + 2, 0, 0, 0), displayName);

            const FXFaceDescriptor & desc = sourceModel->db()->faceDescriptor(sourceIndex.row());

            // Draw font path
            int left = option.rect.left() + iconSize + 8 + painter->fontMetrics().horizontalAdvance(displayName);
            if (!selected)
                painter->setPen(option.palette.text().color().darker());
            painter->drawText(QRect(left, option.rect.top(), option.rect.right() + 99999, option.rect.bottom()),
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
                if (face->isScalable())
                    painter->setRenderHint(QPainter::SmoothPixmapTransform);

                auto sample = previewText_.toStdU32String();

                const qreal sampleHeight = option.rect.height() - iconSize;
                const qreal sampleFontSizePt = fontSize_;
                const qreal sampleFontSizePx = pt2px(sampleFontSizePt);
                const qreal sampleFontScale = sampleFontSizePt / face->fontSize();

                QPointF pen(option.rect.left(), option.rect.top()
                            + iconSize
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

                        pen.setY(option.rect.top() + iconSize + (sampleFontSizePx + scaledBmHeight) / 2);
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
            painter->restore();
            
            faceCache_.gc();
        }

        QSize
        sizeHint(const QStyleOptionViewItem & option,
                 const QModelIndex & index) const override {
            return QSize(10, pt2px(fontSize_) + 30);
        }
    };
}

QXFontBrowser::QXFontBrowser(QWidget * parent)
    : QXThemedWindow<QDialog>(parent)
    , ui_(new Ui::QXFontBrowser) {
    ui_->setupUi(this);
    ui_->openFileButton->setIcon(qApp->loadIcon(":/images/open-font.png"));
	ui_->filterButton->setIcon(qApp->loadIcon(":/images/filter.png"));
    ui_->recentButton->setIcon(qApp->loadIcon(":/images/history.png"));

    // List view
    QSortFilterProxyModel * proxy = new QXSortFilterFontListModel(this);
    proxy->setSourceModel(new QXFontListModel(this));
    ui_->fontListView->setModel(proxy);
    ui_->fontListView->setItemDelegate(new QXFontBrowserItemDelegate(this));
    proxy->sort(0);
    connect(ui_->fontListView, &QListView::doubleClicked, this, &QXFontBrowser::onFontDoubleClicked);

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
        QString("QLineEdit {border-radius: 13px;}"                      \
                "QLineEdit:focus { "                                    \
                "  border:2px solid; "                                  \
                "  border-radius: 13px; "                               \
                "  border-color:palette(highlight);}"));
    ui_->searchLineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui_->searchLineEdit->setMinimumHeight(26);
    ui_->searchLineEdit->setMinimumWidth(200);
    ui_->searchLineEdit->setPlaceholderText(tr("Search..."));
    ui_->searchLineEdit->setClearButtonEnabled(true);
    ui_->searchLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QAction * searchIconAction = new QAction(this);
    searchIconAction->setIcon(qApp->loadIcon(":/images/search.png"));
    ui_->searchLineEdit->addAction(searchIconAction, QLineEdit::LeadingPosition);
    connect(ui_->searchLineEdit, &QLineEdit::returnPressed, this, &QXFontBrowser::onSearchLineEditReturnPressed);
    connect(ui_->searchLineEdit, &QLineEdit::textEdited, this, &QXFontBrowser::onSearchLineEditTextEdited);

    // Open File button
    connect(ui_->openFileButton, &QPushButton::clicked, this, &QXFontBrowser::onOpenFileButtonClicked);

    // Recent button
    recentMenu_ = new QMenu(ui_->recentButton);
    ui_->recentButton->setMenu(recentMenu_);
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
    connect(ui_->filterButton, &QPushButton::clicked, [this]() {
            if (ui_->previewSettingsGoupBox->isVisible())
                ui_->previewSettingsGoupBox->hide();
            else
                ui_->previewSettingsGoupBox->show();
        });
    connect(ui_->previewFontSizeSlider, &QSlider::valueChanged, this, &QXFontBrowser::updatePreviewSettings);
    connect(ui_->previewTextEdit, &QLineEdit::textEdited, this, &QXFontBrowser::updatePreviewSettings);
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
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    quitAction->setShortcuts(QKeySequence::Quit);
    addAction(quitAction);
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
    atts.names.familyName();
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
QXFontBrowser::accept() {
    faceCache_.clear();
    QDialog::accept();
}

void
QXFontBrowser::reject() {
    faceCache_.clear();
    QDialog::reject();
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

void
QXFontBrowser::scrollToCurrentIndex() {
    ui_->fontListView->scrollTo(ui_->fontListView->currentIndex(), QAbstractItemView::PositionAtTop);    
}

void
QXFontBrowser::onFontDoubleClicked(const QModelIndex & index) {
    accept();
}

void
QXFontBrowser::onSearchLineEditReturnPressed() {
}

void
QXFontBrowser::onSearchLineEditTextEdited(const QString & text) {
    proxyModel()->setFilter(text);
    if (!text.isEmpty() || selectedFontIndex() == -1)
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
QXFontBrowser::updatePreviewSettings() {
    QXFontBrowserItemDelegate * delegate = dynamic_cast<QXFontBrowserItemDelegate *>(ui_->fontListView->itemDelegate());
    if (delegate) {
        delegate->setFontSize(ui_->previewFontSizeSlider->value());
        delegate->setPreviewText(ui_->previewTextEdit->text());
        ui_->fontListView->model()->layoutChanged();
        scrollToCurrentIndex();
    }
}

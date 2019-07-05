#include <QActionGroup>
#include <QClipboard>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>
#include <QToolButton>
#include <QWidgetAction>

#include "QXApplication.h"
#include "QXCMapBlockWidget.h"
#include "QXConv.h"
#include "QXDockTitleBarWidget.h"
#include "QXDocumentWindow.h"
#include "QXDocumentWindowManager.h"
#include "QXEncoding.h"
#include "QXFontInfoWidget.h"
#include "QXGlyphInfoWidget.h"
#include "QXGlyphTableWidget.h"
#include "QXMenuBar.h"
#include "QXPopoverWindow.h"
#include "QXSearchEngine.h"
#include "QXShapingWidget.h"
#include "QXTheme.h"
#include "QXVariableWidget.h"
#include "QXToastMessage.h"

#if defined(Q_OS_MAC)
#  include "MacHelper.h"
#endif

#include "ui_QXDocumentWindow.h"

QXDocumentWindow::QXDocumentWindow(QXDocument * document, QWidget *parent) 
    : QXThemedWindow<QMainWindow>(parent)
    , ui_(new Ui::QXDocumentWindow)
    , cmapBlockPopover_(nullptr)
    , variablePopover_(nullptr)
    , shapingDockWidget_(nullptr)
    , tableDockWidget_(nullptr)
    , infoDockWidget_(nullptr)
    , glyphPopover_(nullptr)
    , glyphWidget_(nullptr)
    , document_(document)
{
    ui_->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    initUI();

    document->setParent(this);
    ui_->glyphCollectionView->widget()->installEventFilter(this);
    
}

QXDocumentWindow::~QXDocumentWindow() {
    delete ui_;
}

void 
QXDocumentWindow::initUI() {
    initWindowTitle();
    initMenu();
    initToolBar();
    initCollectionView();
    initGlyphInfoView();

    connectSingals();
}

void
QXDocumentWindow::initWindowTitle() {
    const QString & filePath = document_->uri().filePath;
    setWindowFilePath(filePath);
    setWindowTitle(document_->displayName());

#if defined (Q_OS_MAC)
    QFileInfo fi(filePath);
    setWindowIcon(QFileIconProvider().icon(fi));
#endif
}

void
QXDocumentWindow::initMenu() {
    menuBar_ = new QXMenuBar(this);

    connect(menuBar_->actionCopy, &QAction::triggered,
            this, &QXDocumentWindow::onCopyAction);

    connect(menuBar_->actionSearch, &QAction::triggered, 
        this, &QXDocumentWindow::onSearchAction);

    connect(menuBar_->actionToolBar, &QAction::toggled, [this](bool checked) {
        this->ui_->toolBar->setVisible(checked);
    });

    connect(menuBar_->actionStatusBar, &QAction::toggled, [this](bool checked) {
        this->ui_->statusBar->setVisible(checked);
    });
    menuBar_->actionStatusBar->setChecked(false);

    connect(menuBar_, &QXMenuBar::glyphLabelActionTriggered, [this](QAction * action, QXGlyphLabel label) {
        document_->setGlyphLabel(label);
    });

    connect(menuBar_->actionShowAllGlyphs, &QAction::toggled, [this](bool checked) {
        document_->setCharMode(!checked);
    });

    connect(document_, &QXDocument::charModeChanged, [this](bool activated) {
        menuBar_->actionShowAllGlyphs->setChecked(!activated);
    });

    connect(menuBar_->actionFullScreen, &QAction::toggled, [this](bool checked) {
        if (checked)
            showFullScreen();
        else
            showNormal();
    });

    setMenuBar(menuBar_);

#if !defined(Q_OS_MAC)
    QXTheme::current()->applyToMenuBar(menuBar_);
#endif
}

void
QXDocumentWindow::initToolBar() {
    QToolBar * toolBar = ui_->toolBar;

    openFontAction_ = toolBar->addAction(
        qApp->loadIcon(":/images/open-font.png"),tr("Open Font"),
        this, &QXDocumentWindow::onOpenFontAction);

    cmapBlockAction_ = toolBar->addAction(
        qApp->loadIcon(":/images/cmap.png"),tr("CMap"),
        this, &QXDocumentWindow::onCMapBlockAction);

    variableAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/variant.png"), tr("OpenType Variable / Multiple Master"),
        this, &QXDocumentWindow::onVariableAction);

    shapingAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/shape.png"), tr("Shape"),
        this, &QXDocumentWindow::onShapingAction);
    
    tableAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/table.png"), tr("Table"),
        this, &QXDocumentWindow::onTableAction);
    
    infoAction_ = toolBar->addAction(
		qApp->loadIcon(":/images/info.png"), tr("Info"),
        this, &QXDocumentWindow::onFontInfoAction);

    QWidget * spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);

    initSearchField();

    variableAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    shapingAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    tableAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    infoAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

#if defined(Q_OS_MAC)
    this->setUnifiedTitleAndToolBarOnMac(true);
    QXTheme::current()->applyToToolBar(toolBar);
#else
    QXTheme::current()->applyToToolBar(toolBar);
#endif
}

void
QXDocumentWindow::initSearchField() {
    QMenu * searchHistoryMenu = new QMenu(this);
    searchHistoryMenu->addAction("U+FB01");
    searchHistoryMenu->addAction("fi");

    searchLineEdit_ = new QLineEdit(this);
    // Round box
    searchLineEdit_->setStyleSheet(
        QString("QLineEdit {border-radius: 15px;}"                      \
                "QLineEdit:focus { "                                    \
                "  border:2px solid; "                                  \
                "  border-radius: 15px; "                               \
                "  border-color:palette(highlight);}"));
    searchLineEdit_->setAttribute(Qt::WA_MacShowFocusRect, 0);

    searchLineEdit_->setMinimumHeight(30);
    searchLineEdit_->setMinimumWidth(200);
    searchLineEdit_->setPlaceholderText(tr("Search..."));
    searchLineEdit_->setClearButtonEnabled(true);
    searchLineEdit_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QAction * searchIconAction = new QAction(this);
    searchIconAction->setIcon(qApp->loadIcon(":/images/search.png"));
    searchIconAction->setMenu(searchHistoryMenu);
    searchLineEdit_->addAction(searchIconAction, QLineEdit::LeadingPosition);
    ui_->toolBar->addWidget(searchLineEdit_);
    
    connect(searchLineEdit_, &QLineEdit::returnPressed, this, &QXDocumentWindow::onSearchLineEditReturnPressed);
    
    // Right Margin
    QWidget * spacer = new QWidget(this);
    spacer->setFixedWidth(3);
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    ui_->toolBar->addWidget(spacer);
}

void
QXDocumentWindow::initCollectionView() {
    ui_->glyphCollectionView->setDocument(document_);
    ui_->glyphCollectionView->setAcceptDrops(true);

    connect(ui_->glyphCollectionView, &QXCollectionView::clicked, 
        this, &QXDocumentWindow::onGlyphClicked);
    connect(ui_->glyphCollectionView, &QXCollectionView::doubleClicked,
        this, &QXDocumentWindow::onGlyphDoubleClicked);
}

void
QXDocumentWindow::initGlyphInfoView() {
    glyphPopover_ = new QXPopoverWindow(this);
    glyphPopover_->setBorderRadius(0);
    glyphWidget_  = new QXGlyphInfoWidget(glyphPopover_);
    glyphWidget_->setMinimumSize(300, 400);
    glyphPopover_->setWidget(glyphWidget_);
    glyphWidget_->setDocument(document_);

    connect(glyphWidget_, &QXGlyphInfoWidget::charLinkClicked,
        this, &QXDocumentWindow::onCharLinkClicked);
}
    
void
QXDocumentWindow::connectSingals() {
    connect(document_, &QXDocument::searchDone,
            this, &QXDocumentWindow::onSearchResult);
}

QToolButton *
QXDocumentWindow::senderToolButton() {
    QAction * action = qobject_cast<QAction*>(sender());
    if (action) 
        return qobject_cast<QToolButton*>(ui_->toolBar->widgetForAction(action));
    
    return qobject_cast<QToolButton *>(sender());
}

void
QXDocumentWindow::closeEvent(QCloseEvent * event) {
    emit aboutToClose(this);
    QMainWindow::closeEvent(event);
}

void
QXDocumentWindow::dropEvent(QDropEvent * event) {
    for (const QUrl & url: event->mimeData()->urls()) {
        event->acceptProposedAction();
        QString filePath = url.toLocalFile();
        QXDocumentWindowManager::instance()->openFontFile(filePath);
        return;
    }
    QMainWindow::dropEvent(event);
}

bool
QXDocumentWindow::eventFilter(QObject * watched, QEvent * event) {
    if (watched == ui_->glyphCollectionView->widget() || watched == menuBar_) {
        if (event->type() == QEvent::Drop) {
            dropEvent(static_cast<QDropEvent *>(event));
            event->accept();
            return true;
        }
        else if (event->type() == QEvent::DragEnter) {
            auto ev = static_cast<QDragEnterEvent *>(event);
            if (ev->mimeData()->hasUrls())
                ev->acceptProposedAction();
            return true;
        }
    }
    return false;
}

void
QXDocumentWindow::showGlyphPopover(const FXGChar & c, const QRect & rect, QXPopoverEdges preferedEgdes) {
    glyphWidget_->setChar(c);
    glyphPopover_->showRelativeTo(rect, preferedEgdes);
}

void
QXDocumentWindow::onGlyphClicked(const QXCollectionModelIndex & index) {
    // Show info on status bar
    if (index.item == -1) {
        // Nothing selected, show file path
        QString message;
        if (document_->face()->faceCount() > 1)
            message = QString("%1: %2").arg(document_->uri().filePath).arg(document_->uri().faceIndex);
        else
            message = document_->uri().filePath;
        ui_->statusBar->showMessage(message);
    }
    else {
        FXGChar c = document_->charAt(index);
        FXGlyph g = document_->face()->glyph(c);
        QString message = QString(tr("%1         %2         GID %3         W %4         H %5         HAdv %6         VAdv %7"))
            .arg(toQString(g.name))
            .arg(QXEncoding::charHexNotation(c))
            .arg(g.gid)
            .arg(g.metrics.width)
            .arg(g.metrics.height)
            .arg(g.metrics.horiAdvance)
            .arg(g.metrics.vertAdvance);
        ui_->statusBar->showMessage(message);
    }
}

void
QXDocumentWindow::onGlyphDoubleClicked(const QXCollectionModelIndex & index) {
    if (index.item == -1 || index.section == -1)
        return;
    
    QRect rect = ui_->glyphCollectionView->itemRect(index);
    QRect globalRect(ui_->glyphCollectionView->mapToGlobal(rect.topLeft()),
                     ui_->glyphCollectionView->mapToGlobal(rect.bottomRight()));

    showGlyphPopover(document_->charAt(index), globalRect, QXPopoverBottom);
}

void
QXDocumentWindow::onCharLinkClicked(FXGChar c) {
    //ui_->listView->selectChar(c);
}

void
QXDocumentWindow::onCopyAction() {
    if (!ui_->glyphCollectionView->hasFocus()) {
        if (QWidget * focus = qApp->focusWidget())
            QMetaObject::invokeMethod(focus, "copy", Qt::QueuedConnection);
        return;
    }
    
    FXGChar c = document_->charAt(ui_->glyphCollectionView->selectedIndex());
    if (c.isUnicode()) {
        QString text = QString::fromUcs4(static_cast<uint*>(&c.value), 1);
        qApp->clipboard()->setText(text);
        static_assert(sizeof(uint) == sizeof(FXChar), "");

        QXToastMessage * message = new QXToastMessage(this);
        message->showToParent(style()->standardIcon(QStyle::SP_MessageBoxInformation),
                              QString(R"("%1" has been copied to clipboard)").arg(text));
    }
}

void
QXDocumentWindow::onOpenFontAction() {
    QXDocumentWindowManager::instance()->doOpenFontDialog();
}

void
QXDocumentWindow::onCMapBlockAction() {
    if (!cmapBlockPopover_) {
        cmapBlockPopover_ = new QXPopoverWindow(this);
        QXCMapBlockWidget * widget = new QXCMapBlockWidget;
        widget->setDocument(document_);
        cmapBlockPopover_->setWidget(widget);
    }
    cmapBlockPopover_->showRelativeTo(senderToolButton(), QXPopoverBottom);
}

void
QXDocumentWindow::onVariableAction() {
    if (!variablePopover_) {
        variablePopover_ = new QXPopoverWindow(this);
        QXVariableWidget * widget = new QXVariableWidget;
        widget->setDocument(document_);
        variablePopover_->setWidget(widget);
    }
    variablePopover_->showRelativeTo(senderToolButton(), QXPopoverBottom);
}

void
QXDocumentWindow::onTableAction() {
    if (!tableDockWidget_) {
        tableDockWidget_ = new QDockWidget(tr("Glyphs Table"), this);
        tableDockWidget_->setWidget(new QxGlyphTableWidget(document_));
        tableDockWidget_->setTitleBarWidget(new QXDockTitleBarWidget(tableDockWidget_));
        addDockWidget(Qt::BottomDockWidgetArea, tableDockWidget_);
    }
    toggleDockWidget(tableDockWidget_);
}

void
QXDocumentWindow::onShapingAction() {
    if (!shapingDockWidget_) {
        shapingDockWidget_ = new QDockWidget(tr("Shape"), this);
        QXShapingWidget * widget = new QXShapingWidget(this);
        widget->setDocument(document_);
        shapingDockWidget_->setWidget(widget);
        shapingDockWidget_->setTitleBarWidget(new QXDockTitleBarWidget(shapingDockWidget_));
        addDockWidget(Qt::BottomDockWidgetArea, shapingDockWidget_);    
    }
    toggleDockWidget(shapingDockWidget_);
    if (shapingDockWidget_->isVisible())
        ((QXShapingWidget*)(shapingDockWidget_->widget()))->focusLineEdit();
}

void
QXDocumentWindow::onFontInfoAction() {
    if (!infoDockWidget_) {
        infoDockWidget_ = new QDockWidget(tr("Info"), this);
        infoDockWidget_->setWidget(new QXFontInfoWidget(document_->face(), infoDockWidget_));
        infoDockWidget_->setTitleBarWidget(new QXDockTitleBarWidget(infoDockWidget_));
        addDockWidget(Qt::LeftDockWidgetArea, infoDockWidget_);
    }

    toggleDockWidget(infoDockWidget_);
}

void
QXDocumentWindow::onSearchAction() {
    searchLineEdit_->setFocus();
    searchLineEdit_->selectAll();
}

void
QXDocumentWindow::onSearchLineEditReturnPressed() {
    document_->search(searchLineEdit_->text());
}

void
QXDocumentWindow::onSearchResult(const QXSearchResult & result, const QString & text) {
    if (!result.found) {
        qApp->warning(this,
                      tr("Failed to find glyph"),
                      tr(R"(Expression "%1" doesn't match any glypyh!)").arg(text));
        return;
    }

    document_->setCharMode(document_->books()[result.book].type() != QXGCharBook::GlyphList);
    document_->selectBook(result.book);
    QXCollectionModelIndex modelIndex{ result.block, result.index };
    ui_->glyphCollectionView->select(modelIndex);
    ui_->glyphCollectionView->scrollTo(modelIndex);
    ui_->glyphCollectionView->setFocus();
}

void
QXDocumentWindow::toggleDockWidget(QDockWidget * dockWidget) {
    if (dockWidget->isVisible())
        dockWidget->hide();
    else {
        dockWidget->show();
        dockWidget->raise();
    }
}
    

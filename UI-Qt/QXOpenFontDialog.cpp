#include <QFileDialog>
#include <QMenu>
#include <QSettings>
#include <QTimer>

#include "QXApplication.h"
#include "QXConv.h"
#include "QXDocumentWindowManager.h"
#include "QXHtmlTemplate.h"
#include "QXFontManager.h"
#include "QXResource.h"
#include "QXOpenFontDialog.h"

#if defined(Q_OS_MAC)
#  include "MacHelper.h"
#endif

#include "ui_QXOpenFontDialog.h"

namespace {
    QMap<QString, QVariant>
    templateValues(const FXFaceDescriptor & desc, const FXFaceAttributes & atts) {
        QMap<QString, QVariant> map;

        map["FULL_NAME"]   = QXDocument::faceDisplayName(atts);
        map["PS_NAME"]     = toQString(atts.names.postscriptName());
        map["VENDOR"]      = toQString(atts.names.vendor());
        map["VERSION"]     = toQString(atts.names.version());
        map["FILE"]        = toQString(desc.filePath);
        map["INDEX"]       = quint32(desc.index);
        
        map["GLYPH_COUNT"] = quint32(atts.glyphCount);
        map["UPEM"]        = quint32(atts.upem);
        map["FORMAT"]      = toQString(atts.format);
        map["IS_CID"]      = atts.isCID;
        map["CID"]         = toQString(atts.cid);
        map["IS_OT_VARIANT"] = atts.isOpenTypeVariable;
        map["IS_MM"]       = atts.isMultipleMaster;
        return map;
    }
}

QXOpenFontDialog::QXOpenFontDialog(QWidget *parent)
    : QXThemedWindow<QDialog>(parent)
    , ui_(new Ui::QXOpenFontDialog)
    , recentMenu_(nullptr) {
    ui_->setupUi(this);
    ui_->openButton->setIcon(qApp->loadIcon(":/images/open-font.png"));
	ui_->filterButton->setIcon(qApp->loadIcon(":/images/filter.png"));
    ui_->recentButton->setIcon(qApp->loadIcon(":/images/history.png"));

    connect(ui_->openButton, &QPushButton::clicked,
            this, &QXOpenFontDialog::onOpenFileButtonClicked);

    connect(ui_->fontComboBox, &QXFontComboBox::fontSelected,
            this, &QXOpenFontDialog::onFontSelected);
    
    QXFontManager::instance();

    QXDocumentWindowManager * mgr = QXDocumentWindowManager::instance();

    if (mgr->recentFonts().size()) {
        for (const auto & uri: mgr->recentFonts()) {
            if (-1 != ui_->fontComboBox->selectFont(uri))
                break;
        }
    }
    if (-1 == ui_->fontComboBox->selectedFontIndex())
        ui_->fontComboBox->selectFont(0);
    

    onFontSelected(ui_->fontComboBox->selectedFont(),
                     ui_->fontComboBox->selectedFontIndex());

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
                        if (-1 == ui_->fontComboBox->selectFont(uri)) {
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

    // Add actions
    QAction * closeAction = new QAction(this);
    connect(closeAction, &QAction::triggered, this, &QDialog::close);
    closeAction->setShortcuts(QKeySequence::Close);
    addAction(closeAction);

    QAction * quitAction = new QAction(this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    quitAction->setShortcuts(QKeySequence::Quit);
    addAction(quitAction);
}

QXOpenFontDialog::~QXOpenFontDialog() {
    delete ui_;
}

QXFontURI
QXOpenFontDialog::selectedFont() {
    return ui_->fontComboBox->selectedFont();
}

void
QXOpenFontDialog::accept() {
    ui_->fontComboBox->clearFilter();
    QDialog::accept();
}

void
QXOpenFontDialog::onOpenFileButtonClicked() {
    if (QXDocumentWindowManager::instance()->doOpenFontFromFile())
        reject();
}

void
QXOpenFontDialog::onFontSelected(const QXFontURI & uri, size_t index) {
    if (index == -1)
        return;
    auto & desc = QXFontManager::instance().db()->faceDescriptor(index);
    auto & atts = QXFontManager::instance().db()->faceAttributes(index);

    QXHtmlTemplate * html = QXHtmlTemplate::createFromFile(QXResource::path("/Html/FontInfoTemplate.html"));
    ui_->textBrowser->setHtml(html->instantialize(templateValues(desc, atts)));
    html->deleteLater();
}

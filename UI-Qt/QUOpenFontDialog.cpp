#include <QFileDialog>
#include <QMenu>
#include <QSettings>

#include "QUApplication.h"
#include "QUConv.h"
#include "QUDocumentWindowManager.h"
#include "QUHtmlTemplate.h"
#include "QUFontManager.h"
#include "QUResource.h"
#include "QUOpenFontDialog.h"

#if defined(Q_OS_MAC)
#  include "MacHelper.h"
#endif

#include "ui_QUOpenFontDialog.h"

namespace {
    QMap<QString, QVariant>
    templateValues(const FXFaceDescriptor & desc, const FXFaceAttributes & atts) {
        QMap<QString, QVariant> map;

        map["FULL_NAME"]   = QUDocument::faceDisplayName(atts);
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

QUOpenFontDialog::QUOpenFontDialog(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::QUOpenFontDialog)
    , recentMenu_(nullptr) {
    ui_->setupUi(this);
	ui_->filterButton->setIcon(quApp->loadIcon(":/images/filter.png"));
    ui_->recentButton->setIcon(quApp->loadIcon(":/images/history.png"));

    
    connect(ui_->fontComboBox, &QUFontComboBox::fontSelected,
            this, &QUOpenFontDialog::slotFontSelected);
    
    QUFontManager::get();

    if (QUDocumentWindowManager::instance()->recentFonts().size())
        ui_->fontComboBox->selectFont(QUDocumentWindowManager::instance()->recentFonts()[0]);

    slotFontSelected(ui_->fontComboBox->selectedFont(),
                     ui_->fontComboBox->selectedFontIndex());

    recentMenu_ = new QMenu(ui_->recentButton);
    ui_->recentButton->setMenu(recentMenu_);

    connect(recentMenu_, &QMenu::aboutToShow, [this]() {
        QUDocumentWindowManager::instance()->aboutToShowRecentMenu(recentMenu_);
        foreach (QAction * action, recentMenu_->actions()) {
            if (!action->isSeparator() && !action->menu()) {
                connect(action, &QAction::triggered, [this, action]() {
                    QVariant data = action->data();
                    if (data.canConvert<QUFontURI>()) {
                        QUFontURI uri = data.value<QUFontURI>();
                        ui_->fontComboBox->selectFont(uri);
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

QUOpenFontDialog::~QUOpenFontDialog() {
    delete ui_;
}

QUFontURI
QUOpenFontDialog::selectedFont() {
    return ui_->fontComboBox->selectedFont();
}

void
QUOpenFontDialog::accept() {
    QDialog::accept();

    //QSettings settings;
    //settings.setValue("")
}

void
QUOpenFontDialog::slotBrowseFile() {
    QString file = QFileDialog::getOpenFileName(
        this,
        tr("Open Font"),
        QString(),
        tr("Fonts (*.otf *.ttf);;All Files (*)"));

}

void
QUOpenFontDialog::slotFontSelected(const QUFontURI & uri, size_t index) {
    if (index == -1)
        return;
    auto & desc = QUFontManager::get().db()->faceDescriptor(index);
    auto & atts = QUFontManager::get().db()->faceAttributes(index);

    QUHtmlTemplate * html = QUHtmlTemplate::createFromFile(QUResource::path("/Html/FontInfoTemplate.html"));
    ui_->textBrowser->setHtml(html->instantialize(templateValues(desc, atts)));
    html->deleteLater();
}


void
QUOpenFontDialog::showEvent(QShowEvent * event) {
#if defined(Q_OS_MAC)
    MacHelper::hideTitleBar(this);
#endif
    QDialog::showEvent(event);
}

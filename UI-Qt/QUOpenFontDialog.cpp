#include <QFileDialog>
#include <QSettings>

#include "QUConv.h"
#include "QUDocumentWindowManager.h"
#include "QUHtmlTemplate.h"
#include "QUFontManager.h"
#include "QUResource.h"
#include "QUOpenFontDialog.h"
#include "ui_QUOpenFontDialog.h"

namespace {
    QMap<QString, QVariant>
    templateValues(const FXFaceDescriptor & desc, const FXFaceAttributes & atts) {
        QMap<QString, QVariant> map;

        QString familyName = toQString(atts.names.familyName());
        QString styleName = toQString(atts.names.styleName());

        QString fullName;
        if (!familyName.isEmpty())
            fullName = QString("%1 - %2").arg(familyName, styleName);
        else
            fullName = QString("%1 - %2").arg(QFileInfo(toQString(desc.filePath)).fileName(), desc.index);

        map["FULL_NAME"]   = fullName;
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
        map["IS_OT_VARIANT"] = atts.isOTVariant;
        map["IS_MM"]       = atts.isMM;
        return map;
    }
}

QUOpenFontDialog::QUOpenFontDialog(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::QUOpenFontDialog) {
    ui_->setupUi(this);
    
    connect(ui_->browseButton, &QPushButton::clicked,
            this, &QUOpenFontDialog::slotBrowseFile);
    connect(ui_->fontComboBox, &QUFontComboBox::fontSelected,
            this, &QUOpenFontDialog::slotFontSelected);
    
    QUFontManager::get();

    if (QUDocumentWindowManager::instance()->recentFonts().size())
        ui_->fontComboBox->selectFont(QUDocumentWindowManager::instance()->recentFonts()[0]);

    slotFontSelected(ui_->fontComboBox->selectedFont(),
                     ui_->fontComboBox->selectedFontIndex());
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
    auto & desc = QUFontManager::get().db()->faceDescriptor(index);
    auto & atts = QUFontManager::get().db()->faceAttributes(index);

    QUHtmlTemplate html(QUResource::path("/Html/FontInfoTemplate.html"));
    ui_->textBrowser->setHtml(html.instantialize(templateValues(desc, atts)));
}

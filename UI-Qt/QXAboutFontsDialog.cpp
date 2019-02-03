#include "QXAboutFontsDialog.h"
#include "QXHtmlTemplate.h"
#include "QXFontManager.h"

#include "ui_QXAboutFontsDialog.h"

namespace {
    struct FormatStats {
        size_t total;
        
        size_t truetype;
        size_t cff;
        size_t type1;
        size_t winfnt;
        size_t other;
    };

    FormatStats
    fontFormatStats() {
        FXSet<FXString> total, tt, cff, t1, winfnt, other;
        auto db = QXFontManager::instance().db();
        for (size_t i = 0;  i < db->faceCount(); ++ i) {
            auto & desc = db->faceDescriptor(i);
            auto & atts = db->faceAttributes(i);
            total.insert(desc.filePath);
            if (atts.format == FXFaceFormatConstant::TrueType)
                tt.insert(desc.filePath);
            else if (atts.format == FXFaceFormatConstant::Type1)
                t1.insert(desc.filePath);
            else if (atts.format == FXFaceFormatConstant::CFF)
                cff.insert(desc.filePath);
            else if (atts.format == FXFaceFormatConstant::WinFNT)
                winfnt.insert(desc.filePath);
            else
                other.insert(desc.filePath);
        }

        FormatStats stats;
        stats.total = total.size();
        stats.truetype = tt.size();
        stats.cff = cff.size();
        stats.type1 = t1.size();
        stats.winfnt = winfnt.size();
        stats.other = other.size();
        return stats;
    }
}

QXAboutFontsDialog::QXAboutFontsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QXAboutFontsDialog) {
    ui->setupUi(this);

    QXHtmlTableTemplate html;
    html.addHeadRow(tr("Font Folders"));
    html.addDataRow(tr("System"), QXFontManager::instance().systemFontFolders().join("<br>"));
    html.addDataRow(tr("User"), QXFontManager::instance().userFontFolders().join("<br>"));

    html.addEmptyRow();
    FormatStats stats = fontFormatStats();
    html.addHeadRow("Font Formats");
    html.addDataRow(tr("Total"), stats.total);
    html.addDataRow(tr("True Type"), stats.truetype);
    html.addDataRow(tr("CFF"), stats.cff);
    html.addDataRow(tr("Postscript Type 1"), stats.type1);
    html.addDataRow(tr("Windows FNT"), stats.winfnt);
    html.addDataRow(tr("Other"), stats.other);
    
    ui->textBrowser->setHtml(html.html());
}

QXAboutFontsDialog::~QXAboutFontsDialog() {
    delete ui;
}

void
QXAboutFontsDialog::showAbout() {
    QXAboutFontsDialog dialog;
    dialog.exec();
}

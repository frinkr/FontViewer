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

    struct DuplicateStats {
        FXMap<FXStrig, FXSet<FXFaceDescriptor>> duplicates;
    };

    struct FontDbStats {
        FormatStats format;
    };

    FontDbStats
    fontDbStats() {
        FXSet<FXString> total, tt, cff, t1, winfnt, other;
        FXMap<FXStrig, FXSet<FXFaceDescriptor>> duplicates;

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

        FormatStats fmt;
        fmt.total = total.size();
        fmt.truetype = tt.size();
        fmt.cff = cff.size();
        fmt.type1 = t1.size();
        fmt.winfnt = winfnt.size();
        fmt.other = other.size();

        FontDbStats stats;
        stats.format = fmt;
        return stats;
    }
}

QXAboutFontsDialog::QXAboutFontsDialog(QWidget *parent)
    : QDialog(parent, Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::WindowShadeButtonHint)
    , ui(new Ui::QXAboutFontsDialog) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    const auto stats = fontDbStats();

    QXHtmlTableTemplate html;
    html.addHeadRow(tr("Font Folders"));
    html.addDataRow(tr("System"), QXFontManager::instance().systemFontFolders().join("<br>"));
    html.addDataRow(tr("User"), QXFontManager::instance().userFontFolders().join("<br>"));

    html.addEmptyRow();
    html.addHeadRow("Font Files");
    html.addDataRow(tr("Total"), stats.format.total);
    html.addDataRow(tr("True Type"), stats.format.truetype);
    html.addDataRow(tr("CFF"), stats.format.cff);
    html.addDataRow(tr("Postscript Type 1"), stats.format.type1);
    html.addDataRow(tr("Windows FNT"), stats.format.winfnt);
    html.addDataRow(tr("Other"), stats.format.other);
    
    ui->textBrowser->setHtml(html.html());
}

QXAboutFontsDialog::~QXAboutFontsDialog() {
    delete ui;
}

void
QXAboutFontsDialog::showAbout(bool modal) {
    if (modal) {
        QXAboutFontsDialog dialog;
        dialog.exec();
    } else {
        static QXAboutFontsDialog * instance = nullptr;
        if (!instance) {
            instance = new QXAboutFontsDialog;
            connect(instance, &QObject::destroyed, []() {
                instance = nullptr;
            });
        }
        
        instance->show();
        instance->raise();
        instance->activateWindow();
    }
}

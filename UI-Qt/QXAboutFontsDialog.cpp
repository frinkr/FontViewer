#include "QXAboutFontsDialog.h"
#include "QXHtmlTemplate.h"
#include "QXFontManager.h"

#include "ui_QXAboutFontsDialog.h"

namespace {
    
    struct FontDbStats {
        size_t totalFiles;

        // Format to count
        FXMap<FXString, size_t> formatCount;
        // PS name to files
        FXMap<FXString, FXSet<FXFaceDescriptor>> duplicates;
    };
    
    FontDbStats
    fontDbStats() {
        FXSet<FXString> allFiles;
        FXMap<FXString, FXSet<FXString> > formats;
        FXMap<FXString, FXSet<FXFaceDescriptor>> duplicates;

        auto db = QXFontManager::instance().db();
        for (size_t i = 0;  i < db->faceCount(); ++ i) {
            auto & desc = db->faceDescriptor(i);
            auto & atts = db->faceAttributes(i);
            allFiles.insert(desc.filePath);

            formats[atts.format].insert(desc.filePath);
            duplicates[atts.names.postscriptName()].insert(desc);
        }

        FontDbStats stats;
        stats.totalFiles = allFiles.size();
        for (const auto &kv : formats) 
            stats.formatCount[kv.first] = kv.second.size();
        
        for (const auto &kv : duplicates) {
            if (kv.second.size() > 1)
                stats.duplicates[kv.first] = kv.second;
        }
        return stats;
    }
}

QXAboutFontsDialog::QXAboutFontsDialog(QWidget *parent)
    : QXThemedWindow<QDialog>(parent, Qt::Dialog | Qt::WindowStaysOnTopHint)
    , ui(new Ui::QXAboutFontsDialog) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    const auto stats = fontDbStats();

    QXHtmlTableTemplate html;
    html.addHeadRow(tr("Font Folders"));
    html.addDataRow(tr("System"), QXFontManager::instance().systemFontFolders().join("<br>"));
    html.addDataRow(tr("User"), QXFontManager::instance().userFontFolders().join("<br>"));

    html.addHeadRow("Font Files");
    html.addDataRow(tr("Total Fonts"), QXFontManager::instance().db()->faceCount());
    html.addDataRow(tr("Total Files"), stats.totalFiles);
    for (const auto & kv : stats.formatCount) {
        QString format = kv.first.empty() ? "<i>UNKNOWN</i>" : toQString(kv.first);
        html.addDataRow(format, kv.second);
    }
    
    html.addHeadRow(tr("Postscript Name Duplicates"));
    for (const auto & kv : stats.duplicates) {
        QList<QString> files;
        for (auto & file : kv.second)
            files.append(toQString(file.filePath));
        html.addDataRow(toQString(kv.first), files.join("<br>"));
    }
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

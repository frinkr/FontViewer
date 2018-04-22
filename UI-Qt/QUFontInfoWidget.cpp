#include <QVBoxLayout>
#include "QUConv.h"
#include "QUResource.h"
#include "QUHtmlTemplate.h"
#include "QUFontInfoWidget.h"
#include "ui_QUFontInfoWidget.h"

namespace {
    class QUFontHtmlTemplatePage : public QUFontInfoPage {
    public:
        using QUFontInfoPage::QUFontInfoPage;

        QUFontHtmlTemplatePage(const QString & title, FXPtr<FXFace> face, const QString & templateFile, QObject * parent = nullptr)
            : QUFontInfoPage(title, face, parent) {
            htmlTemplate_ = new QUHtmlTemplate(templateFile, this);
        }
        
        virtual QString html() {
            return htmlTemplate_->instantialize(variables());
        }
        
        virtual QMap<QString, QVariant>
        variables() const = 0;
    protected:
        QUHtmlTemplate   * htmlTemplate_;
    };
    
    
    class QUFontGeneralPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
        variables() const {
            QMap<QString, QVariant> map;

            map["FILE"] = toQString(faceAtts().desc.filePath);
            map["INDEX"] = (int)faceAtts().desc.index;
            
            map["FAMILY_NAME"] = toQString(faceAtts().names.familyName());
            map["STYLE_NAME"] = toQString(faceAtts().names.styleName());
            map["PS_NAME"] = toQString(faceAtts().names.postscriptName());

            map["FORMAT"] = toQString(faceAtts().format);
            map["UPEM"] = (int)faceAtts().upem;
            return map;
        }
    };

    class QUFontHheaPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;


        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };

    class QUFontHmtxPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };

    class QUFontOS2Page : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };

    class QUFontPostPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };

    class QUFontGDEFPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };

    class QUFontGSUBPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };

    class QUFontGPOSPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };
    class QUFontGlyfPage : public QUFontHtmlTemplatePage {
    public:
        using QUFontHtmlTemplatePage::QUFontHtmlTemplatePage;

        virtual QMap<QString, QVariant>
            variables() const {
            QMap<QString, QVariant> map;
            return map;
        }
    };
}

QUFontInfoPage::QUFontInfoPage(const QString & title, FXPtr<FXFace> face, QObject * parent)
    : QObject(parent)
    , title_(title)
    , face_(face)
{
}

const QString &
QUFontInfoPage::title() const {
    return title_;
}

const FXFaceAttributes &
QUFontInfoPage::faceAtts() const {
    return face_->attributes();
}

QUFontInfoWidget::QUFontInfoWidget(FXPtr<FXFace> face, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QUFontInfoWidget)
{
    ui->setupUi(this);

    pages_.append(new QUFontGeneralPage(tr("General"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontHheaPage(tr("hhea"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontHmtxPage(tr("hmtx"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontOS2Page(tr("OS/2"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontPostPage(tr("post"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontGDEFPage(tr("GDEF"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontGSUBPage(tr("GSUB"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontGPOSPage(tr("GPOS"), face, QUResource::path("/Html/Font/general.html"), this));
    pages_.append(new QUFontGlyfPage(tr("glyf"), face, QUResource::path("/Html/Font/general.html"), this));
    
    ui->comboBox->clear();
    foreach(QUFontInfoPage * page, pages_)
        ui->comboBox->addItem(page->title());
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QUFontInfoWidget::onCombobox);

    onCombobox(0);
}

QUFontInfoWidget::~QUFontInfoWidget()
{
    delete ui;
}
    
void
QUFontInfoWidget::onCombobox(int index) {
    ui->textBrowser->setHtml(pages_.at(index)->html());
}

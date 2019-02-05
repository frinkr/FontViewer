#include <QVBoxLayout>
#include "FontX/FXFT.h"
#include "FontX/FXTag.h"

#include "QXConv.h"
#include "QXResource.h"
#include "QXHtmlTemplate.h"
#include "QXFontInfoWidget.h"
#include "ui_QXFontInfoWidget.h"

#define QXFONTINFO_ADDFLAG(f) {f, #f}

namespace {
    template <typename T>
    struct QStringTraits {
        static QString
        toQString(const T & v) {
            return QString("%1").arg(v);
        }
    };

    template <> struct QStringTraits<bool> {
        static QString
        toQString(bool value) {
            return value? "Yes": "No";
        }
    };

    template <> struct QStringTraits<FXString> {
        static QString
        toQString(const FXString & value) {
            return ::toQString(value);
        }
    };

    class QXFontHtmlTemplatePage : public QXFontInfoPage {
    public:
        using QXFontInfoPage::QXFontInfoPage;

        QXFontHtmlTemplatePage(const QString & title, FXPtr<FXFace> face, QObject * parent = nullptr)
            : QXFontInfoPage(title, face, parent) {
            tableTemplate_ = new QXHtmlTableTemplate(this);
        }

        virtual QString html() {
            if (!rowsLoaded_) {
                loadTableRows();
                rowsLoaded_ = true;
            }
            return tableTemplate_->html();
        }

        template <typename T> void
        addDataRow(const QString & name, const T & value) {
            tableTemplate_->addDataRow(name, value);
        }

        void
        addHeadRow(const QString & text) {
            tableTemplate_->addHeadRow(text);
        }

        void
        addEmptyRow() {
            tableTemplate_->addEmptyRow();
        }


        virtual void
        loadTableRows() {
        }

    protected:
        bool rowsLoaded_{false};
        QXHtmlTableTemplate * tableTemplate_;
    };


    class QXFontGeneralPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

        void
        loadTableRows() override {
            FT_Face ftFace = face_->face();

            TT_Header * head = (TT_Header *)FT_Get_Sfnt_Table(ftFace, FT_SFNT_HEAD);
            TT_OS2 * os2 = (TT_OS2 *)FT_Get_Sfnt_Table(ftFace, FT_SFNT_OS2);

            addHeadRow(tr("Face"));
            addDataRow(tr("File"), toQString(faceAtts().desc.filePath));
            addDataRow(tr("Index"), (ftFace->num_faces > 1)?
                       QString("%1/%2").arg(static_cast<int>(faceAtts().desc.index)).arg(ftFace->num_faces):
                       QString("%1").arg(static_cast<int>(faceAtts().desc.index)));
            addDataRow(tr("Postscript"), toQString(faceAtts().names.postscriptName()));
            addDataRow(tr("Family Name"), toQString(faceAtts().names.familyName()));
            addDataRow(tr("Style Name"), toQString(faceAtts().names.styleName()));
            addDataRow(tr("Face Flags"), faceFlagsStr(ftFace->face_flags).join("<br>"));
            addDataRow(tr("Style Flags"), styleFlagsStr(ftFace->style_flags).join("<br>"));
            addDataRow(tr("Num Glyph"), static_cast<int>(faceAtts().glyphCount));
            addDataRow(tr("Num Fixed Sizes"), static_cast<int>(ftFace->num_fixed_sizes));
            addDataRow(tr("Num CMaps"), static_cast<int>(ftFace->num_charmaps));
            addDataRow(tr("UPEM"), static_cast<int>(faceAtts().upem));
            addDataRow(tr("Ascender"), static_cast<int>(ftFace->ascender));
            addDataRow(tr("Descender"), static_cast<int>(ftFace->descender));
            addDataRow(tr("Height"), static_cast<int>(ftFace->height));
            addDataRow(tr("Max Adv Width"), static_cast<int>(ftFace->max_advance_width));
            addDataRow(tr("Max Adv Height"), static_cast<int>(ftFace->max_advance_height));
            addDataRow(tr("Underline Position"), static_cast<int>(ftFace->underline_position));
            addDataRow(tr("Underline Thickness"), static_cast<int>(ftFace->underline_thickness));

            addEmptyRow();
            addHeadRow(tr("Format"));            
            addDataRow(tr("Format"), toQString(faceAtts().format));
            addDataRow(tr("IsCID"), faceAtts().isCID);
            addDataRow(tr("CID"), toQString(faceAtts().cid));
            addDataRow(tr("OpenType Variable"), faceAtts().isOpenTypeVariable);
            addDataRow(tr("Multiple Master"), faceAtts().isMultipleMaster);
            QString tables;
            if (FT_IS_SFNT(ftFace)) {
                FT_Error error = 0;
                FT_UInt tableIndex = 0;
                while (error != FT_Err_Table_Missing) {
                    FT_ULong length = 0, tag = 0;
                    error = FT_Sfnt_Table_Info(ftFace, tableIndex, &tag, &length);
                    ++ tableIndex;
                    if (!tables.isEmpty())
                        tables += ", ";
                    if (!error)
                        tables += QString("%1(%2)").arg(toQString(FXTag2Str(tag))).arg(length);
                }
            }
            addDataRow(tr("Tables"), tables.isEmpty()? tr("<i>NOT A SFNT FONT</i>"): tables);

            addEmptyRow();
            addHeadRow(tr("Version"));
            if (os2)
                addDataRow(tr("Vendor"), QString("<a href=https://www.microsoft.com/typography/links/vendorlist.aspx>%1</a>").arg(toQString(FXString((const char *)(os2->achVendID), 4))));
            else
                addDataRow(tr("Vendor"), faceAtts().names.vendor());
            addDataRow(tr("Version"), faceAtts().names.version());
            addDataRow(tr("Unique ID"), faceAtts().names.getSFNTName(TT_NAME_ID_UNIQUE_ID));
            addDataRow(tr("Copyright"), faceAtts().names.getSFNTName(TT_NAME_ID_COPYRIGHT));
            addDataRow(tr("License"), faceAtts().names.getSFNTName(TT_NAME_ID_LICENSE));
            if (head) {
                int64_t created = ((head->Created[0] & 0xFFFFFFFF) << 32) + (head->Created[1] & 0xFFFFFFFF);
                int64_t modified = ((head->Modified[0] & 0xFFFFFFFF) << 32) + (head->Modified[1] & 0xFFFFFFFF);
                addDataRow(tr("Creation"), ftDateTimeToString(created));
                addDataRow(tr("Modification"), ftDateTimeToString(modified));
            }
            else {
                addDataRow(tr("Creation"), QString());
                addDataRow(tr("Modification"), QString());
            }
        }



        static QStringList
        faceFlagsStr(FT_Long flags) {
            static FXMap<FT_Long, FXString> map = {
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_SCALABLE),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_FIXED_SIZES),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_FIXED_WIDTH),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_SFNT),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_HORIZONTAL),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_VERTICAL),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_KERNING),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_FAST_GLYPHS),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_MULTIPLE_MASTERS),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_GLYPH_NAMES),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_EXTERNAL_STREAM),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_HINTER),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_CID_KEYED),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_TRICKY),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_COLOR),
                QXFONTINFO_ADDFLAG(FT_FACE_FLAG_VARIATION)
            };
            QStringList list;
            for (const auto & kv: map) {
                if (flags & kv.first)
                    list << toQString(kv.second);
            }
            return list;
        }

        static QStringList
        styleFlagsStr(FT_Long flags) {
            static FXMap<FT_Long, FXString> map = {
                QXFONTINFO_ADDFLAG(FT_STYLE_FLAG_ITALIC),
                QXFONTINFO_ADDFLAG(FT_STYLE_FLAG_BOLD),
            };
            QStringList list;
            for (const auto & kv: map) {
                if (flags & kv.first)
                    list << toQString(kv.second);
            }
            return list;            
        }
    };

    class QXHheaPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXHmtxPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXOS2Page : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXPostPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXGDEFPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXGSUBPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXGPOSPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXGlyfPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    class QXWinFNTPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        void
        loadTableRows() override {
            FT_WinFNT_HeaderRec h;
            if (FT_Get_WinFNT_Header(ftFace_, &h)) {
                addHeadRow(tr("<i>ERROR TO LOAD WINDOWS FNT HEADER</i>"));
                return;
            }

            addDataRow(tr("File"), toQString(faceAtts().desc.filePath));
            addDataRow(tr("Index"), static_cast<int>(faceAtts().desc.index));
            addDataRow(tr("Version"), h.version);
            addDataRow(tr("File Size"), h.file_size);
            addDataRow(tr("Copyright"), FXString(reinterpret_cast<char*>(h.copyright)));
            addDataRow(tr("File Type"), h.file_type);
            addDataRow(tr("Nominal Point Size"), h.nominal_point_size);
            addDataRow(tr("Vertical Resolution"), h.vertical_resolution);
            addDataRow(tr("Horizontal Resolution"), h.horizontal_resolution);
            addDataRow(tr("Ascent"), h.ascent);
            addDataRow(tr("Internal Leading"), h.internal_leading);
            addDataRow(tr("External Leading"), h.external_leading);
            addDataRow(tr("Italic"), static_cast<bool>(h.italic));
            addDataRow(tr("Underline"), static_cast<bool>(h.underline));
            addDataRow(tr("Strike Out"), static_cast<bool>(h.strike_out));
            addDataRow(tr("Weight"), h.weight);
            addDataRow(tr("Charset"), charsetStr(h.charset));
            addDataRow(tr("Pixel Width"), h.pixel_width);
            addDataRow(tr("Pixel Height"), h.pixel_height);
            addDataRow(tr("Pitch & Family"), h.pitch_and_family);
            addDataRow(tr("Avg Width"), h.avg_width);
            addDataRow(tr("Max Width"), h.max_width);
            addDataRow(tr("First Char"), h.first_char);
            addDataRow(tr("Last Char"), h.last_char);
            addDataRow(tr("Default Char"), h.default_char);
            addDataRow(tr("Break Char"), h.break_char);
            addDataRow(tr("Bytes Per Row"), h.bytes_per_row);
            addDataRow(tr("Device Offset"), h.device_offset);
        }

    private:
        static QString
        charsetStr(FT_Byte cs) {
            switch(cs) {
            case FT_WinFNT_ID_DEFAULT: return "DEFAULT";
            case FT_WinFNT_ID_SYMBOL: return "SYMBOL";
            case FT_WinFNT_ID_MAC: return "MAC";
            case FT_WinFNT_ID_OEM: return "OEM";
            case FT_WinFNT_ID_CP874: return "CP874";
            case FT_WinFNT_ID_CP932: return "CP932";
            case FT_WinFNT_ID_CP936: return "CP936";
            case FT_WinFNT_ID_CP949: return "CP949";
            case FT_WinFNT_ID_CP950: return "CP950";
            case FT_WinFNT_ID_CP1250: return "CP1250";
            case FT_WinFNT_ID_CP1251: return "CP1251";
            case FT_WinFNT_ID_CP1252: return "CP1252";
            case FT_WinFNT_ID_CP1253: return "CP1253";
            case FT_WinFNT_ID_CP1254: return "CP1254";
            case FT_WinFNT_ID_CP1255: return "CP1255";
            case FT_WinFNT_ID_CP1256: return "CP1256";
            case FT_WinFNT_ID_CP1257: return "CP1257";
            case FT_WinFNT_ID_CP1258: return "CP1258";
            case FT_WinFNT_ID_CP1361: return "CP1361";
            default: return "UNKNOWN";
            }
        }
    };

    class QXType1Page : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        void
        loadTableRows() override {
            loadGeneralInfo();
        }

    private:
        void
        loadGeneralInfo() {
            PS_FontInfoRec info;
            if (FT_Get_PS_Font_Info(ftFace_, &info)) {
                addHeadRow(tr("<i>ERROR TO READ TYPE 1 INFO</i>"));
                return;
            }
            addHeadRow(tr("FontInfo"));
            addDataRow(tr("Version"), info.version);
            addDataRow(tr("Notice"), info.notice);
            addDataRow(tr("Full Name"), info.full_name);
            addDataRow(tr("Family Name"), info.family_name);
            addDataRow(tr("Weight"), info.weight);
            addDataRow(tr("Italic Angle"), info.italic_angle);
            addDataRow(tr("Fixed Pitch"), static_cast<bool>(info.is_fixed_pitch));
            addDataRow(tr("Underline Position"), info.underline_position);
            addDataRow(tr("Underline Thickness"), info.underline_thickness);

            T1_EncodingType encodingType;
            if (sizeof(encodingType) == FT_Get_PS_Font_Value(ftFace_, PS_DICT_ENCODING_TYPE, 0, &encodingType, sizeof(encodingType)))
                addDataRow(tr("Encoding Type"), encodingTypeToString(encodingType));
            else
                addDataRow(tr("Encoding Type"), tr("<i>UNKNOWN</i>"));

        }
        static QString
        encodingTypeToString(T1_EncodingType e) {
            switch(e) {
            case T1_ENCODING_TYPE_NONE: return "None";
            case T1_ENCODING_TYPE_ARRAY: return "Array";
            case T1_ENCODING_TYPE_STANDARD: return "Standard";
            case T1_ENCODING_TYPE_ISOLATIN1: return "ISO Latin 1";
            case T1_ENCODING_TYPE_EXPERT: return "Expert";
            }
            return "<i>UNKNOWN</i>";
        }
    };
}

QXFontInfoPage::QXFontInfoPage(const QString & title, FXPtr<FXFace> face, QObject * parent)
    : QObject(parent)
    , title_(title)
    , face_(face)
    , ftFace_(face_->face())
{
}

const QString &
QXFontInfoPage::title() const {
    return title_;
}

const FXFaceAttributes &
QXFontInfoPage::faceAtts() const {
    return face_->attributes();
}

QXFontInfoWidget::QXFontInfoWidget(FXPtr<FXFace> face, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QXFontInfoWidget)
{
    ui->setupUi(this);

    pages_.append(new QXFontGeneralPage(tr("General"), face, this));

    if (FT_IS_SFNT(face->face())) {
        pages_.append(new QXHheaPage(tr("hhea"), face, this));
        pages_.append(new QXHmtxPage(tr("hmtx"), face, this));
        pages_.append(new QXOS2Page(tr("OS/2"),  face, this));
        pages_.append(new QXPostPage(tr("post"), face, this));
        pages_.append(new QXGDEFPage(tr("GDEF"), face, this));
        pages_.append(new QXGSUBPage(tr("GSUB"), face, this));
        pages_.append(new QXGPOSPage(tr("GPOS"), face, this));
        pages_.append(new QXGlyfPage(tr("glyf"), face, this));
    }
    else if (face->attributes().format == FXFaceFormatConstant::WinFNT) {
        pages_.append(new QXWinFNTPage(tr("Windows FNT"), face, this));
    }
    else if (face->attributes().format == FXFaceFormatConstant::Type1) {
        pages_.append(new QXType1Page(tr("Postscript Type 1"), face, this));
    }

    ui->comboBox->clear();
    foreach(QXFontInfoPage * page, pages_)
        ui->comboBox->addItem(page->title());
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::activated),
            this, &QXFontInfoWidget::onCombobox);

    onCombobox(0);
}

QXFontInfoWidget::~QXFontInfoWidget()
{
    delete ui;
}

void
QXFontInfoWidget::onCombobox(int index) {
    ui->textBrowser->setHtml(pages_.at(index)->html());
}

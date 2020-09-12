#include <ctime>
#include <QVBoxLayout>
#include <QDateTime>
#include <QTimeZone>
#include "FontX/FXFT.h"
#include "FontX/FXFTNames.h"
#include "FontX/FXTag.h"
#include "FontX/FXInspector.h"
#include "FontX/FXPDF.h"

#include "QXConv.h"
#include "QXResources.h"
#include "QXHtmlTemplate.h"
#include "QXFontInfoWidget.h"
#include "QXNames.h"

#include "ui_QXFontInfoWidget.h"

#define QXFONTINFO_ADDFLAG(f) {f, #f}
#define TF_TABLE_POST    'post'

namespace {

    namespace fmt {
        template <typename T>
        std::string
        hexString(T w, const std::string & prefix = "0x") {
            constexpr size_t len = sizeof(T) << 1;
            static const char* digits = "0123456789ABCDEF";
            std::string rc(len, '0');
            for (size_t i = 0, j = (len-1)*4 ; i < len; ++ i, j -= 4)
                rc[i] = digits[(w>>j) & 0x0f];
            return prefix + rc;
        }
        
        template <typename T>
        const char * bitsString(T v) {
            constexpr size_t count = sizeof(T) * 8;
            static char buf[count + 1];
            
            for (size_t i = 0; i < count; ++ i) {
                if ((1 << i) & v)
                    buf[count - 1 - i] = '1';
                else
                    buf[count - 1 - i] = '0';
            }
            buf[count] = 0;
            return buf;
        }

        template <typename S>
        std::vector<std::string>
        joinImp(const S & s) {
            return {s};
        }

        std::vector<std::string>
        joinImp(const std::vector<std::string> & s) {
            return s;
        }

        template <typename T>
        std::string
        join(const T & t) {
            auto v = joinImp(t);
            std::string s;
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) s += "<br>";
                s += v[i];
            }
            return s;
        }

        template <typename T, typename ...U>
        std::string
        join(const T & t, const U & ...u) {
            auto head = join(t);
            auto tail = join(u...);
            return head + "<br>" + tail;
        }
    }

#define SWAP_ENDIAN_16(u) ((((u) & 0xFF) << 8) + (((u) & 0xFF00) >> 8))
    
#define SWAP_ENDIAN_32(u) ((((u) & 0xFF) << 24) + (((u) & 0xFF00) << 8) + (((u) & 0xFF0000) >> 8) + (((u) & 0xFF000000) >> 24))

    
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
            TT_Header * head = reinterpret_cast<TT_Header *>(FT_Get_Sfnt_Table(ftFace_, FT_SFNT_HEAD));
            TT_OS2 * os2 = reinterpret_cast<TT_OS2 *>(FT_Get_Sfnt_Table(ftFace_, FT_SFNT_OS2));

            addHeadRow(tr("Face"));
            addDataRow(tr("File"), toQString(faceAtts().desc.filePath));
            addDataRow(tr("Index"), (ftFace_->num_faces > 1)?
                       QString("%1/%2").arg(static_cast<int>(faceAtts().desc.index)).arg(ftFace_->num_faces):
                       QString("%1").arg(static_cast<int>(faceAtts().desc.index)));
            addDataRow(tr("Postscript"), toQString(faceAtts().names.postscriptName()));
            addDataRow(tr("Family Name"), toQString(faceAtts().names.familyName()));
            addDataRow(tr("Style Name"), toQString(faceAtts().names.styleName()));
            addDataRow(tr("Face Flags"), faceFlagsStr(ftFace_->face_flags).join("<br>"));
            addDataRow(tr("Style Flags"), styleFlagsStr(ftFace_->style_flags).join("<br>"));
            addDataRow(tr("FS Type Flags"), fsTypeFlagsStr(FT_Get_FSType_Flags(ftFace_)).join("<br>"));
            addDataRow(tr("Num Glyph"), static_cast<int>(faceAtts().glyphCount));
            addDataRow(tr("Num Fixed Sizes"), static_cast<int>(ftFace_->num_fixed_sizes));
            addDataRow(tr("Num CMaps"), static_cast<int>(ftFace_->num_charmaps));
            addDataRow(tr("UPEM"), static_cast<int>(faceAtts().upem));
            addDataRow(tr("Ascender"), static_cast<int>(ftFace_->ascender));
            addDataRow(tr("Descender"), static_cast<int>(ftFace_->descender));
            addDataRow(tr("Height"), static_cast<int>(ftFace_->height));
            addDataRow(tr("Max Adv Width"), static_cast<int>(ftFace_->max_advance_width));
            addDataRow(tr("Max Adv Height"), static_cast<int>(ftFace_->max_advance_height));
            addDataRow(tr("Underline Position"), static_cast<int>(ftFace_->underline_position));
            addDataRow(tr("Underline Thickness"), static_cast<int>(ftFace_->underline_thickness));

            addHeadRow(tr("Format"));            
            addDataRow(tr("Format"), toQString(faceAtts().format));
            addDataRow(tr("IsCID"), faceAtts().isCID);
            addDataRow(tr("CID"), toQString(faceAtts().cid));
            addDataRow(tr("OpenType Variable"), faceAtts().isOpenTypeVariable);
            addDataRow(tr("Multiple Master"), faceAtts().isMultipleMaster);
            QString tables;
            if (FT_IS_SFNT(ftFace_)) {
                FT_Error error = 0;
                FT_UInt tableIndex = 0;
                while (error != FT_Err_Table_Missing) {
                    FT_ULong length = 0, tag = 0;
                    error = FT_Sfnt_Table_Info(ftFace_, tableIndex, &tag, &length);
                    ++ tableIndex;
                    if (!tables.isEmpty())
                        tables += ", ";
                    if (!error)
                        tables += QString("%1(%2)").arg(toQString(FXTag2Str(tag))).arg(length);
                }
            }
            addDataRow(tr("Tables"), tables.isEmpty()? tr("<i>NOT A SFNT FONT</i>"): tables);

            addHeadRow(tr("Version"));
            if (os2)
                addDataRow(tr("Vendor"), QString("<a href=https://www.microsoft.com/typography/links/vendorlist.aspx>%1</a>").arg(toQString(FXString(reinterpret_cast<const char *>(os2->achVendID), 4))));
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
                {FT_FACE_FLAG_SCALABLE, "SCALABLE"},
                {FT_FACE_FLAG_FIXED_SIZES, "FIXED_SIZES"},
                {FT_FACE_FLAG_FIXED_WIDTH, "FIXED_WIDTH"},
                {FT_FACE_FLAG_SFNT, "SFNT"},
                {FT_FACE_FLAG_HORIZONTAL, "HORIZONTAL"},
                {FT_FACE_FLAG_VERTICAL, "VERTICAL"},
                {FT_FACE_FLAG_KERNING, "KERNING"},
                {FT_FACE_FLAG_FAST_GLYPHS, "GLYPHS"},
                {FT_FACE_FLAG_MULTIPLE_MASTERS, "MULTIPLE_MASTERS"},
                {FT_FACE_FLAG_GLYPH_NAMES, "GLYPH_NAMES"},
                {FT_FACE_FLAG_EXTERNAL_STREAM, "EXTERNAL_STREAM"},
                {FT_FACE_FLAG_HINTER, "HINTER"},
                {FT_FACE_FLAG_CID_KEYED, "CID_KEYED"},
                {FT_FACE_FLAG_TRICKY, "TRICKY"},
                {FT_FACE_FLAG_COLOR, "COLOR"},
#ifdef FT_FACE_FLAG_VARIATION
                {FT_FACE_FLAG_VARIATION, "VARIATION"},
#endif
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
                {FT_STYLE_FLAG_ITALIC, "ITALIC"},
                {FT_STYLE_FLAG_BOLD, "BOLD"},
            };
            QStringList list;
            for (const auto & kv: map) {
                if (flags & kv.first)
                    list << toQString(kv.second);
            }
            return list;            
        }
        
        static QStringList
        fsTypeFlagsStr(FT_UShort flags) {
            static FXMap<FT_Long, FXString> map = {
                {FT_FSTYPE_INSTALLABLE_EMBEDDING, "INSTALLABLE_EMBEDDING"},
                {FT_FSTYPE_RESTRICTED_LICENSE_EMBEDDING, "RESTRICTED_LICENSE_EMBEDDING"},
                {FT_FSTYPE_PREVIEW_AND_PRINT_EMBEDDING, "PREVIEW_AND_PRINT_EMBEDDING"},
                {FT_FSTYPE_EDITABLE_EMBEDDING, "EDITABLE_EMBEDDING"},
                {FT_FSTYPE_NO_SUBSETTING, "NO_SUBSETTING"},
                {FT_FSTYPE_BITMAP_EMBEDDING_ONLY, "BITMAP_EMBEDDING_ONLY"},
            };
            QStringList list;
            for (const auto & kv: map) {
                if (flags & kv.first)
                    list << toQString(kv.second);
            }
            return list;
        }
    };

    class QXHeadPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        
        void
        loadTableRows() override {
            if (TT_Header * head = (TT_Header *)FT_Get_Sfnt_Table(ftFace_, FT_SFNT_HEAD)) {
                int64_t created = ((head->Created[0] & 0xFFFFFFFF) << 32) + (head->Created[1] & 0xFFFFFFFF);
                int64_t modified = ((head->Modified[0] & 0xFFFFFFFF) << 32) + (head->Modified[1] & 0xFFFFFFFF);
                
                addDataRow(tr("Table Version"), fmt::hexString<uint32_t>(head->Table_Version));
                addDataRow(tr("Font Version"), fmt::hexString<uint32_t>(head->Font_Revision));
                addDataRow(tr("CheckSum Adjustment"), fmt::hexString<uint32_t>(head->CheckSum_Adjust));
                addDataRow(tr("Magic Number"), fmt::hexString<uint32_t>(head->Magic_Number));
                addDataRow(tr("Flags"), fmt::join(fmt::hexString<uint16_t>(head->Flags), QXNames::HeadGetFlagDescription(head->Flags)));
                addDataRow(tr("Units Per EM"), head->Units_Per_EM);
                addDataRow(tr("Created"), ftDateTimeToString(created));
                addDataRow(tr("Modified"), ftDateTimeToString(modified));
                addDataRow(tr("xMin"), head->xMin);
                addDataRow(tr("xMax"), head->xMax);
                addDataRow(tr("yMin"), head->yMin);
                addDataRow(tr("yMax"), head->yMax);
                addDataRow(tr("Mac Style"), fmt::join(fmt::hexString<uint16_t>(head->Mac_Style), QXNames::HeadGetMacStyleDescription(head->Mac_Style)));
                addDataRow(tr("Lowest Rec PPEM"), head->Lowest_Rec_PPEM);
                addDataRow(tr("Font Direction"), head->Font_Direction);
                addDataRow(tr("Index To Loc Format"), head->Index_To_Loc_Format);
                addDataRow(tr("Glyph Data Format"), head->Glyph_Data_Format);
            }
            
        }
    };
    
    class QXHheaPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

        void
        loadTableRows() override {
            if (TT_HoriHeader * hhea = (TT_HoriHeader *)FT_Get_Sfnt_Table(ftFace_, FT_SFNT_HHEA)) {
                addDataRow(tr("Version"), fmt::hexString<uint32_t>(hhea->Version));
                addDataRow(tr("Ascender"), hhea->Ascender);
                addDataRow(tr("Descender"), hhea->Descender);
                addDataRow(tr("Line Gap"), hhea->Line_Gap);
                addDataRow(tr("Advance Width Max"), hhea->advance_Width_Max);
                addDataRow(tr("Min Left Side Bearing"), hhea->min_Left_Side_Bearing);
                addDataRow(tr("Min Right Side Bearing"), hhea->min_Right_Side_Bearing);
                addDataRow(tr("xMax Extent"), hhea->xMax_Extent);
                addDataRow(tr("Caret Slope Rise"), hhea->caret_Slope_Rise);
                addDataRow(tr("Caret Slope Run"), hhea->caret_Slope_Run);
                addDataRow(tr("Caret Offset"), hhea->caret_Offset);
                addDataRow(tr("Metric Data Format"), hhea->metric_Data_Format);
                addDataRow(tr("Number Of HMetrics"), hhea->number_Of_HMetrics);
                addDataRow(tr("<i>Long Metrics</i>"), fmt::hexString(reinterpret_cast<uint64_t>(hhea->long_metrics)));
                addDataRow(tr("<i>Short Metrics</i>"), fmt::hexString(reinterpret_cast<uint64_t>(hhea->short_metrics)));
            }
            
        }
    };

    class QXHmtxPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        
        void
        loadTableRows() override {
            FT_Error error = 0;
            FT_ULong length = 0;
            
            TT_HoriHeader * hhea = (TT_HoriHeader *)FT_Get_Sfnt_Table(ftFace_, FT_SFNT_HHEA);
            
            typedef struct {
                uint16_t advanceWidth;
                int16_t lsb;
            } HorMetric;
            
            constexpr size_t rowsNumberLimit = 1000;
            bool rowsNumberLimitReach = false;
            
            error = FT_Load_Sfnt_Table(ftFace_, 'hmtx', 0, 0, &length);
            if (error ||  !hhea)
                return;
            
            FT_Byte * tableData = (FT_Byte *)malloc(length);
            error = FT_Load_Sfnt_Table(ftFace_, 'hmtx', 0, tableData, &length);
            if (error) {
                free(tableData);
                return;
            }
            
            
            HorMetric * metricHead = (HorMetric*)tableData;
            FT_UShort index = 0;
            for (; index < hhea->number_Of_HMetrics; ++ index) {
                if (index > rowsNumberLimit) {
                    rowsNumberLimitReach = true;
                    break;
                }
                HorMetric * metric = metricHead + index;
                char glyphName[128] = {0};
                if (FT_Get_Glyph_Name(ftFace_, index, glyphName, 128))
                    sprintf(glyphName, "glyph#%d", index);
                
                addDataRow(QString::number(index),
                           QString("%1, advance %2, lsb %3")
                           .arg(glyphName)
                           .arg((uint16_t)SWAP_ENDIAN_16(metric->advanceWidth))
                           .arg((int16_t)SWAP_ENDIAN_16(metric->lsb)));
            }
            
            TT_MaxProfile * maxp = (TT_MaxProfile *)FT_Get_Sfnt_Table(ftFace_, FT_SFNT_MAXP);
            if (maxp) {
                for (; index < maxp->numGlyphs; ++ index) {
                    if (index > rowsNumberLimit) {
                        rowsNumberLimitReach = true;
                        break;
                    }
                    
                    char glyphName[128] = {0};
                    if (FT_Get_Glyph_Name(ftFace_, index, glyphName, 128))
                        sprintf(glyphName, "glyph#%d", index);
                    
                    int16_t * lsb = (int16_t*)(metricHead + hhea->number_Of_HMetrics) + index;
                    addDataRow(QString::number(index), QString("%1, lsb %2").arg(glyphName).arg((int16_t)SWAP_ENDIAN_16(*lsb)));
                }
            }
            
            if (rowsNumberLimitReach) {
                addDataRow(QString(">_<"),
                           QString("<i>Too Many Glyphs, showing %1, more glyphs ommited</i>").arg(rowsNumberLimit));
            }
        
            free(tableData);
        }
    };

    class QXNamePage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

        void
        loadTableRows() override {
            FT_UInt sfntCount = FT_Get_Sfnt_Name_Count(ftFace_);
            for (FT_UInt i = 0; i < sfntCount; ++i) {
                FT_SfntName sfnt;
                if (FT_Get_Sfnt_Name(ftFace_, i, &sfnt))
                    continue;

                addDataRow(QString("(%1-%2 %3) %4")
                           .arg(sfnt.platform_id)
                           .arg(sfnt.encoding_id)
                           .arg(toQString(FXSFNTGetLanguage(&sfnt, ftFace_)))
                           .arg(toQString(FXSFNTGetName(&sfnt))),
                           toQString(FXSFNTGetValue(&sfnt)));
            } 
        }
    };

    class QXOS2Page : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        void
        loadTableRows() override {
            TT_OS2 * os2 = (TT_OS2 *)FT_Get_Sfnt_Table(ftFace_, FT_SFNT_OS2);
            if (!os2) 
                return;

            addDataRow("version", os2->version);
            addDataRow("xAvgCharWidth", os2->xAvgCharWidth);
            addDataRow("usWeightClass", QString("%1, %2").arg(os2->usWeightClass).arg(QXNames::OS2GetWeightClassName(os2->usWeightClass)));
            addDataRow("usWidthClass", QString("%1, %2").arg(os2->usWidthClass).arg(QXNames::OS2GetWidthClassName(os2->usWidthClass)));
            addDataRow("fsType", fmt::join(fmt::hexString(os2->fsType), QXNames::OS2GetFsTypeDescription(os2->fsType)));
            
            addDataRow("ySubscriptXSize", os2->ySubscriptXSize);
            addDataRow("ySubscriptYSize", os2->ySubscriptYSize);
            addDataRow("ySubscriptXOffset", os2->ySubscriptXOffset);
            addDataRow("ySubscriptYOffset", os2->ySubscriptYOffset);
            addDataRow("ySuperscriptXSize", os2->ySuperscriptXSize);
            addDataRow("ySuperscriptYSize", os2->ySuperscriptYSize);
            addDataRow("ySuperscriptXOffset", os2->ySuperscriptXOffset);
            addDataRow("ySuperscriptYOffset", os2->ySuperscriptYOffset);
            addDataRow("yStrikeoutSize", os2->yStrikeoutSize);
            addDataRow("yStrikeoutPosition", os2->yStrikeoutPosition);


            // Family class
            unsigned short familyClass = ((os2->sFamilyClass & 0xFF00) >> 8);
            unsigned short subFamilyClass = (os2->sFamilyClass & 0xFF);

            addDataRow("sFamilyClass", QString("%1, %2 (%3)")
                       .arg(familyClass)
                       .arg(subFamilyClass)
                       .arg(QString::fromStdString(QXNames::OS2GetFamilyClassFullName(os2->sFamilyClass))));

            // panose
            {
                std::vector<std::string> panose;
                std::string hex;
                for (size_t i = 0; i < 10; ++ i) {
                    if (i) hex.append(" ");
                    hex.append(fmt::hexString((unsigned short)os2->panose[i]));
                }
                panose.push_back(hex);
                panose.push_back(std::string("Family: ") + QXNames::OS2GetPanoseFamilyType(os2->panose[0]));
                panose.push_back(std::string("Serif: ") + QXNames::OS2GetPanoseSerifType(os2->panose[1]));
                panose.push_back(std::string("Weight: ") + QXNames::OS2GetPanoseWeight(os2->panose[2]));
                panose.push_back(std::string("Proportion: ") + QXNames::OS2GetPanoseProportion(os2->panose[3]));
                panose.push_back(std::string("Contrast: ") + QXNames::OS2GetPanoseContrast(os2->panose[4]));
                panose.push_back(std::string("Stroke Variation: ") + QXNames::OS2GetPanoseStrokeVariation(os2->panose[5]));
                panose.push_back(std::string("Arm: ") + QXNames::OS2GetPanoseArmStyle(os2->panose[6]));
                panose.push_back(std::string("Letter: ") + QXNames::OS2GetPanoseLetterform(os2->panose[7]));
                panose.push_back(std::string("Midline: ") + QXNames::OS2GetPanoseMidline(os2->panose[8]));
                panose.push_back(std::string("X Height: ") + QXNames::OS2GetPanoseXHeight(os2->panose[9]));
                addDataRow("PANOSE" , fmt::join(panose));
            }
                 

            // unicode range
            addDataRow("ulUnicodeRange1", fmt::bitsString<uint32_t>(os2->ulUnicodeRange1));
            addDataRow("ulUnicodeRange2", fmt::bitsString<uint32_t>(os2->ulUnicodeRange2));
            addDataRow("ulUnicodeRange3", fmt::bitsString<uint32_t>(os2->ulUnicodeRange3));
            addDataRow("ulUnicodeRange4", fmt::bitsString<uint32_t>(os2->ulUnicodeRange4));


            addDataRow("<i>Unicode Ranges</i>" , fmt::join(QXNames::OS2GetUnicodeRanges(os2->ulUnicodeRange1, os2->ulUnicodeRange2,
                                                                                        os2->ulUnicodeRange3, os2->ulUnicodeRange4)));

            // Vender
            addDataRow("achVendID" , std::string((char*)(&os2->achVendID), 4));
            addDataRow("fsSelection" , fmt::join(fmt::hexString(os2->fsSelection), QXNames::OS2GetFsSelectionDescription(os2->fsSelection)));

            addDataRow("usFirstCharIndex" ,os2->usFirstCharIndex);
            addDataRow("usLastCharIndex" ,os2->usLastCharIndex);
            addDataRow("sTypoAscender" ,os2->sTypoAscender);
            addDataRow("sTypoDescender" ,os2->sTypoDescender);
            addDataRow("sTypoLineGap" ,os2->sTypoLineGap);
            addDataRow("usWinAscent" ,os2->usWinAscent);
            addDataRow("usWinDescent" ,os2->usWinDescent);

            if (os2->version >= 1) {
                addDataRow("ulCodePageRange1", fmt::bitsString<uint32_t>(os2->ulCodePageRange1));
                addDataRow("ulCodePageRange2", fmt::bitsString<uint32_t>(os2->ulCodePageRange2));
                addDataRow("<i>CodePage Ranges</i>", fmt::join(QXNames::OS2GetCodePageRanges(os2->ulCodePageRange1,os2->ulCodePageRange2)));
            }

            if (os2->version >= 2) {
                addDataRow("sxHeight" ,os2->sxHeight);
                addDataRow("sCapHeight" ,os2->sCapHeight);
                addDataRow("usDefaultChar" ,os2->usDefaultChar);
                addDataRow("usBreakChar" ,os2->usBreakChar);
                addDataRow("usMaxContext" ,os2->usMaxContext);
            }

            if (os2->version >= 5) {
                addDataRow("usLowerOpticalPointSize" ,os2->usLowerOpticalPointSize);
                addDataRow("usUpperOpticalPointSize" ,os2->usUpperOpticalPointSize);
            }
        }
    };

    class QXPostPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

        void
        loadTableRows() override {
            TT_Postscript * post = (TT_Postscript *)FT_Get_Sfnt_Table(ftFace_, FT_SFNT_POST);
            if (!post)
                return;
            addDataRow("FormatType", fmt::hexString(post->FormatType));
            addDataRow("italicAngle", post->italicAngle);
            addDataRow("underlinePosition", post->underlinePosition);
            addDataRow("underlineThickness", post->underlineThickness);
            addDataRow("isFixedPitch", post->isFixedPitch);
            addDataRow("minMemType42", post->minMemType42);
            addDataRow("maxMemType42", post->maxMemType42);
            addDataRow("minMemType1", post->minMemType1);
            addDataRow("maxMemType1", post->maxMemType1);
            
            // load names
            FT_ULong postLength = 0;
            FT_Error error = FT_Load_Sfnt_Table(ftFace_, TF_TABLE_POST, 0, 0, &postLength);
            if (error)
                return;
            
            uint32_t version = 0;
            FT_ULong versionLen = 4;
            error = FT_Load_Sfnt_Table(ftFace_, TF_TABLE_POST, 0, (FT_Byte*)&version, &versionLen);
            if (error)
                return;
            
            addDataRow("Version", fmt::hexString(SWAP_ENDIAN_32(version)));
                    
            if (SWAP_ENDIAN_32(version) == 0x00020000) {
                FT_Byte * buffer = (FT_Byte*) malloc(postLength);
                error = FT_Load_Sfnt_Table(ftFace_, TF_TABLE_POST, 0, buffer, &postLength);
                if (!error) {
                    FT_Byte * base = buffer + 8 * sizeof(uint32_t);
                    uint16_t numberGlyphs = SWAP_ENDIAN_16(*(uint16_t*)base);
                    uint16_t * glyphNameIndexBase = (uint16_t *)(base + sizeof(uint16_t));
                            
                    uint16_t macGlyphNameStart = 258;
                    int8_t * namesBase = (int8_t *)(base + sizeof(uint16_t) + numberGlyphs * sizeof(uint16_t));
                    int8_t * p = namesBase;
                            
                    uint16_t numNames = 0;
                    while ((p - (int8_t*)buffer) < postLength) {
                        p += (*p + 1);
                        ++ numNames;
                    }
                    if (numNames) {
                        p = namesBase;
                        int8_t * * nameArray = (int8_t * *)malloc(numNames * sizeof(int8_t *));
                        for (uint16_t index = 0; index < numNames; ++ index) {
                            nameArray[index] = p;
                            p += (*p + 1);
                        }
                                
                        for (uint16_t index = 0; index < numberGlyphs; ++ index) {
                            uint16_t nameIndex = SWAP_ENDIAN_16(*(glyphNameIndexBase + index));
                                    
                            std::string glyphName;
                            if (nameIndex < macGlyphNameStart) {
                                glyphName = QXNames::PostGetMacintoshGlyphName(nameIndex);
                            }
                            else {
                                nameIndex -= macGlyphNameStart;
                                if (nameIndex >= numNames)
                                    break;
                                int8_t * name = nameArray[nameIndex];
                                glyphName = std::string((char*)name + 1, *name);
                            }
                                    
                            addDataRow(QString::number(index), glyphName);
                        }
                        free(nameArray);
                    }
                }
                free(buffer);
            }
        }
    };

    class QXGDEFPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;

    };

    template <FXTag otTable>
    class QXGSUBGPOSPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        void
        loadTableRows() override {
            FXPtr<FXInspector> inspector = face_->inspector();
            auto otScripts = inspector->otScripts(otTable);
            for (auto otScript: otScripts) {
                tableTemplate_->addLongHeadRow(QString::fromStdString(
                           FXTag2Str(otScript) + " - " +
                           QXNames::OTGetScriptFullName(otScript)));
                
                auto otLanguages = inspector->otLanguages(otScript, otTable);
                if (otLanguages.empty())
                    addDataRow("<i>None</i>", "");
                
                for (auto otLanguage : otLanguages) {
                    auto otFeatures = inspector->otFeatures(otScript, otLanguage, otTable);
                    std::vector<std::string> otFeatureStrs;
                    for (auto feat : otFeatures)
                        otFeatureStrs.push_back(FXTag2Str(feat) + " (" + QXNames::OTGetFeatureFullName(feat) + ")");
                    addDataRow(QString::fromStdString(FXTag2Str(otLanguage)), fmt::join(otFeatureStrs));
                }
            }
        }
    };
    
    using QXGSUBPage = QXGSUBGPOSPage<FXTableGSUB>;
    using QXGPOSPage = QXGSUBGPOSPage<FXTableGPOS>;

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

    class QXPsFontPage : public QXFontHtmlTemplatePage {
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

            FT_Long error = 0;
            T1_EncodingType encodingType;
            error = FT_Get_PS_Font_Value(ftFace_, PS_DICT_ENCODING_TYPE, 0, &encodingType, sizeof(encodingType));
            if (error == sizeof(encodingType))
                addDataRow(tr("Encoding Type"), encodingTypeToString(encodingType));
            else
                addDataRow(tr("Encoding Type"), tr("<i>UNKNOWN</i>"));

            addHeadRow(tr("Private Dict"));
            PS_PrivateRec privateRec;
            if (!FT_Get_PS_Font_Private(ftFace_, &privateRec)) {
                addDataRow(tr("Unique ID"), privateRec.unique_id);
            }
            
            FT_UShort stdHw;
            error = FT_Get_PS_Font_Value(ftFace_, PS_DICT_STD_HW, 0, &stdHw, sizeof(stdHw));
            if (error == sizeof(stdHw)) {
                
            }
            


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

#if FX_HAS_PDF_ADDON
    class QXPdfPage : public QXFontHtmlTemplatePage {
    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        void
        loadTableRows() override {
            auto & info = face_->userProperties().get<const FXPDFDocumentInfo &>(FXPDFDocumentInfoKey);
            addDataRow(tr("Pages"), info.pages);
            addDataRow(tr("Application"), info.application);
            addDataRow(tr("Created"), utcTimeToLocal(info.created).toString("M/d/yyyy, h:m:ss AP"));
            addDataRow(tr("Modified"), utcTimeToLocal(info.modified).toString("M/d/yyyy, h:m:ss AP"));
        }
        
    private:
        QDateTime utcTimeToLocal(time_t time) {
            auto dt = QDateTime::fromSecsSinceEpoch(time, Qt::UTC);
            auto tz = QTimeZone::systemTimeZone();
            auto local = dt.toLocalTime();
            return local.addSecs(tz.offsetFromUtc(dt));
        }
    };
#endif
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
        pages_.append(new QXHeadPage(tr("head"), face, this));
        pages_.append(new QXHheaPage(tr("hhea"), face, this));
        pages_.append(new QXHmtxPage(tr("hmtx"), face, this));
        pages_.append(new QXNamePage(tr("name"), face, this));
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
        pages_.append(new QXPsFontPage(tr("Postscript Type 1"), face, this));
    }
    else if (face->attributes().format == FXFaceFormatConstant::CFF) {
        pages_.append(new QXPsFontPage(tr("CFF"), face, this));
    }
    
#if FX_HAS_PDF_ADDON
    if (face->userProperties().has(FXPDFDocumentInfoKey)) {
        pages_.append(new QXPdfPage(tr("PDF"), face, this));
    }
#endif
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

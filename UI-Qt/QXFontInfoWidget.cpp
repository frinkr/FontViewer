#include <boost/algorithm/string/join.hpp>

#include <QVBoxLayout>
#include "FontX/FXFT.h"
#include "FontX/FXFTNames.h"
#include "FontX/FXTag.h"

#include "QXConv.h"
#include "QXResource.h"
#include "QXHtmlTemplate.h"
#include "QXFontInfoWidget.h"
#include "ui_QXFontInfoWidget.h"

#define QXFONTINFO_ADDFLAG(f) {f, #f}

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
            return boost::algorithm::join(joinImp(t), "<br>");
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

            addEmptyRow();
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

            addEmptyRow();
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
                addDataRow(QString("∞"),
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
        static const char * OS2GetWeightClassName(uint16_t value) {
            switch (value) {
            case 100: return "Thin";
            case 200: return "Extra-light (Ultra-light)";
            case 300: return "Light";
            case 400: return "Normal (Regular)";
            case 500: return "Medium";
            case 600: return "Semi-bold (Demi-bold)";
            case 700: return "Bold";
            case 800: return "Extra-bold (Ultra-bold)";
            case 900: return "Black (Heavy)";
            default: return "Unknown";
            }
        }
        
        static const char * OS2GetWidthClassName(uint16_t value) {
            switch (value) {
            case 1: return "Ultra-condensed";
            case 2: return "Extra-condensed";
            case 3: return "Condensed";
            case 4: return "Semi-condensed";
            case 5: return "Medium (normal)";
            case 6: return "Semi-expanded";
            case 7: return "Expanded";
            case 8: return "Extra-expanded";
            case 9: return "Ultra-expanded";
            default: return "Unknown";
            }
        }
        
        static std::vector<std::string> OS2GetFsTypeDescription(uint16_t value) {
            if (!value)
                return {"Installable Embedding"};
            
            std::vector<std::string> names;
            
            if (value & (1 << 1)) names.push_back("Restricted License Embedding");
            if (value & (1 << 2)) names.push_back("Preview & Print Embedding");
            if (value & (1 << 3)) names.push_back("Editable Embedding");
            if (value & (1 << 8)) names.push_back("No Subsetting");
            if (value & (1 << 9)) names.push_back("Bitmap Embedding Only");
            
            return names;
        }
        
        static const char * OS2GetFamilyClassName(uint16_t value) {
            unsigned short family = ((value & 0xFF00) >> 8);
            
            const char * names [] = {"No classification",
                                     "OldStyle Serifs",
                                     "Transitional Serifs",
                                     "Modern Serifs",
                                     "Clarendon Serifs",
                                     "Slab Serifs",
                                     "Reserved",
                                     "Freeform Serifs",
                                     "Sans Serif",
                                     "Ornamentals",
                                     "Scripts",
                                     "reserved",
                                     "Symbolic",
                                     "reserved",
                                     "reserved",
            };
            
            if (family < sizeof(names)/sizeof(names[0]))
                return names[family];
            return "Unknown Family Class";
        }
        
        static const char * OS2GetSubFamilyClassName(uint16_t value) {
            uint16_t family = ((value & 0xFF00) >> 8);
            uint16_t subFamily = (value & 0xFF);
            
            std::vector<const char *> subFamilyNames;
            switch (family) {
            case 1: subFamilyNames = {"No classification",
                                      "IBM Rounded Legibility",
                                      "Garalde",
                                      "Venetian",
                                      "Modified Venetian",
                                      "Dutch Modern",
                                      "Dutch Traditional",
                                      "Contemporary",
                                      "Calligraphic",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "Miscellaneous"};
                break;
            case 2: subFamilyNames = {"No Classification",
                                      "Direct Line",
                                      "Script",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "Miscellaneous"};
                break;
            case 3: subFamilyNames = {"No Classification",
                                      "Italian",
                                      "Script",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "Miscellaneous"};
                break;
            case 4: subFamilyNames = {"No Classification",
                                      "Clarendon",
                                      "Modern",
                                      "Traditional",
                                      "Newspaper",
                                      "Stub Serif",
                                      "Monotone",
                                      "Typewriter",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "Miscellaneous"};
                break;
            case 5: subFamilyNames = {"No classification",
                                      "Monotone",
                                      "Humanist",
                                      "Geometric",
                                      "Swiss",
                                      "Typewriter",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "Miscellaneous"};
                break;
            case 7: subFamilyNames = {"No classification",
                                      "Modern",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "reserved",
                                      "Miscellaneous"};
                break;
            case 8: subFamilyNames = {"No classification",
                                      "IBM Neo-grotesque Gothic",
                                      "Humanist",
                                      "Low-x Round Geometric",
                                      "High-x Round Geometric",
                                      "Neo-grotesque Gothic",
                                      "Modified neo-grotesque Gothic",
                                      "Reserved",
                                      "Reserved",
                                      "Typewriter Gothic",
                                      "Matrix",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Miscellaneous"};
                break;
            case 9: subFamilyNames = {"No classification",
                                      "Engraver",
                                      "Black Letter",
                                      "Decorative",
                                      "Three Dimensional",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Miscellaneous"};
                break;
            case 10: subFamilyNames = {"No classification",
                                       "Uncial",
                                       "Brush Joined",
                                       "Formal Joined",
                                       "Monotone Joined",
                                       "Calligraphic",
                                       "Brush Unjoined",
                                       "Formal Unjoined",
                                       "Monotone Unjoined",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Miscellaneous"};
                break;
            case 12: subFamilyNames = {"No classification",
                                       "Reserved",
                                       "Reserved",
                                       "Mixed Serif",
                                       "Reserved",
                                       "Reserved",
                                       "Oldstyle Serif",
                                       "Neo-grotesque Sans Serif",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Reserved",
                                       "Miscellaneous"};
                break;
                    
            default:
                break;
            }
            
            if (subFamily < subFamilyNames.size())
                return subFamilyNames[subFamily];
            
            return "Unknown SubFamily Class";
        }

        static std::string OS2GetFamilyClassFullName(uint16_t value) {
            return OS2GetFamilyClassName(value) + std::string(OS2GetSubFamilyClassName(value));
        }
        
        static auto OS2GetUnicodeRanges(uint32_t range1, uint32_t range2, uint32_t range3, uint32_t range4) {
            std::vector<std::vector<const char *> > allBlockNames
                = {{"Basic Latin"},
                   {"Latin-1 Supplement"},
                   {"Latin Extended-A"},
                   {"Latin Extended-B"},
                   {"IPA Extensions", "Phonetic Extensions", "Phonetic Extensions Supplement"},
                   {"Spacing Modifier Letters", "Modifier Tone Letters"},
                   {"Combining Diacritical Marks", "Combining Diacritical Marks Supplement"},
                   {"Greek and Coptic"},
                   {"Coptic"},
                   {"Cyrillic", "Cyrillic Supplement", "Cyrillic Extended-A", "Cyrillic Extended-B"},
                   {"Armenian"},
                   {"Hebrew"},
                   {"Vai"},
                   {"Arabic", "Arabic Supplement"},
                   {"NKo"},
                   {"Devanagari"},
                   {"Bengali"},
                   {"Gurmukhi"},
                   {"Gujarati"},
                   {"Oriya"},
                   {"Tamil"},
                   {"Telugu"},
                   {"Kannada"},
                   {"Malayalam"},
                   {"Thai"},
                   {"Lao"},
                   {"Georgian", "Georgian Supplement"},
                   {"Balinese"},
                   {"Hangul Jamo"},
                   {"Latin Extended Additional", "Latin Extended-C", "Latin Extended-D"},
                   {"Greek Extended"},
                   {"General Punctuation", "Supplemental Punctuation"},
                   {"Superscripts And Subscripts"},
                   {"Currency Symbols"},
                   {"Combining Diacritical Marks For Symbols"},
                   {"Letterlike Symbols"},
                   {"Number Forms"},
                   {"Arrows", "Supplemental Arrows-A", "Supplemental Arrows-B", "Miscellaneous Symbols and Arrows"},
                   {"Mathematical Operators", "Supplemental Mathematical Operators", "Miscellaneous Mathematical Symbols-A", "Miscellaneous Mathematical Symbols-B"},
                   {"Miscellaneous Technical"},
                   {"Control Pictures"},
                   {"Optical Character Recognition"},
                   {"Enclosed Alphanumerics"},
                   {"Box Drawing"},
                   {"Block Elements"},
                   {"Geometric Shapes"},
                   {"Miscellaneous Symbols"},
                   {"Dingbats"},
                   {"CJK Symbols And Punctuation"},
                   {"Hiragana"},
                   {"Katakana", "Katakana Phonetic Extensions"},
                   {"Bopomofo", "Bopomofo Extended"},
                   {"Hangul Compatibility Jamo"},
                   {"Phags-pa"},
                   {"Enclosed CJK Letters And Months"},
                   {"CJK Compatibility"},
                   {"Hangul Syllables"},
                   {"High Surrogates", "High Private Use Surrogates", "Low Surrogates"},
                   {"Phoenician"},
                   {"CJK Unified Ideographs", "CJK Radicals Supplement", "Kangxi Radicals", "Ideographic Description Characters", "CJK Unified Ideographs Extension A", "CJK Unified Ideographs Extension B", "Kanbun"},
                   {"Private Use Area"},
                   {"CJK Strokes", "CJK Compatibility Ideographs", "CJK Compatibility Ideographs Supplement"},
                   {"Alphabetic Presentation Forms"},
                   {"Arabic Presentation Forms-A"},
                   {"Combining Half Marks"},
                   {"Vertical Forms", "CJK Compatibility Forms"},
                   {"Small Form Variants"},
                   {"Arabic Presentation Forms-B"},
                   {"Halfwidth And Fullwidth Forms"},
                   {"Specials"},
                   {"Tibetan"},
                   {"Syriac"},
                   {"Thaana"},
                   {"Sinhala"},
                   {"Myanmar"},
                   {"Ethiopic", "Ethiopic Supplement", "Ethiopic Extended"},
                   {"Cherokee"},
                   {"Unified Canadian Aboriginal Syllabics"},
                   {"Ogham"},
                   {"Runic"},
                   {"Khmer", "Khmer Symbols"},
                   {"Mongolian"},
                   {"Braille Patterns"},
                   {"Yi Syllables", "Yi Radicals"},
                   {"Tagalog", "Hanunoo", "Buhid", "Tagbanwa"},
                   {"Old Italic"},
                   {"Gothic"},
                   {"Deseret"},
                   {"Byzantine Musical Symbols", "Musical Symbols", "Ancient Greek Musical Notation"},
                   {"Mathematical Alphanumeric Symbols"},
                   {"Supplementary Private Use Area-A", "Supplementary Private Use Area-B"},
                   {"Variation Selectors", "Variation Selectors Supplement"},
                   {"Tags"},
                   {"Limbu"},
                   {"Tai Le"},
                   {"New Tai Lue"},
                   {"Buginese"},
                   {"Glagolitic"},
                   {"Tifinagh"},
                   {"Yijing Hexagram Symbols"},
                   {"Syloti Nagri"},
                   {"Linear B Syllabary", "Linear B Ideograms", "Aegean Numbers"},
                   {"Ancient Greek Numbers"},
                   {"Ugaritic"},
                   {"Old Persian"},
                   {"Shavian"},
                   {"Osmanya"},
                   {"Cypriot Syllabary"},
                   {"Kharoshthi"},
                   {"Tai Xuan Jing Symbols"},
                   {"Cuneiform", "Cuneiform Numbers and Punctuation"},
                   {"Counting Rod Numerals"},
                   {"Sundanese"},
                   {"Lepcha"},
                   {"Ol Chiki"},
                   {"Saurashtra"},
                   {"Kayah Li"},
                   {"Rejang"},
                   {"Cham"},
                   {"Ancient Symbols"},
                   {"Phaistos Disc"},
                   {"Carian", "Lycian", "Lydian"},
                   {"Domino Tiles", "Mahjong Tiles"},
            };
            
            std::vector<std::string> blockNames;
            uint32_t range[] = {range1, range2, range3, range4};
            for (size_t i = 0; i < allBlockNames.size(); ++ i) {
                unsigned char k = i % 32;
                unsigned char f = i / 32;
                if (range[f] & (1 << k)) {
                    auto & names = allBlockNames[i];
                    blockNames.insert(blockNames.end(), names.begin(), names.end());
                }
            }
            return blockNames;
        }
        
        auto OS2GetFsSelectionDescription(uint16_t value) {
            std::vector<std::string> names;
            
            if (value & (1 << 0)) names.push_back("Italic");
            if (value & (1 << 1)) names.push_back("Underscore");
            if (value & (1 << 2)) names.push_back("Negative");
            if (value & (1 << 3)) names.push_back("Outlined");
            if (value & (1 << 4)) names.push_back("Strikeout");
            if (value & (1 << 5)) names.push_back("Bold");
            if (value & (1 << 6)) names.push_back("Regular");
            if (value & (1 << 7)) names.push_back("Use Typo Metrics");
            if (value & (1 << 8)) names.push_back("WWS");
            if (value & (1 << 9)) names.push_back("Oblique");
            
            
            return names;
        }
        
        auto OS2GetCodePageRanges(uint32_t range1, uint32_t range2) {
            std::vector<std::pair<const char *, const char *>> allCodePages
            = {
                {"1252" , "Latin 1"},
                {"1250" , "Latin 2: Eastern Europe"},
                {"1251" , "Cyrillic"},
                {"1253" , "Greek"},
                {"1254" , "Turkish"},
                {"1255" , "Hebrew"},
                {"1256" , "Arabic"},
                {"1257" , "Windows Baltic"},
                {"1258" , "Vietnamese"},
                {"udef" , "Reserved for Alternate ANSI"},
                {"udef" , "Reserved for Alternate ANSI"},
                {"udef" , "Reserved for Alternate ANSI"},
                {"udef" , "Reserved for Alternate ANSI"},
                {"udef" , "Reserved for Alternate ANSI"},
                {"udef" , "Reserved for Alternate ANSI"},
                {"udef" , "Reserved for Alternate ANSI"},
                {"874"  , "Thai"},
                {"932"  , "JIS/Japan"},
                {"936"  , "Chinese: Simplified chars--PRC and Singapore"},
                {"949"  , "Korean Wansung"},
                {"950"  , "Chinese: Traditional chars--Taiwan and Hong Kong"},
                {"1361" , "Korean Johab"},
                {"udef" , "Reserved for Alternate ANSI & OEM"},
                {"udef" , "Reserved for Alternate ANSI & OEM"},
                {"udef" , "Reserved for Alternate ANSI & OEM"},
                {"udef" , "Reserved for Alternate ANSI & OEM"},
                {"udef" , "Reserved for Alternate ANSI & OEM"},
                {"udef" , "Reserved for Alternate ANSI & OEM"},
                {"udef" , "Reserved for Alternate ANSI & OEM"},
                {"udef" , "Macintosh Character Set (US Roman)"},
                {"udef" , "OEM Character Set"},
                {"udef" , "Symbol Character Set"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"udef" , "Reserved for OEM"},
                {"869"  , "IBM Greek"},
                {"866"  , "MS-DOS Russian"},
                {"865"  , "MS-DOS Nordic"},
                {"864"  , "Arabic"},
                {"863"  , "MS-DOS Canadian French"},
                {"862"  , "Hebrew"},
                {"861"  , "MS-DOS Icelandic"},
                {"860"  , "MS-DOS Portuguese"},
                {"857"  , "IBM Turkish"},
                {"855"  , "IBM Cyrillic; primarily Russian"},
                {"852"  , "Latin 2"},
                {"775"  , "MS-DOS Baltic"},
                {"737"  , "Greek; former 437 G"},
                {"708"  , "Arabic; ASMO 708"},
                {"850"  , "WE/Latin 1"},
                {"437"  , "US"},
            };
                
            std::vector<std::string> codePages;
                
            uint32_t range[] = {range1, range2};
            for (size_t i = 0; i < allCodePages.size(); ++ i) {
                unsigned char k = i % 32;
                unsigned char f = i / 32;
                if (range[f] & (1 << k)) {
                    auto code = allCodePages[i].first;
                    auto name = allCodePages[i].second;
                    if (atoi(code))
                        codePages.push_back(code + std::string(" ") + name);
                    else
                        codePages.push_back(name);
                }
            }
            return codePages;
        }
        
        template <typename T>
        static const char * OS2PanoseSearchInArray(uint8_t value, const std::initializer_list<T> & array) {
            
            auto itr = std::begin(array);
            if (std::distance(itr, std::end(array)) <= value)
                return "";
            std::advance(itr, value);
            return *itr;
        }
        
        static const char * OS2GetPanoseFamilyType(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Text and Display",
                "Script",
                "Decorative",
                "Pictorial",
                
            });
        }
        
        static const char * OS2GetPanoseSerifType(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Cove",
                "Obtuse Cove",
                "Square Cove",
                "Obtuse Square Cove",
                "Square",
                "Thin",
                "Bone",
                "Exaggerated",
                "Triangle",
                "Normal Sans",
                "Obtuse Sans",
                "Perp Sans",
                "Flared",
                "Rounded",
            });
        }
        
        static const char * OS2GetPanoseWeight(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Very Light",
                "Light",
                "Thin",
                "Book",
                "Medium",
                "Demi",
                "Bold",
                "Heavy",
                "Black",
                "Nord",
                
            });
        }
        
        static const char * OS2GetPanoseProportion(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Old Style",
                "Modern",
                "Even Width",
                "Expanded",
                "Condensed",
                "Very Expanded",
                "Very Condensed",
                "Monospaced",
                
            });
        }
        static const char * OS2GetPanoseContrast(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "None",
                "Very Low",
                "Low",
                "Medium Low",
                "Medium",
                "Medium High",
                "High",
                "Very High",
                
            });
        }
        static const char * OS2GetPanoseStrokeVariation(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Gradual/Diagonal",
                "Gradual/Transitional",
                "Gradual/Vertical",
                "Gradual/Horizontal",
                "Rapid/Vertical",
                "Rapid/Horizontal",
                "Instant/Vertical",
                
            });
        }
        static const char * OS2GetPanoseArmStyle(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Straight Arms/Horizontal",
                "Straight Arms/Wedge",
                "Straight Arms/Vertical",
                "Straight Arms/Single Serif",
                "Straight Arms/Double Serif",
                "Non-Straight Arms/Horizontal",
                "Non-Straight Arms/Wedge",
                "Non-Straight Arms/Vertical",
                "Non-Straight Arms/Single Serif",
                "Non-Straight Arms/Double Serif",
                
            });
        }
        static const char * OS2GetPanoseLetterform(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Normal/Contact",
                "Normal/Weighted",
                "Normal/Boxed",
                "Normal/Flattened",
                "Normal/Rounded",
                "Normal/Off Center",
                "Normal/Square",
                "Oblique/Contact",
                "Oblique/Weighted",
                "Oblique/Boxed",
                "Oblique/Flattened",
                "Oblique/Rounded",
                "Oblique/Off Center",
                "Oblique/Square",
                
            });
        }
        static const char * OS2GetPanoseMidline(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Standard/Trimmed",
                "Standard/Pointed",
                "Standard/Serifed",
                "High/Trimmed",
                "High/Pointed",
                "High/Serifed",
                "Constant/Trimmed",
                "Constant/Pointed",
                "Constant/Serifed",
                "Low/Trimmed",
                "Low/Pointed",
                "Low/Serifed",
                
            });
        }
        static const char * OS2GetPanoseXHeight(uint8_t value) {
            return OS2PanoseSearchInArray(value, {
                "Any",
                "No Fit",
                "Constant/Small",
                "Constant/Standard",
                "Constant/Large",
                "Ducking/Small",
                "Ducking/Standard",
                "Ducking/Large",
                
            });
        }

    public:
        using QXFontHtmlTemplatePage::QXFontHtmlTemplatePage;
        void
        loadTableRows() override {
            TT_OS2 * os2 = (TT_OS2 *)FT_Get_Sfnt_Table(ftFace_, FT_SFNT_OS2);
            if (!os2) 
                return;

            addDataRow("version", os2->version);
            addDataRow("xAvgCharWidth", os2->xAvgCharWidth);
            addDataRow("usWeightClass", QString("%1, %2").arg(os2->usWeightClass).arg(OS2GetWeightClassName(os2->usWeightClass)));
            addDataRow("usWidthClass", QString("%1, %2").arg(os2->usWidthClass).arg(OS2GetWidthClassName(os2->usWidthClass)));
            addDataRow("fsType", fmt::join(fmt::hexString(os2->fsType), OS2GetFsTypeDescription(os2->fsType)));
            
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
                       .arg(QString::fromStdString(OS2GetFamilyClassFullName(os2->sFamilyClass))));

            // panose
            {
                std::vector<std::string> panose;
                std::string hex;
                for (size_t i = 0; i < 10; ++ i) {
                    if (i) hex.append(" ");
                    hex.append(fmt::hexString((unsigned short)os2->panose[i]));
                }
                panose.push_back(hex);
                panose.push_back(std::string("Family: ") + OS2GetPanoseFamilyType(os2->panose[0]));
                panose.push_back(std::string("Serif: ") + OS2GetPanoseSerifType(os2->panose[1]));
                panose.push_back(std::string("Weight: ") + OS2GetPanoseWeight(os2->panose[2]));
                panose.push_back(std::string("Proportion: ") + OS2GetPanoseProportion(os2->panose[3]));
                panose.push_back(std::string("Contrast: ") + OS2GetPanoseContrast(os2->panose[4]));
                panose.push_back(std::string("Stroke Variation: ") + OS2GetPanoseStrokeVariation(os2->panose[5]));
                panose.push_back(std::string("Arm: ") + OS2GetPanoseArmStyle(os2->panose[6]));
                panose.push_back(std::string("Letter: ") + OS2GetPanoseLetterform(os2->panose[7]));
                panose.push_back(std::string("Midline: ") + OS2GetPanoseMidline(os2->panose[8]));
                panose.push_back(std::string("X Height: ") + OS2GetPanoseXHeight(os2->panose[9]));
                addDataRow("PANOSE" , fmt::join(panose));
            }
                 

            // unicode range
            addDataRow("ulUnicodeRange1", fmt::bitsString<uint32_t>(os2->ulUnicodeRange1));
            addDataRow("ulUnicodeRange2", fmt::bitsString<uint32_t>(os2->ulUnicodeRange2));
            addDataRow("ulUnicodeRange3", fmt::bitsString<uint32_t>(os2->ulUnicodeRange3));
            addDataRow("ulUnicodeRange4", fmt::bitsString<uint32_t>(os2->ulUnicodeRange4));


            addDataRow("<i>Unicode Ranges</i>" , fmt::join(OS2GetUnicodeRanges(os2->ulUnicodeRange1, os2->ulUnicodeRange2,
                                                                               os2->ulUnicodeRange3, os2->ulUnicodeRange4)));

            // Vender
            addDataRow("achVendID" , std::string((char*)(&os2->achVendID), 4));
                 addDataRow("fsSelection" , fmt::join(fmt::hexString(os2->fsSelection), OS2GetFsSelectionDescription(os2->fsSelection)));

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
                addDataRow("<i>CodePage Ranges</i>", fmt::join(OS2GetCodePageRanges(os2->ulCodePageRange1,os2->ulCodePageRange2)));
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

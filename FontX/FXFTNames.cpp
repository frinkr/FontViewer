#include <map>
#include <iconv.h>
#include <boost/predef.h>
#include <unicode/utypes.h>
#include <unicode/stringpiece.h>
#include <unicode/utf8.h>
#include <unicode/ustring.h>
#include <unicode/unistr.h>
#include <unicode/uchar.h>

#include "FXFTNames.h"

#if FX_MAC
#  include <CoreServices/CoreServices.h>
#endif

typedef struct {
    const FT_UShort	platform_id;
    const FT_UShort	language_id;
    const char	lang[8];
} FtLanguage;

#define TT_LANGUAGE_DONT_CARE	0xffff

// Copy from fontconfig
static const FtLanguage   ftLanguages[] = {
    {  TT_PLATFORM_APPLE_UNICODE,	TT_LANGUAGE_DONT_CARE,		"" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ENGLISH,		    "en" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FRENCH,		    "fr" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GERMAN,		    "de" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ITALIAN,		    "it" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_DUTCH,		    "nl" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SWEDISH,		    "sv" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SPANISH,		    "es" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_DANISH,		    "da" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_PORTUGUESE,	    "pt" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_NORWEGIAN,	    "no" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_HEBREW,		    "he" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_JAPANESE,		    "ja" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ARABIC,		    "ar" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FINNISH,		    "fi" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GREEK,		    "el" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ICELANDIC,	    "is" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALTESE,		    "mt" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TURKISH,		    "tr" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CROATIAN,		    "hr" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CHINESE_TRADITIONAL,  "zh-hant" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_URDU,		        "ur" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_HINDI,		    "hi" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_THAI,		        "th" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KOREAN,		    "ko" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LITHUANIAN,	    "lt" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_POLISH,		    "pl" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_HUNGARIAN,	    "hu" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ESTONIAN,		    "et" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LETTISH,		    "lv" },
    /* {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SAAMISK, ??? */
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FAEROESE,		    "fo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FARSI,		    "fa" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_RUSSIAN,		    "ru" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CHINESE_SIMPLIFIED,   "zh-hans" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FLEMISH,		    "nl" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_IRISH,		    "ga" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ALBANIAN,		    "sq" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ROMANIAN,		    "ro" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CZECH,		    "cs" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SLOVAK,		    "sk" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SLOVENIAN,	    "sl" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_YIDDISH,		    "yi" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SERBIAN,		    "sr" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MACEDONIAN,	    "mk" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BULGARIAN,	    "bg" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_UKRAINIAN,	    "uk" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BYELORUSSIAN,	    "be" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_UZBEK,		    "uz" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KAZAKH,		    "kk" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI,	    "az" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI_CYRILLIC_SCRIPT, "az" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI_ARABIC_SCRIPT,    "ar" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ARMENIAN,		    "hy" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GEORGIAN,		    "ka" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MOLDAVIAN,	    "mo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KIRGHIZ,		    "ky" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TAJIKI,		    "tg" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TURKMEN,		    "tk" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MONGOLIAN,	    "mo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MONGOLIAN_MONGOLIAN_SCRIPT,"mo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MONGOLIAN_CYRILLIC_SCRIPT, "mo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_PASHTO,		    "ps" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KURDISH,		    "ku" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KASHMIRI,		    "ks" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SINDHI,		    "sd" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TIBETAN,		    "bo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_NEPALI,		    "ne" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SANSKRIT,		    "sa" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MARATHI,		    "mr" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BENGALI,		    "bn" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ASSAMESE,		    "as" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GUJARATI,		    "gu" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_PUNJABI,		    "pa" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ORIYA,		    "or" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAYALAM,	    "ml" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KANNADA,		    "kn" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TAMIL,		    "ta" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TELUGU,		    "te" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SINHALESE,	    "si" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BURMESE,		    "my" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KHMER,		    "km" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LAO,		        "lo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_VIETNAMESE,	    "vi" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_INDONESIAN,	    "id" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TAGALOG,		    "tl" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAY_ROMAN_SCRIPT,   "ms" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAY_ARABIC_SCRIPT,  "ms" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AMHARIC,		    "am" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TIGRINYA,		    "ti" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GALLA,		    "om" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SOMALI,		    "so" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SWAHILI,		    "sw" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_RUANDA,		    "rw" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_RUNDI,		    "rn" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CHEWA,		    "ny" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAGASY,		    "mg" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ESPERANTO,	    "eo" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_WELSH,		    "cy" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BASQUE,		    "eu" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CATALAN,		    "ca" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LATIN,		    "la" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_QUECHUA,		    "qu" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GUARANI,		    "gn" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AYMARA,		    "ay" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TATAR,		    "tt" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_UIGHUR,		    "ug" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_DZONGKHA,		    "dz" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_JAVANESE,		    "jw" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SUNDANESE,	    "su" },
    
#if 0  /* these seem to be errors that have been dropped */
    
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SCOTTISH_GAELIC },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_IRISH_GAELIC },
    
#endif
    
    /* The following codes are new as of 2000-03-10 */
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GALICIAN,		    "gl" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AFRIKAANS,	    "af" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BRETON,		    "br" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_INUKTITUT,	    "iu" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SCOTTISH_GAELIC,	"gd" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MANX_GAELIC,	    "gv" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_IRISH_GAELIC,	    "ga" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TONGAN,		    "to" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GREEK_POLYTONIC,	"el" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GREELANDIC,	    "ik" },
    {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI_ROMAN_SCRIPT,"az" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_SAUDI_ARABIA,	"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_IRAQ,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_EGYPT,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_LIBYA,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_ALGERIA,	"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_MOROCCO,	"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_TUNISIA,	"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_OMAN,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_YEMEN,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_SYRIA,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_JORDAN,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_LEBANON,	"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_KUWAIT,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_UAE,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_BAHRAIN,	"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_QATAR,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BULGARIAN_BULGARIA,"bg" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CATALAN_SPAIN,		"ca" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_TAIWAN,	"zh-tw" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_PRC,		"zh-cn" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_HONG_KONG,	"zh-hk" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_SINGAPORE,	"zh-sg" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_MACAU,		"zh-mo" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CZECH_CZECH_REPUBLIC,	"cs" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DANISH_DENMARK,		"da" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_GERMANY,		"de" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_SWITZERLAND,	"de" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_AUSTRIA,		"de" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_LUXEMBOURG,		"de" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_LIECHTENSTEI,	"de" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GREEK_GREECE,		    "el" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_UNITED_STATES,	"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_UNITED_KINGDOM,"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_AUSTRALIA,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_CANADA,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_NEW_ZEALAND,	"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_IRELAND,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_SOUTH_AFRICA,	"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_JAMAICA,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_CARIBBEAN,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_BELIZE,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_TRINIDAD,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_ZIMBABWE,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_PHILIPPINES,	"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_SPAIN_TRADITIONAL_SORT,"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_MEXICO,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_SPAIN_INTERNATIONAL_SORT,"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_GUATEMALA,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_COSTA_RICA,	"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PANAMA,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_DOMINICAN_REPUBLIC,"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_VENEZUELA,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_COLOMBIA,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PERU,		    "es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_ARGENTINA,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_ECUADOR,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_CHILE,		    "es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_URUGUAY,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PARAGUAY,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_BOLIVIA,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_EL_SALVADOR,	"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_HONDURAS,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_NICARAGUA,		"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PUERTO_RICO,	"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FINNISH_FINLAND,		"fi" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_FRANCE,		    "fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_BELGIUM,		"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_CANADA,		    "fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_SWITZERLAND,	"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_LUXEMBOURG,		"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_MONACO,		    "fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HEBREW_ISRAEL,		    "he" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HUNGARIAN_HUNGARY,		"hu" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ICELANDIC_ICELAND,		"is" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ITALIAN_ITALY,		    "it" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ITALIAN_SWITZERLAND,	"it" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_JAPANESE_JAPAN,		"ja" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KOREAN_EXTENDED_WANSUNG_KOREA,"ko" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KOREAN_JOHAB_KOREA,	"ko" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DUTCH_NETHERLANDS,		"nl" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DUTCH_BELGIUM,		    "nl" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NORWEGIAN_NORWAY_BOKMAL,	"no" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NORWEGIAN_NORWAY_NYNORSK,	"nn" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_POLISH_POLAND,		    "pl" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PORTUGUESE_BRAZIL,		"pt" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PORTUGUESE_PORTUGAL,	"pt" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_RHAETO_ROMANIC_SWITZERLAND,"rm" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ROMANIAN_ROMANIA,		"ro" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MOLDAVIAN_MOLDAVIA,	"mo" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_RUSSIAN_RUSSIA,		"ru" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_RUSSIAN_MOLDAVIA,		"ru" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CROATIAN_CROATIA,		"hr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SERBIAN_SERBIA_LATIN,	"sr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SERBIAN_SERBIA_CYRILLIC,	"sr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SLOVAK_SLOVAKIA,		"sk" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ALBANIAN_ALBANIA,		"sq" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SWEDISH_SWEDEN,		"sv" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SWEDISH_FINLAND,		"sv" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_THAI_THAILAND,		    "th" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TURKISH_TURKEY,		"tr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_URDU_PAKISTAN,		    "ur" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_INDONESIAN_INDONESIA,	"id" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_UKRAINIAN_UKRAINE,		"uk" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BELARUSIAN_BELARUS,	"be" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SLOVENE_SLOVENIA,		"sl" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ESTONIAN_ESTONIA,		"et" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LATVIAN_LATVIA,		"lv" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LITHUANIAN_LITHUANIA,	"lt" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CLASSIC_LITHUANIAN_LITHUANIA,"lt" },
    
#ifdef TT_MS_LANGID_MAORI_NEW_ZELAND
    /* this seems to be an error that have been dropped */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MAORI_NEW_ZEALAND,		"mi" },
#endif
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FARSI_IRAN,		    "fa" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_VIETNAMESE_VIET_NAM,	"vi" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARMENIAN_ARMENIA,		"hy" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AZERI_AZERBAIJAN_LATIN,	"az" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AZERI_AZERBAIJAN_CYRILLIC,	"az" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BASQUE_SPAIN,		    "eu" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SORBIAN_GERMANY,		"wen" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MACEDONIAN_MACEDONIA,	"mk" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SUTU_SOUTH_AFRICA,		"st" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TSONGA_SOUTH_AFRICA,	"ts" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TSWANA_SOUTH_AFRICA,	"tn" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_VENDA_SOUTH_AFRICA,	"ven" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_XHOSA_SOUTH_AFRICA,	"xh" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ZULU_SOUTH_AFRICA,		"zu" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AFRIKAANS_SOUTH_AFRICA,"af" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GEORGIAN_GEORGIA,		"ka" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FAEROESE_FAEROE_ISLANDS,	"fo" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HINDI_INDIA,		    "hi" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALTESE_MALTA,		    "mt" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SAAMI_LAPONIA,		    "se" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SCOTTISH_GAELIC_UNITED_KINGDOM,"gd" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_IRISH_GAELIC_IRELAND,	"ga" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALAY_MALAYSIA,		    "ms" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALAY_BRUNEI_DARUSSALAM,	"ms" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KAZAK_KAZAKSTAN,		    "kk" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SWAHILI_KENYA,		        "sw" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_UZBEK_UZBEKISTAN_LATIN,	"uz" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_UZBEK_UZBEKISTAN_CYRILLIC,	"uz" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TATAR_TATARSTAN,		    "tt" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BENGALI_INDIA,		        "bn" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PUNJABI_INDIA,		        "pa" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GUJARATI_INDIA,	    	"gu" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ORIYA_INDIA,		        "or" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAMIL_INDIA,		        "ta" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TELUGU_INDIA,		        "te" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KANNADA_INDIA,		        "kn" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALAYALAM_INDIA,	    	"ml" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ASSAMESE_INDIA,	    	"as" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MARATHI_INDIA,		        "mr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SANSKRIT_INDIA,	    	"sa" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KONKANI_INDIA,		        "kok" },
    
    /* new as of 2001-01-01 */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_GENERAL,		"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_GENERAL,		"zh" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_GENERAL,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_WEST_INDIES,	"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_REUNION,		"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_CONGO,		    "fr" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_SENEGAL,		"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_CAMEROON,		"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_COTE_D_IVOIRE,	"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_MALI,		    "fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BOSNIAN_BOSNIA_HERZEGOVINA,"bs" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_URDU_INDIA,		    "ur" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAJIK_TAJIKISTAN,		"tg" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_YIDDISH_GERMANY,		"yi" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KIRGHIZ_KIRGHIZSTAN,	"ky" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TURKMEN_TURKMENISTAN,	"tk" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MONGOLIAN_MONGOLIA,	"mn" },
    
    /* the following seems to be inconsistent;
       here is the current "official" way: */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIBETAN_BHUTAN,		"bo" },
    /* and here is what is used by Passport SDK */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIBETAN_CHINA,		    "bo" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DZONGHKA_BHUTAN,		"dz" },
    /* end of inconsistency */
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_WELSH_WALES,		    "cy" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KHMER_CAMBODIA,		"km" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LAO_LAOS,			    "lo" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BURMESE_MYANMAR,		"my" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GALICIAN_SPAIN,		"gl" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MANIPURI_INDIA,		"mni" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SINDHI_INDIA,		    "sd" },
    /* the following one is only encountered in Microsoft RTF specification */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KASHMIRI_PAKISTAN,		"ks" },
    /* the following one is not in the Passport list, looks like an omission */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KASHMIRI_INDIA,		"ks" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NEPALI_NEPAL,		    "ne" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NEPALI_INDIA,	    	"ne" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRISIAN_NETHERLANDS,	"fy" },
    
    /* new as of 2001-03-01 (from Office Xp) */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_HONG_KONG,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_INDIA,		    "en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_MALAYSIA,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_SINGAPORE,		"en" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SYRIAC_SYRIA,		    "syr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SINHALESE_SRI_LANKA,	"si" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHEROKEE_UNITED_STATES,	"chr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_INUKTITUT_CANADA,		"iu" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AMHARIC_ETHIOPIA,		"am" },
#if 0
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAMAZIGHT_MOROCCO },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAMAZIGHT_MOROCCO_LATIN },
#endif
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PASHTO_AFGHANISTAN,	"ps" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FILIPINO_PHILIPPINES,	"phi" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DHIVEHI_MALDIVES,		"div" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_OROMO_ETHIOPIA,		"om" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIGRIGNA_ETHIOPIA,		"ti" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIGRIGNA_ERYTHREA,		"ti" },
    
    /* New additions from Windows Xp/Passport SDK 2001-11-10. */
    
    /* don't ask what this one means... It is commented out currently. */
#if 0
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GREEK_GREECE2 },
#endif
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_UNITED_STATES,	"es" },
    /* The following two IDs blatantly violate MS specs by using a */
    /* sublanguage >,.                                         */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_LATIN_AMERICA,	"es" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_NORTH_AFRICA,	"fr" },
    
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_MOROCCO,		"fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_HAITI,		    "fr" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BENGALI_BANGLADESH,	"bn" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PUNJABI_ARABIC_PAKISTAN,	"ar" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MONGOLIAN_MONGOLIA_MONGOLIAN,"mn" },
#if 0
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_EDO_NIGERIA },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FULFULDE_NIGERIA },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_IBIBIO_NIGERIA },
#endif
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HAUSA_NIGERIA,		    "ha" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_YORUBA_NIGERIA,		"yo" },
    /* language codes from, to, are (still) unknown. */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_IGBO_NIGERIA,		    "ibo" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KANURI_NIGERIA,		"kau" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GUARANI_PARAGUAY,		"gn" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HAWAIIAN_UNITED_STATES,"haw" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LATIN,			        "la" },
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SOMALI_SOMALIA,		"so" },
#if 0
    /* Note: Yi does not have a (proper) ISO 639-2 code, since it is mostly */
    /*       not written (but OTOH the peculiar writing system is worth     */
    /*       studying).                                                     */
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_YI_CHINA },
#endif
    {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PAPIAMENTU_NETHERLANDS_ANTILLES,"pap" },
};

#define NUM_FT_LANGUAGES  (sizeof (ftLanguages) / sizeof (ftLanguages[0]))


template <typename T> T
swap2(T value) {
    return static_cast<T>(((value & 0xff) << 8) + ((value & 0xff00) >> 8));
}


static FXString 
FXUTF16BE2UTF8(void * buf, size_t bufLen) {
    if (!buf || !bufLen) return FXString{};
    const uint16_t * u16Buf = reinterpret_cast<const uint16_t*>(buf);
    const size_t u16Len = bufLen / 2;
    FXVector<uint16_t> v;
    v.reserve(u16Len);
    for (uint32_t i = 0; i < u16Len; i++)
        v.push_back(swap2(u16Buf[i]));
    icu::UnicodeString uStr(&v[0], static_cast<int32_t>(u16Len));
    FXString u8;
    return uStr.toUTF8String(u8);
}

[[maybe_unused]] static FXString 
FXUTF16BE2UTF8(const uint16_t * u16Buf, size_t u16Len) {
    if (!u16Buf || !u16Len) return FXString{};
    FXVector<uint16_t> v;
    v.reserve(u16Len);
    for (uint32_t i = 0; i < u16Len; i++)
        v.push_back(swap2(u16Buf[i]));
    icu::UnicodeString uStr(&v[0], static_cast<int32_t>(u16Len));
    FXString u8;
    return uStr.toUTF8String(u8);
}

static FXString 
FXUTF162UTF8(void * buf, size_t bufLen) {
    if (!buf || !bufLen) return FXString{};
    const uint16_t * u16Buf = reinterpret_cast<const uint16_t*>(buf);
    const size_t u16Len = bufLen / 2;
    icu::UnicodeString uStr(u16Buf, static_cast<int32_t>(u16Len));
    FXString u8;
    return uStr.toUTF8String(u8);
}

static FXString 
FXUTF162UTF8(const uint16_t * u16Buf, size_t u16Len) {
    if (!u16Buf || !u16Len) return FXString{};
    icu::UnicodeString uStr(u16Buf, static_cast<int32_t>(u16Len));
    FXString u8;
    return uStr.toUTF8String(u8);
}

[[maybe_unused]] static FXString
FXMacRoman2UTF8(const unsigned char * buffer, size_t bufferLen) {
    static const uint16_t uni[128] = {
        /* 0x80 */
        0x00c4, 0x00c5, 0x00c7, 0x00c9, 0x00d1, 0x00d6, 0x00dc, 0x00e1,
        0x00e0, 0x00e2, 0x00e4, 0x00e3, 0x00e5, 0x00e7, 0x00e9, 0x00e8,
        /* 0x90 */
        0x00ea, 0x00eb, 0x00ed, 0x00ec, 0x00ee, 0x00ef, 0x00f1, 0x00f3,
        0x00f2, 0x00f4, 0x00f6, 0x00f5, 0x00fa, 0x00f9, 0x00fb, 0x00fc,
        /* 0xa0 */
        0x2020, 0x00b0, 0x00a2, 0x00a3, 0x00a7, 0x2022, 0x00b6, 0x00df,
        0x00ae, 0x00a9, 0x2122, 0x00b4, 0x00a8, 0x2260, 0x00c6, 0x00d8,
        /* 0xb0 */
        0x221e, 0x00b1, 0x2264, 0x2265, 0x00a5, 0x00b5, 0x2202, 0x2211,
        0x220f, 0x03c0, 0x222b, 0x00aa, 0x00ba, 0x2126, 0x00e6, 0x00f8,
        /* 0xc0 */
        0x00bf, 0x00a1, 0x00ac, 0x221a, 0x0192, 0x2248, 0x2206, 0x00ab,
        0x00bb, 0x2026, 0x00a0, 0x00c0, 0x00c3, 0x00d5, 0x0152, 0x0153,
        /* 0xd0 */
        0x2013, 0x2014, 0x201c, 0x201d, 0x2018, 0x2019, 0x00f7, 0x25ca,
        0x00ff, 0x0178, 0x2044, 0x00a4, 0x2039, 0x203a, 0xfb01, 0xfb02,
        /* 0xe0 */
        0x2021, 0x00b7, 0x201a, 0x201e, 0x2030, 0x00c2, 0x00ca, 0x00c1,
        0x00cb, 0x00c8, 0x00cd, 0x00ce, 0x00cf, 0x00cc, 0x00d3, 0x00d4,
        /* 0xf0 */
        0xfffd, 0x00d2, 0x00da, 0x00db, 0x00d9, 0x0131, 0x02c6, 0x02dc,
        0x00af, 0x02d8, 0x02d9, 0x02da, 0x00b8, 0x02dd, 0x02db, 0x02c7,
    };

    FXVector<uint16_t> u16(bufferLen);
    for (size_t i = 0; i < bufferLen; ++ i) {
        if (buffer[i] < 0x80)
            u16[i] = buffer[i];
        else
            u16[i] = uni[buffer[i] - 0x80];
    }
    return FXUTF162UTF8(&u16[0], u16.size());
}

static FXString
FXIconv(const FXString & encoding, const char * buffer, size_t bufferLen) {
    // byte 0 is not valid in MBS, so remove all byte 0
    char * trimed = (char *)malloc(bufferLen);
    size_t trimedLen = 0;
    for (size_t i = 0; i < bufferLen; ++ i) {
        char c = buffer[i];
        if (c)
            trimed[trimedLen++] = c;
    }
    
    constexpr size_t BLOCK_SIZE = 128;
    size_t outLen = BLOCK_SIZE;
    char * outBuf = reinterpret_cast<char*>(malloc(outLen));
    
    do {
        iconv_t cd = iconv_open("UTF-8", encoding.c_str());
        if (cd == reinterpret_cast<iconv_t>(-1)) {
            free(outBuf);
            return FXString();
        }
        char * inBuf = const_cast<char*>(trimed);
        size_t inLen = trimedLen;
        
        size_t outLenIconv = outLen;
        char * outBufIconv = outBuf;
        size_t ret = iconv(cd, &inBuf, &inLen, &outBufIconv, &outLenIconv);
        iconv_close(cd);

        if (ret == -1 && errno == E2BIG)  {
            // Need more memory
            outLen += BLOCK_SIZE;
            outBuf = reinterpret_cast<char*>(realloc(outBuf, outLen));
        } else if (ret == 0) {
            // OK
            outLen = outLen - outLenIconv;
            break;
        }
        else {
            // Error
            free(outBuf);
            return FXString();
        }
    } while (true);

    FXString ret(outBuf, outLen);
    free(outBuf);
    free(trimed);
    return ret;
}

std::string
FXGetPlatformName(FT_UShort platformId) {
    switch (platformId) {
        case TT_PLATFORM_APPLE_UNICODE: return  "Unicode";
        case TT_PLATFORM_MACINTOSH: return "Macintosh";
        case TT_PLATFORM_ISO: return "ISO";
        case TT_PLATFORM_MICROSOFT: return "Microsoft";
        case TT_PLATFORM_ADOBE: return "Adobe";
        default: return "Unknown Platform";
    }
}

std::string
FXGetEncodingName(FT_UShort platformId, FT_UShort encodingId) {

    std::map<FT_UShort, std::string> encodings;
    switch (platformId) {
        case TT_PLATFORM_APPLE_UNICODE:
            encodings =  {
                {TT_APPLE_ID_DEFAULT,                  "1.0"},
                {TT_APPLE_ID_UNICODE_1_1,              "1.1"},
                {TT_APPLE_ID_ISO_10646,                "ISO/IEC 10646"},
                {TT_APPLE_ID_UNICODE_2_0,              "2.0, BMP only"},
                {TT_APPLE_ID_UNICODE_32,               "2.0, full repertoire"},
                {TT_APPLE_ID_VARIANT_SELECTOR,         "Variation Sequences"},
                {6,                                    "full repertoire"},
            };
            break;

        case TT_PLATFORM_MACINTOSH:
            encodings = {
                {TT_MAC_ID_ROMAN,                       "Roman"},
                {TT_MAC_ID_JAPANESE,                    "Japanese"},
                {TT_MAC_ID_TRADITIONAL_CHINESE,         "Chinese (Traditional)"},
                {TT_MAC_ID_KOREAN,                      "Korean"},
                {TT_MAC_ID_ARABIC,                      "Arabic"},
                {TT_MAC_ID_HEBREW,                      "Hebrew"},
                {TT_MAC_ID_GREEK,                       "Greek"},
                {TT_MAC_ID_RUSSIAN,                     "Russian"},
                {TT_MAC_ID_RSYMBOL,                     "RSymbol"},
                {TT_MAC_ID_DEVANAGARI,                  "Devanagari"},
                {TT_MAC_ID_GURMUKHI,                    "Gurmukhi"},
                {TT_MAC_ID_GUJARATI,                    "Gujarati"},
                {TT_MAC_ID_ORIYA,                       "Oriya"},
                {TT_MAC_ID_BENGALI,                     "Bengali"},
                {TT_MAC_ID_TAMIL,                       "Tamil"},
                {TT_MAC_ID_TELUGU,                      "Telugu"},
                {TT_MAC_ID_KANNADA,                     "Kannada"},
                {TT_MAC_ID_MALAYALAM,                   "Malayalam"},
                {TT_MAC_ID_SINHALESE,                   "Sinhalese"},
                {TT_MAC_ID_BURMESE,                     "Burmese"},
                {TT_MAC_ID_KHMER,                       "Khmer"},
                {TT_MAC_ID_THAI,                        "Thai"},
                {TT_MAC_ID_LAOTIAN,                     "Laotian"},
                {TT_MAC_ID_GEORGIAN,                    "Georgian"},
                {TT_MAC_ID_ARMENIAN,                    "Armenian"},
                {TT_MAC_ID_SIMPLIFIED_CHINESE,          "Chinese (Simplified)"},
                {TT_MAC_ID_TIBETAN,                     "Tibetan"},
                {TT_MAC_ID_MONGOLIAN,                   "Mongolian"},
                {TT_MAC_ID_GEEZ,                        "Geez"},
                {TT_MAC_ID_SLAVIC,                      "Slavic"},
                {TT_MAC_ID_VIETNAMESE,                  "Vietnamese"},
                {TT_MAC_ID_SINDHI,                      "Sindhi"},
                {TT_MAC_ID_UNINTERP,                    "Uninterpreted"},
            };
            break;
            
        case TT_PLATFORM_ISO:
            encodings = {
                {TT_ISO_ID_7BIT_ASCII,                  "7-bit ASCII"},
                {TT_ISO_ID_10646,                       "10646"},
                {TT_ISO_ID_8859_1,                      "8859-1"},
            };
            break;
            
        case TT_PLATFORM_MICROSOFT:
            encodings = {
                {TT_MS_ID_SYMBOL_CS,                    "Symbol"},
                {TT_MS_ID_UNICODE_CS,                   "Unicode BMP"},
                {TT_MS_ID_SJIS,                         "ShiftJIS"},
                {TT_MS_ID_PRC,                          "RPC"},
                {TT_MS_ID_BIG_5,                        "Big5"},
                {TT_MS_ID_WANSUNG,                      "Wansung"},
                {TT_MS_ID_JOHAB,                        "Johab"},
                {7,                                     "Reserved"},
                {8,                                     "Reserved"},
                {9,                                     "Reserved"},
                {TT_MS_ID_UCS_4,                        "Unicode UCS-4"},
            };
            break;
        case TT_PLATFORM_ADOBE:
            encodings = {
                {TT_ADOBE_ID_STANDARD,                  "Standard"},
                {TT_ADOBE_ID_EXPERT,                    "Expert"},
                {TT_ADOBE_ID_CUSTOM,                    "Custom"},
                {TT_ADOBE_ID_LATIN_1,                   "Latin 1"},
            };
            break;

        default:
            break;
    }

    auto itr = encodings.find(encodingId);
    if (itr != encodings.end())
        return itr->second;
    
    return "Unknown Encoding";
}

FXString
FXGetLanguageName(FT_UShort platformId, FT_UShort languageId) {
    for (size_t i = 0; i < NUM_FT_LANGUAGES; i++)
        if (ftLanguages[i].platform_id == platformId &&
            (ftLanguages[i].language_id == TT_LANGUAGE_DONT_CARE ||
             ftLanguages[i].language_id == languageId))
        {
            if (ftLanguages[i].lang[0] == '\0')
                return FXString();
            else
                return ftLanguages[i].lang;
        }
    return FXString();
}


static FXString
FXFTStringMacintosh(FT_UShort encodingId, void * string, uint32_t stringLen) {
    if (encodingId == TT_MAC_ID_ROMAN)
        return FXIconv("MacRoman", reinterpret_cast<const char *>(string), stringLen);
        //return FXMacRoman2UTF8(reinterpret_cast<const unsigned char *>(string), stringLen);

#if FX_MAC
    FXString name;
    while (true) {
        OSStatus error = 0;
        TextEncoding textEncoding;
        error = UpgradeScriptInfoToTextEncoding(encodingId,
                                                kTextLanguageDontCare,
                                                kTextRegionDontCare,
                                                NULL,
                                                &textEncoding);
        if (error) break;
        
        TextToUnicodeInfo textToUnicodeInfo;
        error = CreateTextToUnicodeInfoByEncoding(textEncoding, &textToUnicodeInfo);
        if (error) break;
        
        ByteCount sourceRead = 0, unicodeLen = 0;
        
        int bufLen = stringLen * 4;
        UniChar * buf = (UniChar*)malloc(bufLen * sizeof(UniChar));
        error = ConvertFromTextToUnicode(textToUnicodeInfo,
                                         stringLen, string, 
                                         0, 0, 0, 0, 0, // no font offset
                                         bufLen, &sourceRead, &unicodeLen,
                                         buf);
        
        if (!error)
            name = FXUTF162UTF8((void*)buf, unicodeLen);
        
        DisposeTextToUnicodeInfo(&textToUnicodeInfo);
        free(buf);
        break;
    }
    
    return name;
#else
    return FXString();
#endif
}



static FXString 
FXFTStringAppleUnicode(FT_UShort /*encodingId*/, void * buf, uint32_t bufLen) {
    return FXUTF16BE2UTF8(buf, bufLen);
}


static FXString
FXFTStringMicrosoft(FT_UShort encodingId, void * buf, uint32_t bufLen) {
    switch (encodingId)
    {
    case TT_MS_ID_UNICODE_CS:
    case TT_MS_ID_SYMBOL_CS: 
        return FXUTF16BE2UTF8(buf, bufLen);
    case TT_MS_ID_PRC:
        return FXIconv("GB18030", reinterpret_cast<const char *>(buf), bufLen);
    case TT_MS_ID_SJIS:
        return FXIconv("SHIFT-JIS", reinterpret_cast<const char *>(buf), bufLen);
    case TT_MS_ID_BIG_5:
        return FXIconv("BIGFIVE", reinterpret_cast<const char *>(buf), bufLen);
    default:
        return FXString();
    }
}

FXString
FXToString(FT_UShort platformId, FT_UShort encodingId, void * string, uint32_t stringLen) {
    FXString str;
    switch(platformId) {
    case TT_PLATFORM_MACINTOSH: str = FXFTStringMacintosh(encodingId, string, stringLen); break;
    case TT_PLATFORM_MICROSOFT: str = FXFTStringMicrosoft(encodingId, string, stringLen); break;
    case TT_PLATFORM_APPLE_UNICODE: str = FXFTStringAppleUnicode(encodingId, string, stringLen); break;
    default: break;
    }
    //if (str.empty())
    //    return FXString((const char * )string, stringLen);
    return str;
}

FXString
FXSFNTGetName(FT_SfntName * sfnt) {
    const char * names[] = {
        "Copyright",
        "Font Family",
        "Font Subfamily",
        "Unique ID",
        "Full Name",
        "Version",
        "PS Name",
        "Trademark",
        "Manufacturer",
        "Designer",
        "Description",
        "Vendor URL",
        "Designer URL",
        "License",
        "License URL",
        "15",
        "Typographic Family",
        "Typographic Subfamily",
        "Mac Fullname",
        "Sample Text",
        "CID Findfont Name",
        "WWS Family",
        "WWS Subfamily",
        "Light Background",
        "Dark Background",
        "Variations Prefix",
    };
    
    if (sfnt->name_id >= sizeof(names)/sizeof(names[0]))
        return std::to_string(sfnt->name_id);
    return names[sfnt->name_id];
}

FXString
FXSFNTGetValue(FT_SfntName * sfnt) {
    return FXToString(sfnt->platform_id,
                        sfnt->encoding_id,
                        sfnt->string,
                        sfnt->string_len);
}

FXString
FXSFNTGetLanguage(FT_SfntName *sfnt, FT_Face face) {
    if (sfnt->language_id >= 0x8000) {
        FT_SfntLangTag langTag;
        if (!FT_Get_Sfnt_LangTag(face, sfnt->language_id, &langTag)) {
            return FXToString(sfnt->platform_id,
                                sfnt->encoding_id,
                                langTag.string,
                                langTag.string_len);
        }
    }
    return FXGetLanguageName(sfnt->platform_id, sfnt->language_id);
}


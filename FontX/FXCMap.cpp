#include "FXFace.h"
#include "FXCMap.h"
#include "FXUnicode.h"
#include "FXFTPrivate.h"
#include "FXFTNames.h"

namespace {
    constexpr FXChar UNDEFINED_CHAR_MARK = FXCharInvalid;
    constexpr FXChar EXTRA_CHARS_MARK = UNDEFINED_CHAR_MARK + 1;
    constexpr FXChar NOT_INIT_MARK = EXTRA_CHARS_MARK + 1;
}

std::vector<FXCMapPlatform> FXCMapPlatform::platforms_;
std::vector<FXPtr<FXCharBlock> > FXCMapPlatform::unicodeBlocks_;

const std::vector<FXCMapPlatform> &
FXCMapPlatform::availablePlatforms() {
    if (platforms_.empty()) {
        platforms_.push_back(TT_PLATFORM_APPLE_UNICODE);
        platforms_.push_back(TT_PLATFORM_MACINTOSH);
        platforms_.push_back(TT_PLATFORM_ISO);
        platforms_.push_back(TT_PLATFORM_MICROSOFT);
        platforms_.push_back(TT_PLATFORM_ADOBE);
    }
    return platforms_;
}

const FXCMapPlatform &
FXCMapPlatform::get(uint16_t platformID) {
    for (const auto & platform : availablePlatforms())
        if (platform.platformID_ == platformID)
            return platform;

    return get(TT_PLATFORM_APPLE_UNICODE);
}

const std::vector<FXPtr<FXCharBlock> > &
FXCMapPlatform::blocks(uint16_t encodingID) const {
    auto itr = blocksMap_.find(encodingID);
    if (itr != blocksMap_.end())
        return itr->second;


    static std::vector<FXPtr<FXCharBlock> > empty;
    return empty;
}

FXCMapPlatform::FXCMapPlatform(uint16_t platformID)
    : platformID_(platformID) {
    initEncodings();
}

void
FXCMapPlatform::initEncodings() {
    switch(platformID_) {
        case TT_PLATFORM_APPLE_UNICODE: initUnicodeEncoding(); break;
        case TT_PLATFORM_MACINTOSH: initMacintoshEncoding(); break;
        case TT_PLATFORM_ISO: initISOEncoding(); break;
        case TT_PLATFORM_MICROSOFT: initMicrosoftEncoding(); break;
        case TT_PLATFORM_ADOBE: initAdobeEncoding(); break;
        default: initUnicodeEncoding(); break;
    }
}

void
FXCMapPlatform::initUnicodeEncoding() {
    auto unicodeBlocks = getUnicodeBlocks();
    uint16_t encodings[] = {TT_APPLE_ID_DEFAULT,
                            TT_APPLE_ID_UNICODE_1_1,
                            TT_APPLE_ID_ISO_10646,
                            TT_APPLE_ID_UNICODE_2_0,
                            TT_APPLE_ID_UNICODE_32,
                            TT_APPLE_ID_VARIANT_SELECTOR,
                            TT_APPLE_ID_FULL_UNICODE};
    for (auto encoding : encodings)
        blocksMap_[encoding] = unicodeBlocks;
}

void
FXCMapPlatform::initMacintoshEncoding() {
    blocksMap_[TT_MAC_ID_ROMAN].push_back(std::make_shared<FXCharRangeBlock>(0, 255, "Mac Roman", false));
}

void
FXCMapPlatform::initISOEncoding() {
    initUnicodeEncoding();
}

void
FXCMapPlatform::initMicrosoftEncoding() {
    blocksMap_[TT_MS_ID_UNICODE_CS] = getUnicodeBlocks();
    blocksMap_[TT_MS_ID_UCS_4] = getUnicodeBlocks();
    blocksMap_[TT_MS_ID_SYMBOL_CS].push_back(std::make_shared<FXCharRangeBlock>(0xF020, 0xF0FF, "Windows Symbol"));
}

void
FXCMapPlatform::initAdobeEncoding() {
    blocksMap_[TT_ADOBE_ID_STANDARD].push_back(std::make_shared<FXCharRangeBlock>(0, 255, "Standard", false));
    blocksMap_[TT_ADOBE_ID_EXPERT].push_back(std::make_shared<FXCharRangeBlock>(0, 255, "Expert", false));
    blocksMap_[TT_ADOBE_ID_CUSTOM].push_back(std::make_shared<FXCharRangeBlock>(0, 255, "Custom", false));
    blocksMap_[TT_ADOBE_ID_LATIN_1].push_back(std::make_shared<FXCharRangeBlock>(0, 255, "Latin 1", false));
}

const std::vector<FXPtr<FXCharBlock> > &
FXCMapPlatform::getUnicodeBlocks() {
    if (unicodeBlocks_.size())
        return unicodeBlocks_;

    for (const FXUCDBlock & ucdBlock : FXUnicode::blocks()) 
        unicodeBlocks_.push_back(std::make_shared<FXCharRangeBlock>(
                                     ucdBlock.from,
                                     ucdBlock.to,
                                     ucdBlock.name,
                                     true));
    return unicodeBlocks_;
}

//////////////////////////////////////////////////////////////////////////////////////////
//            FXCMAP
FXCMap::FXCMap(FXFace * face, uint16_t platformID, uint16_t encodingID, size_t index)
    : face_(face)
    , index_(index)
    , platformID_(platformID)
    , encodingID_(encodingID) {
    glyphMap_.push_back(NOT_INIT_MARK);
}

uint16_t
FXCMap::platformID() const {
    return platformID_;
}

uint16_t
FXCMap::encodingID() const {
    return encodingID_;
}

size_t
FXCMap::index() const {
    return index_;
}

bool
FXCMap::isCurrent() const {
    return face_->currentCMapIndex() == index_;
}

bool
FXCMap::setCurrent() const {
    return face_->selectCMap(index_);
}
    
std::string
FXCMap::platformName() const {
    return FXGetPlatformName(platformID_);
}
    
std::string
FXCMap::encodingName() const {
    return FXGetEncodingName(platformID_, encodingID_);
}

std::string
FXCMap::description() const {
    return FXGetPlatformName(platformID_) + " - " + FXGetEncodingName(platformID_, encodingID_);
}

bool
FXCMap::isUnicode() const {
    return platformID_ == TT_PLATFORM_APPLE_UNICODE
        || (platformID_ == TT_PLATFORM_MICROSOFT && (encodingID_ == TT_MS_ID_UNICODE_CS || encodingID_ == TT_MS_ID_UCS_4))
        ;
}

const FXVector<FXPtr<FXCharBlock> > & 
FXCMap::blocks() const {
    auto & fullBlocks = FXCMapPlatform::get(platformID_).blocks(encodingID_);
    if (fullBlocks.empty()) {
        static std::vector<FXPtr<FXCharBlock> > nilVec;
        if (nilVec.empty()) {
            FXPtr<FXCharBlock> nilBlock(new FXNullCharBlock("NIL"));
            nilVec.push_back(nilBlock);
        }
        return nilVec;
    }
    return fullBlocks;
}

FXVector<FXChar>
FXCMap::charsForGlyph(FXGlyphID gid) const {
    FXVector<FXChar> ret;
        
    if (glyphMap_.size() && (glyphMap_[0] == NOT_INIT_MARK)) {
        if (!const_cast<FXCMap*>(this)->initGlyphsMap())
            return ret;
    }

    FXChar c = glyphMap_[gid];
    if (c == UNDEFINED_CHAR_MARK)
        return ret;
    else if (c == EXTRA_CHARS_MARK) {
        auto itr = extraGlyphsMap_.find(gid);
        if (itr != extraGlyphsMap_.end())
            return itr->second;
        else
            return ret;
    }
    else {
        ret.push_back(c);
        return ret;
    }
}

FXGlyphID
FXCMap::glyphForChar(FXChar c) const {
    assert(isCurrent());
    // FIXME: remove requirement of isCurrent
    return FT_Get_Char_Index(face0(), c);
}

bool
FXCMap::initGlyphsMap() {
    glyphMap_.clear();
    
    FXAutoCMap acm(face_, index_);
    if (!acm.ok())
        return false;
    
    assert(isCurrent());
    
    FXFTFace face = face0();
    glyphMap_ = FXVector<FXChar>(face->num_glyphs, UNDEFINED_CHAR_MARK);
    FT_UInt gid = 0;
    FT_ULong ch = FT_Get_First_Char(face, &gid);
    while (gid != 0) {
        FXChar c = glyphMap_[gid];
        if (c == UNDEFINED_CHAR_MARK)
            glyphMap_[gid] = ch;
        else {
            glyphMap_[gid] = EXTRA_CHARS_MARK;
            extraGlyphsMap_[gid].push_back(ch);
        }
        ch = FT_Get_Next_Char(face, ch, &gid);
    }
    return true;
}
    
FXFTFace
FXCMap::face0() const {
    return face_->face();
}

FXAutoCMap::FXAutoCMap(FXFace * face, size_t index)
    : face_(face)
    , index_(index)
    , ok_(true) {
    oldIndex_ = face_->currentCMapIndex();
    if (index != oldIndex_) 
        ok_ = face_->selectCMap(index);
}

size_t
FXAutoCMap::index() const {
    return index_;
}
    
bool
FXAutoCMap::ok() const {
    return ok_;
}

FXAutoCMap::~FXAutoCMap() {
    if (ok_) 
        face_->selectCMap(oldIndex_);
}



#include <algorithm>
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
std::vector<FXPtr<FXGCharBlock> > FXCMapPlatform::unicodeBlocks_;

size_t
FXCharArrayBlock::index(const FXGChar & c) const {
    auto itr = std::lower_bound(chs_.begin(), chs_.end(), c, [](const FXGChar & v, const FXGChar & c) {
        return (v.type == c.type) && (v.value < c.value);
    });
    if (itr != chs_.end() && itr->type == c.type && itr->value == c.value)
        return std::distance(chs_.begin(), itr);
    return -1;
}

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

const std::vector<FXPtr<FXGCharBlock> > &
FXCMapPlatform::blocks(uint16_t encodingID) const {
    auto itr = blocksMap_.find(encodingID);
    if (itr != blocksMap_.end())
        return itr->second;


    static std::vector<FXPtr<FXGCharBlock> > empty;
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
    blocksMap_[TT_MAC_ID_ROMAN].push_back(std::make_shared<FXCharRangeBlock>(0, 256, FXGCharTypeOther, "Mac Roman"));
}

void
FXCMapPlatform::initISOEncoding() {
    initUnicodeEncoding();
}

void
FXCMapPlatform::initMicrosoftEncoding() {
    blocksMap_[TT_MS_ID_UNICODE_CS] = getUnicodeBlocks();
    blocksMap_[TT_MS_ID_UCS_4] = getUnicodeBlocks();
    blocksMap_[TT_MS_ID_SYMBOL_CS] = getUnicodeBlocks();//.push_back(std::make_shared<FXCharRangeBlock>(0xF020, 0xF0FF + 1, FXGCharTypeUnicode, "Windows Symbol"));
}

void
FXCMapPlatform::initAdobeEncoding() {
    blocksMap_[TT_ADOBE_ID_STANDARD].push_back(std::make_shared<FXCharRangeBlock>(0, 256, FXGCharTypeOther, "Standard"));
    blocksMap_[TT_ADOBE_ID_EXPERT].push_back(std::make_shared<FXCharRangeBlock>(0, 256, FXGCharTypeOther, "Expert"));
    blocksMap_[TT_ADOBE_ID_CUSTOM].push_back(std::make_shared<FXCharRangeBlock>(0, 256, FXGCharTypeOther, "Custom"));
    blocksMap_[TT_ADOBE_ID_LATIN_1].push_back(std::make_shared<FXCharRangeBlock>(0, 256, FXGCharTypeOther, "Latin 1"));
}

const std::vector<FXPtr<FXGCharBlock> > &
FXCMapPlatform::getUnicodeBlocks() {
    if (unicodeBlocks_.size())
        return unicodeBlocks_;

    for (const FXUCDBlock & ucdBlock : FXUnicode::blocks()) 
        unicodeBlocks_.push_back(std::make_shared<FXCharRangeBlock>(
                                     ucdBlock.from,
                                     ucdBlock.to,
                                     FXGCharTypeUnicode,
                                     ucdBlock.name));
    return unicodeBlocks_;
}

//////////////////////////////////////////////////////////////////////////////////////////
//            FXCMAP
FXCMap::FXCMap(FXFace * face, uint16_t platformID, uint16_t encodingID, size_t index, bool valid)
    : face_(face)
    , index_(index)
    , platformID_(platformID)
    , encodingID_(encodingID)
    , valid_(valid) {
    glyphMap_.push_back(NOT_INIT_MARK);
}

FXCMap::FXCMap(FXFace * face, const FXString & cidROS, size_t index, bool valid) 
    : face_(face)
    , index_(index)
    , cidROS_(cidROS)
    , valid_(valid) {
}

uint16_t
FXCMap::platformID() const {
    return platformID_;
}

uint16_t
FXCMap::encodingID() const {
    return encodingID_;
}

const FXString &
FXCMap::cidROS() const {
    return cidROS_;
}

size_t
FXCMap::index() const {
    return index_;
}

bool
FXCMap::isValid() const {
    return valid_;
}

bool
FXCMap::isCID() const {
    return !cidROS_.empty();
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
    if (isCID())
        return cidROS_;
    else
        return FXGetPlatformName(platformID_) + " - " + FXGetEncodingName(platformID_, encodingID_);
}

bool
FXCMap::isUnicode() const {
    if (isCID())
        return false;

    return platformID_ == TT_PLATFORM_APPLE_UNICODE
        || (platformID_ == TT_PLATFORM_MICROSOFT && (encodingID_ == TT_MS_ID_UNICODE_CS || encodingID_ == TT_MS_ID_UCS_4 || encodingID_ == TT_MS_ID_SYMBOL_CS))
        ;
}

bool
FXCMap::isSymbol() const {
    return platformID_ == TT_PLATFORM_MICROSOFT && encodingID_ == TT_MS_ID_SYMBOL_CS;
}

const FXVector<FXPtr<FXGCharBlock> > & 
FXCMap::blocks() const {
    if (blocks_.empty())
        const_cast<FXCMap*>(this)->initBlocks();
    
    return blocks_;
}

const FXVector<FXPtr<FXGCharBlock> > &
FXCMap::fullUnicodeBlocks() const {
    assert(isUnicode());
    return FXCMapPlatform::get(platformID_).blocks(encodingID_);
}
    
FXVector<FXChar>
FXCMap::charsForGlyph(FXGlyphID gid) const {
    if (isCID()) {
        return {FXChar(gid)}; // GID is CID
    }
    
    FXVector<FXChar> ret;
        
    if (glyphMap_.size() && (glyphMap_[0] == NOT_INIT_MARK)) {
        if (!const_cast<FXCMap*>(this)->initGlyphsMap())
            return ret;
    }

    // can't init glyph map, invalid cmap?
    if (glyphMap_.empty())
        return ret;
    
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
    if (isCID()) {
        return FXGlyphID(c); // CID is GID
    }
    
    auto itr = std::lower_bound(charMap_.begin(), charMap_.end(), c, [](const FXCharMapItem & item, FXChar c) {
        return item.c < c;
    });
    if (itr != charMap_.end() && (c == itr->c)) 
        return itr->g;
    return FXGIDNotDef;
}


void
FXCMap::initBlocks() {
    if (isCID()) {
        FXPtr<FXGCharBlock> nilBlock(new FXCharRangeBlock(0, face_->glyphCount(), FXGCharType::FXGCharTypeGlyphID, cidROS_));
        blocks_.push_back(nilBlock);
        return;
    }

    auto & fullBlocks = FXCMapPlatform::get(platformID_).blocks(encodingID_);
    if (fullBlocks.empty()) {
        //new FXCharRangeBlock(0, face_->glyphCount(), FXGCharType::FXGCharTypeGlyphID, "Glyphs");
        FXPtr<FXGCharBlock> nilBlock(new FXCharRangeBlock(0, face_->glyphCount(), FXGCharType::FXGCharTypeGlyphID, "All Glyphs"));
        blocks_.push_back(nilBlock);
    } else if (!isUnicode()) {
        blocks_ = fullBlocks;
    }
    else {
        // ensure charMap_ is properly initialized.
        charsForGlyph(0);
        
        FXVector<bool> charFound(face0()->num_glyphs);
        FXVector<FXPtr<FXGCharBlock> > uniCompactBlocks;
        FXVector<FXGChar> chs;
        size_t currentUniBlockIndex = 0;
        
        for (const auto & it : charMap_) {
            charFound[it.g] = true;
            FXGChar c {it.c, FXGCharTypeUnicode};
            FXPtr<FXGCharBlock> currentUniBlock = fullBlocks[currentUniBlockIndex];
            if (currentUniBlock->contains(c)) {
                chs.push_back(c);
            }
            else {
                if (chs.size()) {
                    FXPtr<FXCharArrayBlock> newBlock(new FXCharArrayBlock(chs, currentUniBlock->name()));
                    uniCompactBlocks.push_back(newBlock);
                }
                // find next Unicode block
                size_t uniBlockIndex = currentUniBlockIndex + 1;
                while (uniBlockIndex < fullBlocks.size() && (!fullBlocks[uniBlockIndex]->contains(c)))
                    ++ uniBlockIndex;
                // can't find the next block
                if (uniBlockIndex == fullBlocks.size())
                    break;
                
                chs.clear();
                chs.push_back(c);
                currentUniBlockIndex = uniBlockIndex;
            }
        }

        if (currentUniBlockIndex < fullBlocks.size() && chs.size()) {
            FXPtr<FXGCharBlock> currentUniBlock = fullBlocks[currentUniBlockIndex];
            FXPtr<FXCharArrayBlock> newBlock(new FXCharArrayBlock(chs, currentUniBlock->name()));
            uniCompactBlocks.push_back(newBlock);
        }

        // for glyphs which are not in the charMap
        chs.clear();
        for (size_t gid = 0; gid < charFound.size(); ++ gid) {
            if (!charFound[gid])
                chs.push_back({(FXChar)gid, FXGCharTypeGlyphID});
        }
        FXPtr<FXCharArrayBlock> newBlock(new FXCharArrayBlock(chs, "Unassigned"));
        uniCompactBlocks.push_back(newBlock); 

        blocks_ = uniCompactBlocks;
    }
}

bool
FXCMap::initGlyphsMap() {
    if (isCID())
        return true;
    
    glyphMap_.clear();
    
    FXAutoCMap acm(face_, index_);
    if (!acm.ok())
        return false;
    
    assert(isCurrent());
    
    FXFTFace face = face0();
    glyphMap_ = FXVector<FXChar>(face->num_glyphs, UNDEFINED_CHAR_MARK);
    charMap_.reserve(face->num_glyphs);
    FT_UInt gid = 0;

    // ch in order
    FT_ULong ch = FT_Get_First_Char(face, &gid);
    while (gid != 0) {
        FXChar c = glyphMap_[gid];
        if (c == UNDEFINED_CHAR_MARK)
            glyphMap_[gid] = ch;
        else {
            glyphMap_[gid] = EXTRA_CHARS_MARK;
            extraGlyphsMap_[gid].push_back(ch);
        }
        charMap_.emplace_back(ch, gid);
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



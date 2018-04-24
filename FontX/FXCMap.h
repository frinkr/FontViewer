#pragma once
#include <unordered_map>
#include "FX.h"
class FXFace;

class FXCharBlock {
public:
    explicit FXCharBlock(const FXString & name)
        : name_(name) {}
    virtual ~FXCharBlock() {}

    virtual std::string
    name() const {
        return name_;
    }
    
    virtual size_t
    size() const = 0;

    virtual FXChar
    get(size_t index) const = 0;
protected:
    FXString   name_;
};

class FXNullCharBlock : public FXCharBlock {
public:
    using FXCharBlock::FXCharBlock;
    virtual size_t
    size() const {
        return 0;
    }

    virtual FXChar
    get(size_t) const {
        return FXCharInvalid;
    }
};

class FXCharRangeBlock : public FXCharBlock {
public:
    FXCharRangeBlock(FXChar from, FXChar to, const std::string & name, bool isUnicode = true)
        : FXCharBlock(name)
        , range_{from, to}
        , isUnicode_(isUnicode) {}

    virtual size_t
    size() const {
        return range_.to - range_.from + 1;
    }

    virtual FXChar
    get(size_t index) const {
        return static_cast<FXChar>(index + range_.from);
    }

    const FXCharRange &
    range() const {
        return range_;
    }

    bool
    isUnicode() const {
        return isUnicode_;
    }
    
protected:
    FXCharRange    range_;
    bool           isUnicode_;
};

class FXCharListBlock : public FXCharBlock {
public:
    using FXCharBlock::FXCharBlock;

    virtual size_t
    size() const {
        return chs_.size();
    }

    virtual FXChar
    get(size_t index) const {
        return chs_[index];
    }
protected:
    FXVector<FXChar>  chs_;
};

class FXCMapPlatform {
public:
    static const std::vector<FXCMapPlatform> &
    availablePlatforms();

    static const FXCMapPlatform &
    get(uint16_t platformID);

private:
    static std::vector<FXCMapPlatform> platforms_;
    static std::vector<FXPtr<FXCharBlock> > unicodeBlocks_;
public:
    const std::vector<FXPtr<FXCharBlock> > &
    blocks(uint16_t encodingID) const;
    
private:
    FXCMapPlatform(uint16_t platformID);

    void
    initEncodings();

    void
    initUnicodeEncoding();

    void
    initMacintoshEncoding();

    void
    initISOEncoding();

    void
    initMicrosoftEncoding();

    void
    initAdobeEncoding();
    
    static const std::vector<FXPtr<FXCharBlock> > &
    getUnicodeBlocks() ;
    
private:
    uint16_t platformID_;
    std::map<uint16_t, std::vector<FXPtr<FXCharBlock> > > blocksMap_;
};

class FXCMap {
public:
    FXCMap(FXFace * face, uint16_t platformID, uint16_t encodingID, size_t index);

    uint16_t
    platformID() const;

    uint16_t
    encodingID() const;

    size_t
    index() const;

    bool
    isCurrent() const;

    bool
    setCurrent() const;

    std::string
    platformName() const;
    
    std::string
    encodingName() const;
    
    std::string
    description() const;

    bool
    isUnicode() const;

    const FXVector<FXPtr<FXCharBlock> > &
    blocks() const;

    FXVector<FXChar> 
    charsForGlyph(FXGlyphID gid) const;

    FXGlyphID
    glyphForChar(FXChar c) const;
private:
    void
    initBlocks();

    bool
    initGlyphsMap();

    FXPtr<FXFace>
    face() const;

    FXFTFace
    face0() const;
private:
    FXFace           * face_;
    size_t             index_;
    uint16_t           platformID_;
    uint16_t           encodingID_;
    FXVector<FXChar>   glyphMap_;
    FXMap<FXGlyphID, FXVector<FXChar> > extraGlyphsMap_;
};


class FXAutoCMap  {
public:
    FXAutoCMap(FXFace * face, size_t index);
    ~FXAutoCMap();

    size_t
    index() const;
    
    bool
    ok() const;
private:
    FXFace   * face_;
    size_t     index_;
    bool       ok_;
    size_t     oldIndex_;
};

#pragma once
#include <unordered_map>
#include "FX.h"
class FXFace;

class FXGCharBlock {
public:
    explicit FXGCharBlock(const FXString & name)
        : name_(name) {}
    virtual ~FXGCharBlock() {}

    virtual std::string
    name() const {
        return name_;
    }
    
    virtual size_t
    size() const = 0;

    virtual FXGChar
    get(size_t index) const = 0;

    virtual bool
    contains(const FXGChar & c) const = 0;

protected:
    FXString   name_;
};

class FXNullGCharBlock : public FXGCharBlock {
public:
    using FXGCharBlock::FXGCharBlock;
    virtual size_t
    size() const {
        return 0;
    }

    virtual FXGChar
    get(size_t) const {
        return FXGCharInvalid;
    }
    
    virtual bool
    contains(const FXGChar & ) const {
        return false;
    }
};

class FXCharRangeBlock : public FXGCharBlock {
public:
    FXCharRangeBlock(FXChar from, FXChar to, FXGCharType type, const std::string & name)
        : FXGCharBlock(name)
        , range_{from, to}
        , type_(type){}

    virtual size_t
    size() const {
        return range_.to - range_.from + 1;
    }

    virtual FXGChar
    get(size_t index) const {
        return {type_, static_cast<FXChar>(index + range_.from)};
    }
    
    virtual bool
    contains(const FXGChar & c) const {
        if (c.type != type_)
            return false;
        return c.value >= range_.from && c.value <= range_.to;
    }
    
    const FXCharRange &
    range() const {
        return range_;
    }

protected:
    FXCharRange    range_;
    FXGCharType    type_;
};

class FXCharArrayBlock : public FXGCharBlock {
public:
    FXCharArrayBlock(const FXVector<FXGChar> chs, const FXString & name)
        : FXGCharBlock(name)
        , chs_(chs) {}

    virtual size_t
    size() const {
        return chs_.size();
    }

    virtual FXGChar
    get(size_t index) const {
        return chs_[index];
    }

    virtual bool
    contains(const FXGChar & c) const;

protected:
    FXVector<FXGChar>  chs_;
};

class FXCMapPlatform {
public:
    static const std::vector<FXCMapPlatform> &
    availablePlatforms();

    static const FXCMapPlatform &
    get(uint16_t platformID);

private:
    static std::vector<FXCMapPlatform> platforms_;
    static std::vector<FXPtr<FXGCharBlock> > unicodeBlocks_;
public:
    const std::vector<FXPtr<FXGCharBlock> > &
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
    
    static const std::vector<FXPtr<FXGCharBlock> > &
    getUnicodeBlocks() ;
    
private:
    uint16_t platformID_;
    std::map<uint16_t, std::vector<FXPtr<FXGCharBlock> > > blocksMap_;
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

    const FXVector<FXPtr<FXGCharBlock> > &
    blocks() const;

    const FXVector<FXPtr<FXGCharBlock> > &
    unicodeBlocks() const;
    
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
    struct FXCharMapItem {
        FXChar    c;
        FXGlyphID g;
        FXCharMapItem(FXChar c, FXGlyphID g) : c(c), g(g) {}
    };
    
private:
    FXFace           * face_;
    size_t             index_;
    uint16_t           platformID_;
    uint16_t           encodingID_;
    FXVector<FXChar>   glyphMap_;
    FXMap<FXGlyphID, FXVector<FXChar> > extraGlyphsMap_;
    FXVector<FXCharMapItem> charMap_;

    FXVector<FXPtr<FXGCharBlock> > blocks_;
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

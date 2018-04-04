#pragma once

#include "FX.h"

class FXCharBlock {
public:
    virtual ~FXCharBlock() {}

    virtual std::string
    name() const = 0;
    
    virtual size_t
    size() const = 0;

    virtual FXChar
    get(size_t index) const = 0;
};

class FXCharRangeBlock : public FXCharBlock {
public:
    FXCharRangeBlock(FXChar from, FXChar to, const std::string & name, bool isUnicode = true)
        : range_{from, to}
        , name_(name)
        , isUnicode_(isUnicode) {}

    virtual std::string
    name() const {
        return name_;
    }

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
    std::string    name_;    
    bool           isUnicode_;
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
    FXCMap(uint16_t platformID, uint16_t encodingID);

    uint16_t
    platformID() const;

    uint16_t
    encodingID() const;
    
    std::string
    platformName() const;
    
    std::string
    encodingName() const;
    
    std::string
    description() const;

    bool
    isUnicode() const;

    const std::vector<FXPtr<FXCharBlock> > &
    blocks() const;

private:
    void
    initBlocks();
    
private:
    uint16_t       platformID_;
    uint16_t       encodingID_;
};

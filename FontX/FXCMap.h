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

class FXSimpleCharBlock : public FXCharBlock {
public:
    FXSimpleCharBlock(FXChar from, FXChar to, const std::string & name, bool isUnicode = true)
        : from_(from)
        , to_(to)
        , name_(name) {}

    virtual std::string
    name() const {
        return name_;
    }

    virtual size_t
    size() const {
        return to_ - from_ + 1;
    }

    virtual FXChar
    get(size_t index) const {
        return index + from_;
    }
    
    FXChar
    from() const {
        return from_;
    }

    FXChar
    to() const {
        return to_;
    }

    bool
    isUnicode() const {
        return isUnicode_;
    }
protected:
    FXChar      from_;
    FXChar      to_;
    bool        isUnicode_;
    std::string name_;
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

#pragma once
#include "FX.h"
#include "FXCMap.h"
#include "FXGlyph.h"

struct FXFaceDescriptor {
    FXString  filePath;
    size_t    index;
};

struct FXSFNTEntry {
    uint16_t     platformId;
    uint16_t     encodingId;
    FXString     language;
    uint16_t     nameId;
    FXString     value;
};

class FXSFNTNames : public FXVector<FXSFNTEntry> {
public:
    FXString
    familyName() const;

    FXString
    styleName() const;

    FXString
    postscriptName() const;

protected:
    FXString
    findName(const FXVector<int> & nameIds,
             const FXVector<FXString> & languages) const;
        
    
};

struct FXFaceAttributes {
    size_t      index { 0 };
    size_t      upem { 0 };
    size_t      glyphCount { 0 };
    FXString    fileName;
    FXSFNTNames names;
};

constexpr double FXDefaultFontSize = 200;
constexpr double FXDefaultDPI      = 72;

double
pt2px(double p, double dpi = FXDefaultDPI);

double
px2pt(double p, double dpi = FXDefaultDPI);
    
class FXFace : public std::enable_shared_from_this<FXFace> {
public:
    static FXPtr<FXFace>
    createFace(const FXFaceDescriptor & descriptor);
    
    static FXPtr<FXFace>
    createFace(const std::string & filePath, size_t index);

    static FXPtr<FXFace>
    createFace(FXFTFace face);
public:
    FXFTFace
    face() const;
    
    size_t
    index() const;

    std::string
    postscriptName() const;

    size_t
    upem() const;

    size_t
    glyphCount() const;
    
    const FXFaceAttributes &
    attributes() const;

public:
    const std::vector<FXCMap> &
    cmaps() const;

    const FXCMap &
    currentCMap() const;

    size_t
    currentCMapIndex() const;

    bool
    selectCMap(size_t cmapIndex);

    bool
    selectCMap(const FXCMap & cmap);

public:
    FXGlyph
    glyph(FXChar c, bool isGID = false);

    /**
     * return the chars which maps to the gid in current cmap
     */
    FXVector<FXChar>
    charsForGlyph(FXGlyphID gid) const;
    
private:
    explicit FXFace(const FXFaceDescriptor & descriptor);
    explicit FXFace(FXFTFace face);
    FXFace(const FXFace &) = delete;
    FXFace & operator=(const FXFace & ) = delete;
public:    
    ~FXFace();
    
private:
    bool
    init();

    bool
    initAttributes();

    bool
    initCMap();
protected:
    FXFaceDescriptor     desc_;
    FXFTFace             face_;
    FXFaceAttributes     atts_;
    std::vector<FXCMap>  cmaps_;
};

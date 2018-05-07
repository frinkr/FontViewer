#pragma once
#include "FX.h"
#include "FXCMap.h"
#include "FXGlyph.h"

class FXGlyphCache;

struct FXFaceDescriptor {
    FXString  filePath;
    size_t    index;
};

struct FXSFNTName {
    uint16_t     platformId;
    uint16_t     encodingId;
    FXString     language;
    uint16_t     nameId;
    FXString     value;
};

class FXFaceNames : public FXVector<FXSFNTName> {
public:
    FXString
    familyName() const;

    FXString
    styleName() const;

    FXString
    postscriptName() const;

    void
    setFamilyName(const FXString & name);

    void
    setStyleName(const FXString & name);

    void
    setPostscriptName(const FXString & name);

    FXMap<FXString, FXString>
    localizedFamilyNames() const;

    FXMap<FXString, FXString>
    localizedStyleNames() const;

    FXMap<FXString, FXString>
    localizedPostscriptNames() const;

    FXMap<FXString, FXString>
    findSFNTNames(const FXVector<int> & nameIds) const;
    
protected:
    FXString
    findSFNTName(const FXVector<int> & nameIds,
                 const FXVector<FXString> & languages,
                 const FXString & defaultName = FXString()) const;
    
    FXString     familyName_;
    FXString     styleName_;
    FXString     postscriptName_;
    
};

struct FXFaceAttributes {
    FXFaceDescriptor desc;
    size_t           upem { 0 };
    FXString         format;
    size_t           glyphCount { 0 };
    FXString         fileName;
    FXFaceNames      names;
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
    glyph(FXGChar c);

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
    FXPtr<FXGlyphCache>  cache_;
};

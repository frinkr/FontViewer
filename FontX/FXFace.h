#pragma once
#include "FX.h"
#include "FXCMap.h"
#include "FXGlyph.h"
#include "FXTag.h"

class FXGlyphCache;
class FXInspector;

struct FXFaceDescriptor {
    FXString  filePath;
    size_t    index;

    bool
    operator==(const FXFaceDescriptor & other) const;
    
    bool
    operator<(const FXFaceDescriptor & other) const;

    bool
    operator!=(const FXFaceDescriptor & other) const;
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

    FXString
    getSFNTName(int nameId) const;

    FXString
    vendor() const;

    FXString
    version() const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        FXVector<FXSFNTName> & vec = *this;
        ar & vec;
        ar & familyName_;
        ar & styleName_;
        ar & postscriptName_;
    }
protected:
    FXString
    findSFNTName(const FXVector<int> & nameIds,
                 const FXVector<FXString> & languages,
                 const FXString & defaultName = FXString()) const;
    
    FXString     familyName_;
    FXString     styleName_;
    FXString     postscriptName_;
    
};

struct FXFaceFormatConstant {
    static const FXString TrueType;
    static const FXString Type1;
    static const FXString CFF;
    static const FXString WinFNT;
    static const FXString Other;
};

struct FXFaceAttributes {
    FXFaceDescriptor desc;
    size_t           upem { 0 };
    FXString         format;
    size_t           glyphCount { 0 };
    FXFaceNames      names;
    fu               ascender;
    fu               descender;
    FXRect<fu>       bbox;

    bool             isCID;
    FXString         cid;
    bool             isOpenTypeVariable;
    bool             isMultipleMaster;
};


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
    createFace(FXPtr<FXStream> stream, size_t faceIndex);

    static FXPtr<FXFace>
    createFace(FXFTFace face);

public:
    virtual bool
    valid() const;
    
    FXFTFace
    face() const;
    
    virtual size_t
    index() const;

    virtual std::string
    postscriptName() const;

    virtual size_t
    upem() const;

    virtual size_t
    glyphCount() const;
    
    virtual const FXFaceAttributes &
    attributes() const;

    virtual size_t
    faceCount() const;

    virtual FXPtr<FXFace>
    openFace(size_t index);

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
    struct AutoFontSize {
        AutoFontSize(FXFace * face, double fontSize);
        ~AutoFontSize();
    private:
        FXFace * face_;
        FXFTSize oldSize_;
        FXFTSize newSize_;
    };
    
    double
    fontSize() const;

    bool
    isScalable() const;

    double
    bmScale() const;

    /**
     * Set new font size, the returned font size may be different to required size
     */
    double
    selectFontSize(double fontSize);

public:
    FXGlyph
    glyph(FXGChar c);

    FXPixmapARGB
    pixmap(FXGlyphID gid, FXVec2d<int> * offset = nullptr);

    /**
     * return the chars which maps to the gid in current cmap
     */
    FXVector<FXChar>
    charsForGlyph(FXGlyphID gid) const;

public:
    struct VariableAxis {
        size_t    index;
        FXString  name;
        FXTag     tag;
        FXFixed   minValue;
        FXFixed   maxValue;
        FXFixed   defaultValue;
    };

    struct VariableNamedInstance {
        size_t            index;
        FXString          name;
        FXString          psName;
        FXVector<FXFixed> coordinates;
    };

    const FXVector<VariableAxis> &
    variableAxises() const;

    const FXVector<VariableNamedInstance> &
    variableNamedInstances() const;

    const FXVector<FXFixed>
    currentVariableCoordinates() const;

    /*return true if changed*/
    bool 
    setCurrentVariableCoordinates(const FXVector<FXFixed> & coords);

    void
    resetVariableCoordinates();

public:
    FXPtr<FXInspector>
    inspector();

protected:
    FXFace();
    explicit FXFace(const FXFaceDescriptor & descriptor);
    explicit FXFace(const std::string & filePath, size_t index);
    explicit FXFace(FXFTFace face);
    explicit FXFace(FXPtr<FXStream> stream, size_t faceIndex);

    FXFace(const FXFace &) = delete;
    FXFace & operator=(const FXFace & ) = delete;
public:    
    virtual ~FXFace();
    
protected:
    virtual bool
    init();

    virtual bool
    initAttributes();

    virtual bool
    initCMap();

    virtual bool
    initVariables();

protected:
    FXFaceDescriptor     desc_{};
    FXPtr<FXStream>      stream_{};
    FXFTFace             face_{nullptr};

    double               fontSize_{1.0};
    bool                 scalable_{false};
    double               bmScale_{1.0};
    int                  bmStrikeIndex_{0};

    FXFaceAttributes     atts_{};
    std::vector<FXCMap>  cmaps_{};
    FXPtr<FXGlyphCache>  cache_{};
    FXPtr<FXInspector>   inspector_{};

    FXVector<VariableAxis>            variableAxises_{};
    FXVector<VariableNamedInstance>   variableNamedInstances_{};
};

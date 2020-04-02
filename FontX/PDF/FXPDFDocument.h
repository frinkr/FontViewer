#pragma once
#include <ctime>
#include "FontX/FX.h"

namespace PoDoFo {
    class PdfObject;
    class PdfMemDocument;
}

class FXPDFFace;

struct FXPDFFontInfo {
    FXString  reference {};
    FXString  baseFont{};
    FXString  subType{};
    bool      isSubset{false};
    const PoDoFo::PdfObject * fontObject {nullptr};
};

struct FXPDFDocumentInfo {
    std::time_t  created {};
    std::time_t  modified {};
    FXString     application {};
    size_t       pages {};
};

class FXPDFDocument : public std::enable_shared_from_this<FXPDFDocument> {
public:
    /** Throw exception if the password is wrong */
    static FXPtr<FXPDFDocument>
    open(const FXString & file,
         const FXSet<size_t> & pages = {},
         const FXString & password = {});

public:
    explicit FXPDFDocument(const FXString & path,
                           const FXSet<size_t> & pages = {},
                           const FXString & password = {});
    ~FXPDFDocument();

    bool
    open();

    bool
    close();

    const FXPDFDocumentInfo &
    documentInfo() const;
    
    const FXString &
    filePath() const;

    size_t
    pageCount() const;

    size_t
    fontCount() const;

    FXPDFFontInfo
    fontInfo(size_t index) const;

    const PoDoFo::PdfObject *
    fontObject(size_t index) const;

    size_t
    fontObjectIndex(const PoDoFo::PdfObject *) const;

    FXPtr<FXPDFFace>
    createFace(size_t index);

    void
    faceDestroyed(FXPDFFace * face);

private:

    bool
    loadDocumentInfo();

    void
    processPage(int pageIndex);

    void
    processResource(const PoDoFo::PdfObject * resource);
    
    void
    processFontResource(const PoDoFo::PdfObject * fontResource);
    
private:
    FXString              file_;
    FXPDFDocumentInfo     info_;
    FXVector<FXPDFFontInfo>   fontsInfo_;
    FXSet<FXString>           baseFontsNames_;
    std::unique_ptr<PoDoFo::PdfMemDocument>  document_;
};

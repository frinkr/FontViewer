#pragma once
#include "FontX/FX.h"

namespace PoDoFo {
    class PdfObject;
    class PdfMemDocument;
}

class FXPDFFace;

struct FXPDFFontInfo {
    FXString  baseFont{};
    FXString  subType{};
    bool      isSubset{false};
    const PoDoFo::PdfObject * fontObject {nullptr};
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
    void
    processPage(int pageIndex);

private:
    FXString              file_;
    FXVector<FXPDFFontInfo>   fonts_;
    FXSet<const PoDoFo::PdfObject *> fontObjects_;
    std::unique_ptr<PoDoFo::PdfMemDocument>  document_;
};

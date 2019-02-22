#pragma once
#include "FontX/FX.h"

class FXPDFDocumentImp;
class FXPDFFace;

struct FXPDFFontInfo {
    FXString  baseFont;
    FXString  subType;
    bool      isSubset;
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

    size_t
    pageCount() const;

    size_t
    fontCount() const;

    FXPDFFontInfo
    fontInfo(size_t index) const;

    FXPtr<FXPDFFace>
    createFace(int index) const;

private:
    std::unique_ptr<FXPDFDocumentImp> imp_;
};

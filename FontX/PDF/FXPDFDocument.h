#pragma once
#include "FontX/FX.h"
#include "FontX/FXFace.h"

class FXPDFDocumentImp;

struct FXPDFFontInfo {
    FXString  baseFont;
    FXString  subType;
    bool      isSubset;
};

class FXPDFDocument {
public:
    explicit FXPDFDocument(const FXString & path);

    ~FXPDFDocument();

    bool
    open();

    bool
    close();

    size_t
    fontCount() const;

    FXPDFFontInfo
    fontInfo(size_t index) const;

    FXPtr<FXFace>
    createFace(int index) const;

private:
    std::unique_ptr<FXPDFDocumentImp> imp_;
};

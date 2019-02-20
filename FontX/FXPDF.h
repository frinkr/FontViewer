#pragma once
#include "FX.h"
#include "FXFace.h"

class FXPDFDocumentImp;

struct FXPDFFontEntry {
    FXString  name;
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

    FXPDFFontEntry
    fontEntry(size_t index) const;

    FXPtr<FXFace>
    createFace(int index) const;

private:
    std::unique_ptr<FXPDFDocumentImp> imp_;
};

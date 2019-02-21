#pragma once
#include "FontX/FX.h"
#include "FontX/FXFace.h"

class PDFDocumentImp;

struct PDFFontEntry {
    FXString  name;
    bool      isSubset;
};

class PDFDocument {
public:
    explicit PDFDocument(const FXString & path);

    ~PDFDocument();

    bool
    open();

    bool
    close();

    size_t
    fontCount() const;

    PDFFontEntry
    fontEntry(size_t index) const;

    FXPtr<FXFace>
    createFace(int index) const;

private:
    std::unique_ptr<PDFDocumentImp> imp_;
};

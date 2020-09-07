#pragma once

#include "FontX/FXFace.h"

namespace PoDoFo {
    class PdfMemDocument;
    class PdfObject;
}

constexpr const char * FXPDFDocumentInfoKey = "info.document.pdf";

class FXPDFDocument;
struct FXPDFFontInfo;

class FXPDFFace : public FXFace {
public:
    FXPDFFace(FXPtr<FXPDFDocument> document, const FXPDFFontInfo & font);

    ~FXPDFFace();

    std::string
    postscriptName() const override;
    
    bool
    isSubset() const override;

    size_t
    faceCount() const override;

    FXPtr<FXFace>
    openFace(size_t index) override;

    FXPtr<FXPDFDocument>
    document() const;

protected:
    bool
    init() override;

private:
    FXPtr<FXPDFDocument>      document_;
    const FXPDFFontInfo     & font_;
};

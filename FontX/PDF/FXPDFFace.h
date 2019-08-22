#pragma once

#include "FontX/FXFace.h"

namespace PoDoFo {
    class PdfMemDocument;
    class PdfObject;
}

constexpr const char * FXPDFDocumentInfoKey = "info.document.pdf";

class FXPDFDocument;
class FXPDFFace : public FXFace {
public:
    FXPDFFace(FXPtr<FXPDFDocument> document, const PoDoFo::PdfObject * fontObj);

    ~FXPDFFace();

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
    const PoDoFo::PdfObject * fontObj_{nullptr};
};

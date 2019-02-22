#pragma once

#include "FontX/FXFace.h"

namespace PoDoFo {
    class PdfMemDocument;
    class PdfObject;
}

class FXPDFDocument;
class FXPDFFace : public FXFace {
public:
    FXPDFFace(FXPtr<FXPDFDocument> document, const PoDoFo::PdfObject * fontObj);

    size_t
    faceCount() const override;

    FXPtr<FXFace>
    openFace(size_t index) override;

protected:
    bool
    init() override;

private:
    FXPtr<FXPDFDocument>      document_;
    const PoDoFo::PdfObject * fontObj_{nullptr};
};

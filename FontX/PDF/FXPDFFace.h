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

protected:
    bool
    init() override;

private:
    FXPtr<FXPDFDocument>      document_;
    const PoDoFo::PdfObject * fontObj_{nullptr};
};

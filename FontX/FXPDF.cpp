#include <podofo/podofo.h>
#include "FXPDF.h"

using namespace PoDoFo;
class FXPDFDocumentImp {
public:
    explicit FXPDFDocumentImp(const FXString & file)
        : file_(file) {
    }

    bool
    open() {
        PdfMemDocument doc;
        PoDoFo::PdfStreamedDocument document(file_.c_str());
        return true;
    }

private:
    FXString file_;
};


FXPDFDocument::FXPDFDocument(const FXString & path)
    : imp_(std::make_unique<FXPDFDocumentImp>(path)) {
}

FXPDFDocument::~FXPDFDocument() {
    
}

bool
FXPDFDocument::open() {
    return imp_->open();
}

bool
FXPDFDocument::close() {
    return false;
}

size_t
FXPDFDocument::fontCount() const {
    return 0;
}

FXPDFFontEntry
FXPDFDocument::fontEntry(size_t index) const {
    
}

FXPtr<FXFace>
FXPDFDocument::createFace(int index) const {
    return nullptr;
}

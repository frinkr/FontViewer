#include <podofo/podofo.h>
#include "FXPDF.h"
#include <iostream>
using namespace PoDoFo;
class FXPDFDocumentImp {
public:
    explicit FXPDFDocumentImp(const FXString & file)
        : file_(file) {
    }

    bool
    open() {
        try {
            PdfMemDocument doc(file_.c_str());
            auto extension = doc.GetPdfExtensions();
            EPdfVersion ver = doc.GetPdfVersion();
            const PdfVecObjects & objects = doc.GetObjects();
            for (PdfObject * obj: objects) {
                if (obj->GetDataType() != ePdfDataType_Dictionary)
                    continue;
                
                if (obj->GetDictionary().GetKey(PdfName::KeyType)->GetName() != PdfName("Font"))
                    continue;
                PdfFont * font = doc.GetFont(obj);
                if (font) {
                    std::cout << font->IsSubsetting();
                }
            }
        } catch(PdfError err) {
            std::cout << err.GetError();
        }
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

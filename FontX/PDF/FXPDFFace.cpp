#include "podofo/podofo.h"
#include "FontX/FXLib.h"
#include "FontX/FXFTPrivate.h"
#include "FXPDFFace.h"
#include "FXPDFDocument.h"

using namespace PoDoFo;

namespace {
    const PdfObject *
    getFontFileObj(const PdfObject * fontObj) {
        const PdfObject * descriptor = fontObj->GetIndirectKey("FontDescriptor");
        
        if (!descriptor) {
            // Type0
            const PdfObject * descendantFonts = fontObj->GetIndirectKey("DescendantFonts");
            if (descendantFonts) {
                const PdfArray & array = descendantFonts->GetArray();
                if (array.size() == 1) {
                    const PdfReference & fontRef = array.front().GetReference();
                    const PdfObject * font = fontObj->GetOwner()->GetObject(fontRef);
                    if (font)
                        descriptor = font->GetIndirectKey("FontDescriptor");
                }
            }
        }
        
        if (!descriptor)
            return nullptr;
        
        const PdfObject * fontFile = descriptor->GetIndirectKey("FontFile");
        if (!fontFile)
            fontFile = descriptor->GetIndirectKey("FontFile2");
        if (!fontFile)
            fontFile = descriptor->GetIndirectKey("FontFile3");
        return fontFile;
    }

}

FXPDFFace::FXPDFFace(FXPtr<FXPDFDocument> document, const FXPDFFontInfo & font)
    : document_(document)
    , font_(font) {

    const PdfObject * fontFile = getFontFileObj(font_.fontObject);
    if (fontFile) {
        const PdfStream * stream = fontFile->GetStream();
        if (stream) {
            char * buffer = nullptr;
            pdf_long bufferLength = 0;
            stream->GetFilteredCopy(&buffer, &bufferLength);

            FXPtr<FXStream> stream(new FXMemoryStream((unsigned char *)buffer, bufferLength, [](const FXMemoryStream::Byte * data){
                podofo_free((void*)data);
            }));

            if (FXFTOpenFace(FXLib::get(), stream, 0, &face_))
                return;
            init();
        }
    }

    properties_.set(FXPDFDocumentInfoKey, document->documentInfo());
}

FXPDFFace::~FXPDFFace() {
    document_->faceDestroyed(this);
}

FXPtr<FXPDFDocument>
FXPDFFace::document() const {
    return document_;
}

std::string
FXPDFFace::postscriptName() const {
    return font_.baseFont;
}

bool
FXPDFFace::isSubset() const {
    return font_.isSubset;
}

size_t
FXPDFFace::faceCount() const {
    if (document_)
        return document_->fontCount();
    return 0;
}

FXPtr<FXFace>
FXPDFFace::openFace(size_t index) {
    if (document_)
        return document_->createFace(index);
    return nullptr;
}

bool
FXPDFFace::init() {
    if(!FXFace::init())
        return false;
    atts_.desc.filePath = document_->filePath();
    atts_.desc.index = document_->fontObjectIndex(font_.fontObject);
    desc_ = atts_.desc;
    return true;
}

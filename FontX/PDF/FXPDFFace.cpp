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
FXPDFFace::FXPDFFace(FXPtr<FXPDFDocument> document, const PdfObject * fontObj)
    : document_(document)
    , fontObj_(fontObj) {

    const PdfObject * fontFile = getFontFileObj(fontObj_);
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
    return true;
}

#include <iostream>
#include <stack>
#include "podofo/podofo.h"
#include "FontX/FXLog.h"
#include "FontX/FXFace.h"
#include "PDFDocument.h"
using namespace PoDoFo;

class PDFDocumentImp {
public:
    explicit PDFDocumentImp(const FXString & file)
        : file_(file) {
    }

    bool
    open() {
        try {
            document_.Load( file_.c_str() );
            int nCount = document_.GetPageCount();
            for( int i = 0; i< nCount; i++) 
                processPage(i);
            
            return true;
            
        } catch(PdfError err) {
            std::cout << err.GetError();
        }
        return true;
    }

    void
    processPage(int pageIndex) {
        PdfPage* pPage = document_.GetPage(pageIndex);
        
        PdfObject * pageResources = pPage->GetResources();
        if (!pageResources->GetDictionary().HasKey(PdfName("Font")))
            return;
        PdfObject * pageFontRes = pageResources->GetIndirectKey(PdfName("Font"));
        if (!pageFontRes || !pageFontRes->IsDictionary())
            return;

        const PdfDictionary & pageFontDict = pageFontRes->GetDictionary();
        for (const TKeyMap::value_type & kv: pageFontDict.GetKeys()) {
            PdfName fontId = kv.first;
            PdfObject * fontRef = kv.second;
            assert(fontRef->IsReference());
            FX_INFO(fontId.GetName() << std::string(" : ") << fontRef->GetReference().ToString());
            
            PdfObject * fontObj = pPage->GetObject()->GetOwner()->GetObject(fontRef->GetReference());
            assert(fontObj->IsDictionary());
                        
            const PdfObject * baseFont = fontObj->GetIndirectKey("BaseFont");
            if (baseFont && baseFont->IsName())
                FX_INFO("    /BaseFont: " << baseFont->GetName().GetName());
            
            const PdfObject * subType = fontObj->GetIndirectKey("Subtype");
            if (subType && subType->IsName())
                FX_INFO("    /Subtype: " << subType->GetName().GetName());
            
            const PdfObject * encoding = fontObj->GetIndirectKey("Encoding");
            if (encoding && encoding->IsName())
                FX_INFO("    /Encoding: " << encoding->GetName().GetName());
            
            const PdfObject * firstChar = fontObj->GetIndirectKey("FirstChar");
            if (firstChar && firstChar->IsNumber())
                FX_INFO("    /FirstChar: " << firstChar->GetNumber());
            
            const PdfObject * lastChar = fontObj->GetIndirectKey("LastChar");
            if (lastChar && lastChar->IsNumber())
                FX_INFO("    /LastChar: " << lastChar->GetNumber());
            
                        
            const PdfObject * descriptor = fontObj->GetIndirectKey("FontDescriptor");
            if(descriptor) {
                if (const PdfObject * fontFile = descriptor->GetIndirectKey("FontFile3")) {
                    const PdfStream * stream = fontFile->GetStream();
                    if (stream) {
                        
                        char * buffer = nullptr;
                        pdf_long bufferLength = 0;
                        stream->GetFilteredCopy(&buffer, &bufferLength);
                        {
                            FXPtr<FXStream> fxStream(new FXMemoryStream((unsigned char *)buffer, bufferLength));
                            FXPtr<FXFace> face = FXFace::createFace(fxStream, 0);
                            if (face) {
                                FX_INFO("     UPEM: " << face->upem());
                            }
                        }
                        
                        podofo_free(buffer);
                        
                        FX_INFO("    FontFile3: Compressed Length: " << stream->GetLength() << ", Uncompressed Length: " << bufferLength);
                    }
                }
            }
        }
    }
private:
    FXString file_;

    PdfMemDocument document_;
};


PDFDocument::PDFDocument(const FXString & path)
    : imp_(std::make_unique<PDFDocumentImp>(path)) {
}

PDFDocument::~PDFDocument() {
    
}

bool
PDFDocument::open() {
    return imp_->open();
}

bool
PDFDocument::close() {
    return false;
}

size_t
PDFDocument::fontCount() const {
    return 0;
}

PDFFontEntry
PDFDocument::fontEntry(size_t index) const {
    return PDFFontEntry();
}

FXPtr<FXFace>
PDFDocument::createFace(int index) const {
    return nullptr;
}

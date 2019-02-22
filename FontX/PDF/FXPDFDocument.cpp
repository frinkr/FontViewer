#include <iostream>
#include <stack>
#include <boost/filesystem.hpp>

#include "podofo/podofo.h"
#include "FontX/FXLog.h"
#include "FontX/FXFace.h"
#include "FXPDFDocument.h"
using namespace PoDoFo;

class FXPDFDocumentImp {
public:
    explicit FXPDFDocumentImp(const FXString & file)
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
                const PdfObject * fontFile = descriptor->GetIndirectKey("FontFile");
                if (!fontFile)
                    fontFile = descriptor->GetIndirectKey("FontFile2");
                if (!fontFile)
                    fontFile = descriptor->GetIndirectKey("FontFile3");
                
                if (fontFile) {
                    const PdfStream * stream = fontFile->GetStream();
                    if (stream) {
                        
                        char * buffer = nullptr;
                        pdf_long bufferLength = 0;
                        stream->GetFilteredCopy(&buffer, &bufferLength);
                        {
                            boost::filesystem::path temp = boost::filesystem::unique_path();
                            const std::string tempstr    = temp.native();  // optional
                            
                            FXPtr<FXStream> fxStream(new FXMemoryStream((unsigned char *)buffer, bufferLength));
                            FXPtr<FXFace> face = FXFace::createFace(fxStream, 0);
                            if (!face) {
                                FILE * file = fopen(tempstr.c_str(), "wb");
                                fwrite(buffer, bufferLength, 1, file);
                                fclose(file);
                                face = FXFace::createFace(tempstr, 0);
                            }
                            
                            if (face) {
                                FX_INFO("\t   face: UPEM: " << face->upem());
                                FX_INFO("\t   face: PSName: " << face->postscriptName());
                                FX_INFO("\t   face: GlyphCount: " << face->glyphCount());
                                FX_INFO("\t   face: Format: " << face->attributes().format);
                                FX_INFO("\t   face: Family Name: " << face->attributes().names.familyName());
                                FX_INFO("\t   face: Style Name: " << face->attributes().names.styleName());
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
    return FXPDFFontEntry();
}

FXPtr<FXFace>
FXPDFDocument::createFace(int index) const {
    return nullptr;
}

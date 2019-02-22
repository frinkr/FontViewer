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
    struct FontEntry {
        const PdfObject * fontObj;
        const PdfObject * fileObj;
    };
public:
    explicit FXPDFDocumentImp(const FXString & file)
        : file_(file) {
    }

    ~FXPDFDocumentImp() {
        close();
    }

    bool
    open() {
        try {
            document_.Load( file_.c_str() );
            int nCount = document_.GetPageCount();
            for( int i = 0; i< nCount; i++) 
                processPage(i);
            
            return true;    
        }
        catch(PdfError err) {
            FX_ERROR("Error (" << err.GetError() << "when paring " << file_);
            err.PrintErrorMsg();
            return false;
        }
        return true;
    }

    bool
    close() {
        return true;
    }

    size_t
    fontCount() const {
        return fonts_.size();
    }

    FXPDFFontInfo
    fontInfo(size_t index) const {
        FXPDFFontInfo info;
        const PdfObject * fontObj = fonts_[index].fontObj;
        
        const PdfObject * baseFont = fontObj->GetIndirectKey("BaseFont");
        if (baseFont && baseFont->IsName())
            info.baseFont = baseFont->GetName().GetName();

        const PdfObject * subType = fontObj->GetIndirectKey("Subtype");
        if (subType && subType->IsName())
            info.subType = subType->GetName().GetName();

        return info;
    }

    FXPtr<FXFace>
    createFace(size_t index) {
        auto & entry = fonts_[index];
        const PdfObject * fontFile = entry.fileObj;
        if (!fontFile) return nullptr;

        const PdfStream * stream = fontFile->GetStream();
        if (!stream)
            return nullptr;
        
        char * buffer = nullptr;
        pdf_long bufferLength = 0;
        stream->GetFilteredCopy(&buffer, &bufferLength);
        
        FXPtr<FXStream> fxStream(new FXMemoryStream((unsigned char *)buffer, bufferLength, [](const FXMemoryStream::Byte * data){
            podofo_free((void*)data);
        }));
        
        FXPtr<FXFace> face = FXFace::createFace(fxStream, 0);
        return face;
    }

    void
    processPage(int pageIndex) {
        PdfPage* pPage = document_.GetPage(pageIndex);
        
        PdfObject * pageResources = pPage->GetResources();
        PdfObject * pageFontRes = pageResources->GetIndirectKey(PdfName("Font"));
        if (!pageFontRes || !pageFontRes->IsDictionary()) {
            FX_WARNING("Page " << pageIndex << " doesn't use any fonts");
            return;
        }

        const PdfDictionary & pageFontDict = pageFontRes->GetDictionary();
        if (pageFontDict.GetKeys().empty()) {
            FX_WARNING("Page " << pageIndex << " doesn't use any fonts");
            return;
        }
        
        for (const TKeyMap::value_type & kv: pageFontDict.GetKeys()) {
            PdfObject * fontObj = pPage->GetObject()->GetOwner()->GetObject(kv.second->GetReference());
            dumpFontObject(kv.first, kv.second, fontObj);
            
            const PdfObject * fontFile = getFontFile(fontObj);
            
            FontEntry font;
            font.fontObj = fontObj;
            font.fileObj = fontFile;
            fonts_.push_back(font);
            
            if (!fontFile)
                FX_WARNING("    CAN'T load font file" << kv.first.GetName());
        }
    }

private:
    const PdfObject *
    getFontFile(PdfObject * fontObj) {
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

    void
    dumpFontObject(PdfName fontId, PdfObject * fontRef, PdfObject * fontObj) {
        assert(fontRef->IsReference());
        assert(fontObj->IsDictionary());

        FX_INFO(fontId.GetName() << std::string(" : ") << fontRef->GetReference().ToString());

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
    }

private:
    FXString              file_;
    PdfMemDocument        document_;
    FXVector<FontEntry>   fonts_;
};


bool
FXPDFDocument::countFaces(const FXString & path, size_t & count) {
    FXPDFDocument doc(path);
    if (doc.open()) {
        count = doc.fontCount();
        return true;
    }
    return false;
}

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
    return imp_->close();
}

size_t
FXPDFDocument::fontCount() const {
    return imp_->fontCount();
}

FXPDFFontInfo
FXPDFDocument::fontInfo(size_t index) const {
    return imp_->fontInfo(index);
}

FXPtr<FXFace>
FXPDFDocument::createFace(int index) const {
    return imp_->createFace(index);
}

#include <iostream>
#include <stack>
#include <boost/filesystem.hpp>

#include "podofo/podofo.h"
#include "FontX/FXLog.h"
#include "FontX/FXFace.h"
#include "FXPDFFace.h"
#include "FXPDFDocument.h"
using namespace PoDoFo;

class FXPDFDocumentImp {
public:
    struct FontEntry {
        const PdfObject * fontObj;
    };
public:
    FXPDFDocumentImp(const FXString & file,
                     const FXSet<size_t> & pages,
                     const FXString & password,
                     FXPDFDocument * parent)
        : file_(file)
        , parent_(parent){
    }

    ~FXPDFDocumentImp() {
        close();
    }

    bool
    open() {
        try {
            document_.Load( file_.c_str() );
            int nCount = document_.GetPageCount();
            for (int i = 0; i< nCount; i++) 
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
    pageCount() const {
        return document_.GetPageCount();
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

    FXPtr<FXPDFFace>
    createFace(size_t index) {
        return std::make_shared<FXPDFFace>(parent_->shared_from_this(), fonts_[index].fontObj);
    }

    void
    processPage(int pageIndex) {
        PdfPage * pPage = document_.GetPage(pageIndex);
        
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
            if (fontObjects_.find(fontObj) != fontObjects_.end())
                continue;

            fontObjects_.insert(fontObj);

            dumpFontObject(kv.first, kv.second, fontObj);
            
            FontEntry font;
            font.fontObj = fontObj;
            fonts_.push_back(font);
        }
    }

private:
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
    FXPDFDocument       * parent_{nullptr};
    FXVector<FontEntry>   fonts_;
    FXSet<PdfObject *>    fontObjects_;
};

FXPtr<FXPDFDocument>
FXPDFDocument::open(const FXString & file, const FXSet<size_t> & pages, const FXString & password) {
    auto doc = std::make_shared<FXPDFDocument>(file, pages, password);
    if (!doc->open())    
        return nullptr;
    return doc;
}

FXPDFDocument::FXPDFDocument(const FXString & path, const FXSet<size_t> & pages, const FXString & password)
    : imp_(std::make_unique<FXPDFDocumentImp>(path, pages, password, this)) {
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
FXPDFDocument::pageCount() const {
    return imp_->pageCount();
}

size_t
FXPDFDocument::fontCount() const {
    return imp_->fontCount();
}

FXPDFFontInfo
FXPDFDocument::fontInfo(size_t index) const {
    return imp_->fontInfo(index);
}

FXPtr<FXPDFFace>
FXPDFDocument::createFace(int index) const {
    return imp_->createFace(index);
}

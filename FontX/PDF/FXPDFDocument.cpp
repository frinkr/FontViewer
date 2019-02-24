#include <iostream>
#include <stack>
#include <boost/filesystem.hpp>

#include "podofo/podofo.h"
#include "FontX/FXLog.h"
#include "FontX/FXFace.h"
#include "FXPDFFace.h"
#include "FXPDFDocument.h"
using namespace PoDoFo;

namespace {
    FXSet<FXPDFFace*> openFaces_;
}

class FXPDFDocumentImp {
public:
    friend class FXPDFDocument;
    struct FontEntry : public FXPDFFontInfo {
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
            int pageCount = document_.GetPageCount();
            for (int pageIndex = 0; pageIndex < pageCount; pageIndex ++)
                processPage(pageIndex);
            return true;    
        }
        catch(PdfError err) {
            FX_ERROR("Failed to open file " << file_ << ", error " << err.GetError());
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
        return fonts_[index];
    }

    FXPtr<FXPDFFace>
    createFace(size_t index) {
        if (index >= fonts_.size())
            return nullptr;
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

            FontEntry font;
            font.fontObj = fontObj;

            const PdfObject * baseFont = fontObj->GetIndirectKey("BaseFont");
            if (baseFont && baseFont->IsName())
                font.baseFont = baseFont->GetName().GetName();

            const PdfObject * subType = fontObj->GetIndirectKey("Subtype");
            if (subType && subType->IsName())
                font.subType = subType->GetName().GetName();
         
            fonts_.push_back(font);
        }
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
    // Find in openFaces_ for document
    for (FXPDFFace * face: openFaces_) {
        if (face->document()->filePath() == file)
            return face->document();
    }

    // Or create new
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

const FXString &
FXPDFDocument::filePath() const {
    return imp_->file_;
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
FXPDFDocument::createFace(size_t index) const {
    auto face = imp_->createFace(index);
    if (face) 
        openFaces_.insert(face.get());
    return face;
}

void
FXPDFDocument::faceDestroyed(FXPDFFace * face) {
    auto itr = openFaces_.find(face);
    if (itr != openFaces_.end())
        openFaces_.erase(itr);
}

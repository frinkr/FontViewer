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
    : file_(path){
}

FXPDFDocument::~FXPDFDocument() {
    close();
}

bool
FXPDFDocument::open() {
    try {
        document_ = std::make_unique<PdfMemDocument>();
        document_->Load(file_.c_str());
        loadDocumentInfo();
        int pageCount = document_->GetPageCount();
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
FXPDFDocument::close() {
    return true;
}

const FXPDFDocumentInfo &
FXPDFDocument::documentInfo() const {
    return info_;
}

const FXString &
FXPDFDocument::filePath() const {
    return file_;
}

size_t
FXPDFDocument::pageCount() const {
    return document_->GetPageCount();
}

size_t
FXPDFDocument::fontCount() const {
    return fontsInfo_.size();
}

FXPDFFontInfo
FXPDFDocument::fontInfo(size_t index) const {
    return fontsInfo_[index];
}

const PoDoFo::PdfObject *
FXPDFDocument::fontObject(size_t index) const {
    return fontsInfo_[index].fontObject;
}

size_t
FXPDFDocument::fontObjectIndex(const PoDoFo::PdfObject * fontObj) const {
    for (size_t i = 0; i < fontsInfo_.size(); ++i) {
        if (fontObj == fontsInfo_[i].fontObject)
            return i;
    }
    return -1;
}

FXPtr<FXPDFFace>
FXPDFDocument::createFace(size_t index) {
    if (index >= fontsInfo_.size())
        return nullptr;
    auto face = std::make_shared<FXPDFFace>(shared_from_this(), fontsInfo_[index]);
    openFaces_.insert(face.get());
    return face;
}

void
FXPDFDocument::faceDestroyed(FXPDFFace * face) {
    auto itr = openFaces_.find(face);
    if (itr != openFaces_.end())
        openFaces_.erase(itr);
}

bool
FXPDFDocument::loadDocumentInfo() {
    info_.pages = document_->GetPageCount();
    PdfInfo * pdf = document_->GetInfo();
    info_.application = pdf->GetCreator().GetStringUtf8();
    info_.created = pdf->GetCreationDate().GetTime();
    info_.modified = pdf->GetModDate().GetTime();
    return true;
}

void
FXPDFDocument::processPage(int pageIndex) {
    const PdfPage * pPage = document_->GetPage(pageIndex);
    if (auto pageRes = pPage->GetResources())
        processResource(pageRes);
}

void
FXPDFDocument::processResource(const PoDoFo::PdfObject * resource) {
    if (auto fontRes = resource->GetIndirectKey(PdfName("Font")))
        processFontResource(fontRes);
    
    if (auto xobjs = resource->GetIndirectKey(PdfName("XObject"))) {
        if (xobjs->IsDictionary()) {
            const PdfDictionary & xobjDict = xobjs->GetDictionary();
            if (!xobjDict.GetKeys().empty()) {
                for (const TKeyMap::value_type & kv: xobjDict.GetKeys()) {
                    PdfObject * xobj = resource->GetOwner()->GetObject(kv.second->GetReference());
                    if (auto xobjRes = xobj->GetIndirectKey(PdfName("Resources")))
                        processResource(xobjRes);
                }
            }
        }
    }
}

void
FXPDFDocument::processFontResource(const PoDoFo::PdfObject * fontRes) {
    if (!fontRes || !fontRes->IsDictionary())
        return;
    
    const PdfDictionary & fontDict = fontRes->GetDictionary();
    if (fontDict.GetKeys().empty())
        return;
    

    for (const TKeyMap::value_type & kv: fontDict.GetKeys()) {
        PdfObject * fontObj = fontRes->GetOwner()->GetObject(kv.second->GetReference());
        
        FXPDFFontInfo font;
        font.reference = kv.second->GetReference().ToString();
        font.fontObject = fontObj;
        
        const PdfObject * baseFont = fontObj->GetIndirectKey("BaseFont");
        if (baseFont && baseFont->IsName())
            font.baseFont = baseFont->GetName().GetName();
        
        // Don't add if exists
        if (font.baseFont.size()) {
            if (baseFontsNames_.count(font.baseFont) != 0)
                continue;
            baseFontsNames_.insert(font.baseFont);
        }
        
        const PdfObject * subType = fontObj->GetIndirectKey("Subtype");
        if (subType && subType->IsName())
            font.subType = subType->GetName().GetName();
        
        fontsInfo_.push_back(font);
    }
}

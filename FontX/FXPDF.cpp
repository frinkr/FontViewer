#include <podofo/podofo.h>
#include "FXPDF.h"
#include <iostream>
#include <stack>
#include "FXLog.h"


using namespace PoDoFo;

#ifndef MAX_PATH
#define MAX_PATH 512
#endif // MAX_PATH

/** This class uses the PoDoFo lib to parse
 *  a PDF file and to write all text it finds
 *  in this PDF document to stdout.
 */
class TextExtractor {
public:
    TextExtractor();
    virtual ~TextExtractor();
    
    void Init( const char* pszInput );
    
private:
    /** Extract all text from the given page
     *
     *  \param pDocument the owning document
     *  \param pPage extract the text of this page.
     */
    void ExtractText( PdfMemDocument* pDocument, PdfPage* pPage );
    
    /** Adds a text string to a list which can be sorted by
     *  position on the page later, so that the whole structure
     *  of the text including formatting can be reconstructed.
     *
     *  \param dCurPosX x position of the text
     *  \param dCurPosY y position of the text
     *  \param pCurFont font of the text
     *  \param rString the actual string
     */
    void AddTextElement( double dCurPosX, double dCurPosY,
                        PdfFont* pCurFont, const PdfString & rString );
};


TextExtractor::TextExtractor()
{
    
}

TextExtractor::~TextExtractor()
{
}

void TextExtractor::Init( const char* pszInput )
{
    if( !pszInput )
    {
        PODOFO_RAISE_ERROR( ePdfError_InvalidHandle );
    }
    
    PdfMemDocument document;
    
    document.Load( pszInput );
    
    int nCount = document.GetPageCount();
    for( int i=0; i<nCount; i++ )
    {
        PdfPage* pPage = document.GetPage( i );
        
        this->ExtractText( &document, pPage );
    }
}

void TextExtractor::ExtractText( PdfMemDocument* pDocument, PdfPage* pPage )
{
    const char*      pszToken = NULL;
    PdfVariant       var;
    EPdfContentsType eType;
    
    PdfContentsTokenizer tokenizer( pPage );
    
    double dCurPosX     = 0.0;
    double dCurPosY     = 0.0;
    bool   bTextBlock   = false;
    PdfFont* pCurFont   = NULL;
    
    std::stack<PdfVariant> stack;
    
    while( tokenizer.ReadNext( eType, pszToken, var ) )
    {
        if( eType == ePdfContentsType_Keyword )
        {
            // support 'l' and 'm' tokens
            if( strcmp( pszToken, "l" ) == 0 ||
               strcmp( pszToken, "m" ) == 0 )
            {
                if( stack.size() == 2 )
                {
                    dCurPosX = stack.top().GetReal();
                    stack.pop();
                    dCurPosY = stack.top().GetReal();
                    stack.pop();
                }
                else
                {
                    fprintf( stderr, "WARNING: Token '%s' expects two arguments, but %" PDF_FORMAT_INT64 " given; ignoring\n",
                            pszToken, static_cast<pdf_int64>( stack.size() ) );
                    
                    while( !stack.empty() )
                        stack.pop();
                }
            }
            else if( strcmp( pszToken, "BT" ) == 0 )
            {
                bTextBlock   = true;
                // BT does not reset font
                // pCurFont     = NULL;
            }
            else if( strcmp( pszToken, "ET" ) == 0 )
            {
                if( !bTextBlock )
                    fprintf( stderr, "WARNING: Found ET without BT!\n" );
            }
            
            if( bTextBlock )
            {
                if( strcmp( pszToken, "Tf" ) == 0 )
                {
                    if( stack.size() < 2 )
                    {
                        fprintf( stderr, "WARNING: Expects two arguments for 'Tf', ignoring\n" );
                        pCurFont = NULL;
                        continue;
                    }
                    
                    stack.pop();
                    PdfName fontName = stack.top().GetName();
                    PdfObject* pFont = pPage->GetFromResources( PdfName("Font"), fontName );
                    if( !pFont )
                    {
                        PODOFO_RAISE_ERROR_INFO( ePdfError_InvalidHandle, "Cannot create font!" );
                    }
                    
                    pCurFont = pDocument->GetFont( pFont );
                    if( !pCurFont )
                    {
                        fprintf( stderr, "WARNING: Unable to create font for object %" PDF_FORMAT_INT64 " %" PDF_FORMAT_INT64 " R\n",
                                static_cast<pdf_int64>( pFont->Reference().ObjectNumber() ),
                                static_cast<pdf_int64>( pFont->Reference().GenerationNumber() ) );
                    }
                }
                else if( strcmp( pszToken, "Tj" ) == 0 ||
                        strcmp( pszToken, "'" ) == 0 )
                {
                    if( stack.size() < 1 )
                    {
                        fprintf( stderr, "WARNING: Expects one argument for '%s', ignoring\n", pszToken );
                        continue;
                    }
                    
                    AddTextElement( dCurPosX, dCurPosY, pCurFont, stack.top().GetString() );
                    stack.pop();
                }
                else if( strcmp( pszToken, "\"" ) == 0 )
                {
                    if( stack.size() < 3 )
                    {
                        fprintf( stderr, "WARNING: Expects three arguments for '%s', ignoring\n", pszToken );
                        
                        while( !stack.empty() )
                            stack.pop();
                        
                        continue;
                    }
                    
                    AddTextElement( dCurPosX, dCurPosY, pCurFont, stack.top().GetString() );
                    stack.pop();
                    stack.pop(); // remove char spacing from stack
                    stack.pop(); // remove word spacing from stack
                }
                else if( strcmp( pszToken, "TJ" ) == 0 )
                {
                    if( stack.size() < 3 )
                    {
                        fprintf( stderr, "WARNING: Expects one argument for '%s', ignoring\n", pszToken );
                        continue;
                    }
                    
                    PdfArray array = stack.top().GetArray();
                    stack.pop();
                    
                    for( int i=0; i<static_cast<int>(array.GetSize()); i++ )
                    {
                        if( array[i].IsString() || array[i].IsHexString() )
                            AddTextElement( dCurPosX, dCurPosY, pCurFont, array[i].GetString() );
                    }
                }
            }
        }
        else if ( eType == ePdfContentsType_Variant )
        {
            stack.push( var );
        }
        else
        {
            // Impossible; type must be keyword or variant
            PODOFO_RAISE_ERROR( ePdfError_InternalLogic );
        }
    }
}

void TextExtractor::AddTextElement( double dCurPosX, double dCurPosY,
                                   PdfFont* pCurFont, const PdfString & rString )
{
    if( !pCurFont )
    {
        fprintf( stderr, "WARNING: Found text but do not have a current font: %s\n", rString.GetString() );
        return;
    }
    
    if( !pCurFont->GetEncoding() )
    {
        fprintf( stderr, "WARNING: Found text but do not have a current encoding: %s\n", rString.GetString() );
        return;
    }
    
    // For now just write to console
    PdfString unicode = pCurFont->GetEncoding()->ConvertToUnicode( rString, pCurFont );
    const char* pszData = unicode.GetStringUtf8().c_str();
    while( *pszData ) {
        //printf("%02x", static_cast<unsigned char>(*pszData) );
        ++pszData;
    }
    printf("(%.3f,%.3f) %s \n", dCurPosX, dCurPosY, unicode.GetStringUtf8().c_str() );
}


class FXPDFDocumentImp {
public:
    explicit FXPDFDocumentImp(const FXString & file)
        : file_(file) {
    }

    bool
    open() {
        try {
            TextExtractor extractor;
            extractor.Init(file_.c_str());
            return true;
            
            PdfMemDocument doc(file_.c_str());
            auto extension = doc.GetPdfExtensions();
            
            int pageCount = doc.GetPageCount();
            for (int pageIndex = 0; pageIndex < pageCount; ++ pageIndex) {
                PdfPage * page = doc.GetPage(pageIndex);
                auto rect = page->GetPageSize();
                std::cout << "Page " << pageIndex << " rect(" << rect.GetLeft() << ", " << rect.GetBottom() << ", " << rect.GetWidth() << ", " << rect.GetHeight() << ")" << std::endl;
#if 0
                PdfObject * pageResources = page->GetResources();
                if (pageResources) {
                    EPdfDataType resourceDataType = pageResources->GetDataType();
                    auto name = pageResources->GetName();
                    
                }
#endif
                PdfContentsTokenizer tokenizer(page);
                const char*      pszToken = NULL;
                PdfVariant       var;
                EPdfContentsType eType;
                double dCurPosX     = 0.0;
                double dCurPosY     = 0.0;
                double dCurFontSize = 0.0;
                bool   bTextBlock   = false;
                PdfFont* pCurFont   = NULL;
                
                std::stack<PdfVariant> stack;
                
                while( tokenizer.ReadNext( eType, pszToken, var ) )
                {
                    
                    if( eType == ePdfContentsType_Keyword )
                    {
                        // support 'l' and 'm' tokens
                        
                        FX_WARNING(pszToken);
                        
                        if( strcmp( pszToken, "l" ) == 0 ||
                           strcmp( pszToken, "m" ) == 0 )
                        {
                            dCurPosX = stack.top().GetReal();
                            stack.pop();
                            dCurPosY = stack.top().GetReal();
                            stack.pop();
                        }
                        else if (strcmp(pszToken, "Td") == 0)
                        {
                            dCurPosY = stack.top().GetReal();
                            stack.pop();
                            dCurPosX = stack.top().GetReal();
                            stack.pop();
                        }
                        else if (strcmp(pszToken, "Tm") == 0)
                        {
                            dCurPosY = stack.top().GetReal();
                            stack.pop();
                            dCurPosX = stack.top().GetReal();
                            stack.pop();
                        }
                        else if( strcmp( pszToken, "BT" ) == 0 )
                        {
                            bTextBlock   = true;
                            // BT does not reset font
                            // dCurFontSize = 0.0;
                            // pCurFont     = NULL;
                        }
                        else if( strcmp( pszToken, "ET" ) == 0 )
                        {
                            if( !bTextBlock )
                                fprintf( stderr, "WARNING: Found ET without BT!\n" );
                        }
                        
                        if( bTextBlock )
                        {
                            if( strcmp( pszToken, "Tf" ) == 0 )
                            {
                                dCurFontSize = stack.top().GetReal();
                                stack.pop();
                                PdfName fontName = stack.top().GetName();
                                PdfObject* pFont = page->GetFromResources( PdfName("Font"), fontName );
                                if( !pFont )
                                {
                                    PODOFO_RAISE_ERROR_INFO( ePdfError_InvalidHandle, "Cannot create font!" );
                                }
                                
                                pCurFont = doc.GetFont( pFont );
                                if( !pCurFont )
                                {
                                    fprintf( stderr, "WARNING: Unable to create font for object %i %i R\n",
                                            pFont->Reference().ObjectNumber(),
                                            pFont->Reference().GenerationNumber() );
                                }
                            }
                            else if( strcmp( pszToken, "Tj" ) == 0 ||
                                    strcmp( pszToken, "'" ) == 0 )
                            {
                                //AddTextElement( dCurPosX, dCurPosY, pCurFont, stack.top().GetString() );
                                stack.pop();
                            }
                            else if( strcmp( pszToken, "\"" ) == 0 )
                            {
                                //AddTextElement( dCurPosX, dCurPosY, pCurFont, stack.top().GetString() );
                                stack.pop();
                                stack.pop(); // remove char spacing from stack
                                stack.pop(); // remove word spacing from stack
                            }
                            else if( strcmp( pszToken, "TJ" ) == 0 )
                            {
                                PdfArray array = stack.top().GetArray();
                                stack.pop();
                                
                                for( int i=0; i<static_cast<int>(array.GetSize()); i++ )
                                {
                                    FX_WARNING(" variant: " << array[i].GetDataTypeString());
                                    if(array[i].IsHexString()) {
                                        if(!pCurFont) {
                                            FX_WARNING(" : Could not Get font!!" << i);
                                        }
                                        else {
                                            if(!pCurFont->GetEncoding()) {
                                                FX_WARNING(": could not get encoding");
                                            } else {
                                                PdfString s = array[i].GetString();
                                                FX_WARNING(" : valid " << (s.IsValid()?"yes":"not"));
                                                FX_WARNING(" ;hex :   " <<  (s.IsHex()?"yes":"not"));
                                                FX_WARNING(" ;unicode:   " << (s.IsUnicode()?"yes":"not"));
                                                
                                                PdfString unicode = pCurFont->GetEncoding()->ConvertToUnicode(s,pCurFont);
                                                const char* szText = unicode.GetStringUtf8().c_str();
                                                FX_WARNING(" : " << (strlen(szText)> 0? szText: "nothing"));
                                                
                                            }
                                            
                                        }
                                    }
                                    else if(array[i].IsNumber()) {
                                        FX_WARNING(" : " <<  array[i].GetNumber());
                                    }
                                    
                                    if( array[i].IsString() )//|| array[i].IsHexString())
                                        //AddTextElement( dCurPosX, dCurPosY, pCurFont, array[i].GetString() );
                                        ;
                                }
                            }
                        }
                    }
                    else if ( eType == ePdfContentsType_Variant )
                    {
                        stack.push( var );
                        
                        FX_WARNING( " variant: " <<  var.GetDataTypeString());
                    }
                    else
                    {
                        // Impossible; type must be keyword or variant
                        PODOFO_RAISE_ERROR( ePdfError_InternalLogic );
                    }
                }
            }
            
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

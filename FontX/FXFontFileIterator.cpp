#include "FXFontFileIterator.h"
#include "FXFS.h"

#if FX_MAC
#include <CoreText/CoreText.h>
namespace {
    std::string toStdString(CFStringRef s) {
        if (auto fastCString = CFStringGetCStringPtr(s, kCFStringEncodingUTF8))
            return std::string(fastCString);
        
        auto utf16length = CFStringGetLength(s);
        auto maxUtf8len = CFStringGetMaximumSizeForEncoding(utf16length, kCFStringEncodingUTF8);
        std::string converted(maxUtf8len, '\0');

        CFStringGetCString(s, converted.data(), maxUtf8len, kCFStringEncodingUTF8);
        converted.resize(std::strlen(converted.data()));

        return converted;
    }

    class FXMacFontFileIterator : public FXFileIterator {
    private:
        CTFontCollectionRef fontCollection_ {};
        CFArrayRef allFonts_ {};
        CFIndex currentFontIndex_ {};
        CFIndex fontCount_ {};
    public:
        FXMacFontFileIterator() {
            fontCollection_ = CTFontCollectionCreateFromAvailableFonts(NULL);
            allFonts_       = CTFontCollectionCreateMatchingFontDescriptors(fontCollection_);
            fontCount_      = CFArrayGetCount(allFonts_);
        }

        ~FXMacFontFileIterator() {
            CFRelease(allFonts_);
            CFRelease(fontCollection_);
        }

        FXOpt<size_t>
        size() const override {
            return fontCount_;
        }

        bool
        reset() override {
            currentFontIndex_ = 0;
            return true;
        }

        FXOpt<FXString>
        next() override {
            if (currentFontIndex_ >= fontCount_)
                return FXNone<FXString>;

            CTFontDescriptorRef descRef = (CTFontDescriptorRef)CFArrayGetValueAtIndex(allFonts_, currentFontIndex_ ++ );
            CFURLRef fontURL = (CFURLRef)CTFontDescriptorCopyAttribute(descRef, kCTFontURLAttribute);

            auto filePath = toStdString(CFURLCopyFileSystemPath(fontURL, kCFURLPOSIXPathStyle));
            CFRelease(fontURL);

            return filePath;
        }
    };
}
#endif

    
FXPtr<FXFileIterator> FXCreateSystemFontFileIterator() {
#if FX_MAC
    return std::make_shared<FXMacFontFileIterator>();
#else
    static_assert(false, "not implemented");
#endif
}

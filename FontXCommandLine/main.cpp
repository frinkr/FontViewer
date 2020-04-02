#include <iostream>
#include "FCLDatabase.h"
#include "FontX/FXLib.h"
namespace {
    std::string getUCDRoot() {
        return std::string(FX_RESOURCES_DIR) + "/UCD";
    }
}

int main() {
    FXLib::init(getUCDRoot());
    
    auto db = FCLDatabase::instance();
    for (size_t i = 0; i < db->faceCount(); ++ i) {
        auto desc = db->faceDescriptor(i);
        auto atts = db->faceAttributes(i);
        std::cout << atts.sfntNames.postscriptName() << std::endl;
    }
    return 0;
}


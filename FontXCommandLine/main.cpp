#include <iostream>
#include "FCLDatabase.h"
#include "FontX/FXLib.h"
#include "FontX/FXLog.h"
#include "FontX/FXFS.h"
#include "FontX/FXFontFileIterator.h"

namespace {
    std::string getUCDRoot() {
        return std::string(FX_RESOURCES_DIR) + "/UCD";
    }
}

int main(int argc, const char ** argv) {

    if (false)
    {
        FXFileInDirectoryIterator itr("/Users/frinkr/Library/Fonts");
        while (auto file = itr.next())
            FX_ERROR(*file);
    }
    if (false)
    {
        auto itr = FXCreateSystemFontFileIterator();
        while (auto file = itr->next())
            FX_ERROR(*file);
        
    }
    
    
    if (argc != 2) {
        FX_ERROR(argv[0] << ": processor");
        return -1;
    }
    
    if (auto proc = FCLFindDatabaseProcessors(argv[1])) {
        FXLib::init(getUCDRoot());
        if (auto db = FCLDatabase::instance()) {
            proc->processDatabase(db);
            return 0;
        }
        else {
            FX_ERROR("Failed to load font database.");
            return -2;
        }
    }
    else {
        FX_ERROR("Failed to find the processor " << argv[1]);
        return -1;
    }
    
}


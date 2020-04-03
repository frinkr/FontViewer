#include <iostream>
#include "FCLDatabase.h"
#include "FontX/FXLib.h"
#include "FontX/FXLog.h"
namespace {
    std::string getUCDRoot() {
        return std::string(FX_RESOURCES_DIR) + "/UCD";
    }
}

int main(int argc, const char ** argv) {
    
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


#include <iostream>
#include <fstream>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <sqlite3.h>
#include "FXLib.h"
#include "FXFaceDatabase.h"
#include "FXBoostPrivate.h"
#include "FXPrivate.h"


FXFaceDatabase::FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath)
    : folders_(folders)
    , dbPath_(dbPath) {
    sqlite3_libversion();

    std::ofstream ofs(dbPath);
    
    boost::archive::text_oarchive ar(ofs);

    int count = 0;
    for(const auto & folder : folders) {
        BFS::foreachFile(folder, true, [&ar, &count](const FXString & file) {
            FXFTEnumurateFaces(FXLib::get(), file, [&ar, &count] (FXFTFace face, size_t index){
                if (face->family_name)
                    printf("face %d: %s\n", ++ count, face->family_name);
                return true;
            });
            ar & file;
            return true;
        });
    }

}

#include <iostream>
#include <fstream>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "FXLib.h"
#include "FXFaceDatabase.h"
#include "FXBoostPrivate.h"
#include "FXPrivate.h"

namespace {
    FXFaceAttributes emptyAttributes;
}

FXFaceDatabase::FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath)
    : folders_(folders)
    , dbPath_(dbPath) {
    rescan();
}

size_t
FXFaceDatabase::faceCount() const {
    return faces_.size();
}

const FXFaceDescriptor &
FXFaceDatabase::faceDescriptor(size_t index) {
    return faces_[index];
}

const FXFaceAttributes &
FXFaceDatabase::faceAttributes(size_t index) const {
    return attrs_[index];
}

FXPtr<FXFace>
FXFaceDatabase::createFace(const FXFaceDescriptor & descriptor) const {
    return FXFace::createFace(descriptor);
}

void
FXFaceDatabase::rescan() {
    std::ofstream ofs(dbPath_);
    
    boost::archive::text_oarchive ar(ofs);

    faces_.clear();
    attrs_.clear();
    int count = 0;
    for(const auto & folder : folders_) {
        BFS::foreachFile(folder, true, [this, &ar, &count](const FXString & file) {
            size_t count = 0;
            if (!FXFTCountFaces(FXLib::get(), file, count)) {
                for (size_t i = 0; i < count; ++ i) {
                    FXFaceDescriptor desc = {file, i};
                    faces_.push_back(desc);
                }
            }
            return true;
        });
    }

    for (const auto & desc : faces_) {
        FXPtr<FXFace> face = FXFace::createFace(desc);
        attrs_.push_back(face->attributes());
    }
}

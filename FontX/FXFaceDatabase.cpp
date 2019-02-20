#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/functional/hash.hpp>

#include "FXLib.h"
#include "FXFaceDatabase.h"
#include "FXBoostPrivate.h"
#include "FXFTPrivate.h"

namespace {
    constexpr int FACE_DB_VERSION = 5;

    size_t hashFile(const FXString & file, size_t hash) {
        boost::hash_combine(hash, file);
        boost::hash_combine(hash, BST::fileSize(file));
        return hash;
    }

    size_t
    hashFiles(const FXSet<FXString> & files) {
        size_t hash = 0;
        boost::hash_combine(hash, 0);
        for (const auto & f : files) 
            hash = hashFile(f, hash);
        
        return hash;
    }

    size_t hashFile(const FXString & file) {
        return hashFiles({file});
    }
}

namespace boost {namespace serialization {

        template <class Archive>
        void
        serialize(Archive & ar, FXSFNTName & sfnt, const unsigned int /*version*/) {
            ar & sfnt.platformId;
            ar & sfnt.encodingId;
            ar & sfnt.language;
            ar & sfnt.nameId;
            ar & sfnt.value;
        }
        
        template <class Archive>
        void
        serialize(Archive & ar, FXFaceDescriptor & desc, const unsigned int /*version*/) {
            ar & desc.filePath;
            ar & desc.index;
        }

        template <class Archive>
        void
        serialize(Archive & ar, FXFaceAttributes & atts, const unsigned int /*version*/) {
            ar & atts.desc;
            ar & atts.upem;
            ar & atts.format;
            ar & atts.glyphCount;
            ar & atts.names;
            ar & atts.ascender;
            ar & atts.descender;
            ar & atts.isCID;
            ar & atts.cid;
            ar & atts.isOpenTypeVariable;
            ar & atts.isMultipleMaster;
        }
        
        template <class Archive>
        void
        serialize(Archive & ar, FXFaceDatabase::FaceItem & item, const unsigned int /*version*/) {
            ar & item.desc;
            ar & item.atts;
        }

        template <class Archive>
        void
        serialize(Archive & ar, FXFaceDatabase::FoldersHash & dbHash, const unsigned int /*version*/) {
            ar & dbHash.hash;
            ar & dbHash.files;
        }
    }
}

FXFaceDatabase::FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath, ProgressCallback progressCallback)
    : folders_(folders)
    , dbPath_(dbPath)
    , progress_(progressCallback) {
    initDiskHash();
    if (!load() || checkForUpdate())
        rescan();
}

size_t
FXFaceDatabase::faceCount() const {
    return faces_.size();
}

size_t
FXFaceDatabase::faceIndex(const FXFaceDescriptor & desc) const {
    for (size_t i = 0; i < faces_.size(); ++ i) {
        if (desc == faces_[i].desc)
            return i;
    }
    return static_cast<size_t>(-1);
}

const FXFaceDescriptor &
FXFaceDatabase::faceDescriptor(size_t index) const {
    return faces_[index].desc;
}

const FXFaceAttributes &
FXFaceDatabase::faceAttributes(size_t index) const {
    return faces_[index].atts;
}

FXPtr<FXFace>
FXFaceDatabase::createFace(const FXFaceDescriptor & descriptor) const {
    return FXFace::createFace(descriptor);
}

void
FXFaceDatabase::rescan() {
    FXVector<FaceItem> faces;

    size_t current = 0;
    for (auto & kv: diskHash_.files) {
        ++ current;

        auto & file = kv.first;
        
        // Reuse the FaceItems if file hash is not changed
        if (kv.second == dbHash_.files[file]) {
            auto & indexes = diskHash_.faces[file];
            for (auto index: indexes)
                faces.push_back(faces_[index]);

            if (progress_ && (current % 100 == 0))
                progress_(current, diskHash_.files.size(), file);
            continue;
        }

        if (progress_)
            progress_(current, diskHash_.files.size(), file);

        size_t count = 0;
        if (!FXFTCountFaces(FXLib::get(), file, count)) {
            for (size_t i = 0; i < count; ++ i) {
                FXFaceDescriptor desc = {file, i};
                FXPtr<FXFace> face = FXFace::createFace(desc);
                if (!face)
                    continue;
                FaceItem item = {desc, face->attributes()};
                faces.push_back(item);
            }
        }
    }

    // make progress finish
    if (progress_)
        progress_(diskHash_.files.size(), diskHash_.files.size(), FXString());

    faces_ = faces;
    dbHash_ = FoldersHash(); // don't need it anymore
    save();
}

bool
FXFaceDatabase::save() {
    try {
        std::ofstream ofs(dbPath_);
        boost::archive::text_oarchive ar(ofs);
        ar << FACE_DB_VERSION;
        ar << diskHash_;
        ar << faces_;
        
        return true;
    }
    catch(const boost::archive::archive_exception &)
    {}
    return false;
}
    
bool
FXFaceDatabase::load() {
    try {
        std::ifstream ifs(dbPath_);
        boost::archive::text_iarchive ia(ifs);

        // Load and check version
        int dbVersion = -1;
        ia >> dbVersion;
        if (dbVersion != FACE_DB_VERSION)
            return false;
        
        ia >> dbHash_;
        ia >> faces_;
        
        // Rebuild face indexes in diskHash_
        for (size_t i = 0; i < faces_.size(); ++ i)
            diskHash_.faces[faces_[i].desc.filePath].push_back(i);
    }
    catch(const boost::archive::archive_exception &) {
        return false;
    }
    
    return !faces_.empty();
}

bool
FXFaceDatabase::checkForUpdate() {
    return dbHash_.hash != diskHash_.hash;
}

void
FXFaceDatabase::initDiskHash() {
    diskHash_ = FoldersHash();
    for(const auto & folder : folders_)
        BST::foreachFile(folder, true, [this](const FXString & file) {
            diskHash_.hash = hashFile(file, diskHash_.hash);
            diskHash_.files[file] = hashFile(file);
            return true;
        });
}

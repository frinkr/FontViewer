#include <iostream>
#include <fstream>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/functional/hash.hpp>

#include "FXLib.h"
#include "FXFaceDatabase.h"
#include "FXBoostPrivate.h"
#include "FXFTPrivate.h"

namespace {
    constexpr int FACE_DB_VERSION = 2;

    size_t
    hashFiles(const FXSet<FXString> & files) {
        size_t hash = 0;
        boost::hash_combine(hash, 0);
        for (const auto & f : files) {
            boost::hash_combine(hash, f);
            boost::hash_combine(hash, BST::fileSize(f));
        }
        return hash;
    }
}
namespace boost {namespace serialization {

        template <class Archive>
        void
        serialize(Archive & ar, FXSFNTName & sfnt, const unsigned int version) {
            ar & sfnt.platformId;
            ar & sfnt.encodingId;
            ar & sfnt.language;
            ar & sfnt.nameId;
            ar & sfnt.value;
        }
        
        template <class Archive>
        void
        serialize(Archive & ar, FXFaceDescriptor & desc, const unsigned int version) {
            ar & desc.filePath;
            ar & desc.index;
        }

        template <class Archive>
        void
        serialize(Archive & ar, FXFaceAttributes & atts, const unsigned int version) {
            ar & atts.desc;
            ar & atts.upem;
            ar & atts.format;
            ar & atts.glyphCount;
            ar & atts.names;
            ar & atts.ascender;
            ar & atts.descender;
            ar & atts.isCID;
            ar & atts.cid;
            ar & atts.isOTVariant;
            ar & atts.isMM;
        }
        
        template <class Archive>
        void
        serialize(Archive & ar, FXFaceDatabase::FaceItem & item, const unsigned int version) {
            ar & item.desc;
            ar & item.atts;
        }
    }
}

FXFaceDatabase::FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath)
    : folders_(folders)
    , dbPath_(dbPath)
    , hash_(-1) {
    if (!load() || checkUpdate())
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
    return -1;
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
    faces_.clear();
    FXSet<FXString> files;
    for(const auto & folder : folders_) {
        BST::foreachFile(folder, true, [this, &files](const FXString & file) {
            files.insert(file);
            
            size_t count = 0;
            if (!FXFTCountFaces(FXLib::get(), file, count)) {
                for (size_t i = 0; i < count; ++ i) {
                    FXFaceDescriptor desc = {file, i};
                    FXPtr<FXFace> face = FXFace::createFace(desc);
					if (!face)
						continue;
                    FaceItem item = {desc, face->attributes()};
                    faces_.push_back(item);
                }
            }
            return true;
        });
    }

    hash_ = hashFiles(files);

    save();
}

bool
FXFaceDatabase::save() {
    try {
        std::ofstream ofs(dbPath_);
        boost::archive::text_oarchive ar(ofs);
        ar << FACE_DB_VERSION;
        ar << faces_;
        ar << hash_;
        
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
        
        int dbVersion = -1;
        ia >> dbVersion;
        if (dbVersion != FACE_DB_VERSION)
            return false;
        
        ia >> faces_;
        ia >> hash_;
    }
    catch(const boost::archive::archive_exception &) {
        return false;
    }
    
    return !faces_.empty();
}

bool
FXFaceDatabase::checkUpdate() const {
    FXSet<FXString> files;
    for(const auto & folder : folders_) 
        BST::foreachFile(folder, true, [&files](const FXString & file) {
            //size_t count;
            //if (!FXFTCountFaces(FXLib::get(), file, count))
                files.insert(file);
            return true;
        });

    size_t hash = hashFiles(files);
    return hash != hash_;
}

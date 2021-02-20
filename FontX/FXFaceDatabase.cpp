#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <thread>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/complex.hpp>

#include "FXLib.h"
#include "FXFaceDatabase.h"
#include "FXFS.h"
#include "FXFTPrivate.h"

namespace {
    constexpr int FACE_DB_VERSION = 14;

    template <typename T, typename V, typename... Rest>
    void hashCombine(T & seed, const V& v, Rest... rest) {
        seed ^= std::hash<V>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    }

    size_t hashFile(const FXString & file, size_t hash) {
        hashCombine(hash, file);
        hashCombine(hash, FXFS::lastWriteTime(file));
        return hash;
    }

    size_t
    hashFiles(const FXSet<FXString> & files) {
        size_t hash = 0;
        hashCombine(hash, 0);
        for (const auto & f : files) 
            hash = hashFile(f, hash);
        
        return hash;
    }

    size_t hashFile(const FXString & file) {
        return hashFiles({file});
    }
}

namespace cereal  {
    template <class Archive>
    void
    serialize(Archive & ar, FXSFNTName & sfnt) {
        ar & sfnt.platformId;
        ar & sfnt.encodingId;
        ar & sfnt.language;
        ar & sfnt.nameId;
        ar & sfnt.value;
    }

    template <class Archive>
    void
    serialize(Archive & ar, FXFaceDescriptor & desc) {
        ar & desc.filePath;
        ar & desc.index;
    }

    template <class Archive>
    void
    serialize(Archive & ar, FXFaceAttributes & atts) {
        ar & atts.desc;
        ar & atts.upem;
        ar & atts.format;
        ar & atts.glyphCount;
        ar & atts.names;
        ar & atts.ascender;
        ar & atts.descender;
        ar & atts.haveUnicodeCMap;
        ar & atts.isCID;
        ar & atts.cid;
        ar & atts.isOpenTypeVariable;
        ar & atts.isMultipleMaster;
    }
        
    template <class Archive>
    void
    serialize(Archive & ar, FXFaceDatabase::FaceItem & item) {
        ar & item.desc;
        ar & item.atts;
    }


    template<class Archive>
    void 
    serialize(Archive & ar, FXFaceDatabase::FoldersHash & dbHash) {
        ar & dbHash.hash;
        ar & dbHash.files;
    }
}


FXFaceDatabase::FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath, ProgressCallback progressCallback)
    : folders_(folders)
    , dbPath_(dbPath)
    , progress_(progressCallback) {
    
    initDiskHash();

    if (!load() || (diskHash_.hash && diskHash_.hash != dbHash_.hash)) {
        rescan();
        save();
    }
}

FXFaceDatabase::FXFaceDatabase(const FXString & dbPath)
    : FXFaceDatabase({}, dbPath, nullptr) {
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

FXOpt<size_t>
FXFaceDatabase::findIndex(const FXString & psName) const {
    for (size_t i = 0; i < faces_.size(); ++ i) {
        auto & f =  faces_[i];
        if (f.atts.names.postscriptName() == psName)
            return i;
        for (auto [lang, name]: f.atts.names.localizedPostscriptNames()) {
            if (name == psName)
                return i;
        }
    }
    return FXNone<size_t>;
}

FXOpt<FXFaceDescriptor>
FXFaceDatabase::findDescriptor(const FXString & psName) const {
    if (auto idx = findIndex(psName))
        return faceDescriptor(*idx);
    else
        return FXNone<FXFaceDescriptor>;
}

namespace {
    template <typename K, typename V>
    bool haveSharedValue(const FXHashMap<K, V> & m1, const FXHashMap<K, V> & m2) {
        std::set<V> s1, s2, s;
        for (auto & [k, v]: m1) s1.insert(v);
        for (auto & [k, v]: m2) s2.insert(v);
        std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(s, s.begin()));
        return !s.empty();
    }
}

FXVector<size_t>
FXFaceDatabase::findFamily(const FXString & psName) const {
    auto index = findIndex(psName);
    if (!index)
        return {};
    const auto & face = faces_[*index];
    auto familyNames = face.atts.names.localizedFamilyNames();

    FXVector<size_t> ret;

    for (size_t i = 0; i < faces_.size(); ++ i) {
        auto & f = faces_[i];
        const auto names = f.atts.names.localizedFamilyNames();
        if (haveSharedValue(familyNames, names))
            ret.push_back(i);
    }
    
    return ret;
};

void
FXFaceDatabase::rescan() {
    
    // Rebuild face indexes in diskHash_
    for (size_t i = 0; i < faces_.size(); ++i)
        diskHash_.faces[faces_[i].desc.filePath].push_back(i);

    FXVector<FaceItem> faces;

    size_t current = 0;
    FXString lastFontFile;
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

        if (progress_ && !lastFontFile.empty())
            progress_(current, diskHash_.files.size(), lastFontFile);

        size_t count = 0;
        if (!FXFTCountFaces(FXLib::get(), file, count)) {
            lastFontFile = file;
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
}

bool
FXFaceDatabase::save() {
    try {
        std::ofstream ofs(dbPath_);
        cereal::JSONOutputArchive ar(ofs);
        ar << FACE_DB_VERSION;
        ar << diskHash_;
        ar << faces_;
        
        return true;
    }
    catch(cereal::Exception ex)
    {}
    return false;
}
    
bool
FXFaceDatabase::load() {
    try {
        std::ifstream ifs(dbPath_);
        cereal::JSONInputArchive ia(ifs);

        // Load and check version
        int dbVersion = -1;
        ia >> dbVersion;
        if (dbVersion != FACE_DB_VERSION)
            return false;
        
        ia >> dbHash_;
        ia >> faces_;
    }
    catch (cereal::Exception ex) {
        return false;
    }
    
    return !faces_.empty();
}

bool
FXFaceDatabase::initDiskHash() {
    diskHash_ = FoldersHash();
    for(const auto & folder : folders_)
        FXFS::foreachFile(folder, true, [this](const FXString & file) {
            diskHash_.hash = hashFile(file, diskHash_.hash);
            diskHash_.files[file] = hashFile(file);
            return true;
        });
    
    return diskHash_.hash != 0;
}

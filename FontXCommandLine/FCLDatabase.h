#pragma once
#include "FontX/FXFaceDatabase.h"

class FCLDatabase : public FXFaceDatabase{
public:
    using FXFaceDatabase::FXFaceDatabase;

    static FXPtr<const FCLDatabase>
    instance();
};


class FCLDatabaseProcessor {
public:
    virtual ~FCLDatabaseProcessor() {}

    virtual FXString
    name() const = 0;

    virtual void
    processDatabase(FXPtr<const FCLDatabase> db) = 0;
};


const FXVector<FXPtr<FCLDatabaseProcessor>> &
FCLGetDatabaseProcessors();

void
FCLAddDatabaseProcessors(FXPtr<FCLDatabaseProcessor> processor);

FXPtr<FCLDatabaseProcessor>
FCLFindDatabaseProcessors(const FXString & name);


template <typename T>
struct FCLDatabaseProcessorAutoRegister {
    FCLDatabaseProcessorAutoRegister() {
        FCLAddDatabaseProcessors(std::make_shared<T>());
    }
};

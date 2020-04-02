#pragma once
#include "FontX/FXFaceDatabase.h"

class FCLDatabase : public FXFaceDatabase{
public:
    using FXFaceDatabase::FXFaceDatabase;

    static FXPtr<FCLDatabase>
    instance();
};

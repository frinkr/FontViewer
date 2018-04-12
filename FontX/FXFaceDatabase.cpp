#include <sqlite3.h>
#include "FXFaceDatabase.h"

FXFaceDatabase::FXFaceDatabase(const FXVector<FXString> & folders, const FXString & dbPath)
    : folders_(folders)
    , dbPath_(dbPath) {
    sqlite3_libversion();
}

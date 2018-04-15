#include "FXFTPrivate.h"

FT_Error FXFilePathToOpenArgs(const FXString & filePath, FT_Open_Args * args) {
    memset(args, 0, sizeof(FT_Open_Args));
    args->flags    = FT_OPEN_PATHNAME;
    args->pathname = (char*)filePath.c_str();
    args->stream   = NULL;
    return FT_Err_Ok;
};

FT_Error FXFTCountFaces(FXFTLibrary lib, const FXString & filePath, size_t & count) {
    FT_Error error = FT_Err_Ok;    

    FT_Face  face;
    FT_Open_Args  args;
        
    error = FXFilePathToOpenArgs(filePath, &args);
    if (error) return  error;
        
    error = FT_Open_Face(lib, &args, -1, &face);
    if (error) return error;
        
    count = face->num_faces;
    FT_Done_Face(face);

    return error;
}

FT_Error
FXFTEnumurateFaces(FXFTLibrary lib, const FXString & filePath, std::function<bool(FXFTFace face, size_t index)> callback) {
    FT_Error error = FT_Err_Ok;    
        
    size_t numFaces;
    error = FXFTCountFaces(lib, filePath, numFaces);
    if (error) return error;
        
    // new ft face
    for (size_t i = 0; i < numFaces; i++ ) {
        FT_Face  face;
        FT_Open_Args  args;
            
        error = FXFilePathToOpenArgs(filePath, &args);
        if (error) return error;
            
        error = FT_Open_Face(lib, &args, FT_Long(i), &face);
        if (error) return error;

        bool continueLoop = callback(face, i);
        FT_Done_Face(face);
        if (!continueLoop)
            break;
    }
    return FT_Err_Ok;
}

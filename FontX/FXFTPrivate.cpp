#include "FXFTPrivate.h"
#include "FXFS.h"

#if FX_WIN
#include <stdio.h>
#include <locale>
#include <codecvt>
#endif

namespace {
#if FX_WIN
    // adapted from freetype/ftsystem.c
    unsigned long
    streamRead( FT_Stream       stream,
                 unsigned long   offset,
                 unsigned char*  buffer,
                 unsigned long   count ) {
        FILE*  file;

        if ( !count && offset > stream->size )
            return 1;

        file = (FILE*)stream->descriptor.pointer;

        if ( stream->pos != offset )
            fseek( file, (long)offset, SEEK_SET );

        return (unsigned long)fread( buffer, 1, count, file );        
    }

    // adapted from freetype/ftsystem.c
    void
    streamClose( FT_Stream  stream ) {
        FILE * file = (FILE*)stream->descriptor.pointer;
        if (file)
            fclose(file);
        
        free(stream->pathname.pointer);
        
        stream->descriptor.pointer = NULL;
        stream->pathname.pointer   = NULL;
        stream->size               = 0;
        stream->base               = NULL;

        // free the stream created by calloc (file_path_to_open_args)
        free(stream);
    }
    
    FT_Error
    streamFromUTF8FilePath(const char * filePath, FT_Stream stream) {
        static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        std::wstring wFilePath = convert.from_bytes(filePath);
        FILE * file = _wfopen(wFilePath.c_str(), L"rb");

        if (!file)
            return FT_Err_Cannot_Open_Resource;

        fseek(file, 0, SEEK_END);
        stream->size = ftell(file);
        if (!stream->size) {
            fclose(file);
            return FT_Err_Cannot_Open_Resource;
        }
        
        fseek(file, 0, SEEK_SET);
        
        stream->descriptor.pointer = file;
        stream->pathname.pointer = (void*)strdup(filePath);
        stream->read = streamRead;
        stream->close = streamClose;


        return FT_Err_Ok;
    }
#endif

}

FT_Error FXFilePathToOpenArgs(const FXString & filePath, FT_Open_Args * args) {
    memset(args, 0, sizeof(FT_Open_Args));
#if FX_WIN
    FT_Stream stream = (FT_Stream)calloc(1, sizeof(FT_StreamRec));
    FT_Error error = streamFromUTF8FilePath(filePath.c_str(), stream);
    if (error) {
        streamClose(stream);
        return error;
    }
        
    args->flags    = FT_OPEN_STREAM;
    args->pathname = NULL;
    args->stream   = stream;

#else
    args->flags    = FT_OPEN_PATHNAME;
    args->pathname = (char*)filePath.c_str();
    args->stream   = NULL;
#endif
    return FT_Err_Ok;
};


namespace {
    unsigned long
    FXStreamRead(FT_Stream       stream,
                 unsigned long   offset,
                 unsigned char*  buffer,
                 unsigned long   count ) {
        FXPtr<FXStream> * s = (FXPtr<FXStream>*)stream->descriptor.pointer;
        (*s)->seek(offset);
        return (unsigned long)(*s)->read(buffer, count);
    }

    void
    FXStreamClose( FT_Stream  stream ) {
        FXPtr<FXStream> * s = (FXPtr<FXStream>*)stream->descriptor.pointer;
        if (s) {
            (*s)->close();
            delete s;
        }
        stream->descriptor.pointer = NULL;
        stream->pathname.pointer   = NULL;
        stream->size               = 0;
        stream->base               = NULL;
        // free the stream created by calloc (FXStreamToOpenArgs)
        free(stream);
    }

}

FT_Error FXStreamToOpenArgs(FXPtr<FXStream> stream, FT_Open_Args * args) {
    memset(args, 0, sizeof(FT_Open_Args));

    FT_Stream ftStream = (FT_Stream)calloc(1, sizeof(FT_StreamRec));
    ftStream->descriptor.pointer = new FXPtr<FXStream>(stream);
    ftStream->size = stream->size();
    ftStream->read = FXStreamRead;
    ftStream->close = FXStreamClose;
        
    args->flags    = FT_OPEN_STREAM;
    args->pathname = NULL;
    args->stream   = ftStream;
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

FT_Error
FXFTOpenFace(FXFTLibrary lib, const FXString & filePath, size_t index, FXFTFace * face) {
    FT_Error error = FT_Err_Ok;
    FT_Open_Args  args;
            
    error = FXFilePathToOpenArgs(filePath, &args);
    if (error)
        return error;

    return FT_Open_Face(lib, &args, FT_Long(index), face);
}

FT_Error
FXFTOpenFace(FXFTLibrary lib, FXPtr<FXStream> stream, size_t index, FXFTFace * face) {
    FT_Error error = FT_Err_Ok;
    FT_Open_Args  args;
            
    error = FXStreamToOpenArgs(stream, &args);
    if (error)
        return error;

    return FT_Open_Face(lib, &args, FT_Long(index), face);
}


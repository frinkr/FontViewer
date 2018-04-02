#pragma once

#include <cassert>
#include <memory>

template <typename T>
using FXPtr = std::shared_ptr<T>;

struct FT_LibraryRec_;
struct FT_FaceRec_;
struct hb_face_t;
struct hb_font_t;

using TXFTLibrary = struct FT_LibraryRec_ *;
using TXFTFace    = struct FT_FaceRec_ *;
using TXHBFace    = struct hb_face_t *;
using TXHBFont    = struct hb_font_t *;

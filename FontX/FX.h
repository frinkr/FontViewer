#pragma once

#include <cassert>
#include <memory>
#include <string>

#include <map>
#include <vector>

template <typename T>
using FXPtr = std::shared_ptr<T>;

struct FT_LibraryRec_;
struct FT_FaceRec_;
struct hb_face_t;
struct hb_font_t;

using FXFTLibrary = struct FT_LibraryRec_ *;
using FXFTFace    = struct FT_FaceRec_ *;
using FXHBFace    = struct hb_face_t *;
using FXHBFont    = struct hb_font_t *;


using FXChar      = uint32_t;
using FXGlyphID   = uint32_t;

constexpr FXChar UnicodeMax = 0x10FFFF;
constexpr FXGlyphID NotDef  = 0;


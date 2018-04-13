#pragma once

#include <cassert>
#include <algorithm>
#include <memory>
#include <string>

#include <map>
#include <vector>
#include <set>

template <typename T>
using FXPtr = std::shared_ptr<T>;
template <typename T>
using FXWeakPtr = std::weak_ptr<T>;
template <typename T>
using FXVector = std::vector<T>;
template <typename T>
using FXSet = std::set<T>;
using FXString = std::string;

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

constexpr FXChar FXCharMax     = 0x10FFFF;
constexpr FXChar FXCharInvalid = FXCharMax + 1;
constexpr FXGlyphID NotDef     = 0;

/**
 * A closed range
 */
struct FXCharRange {
    FXChar from;
    FXChar to;
};

using fu = short;  // font units


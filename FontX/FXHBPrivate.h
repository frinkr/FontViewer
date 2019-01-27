#pragma once

#include <hb.h>
#include <hb-ft.h>
#include <hb-ot.h>
#include "FXFace.h"

void
FXHBCreateFontFace(FXFace * face, hb_font_t ** hbFont, hb_face_t ** hbFace);

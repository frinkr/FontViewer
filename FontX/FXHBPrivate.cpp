#include "FXFace.h"
#include "FXHBPrivate.h"

void
FXHBCreateFontFace(FXFace * face, hb_font_t ** hbFont, hb_face_t ** hbFace) {
    FT_Face ftFace = face->face();
        
    *hbFont = hb_ft_font_create_referenced(ftFace);
    *hbFace = hb_font_get_face(*hbFont);
        
    hb_ot_font_set_funcs(*hbFont);
    unsigned int upem = hb_face_get_upem(*hbFace);
    hb_font_set_scale(*hbFont, upem, upem);
}

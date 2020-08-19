if (MUPDF_INCLUDE_DIR AND MUPDF_LIBRARY)
  set(MUPDF_FOUND TRUE)
else (MUPDF_INCLUDE_DIR AND MUPDF_LIBRARY)
  find_path(MUPDF_INCLUDE_DIR mupdf/fitz.h)
  find_library(MUPDF_LIBRARY NAMES mupdf)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(MuPDF DEFAULT_MSG MUPDF_LIBRARY MUPDF_INCLUDE_DIR)
  
  mark_as_advanced(MUPDF_LIBRARY MUPDF_INCLUDE_DIR)  
endif (MUPDF_INCLUDE_DIR AND MUPDF_LIBRARY)

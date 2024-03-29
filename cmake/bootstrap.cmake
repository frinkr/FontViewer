# Moudle path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if (WIN32)
  # Set vkpkg tool chain file
  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    file(TO_CMAKE_PATH "${VCPKG_TOOLCHAIN_FILE}" VCPKG_TOOLCHAIN_FILE)
    if (EXISTS "${VCPKG_TOOLCHAIN_FILE}")
      set(CMAKE_TOOLCHAIN_FILE ${VCPKG_TOOLCHAIN_FILE})
      include(${VCPKG_TOOLCHAIN_FILE})
    endif()
  endif()
  
elseif (APPLE)
  execute_process(COMMAND brew --prefix qt OUTPUT_VARIABLE QT_ROOT)
  string(REGEX REPLACE "\n$" "" QT_ROOT "${QT_ROOT}")
  list(APPEND CMAKE_PREFIX_PATH ${QT_ROOT})

  execute_process(COMMAND brew --prefix icu4c OUTPUT_VARIABLE ICU_ROOT)
  string(REGEX REPLACE "\n$" "" ICU_ROOT "${ICU_ROOT}")
  list(APPEND CMAKE_PREFIX_PATH ${ICU_ROOT})
  
elseif (UNIX)
  # Nothing to configure
endif()

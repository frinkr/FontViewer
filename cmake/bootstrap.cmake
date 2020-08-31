# Moudle path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if (WIN32)
  # Set vkpkg tool chain file
  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_HOME})
    set(VCPKG_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")
    file(TO_CMAKE_PATH "${VCPKG_TOOLCHAIN_FILE}" VCPKG_TOOLCHAIN_FILE)
    if (EXISTS "${VCPKG_TOOLCHAIN_FILE}")
      set(CMAKE_TOOLCHAIN_FILE ${VCPKG_TOOLCHAIN_FILE})
      include(${VCPKG_TOOLCHAIN_FILE})
    endif()
  endif()
  
elseif (APPLE)
  message(FATAL_ERROR "Not implemented")
  # Qt
  # OpenSSL
elseif (UNIX)
  # Nothing to configure
endif()

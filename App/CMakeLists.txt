cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME FontViewer)
project(${PROJECT_NAME})

if (APPLE)
  set(
    RESOURCE_FILES
    macOS/Assets.xcassets
    )
  add_executable(
    ${PROJECT_NAME}
    MACOSX_BUNDLE
    main.cpp
    ${RESOURCE_FILES}
    )
elseif(WIN32)
  set(
    RESOURCE_FILES
    win32/Resource.h
    win32/Resource.rc
    )
  add_executable(
    ${PROJECT_NAME}
	WIN32
    main.cpp
    ${RESOURCE_FILES}
    )

  # Disable console in Release
  #if(CMAKE_BUILD_TYPE STREQUAL "Release")
  #  set_property(TARGET main PROPERTY WIN32_EXECUTABLE true)
  #endif()

else()
  add_executable(
    ${PROJECT_NAME}
    main.cpp
    )
endif()

target_link_libraries(${PROJECT_NAME} FontX UI-Qt)

if (APPLE)
  
  set(APP_VERSION "1.0.0" CACHE STRING "The version of the application")
  set(APP_NAME ${PROJECT_NAME})

  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    MACOSX_BUNDLE YES

    # bundle config
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/macOS/plist.in
    MACOSX_BUNDLE_ICON_FILE "AppIcon"
    MACOSX_BUNDLE_BUNDLE_VERSION ${APP_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_VERSION}
    MACOSX_BUNDLE_LONG_VERSION_STRING ${APP_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2020 Yuqing Jiang. All rights reserved."
    
    XCODE_ATTRIBUTE_PRODUCT_NAME   ${APP_NAME}
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER   "me.frinkr.fontviewer"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    XCODE_ATTRIBUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/ProductRelease"
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"

    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.11"

    XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""

    RESOURCE "${RESOURCE_FILES}"    
    )

  # Get reference to deployqt
  macro(FindDeployQt)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    get_target_property(uic_location Qt5::uic IMPORTED_LOCATION)
    get_filename_component( _qt_dir ${uic_location} DIRECTORY)
    set(deployqt "${_qt_dir}/macdeployqt")
    if(NOT EXISTS ${deployqt})
      message(FATAL_ERROR "Failed to locate deployqt executable: [${deployqt}]")
      unset(deployqt)
    endif()
  endmacro()

  FindDeployQt()

  #ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
  #  POST_BUILD
  #  COMMAND "${deployqt}" "${CMAKE_CURRENT_BINARY_DIR}/\${CONFIGURATION}/${APP_NAME}.app" "-verbose=2"
  #  #"-codesign=V45C82Y9RF"
  #)

endif()

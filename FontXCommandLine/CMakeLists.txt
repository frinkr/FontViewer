cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME FontXCommandLine)

project(${PROJECT_NAME})

find_package(Qt6Core REQUIRED)


if (NOT WIN32)
  find_package(PkgConfig)
endif()

if (WIN32)
  find_package(ICU REQUIRED uc)
  find_package(HARFBUZZ REQUIRED harfbuzz)
else()
  pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
  find_package(ICU COMPONENTS uc REQUIRED)
endif()

add_executable(${PROJECT_NAME}
  main.cpp
  FCL.h
  FCL.cpp
  FCLDatabase.h
  FCLDatabase.cpp
  FCLDump.cpp
  FCLDumpKern.cpp
  FCLPlayground.cpp
  FCLDumpTextInShape.cpp
  FCLShaping.cpp
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE
  FCL_RESOURCES_DIR=\"${PROJECT_SOURCE_DIR}/Resources\")

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}
  ${ICU_INCLUDE_DIRS}
  ${HARFBUZZ_INCLUDE_DIRS}  
  )

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt6::Core
  FontX
  ${ICU_LIBRARIES}
  ${HARFBUZZ_LIBRARIES})


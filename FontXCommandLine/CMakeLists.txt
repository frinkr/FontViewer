cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME FontXCommandLine)

project(${PROJECT_NAME})

find_package(Qt5Core REQUIRED)

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
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE
  FCL_RESOURCES_DIR=\"${PROJECT_SOURCE_DIR}/Resources\")

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}
  )

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core FontX)


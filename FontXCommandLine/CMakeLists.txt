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
  FCLPDump.cpp
  )

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}
  )

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core FontX)


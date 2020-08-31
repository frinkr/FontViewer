include(${CMAKE_SOURCE_DIR}/cmake/Resources.cmake)

if(WIN32)
  ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_SOURCE_DIR}/Resources/UCD"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/UCD"
    )
endif()

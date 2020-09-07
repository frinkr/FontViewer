function(CopyResource fileOrDir)
  if(IS_DIRECTORY "${fileOrDir}")
    get_filename_component(name "${fileOrDir}" NAME)
    
    if (APPLE)
      ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${fileOrDir}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/${name}"
        )
    else()
      ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${fileOrDir}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/${name}"
        )
    endif()
  else()
    message(FATAL_ERROR "Not Implemented!")
  endif()
endfunction()

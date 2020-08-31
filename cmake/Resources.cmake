function(CopyResource fileOrDir)
  if (WIN32)
    if(IS_DIRECTORY "${fileOrDir}")

      get_filename_component(name "${fileOrDir}" NAME)
      
      ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${fileOrDir}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/${name}"
        )
    else()
      message(FATAL_ERROR "Not Implemented!")
    endif()
  else()
    message(FATAL_ERROR "Not Implemented!")
  endif()
endfunction()

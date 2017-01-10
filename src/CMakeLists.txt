add_executable(vrproj
  ${SHARED_SRC_FILES}
  main.cpp
)

target_link_libraries(vrproj
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${OPENVR_LIBRARIES}
  ${CMAKE_DL_LIBS}
)

setTargetOutputDirectory(vrproj)

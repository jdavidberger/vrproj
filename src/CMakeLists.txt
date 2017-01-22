set(RESOURCES)

set_property( GLOBAL PROPERTY USE_FOLDERS ON)
add_custom_target(SHADERS )
set_property(TARGET SHADERS PROPERTY FOLDER "Shader Projects")

file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/Resources/Resources.h "")
macro(generate_resource filename) 
    get_filename_component(name ${filename} NAME)		 
    string(REPLACE "." "_" name ${name} )     
 
    add_custom_target(${name}_shader	
        COMMAND ${CMAKE_COMMAND} -DDESTINATION=${CMAKE_CURRENT_BINARY_DIR} -Dfilename=${filename} -P ${CMAKE_CURRENT_SOURCE_DIR}/CreateShaderFiles.cmake 
        DEPENDS ${filename} ${CMAKE_CURRENT_SOURCE_DIR}/CreateShaderFiles.cmake 
        COMMENT "Generating shader ${filename}..."
		SOURCES ${filename} ${CMAKE_CURRENT_SOURCE_DIR}/CreateShaderFiles.cmake 
    )
    add_dependencies(SHADERS ${name}_shader)
    list(APPEND RESOURCES "${filename}")
	set_property(TARGET ${name}_shader PROPERTY FOLDER "Shader Projects")
    file (APPEND ${CMAKE_CURRENT_BINARY_DIR}/Resources/Resources.h "#include \"${CMAKE_CURRENT_BINARY_DIR}/Resources/${name}.h\"\n")
endmacro()

add_definitions(-DGLEW_STATIC)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Resources )

FILE(GLOB resources_src "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*")

FOREACH(rsrc ${resources_src})
	generate_resource(${rsrc})	
ENDFOREACH()
 
 add_subdirectory(GeomLib)
 
add_library(vrprojLib 
  ${CMAKE_CURRENT_BINARY_DIR}/Resources/Resources.h  
  ${SHARED_SRC_FILES}
  ${RESOURCES} 
  ObjectBuffer.cc ObjectBuffer.h      
  DrawText.h DrawText.cc
  GlUtils.h GlUtils.cc
  )    
add_dependencies(vrprojLib SHADERS)

target_link_libraries( vrprojLib
	  ${OPENGL_LIBRARIES}   
	  GLEW::GLEW
	  opencv_core
	  SDL2::SDL2 
      GeomLib
	  ${OPENVR_LIBRARY}    
	  ${CMAKE_DL_LIBS} 
	  GLUT::GLUT
	  ${FREETYPE_LIBRARY}
	  )
	   
add_executable(vrproj
  main.cpp  
)
 
target_link_libraries(vrproj vrprojLib)

add_executable(vrprojTest test.cc)
target_link_libraries(vrprojTest vrprojLib GeomLib)

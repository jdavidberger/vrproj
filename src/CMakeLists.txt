set(RESOURCES)
file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/Resources/Resources.h "")
macro(generate_resource filename)
	set(name ${ARGN}) 
	if(name MATCHES "")
		get_filename_component(name ${filename} NAME)		
		string(REPLACE "." "_" name ${name})     
	endif() 
	   
	file(READ ${filename} contents)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Resources/${name}.h "namespace Resources { extern const char* ${name}; }")
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp "namespace Resources { const char* ${name} = R\"-_+*delimiter(${contents})-_+*delimiter\"; }")
	list(APPEND RESOURCES "${filename}" "${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/Resources/${name}.h")
	file (APPEND ${CMAKE_CURRENT_BINARY_DIR}/Resources/Resources.h "#include \"${CMAKE_CURRENT_BINARY_DIR}/Resources/${name}.h\"\n")
endmacro()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/resources)

FILE(GLOB resources_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*")

FOREACH(rsrc ${resources_src})
	generate_resource(${rsrc})	
ENDFOREACH()

add_executable(vrproj
  ${CMAKE_CURRENT_BINARY_DIR}/Resources/Resources.h
  ${SHARED_SRC_FILES}
  main.cpp  
  ${RESOURCES}
)
 
target_link_libraries(vrproj
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${OPENVR_LIBRARIES}
  ${CMAKE_DL_LIBS}
)

setTargetOutputDirectory(vrproj)

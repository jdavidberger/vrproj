add_subdirectory(ThirdParty)

add_definitions(-DNO_TIMER -DSINGLE -DANSI_DECLARATORS -DTRILIBRARY)
include_directories( ./ThirdParty/triangle . ) 

add_library(GeomLib
    Polygon.h Polygon.cpp Polytope.h ./ThirdParty/triangle/triangle.c ./ThirdParty/triangle/triangle.h
  )    

target_link_libraries( GeomLib
	  opencv_core
     )
	       
target_include_directories(GeomLib PUBLIC . ./ThirdParty/triangle/)
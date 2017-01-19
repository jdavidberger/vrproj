set(name "") 
if(name MATCHES "") 
    get_filename_component(name ${filename} NAME)		 
    string(REPLACE "." "_" name ${name} )     
endif()      
        
file(READ ${filename} contents)
file(WRITE ${DESTINATION}/Resources/${name}.h "namespace Resources { const char* ${name} = R\"-_+*delimiter(${contents})-_+*delimiter\"; }")

set(glad_path ${CMAKE_SOURCE_DIR}/libs/glad)

set(glad_source 
            ${glad_path}/include/EGL/eglplatform.h 
            ${glad_path}/include/glad/egl.h 
            ${glad_path}/include/glad/gl.h 
            ${glad_path}/include/glad/gles1.h 
            ${glad_path}/include/glad/gles2.h  
            ${glad_path}/include/KHR/khrplatform.h 
            ${glad_path}/src/gl.c 
            ${glad_path}/src/egl.c 
            ${glad_path}/src/gles1.c 
            ${glad_path}/src/gles2.c)

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)

else()
        
    if(CMAKE_SYSTEM_NAME STREQUAL Darwin)

    elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
        list(APPEND glad_source 
                ${glad_path}/include/glad/wgl.h  
                ${glad_path}/src/wgl.c)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
        list(APPEND glad_source     
                ${glad_path}/include/glad/glx.h 
                ${glad_path}/src/glx.c )
    elseif (CMAKE_SYSTEM_NAME STREQUAL Android) 
    endif()
endif()

add_library(glad STATIC ${glad_source})

target_include_directories(glad PRIVATE ${glad_path}/include)

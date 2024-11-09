set(TARGET TinyCore)
set(CURRENT_PATH "${CMAKE_SOURCE_DIR}/sources/core")
set(CORE_DEPENDCIES "")
#=== LIBRARY
set(TARGET_SOURCE 
    "${CURRENT_PATH}/graphics/platforms/OpenGL/OpenGLUtils.h" 
   "${CURRENT_PATH}/graphics/platforms/OpenGL/OpenGLUtils.cpp")
   
if(TINYUI_EXPORT_DYNAMIC_LIBRARY_MODULE)
   add_library(${TARGET} STATIC ${TARGET_SOURCE})
else()
   add_library(${TARGET} STATIC ${TARGET_SOURCE})
endif()

set(NATIVE_LIBS "")
target_include_directories(${TARGET} PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Vendor/OpenGL>"  
    "$<INSTALL_INTERFACE:include>"
)

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    message(STATUS "Configuration OSX")
    target_compile_options(${TARGET} PRIVATE -x objective-c)
    list(APPEND CORE_DEPENDCIES 
            "-framework QuartzCore" 
            "-framework Cocoa" 
            "-framework MetalKit" 
            "-framework Metal" )
elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "Configuration Window")
        # OpenGL
        find_package(OpenGL REQUIRED)
        list(APPEND CORE_DEPENDCIES  ${OPENGL_gl_LIBRARY})

elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
    message(STATUS "Configuration Linux")    
    find_package(X11 REQUIRED)
    message(STATUS "X11_FOUND = ${X11_FOUND}")
    message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
    list(APPEND CORE_DEPENDCIES X11 Xi Xcursor GL dl m Threads::Threads)
elseif (CMAKE_SYSTEM_NAME STREQUAL Android) 
    message(STATUS "Configuration Android")
endif()

target_link_libraries(${TARGET} PUBLIC ${CORE_DEPENDCIES})


set(INSTALL_TARGETS_LIST "imgui;stb;cglm;${TARGET}" CACHE INTERNAL  "LIBS TARGET INSTAL NAME")
set(LIBS_LIST "imgui;stb;cglm;${TARGET};${CORE_DEPENDCIES}" CACHE INTERNAL  "LIBS TARGET NAME")

set(TARGET GraphicsBox)
set(TARGET_SOURCE "")
set(LIBS "")

#=== EXECUTABLE: ${TARGET}
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(${TARGET} WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET})
else()
    add_executable(${TARGET})
endif()


target_include_directories(${TARGET} PRIVATE "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/sources/>" 
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/sources/vendor/OpenGL>"  
    "$<INSTALL_INTERFACE:include>"
)


message(STATUS "SYSTEM ${CMAKE_SYSTEM_NAME}")
# Emscripten-specific linker options
if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    message(STATUS "Configuration Emscripten")
    list(APPEND TARGET_SOURCE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/wasm/main.cpp>")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${TARGET} PRIVATE -sEXPORTED_FUNCTIONS=_main)
    # File system support: https://emscripten.org/docs/api_reference/Filesystem-API.html#including-file-system-support
    target_link_options(${TARGET} PRIVATE -sWASMFS -sFORCE_FILESYSTEM -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','FS'])
    # use our own minimal shell.html
    target_link_options(${TARGET} PRIVATE --shell-file "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/wasm/html/shell.html>")
    # link with WebGL2
    target_link_options(${TARGET} PRIVATE -sUSE_WEBGL2=1 --bind -sALLOW_MEMORY_GROWTH=1 -sNO_EXIT_RUNTIME=1  ${EXTRA_FLAGS}  --preload-file ${PROJECT_SOURCE_DIR}/assets@/ )
    # link with WebGPU 
    #-sASYNCIFY # Required by WebGPU-C++
    target_link_options(${TARGET} PRIVATE -sUSE_WEBGPU -sASYNCIFY)
    # WASM+JS size optimizations
    target_link_options(${TARGET} PRIVATE -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1)

    if (TINYUI_WASM_SINGLE_FILE)
        set(EXTRA_FLAGS "-s SINGLE_FILE=1")
        message(STATUS "Embedding WASM inside .js")
    endif()

    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "--preload-file ${PROJECT_SOURCE_DIR}/assets/ ${EXTRA_FLAGS} ")
else()
        # explicitly strip dead code
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_link_options(${TARGET} PRIVATE LINKER:-dead_strip)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        message(STATUS "Configuration OSX")
        list(APPEND TARGET_SOURCE  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/osx/main.swift>")
        target_compile_options(${TARGET} PRIVATE -x objective-c)
        list(APPEND LIBS "-framework QuartzCore" "-framework Cocoa" "-framework MetalKit" "-framework Metal" )
    elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "Configuration Window")
        list(APPEND TARGET_SOURCE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/windows/main.cpp>")
        if(MSVC)
        set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET}>"
                        VS_DEBUGGER_COMMAND           "$<TARGET_FILE:${TARGET}>"
                        VS_DEBUGGER_ENVIRONMENT       "PATH=%PATH%;${CMAKE_SOURCE_DIR}/bin")
        endif()
        find_package(OpenGL REQUIRED)
        list(APPEND LIBS  ${OPENGL_gl_LIBRARY})
    elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
        list(APPEND TARGET_SOURCE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/linux/main.cpp>")
        list(APPEND LIBS X11 Xi Xcursor GL dl m Threads::Threads)    
    elseif (CMAKE_SYSTEM_NAME STREQUAL Android) 
        message(STATUS "Configuration Android")
    endif()
endif()

target_sources(${TARGET} PUBLIC ${TARGET_SOURCE})
target_link_libraries(${TARGET} PUBLIC ${LIBS})
# Set the source and destination paths for the folder
set(ASSET_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets/")
set(ASSET_DESTINATION_DIR "${CMAKE_BINARY_DIR}/assets/")

# Add a custom command to copy the folder after building
add_custom_command(
    TARGET ${TARGET}    # The target after which this should be done
    PRE_BUILD          # Ensures the command is executed after the target is built
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SOURCE_DIR} ${ASSET_DESTINATION_DIR}
    COMMENT "Copying asset folder after build..."
)

# Optionally, add a custom target to ensure the copy happens as part of the build process
add_custom_target(copy_assets ALL DEPENDS ${TARGET})
list(APPEND INSTALL_TARGETS_LIST ${TARGET})
install(TARGETS ${INSTALL_TARGETS_LIST}
        EXPORT GraphicsBoxTargets
        LIBRARY DESTINATION "${PACKAGE_INSTALL_PATH}/lib/"
        ARCHIVE DESTINATION "${PACKAGE_INSTALL_PATH}/lib/"
        RUNTIME DESTINATION "${PACKAGE_INSTALL_PATH}/bin/")

install(EXPORT GraphicsBoxTargets
        FILE GraphicsBoxTargets.cmake
        NAMESPACE GraphicsBox::
        DESTINATION "${PACKAGE_INSTALL_PATH}/cmake/GraphicsBox/")
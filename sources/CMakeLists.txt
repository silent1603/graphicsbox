set(TARGET core)
set(TARGET_SOURCE "")
#=== EXECUTABLE: ${TARGET}
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(${TARGET} WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET})
else()
    add_executable(${TARGET})
endif()



target_include_directories(${TARGET} PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libs/imgui/>" 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libs/imgui/backends/>" 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libs/stb/>" 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libs/cglm/include/>" 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libs/glad/include/>" 
    "$<INSTALL_INTERFACE:include>"
)

set(common_lib imgui stb cglm)
target_link_libraries(${TARGET} PUBLIC ${common_lib})

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
    target_link_options(${TARGET} PRIVATE --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/entry/wasm/html/shell.html)
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
target_link_libraries(${TARGET} PUBLIC glad)
    if(Vulkan_FOUND)
        # Include Vulkan directories
        target_include_directories(${TARGET} PRIVATE ${Vulkan_INCLUDE_DIR})
        # Link Vulkan libraries
        target_link_libraries(${TARGET} PRIVATE ${Vulkan_LIBRARIES})
    endif()

    # explicitly strip dead code
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_link_options(${TARGET} PRIVATE LINKER:-dead_strip)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        message(STATUS "Configuration OSX")
        list(APPEND TARGET_SOURCE  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/osx/main.mm>")
        target_compile_options(${TARGET} PRIVATE -x objective-c)
        target_link_libraries(${TARGET} PUBLIC
            "-framework QuartzCore"
            "-framework Cocoa"
            "-framework MetalKit"
            "-framework Metal")
    elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "Configuration Window")
        list(APPEND TARGET_SOURCE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/win32/main.cpp>")
        # OpenGL
        find_package(OpenGL REQUIRED)
        target_link_libraries(${TARGET} PUBLIC ${OPENGL_gl_LIBRARY})
        if(MSVC)
            set_property(TARGET ${TARGET} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY " $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/bin/>")
        endif()
    elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
        message(STATUS "Configuration Linux")
        find_package(X11 REQUIRED)
        message(STATUS "X11_FOUND = ${X11_FOUND}")
        message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
        message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
        list(APPEND TARGET_SOURCE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entry/linux/main.cpp>")
        target_link_libraries(${TARGET} PUBLIC X11 Xi Xcursor GL dl m)
        target_link_libraries(${TARGET} PUBLIC Threads::Threads)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Android) 
        message(STATUS "Configuration Android")
    endif()
endif()

target_sources(${TARGET} PUBLIC ${TARGET_SOURCE})

# Set the source and destination paths for the folder
set(ASSET_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../assets/")
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
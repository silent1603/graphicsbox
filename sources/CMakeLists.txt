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
    ${CMAKE_SOURCE_DIR}/libs/imgui
    ${CMAKE_SOURCE_DIR}/libs/imgui/backends
    ${CMAKE_SOURCE_DIR}/libs/stb 
    ${CMAKE_SOURCE_DIR}/libs/cglm/include 
    ${CMAKE_SOURCE_DIR}/libs/glad/include
)

set(common_lib imgui stb cglm)
target_link_libraries(${TARGET} PUBLIC ${common_lib})

message(STATUS "SYSTEM ${CMAKE_SYSTEM_NAME}")
# Emscripten-specific linker options
if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    message(STATUS "Configuration Emscripten")
    list(APPEND TARGET_SOURCE cpp/entry/wasm/main.cpp)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${TARGET} PRIVATE -sEXPORTED_FUNCTIONS=_main)
    # File system support: https://emscripten.org/docs/api_reference/Filesystem-API.html#including-file-system-support
    target_link_options(${TARGET} PRIVATE -sWASMFS -sFORCE_FILESYSTEM -sEXPORTED_RUNTIME_METHODS=['FS'])
    # use our own minimal shell.html
    target_link_options(${TARGET} PRIVATE --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/html/shell.html)
    # link with WebGL2
    target_link_options(${TARGET} PRIVATE -sUSE_WEBGL2=1 --bind -sALLOW_MEMORY_GROWTH=1 -sNO_EXIT_RUNTIME=0  ${EXTRA_FLAGS}  --preload-file ${PROJECT_SOURCE_DIR}/assets@/ )
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
        list(APPEND TARGET_SOURCE cpp/entry/osx/main.mm)
        target_compile_options(${TARGET} PRIVATE -x objective-c)
        target_link_libraries(${TARGET} PUBLIC
            "-framework QuartzCore"
            "-framework Cocoa"
            "-framework MetalKit"
            "-framework Metal")
    elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "Configuration Window")
        list(APPEND TARGET_SOURCE cpp/entry/win32/main.cpp)
        # find_package(OpenGL REQUIRED COMPONENTS OpenGL)
        # target_link_libraries(${TARGET} PUBLIC ${OPENGL_gl_LIBRARY})
        # message(STATUS ${OPENGL_INCLUDE_DIR})
        if(MSVC)
            set_property(TARGET ${TARGET} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
        endif()
    elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
        message(STATUS "Configuration Linux")
        list(APPEND TARGET_SOURCE cpp/entry/wasm/main.cpp)
        target_link_libraries(${TARGET} INTERFACE X11 Xi Xcursor GL dl m)
        target_link_libraries(${TARGET} PUBLIC Threads::Threads)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
        message(STATUS "Configuration Android")
        target_link_libraries(${TARGET} INTERFACE X11 Xi Xcursor GL dl m)
        target_link_libraries(${TARGET} PUBLIC Threads::Threads)
    
    endif()
endif()

target_sources(${TARGET} PUBLIC ${TARGET_SOURCE})

# Set the source and destination paths for the folder
set(ASSET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)
set(ASSET_DESTINATION_DIR ${CMAKE_BINARY_DIR}/assets)

# Add a custom command to copy the folder after building
add_custom_command(
    TARGET ${TARGET}    # The target after which this should be done
    PRE_BUILD          # Ensures the command is executed after the target is built
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SOURCE_DIR} ${ASSET_DESTINATION_DIR}
    COMMENT "Copying asset folder after build..."
)

# Optionally, add a custom target to ensure the copy happens as part of the build process
add_custom_target(copy_assets ALL DEPENDS ${TARGET})
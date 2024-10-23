set(imgui_path ${CMAKE_SOURCE_DIR}/libs/imgui)

set(imgui_source     
${imgui_path}/imgui.cpp
${imgui_path}/imgui.h
${imgui_path}/imgui.cpp
${imgui_path}/imgui.h
${imgui_path}/imgui_widgets.cpp
${imgui_path}/imgui_draw.cpp
${imgui_path}/imgui_tables.cpp
${imgui_path}/imgui_demo.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    list(APPEND imgui_source     
    ${imgui_path}/backends/imgui_impl_opengl3.h 
    ${imgui_path}/backends/imgui_impl_opengl3.cpp 
    ${imgui_path}/backends/imgui_impl_wgpu.h 
    ${imgui_path}/backends/imgui_impl_wgpu.cpp)
else()

    if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        list(APPEND imgui_source 
            ${imgui_path}/backends/imgui_impl_opengl3.h 
            ${imgui_path}/backends/imgui_impl_opengl3.cpp      
            ${imgui_path}/backends/imgui_impl_metal.h 
            ${imgui_path}/backends/imgui_impl_metal.mm 
            ${imgui_path}/backends/imgui_impl_osx.h 
            ${imgui_path}/backends/imgui_impl_osx.mm)
    elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
        list(APPEND imgui_source     
            ${imgui_path}/backends/imgui_impl_opengl3.h 
            ${imgui_path}/backends/imgui_impl_opengl3.cpp 
            ${imgui_path}/backends/imgui_impl_dx11.h 
            ${imgui_path}/backends/imgui_impl_dx11.cpp 
            ${imgui_path}/backends/imgui_impl_dx12.h 
            ${imgui_path}/backends/imgui_impl_dx12.cpp 
            ${imgui_path}/backends/imgui_impl_win32.h  
            ${imgui_path}/backends/imgui_impl_win32.cpp)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
        list(APPEND imgui_source     
            ${imgui_path}/backends/imgui_impl_opengl3.h 
            ${imgui_path}/backends/imgui_impl_opengl3.cpp)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Android) 
        list(APPEND imgui_source     
            ${imgui_path}/backends/imgui_impl_opengl3.h 
            ${imgui_path}/backends/imgui_impl_opengl3.cpp 
            ${imgui_path}/backends/imgui_impl_android.h 
            ${imgui_path}/backends/imgui_impl_android.cpp)
    endif()
endif()

if(Vulkan_FOUND)
    list(APPEND imgui_source     
    ${imgui_path}/backends/imgui_impl_vulkan.h 
    ${imgui_path}/backends/imgui_impl_vulkan.cpp)
  
endif()


add_library(imgui STATIC ${imgui_source})

target_include_directories(imgui PRIVATE ${imgui_path})

if(Vulkan_FOUND)
    # Include Vulkan directories
    target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIR})

    # Link Vulkan libraries
    target_link_libraries(imgui PRIVATE ${Vulkan_LIBRARIES})
endif()
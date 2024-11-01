set(stb_path ${CMAKE_SOURCE_DIR}/libs/stb)
set(TARGET stb)
add_library(${TARGET} STATIC 
    ${stb_path}/stb_image.h stb_image.cpp)
target_include_directories(${TARGET} PRIVATE ${stb_path})


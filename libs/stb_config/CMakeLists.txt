set(stb_path ${CMAKE_SOURCE_DIR}/libs/stb)

add_library(stb STATIC 
    ${stb_path}/stb_image.h stb_image.cpp)
target_include_directories(stb PRIVATE ${stb_path})

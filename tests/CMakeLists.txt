set(TARGET tests)
set(TARGET_SOURCE main.cpp)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/Catch2/extras)
add_executable(${TARGET})

target_include_directories(${TARGET} PUBLIC 
    ${CMAKE_SOURCE_DIR}/sources/cpp 
    ${CMAKE_SOURCE_DIR}/libs/Catch2/src
)

set(common_lib Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(${TARGET} PUBLIC ${common_lib})

target_sources(${TARGET} PUBLIC ${TARGET_SOURCE})

include(CTest)
include(Catch)
catch_discover_tests(${TARGET})

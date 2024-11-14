set(TARGET tests)
set(TARGET_SOURCE main.cpp)

add_executable(${TARGET})

target_include_directories(${TARGET} PUBLIC 
    ${CMAKE_SOURCE_DIR}/sources 
)
target_sources(${TARGET} PUBLIC ${TARGET_SOURCE})
include(CTest)


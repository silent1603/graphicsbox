cmake_minimum_required(VERSION 3.22)
project(graphicbox C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CPP_RTTI_ENABLED OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CheckIPOSupported)
include(CPack)
include(CMakeFindDependencyMacro)
include(FetchContent)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

include(utils)
include(setup_ninja)
#include(setup_ispc)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(PACKAGE_INSTALL_PATH "${CMAKE_SOURCE_DIR}/package/" CACHE STRING "Assert path" )
set(INSTALL_TARGETS_LIST "" CACHE INTERNAL  "LIBS TARGET INSTAL NAME")

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin )
	set( CMAKE_${OUTPUTCONFIG}_POSTFIX ${OUTPUTCONFIG})
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if (MSVC)

	# Set runtime library
	if (USE_STATIC_MSVC_RUNTIME_LIBRARY)
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()

	# Set general compiler flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Gm- /MP /nologo /diagnostics:classic /FC /fp:except- /Zc:inline /ZI /INCREMENTAL")
	
	# Optionally generate debug symbols
	if (GENERATE_DEBUG_SYMBOLS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
	endif()

	if (NOT CPP_RTTI_ENABLED)
		# Set compiler flag for disabling RTTI
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
	else()
		# Set compiler flag for enabling RTTI
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR")
	endif()

	if (NOT CPP_EXCEPTIONS_ENABLED)
		# Remove any existing compiler flag that enables exceptions
		string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

		# Disable warning about STL and compiler-generated types using noexcept when exceptions are disabled
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4577")
	else()
		# Enable exceptions
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	endif()

	# Set compiler flags for various configurations
	if (OVERRIDE_CXX_FLAGS)
		set(CMAKE_CXX_FLAGS_DEBUG "/GS /Od /Ob0 /RTC1")
		set(CMAKE_CXX_FLAGS_RELEASE "/GS- /Gy /O2 /Oi /Ot")
	endif()

	# Set linker flags
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		if (CROSS_PLATFORM_DETERMINISTIC)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise")
		else()
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast") # Clang doesn't use fast math because it cannot be turned off inside a single compilation unit
		endif()
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /showFilenames")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments") # Clang emits warnings about unused arguments such as /MP and /GL
	endif()
else()
	# Enable warnings
	# if (ENABLE_ALL_WARNINGS)
	# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
	# endif()

	# Optionally generate debug symbols
	if (GENERATE_DEBUG_SYMBOLS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif()

	if (NOT CPP_RTTI_ENABLED)
		# Set compiler flag for disabling RTTI
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
	else()
		# Set compiler flag for enabling RTTI
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
	endif()

	if (NOT CPP_EXCEPTIONS_ENABLED)
		# Set compiler flag for disabling exception-handling
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
	else()
		# Set compiler flag for enabling exception-handling
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
	endif()

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		# Also disable -Wstringop-overflow or it will generate false positives that can't be disabled from code when link-time optimizations are enabled
		# Also turn off automatic fused multiply add contractions, there doesn't seem to be a way to do this selectively through the macro JPH_PRECISE_MATH_OFF
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-overflow -ffp-contract=off")
	else()
		# Do not use -ffast-math since it cannot be turned off in a single compilation unit under clang, see Core.h
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffp-model=precise")

		# On clang 14 and later we can turn off float contraction through a pragma, older versions and deterministic versions need it off always, see Core.h
		if (CMAKE_CXX_COMPILER_VERSION LESS 14 OR CROSS_PLATFORM_DETERMINISTIC)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffp-contract=off")
		endif()
	endif()

	# See https://github.com/jrouwe/JoltPhysics/issues/922. When compiling with DOUBLE_PRECISION=YES and CMAKE_OSX_DEPLOYMENT_TARGET=10.12 clang triggers a warning that we silence here.
	if ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -faligned-allocation")
	endif()

	# Set compiler flags for various configurations
	if (OVERRIDE_CXX_FLAGS)
		set(CMAKE_CXX_FLAGS_DEBUG "")
		set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	endif()
	set(CMAKE_CXX_FLAGS_DISTRIBUTION "${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# Set linker flags
set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTION "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTION "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
# Enable link time optimization in Release and Distribution mode if requested and available
#set_property(TARGET ... PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

if (EMSCRIPTEN)
    option(TINYUI_WASM_SINGLE_FILE "Embed WASM inside the generated .js" ON)
endif()

option(TINYUI_SANITIZE_THREAD                         "Enable thread sanitizer" OFF)
option(TINYUI_SANITIZE_ADDRESS                        "Enable address sanitizer" OFF)
option(TINYUI_SANITIZE_UNDEFINED      				  "Enable undefined sanitizer" OFF)
option(TINYUI_ENABLE_TESTS      	  				  "Enable tests" ON)
option(TINYUI_USE_ISPC      	 					  "USE ISPC" OFF)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
if (TINYUI_SANITIZE_THREAD)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

if (TINYUI_SANITIZE_ADDRESS)
	if(MSVC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
	endif()
endif()

if (TINYUI_SANITIZE_UNDEFINED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

find_package(Vulkan)

add_subdirectory(sources)

if(TINYUI_ENABLE_TESTS)
enable_testing()
add_subdirectory(tests)
endif()
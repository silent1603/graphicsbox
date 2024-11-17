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
option(TINYUI_EXPORT_DYNAMIC_LIBRARY_MODULE      	  "Export dynamic module" ON)


if(CMAKE_GENERATOR STREQUAL "Ninja")
	set(arch "")
	set(os "")
	if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
		set(os "mac")
	elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
		set(os "win")
		if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
			set(arch "aarch64")
		endif()
	elseif (CMAKE_SYSTEM_NAME STREQUAL Linux) 
		set(os "linux")
		if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
			set(arch "arm64")
		endif()
	endif()
	
	if(EXISTS ${CMAKE_SOURCE_DIR}/tools/ninja)
		download_file(https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-${os}${arch}.zip ${CMAKE_SOURCE_DIR}/tools/ninja e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855)
	endif()
endif()

set(ISPC_PATH "${CMAKE_SOURCE_DIR}/tools/ispc/bin")
IF(NOT EXISTS ${ISPC_PATH})
	set(ISPC_VERSION "v1.25.3")
	set(ISPC_DOWNLOAD_URL "")
	set(ISPC_OS "")
	set(ISPC_DOWNLOAD_TYPE "")
	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		set(ISPC_DOWNLOAD_TYPE "tar.gz")
		set(ISPC_OS "macOS.universal")
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(ISPC_DOWNLOAD_TYPE "zip")
		set(ISPC_OS "windows")
	elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux") 
		set(ISPC_DOWNLOAD_TYPE "tar.gz")
		set(ISPC_OS "linux")
	endif()
	
	set(ISPC_DOWNLOAD_URL "https://github.com/ispc/ispc/releases/download/${ISPC_VERSION}/ispc-${ISPC_VERSION}-${ISPC_OS}.${ISPC_DOWNLOAD_TYPE}")
	message(STATUS "Download ispc from ${ISPC_DOWNLOAD_URL}")
	set(ISPC_DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tools")
	if(${ISPC_DOWNLOAD_URL} MATCHES "\\.tar\\.gz$")
		set(ISPC_FILE "ispc.tar.gz" )
		file(DOWNLOAD ${ISPC_DOWNLOAD_URL} ${ISPC_DOWNLOAD_DIR}/${ISPC_FILE} SHOW_PROGRESS)
   	 	execute_process(
        	COMMAND ${CMAKE_COMMAND} -E tar xzf ${ISPC_DOWNLOAD_DIR}/${ISPC_FILE} --strip-components=1
        	WORKING_DIRECTORY ${ISPC_DOWNLOAD_DIR})

	elseif(${ISPC_DOWNLOAD_URL} MATCHES "\\.zip$")
		set(ISPC_FILE "ispc.zip" )
		file(DOWNLOAD ${ISPC_DOWNLOAD_URL} ${ISPC_DOWNLOAD_DIR}/${ISPC_FILE} SHOW_PROGRESS)
    	execute_process(
        	COMMAND ${CMAKE_COMMAND} -E tar xzf ${ISPC_DOWNLOAD_DIR}/${ISPC_FILE}
        	WORKING_DIRECTORY ${ISPC_DOWNLOAD_DIR})
	else()
    	message(FATAL_ERROR "Unsupported archive format: ${ISPC_DOWNLOAD_URL}")
	endif()
	file(REMOVE ${ISPC_DOWNLOAD_DIR}/${ISPC_FILE})
	file(RENAME "${ISPC_DOWNLOAD_DIR}/ispc-${ISPC_VERSION}-${ISPC_OS}" ${ISPC_DOWNLOAD_DIR}/ispc)
endif()
FIND_PROGRAM(ISPC_EXECUTABLE ispc PATHS ${ISPC_PATH} DOC "Path to the ISPC executable.")

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
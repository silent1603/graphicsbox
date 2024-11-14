include(FetchContent)

function(add_IPO TARGET)
  # Check if the current CMake version supports IPO (introduced in CMake 3.9+)
  include(CheckIPOSupported)

  # Check IPO support for the current target
  check_ipo_supported(RESULT)

  # If supported, set the INTERPROCEDURAL_OPTIMIZATION property to TRUE for the target
  if(RESULT)
    set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endfunction(add_IPO)


function(download_file url path hash)

FetchContent_Declare(download_${hash}
URL ${url}
URL_HASH SHA256=${hash}
DOWNLOAD_NO_EXTRACT true 
SOURCE_DIR   ${path}
)

if(NOT download_${hash}_POPULATED)
  FetchContent_Populate(download_${hash})
endif()

endfunction(download_file)


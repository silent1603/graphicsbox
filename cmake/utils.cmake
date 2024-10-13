

function(add_IPO TARGET)
  # Check if the current CMake version supports IPO (introduced in CMake 3.9+)
  include(CheckIPOSupported)

  # Check IPO support for the current target
  check_ipo_supported(RESULT)

  # If supported, set the INTERPROCEDURAL_OPTIMIZATION property to TRUE for the target
  if(RESULT)
    set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endfunction()
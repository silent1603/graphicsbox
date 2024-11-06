
#=== LIBRARY: imgui + Dear ImGui
add_subdirectory(imgui_config)
add_IPO(imgui)
list(APPEND INSTALL_TARGETS_LIST imgui)
#=== LIBRARY: stb
add_subdirectory(stb_config)
add_IPO(stb)
list(APPEND INSTALL_TARGETS_LIST stb)
#=== LIBRARY: glm
add_subdirectory(cglm)
add_IPO(cglm)
list(APPEND INSTALL_TARGETS_LIST cglm)
#=== LIBRARY: Catch2
add_subdirectory(Catch2)
add_IPO(Catch2)


#=== LIBRARY: imgui + Dear ImGui
add_subdirectory(imgui_config)
add_IPO(imgui)

#=== LIBRARY: stb
add_subdirectory(stb_config)
add_IPO(stb)

#=== LIBRARY: glm
add_subdirectory(cglm)
add_IPO(cglm)


#=== LIBRARY: Catch2
add_subdirectory(Catch2)
add_IPO(Catch2)


set(INSTALL_TARGETS_LIST "imgui;stb;cglm" CACHE INTERNAL  "LIBS TARGET INSTAL NAME")
set(LIBS_LIST "imgui;stb;cglm" CACHE INTERNAL  "LIBS TARGET NAME")
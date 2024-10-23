@echo off

if exist ./Builds/win_wasm (
   echo has builds folder
) else (
  mkdir Builds\win_wasm
)

emmake cmake -DCMAKE_TOOLCHAIN_FILE=%cd%/tools/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake  -G "Ninja" -B ./Builds/win_wasm
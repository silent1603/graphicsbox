@echo off

if exist ./prj/win_wasm (
   echo has prj folder
) else (
  mkdir prj\win_wasm
)

emmake cmake -DCMAKE_TOOLCHAIN_FILE=%cd%/tools/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake  -G "Ninja" -B ./prj/win_wasm
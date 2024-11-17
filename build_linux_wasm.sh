#!/bin/bash

if [ -d ./prj/linux_wasm ]; then
  echo "Directory exists."
else
  sudo mkdir -p ./prj/linux_wasm
fi
emmake cmake -DCMAKE_TOOLCHAIN_FILE=$PWD/tools/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -G Ninja -B ./prj/linux_wasm


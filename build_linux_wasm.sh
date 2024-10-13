#!/bin/bash

if [ -d ./Builds/linux_wasm ]; then
  echo "Directory exists."
else
  sudo mkdir -p ./Builds/linux_wasm
fi
emmake cmake -DCMAKE_TOOLCHAIN_FILE=$PWD/tools/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -G Ninja -B ./Builds/linux_wasm


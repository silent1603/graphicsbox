#!/bin/sh
if [ -d /Builds/linux ]; then
  echo "Directory exists."
else
  sudo  mkdir -p /Builds/linux
fi
cmake -G Ninja -B ./Builds/linux
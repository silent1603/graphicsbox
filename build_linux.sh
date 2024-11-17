#!/bin/sh
if [ -d /prj/linux ]; then
  echo "Directory exists."
else
  sudo  mkdir -p /prj/linux
fi
cmake -G Ninja -B ./prj/linux
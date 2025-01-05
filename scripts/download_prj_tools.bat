@echo off

pushd "%~dp0\..\tools"

if not exist ispc/bin/ispc.exe (
curl https://github.com/ispc/ispc/releases/download/v1.25.3/ispc-v1.25.3-windows.zip -O -J -L
tar -xf ispc-v1.25.3-windows.zip 
del  ispc-v1.25.3-windows.zip 
ren "ispc-v1.25.3-windows" "ispc"
)
popd
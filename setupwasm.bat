@echo off 


REM Download and install the latest SDK tools.
call ./tools/emsdk/emsdk.bat install latest

REM Make the "latest" SDK "active" for the current user. (writes .emscripten file)
call ./tools/emsdk/emsdk.bat activate latest

REM Activate PATH and other environment variables in the current terminal
call ./tools/emsdk/emsdk_env.bat
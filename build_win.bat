@echo off

if exist ./Builds/window (
   echo has builds folder
) else (
  mkdir Builds\window
)

cmake -G "Visual Studio 17 2022" -B ./Builds/window  
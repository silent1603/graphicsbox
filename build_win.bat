@echo off

if exist ./prj/window (
   echo has prj folder
) else (
  mkdir prj\window
)

cmake -G "Visual Studio 17 2022" -B ./prj/window  
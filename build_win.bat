@echo off

setlocal enabledelayedexpansion

set default_config=%~dp0\config\msvc\build_debug.txt
set default_link=%~dp0\config\msvc\link_debug.txt

set config_file=%default_config%
set link_file=%default_link%

set config_content=
set link_content=

for /f "delims=" %%A in (%config_file%) do (
    set "config_content=!config_content! %%A"
)
set "config_content=%config_content:~1%"

for /f "delims=" %%A in (%link_file%) do (
    set "link_content=!link_content! %%A"
)
set "link_content=%link_content:~1%"

set source_files=

FOR %%G IN (%cd%\sources\*.cpp) DO set "source_files=!source_files! %%G"
FOR %%G IN (%cd%\sources\Entry\Windows\*.cpp) DO set "source_files=!source_files! %%G"
FOR %%G IN (%cd%\sources\Graphics\Renderer\*.cpp) DO set "source_files=!source_files! %%G"
FOR %%G IN (%cd%\sources\Graphics\Renderer\DirectX\*.cpp) DO set "source_files=!source_files! %%G"

echo  %config_content% %source_files% %link_content%
cl %config_content% /I .\sources %source_files% %link_content%
@echo off
pushd %~dp0/..

if not exist common_compile_files.txt (
    type nul > common_compile_files.txt
)
break>common_compile_files.txt
FOR %%G IN (%cd%\sources\*.cpp) DO (
    echo "output %%G"
    echo %%G>>common_compile_files.txt
)
FOR %%G IN (%cd%\sources\Core\*.cpp) DO echo %%G>>common_compile_files.txt

popd
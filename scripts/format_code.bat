@echo off

pushd %~dp0\..

for /r %%t in (*.cpp *.h) do clang-format -i -style=file "%%t"
popd
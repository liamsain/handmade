@echo off

pushd .\build
cl -Zi ..\src\win32_handmade.cpp
popd

cls
@g++ -g3 *.cpp -std=c++11
@echo off
if errorlevel 1 (
    goto end
) 
@a.exe
@echo OFF
if "%1" == "h" (
    hexdump -C file.bmp
)
file.bmp
:end


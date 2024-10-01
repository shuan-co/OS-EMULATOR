@echo off
g++ -std=c++11 os_em_week2.cpp -o a.exe
if %errorlevel% neq 0 (
    echo Compilation failed.
    pause
    exit /b %errorlevel%
)
a.exe
pause

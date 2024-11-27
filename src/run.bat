@echo off

:: Delete all files in the ./logs and ./swap directories
del /q /f .\logs\* > nul
del /q /f .\swap\* > nul

:: Optionally remove the directories themselves if they are empty (if you want them cleaned out completely)
rd /s /q .\logs
rd /s /q .\swap

:: Recreate the directories if they were removed
mkdir .\logs
mkdir .\swap

:: Compile the program
g++ -std=c++11 os_em_week2.cpp -o a.exe
if %errorlevel% neq 0 (
    echo Compilation failed.
    pause
    exit /b %errorlevel%
)

:: Run the executable
a.exe

:: Pause the script to see the result
pause

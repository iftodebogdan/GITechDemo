@echo off
@setlocal

:: Set up some variables
set start=%time%
set py_exec="..\Tools\Python27\python.exe"
set then="&& cd %~dp0 && echo. && ^"

:: Change the working directory to where the script lies
pushd %~dp0

:: Execute the build processes
%py_exec% build_project_win.py release win32 %then%
%py_exec% build_project_win.py profile win32 %then%
%py_exec% build_project_win.py release x64 %then%
%py_exec% build_project_win.py profile x64

:: Save the exit code of the build process that failed (or 0, if all succeeded)
set exit_code=%errorlevel%

:: Calculate the total amount of time it took to execute all the build processes
set end=%time%
set options="tokens=1-4 delims=:.,"
for /f %options% %%a in ("%start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%ms% lss 100 set ms=0%ms%

rem set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%

echo.

:: Print success or error message
if %exit_code% equ 0 (
    echo SUCCESS: Successfully built all project configurations.
) else (
    echo ERROR: An error has occurred while building a project configuration. Aborting...
)

:: Print operation time
echo Operation took %mins% minutes and %secs%.%ms% seconds

:: Determine whether the batch file is running from command line, or was double clicked in Windows Explorer.
:: Only pause if double clicked, so as not to break any automated process that may execute this batch file.
for %%x in (%cmdcmdline%) do if /i "%%~x"=="/c" set doubleclicked=1
if defined doubleclicked pause

:: Restore the original working directory so as not to break any automated process that may execute this batch file
popd

:: Propagate the exit code from the failing build process
exit /b %exit_code%

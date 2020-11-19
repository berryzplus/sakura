setlocal
set BUILD_DIR=%~dp1
set SOURCE_DIR=%~dp0compiletests

:: find generic tools
if not defined CMD_VSWHERE call %~dp0..\tools\find-tools.bat

if not exist "%CMD_NINJA%" (
  echo "no cmake found."
  exit /b 1
)

mkdir %BUILD_DIR% > NUL 2>&1
pushd %BUILD_DIR%

call :run_cmake_configure || endlocal && exit /b 1

endlocal && exit /b 0

:run_cmake_configure
call :find_cl_compiler

:: replace back-slash to slash in the path.
set CL_COMPILER=%CMD_CL:\=/%

:: run cmake configuration.
"%CMD_CMAKE%" -G Ninja ^
  -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
  "-DCMAKE_MAKE_PROGRAM=%CMD_NINJA%" ^
  "-DCMAKE_C_COMPILER=%CL_COMPILER%" ^
  "-DCMAKE_CXX_COMPILER=%CL_COMPILER%" ^
  %SOURCE_DIR% ^
  || endlocal && exit /b 1
goto :EOF

:find_cl_compiler
for /f "usebackq delims=" %%a in (`where cl.exe`) do (
  set "CMD_CL=%%a"
  goto :EOF
)
goto :EOF

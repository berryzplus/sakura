@echo off
set platform=%1
set configuration=%2
set argument=%3
set BUILDDIR=build\%platform%
set PROJECT_TOP=%~dp0

if "%platform%" == "" (
	call :showhelp %0
	exit /b 1
)

if "%configuration%" == "" (
	call :showhelp %0
	exit /b 1
)

cd /d %~dp0

if "%argument%" == "clean" (
	for /l %%n in (1,1,10) do (
		if exist "%BUILDDIR%" (
			@echo ---- removing %BUILDDIR% -----
			rmdir /s /q "%BUILDDIR%"
		)
	)
)

if not exist "%BUILDDIR%" (
	@echo ---- create %BUILDDIR% -----
	mkdir "%BUILDDIR%" || exit /b 1
)

setlocal
set CMAKE_OPTIONS=-DCMAKE_BUILD_TYPE="%configuration%"

if "%platform%" == "MinGW" (
	call :setEnvOfMinGW
) else if "%platform%" == "x64" (
	call :setEnvOfMSVC64
) else (
	call :setEnvOfMSVC
)

echo ---- creating project -----
echo cmake -B%BUILDDIR% %CMAKE_OPTIONS% -H%PROJECT_TOP%
cmake -B%BUILDDIR% %CMAKE_OPTIONS% -H%PROJECT_TOP% || set ERROR_RESULT=1
if errorlevel 1 (
	@echo ERROR %ERRORLEVEL%
	endlocal
	exit /b 1
)

@echo ---- building project -----
@echo cmake --build %BUILDDIR%  --config %configuration%
cmake --build %BUILDDIR%  --config %configuration% || set ERROR_RESULT=1
if errorlevel 1 (
	@echo ERROR %ERRORLEVEL%
	endlocal
	exit /b 1
)

@echo ---- build succeeded -----
endlocal
exit /b 0

@rem ------------------------------------------------------------------------------
@rem show help
@rem ------------------------------------------------------------------------------
:showhelp
@echo off
@echo usage %1 platform configuration
exit /b

@rem ----------------------------------------------
@rem  sub-routines
@rem ----------------------------------------------

:setEnvOfMSVC
	set CMAKE_OPTIONS=--trace-expand -G "Visual Studio 15 2017" %CMAKE_OPTIONS%
exit /b 0

:setEnvOfMSVC64
	set CMAKE_OPTIONS=--trace-expand -G "Visual Studio 15 2017 win64" %CMAKE_OPTIONS%
exit /b 0

:setEnvOfMinGW
    C:\msys64\usr\bin\bash --login -c "pacman -S --noconfirm mingw-w64-x86_64-gtest"
	set MINGW_ROOT=C:/msys64/mingw64
	set PATH=%MINGW_ROOT%\bin;%PATH:C:\Program Files\Git\usr\bin;=%
	set CMAKE_C_COMPILER=%MINGW_ROOT%/bin/gcc.exe
	set CMAKE_CXX_COMPILER=%MINGW_ROOT%/bin/g++.exe
	set CMAKE_PREFIX_PATH=%MINGW_ROOT%
	set CMAKE_OPTIONS=-G "MinGW Makefiles" %CMAKE_OPTIONS% -DCMAKE_CXX_COMPILER="%CMAKE_CXX_COMPILER%" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%"
exit /b 0

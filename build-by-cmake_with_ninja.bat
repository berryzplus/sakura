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
set CMAKE_OPTIONS=-DCMAKE_BUILD_TYPE="%configuration%" -DCMAKE_MAKE_PROGRAM="C:\PROGRAM FILES (X86)\MICROSOFT VISUAL STUDIO\2017\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe"

if "%platform%" == "MinGW" (
	call :setEnvOfMinGW
)

echo ---- creating project -----
echo cmake -G Ninja -B%BUILDDIR% -H%PROJECT_TOP%
cmake -G "Ninja" -B%BUILDDIR% %CMAKE_OPTIONS% -H%PROJECT_TOP% || set ERROR_RESULT=1
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

:setEnvOfMinGW
	set MSYSTEM=MINGW64
	set MSYS2_ROOT=C:/msys64
	set MINGW_ROOT=%MSYS2_ROOT%/mingw64
	set PATH=%MINGW_ROOT%\bin;%MSYS2_ROOT%\usr\local\bin;%MSYS2_ROOT%\usr\bin;%MSYS2_ROOT%\bin;%PATH:C:\Program Files\Git\usr\bin;=%
	set CMAKE_C_COMPILER=%MINGW_ROOT%/bin/gcc.exe
	set CMAKE_CXX_COMPILER=%MINGW_ROOT%/bin/g++.exe
	set CMAKE_PREFIX_PATH=%MINGW_ROOT%
	set CMAKE_OPTIONS=%CMAKE_OPTIONS% -DCMAKE_CXX_COMPILER="%CMAKE_CXX_COMPILER%" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%"
exit /b 0

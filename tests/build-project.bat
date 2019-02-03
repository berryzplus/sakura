set platform=%1
set configuration=%2
set ERROR_RESULT=0

@rem check platform and exit when MSVC.
if "%platform%" == "MinGW" (
	@rem continue processing.
) else (
	@rem build-sln.bat will make test projects.
	exit /b 0
)

pushd %~dp0
if not exist googletest (
    git submodule init
    git submodule update
)

set BUILDDIR=build\%platform%
cmake --build %BUILDDIR%  --config %configuration% || set ERROR_RESULT=1

popd

if "%ERROR_RESULT%" == "1" (
	@echo ERROR
	exit /b 1
)

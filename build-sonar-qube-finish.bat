call %~dp0build-sonar-qube-env.bat
if "%SonarScanner_MSBUILD%" == "" (
	echo do nothing
	exit /b 0
)
if "%SONAR_QUBE_TOKEN%" == "" (
	echo do nothing
	exit /b 0
)

@rem run tests1.exe with coverage.
"C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe" ^
  --export_type xml:tests1-coverage-%platform%-%configuration%.xml ^
  --cover_children ^
  --modules %~dp0%platform%\%configuration%\tests1.exe ^
  --sources %~dp0 ^
  --excluded_sources %~dp0tests ^
  --working_dir %~dp0%platform%\%configuration% ^
  -- ^
  %~dp0%platform%\%configuration%\tests1.exe ^
    --gtest_output=xml:tests1-googletest-%platform%-%configuration%.xml

@rem run tests1.exe with coverage.
"C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe" ^
  --export_type xml:grep-coverage-%platform%-%configuration%.xml ^
  --cover_children ^
  --modules %~dp0%platform%\%configuration%\tests1.exe ^
  --sources %~dp0 ^
  --excluded_sources %~dp0tests ^
  --working_dir %~dp0%platform%\%configuration% ^
  -- ^
  %~dp0%platform%\%configuration%\tests1.exe ^
    -PROF="" ^
    -GREPMODE ^
    -GKEY="\);" ^
    -GFILE="*.h;*.cpp;#.git;#.svn;#.vs;#googletest;#build;#Release" ^
    -GFOLDER="%~dp0" ^
    -GCODE=99 ^
    -GOPT=1RSPU

@rem to ensure hide variable SONAR_QUBE_TOKEN
@echo off
"%SonarScanner_MSBUILD%" end /d:sonar.login="%SONAR_QUBE_TOKEN%"
if errorlevel 1 (
	echo ERROR in %SonarScanner_MSBUILD% end errorlevel %errorlevel%
	exit /b 1
)
exit /b 0

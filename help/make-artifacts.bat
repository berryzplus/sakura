@echo off

if not defined CMD_7Z call %~dp0..\tools\find-tools.bat
if not defined CMD_7Z (
	echo 7z.exe was not found.
	exit /b 1
)


@rem for GIT_TAG_NAME
call %~dp0..\sakura\githash.bat %~dp0..\sakura_core

@rem ----------------------------------------------------------------
@rem prepare environment variable
@rem ----------------------------------------------------------------
@echo checking CI_SOURCE_REPO %CI_SOURCE_REPO%
set BUILD_ACCOUNT=
if "%CI_SOURCE_REPO%" == "sakura-editor/sakura" (
	set BUILD_ACCOUNT=
) else if "%CI_SOURCE_REPO%" == "" (
	set BUILD_ACCOUNT=
) else (
	set BUILD_ACCOUNT=%CI_ACCOUNT_NAME%
)

@echo checking APPVEYOR_BUILD_NUMBER %APPVEYOR_BUILD_NUMBER%
if not "%APPVEYOR_BUILD_NUMBER%" == "" (
	set BUILD_NUMBER=build%APPVEYOR_BUILD_NUMBER%
) else (
	set BUILD_NUMBER=buildLocal
)

@echo checking GIT_TAG_NAME %GIT_TAG_NAME%
if not "%GIT_TAG_NAME%" == "" (
	@rem replace '/' with '_'
	set TEMP_NAME1=!GIT_TAG_NAME:/=_!
	@echo TEMP_NAME1 = !TEMP_NAME1!
	
	@rem replace ' ' with '_'
	set TEMP_NAME2=!TEMP_NAME1: =_!
	@echo TEMP_NAME2 = !TEMP_NAME2!

	@rem replace ' ' with '_'
	set TAG_NAME=tag-!TEMP_NAME2!
	@echo TAG_NAME = !TEMP_NAME2!
)

@echo checking PR_NUMBER %PR_NUMBER%
if not "%PR_NUMBER%" == "" (
	set PR_NAME=PR%PR_NUMBER%
)

@echo hash name
set SHORTHASH=%GIT_ABBR_HASH%

@rem ----------------------------------------------------------------
@rem build BASENAME
@rem ----------------------------------------------------------------
set BASENAME=sakura

@echo adding BUILD_ACCOUNT
if not "%BUILD_ACCOUNT%" == "" (
	set BASENAME=%BASENAME%-%BUILD_ACCOUNT%
)
@echo BASENAME = %BASENAME%

@echo adding TAG_NAME
if not "%TAG_NAME%" == "" (
	set BASENAME=%BASENAME%-%TAG_NAME%
)
@echo BASENAME = %BASENAME%

@echo adding PR_NAME
if not "%PR_NAME%" == "" (
	set BASENAME=%BASENAME%-%PR_NAME%
)
@echo BASENAME = %BASENAME%

@echo adding BUILD_NUMBER
if not "%BUILD_NUMBER%" == "" (
	set BASENAME=%BASENAME%-%BUILD_NUMBER%
)
@echo BASENAME = %BASENAME%

@echo adding SHORTHASH
if not "%SHORTHASH%" == "" (
	set BASENAME=%BASENAME%-%SHORTHASH%
)
@echo BASENAME = %BASENAME%

@rem ---------------------- BASENAME ---------------------------------
@rem "sakura"
@rem BUILD_ACCOUNT: (option) APPVEYOR_ACCOUNT_NAME
@rem TAG_NAME     : (option) tag Name
@rem PR_NAME      : (option) PRxxx (xxx is a PR number)
@rem BUILD_NUMBER : (option) buildYYY or "buildLocal" (YYY is build number)
@rem SHORTHASH    : (option) hash or "buildLocal" (hash is leading 8 charactors)
@rem ----------------------------------------------------------------

set CHM_ARCHIVE=%~dp0..\%BASENAME%-Chm.zip

pushd "%~dp0"
"%CMD_7Z%" a %CHM_ARCHIVE%		^
	macro\macro.chm		^
	macro\Compile.Log	^
	plugin\plugin.chm	^
	plugin\Compile.Log	^
	sakura\sakura.chm	^
	sakura\sakura.hh	^
	sakura\Compile.Log  ^
	|| exit /b 1

@echo start generate MD5 hash
set CMD_FIND=%SystemRoot%\System32\find.exe
certutil -hashfile %CHM_ARCHIVE% MD5 ^
	| %CMD_FIND% /v "MD5"		^
	| %CMD_FIND% /v "CertUtil"	^
	> %CHM_ARCHIVE%.md5			^
	|| exit /b 1
@echo end generate MD5 hash

popd

exit /b 0

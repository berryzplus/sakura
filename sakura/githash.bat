@echo off

set OUT_DIR=%~1
if "%OUT_DIR%" == "" (
	set OUT_DIR=.
)

@rem replace '/' with '\'
set OUT_DIR=%OUT_DIR:/=\%

@echo.
@echo ---- Make githash.h ----
call :set_git_variables
call :set_ci_build_url
call :update_output_githash
exit /b 0

:set_git_variables
	@rem ensure to be in the proper directory
	pushd "%~dp0"

	: Git enabled checking
	set GIT_ENABLED=1
	if not defined CMD_GIT call "%~dp0..\tools\find-tools.bat"
	if not defined CMD_GIT (
		set GIT_ENABLED=0
		@echo NOTE: No git command
	)
	if not exist ..\.git (
		set GIT_ENABLED=0
		@echo NOTE: No .git directory
	)

	@rem clear variable in advance
	set GIT_ABBR_HASH=
	set GIT_LONG_HASH=
	set GIT_ORIGIN_URL=
	set GIT_TAG_NAME=

	: Get git hash if git is enabled
	if "%GIT_ENABLED%" == "1" (
		for /f "usebackq" %%s in (`"%CMD_GIT%" show -s --format^=%%h`) do (
			set GIT_ABBR_HASH=%%s
		)
		for /f "usebackq" %%s in (`"%CMD_GIT%" show -s --format^=%%H`) do (
			set GIT_LONG_HASH=%%s
		)
		for /f "usebackq" %%s in (`"%CMD_GIT%" config --get remote.origin.url`) do (
			set GIT_ORIGIN_URL=%%s
		)
		
		@rem get tag of 'HEAD'
		@rem Ignore errors when `HEAD` is not tagged.
		@rem https://superuser.com/questions/743735/suppressing-errors-from-an-embedded-command-in-a-batch-file-for-loop
		for /f "usebackq" %%s in (`"%CMD_GIT%" describe --tags --contains 2^>nul`) do (
			set GIT_TAG_NAME=%%s
		)
	) else (
		set GIT_ABBR_HASH=
		set GIT_LONG_HASH=
		set GIT_ORIGIN_URL=
		set GIT_TAG_NAME=
	)

	@rem get back to the original directory
	popd

	exit /b 0

:set_ci_build_url
	if "%APPVEYOR%"       == "True" call :set_ci_build_url_for_appveyor
	if "%TF_BUILD%"       == "True" call :set_ci_build_url_for_azurepipelines
	if "%GITHUB_ACTIONS%" == "true" call :set_ci_build_url_for_githubactions

	set "REPOSITORY_ROOT=https://github.com/%CI_SOURCE_REPO%/"
	if defined GITHUB_ON (
		set "CI_SOURCE_URL=%REPOSITORY_ROOT%commit/%GIT_LONG_HASH%"
		if defined PR_NUMBER (
			set "PR_SOURCE_URL=%REPOSITORY_ROOT%pull/%PR_NUMBER%/commits/%GIT_LONG_HASH%"
		)
	)
	exit /b 0

:set_ci_build_url_for_appveyor
	@rem APPVEYOR_BUILD_VERSION=Build1624
	set CI_BUILD_VERSION=Build%APPVEYOR_BUILD_NUMBER%

	set CI_BUILD_URL=%APPVEYOR_URL%/project/%APPVEYOR_ACCOUNT_NAME%/%APPVEYOR_PROJECT_SLUG%/build/%APPVEYOR_BUILD_VERSION%

	set CI_SOURCE_REPO=%APPVEYOR_REPO_NAME%

	set PR_NUMBER=%APPVEYOR_PULL_REQUEST_NUMBER%

	if "%APPVEYOR_REPO_PROVIDER%"=="gitHub" (
		set GITHUB_ON=1
	)
	exit /b 0

:set_ci_build_url_for_azurepipelines
	@rem example BUILD_BUILDNUMBER=AzpBuild20200205.4
	set CI_BUILD_VERSION=AzpBuild%BUILD_BUILDNUMBER%

	set CI_BUILD_URL=%SYSTEM_TEAMFOUNDATIONSERVERURI%%SYSTEM_TEAMPROJECT%/_build/results?buildId=%BUILD_BUILDID%

	set CI_SOURCE_REPO=%BUILD_REPOSITORY_NAME%

	set PR_NUMBER=%SYSTEM_PULLREQUEST_PULLREQUESTNUMBER%

	if "%BUILD_REPOSITORY_PROVIDER%"=="GitHub" (
		set GITHUB_ON=1
	)
	exit /b 0

:set_ci_build_url_for_githubactions
	@rem GitHub Actions build URL
	set CI_BUILD_URL=%GITHUB_SERVER_URL%/%GITHUB_REPOSITORY%/runs/%GITHUB_RUN_ID%

	@rem example build sakura #123
	set CI_BUILD_VERSION=%GITHUB_WORKFLOW% #%GITHUB_RUN_NUMBER%

	set GITHUB_ON=1
	exit /b 0

:update_output_githash
	@rem update githash.h if necessary
	set GITHASH_H=%OUT_DIR%\githash.h
	set GITHASH_H_TMP=%GITHASH_H%.tmp

	@rem set SKIP_CREATE_GITHASH=1 to disable creation of githash.h
	@rem check if skip creation of %GITHASH_H%
	set VALID_CREATE_GITHASH=1
	if "%SKIP_CREATE_GITHASH%" == "1" (
		set VALID_CREATE_GITHASH=0
	)
	if not exist "%GITHASH_H%" (
		set VALID_CREATE_GITHASH=1
	)

	if "%VALID_CREATE_GITHASH%" == "0" (
		@echo skip creation of %GITHASH_H%
		exit /b 0
	)

	call :output_githash > "%GITHASH_H_TMP%"

	fc "%GITHASH_H%" "%GITHASH_H_TMP%" 1>nul 2>&1
	if not errorlevel 1 (
		del "%GITHASH_H_TMP%"
		@echo %GITHASH_H% was not updated.
	) else (
		@echo GIT_ORIGIN_URL   : %GIT_ORIGIN_URL%
		@echo GIT_LONG_HASH    : %GIT_LONG_HASH%
		@echo GIT_ABBR_HASH    : %GIT_ABBR_HASH%
		@echo GIT_TAG_NAME     : %GIT_TAG_NAME%
		@echo.
		@echo CI_BUILD_VERSION : %CI_BUILD_VERSION%
		@echo CI_BUILD_URL     : %CI_BUILD_URL%
		@echo CI_SOURCE_URL    : %CI_SOURCE_URL%
		@echo.
		@echo PR_SOURCE_URL    : %PR_SOURCE_URL%

		if exist "%GITHASH_H%" del "%GITHASH_H%"
		move /y "%GITHASH_H_TMP%" "%GITHASH_H%"
		@echo %GITHASH_H% was updated.
	)

	exit /b 0

:output_githash
	echo /*! @file */
	echo #pragma once

	if defined GIT_ORIGIN_URL (
		echo #define GIT_ORIGIN_URL   "%GIT_ORIGIN_URL%"
	) else (
		echo // GIT_ORIGIN_URL   is not defined
	)
	if defined GIT_LONG_HASH (
		echo #define GIT_LONG_HASH    "%GIT_LONG_HASH%"
	) else (
		echo // GIT_LONG_HASH    is not defined
	)
	if defined GIT_ABBR_HASH (
		echo #define GIT_ABBR_HASH    "%GIT_ABBR_HASH%"
	) else (
		echo // GIT_ABBR_HASH    is not defined
	)

	if defined GIT_TAG_NAME (
		echo #define GIT_TAG_NAME     "%GIT_TAG_NAME%"
	) else (
		echo // GIT_TAG_NAME     is not defined
	)

	@rem enable 'dev version' macro which will be disabled on release branches
	echo #define DEV_VERSION

	if defined CI_BUILD_VERSION (
		echo #define CI_BUILD_VERSION "%CI_BUILD_VERSION%"
	) else (
		echo // CI_BUILD_VERSION is not defined
	)

	if defined CI_BUILD_URL (
		echo #define CI_BUILD_URL     "%CI_BUILD_URL%"
	) else (
		echo // CI_BUILD_URL is not defined
	)

	if defined CI_SOURCE_URL (
		echo #define CI_SOURCE_URL    "%CI_SOURCE_URL%"
	) else (
		echo // CI_SOURCE_URL    is not defined
	)

	if defined PR_NUMBER (
		echo #define PR_NAME          "PR %PR_NUMBER%"
	) else (
		echo // PR_NAME          is not defined
	)

	if defined PR_SOURCE_URL (
		echo #define PR_SOURCE_URL    "%PR_SOURCE_URL%"
	) else (
		echo // PR_SOURCE_URL    is not defined
	)

	exit /b 0

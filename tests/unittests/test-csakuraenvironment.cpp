/*! @file */
/*
	Copyright (C) 2018-2020 Sakura Editor Organization

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such,
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/
#include <gtest/gtest.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif /* #ifndef NOMINMAX */

#include <tchar.h>
#include <Windows.h>
#include <Shlwapi.h>

#include "config/app_constants.h"
#include "config/maxdata.h"
#include "basis/primitive.h"
#include "debug/Debug2.h"
#include "basis/CMyString.h"
#include "mem/CNativeW.h"
#include "env/CSakuraEnvironment.h"
#include "_main/CCommandLine.h"
#include "_main/CControlProcess.h"
#include "util/file.h"

/*!
 * @brief $記号の取得
 */
TEST(CSakuraEnvironment, ExpandParameter_GetDollar)
{
	SFilePath szBuf;
	CSakuraEnvironment::ExpandParameter(L"$$", szBuf, _countof2(szBuf));
	ASSERT_STREQ(L"$", szBuf.c_str());
}

/*!
 * @brief アプリ名の取得
 */
TEST(CSakuraEnvironment, ExpandParameter_GetAppName)
{
	SFilePath szBuf;
	CSakuraEnvironment::ExpandParameter(L"$A", szBuf, _countof2(szBuf));
	ASSERT_STREQ(GSTR_APPNAME_W, szBuf.c_str());
}

/*!
 * @brief exeファイルパスの取得
 */
TEST(CSakuraEnvironment, ExpandParameter_ExeFileName)
{
	SFilePath szExeFile;
	CSakuraEnvironment::ExpandParameter(L"$S", szExeFile, _countof2(szExeFile));
	ASSERT_STREQ(GetExeFileName().c_str(), szExeFile.c_str());
}

/*!
 * @brief iniファイルパスの取得
 */
TEST(CSakuraEnvironment, ExpandParameter_IniFileNameNoProfile)
{
	// コマンドラインのグローバル変数をセットする
	auto pCommandLine = CCommandLine::getInstance();
	pCommandLine->ParseCommandLine(LR"(-PROF="")", false);

	// プロセスのインスタンスを用意する
	CControlProcess dummy(nullptr, LR"(-PROF="")");

	SFilePath szIniFile;
	CSakuraEnvironment::ExpandParameter(L"$I", szIniFile, _countof2(szIniFile));
	ASSERT_STREQ(GetIniFileName().c_str(), szIniFile.c_str());

	// コマンドラインのグローバル変数を元に戻す
	pCommandLine->ParseCommandLine(L"", false);
}

/*!
 * @brief iniファイルパスの取得
 */
TEST(CSakuraEnvironment, ExpandParameter_IniFileNameNamedProfile)
{
	// コマンドラインのグローバル変数をセットする
	auto pCommandLine = CCommandLine::getInstance();
	pCommandLine->ParseCommandLine(LR"(-PROF="profile1")", false);

	// プロセスのインスタンスを用意する
	CControlProcess dummy(nullptr, LR"(-PROF="profile1")");

	SFilePath szIniFile;
	CSakuraEnvironment::ExpandParameter(L"$I", szIniFile, _countof2(szIniFile));
	ASSERT_STREQ(GetIniFileName().c_str(), szIniFile.c_str());

	// コマンドラインのグローバル変数を元に戻す
	pCommandLine->ParseCommandLine(L"", false);
}

/*!
 * @brief プロファイル名の取得
 */
TEST(CSakuraEnvironment, ExpandParameter_GetProfileNameNoProfile)
{
	// コマンドラインのグローバル変数をセットする
	auto pCommandLine = CCommandLine::getInstance();
	pCommandLine->ParseCommandLine(LR"(-PROF="")", false);

	// プロセスのインスタンスを用意する
	CControlProcess dummy(nullptr, LR"(-PROF="")");

	SFilePath szBuf;
	CSakuraEnvironment::ExpandParameter(L"$<profile>", szBuf, _countof2(szBuf));
	ASSERT_STREQ(L"", szBuf.c_str());

	// コマンドラインのグローバル変数を元に戻す
	pCommandLine->ParseCommandLine(L"", false);
}

/*!
 * @brief プロファイル名の取得
 */
TEST(CSakuraEnvironment, ExpandParameter_GetProfileNameNamedProfile)
{
	// コマンドラインのグローバル変数をセットする
	auto pCommandLine = CCommandLine::getInstance();
	pCommandLine->ParseCommandLine(LR"(-PROF="profile1")", false);

	// プロセスのインスタンスを用意する
	CControlProcess dummy(nullptr, LR"(-PROF="profile1")");

	SFilePath szBuf;
	CSakuraEnvironment::ExpandParameter(L"$<profile>", szBuf, _countof2(szBuf));
	ASSERT_STREQ(L"profile1", szBuf.c_str());

	// コマンドラインのグローバル変数を元に戻す
	pCommandLine->ParseCommandLine(L"", false);
}

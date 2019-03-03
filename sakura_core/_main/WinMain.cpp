﻿/*!	@file
	@brief Entry Point

	@author Norio Nakatani
	@date	1998/03/13 作成
	@date	2001/06/26 genta ワード単位のGrepのためのコマンドライン処理追加
	@date	2002/01/08 aroka 処理の流れを整理、未使用コードを削除
	@date	2002/01/18 aroka 虫取り＆リリース
	@date	2009/01/07 ryoji WinMainにOleInitialize/OleUninitializeを追加
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2001, genta
	Copyright (C) 2002, aroka
	Copyright (C) 2007, kobake
	Copyright (C) 2009, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include <Ole2.h>
#include <locale.h>
#include "CProcessFactory.h"
#include "CProcess.h"
#include "util/os.h"
#include "util/module.h"
#include "debug/CRunningTimer.h"
#include "version.h"

/*!
	Windows Entry point

	コマンドラインオプションで -NOWIN を指定するかどうかでプロセスのタイプが変わる。
		+----------+---------------------------+---------------------------+
		|-NOWIN指定|どうなる？                 |作成するProcessインスタンス|
		+----------+---------------------------+---------------------------+
		|有        |コントロールプロセスとなる |CControlProcessクラス      |
		+----------+---------------------------+---------------------------+
		|無        |エディタプロセスとなる     |CNormalProcessクラス       |
		+----------+---------------------------+---------------------------+
*/
int WINAPI _tWinMain(
	HINSTANCE	hInstance,		//!< handle to current instance
	HINSTANCE	hPrevInstance,	//!< handle to previous instance
	LPTSTR		lpCmdLine,		//!< pointer to command line
	int			nCmdShow		//!< show state of window
)
{
#ifdef USE_LEAK_CHECK_WITH_CRTDBG
	// 2009.9.10 syat メモリリークチェックを追加
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

	MY_RUNNINGTIMER(cRunningTimer, "WinMain" );
	{
		// 2014.04.24 DLLの検索パスからカレントディレクトリを削除する
		::SetDllDirectory( _T("") );
		::SetSearchPathMode( BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE | BASE_SEARCH_PATH_PERMANENT );

		setlocale( LC_ALL, "Japanese" ); //2007.08.16 kobake 追加
		::OleInitialize( NULL );	// 2009.01.07 ryoji 追加
	}
	
	//開発情報
	DEBUG_TRACE(_T("-- -- WinMain -- --\n"));
	DEBUG_TRACE(_T("sizeof(DLLSHAREDATA) = %d\n"),sizeof(DLLSHAREDATA));

	//プロセスの生成とメッセージループ
	CProcessFactory aFactory;
	CProcess *process = 0;
	try{
		process = aFactory.Create( hInstance, lpCmdLine );
		MY_TRACETIME( cRunningTimer, "ProcessObject Created" );
	}
	catch(...){
	}
	if( 0 != process ){
		process->Run();
		delete process;
	}

	::OleUninitialize();	// 2009.01.07 ryoji 追加
	return 0;
}


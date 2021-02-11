/*!	@file
	@brief プロセス生成クラス

	@author aroka
	@date 2002/01/03 Create
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, genta
	Copyright (C) 2001, masami shoji
	Copyright (C) 2002, aroka WinMainより分離
	Copyright (C) 2006, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include "CProcessFactory.h"
#include "CControlProcess.h"
#include "CNormalProcess.h"
#include "CCommandLine.h"
#include "CControlTray.h"
#include "dlg/CDlgProfileMgr.h"
#include "basis/CErrorInfo.h"
#include "debug/CRunningTimer.h"
#include "util/os.h"
#include <io.h>
#include <tchar.h>

class CProcess;

/*!
	@brief プロセスクラスを生成する
	
	コマンドライン、コントロールプロセスの有無を判定し、
	適当なプロセスクラスを生成する。
	
	@param[in] hInstance インスタンスハンドル
	@param[in] lpCmdLine コマンドライン文字列
	
	@author aroka
	@date 2002/01/08
	@date 2006/04/10 ryoji
*/
CProcess* CProcessFactory::Create( HINSTANCE hInstance, LPCWSTR lpCmdLine )
{
	// 言語環境を初期化する
	CSelectLang::InitializeLanguageEnvironment();

	if( !ProfileSelect( hInstance, lpCmdLine ) ){
		return 0;
	}

	CProcess* process = 0;
	if( !IsValidVersion() ){
		return 0;
	}

	// プロセスクラスを生成する
	//
	// Note: 以下の処理において使用される IsExistControlProcess() は、コントロールプロセスが
	// 存在しない場合だけでなく、コントロールプロセスが起動して ::CreateMutex() を実行するまで
	// の間も false（コントロールプロセス無し）を返す。
	// 従って、複数のノーマルプロセスが同時に起動した場合などは複数のコントロールプロセスが
	// 起動されることもある。
	// しかし、そのような場合でもミューテックスを最初に確保したコントロールプロセスが唯一生き残る。
	//
	if( IsStartingControlProcess() ){
		if( !IsExistControlProcess() ){
			process = new CControlProcess( hInstance, lpCmdLine );
		}
	}
	else{
		if( !IsExistControlProcess() ){
			StartControlProcess();
		}
		if( WaitForInitializedControlProcess() ){	// 2006.04.10 ryoji コントロールプロセスの初期化完了待ち
			process = new CNormalProcess( hInstance, lpCmdLine );
		}
	}
	return process;
}

bool CProcessFactory::ProfileSelect( HINSTANCE hInstance, LPCWSTR lpCmdLine )
{
	//	May 30, 2000 genta
	//	実行ファイル名をもとに漢字コードを固定する．
	WCHAR szExeFileName[MAX_PATH];
	const int cchExeFileName = ::GetModuleFileName(NULL, szExeFileName, _countof(szExeFileName));
	CCommandLine::getInstance()->ParseKanjiCodeFromFileName(szExeFileName, cchExeFileName);

	CCommandLine::getInstance()->ParseCommandLine(lpCmdLine);

	// コマンドラインオプションから起動プロファイルを判定する
	bool profileSelected = CDlgProfileMgr::TrySelectProfile( CCommandLine::getInstance() );
	if( !profileSelected ){
		CDlgProfileMgr dlgProf;
		if( dlgProf.DoModal( hInstance, NULL, 0 ) ){
			CCommandLine::getInstance()->SetProfileName( dlgProf.m_strProfileName.c_str() );
		}else{
			return false; // プロファイルマネージャで「閉じる」を選んだ。プロセス終了
		}
	}
	return true;
}

/*!
	@brief Windowsバージョンのチェック
	
	Windows 95以上，Windows NT4.0以上であることを確認する．
	Windows 95系では残りリソースのチェックも行う．
	
	@author aroka
	@date 2002/01/03
*/
bool CProcessFactory::IsValidVersion()
{
	// Windowsバージョンは廃止。
	// 動作可能バージョン(=windows7以降)でなければ起動できない。
	return true;
}

/*!
	@brief コマンドラインに -NOWIN があるかを判定する。
	
	@author aroka
	@date 2002/01/03 作成 2002/01/18 変更
*/
bool CProcessFactory::IsStartingControlProcess()
{
	return CCommandLine::getInstance()->IsNoWindow();
}

/*!
	コントロールプロセスの有無を調べる。
	
	@author aroka
	@date 2002/01/03
	@date 2006/04/10 ryoji
*/
bool CProcessFactory::IsExistControlProcess()
{
	const auto pszProfileName = CCommandLine::getInstance()->GetProfileName();
	std::wstring strMutexSakuraCp = GSTR_MUTEX_SAKURA_CP;
	strMutexSakuraCp += pszProfileName;
 	HANDLE hMutexCP;
	hMutexCP = ::OpenMutex( MUTEX_ALL_ACCESS, FALSE, strMutexSakuraCp.c_str() );	// 2006.04.10 ryoji ::CreateMutex() を ::OpenMutex()に変更
	if( NULL != hMutexCP ){
		::CloseHandle( hMutexCP );
		return true;	// コントロールプロセスが見つかった
	}

	return false;	// コントロールプロセスは存在していないか、まだ CreateMutex() してない
}

/*!
	@brief コントロールプロセスを起動する
	
	自分自身に -NOWIN オプションを付けて起動する．
	共有メモリをチェックしてはいけないので，残念ながらCControlTray::OpenNewEditorは使えない．
	
	@author genta
	@date Aug. 28, 2001
	@date 2008.05.05 novice GetModuleHandle(NULL)→NULLに変更
*/
bool CProcessFactory::StartControlProcess()
{
	MY_RUNNINGTIMER(cRunningTimer,"StartControlProcess" );

	try {
		std::wstring profileName;
		if (const auto* pCommandLine = CCommandLine::getInstance(); pCommandLine->IsSetProfile() && *pCommandLine->GetProfileName()) {
			profileName = pCommandLine->GetProfileName();
		}
		CControlProcess::Start(profileName);
		return true;
	}
	catch (const _com_error& ce) {
		TopErrorMessage(nullptr, (const wchar_t*)ce.Description());
		return false;
	}
}

/*!
	@brief コントロールプロセスの初期化完了イベントを待つ。

	@author ryoji by assitance with karoto
	@date 2006/04/10
*/
bool CProcessFactory::WaitForInitializedControlProcess()
{
	try {
		std::wstring profileName;
		if (const auto* pCommandLine = CCommandLine::getInstance(); pCommandLine->IsSetProfile() && *pCommandLine->GetProfileName()) {
			profileName = pCommandLine->GetProfileName();
		}
		// 初期化完了イベントを待つ
		CControlProcess::WaitForInitialized(profileName);
		return true;
	}
	catch (const _com_error& ce) {
		TopErrorMessage(nullptr, (const wchar_t*)ce.Description());
		return false;
	}
}

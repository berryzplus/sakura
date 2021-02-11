/*!	@file
	@brief コントロールプロセスクラス

	@author aroka
	@date 2002/01/07 Create
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2002, aroka CProcessより分離, YAZAKI
	Copyright (C) 2006, ryoji
	Copyright (C) 2007, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include "CControlProcess.h"
#include "CControlTray.h"
#include "env/DLLSHAREDATA.h"
#include "CCommandLine.h"
#include "env/CShareData_IO.h"
#include "basis/CErrorInfo.h"
#include "debug/CRunningTimer.h"
#include "sakura_rc.h"/// IDD_EXITTING 2002/2/10 aroka ヘッダ整理

//-------------------------------------------------

/*!
 * @see https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessagew
 */
std::wstring FormatMessageW(DWORD dwErrorCode, DWORD dwLanguageId)
{
	WCHAR* pMsg;
	DWORD dwLength = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		dwErrorCode,
		dwLanguageId,
		(LPWSTR)&pMsg,
		0,
		nullptr
	);

	std::wstring strMessage;
	if (dwLength > 0) {
		strMessage = pMsg;
	}
	::LocalFree((HLOCAL)pMsg);

	return strMessage;
}

/*!
	@brief iniファイルパスを取得する
 */
std::filesystem::path CControlProcess::GetIniFileName() const
{
	if (GetShareDataPtr()->IsPrivateSettings()) {
		return CProcess::GetIniFileName();
	}

	// exe基準のiniファイルパスを得る
	auto iniPath = GetExeFileName().replace_extension(L".ini");

	// マルチユーザ用のiniファイルパス
	//		exeと同じフォルダに置かれたマルチユーザ構成設定ファイル（sakura.exe.ini）の内容
	//		に従ってマルチユーザ用のiniファイルパスを決める
	auto exeIniPath = GetExeFileName().concat(L".ini");
	if (bool isMultiUserSeggings = ::GetPrivateProfileInt(L"Settings", L"MultiUser", 0, exeIniPath.c_str()); isMultiUserSeggings) {
		return GetPrivateIniFileName(exeIniPath, iniPath.filename());
	}

	const auto filename = iniPath.filename();
	iniPath.remove_filename();

	if (const auto* pCommandLine = CCommandLine::getInstance(); pCommandLine->IsSetProfile() && *pCommandLine->GetProfileName()) {
		iniPath.append(pCommandLine->GetProfileName());
	}

	return iniPath.append(filename.c_str());
}

/*!
	@brief マルチユーザ用のiniファイルパスを取得する
 */
std::filesystem::path CControlProcess::GetPrivateIniFileName(const std::wstring& exeIniPath, const std::wstring& filename) const
{
	KNOWNFOLDERID refFolderId;
	switch (int nFolder = ::GetPrivateProfileInt(L"Settings", L"UserRootFolder", 0, exeIniPath.c_str())) {
	case 1:
		refFolderId = FOLDERID_Profile;			// ユーザのルートフォルダ
		break;
	case 2:
		refFolderId = FOLDERID_Documents;		// ユーザのドキュメントフォルダ
		break;
	case 3:
		refFolderId = FOLDERID_Desktop;			// ユーザのデスクトップフォルダ
		break;
	default:
		refFolderId = FOLDERID_RoamingAppData;	// ユーザのアプリケーションデータフォルダ
		break;
	}

	PWSTR pFolderPath = nullptr;
	::SHGetKnownFolderPath(refFolderId, KF_FLAG_DEFAULT, nullptr, &pFolderPath);
	std::filesystem::path privateIniPath(pFolderPath);
	::CoTaskMemFree(pFolderPath);

	std::wstring subFolder(_MAX_DIR, L'\0');
	::GetPrivateProfileString(L"Settings", L"UserSubFolder", L"sakura", subFolder.data(), (DWORD)subFolder.capacity(), exeIniPath.c_str());
	subFolder.assign(subFolder.data());
	if (subFolder.empty())
	{
		subFolder = L"sakura";
	}
	privateIniPath.append(subFolder);

	if (const auto* pCommandLine = CCommandLine::getInstance(); pCommandLine->IsSetProfile() && *pCommandLine->GetProfileName()) {
		privateIniPath.append(pCommandLine->GetProfileName());
	}

	return privateIniPath.append(filename.c_str());
}

/*!
 * HANDLE型のスマートポインタを実現するためのdeleterクラス
 */
struct handle_closer
{
	void operator()(HANDLE handle) const
	{
		::CloseHandle(handle);
	}
};

//! HANDLE型のスマートポインタ
using handleHolder = std::unique_ptr<std::remove_pointer<HANDLE>::type, handle_closer>;

/*!
 * @brief コントロールプロセスを起動する
 */
void CControlProcess::Start(std::optional<std::wstring> profileName)
{
	// スタートアップ情報
	STARTUPINFO si = { sizeof(STARTUPINFO), 0 };
	si.lpTitle = (LPWSTR)L"sakura control process";
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWDEFAULT;

	auto exePath = GetExeFileName();
	std::wstring strCommandLine(strprintf(L"\"%s\" -NOWIN", exePath.c_str()));
	std::wstring strProfileName;
	if (profileName.has_value()) {
		strProfileName = profileName.value();
		strCommandLine += strprintf(L" -PROF=\"%s\"", strProfileName.data());
	}

	LPWSTR pszCommandLine = strCommandLine.data();
	DWORD dwCreationFlag = CREATE_DEFAULT_ERROR_MODE;
	PROCESS_INFORMATION pi;

	// コントロールプロセスを起動する
	BOOL createSuccess = ::CreateProcess(
		exePath.c_str(),	// 実行可能モジュールパス
		pszCommandLine,		// コマンドラインバッファ
		NULL,				// プロセスのセキュリティ記述子
		NULL,				// スレッドのセキュリティ記述子
		FALSE,				// ハンドルの継承オプション(継承させない)
		dwCreationFlag,		// 作成のフラグ
		NULL,				// 環境変数(変更しない)
		NULL,				// カレントディレクトリ(変更しない)
		&si,				// スタートアップ情報
		&pi					// プロセス情報(作成されたプロセス情報を格納する構造体)
	);
	if( !createSuccess ){
		const auto sysMsg = FormatMessageW(
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
		);
		// L"'%s'\nプロセスの起動に失敗しました。\n%s"
		::_com_raise_error(E_FAIL, MakeMsgError(strprintf(LS(STR_ERR_DLGPROCFACT3), exePath.c_str(), sysMsg.data())));
	}

	// 開いたハンドルは使わないので閉じておく
	::CloseHandle( pi.hThread );
	::CloseHandle( pi.hProcess );

	// コントロールプロセスの初期化完了を待つ
	WaitForInitialized(strProfileName);
}

/*!
 * @brief コントロールプロセスの初期化完了を待つ
 */
void CControlProcess::WaitForInitialized(std::wstring_view profileName)
{
	// 初期化完了イベントを作成する
	std::wstring strInitEvent(GSTR_EVENT_SAKURA_CP_INITIALIZED);
	if (profileName.length() > 0) {
		strInitEvent += profileName;
	}
	auto hEvent = ::CreateEvent(nullptr, TRUE, FALSE, strInitEvent.data());
	if (!hEvent) {
		// L"エディタまたはシステムがビジー状態です。\nしばらく待って開きなおしてください。"
		::_com_raise_error(E_FAIL, MakeMsgError(LS(STR_ERR_DLGPROCFACT5)));
	}

	// イベントハンドラをスマートポインタに入れる
	handleHolder eventHolder(hEvent);

	// 初期化完了イベントを待つ
	if (DWORD dwRet = ::WaitForSingleObject(hEvent, 10000); dwRet == WAIT_TIMEOUT) {
		// L"エディタまたはシステムがビジー状態です。\nしばらく待って開きなおしてください。"
		::_com_raise_error(E_FAIL, MakeMsgError(LS(STR_ERR_DLGPROCFACT5)));
	}
}

/*!
 * @brief コントロールプロセスに終了指示を出して終了を待つ
 *
 */
void CControlProcess::Terminate(std::wstring_view profileName)
{
	// トレイウインドウを検索する
	std::wstring strCEditAppName(GSTR_CEDITAPP);
	if (profileName.length() > 0) {
		strCEditAppName += profileName;
	}
	HWND hTrayWnd = ::FindWindow(strCEditAppName.data(), strCEditAppName.data());
	if (!hTrayWnd) {
		::_com_raise_error(E_FAIL, MakeMsgError(L"トレイウインドウが見つかりませんでした。"));
	}

	// トレイウインドウからプロセスIDを取得する
	DWORD dwControlProcessId = 0;
	::GetWindowThreadProcessId(hTrayWnd, &dwControlProcessId);
	if (!dwControlProcessId) {
		::_com_raise_error(E_FAIL, MakeMsgError(L"プロセスIDを取得できませんでした。"));
	}

	// トレイウインドウを閉じる
	::SendMessage(hTrayWnd, WM_CLOSE, 0, 0);

	// プロセス情報の問い合せを行うためのハンドルを開く
	HANDLE hControlProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, dwControlProcessId);
	if (!hControlProcess) {
		::_com_raise_error(E_FAIL, MakeMsgError(L"プロセスハンドルを取得できませんでした。"));
	}

	// プロセスハンドルをスマートポインタに入れる
	handleHolder processHolder(hControlProcess);

	// プロセス終了を待つ
	DWORD dwExitCode = 0;
	if (::GetExitCodeProcess(hControlProcess, &dwExitCode) && dwExitCode == STILL_ACTIVE) {
		DWORD waitProcessResult = ::WaitForSingleObject(hControlProcess, INFINITE);
		if (waitProcessResult == WAIT_TIMEOUT) {
			::_com_raise_error(E_FAIL, MakeMsgError(L"プロセスが時間内に終了しませんでした。"));
		}
	}
}

/*!
	@brief コントロールプロセスを初期化する
	
	MutexCPを作成・ロックする。
	CControlTrayを作成する。
	
	@author aroka
	@date 2002/01/07
	@date 2002/02/17 YAZAKI 共有メモリを初期化するのはCProcessに移動。
	@date 2006/04/10 ryoji 初期化完了イベントの処理を追加、異常時の後始末はデストラクタに任せる
	@date 2013.03.20 novice コントロールプロセスのカレントディレクトリをシステムディレクトリに変更
*/
bool CControlProcess::InitializeProcess()
{
	MY_RUNNINGTIMER( cRunningTimer, "CControlProcess::InitializeProcess" );

	// アプリケーション実行検出用(インストーラで使用)
	m_hMutex = ::CreateMutex( NULL, FALSE, GSTR_MUTEX_SAKURA );
	if( NULL == m_hMutex ){
		ErrorBeep();
		TopErrorMessage( NULL, L"CreateMutex()失敗。\n終了します。" );
		return false;
	}

	const auto pszProfileName = CCommandLine::getInstance()->GetProfileName();

	// 初期化完了イベントを作成する
	std::wstring strInitEvent = GSTR_EVENT_SAKURA_CP_INITIALIZED;
	strInitEvent += pszProfileName;
	m_hEventCPInitialized = ::CreateEvent( NULL, TRUE, FALSE, strInitEvent.c_str() );
	if( NULL == m_hEventCPInitialized )
	{
		ErrorBeep();
		TopErrorMessage( NULL, L"CreateEvent()失敗。\n終了します。" );
		return false;
	}

	/* コントロールプロセスの目印 */
	std::wstring strCtrlProcEvent = GSTR_MUTEX_SAKURA_CP;
	strCtrlProcEvent += pszProfileName;
	m_hMutexCP = ::CreateMutex( NULL, TRUE, strCtrlProcEvent.c_str() );
	if( NULL == m_hMutexCP ){
		ErrorBeep();
		TopErrorMessage( NULL, L"CreateMutex()失敗。\n終了します。" );
		return false;
	}
	if( ERROR_ALREADY_EXISTS == ::GetLastError() ){
		return false;
	}
	
	/* 共有メモリを初期化 */
	if( !CProcess::InitializeProcess() ){
		return false;
	}

	// コントロールプロセスのカレントディレクトリをシステムディレクトリに変更
	WCHAR szDir[_MAX_PATH];
	::GetSystemDirectory( szDir, _countof(szDir) );
	::SetCurrentDirectory( szDir );

	/* 共有データのロード */
	if( !CShareData_IO::LoadShareData() ){
		/* レジストリ項目 作成 */
		CShareData_IO::SaveShareData();
	}

	/* 言語を選択する */
	CSelectLang::ChangeLang( GetDllShareData().m_Common.m_sWindow.m_szLanguageDll );
	RefreshString();

	MY_TRACETIME( cRunningTimer, "Before new CControlTray" );

	/* タスクトレイにアイコン作成 */
	m_pcTray = new CControlTray;

	MY_TRACETIME( cRunningTimer, "After new CControlTray" );

	HWND hwnd = m_pcTray->Create( GetProcessInstance() );
	if( !hwnd ){
		ErrorBeep();
		TopErrorMessage( NULL, LS(STR_ERR_CTRLMTX3) );
		return false;
	}
	SetMainWindow(hwnd);
	GetDllShareData().m_sHandles.m_hwndTray = hwnd;

	// 初期化完了イベントをシグナル状態にする
	if( !::SetEvent( m_hEventCPInitialized ) ){
		ErrorBeep();
		TopErrorMessage( NULL, LS(STR_ERR_CTRLMTX4) );
		return false;
	}

	return true;
}

/*!
	@brief コントロールプロセスのメッセージループ
	
	@author aroka
	@date 2002/01/07
*/
bool CControlProcess::MainLoop()
{
	if( m_pcTray && GetMainWindow() ){
		m_pcTray->MessageLoop();	/* メッセージループ */
		return true;
	}
	return false;
}

/*!
	@brief コントロールプロセスを終了する
	
	@author aroka
	@date 2002/01/07
	@date 2006/07/02 ryoji 共有データ保存を CControlTray へ移動
*/
void CControlProcess::OnExitProcess()
{
	GetDllShareData().m_sHandles.m_hwndTray = NULL;
}

CControlProcess::~CControlProcess()
{
	delete m_pcTray;

	if( m_hEventCPInitialized ){
		::ResetEvent( m_hEventCPInitialized );
	}
	::CloseHandle( m_hEventCPInitialized );
	if( m_hMutexCP ){
		::ReleaseMutex( m_hMutexCP );
	}
	::CloseHandle( m_hMutexCP );
	// 旧バージョン（1.2.104.1以前）との互換性：「異なるバージョン...」が二回出ないように
	if( m_hMutex ){
		::ReleaseMutex( m_hMutex );
	}
	::CloseHandle( m_hMutex );
};

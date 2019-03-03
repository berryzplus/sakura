/*!	@file
	@brief 文字列共通定義

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2001, MIK, Stonee, jepro
	Copyright (C) 2002, KK
	Copyright (C) 2003, MIK
	Copyright (C) 2005, Moca

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include "global.h"
#include "window/CEditWnd.h"
#include "CNormalProcess.h"

//2007.10.02 kobake CEditWndのインスタンスへのポインタをここに保存しておく
CEditWnd* g_pcEditWnd = NULL;

/*! 選択領域描画用パラメータ */
const COLORREF	SELECTEDAREA_RGB = RGB( 255, 255, 255 );
const int		SELECTEDAREA_ROP2 = R2_XORPEN;

HINSTANCE G_AppInstance()
{
	return CProcess::getInstance()->GetProcessInstance();
}

// アプリ名。2007.09.21 kobake 整理
#ifdef _UNICODE
#define _APP_NAME_(TYPE) TYPE("sakura")
#else
#define _APP_NAME_(TYPE) TYPE("sakura")
#endif

#ifdef _DEBUG
#define _APP_NAME_2_(TYPE) TYPE("(デバッグ版)")
#else
#define _APP_NAME_2_(TYPE) TYPE("")
#endif

#ifdef ALPHA_VERSION
#define _APP_NAME_3_(TYPE) TYPE("(Alpha Version)")
#else
#define _APP_NAME_3_(TYPE) TYPE("")
#endif

#ifdef APPVEYOR_DEV_VERSION
#define _APP_NAME_DEV_(TYPE) TYPE("(dev Version)")
#else
#define _APP_NAME_DEV_(TYPE) TYPE("")
#endif

#define _GSTR_APPNAME_(TYPE)  _APP_NAME_(TYPE) _APP_NAME_2_(TYPE) _APP_NAME_DEV_(TYPE) _APP_NAME_3_(TYPE)

const TCHAR g_szGStrAppName[]  = (_GSTR_APPNAME_(_T)   ); // この変数を直接参照せずに GSTR_APPNAME を使うこと
const CHAR  g_szGStrAppNameA[] = (_GSTR_APPNAME_(ATEXT)); // この変数を直接参照せずに GSTR_APPNAME_A を使うこと
const WCHAR g_szGStrAppNameW[] = (_GSTR_APPNAME_(LTEXT)); // この変数を直接参照せずに GSTR_APPNAME_W を使うこと

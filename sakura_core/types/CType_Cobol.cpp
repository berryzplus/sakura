﻿/*! @file */
/*
	Copyright (C) 2008, kobake
	Copyright (C) 2018-2021, Sakura Editor Organization

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

#include "StdAfx.h"
#include "types/CType.h"

#include <string_view>

#include "doc/CEditDoc.h"
#include "doc/CDocOutline.h"
#include "doc/logic/CDocLine.h"
#include "outline/CFuncInfoArr.h"
#include "view/Colors/EColorIndexType.h"

/* COBOL */
void CType_Cobol::InitTypeConfigImp(STypeConfig* pType)
{
	//名前と拡張子
	wcscpy( pType->m_szTypeName, L"COBOL" );
	wcscpy( pType->m_szTypeExts, L"cbl,cpy,pco,cob" );	//Jun. 04, 2001 JEPRO KENCH氏の助言に従い追加

	//設定
	pType->m_cLineComment.CopyTo( 0, L"*", 6 );			//Jun. 02, 2001 JEPRO 修正
	pType->m_cLineComment.CopyTo( 1, L"D", 6 );			//Jun. 04, 2001 JEPRO 追加
	pType->m_nStringType = STRING_LITERAL_PLSQL;							/* 文字列区切り記号エスケープ方法  0=[\"][\'] 1=[""][''] */
	wcscpy( pType->m_szIndentChars, L"*" );				/* その他のインデント対象文字 */
	pType->m_nKeyWordSetIdx[0] = 3;						/* キーワードセット */		//Jul. 10, 2001 JEPRO
	pType->m_eDefaultOutline = OUTLINE_COBOL;			/* アウトライン解析方法 */
	// 指定桁縦線	//2005.11.08 Moca
	pType->m_ColorInfoArr[COLORIDX_VERTLINE].m_bDisp = true;
	pType->m_nVertLineIdx[0] = CKetaXInt(7);
	pType->m_nVertLineIdx[1] = CKetaXInt(8);
	pType->m_nVertLineIdx[2] = CKetaXInt(12);
	pType->m_nVertLineIdx[3] = CKetaXInt(73);
}

/*! COBOL アウトライン解析 */
void CDocOutline::MakeTopicList_cobol( CFuncInfoArr* pcFuncInfoArr )
{
	const bool	bExtEol = GetDllShareData().m_Common.m_sEdit.m_bEnableExtEol;

	wchar_t szDivision[1024] = {};
	wchar_t szLabel[1024] = {};

	for( CLogicInt nLineCount; nLineCount <  m_pcDocRef->m_cDocLineMgr.GetLineCount(); ++nLineCount ){
		// 行データ取得
		CLogicInt nLineLen;
		const auto *pLine = m_pcDocRef->m_cDocLineMgr.GetLine(nLineCount)->GetDocLineStrWithEOL(&nLineLen);
		if( pLine == nullptr ){
			break;
		}
		if( nLineLen < 7 || pLine[6] == L'*' ){
			continue;	// データ長が足りないか、コメント行
		}
		if( nLineLen < 8 || pLine[7] == L' ' ){
			continue;	// データ長が足りないか、ラベル行
		}

		// 8文字目からドットまたは行区切りまでを抽出する
		size_t i = 7;
		size_t j = 0;
		while( i < nLineLen && j + 1 < _countof(szLabel) ){
			if( pLine[i] == '.' || WCODE::IsLineDelimiter(pLine[i], bExtEol) ){
				break;
			}
			szLabel[j] = pLine[i];
			++j;
			++i;
		}
		szLabel[j] = L'\0';		// NUL終端する

		const std::wstring_view keyword(L"division");
		if( auto *pEnd = szLabel + ::wcslen( szLabel );
			pEnd != std::find_if( &szLabel[0], pEnd, [keyword, pEnd] ( const wchar_t& ch ) { return &ch + keyword.length() <= pEnd && 0 == wmemicmp( &ch, keyword.data(), keyword.length() ); } ) ){
			::wcscpy_s( szDivision, szLabel );
			continue;
		}

		CLayoutPoint ptPos;
		m_pcDocRef->m_cLayoutMgr.LogicToLayout(
			CLogicPoint(0, nLineCount),
			&ptPos
		);

		std::wstring strFuncName = strprintf( L"%s::%s", szDivision, szLabel );
		pcFuncInfoArr->AppendData( nLineCount + 1, ptPos.GetY2() + 1, strFuncName.c_str(), 0 );
	}
	return;
}

//Jul. 10, 2001 JEPRO 追加
const wchar_t* g_ppszKeywordsCOBOL[] = {
	L"ACCEPT",
	L"ADD",
	L"ADVANCING",
	L"AFTER",
	L"ALL",
	L"AND",
	L"ARGUMENT",
	L"ASSIGN",
	L"AUTHOR",
	L"BEFORE",
	L"BLOCK",
	L"BY",
	L"CALL",
	L"CHARACTERS",
	L"CLOSE",
	L"COMP",
	L"COMPILED",
	L"COMPUTE",
	L"COMPUTER",
	L"CONFIGURATION",
	L"CONSOLE",
	L"CONTAINS",
	L"CONTINUE",
	L"CONTROL",
	L"COPY",
	L"DATA",
	L"DELETE",
	L"DISPLAY",
	L"DIVIDE",
	L"DIVISION",
	L"ELSE",
	L"END",
	L"ENVIRONMENT",
	L"EVALUATE",
	L"EXAMINE",
	L"EXIT",
	L"EXTERNAL",
	L"FD",
	L"FILE",
	L"FILLER",
	L"FROM",
	L"GIVING",
	L"GO",
	L"GOBACK",
	L"HIGH-VALUE",
	L"IDENTIFICATION",
	L"IF",
	L"INITIALIZE",
	L"INPUT",
	L"INTO",
	L"IS",
	L"LABEL",
	L"LINKAGE",
	L"LOW-VALUE",
	L"MODE",
	L"MOVE",
	L"NOT",
	L"OBJECT",
	L"OCCURS",
	L"OF",
	L"ON",
	L"OPEN",
	L"OR",
	L"OTHER",
	L"OUTPUT",
	L"PERFORM",
	L"PIC",
	L"PROCEDURE",
	L"PROGRAM",
	L"READ",
	L"RECORD",
	L"RECORDING",
	L"REDEFINES",
	L"REMAINDER",
	L"REMARKS",
	L"REPLACING",
	L"REWRITE",
	L"ROLLBACK",
	L"SECTION",
	L"SELECT",
	L"SOURCE",
	L"SPACE",
	L"STANDARD",
	L"STOP",
	L"STORAGE",
	L"SYSOUT",
	L"TEST",
	L"THEN",
	L"TO",
	L"TODAY",
	L"TRANSFORM",
	L"UNTIL",
	L"UPON",
	L"USING",
	L"VALUE",
	L"VARYING",
	L"WHEN",
	L"WITH",
	L"WORKING",
	L"WRITE",
	L"WRITTEN",
	L"ZERO"
};
int g_nKeywordsCOBOL = _countof(g_ppszKeywordsCOBOL);

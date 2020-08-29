/******************************************************************************/
/*                                                                            */
/*　MIDIData.c - MIDIDataソースファイル                    (C)2002-2020 くず  */
/*                                                                            */
/******************************************************************************/

/* このモジュールは普通のＣ言語で書かれている。 */
/* このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布される。 */
/* プロジェクトホームページ："http://openmidiproject.sourceforge.jp/index.html" */
/* MIDIイベントの取得・設定・生成・挿入・削除 */
/* MIDIトラックの取得・設定・生成・挿入・削除 */
/* MIDIデータの生成・削除・SMFファイル(*.mid)入出力 */

/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <wchar.h>
#include <locale.h>
#include <windows.h>
#include <crtdbg.h>
#include "MIDIData.h"

/* 念のためverifyマクロも定義 */
#ifndef verify
#ifdef _DEBUG
#define verify(f) assert(f)
#else
#define verify(f) ((void)(f))
#endif
#endif

/* 汎用マクロ(最小、最大、挟み込み) */
#ifndef MIN
#define MIN(A,B) ((A)>(B)?(B):(A))
#endif
#ifndef MAX
#define MAX(A,B) ((A)>(B)?(A):(B))
#endif
#ifndef CLIP
#define CLIP(A,B,C) ((A)>(B)?(A):((B)>(C)?(C):(B)))
#endif

/* WSIZEOFマクロ */
#define WSIZEOF(STRING) (sizeof(STRING)/sizeof(wchar_t))

/* 64ビットの整数型int64_t定義 */
/* Microsoft C又はBorland Cの場合は次の行を有効にしてください。 */
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__WATCOMC__)
typedef __int64 int64_t;
/* GNU又はANSI C99対応コンパイラの場合は次の行を有効にしてください。 */
#elif (defined(__GNUC__) && \
      (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)))
typedef long long int64_t;
/* インテルコンパイラの場合は次の行を有効にしてください。*/
#elif defined(__INTEL_COMPILER)
typedef long long int64_t;
/* 64bitｓコンパイラの場合は次の行を有効にしてください。 */
#elif LONG_MAX > 2147483647l
typedef long int64_t;
#endif

/* 汎用関数 */

/* 指定したバイト配列の和を求める */
long Sum (unsigned char* pData, int nLen) {
	long lRet = 0;
	long i;
	assert (pData);
	for (i = 0; i < nLen; i++) {
		lRet += pData[i];
	}
	return lRet;
}

/* UTF16をUTF8に変換 */
size_t wcstoutf8 (char *dest, const wchar_t *source, size_t maxCount) {
	memset (dest, 0, maxCount);
	return WideCharToMultiByte (CP_UTF8, 0, source, -1, dest, maxCount, NULL, NULL);
}

/* UTF8をUTF16に変換 */
size_t utf8towcs (wchar_t *dest, const char *source, size_t maxCount) {
	memset (dest, 0, maxCount * sizeof (wchar_t));
	return MultiByteToWideChar (CP_UTF8, 0, source, -1, dest, maxCount);
}

/******************************************************************************/
/*                                                                            */
/*　MIDIDataLibクラス関数                                                     */
/*                                                                            */
/******************************************************************************/

/* MIDIDataライブラリ環境保持用変数 */
typedef struct {
	long m_lDefaultCharCode;
} MIDIDataLib;

MIDIDataLib g_theMIDIDataLib;

/* デフォルト文字コードの設定 */
long __stdcall MIDIDataLib_SetDefaultCharCode (long lCharCode) {
	g_theMIDIDataLib.m_lDefaultCharCode = lCharCode;
	return 1;
}

/* ロケールの設定(ANSI)(20140517無効化) */
char* __stdcall MIDIDataLib_SetLocaleA (int nCategory, const char *pszLocale) {
/*	g_theMIDIDataLib.m_nCategory = nCategory;
	memset (g_theMIDIDataLib.m_szLocale, 0, sizeof (g_theMIDIDataLib.m_szLocale));
	strncpy (g_theMIDIDataLib.m_szLocale, pszLocale, sizeof (g_theMIDIDataLib.m_szLocale) - 1);
	return setlocale (nCategory, pszLocale);*/
	return NULL;
}

/* ロケールの設定(UNICODE)(20140517無効化) */
wchar_t* __stdcall MIDIDataLib_SetLocaleW (int nCategory, const wchar_t* pszLocale) {
/*	long lRet = 0;
	g_theMIDIDataLib.m_nCategory = nCategory;
	memset (g_theMIDIDataLib.m_szLocale, 0, sizeof (g_theMIDIDataLib.m_szLocale));
	lRet = wcstombs (g_theMIDIDataLib.m_szLocale, pszLocale, sizeof (g_theMIDIDataLib.m_szLocale) - 1);
	if (lRet == -1) {
		return NULL;
	}
	return _wsetlocale (nCategory, pszLocale);*/
	return NULL;
}

/* DLLMain */
BOOL __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	/* DLLが読み込まれたとき */
	if (fdwReason == DLL_PROCESS_ATTACH) {
	}
	return TRUE;
}

/******************************************************************************/
/*                                                                            */
/*　MIDIEventクラス関数                                                       */
/*                                                                            */
/******************************************************************************/

/* 次の同じ種類のイベントを探索 */
/* (この関数は内部隠蔽されています) */
MIDIEvent* __stdcall MIDIEvent_SearchNextSameKindEvent (MIDIEvent* pEvent) {
	MIDIEvent* pSameKindEvent = pEvent->m_pNextEvent;
	while (pSameKindEvent) {
		if (pEvent->m_lKind == pSameKindEvent->m_lKind) {
			break;
		}
		pSameKindEvent = pSameKindEvent->m_pNextEvent;
	}
	return pSameKindEvent;
}

/* 前の同じ種類のイベントを探索 */
/* (この関数は内部隠蔽されています) */
MIDIEvent* __stdcall MIDIEvent_SearchPrevSameKindEvent (MIDIEvent* pEvent) {
	MIDIEvent* pSameKindEvent = pEvent->m_pPrevEvent;
	while (pSameKindEvent) {
		if (pEvent->m_lKind == pSameKindEvent->m_lKind) {
			break;
		}
		pSameKindEvent = pSameKindEvent->m_pPrevEvent;
	}
	return pSameKindEvent;
}

/* 結合イベントの最初のイベントを返す。 */
/* 結合イベントでない場合、pEvent自身を返す。*/
MIDIEvent* __stdcall MIDIEvent_GetFirstCombinedEvent (MIDIEvent* pEvent) {
	MIDIEvent* pTempEvent = pEvent;
	assert (pEvent);
	while (pTempEvent->m_pPrevCombinedEvent) {
		pTempEvent = pTempEvent->m_pPrevCombinedEvent;
	}
	return pTempEvent;
}

/* 結合イベントの最後のイベントを返す。 */
/* 結合イベントでない場合、pEvent自身を返す。*/
MIDIEvent* __stdcall MIDIEvent_GetLastCombinedEvent (MIDIEvent* pEvent) {
	MIDIEvent* pTempEvent = pEvent;
	assert (pEvent);
	while (pTempEvent->m_pNextCombinedEvent) {
		pTempEvent = pTempEvent->m_pNextCombinedEvent;
	}
	return pTempEvent;
}


/* イベントを一時的に浮遊させる */
/* (この関数は内部隠蔽されています) */
long __stdcall MIDIEvent_SetFloating (MIDIEvent* pEvent) {
	/* ただし、結合イベントの解除は行わないことに要注意 */
	assert (pEvent);
	/* 前後のイベントのポインタのつなぎ替え */
	if (pEvent->m_pPrevEvent) {
		pEvent->m_pPrevEvent->m_pNextEvent = pEvent->m_pNextEvent;
	}
	else if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_pFirstEvent = pEvent->m_pNextEvent;
	}
	if (pEvent->m_pNextEvent) {
		pEvent->m_pNextEvent->m_pPrevEvent = pEvent->m_pPrevEvent;
	}
	else if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_pLastEvent = pEvent->m_pPrevEvent;
	}
	/* 前後の同種イベントのポインタのつなぎ替え */
	if (pEvent->m_pNextSameKindEvent) {
		pEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent = pEvent->m_pPrevSameKindEvent;
	}
	if (pEvent->m_pPrevSameKindEvent) {
		pEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = pEvent->m_pNextSameKindEvent;
	}
	/* 前後ポインタのNULL化 */
	pEvent->m_pNextEvent = NULL;
	pEvent->m_pPrevEvent = NULL;
	/* 前後の同種イベントポインタNULL化 */
	pEvent->m_pNextSameKindEvent = NULL;
	pEvent->m_pPrevSameKindEvent = NULL;
	/* 親トラックのイベント数を1減らす。 */
	if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_lNumEvent --;
	}
	pEvent->m_pParent = NULL;
	return 1;
}


/* 前のイベントを設定する */
/* (この関数は内部隠蔽されています) */
long __stdcall MIDIEvent_SetPrevEvent (MIDIEvent* pEvent, MIDIEvent* pInsertEvent) {
	assert (pEvent);
	assert (pInsertEvent);
	assert (pEvent != pInsertEvent);
	/* pInsertEventが既にどこかのトラックに属している場合、異常終了 */
	if (!MIDIEvent_IsFloating (pInsertEvent)) {
		return 0;
	}
	/* EOTの二重挿入防止機構(廃止・上層の関数で処理) */
	/*if (MIDIEvent_IsEndofTrack (pInsertEvent)) {
		return 0;
	}*/
	/* EOTイベントの前に挿入する場合、EOTイベントの時刻を補正する */
	if (MIDIEvent_IsEndofTrack (pEvent) && pEvent->m_pNextEvent == NULL) {
		assert (pEvent->m_pNextEvent == NULL);
		if (pEvent->m_lTime < pInsertEvent->m_lTime) { /* 20080622追加 */
			pEvent->m_lTime = pInsertEvent->m_lTime;
		}
	}
	/* 時刻の整合性がとれていない場合、自動的に挿入イベントの時刻を補正する */
	if (pInsertEvent->m_lTime > pEvent->m_lTime) {
		pInsertEvent->m_lTime = pEvent->m_lTime;
	}
	if (pEvent->m_pPrevEvent) {
		if (pInsertEvent->m_lTime < pEvent->m_pPrevEvent->m_lTime) {
			pInsertEvent->m_lTime = pEvent->m_pPrevEvent->m_lTime;
		}
	}
	/* 前後のイベントのポインタのつなぎかえ */
	pInsertEvent->m_pNextEvent = pEvent;
	pInsertEvent->m_pPrevEvent = pEvent->m_pPrevEvent;
	if (pEvent->m_pPrevEvent) {
		pEvent->m_pPrevEvent->m_pNextEvent = pInsertEvent;
	}
	else if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_pFirstEvent = pInsertEvent;
	}
	pEvent->m_pPrevEvent = pInsertEvent;
	/* 前後の同種イベントのポインタのつなぎかえ */
	if (pInsertEvent->m_pPrevSameKindEvent) {
		pInsertEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = 
			MIDIEvent_SearchNextSameKindEvent (pInsertEvent->m_pPrevSameKindEvent);
	}
	if (pInsertEvent->m_pNextSameKindEvent) {
		pInsertEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent =
			MIDIEvent_SearchPrevSameKindEvent (pInsertEvent->m_pNextSameKindEvent);
	}
	/* 前後の同種イベントポインタ設定 */
	pInsertEvent->m_pPrevSameKindEvent = MIDIEvent_SearchPrevSameKindEvent (pInsertEvent);
	if (pInsertEvent->m_pPrevSameKindEvent) {
		pInsertEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = pInsertEvent;
	}
	pInsertEvent->m_pNextSameKindEvent = MIDIEvent_SearchNextSameKindEvent (pInsertEvent);
	if (pInsertEvent->m_pNextSameKindEvent) {
		pInsertEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent = pInsertEvent;
	}
	/* 親トラックのイベント数を1多くする */
	pInsertEvent->m_pParent = pEvent->m_pParent;
	if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_lNumEvent ++;
	}
	return 1;
}

/* 次のイベントを設定する */
/* (この関数は内部隠蔽されています) */
long __stdcall MIDIEvent_SetNextEvent (MIDIEvent* pEvent, MIDIEvent* pInsertEvent) {
	assert (pEvent);
	assert (pInsertEvent);
	assert (pEvent != pInsertEvent);
	/* pInsertEventが既にどこかのトラックに属している場合、異常終了 */
	if (!MIDIEvent_IsFloating (pInsertEvent)) {
		return 0;
	}
	/* EOTの二重挿入防止機構(廃止・上層の関数で処理) */
	/*if (MIDIEvent_IsEndofTrack (pInsertEvent)) {
		return 0;
	}*/
	/* EOTの後にイベントを入れようとした場合、EOTが後ろに移動しない。 */
	if (MIDIEvent_IsEndofTrack (pEvent) && pEvent->m_pNextEvent == NULL) {
		assert (0);
		return 0; /* EOTの次にはイベントは来れない */
	}
	/* 時刻の整合性がとれていない場合、自動的に挿入イベントの時刻を補正する */
	if (pInsertEvent->m_lTime < pEvent->m_lTime) {
		pInsertEvent->m_lTime = pEvent->m_lTime;
	}
	if (pEvent->m_pNextEvent) {
		if (pInsertEvent->m_lTime > pEvent->m_pNextEvent->m_lTime) {
			pInsertEvent->m_lTime = pEvent->m_pNextEvent->m_lTime;
		}
	}
	/* 前後のイベントのポインタのつなぎかえ */
	pInsertEvent->m_pNextEvent = pEvent->m_pNextEvent;
	pInsertEvent->m_pPrevEvent = pEvent;
	if (pEvent->m_pNextEvent) {
		pEvent->m_pNextEvent->m_pPrevEvent = pInsertEvent;
	}
	else if (pEvent->m_pParent) { /* 最後 */
		((MIDITrack*)(pEvent->m_pParent))->m_pLastEvent = pInsertEvent;
	}
	pEvent->m_pNextEvent = pInsertEvent;
	/* 前後の同種イベントのポインタのつなぎかえ */
	if (pInsertEvent->m_pPrevSameKindEvent) {
		pInsertEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = 
			MIDIEvent_SearchNextSameKindEvent (pInsertEvent->m_pPrevSameKindEvent);
	}
	if (pInsertEvent->m_pNextSameKindEvent) {
		pInsertEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent =
			MIDIEvent_SearchPrevSameKindEvent (pInsertEvent->m_pNextSameKindEvent);
	}
	/* 前後の同種イベントポインタ設定 */
	pInsertEvent->m_pPrevSameKindEvent = MIDIEvent_SearchPrevSameKindEvent (pInsertEvent);
	if (pInsertEvent->m_pPrevSameKindEvent) {
		pInsertEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = pInsertEvent;
	}
	pInsertEvent->m_pNextSameKindEvent = MIDIEvent_SearchNextSameKindEvent (pInsertEvent);
	if (pInsertEvent->m_pNextSameKindEvent) {
		pInsertEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent = pInsertEvent;
	}
	/* 親トラックのイベント数を1多くする。 */
	pInsertEvent->m_pParent = pEvent->m_pParent;
	if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_lNumEvent ++;
	}
	return 1;
}



/* 単一のクローンイベントの作成 */
/* pEventがトラックに属していても浮遊イベントとして作成します。 */
/* (この関数は内部隠蔽されています。) */
MIDIEvent* __stdcall MIDIEvent_CreateCloneSingle (MIDIEvent* pEvent) {
	MIDIEvent* pNewEvent = NULL;
	assert (pEvent);
	pNewEvent = calloc (sizeof (MIDIEvent), 1);
	if (pNewEvent == NULL) {
		return NULL;
	}
	pNewEvent->m_lTime = pEvent->m_lTime;
	pNewEvent->m_lKind = pEvent->m_lKind;
	if (!MIDIEvent_IsMIDIEvent (pEvent)) {
		if (pEvent->m_lLen > 0) { /* 20091024条件式追加 */
			pNewEvent->m_pData = malloc (pEvent->m_lLen);
			if (pNewEvent->m_pData == NULL) {
				free (pNewEvent);
				return NULL;
			}
			memcpy (pNewEvent->m_pData, pEvent->m_pData, pEvent->m_lLen);
		}
		else { /* 20091024条件式追加 */
			pNewEvent->m_pData = NULL;
			pNewEvent->m_lLen = 0;
		}
	}
	else {
		pNewEvent->m_pData = (unsigned char*)(&(pNewEvent->m_lData));
	}
	pNewEvent->m_lLen = pEvent->m_lLen;
	pNewEvent->m_lData = pEvent->m_lData;
	pNewEvent->m_lUser1 = pEvent->m_lUser1;
	pNewEvent->m_lUser2 = pEvent->m_lUser2;
	pNewEvent->m_lUser3 = pEvent->m_lUser3;
	pNewEvent->m_lUserFlag = pEvent->m_lUserFlag;
	pNewEvent->m_pParent = NULL;
	pNewEvent->m_pNextEvent = NULL;
	pNewEvent->m_pPrevEvent = NULL;
	pNewEvent->m_pNextSameKindEvent = NULL;
	pNewEvent->m_pPrevSameKindEvent = NULL;
	pNewEvent->m_pNextCombinedEvent = NULL;
	pNewEvent->m_pPrevCombinedEvent = NULL;
	return pNewEvent;
}


/*****************************************************************************/
/*                                                                           */
/*　MIDIEventクラス関数                                                      */
/*                                                                           */
/*****************************************************************************/

/* イベントを結合する */
long __stdcall MIDIEvent_Combine (MIDIEvent* pEvent) {
	/* ノート化：ノートオン+ノートオフ */
	/* パッチチェンジ化：CC#0+CC#32+PC */
	/* RPNチェンジ化：CC#99+CC#98+CC#6 */
	/* NRPNチェンジ化：CC#101+CC#100+CC#6 */
	assert (pEvent);
	/* 既に結合されてる場合は異常終了 */
	if (MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	/* 次の(a)と(b)は同一ループ内では混用しないでください。 */
	/* 次の(a)か(b)によっていったん結合したら、chopしない限りそれ以上は結合できません。 */
	/* ノートオンイベントにノートオフイベントを結合(a) */
	if (MIDIEvent_IsNoteOn (pEvent)) {
		MIDIEvent* pNoteOff = pEvent;
		while ((pNoteOff = pNoteOff->m_pNextEvent)) {
			if (MIDIEvent_IsNoteOff (pNoteOff) && !MIDIEvent_IsCombined (pNoteOff)) {
				if (MIDIEvent_GetKey (pNoteOff) == MIDIEvent_GetKey (pEvent) &&
					MIDIEvent_GetChannel (pNoteOff) == MIDIEvent_GetChannel (pEvent)) {
					pEvent->m_pNextCombinedEvent = pNoteOff;
					pNoteOff->m_pPrevCombinedEvent = pEvent;
					return 1;
				}
			}
		}
		/* このノートオンに対応するノートオフが見つからなかった。 */
		return 0;
	}
	/* ノートオフイベントにノートオンイベントを結合(b) */
	else if (MIDIEvent_IsNoteOff (pEvent)) {
		MIDIEvent* pNoteOn = pEvent;
		while ((pNoteOn = pNoteOn->m_pPrevEvent)) {
			if (MIDIEvent_IsNoteOn (pNoteOn) && !MIDIEvent_IsCombined (pNoteOn)) {
				if (MIDIEvent_GetKey (pNoteOn) == MIDIEvent_GetKey (pEvent) &&
					MIDIEvent_GetChannel (pNoteOn) == MIDIEvent_GetChannel (pEvent)) {
					pEvent->m_pPrevCombinedEvent = pNoteOn;
					pNoteOn->m_pNextCombinedEvent = pEvent;
					return 1;
				}
			}
		}
		/* このノートオフに対応するノートオンが見つからなかった。 */
		return 0;
	}
	/* CC#6にCC#101とCC#100又はCC#99とCC#98を結合 */
	else if (MIDIEvent_IsControlChange (pEvent)) {
		/* CC#6(データエントリー)に限り探索 */
		if (MIDIEvent_GetNumber (pEvent) == 6) {
			MIDIEvent* p = pEvent;
			MIDIEvent* pCC100 = NULL;
			MIDIEvent* pCC101 = NULL;
			MIDIEvent* pCC98 = NULL;
			MIDIEvent* pCC99 = NULL;
			/* RPNバンク探索 */
			while ((p = p->m_pPrevEvent)) {
				if (MIDIEvent_GetTime (p) != MIDIEvent_GetTime (pEvent)) {
					break;
				}
				else if (MIDIEvent_IsControlChange (p)) {
					if (MIDIEvent_GetNumber (p) == 6) {
						break;
					}
				}
				if (MIDIEvent_IsControlChange (p) && !MIDIEvent_IsCombined (pEvent)) {
					if (MIDIEvent_GetNumber (p) == 100 && pCC100 == NULL &&
						MIDIEvent_GetChannel (p) == MIDIEvent_GetChannel (pEvent)) {
						pCC100 = p;
					}
					else if (MIDIEvent_GetNumber (p) == 101 && pCC101 == NULL && 
						MIDIEvent_GetChannel (p) == MIDIEvent_GetChannel (pEvent)) {
						pCC101 = p;
					}
				}
			}
			if (pCC100 && pCC101) {
				pCC101->m_pPrevCombinedEvent = NULL;
				pCC101->m_pNextCombinedEvent = pCC100;
				pCC100->m_pPrevCombinedEvent = pCC101;
				pCC100->m_pNextCombinedEvent = pEvent;
				pEvent->m_pPrevCombinedEvent = pCC101;
				pEvent->m_pNextCombinedEvent = NULL;
				return 2;
			}
			/* NRPNバンク探索 */
			p = pEvent;
			while ((p = p->m_pPrevEvent)) {
				if (MIDIEvent_GetTime (p) != MIDIEvent_GetTime (pEvent)) {
					break;
				}
				else if (MIDIEvent_IsControlChange (p)) {
					if (MIDIEvent_GetNumber (p) == 6) {
						break;
					}
				}
				
				if (MIDIEvent_IsControlChange (p) && !MIDIEvent_IsCombined (pEvent)) {
					if (MIDIEvent_GetNumber (p) == 98 && pCC98 == NULL &&
						MIDIEvent_GetChannel (p) == MIDIEvent_GetChannel (pEvent)) {
						pCC98 = p;
					}
					else if (MIDIEvent_GetNumber (p) == 99 && pCC99 == NULL &&
						MIDIEvent_GetChannel (p) == MIDIEvent_GetChannel (pEvent)) {
						pCC99 = p;
					}
				}
			}
			if (pCC98 && pCC99) {
				pCC99->m_pPrevCombinedEvent = NULL;
				pCC99->m_pNextCombinedEvent = pCC98;
				pCC98->m_pPrevCombinedEvent = pCC99;
				pCC98->m_pNextCombinedEvent = pEvent;
				pEvent->m_pPrevCombinedEvent = pCC98;
				pEvent->m_pNextCombinedEvent = NULL;
				return 2;
			}
			/* このCC#6に対する{CC#101とCC#100}又は{CC#99とCC#98}が見つからなかった。 */
			return 0;
		}
		return 0;
	}
	/* プログラムチェンジにCC#0とCC#32を結合 */
	else if (MIDIEvent_IsProgramChange (pEvent)) {
		MIDIEvent* p = pEvent;
		MIDIEvent* pCC0 = NULL;
		MIDIEvent* pCC32 = NULL;
		while ((p = p->m_pPrevEvent)) {
			if (MIDIEvent_GetTime (p) != MIDIEvent_GetTime (pEvent)) {
				break;
			}
			else if (MIDIEvent_IsProgramChange (p)) {
				break;
			}
			if (MIDIEvent_IsControlChange (p) && !MIDIEvent_IsCombined (pEvent)) {
				if (MIDIEvent_GetNumber (p) == 0 && pCC0 == NULL &&
					MIDIEvent_GetChannel (p) == MIDIEvent_GetChannel (pEvent)) {
					pCC0 = p;
				}
				else if (MIDIEvent_GetNumber (p) == 32 && pCC32 == NULL &&
					MIDIEvent_GetChannel (p) == MIDIEvent_GetChannel (pEvent)) {
					pCC32 = p;
				}
			}
		}
		if (pCC0 && pCC32) {
			pCC0->m_pPrevCombinedEvent = NULL;
			pCC0->m_pNextCombinedEvent = pCC32;
			pCC32->m_pPrevCombinedEvent = pCC0;
			pCC32->m_pNextCombinedEvent = pEvent;
			pEvent->m_pPrevCombinedEvent = pCC32;
			pEvent->m_pNextCombinedEvent = NULL;
			return 1;
		}
		/* このプログラムチェンジに対するCC#0とCC#32が見つからなかった。 */
		return 0;
	}
	/* 結合できるイベントではなかった。 */
	return 0;
}

/* 結合イベントを切り離す */
long __stdcall MIDIEvent_Chop (MIDIEvent* pEvent) {
	long lCount = 0;
	MIDIEvent* pTempEvent = NULL;
	MIDIEvent* pExplodeEvent = NULL;
	/* 結合イベントでない場合は異常終了 */
	if (!MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	/* 最初の結合から順番に切り離す */
	pExplodeEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	while (pExplodeEvent) {
		pTempEvent = pExplodeEvent->m_pNextCombinedEvent;
		pExplodeEvent->m_pPrevCombinedEvent = NULL;
		pExplodeEvent->m_pNextCombinedEvent = NULL;
		pExplodeEvent = pTempEvent;
		lCount++;
	}
	return lCount;
}

/* MIDIイベントの削除(結合している場合でも単一のMIDIイベントを削除) */
long __stdcall MIDIEvent_DeleteSingle (MIDIEvent* pEvent) {
	if (pEvent == NULL) {
		return 0;
	}
	/* データ部の解放 */
	if (pEvent->m_pData != NULL && pEvent->m_pData != (unsigned char*)(&(pEvent->m_lData))) {
		free (pEvent->m_pData);
		pEvent->m_pData = NULL;
		pEvent->m_lLen = 0;
	}
	/* 結合イベントの切り離し */
	if (pEvent->m_pNextCombinedEvent) {
		pEvent->m_pNextCombinedEvent->m_pPrevCombinedEvent = pEvent->m_pPrevCombinedEvent;
	}
	if (pEvent->m_pPrevCombinedEvent) {
		pEvent->m_pPrevCombinedEvent->m_pNextCombinedEvent = pEvent->m_pNextCombinedEvent;
	}
	/* 前後接続ポインタのつなぎ替え */
	if (pEvent->m_pNextEvent) {
		pEvent->m_pNextEvent->m_pPrevEvent = pEvent->m_pPrevEvent;
	}
	else if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_pLastEvent = pEvent->m_pPrevEvent;
	}
	if (pEvent->m_pPrevEvent) {
		pEvent->m_pPrevEvent->m_pNextEvent = pEvent->m_pNextEvent;
	}
	else if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_pFirstEvent = pEvent->m_pNextEvent;
	}
	/* 前後同種イベント接続ポインタのつなぎ替え */
	if (pEvent->m_pNextSameKindEvent) {
		pEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent = pEvent->m_pPrevSameKindEvent;
	}
	if (pEvent->m_pPrevSameKindEvent) {
		pEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = pEvent->m_pNextSameKindEvent;
	}
	/* 前後結合イベントポインタのつなぎ替え */
	if (pEvent->m_pNextCombinedEvent) {
		pEvent->m_pNextCombinedEvent->m_pPrevCombinedEvent = pEvent->m_pPrevCombinedEvent;
	}
	if (pEvent->m_pPrevCombinedEvent) {
		pEvent->m_pPrevCombinedEvent->m_pNextCombinedEvent = pEvent->m_pNextCombinedEvent;
	}
	/* このイベントの他のイベントへの参照をすべてNULL化 */
	pEvent->m_pNextEvent = NULL;
	pEvent->m_pPrevEvent = NULL;
	pEvent->m_pNextSameKindEvent = NULL;
	pEvent->m_pPrevSameKindEvent = NULL;
	pEvent->m_pNextCombinedEvent = NULL;
	pEvent->m_pPrevCombinedEvent = NULL;
	/* 親トラックのイベント数デクリメント */
	if (pEvent->m_pParent) {
		((MIDITrack*)(pEvent->m_pParent))->m_lNumEvent--;
	}
	pEvent->m_pParent = NULL;
	/* MIDIイベントオブジェクト自体の解放 */
	free (pEvent);
	pEvent = NULL;
	return 1;
}

/* MIDIイベントの削除(結合している場合、結合しているMIDIイベントも削除) */
long __stdcall MIDIEvent_Delete (MIDIEvent* pEvent) {
	long lCount = 0;
	MIDIEvent* pDeleteEvent = pEvent;
	MIDIEvent* pTempEvent = NULL;
	assert (pEvent);
	pDeleteEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	while (pDeleteEvent) {
		pTempEvent = pDeleteEvent->m_pNextCombinedEvent;
		MIDIEvent_DeleteSingle (pDeleteEvent);
		pDeleteEvent = pTempEvent;
		lCount++;
	}
	return lCount;
}

/* 指定イベントに結合しているイベントの削除 */
/* (この関数は内部隠蔽されています) */
long __stdcall MIDIEvent_DeleteCombinedEvent (MIDIEvent* pEvent) {
	long lCount = 0;
	MIDIEvent* pDeleteEvent = NULL;
	MIDIEvent* pTempEvent = NULL;
	/* このイベントより前の結合イベントを削除 */
	pDeleteEvent = pEvent->m_pPrevCombinedEvent;
	while (pDeleteEvent) {
		pTempEvent = pDeleteEvent->m_pPrevCombinedEvent;
		MIDIEvent_DeleteSingle (pDeleteEvent);
		pDeleteEvent = pTempEvent;
		lCount++;
	}
	/* このイベントより後の結合イベントを削除 */
	pDeleteEvent = pEvent->m_pNextCombinedEvent;
	while (pDeleteEvent) {
		pTempEvent = pDeleteEvent->m_pNextCombinedEvent;
		MIDIEvent_DeleteSingle (pDeleteEvent);
		pDeleteEvent = pTempEvent;
		lCount++;
	}
	return lCount;
}


/* MIDIイベント(任意)を生成し、MIDIイベントへのポインタを返す(失敗時NULL) */
/* lTime:挿入時刻[tick], lKind:イベントの種類 */
/* pData:初期データ部へのポインタ, lLen:データ部の長さ[バイト](以下同様) */
/* この関数はランニングステータスをサポートしています */
/* pDataの最初1バイトが欠落していてもlKindを基にpDataを復元します。 */
MIDIEvent* __stdcall MIDIEvent_Create
(long lTime, long lKind, unsigned char* pData, long lLen) {
	MIDIEvent* pEvent;
	/* 引数の正当性チェック */
	if (lTime < 0) {
		return NULL;
	}
	if (lKind < 0 || lKind >= 256) {
		return NULL;
	}
	if (lLen < 0) {
		return NULL;
	}
	if (pData == NULL && lLen != 0) {
		return NULL;
	}
	/* MIDIチャンネルイベントは3バイト以下でなければならない */	
	if (0x80 <= lKind && lKind <= 0xEF && lLen >= 4) {
		return NULL;
	}
	/* MIDIイベントオブジェクト用バッファの確保 */
	pEvent = calloc (sizeof (MIDIEvent), 1);
	if (pEvent == NULL) {
		return NULL;
	}
	/* タイムと種類の設定 */
	pEvent->m_lTime = lTime;
	pEvent->m_lKind = lKind;
	/* pDataにランニングステータスが含まれていない場合の措置 */
	if (((0x80 <= lKind && lKind <= 0xEF) && (0 <= *pData && *pData <= 127)) ||
		((lKind == 0xF0) && (0 <= *pData && *pData <= 127))) {
		lLen ++;
	}
	pEvent->m_lLen = lLen;
	/* MIDIチャンネルイベントの場合は、付属の4バイトのデータバッファを使う。 */
	if (0x80 <= lKind && lKind <= 0xEF) {
		pEvent->m_pData = (unsigned char*)(&(pEvent->m_lData));
	}
	/* SysExかメタイベントの場合は、外部にデータバッファを確保する。 */
	else {
		if (lLen > 0) {  /* 20091024条件式追加 */
			pEvent->m_pData = malloc (lLen);
			if (pEvent->m_pData == NULL) {
				free (pEvent);
				return NULL;
			}
		}
		else {  /* 20091024条件式追加 */
			pEvent->m_pData = NULL;
			pEvent->m_lLen = 0;
		}
	}
	/* 接続ポインタの初期化 */
	pEvent->m_pNextEvent = NULL;
	pEvent->m_pPrevEvent = NULL;
	pEvent->m_pNextSameKindEvent = NULL;
	pEvent->m_pPrevSameKindEvent = NULL;
	pEvent->m_pNextCombinedEvent = NULL;
	pEvent->m_pPrevCombinedEvent = NULL;
	pEvent->m_pParent = NULL;
	pEvent->m_lUser1 = 0;
	pEvent->m_lUser2 = 0;
	pEvent->m_lUser3 = 0;
	pEvent->m_lUserFlag = 0;
	/* データ部の設定 */
	if (pData) {
		/* pDataにランニングステータスが含まれていない場合の措置 */
		if (((0x80 <= lKind && lKind <= 0xEF) && (0 <= *pData && *pData <= 127)) ||
			((lKind == 0xF0) && (0 <= *pData && *pData <= 127))) {
			if (pEvent->m_pData != NULL) {
				memcpy (pEvent->m_pData, &lKind, 1);
			}
			if (pEvent->m_pData != NULL && lLen - 1 > 0) { /* 20091024条件式追加 */
				memcpy (pEvent->m_pData + 1, pData, lLen - 1);
			}
		}
		/* 通常の場合 */
		else {
			/* MIDIチャンネルイベントのイベントの種類のチャンネル情報は、データ部に合わせる */
			if (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xEF) {
				pEvent->m_lKind &= 0xF0;
				pEvent->m_lKind |= (*(pData) & 0x0F);
			}
			if (pEvent->m_pData != NULL && lLen > 0) { /* 20091024条件式追加 */
				memcpy (pEvent->m_pData, pData, lLen);
			}
		}
	}
	return pEvent;

}


/* クローンイベントの作成 */
/* pEventが結合イベントの場合、全く同じ結合イベントを作成する。 */
MIDIEvent* __stdcall MIDIEvent_CreateClone (MIDIEvent* pEvent) {
	long i = 0;
	long lPosition = 0;	
	MIDIEvent* pNewEvent = NULL;
	MIDIEvent* pSourceEvent = NULL;
	MIDIEvent* pPrevEvent = NULL;
	assert (pEvent);

	/* 結合イベントの場合最初のイベントを取得 */
	pSourceEvent = pEvent;
	while (pSourceEvent->m_pPrevCombinedEvent) {
		pSourceEvent = pSourceEvent->m_pPrevCombinedEvent;
		lPosition++;
	}
	/* 最初のイベントから順にひとつづつクローンを作成 */
	while (pSourceEvent) {
		pNewEvent = MIDIEvent_CreateCloneSingle (pSourceEvent);
		if (pNewEvent == NULL) {
			if (pPrevEvent) {
				MIDIEvent* pDeleteEvent = MIDIEvent_GetFirstCombinedEvent (pPrevEvent);
				MIDIEvent_Delete (pDeleteEvent);
			}
			return NULL;
		}
		/* 結合イベントポインタの処理 */
		if (pPrevEvent) {
			pPrevEvent->m_pNextCombinedEvent = pNewEvent;
		}
		pNewEvent->m_pPrevCombinedEvent = pPrevEvent;
		pNewEvent->m_pNextCombinedEvent = NULL;
		/* 次のイベントへ進める */
		pSourceEvent = pSourceEvent->m_pNextCombinedEvent;
		pPrevEvent = pNewEvent;
	}
	/* 戻り値は新しく作成した結合イベントのpEventに対応するイベント(20081124変更) */
	pNewEvent = MIDIEvent_GetFirstCombinedEvent (pNewEvent);
	for (i = 0; i < lPosition; i++) {
		pNewEvent = pNewEvent->m_pNextCombinedEvent;
	}
	return pNewEvent;
}

/* シーケンスナンバーイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateSequenceNumber
(long lTime, long lNumber) {
	unsigned char c[2];
	c[0] = (unsigned char)((CLIP (0, lNumber, 65535) & 0xFF00) >> 8);
	c[1] = (unsigned char)(CLIP (0, lNumber, 65535) & 0x00FF);
	return MIDIEvent_Create (lTime, MIDIEVENT_SEQUENCENUMBER, 
		(unsigned char*)c, 2);
}

/* テキストベースのイベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateTextBasedEventA
(long lTime, long lKind, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA
		(lTime, lKind, MIDIEVENT_NOCHARCODE, pszText);
}

/* テキストベースのイベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateTextBasedEventW
(long lTime, long lKind, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW
		(lTime, lKind, MIDIEVENT_NOCHARCODE, pszText);
}

/* テキストベースのイベントの生成(文字コード指定)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateTextBasedEventExA
(long lTime, long lKind, long lCharCode, const char* pszText) {
	long lRet = 0;
	MIDIEvent* pEvent = NULL;
	if (lKind <= 0x00 || lKind >= 0x1F) {
		return NULL;
	}
	pEvent = MIDIEvent_Create (lTime, lKind, "", 0);
	if (pEvent == NULL) {
		return NULL;
	}
	/* 20191230修正 */
	lRet = MIDIEvent_SetTextA (pEvent, pszText);
	if (lRet == 0) {
		MIDIEvent_Delete (pEvent);
		return NULL;
	}
	/* 20191230修正 */
	if (lCharCode != MIDIEVENT_NOCHARCODE) {
		lRet = MIDIEvent_SetCharCode (pEvent, lCharCode);
		if (lRet == 0) {
			MIDIEvent_Delete (pEvent);
			return NULL;
		}
	}
	return pEvent;
}

/* テキストベースのイベントの生成(文字コード指定)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateTextBasedEventExW
(long lTime, long lKind, long lCharCode, const wchar_t* pszText) {
	long lRet = 0;
	MIDIEvent* pEvent = NULL;
	if (lKind <= 0x00 || lKind >= 0x1F) {
		return NULL;
	}
	pEvent = MIDIEvent_Create (lTime, lKind, "", 0);
	if (pEvent == NULL) {
		return NULL;
	}
	/* 20191230修正 */
	lRet = MIDIEvent_SetTextW (pEvent, pszText);
	if (lRet == 0) {
		MIDIEvent_Delete (pEvent);
		return NULL;
	}
	/* 20191230修正 */
	if (lCharCode != MIDIEVENT_NOCHARCODE) {
		lRet = MIDIEvent_SetCharCode (pEvent, lCharCode);
		if (lRet == 0) {
			MIDIEvent_Delete (pEvent);
			return NULL;
		}
	}
	return pEvent;
}

/* テキストイベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateTextEventA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_TEXTEVENT, MIDIEVENT_NOCHARCODE, pszText);
}

/* テキストイベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateTextEventW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_TEXTEVENT, MIDIEVENT_NOCHARCODE, pszText);
}

/* テキストイベントの生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateTextEventExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_TEXTEVENT, lCharCode, pszText);
}

/* テキストイベントの生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateTextEventExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_TEXTEVENT, lCharCode, pszText);
}

/* 著作権イベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateCopyrightNoticeA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_COPYRIGHTNOTICE, MIDIEVENT_NOCHARCODE, pszText);
}

/* 著作権イベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateCopyrightNoticeW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_COPYRIGHTNOTICE, MIDIEVENT_NOCHARCODE, pszText);
}

/* 著作権イベントの生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateCopyrightNoticeExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_COPYRIGHTNOTICE, lCharCode, pszText);
}

/* 著作権イベントの生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateCopyrightNoticeExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_COPYRIGHTNOTICE, lCharCode, pszText);
}

/* トラック名イベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateTrackNameA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_TRACKNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラック名イベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateTrackNameW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_TRACKNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラック名イベントの生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateTrackNameExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_TRACKNAME, lCharCode, pszText);
}

/* トラック名イベントの生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateTrackNameExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_TRACKNAME, lCharCode, pszText);
}

/* インストゥルメントイベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateInstrumentNameA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_INSTRUMENTNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* インストゥルメントイベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateInstrumentNameW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_INSTRUMENTNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* インストゥルメントイベントの生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateInstrumentNameExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_INSTRUMENTNAME, lCharCode, pszText);
}

/* インストゥルメントイベントの生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateInstrumentNameExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_INSTRUMENTNAME, lCharCode, pszText);
}

/* 歌詞イベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateLyricA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_LYRIC, MIDIEVENT_NOCHARCODE, pszText);
}

/* 歌詞イベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateLyricW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_LYRIC, MIDIEVENT_NOCHARCODE, pszText);
}

/* 歌詞イベントの生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateLyricExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_LYRIC, lCharCode, pszText);
}

/* 歌詞イベントの生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateLyricExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_LYRIC, lCharCode, pszText);
}

/* マーカーイベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateMarkerA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_MARKER, MIDIEVENT_NOCHARCODE, pszText);
}

/* マーカーイベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateMarkerW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_MARKER, MIDIEVENT_NOCHARCODE, pszText);
}

/* マーカーイベントの生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateMarkerExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_MARKER, lCharCode, pszText);
}

/* マーカーイベントの生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateMarkerExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_MARKER, lCharCode, pszText);
}

/* キューポイントイベントの生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateCuePointA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_CUEPOINT, MIDIEVENT_NOCHARCODE, pszText);
}

/* キューポイントイベントの生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateCuePointW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_CUEPOINT, MIDIEVENT_NOCHARCODE, pszText);
}

/* キューポイントイベントの生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateCuePointExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_CUEPOINT, lCharCode, pszText);
}

/* キューポイントイベントの生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateCuePointExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_CUEPOINT, lCharCode, pszText);
}

/* プログラム名の生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateProgramNameA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_PROGRAMNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* プログラム名の生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateProgramNameW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_PROGRAMNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* プログラム名の生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateProgramNameExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_PROGRAMNAME, lCharCode, pszText);
}

/* プログラム名の生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateProgramNameExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_PROGRAMNAME, lCharCode, pszText);
}

/* デバイス名生成(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateDeviceNameA
(long lTime, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_DEVICENAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* デバイス名生成(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateDeviceNameW
(long lTime, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_DEVICENAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* デバイス名生成(文字コード指定あり)(ANSI) */
MIDIEvent* __stdcall MIDIEvent_CreateDeviceNameExA
(long lTime, long lCharCode, const char* pszText) {
	return MIDIEvent_CreateTextBasedEventExA 
		(lTime, MIDIEVENT_DEVICENAME, lCharCode, pszText);
}

/* デバイス名生成(文字コード指定あり)(UNICODE) */
MIDIEvent* __stdcall MIDIEvent_CreateDeviceNameExW
(long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDIEvent_CreateTextBasedEventExW 
		(lTime, MIDIEVENT_DEVICENAME, lCharCode, pszText);
}

/* チャンネルプレフィックスの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateChannelPrefix
(long lTime, long lCh) {
	unsigned char c[1];
	c[0] = (unsigned char)(CLIP (0, lCh, 16));
	return MIDIEvent_Create (lTime, MIDIEVENT_CHANNELPREFIX, c, 1);
}

/* ポートプレフィックスの生成 */
MIDIEvent* __stdcall MIDIEvent_CreatePortPrefix
(long lTime, long lNum) {
	unsigned char c[1];
	c[0] = (unsigned char)(CLIP (0, lNum, 255));
	return MIDIEvent_Create (lTime, MIDIEVENT_PORTPREFIX, c, 1);
}

/* エンドオブトラックイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateEndofTrack (long lTime) {
	return MIDIEvent_Create (lTime, MIDIEVENT_ENDOFTRACK, NULL, 0);
}

/* テンポイベントの生成(lTempo = 60000000/BPMとする) */
MIDIEvent* __stdcall MIDIEvent_CreateTempo (long lTime, long lTempo) {
	unsigned char c[3];
	c[0] = (unsigned char)((CLIP (MIDIEVENT_MINTEMPO, lTempo, MIDIEVENT_MAXTEMPO) & 0xFF0000) >> 16);
	c[1] = (unsigned char)((CLIP (MIDIEVENT_MINTEMPO, lTempo, MIDIEVENT_MAXTEMPO) & 0x00FF00) >> 8);
	c[2] = (unsigned char)((CLIP (MIDIEVENT_MINTEMPO, lTempo, MIDIEVENT_MAXTEMPO) & 0x0000FF) >> 0);
	return MIDIEvent_Create (lTime, MIDIEVENT_TEMPO, c, 3);
}

/* SMPTEオフセットイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateSMPTEOffset
(long lTime, long lMode, long lHour, long lMin, long lSec, long lFrame, long lSubFrame) {
	long lMaxFrame[4] = {23, 24, 29, 29};
	unsigned char c[5];
	c[0] = (unsigned char)(((lMode & 0x03) << 5) | (CLIP (0, lHour, 23)));
	c[1] = (unsigned char)(CLIP (0, lMin, 59));
	c[2] = (unsigned char)(CLIP (0, lSec, 59));
	c[3] = (unsigned char)(CLIP (0, lFrame, lMaxFrame[lMode & 0x03]));
	c[4] = (unsigned char)(CLIP (0, lSubFrame, 99));
	return MIDIEvent_Create (lTime, MIDIEVENT_SMPTEOFFSET, c, 5);
}

/* 拍子イベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateTimeSignature (long lTime, long lnn, long ldd, long lcc, long lbb) {
	unsigned char c[4];
	c[0] = (unsigned char)(CLIP (0, lnn, 255));
	c[1] = (unsigned char)(CLIP (0, ldd, 255));
	c[2] = (unsigned char)(CLIP (0, lcc, 255));
	c[3] = (unsigned char)(CLIP (0, lbb, 255));
	return MIDIEvent_Create (lTime, MIDIEVENT_TIMESIGNATURE, c, 4);
}

/* 調性イベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateKeySignature (long lTime, long lsf, long lmi) {
	unsigned char c[2];
	c[0] = (unsigned char)(CLIP (-7, lsf, +7));
	c[1] = (unsigned char)(CLIP (0, lmi, 1));
	return MIDIEvent_Create (lTime, MIDIEVENT_KEYSIGNATURE, c, 2);
}

/* シーケンサ独自のイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateSequencerSpecific (long lTime, char* pBuf, long lLen) {
	return MIDIEvent_Create (lTime, MIDIEVENT_SEQUENCERSPECIFIC, (unsigned char*)pBuf, lLen);
}

/* ノートオフイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateNoteOff
(long lTime, long lCh, long lKey, long lVel) {
	unsigned char c[3];
	c[0] = (unsigned char)(MIDIEVENT_NOTEOFF | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lKey, 127));
	c[2] = (unsigned char)(CLIP (0, lVel, 127));
	return MIDIEvent_Create (lTime, MIDIEVENT_NOTEOFF | (lCh & 0x0F), c, 3);
}

/* ノートオンイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateNoteOn
(long lTime, long lCh, long lKey, long lVel) {
	unsigned char c[3];
	c[0] = (unsigned char)(MIDIEVENT_NOTEON | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lKey, 127));
	c[2] = (unsigned char)(CLIP (0, lVel, 127));
	return MIDIEvent_Create (lTime, MIDIEVENT_NOTEON | (lCh & 0x0F), c, 3);
}

/* ノートイベントの生成 */
/* (ノートオン(0x9n)・ノートオフ(0x8n)の2イベントを生成し、NoteOnへのポインタを返す) */
MIDIEvent* __stdcall MIDIEvent_CreateNoteOnNoteOff
(long lTime, long lCh, long lKey, long lVel1, long lVel2, long lDur) {
	unsigned char c[3];
	MIDIEvent* pNoteOnEvent;
	MIDIEvent* pNoteOffEvent;
	/* ノートオン(0x9n)イベントの生成 */
	c[0] = (unsigned char)(MIDIEVENT_NOTEON | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lKey, 127));
	c[2] = (unsigned char)(CLIP (1, lVel1, 127));
	pNoteOnEvent = MIDIEvent_Create (lTime, MIDIEVENT_NOTEON | (lCh & 0x0F), c, 3);
	if (pNoteOnEvent == NULL) {
		return NULL;
	}
	/* ノートオフ(0x8n)イベントの生成 */
	c[0] = (unsigned char)(MIDIEVENT_NOTEOFF | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lKey, 127));
	c[2] = (unsigned char)(CLIP (1, lVel2, 127));
	pNoteOffEvent = MIDIEvent_Create (lTime + lDur, MIDIEVENT_NOTEOFF | (lCh & 0x0F), c, 3);
	if (pNoteOffEvent == NULL) {
		MIDIEvent_DeleteSingle (pNoteOnEvent);
		return NULL;
	}
	/* 上の2イベントの結合 */
	pNoteOnEvent->m_pPrevCombinedEvent = NULL;
	pNoteOnEvent->m_pNextCombinedEvent = pNoteOffEvent;
	pNoteOffEvent->m_pPrevCombinedEvent = pNoteOnEvent;
	pNoteOffEvent->m_pNextCombinedEvent = NULL;
	return pNoteOnEvent;
}

/* ノートイベントの生成 */
/* (ノートオン(0x9n)・ノートオン(0x9n(vel==0))の2イベントを生成し、NoteOnへのポインタを返す) */
MIDIEvent* __stdcall MIDIEvent_CreateNoteOnNoteOn0
(long lTime, long lCh, long lKey, long lVel, long lDur) {
	unsigned char c[3];
	MIDIEvent* pNoteOnEvent;
	MIDIEvent* pNoteOffEvent;
	/* ノートオン(0x9n)イベントの生成 */
	c[0] = (unsigned char)(MIDIEVENT_NOTEON | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lKey, 127));
	c[2] = (unsigned char)(CLIP (1, lVel, 127));
	pNoteOnEvent = MIDIEvent_Create (lTime, MIDIEVENT_NOTEON | (lCh & 0x0F), c, 3);
	if (pNoteOnEvent == NULL) {
		return NULL;
	}
	/* ノートオン(0x9n, vel==0)イベントの生成 */
	c[0] = (unsigned char)(MIDIEVENT_NOTEON | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lKey, 127));
	c[2] = (unsigned char)0;
	pNoteOffEvent = MIDIEvent_Create (lTime + lDur, MIDIEVENT_NOTEON | (lCh & 0x0F), c, 3);
	if (pNoteOffEvent == NULL) {
		MIDIEvent_DeleteSingle (pNoteOnEvent);
		return NULL;
	}
	/* 上の2イベントの結合 */
	pNoteOnEvent->m_pPrevCombinedEvent = NULL;
	pNoteOnEvent->m_pNextCombinedEvent = pNoteOffEvent;
	pNoteOffEvent->m_pPrevCombinedEvent = pNoteOnEvent;
	pNoteOffEvent->m_pNextCombinedEvent = NULL;
	return pNoteOnEvent;
}

/* ノートイベントの生成 */
/* (ノートオン・ノート消音の2イベントを生成し、NoteOnへのポインタを返す) */
MIDIEvent* __stdcall MIDIEvent_CreateNote
	(long lTime, long lCh, long lKey, long lVel, long lDur) {
	return MIDIEvent_CreateNoteOnNoteOn0 (lTime, lCh, lKey, lVel, lDur);
}

/* キーアフターイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateKeyAftertouch 
(long lTime, long lCh, long lKey, long lVal) {
	unsigned char c[3];
	c[0] = (unsigned char)(MIDIEVENT_KEYAFTERTOUCH | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lKey, 127));
	c[2] = (unsigned char)(CLIP (0, lVal, 127));
	return MIDIEvent_Create (lTime, MIDIEVENT_KEYAFTERTOUCH | c[0], c, 3);
}

/* コントローラーイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateControlChange
(long lTime, long lCh, long lNum, long lVal) {
	unsigned char c[3];
	c[0] = (unsigned char)(MIDIEVENT_CONTROLCHANGE | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lNum, 127));
	c[2] = (unsigned char)(CLIP (0, lVal, 127));
	return MIDIEvent_Create (lTime, MIDIEVENT_CONTROLCHANGE | c[0], c, 3);
}

/* RPNイベントの生成 */
/* (CC#101+CC#100+CC#6の3イベントを生成し、CC#101へのポインタを返す) */
MIDIEvent* __stdcall MIDIEvent_CreateRPNChange
(long lTime, long lCh, long lCC101, long lCC100, long lVal) {
	MIDIEvent* pCC101Event;
	MIDIEvent* pCC100Event;
	MIDIEvent* pCC6Event;
	/* CC#101イベントの生成 */
	pCC101Event = MIDIEvent_CreateControlChange (lTime, lCh, 101, lCC101);
	if (pCC101Event == NULL) {
		return NULL;
	}
	/* CC#100イベントの生成 */
	pCC100Event = MIDIEvent_CreateControlChange (lTime, lCh, 100, lCC100);
	if (pCC100Event == NULL) {
		MIDIEvent_DeleteSingle (pCC101Event);
		return NULL;
	}
	/* CC#6(データエントリー)イベントの生成 */
	pCC6Event = MIDIEvent_CreateControlChange (lTime, lCh, 6, lVal);
	if (pCC6Event == NULL) {
		MIDIEvent_DeleteSingle (pCC101Event);
		MIDIEvent_DeleteSingle (pCC100Event);
		return NULL;
	}
	/* 上の3イベントの結合 */
	pCC101Event->m_pPrevCombinedEvent = NULL;
	pCC101Event->m_pNextCombinedEvent = pCC100Event;
	pCC100Event->m_pPrevCombinedEvent = pCC101Event;
	pCC100Event->m_pNextCombinedEvent = pCC6Event;
	pCC6Event->m_pPrevCombinedEvent = pCC100Event;
	pCC6Event->m_pNextCombinedEvent = NULL;
	return pCC101Event;
}

/* NRPNイベントの生成 */
/* (CC#99+CC#98+CC#6の3イベントを生成し、CC#99へのポインタを返す) */
MIDIEvent* __stdcall MIDIEvent_CreateNRPNChange 
(long lTime, long lCh, long lCC99, long lCC98, long lVal) {
	MIDIEvent* pCC99Event;
	MIDIEvent* pCC98Event;
	MIDIEvent* pCC6Event;
	/* CC#99イベントの生成 */
	pCC99Event = MIDIEvent_CreateControlChange (lTime, lCh, 99, lCC99);
	if (pCC99Event == NULL) {
		return NULL;
	}
	/* CC#98イベントの生成 */
	pCC98Event = MIDIEvent_CreateControlChange (lTime, lCh, 98, lCC98);
	if (pCC98Event == NULL) {
		MIDIEvent_DeleteSingle (pCC99Event);
		return NULL;
	}
	/* CC#6(データエントリー)イベントの生成 */
	pCC6Event = MIDIEvent_CreateControlChange (lTime, lCh, 6, lVal);
	if (pCC6Event == NULL) {
		MIDIEvent_DeleteSingle (pCC99Event);
		MIDIEvent_DeleteSingle (pCC98Event);
		return NULL;
	}
	/* 上の3イベントの結合 */
	pCC99Event->m_pPrevCombinedEvent = NULL;
	pCC99Event->m_pNextCombinedEvent = pCC98Event;
	pCC98Event->m_pPrevCombinedEvent = pCC99Event;
	pCC98Event->m_pNextCombinedEvent = pCC6Event;
	pCC6Event->m_pPrevCombinedEvent = pCC98Event;
	pCC6Event->m_pNextCombinedEvent = NULL;
	return pCC99Event;
}

/* プログラムイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateProgramChange 
(long lTime, long lCh, long lVal) {
	unsigned char c[2];
	c[0] = (unsigned char)(MIDIEVENT_PROGRAMCHANGE | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lVal, 127));
	return MIDIEvent_Create (lTime, MIDIEVENT_PROGRAMCHANGE | c[0], c, 2);
}

/* パッチチェンジイベントの生成 */
/* (CC#0+CC#32+PCの3イベントを生成し、CC#0へのポインタを返す) */
MIDIEvent* __stdcall MIDIEvent_CreatePatchChange 
(long lTime, long lCh, long lCC0, long lCC32, long lVal) {
	MIDIEvent* pCC0Event;
	MIDIEvent* pCC32Event;
	MIDIEvent* pProgramEvent;
	/* CC#0イベントの生成 */
	pCC0Event = MIDIEvent_CreateControlChange (lTime, lCh, 0, lCC0);
	if (pCC0Event == NULL) {
		return NULL;
	}
	/* CC#32イベントの生成 */
	pCC32Event = MIDIEvent_CreateControlChange (lTime, lCh, 32, lCC32);
	if (pCC32Event == NULL) {
		MIDIEvent_DeleteSingle (pCC0Event);
		return NULL;
	}
	/* プログラムチェンジイベントの生成 */
	pProgramEvent = MIDIEvent_CreateProgramChange (lTime, lCh, lVal);
	if (pProgramEvent == NULL) {
		MIDIEvent_DeleteSingle (pCC0Event);
		MIDIEvent_DeleteSingle (pCC32Event);
		return NULL;
	}
	/* 上の3イベントの結合 */
	pCC0Event->m_pPrevCombinedEvent = NULL;
	pCC0Event->m_pNextCombinedEvent = pCC32Event;
	pCC32Event->m_pPrevCombinedEvent = pCC0Event;
	pCC32Event->m_pNextCombinedEvent = pProgramEvent;
	pProgramEvent->m_pPrevCombinedEvent = pCC32Event;
	pProgramEvent->m_pNextCombinedEvent = NULL;
	return pCC0Event;
}

/* チャンネルアフターイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateChannelAftertouch 
(long lTime, long lCh, long lVal) {
	unsigned char c[2];
	c[0] = (unsigned char)(MIDIEVENT_CHANNELAFTERTOUCH | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lVal, 127));
	return MIDIEvent_Create (lTime, MIDIEVENT_CHANNELAFTERTOUCH | c[0], c, 2);
}


/* ピッチベンドイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreatePitchBend 
(long lTime, long lCh, long lVal) {
	unsigned char c[3];
	c[0] = (unsigned char)(MIDIEVENT_PITCHBEND | (lCh & 0x0F));
	c[1] = (unsigned char)(CLIP (0, lVal, 16383) & 0x7F);
	c[2] = (unsigned char)((CLIP (0, lVal, 16383) >> 7) & 0x7F);
	return MIDIEvent_Create (lTime, MIDIEVENT_PITCHBEND | c[0], c, 3);
}

/* SysExイベントの生成 */
MIDIEvent* __stdcall MIDIEvent_CreateSysExEvent
(long lTime, unsigned char* pBuf, long lLen) {
	assert (pBuf);
	if (*pBuf == 0xF0) {
		return MIDIEvent_Create (lTime, MIDIEVENT_SYSEXSTART, pBuf, lLen);
	}
	else {
		return MIDIEvent_Create (lTime, MIDIEVENT_SYSEXCONTINUE, pBuf, lLen);
	}
}





/* メタイベントであるかどうかを調べる */
/* メタイベントとは、イベントの種類が0x00～0x7Fのもの、すなわち、 */
/* シーケンス番号・テキストイベント・著作権・トラック名・ */
/* インストゥルメント名・歌詞・マーカー・キューポイント・ */
/* プログラム名・デバイス名・チャンネルプリフィックス・ポートプリフィックス・ */
/* エンドオブトラック・テンポ・SMPTEオフセット・拍子記号・調性記号・シーケンサー独自のイベント */
/* などを示す。これらは主に表記メモのためのイベントであり、演奏に影響を与えるものではない。 */
long __stdcall MIDIEvent_IsMetaEvent (MIDIEvent* pEvent) {
	return (MIDIEVENT_SEQUENCENUMBER <= pEvent->m_lKind && 
		pEvent->m_lKind <= MIDIEVENT_SEQUENCERSPECIFIC) ? 1 : 0;
}

/* シーケンス番号であるかどうかを調べる */
long __stdcall MIDIEvent_IsSequenceNumber (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_SEQUENCENUMBER) ? 1 : 0;
}

/* テキストイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsTextEvent (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_TEXTEVENT) ? 1 : 0;
}

/* 著作権イベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsCopyrightNotice (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_COPYRIGHTNOTICE) ? 1 : 0;
}

/* トラック名イベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsTrackName (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_TRACKNAME) ? 1 : 0;
}

/* インストゥルメント名であるかどうかを調べる */
long __stdcall MIDIEvent_IsInstrumentName (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_INSTRUMENTNAME) ? 1 : 0;
}

/* 歌詞イベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsLyric (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_LYRIC) ? 1 : 0;
}

/* マーカーイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsMarker (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_MARKER) ? 1 : 0;
}

/* キューポイントイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsCuePoint (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_CUEPOINT) ? 1 : 0;
}

/* プログラム名であるかどうかを調べる */
long __stdcall MIDIEvent_IsProgramName (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_PROGRAMNAME) ? 1 : 0;
}

/* デバイス名であるかどうかを調べる */
long __stdcall MIDIEvent_IsDeviceName (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_DEVICENAME) ? 1 : 0;
}

/* チャンネルプレフィックスであるかどうかを調べる */
long __stdcall MIDIEvent_IsChannelPrefix (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_CHANNELPREFIX) ? 1 : 0;
}

/* ポートプレフィックスであるかどうかを調べる */
long __stdcall MIDIEvent_IsPortPrefix (MIDIEvent* pEvent) {
	return pEvent->m_lKind == MIDIEVENT_PORTPREFIX ? 1 : 0;
}

/* エンドオブトラックであるかどうかを調べる */
long __stdcall MIDIEvent_IsEndofTrack (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == MIDIEVENT_ENDOFTRACK) ? 1 : 0;
}

/* テンポイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsTempo (MIDIEvent* pEvent) {
	return pEvent->m_lKind == MIDIEVENT_TEMPO ? 1 : 0;
}

/* SMPTEオフセットイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsSMPTEOffset (MIDIEvent* pEvent) {
	return pEvent->m_lKind == MIDIEVENT_SMPTEOFFSET ? 1 : 0;
}

/* 拍子記号イベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsTimeSignature (MIDIEvent* pEvent) {
	return pEvent->m_lKind == MIDIEVENT_TIMESIGNATURE ? 1 : 0;
}

/* 調性記号イベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsKeySignature (MIDIEvent* pEvent) {
	return pEvent->m_lKind == MIDIEVENT_KEYSIGNATURE ? 1 : 0;
}

/* シーケンサ独自のイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsSequencerSpecific (MIDIEvent* pEvent) {
	return pEvent->m_lKind == MIDIEVENT_SEQUENCERSPECIFIC ? 1 : 0;
}

/* MIDIイベントであるかどうかを調べる */
/* MIDIイベントとはノートオフ・オートオン・キーアフタータッチ・コントロールチェンジ・ */
/* プログラムチェンジ・チャンネルアフタータッチ・ピッチベンド、及びこれらの結合した */
/* 結合イベントのことを示す。 */
long __stdcall MIDIEvent_IsMIDIEvent (MIDIEvent* pEvent) {
	return (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xEF) ? 1 : 0;
}

/* NOTEイベントであるかどうかを調べる */
/* これはノートオンとノートオフが結合イベントしたイベントでなければならない。 */
long __stdcall MIDIEvent_IsNote (MIDIEvent* pEvent) {
	MIDIEvent* pNoteOnEvent = NULL;
	MIDIEvent* pNoteOffEvent = NULL;
	if (!MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	pNoteOnEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	if (pNoteOnEvent == NULL) {
		return 0;
	}
	if (!MIDIEvent_IsNoteOn (pNoteOnEvent)) {
		return 0;
	}
	pNoteOffEvent = pNoteOnEvent->m_pNextCombinedEvent;
	if (pNoteOffEvent == NULL) {
		return 0;
	}
	if (!MIDIEvent_IsNoteOff (pNoteOffEvent)) {
		return 0;
	}
	if (pNoteOffEvent->m_pNextCombinedEvent) {
		return 0;
	}
	return 1;
}

/* NOTEONOTEOFFイベントであるかどうかを調べる */
/* これはノートオン(0x9n)とノートオフ(0x8n)が結合イベントしたイベントでなければならない。 */
long __stdcall MIDIEvent_IsNoteOnNoteOff (MIDIEvent* pEvent) {
	MIDIEvent* pNoteOnEvent = NULL;
	MIDIEvent* pNoteOffEvent = NULL;
	if (!MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	pNoteOnEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	if (pNoteOnEvent == NULL) {
		return 0;
	}
	if (!MIDIEvent_IsNoteOn (pNoteOnEvent)) {
		return 0;
	}
	pNoteOffEvent = pNoteOnEvent->m_pNextCombinedEvent;
	if (pNoteOffEvent == NULL) {
		return 0;
	}
	if (!(0x80 <= pNoteOffEvent->m_lKind && pNoteOffEvent->m_lKind <= 0x8F)) {
		return 0;
	}
	if (pNoteOffEvent->m_pNextCombinedEvent) {
		return 0;
	}
	return 1;
}

/* NOTEONNOTEON0イベントであるかどうかを調べる */
/* これはノートオン(0x9n)とノートオフ(0x9n,vel==0)が結合イベントしたイベントでなければならない。 */
long __stdcall MIDIEvent_IsNoteOnNoteOn0 (MIDIEvent* pEvent) {
	MIDIEvent* pNoteOnEvent = NULL;
	MIDIEvent* pNoteOffEvent = NULL;
	if (!MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	pNoteOnEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	if (pNoteOnEvent == NULL) {
		return 0;
	}
	if (!MIDIEvent_IsNoteOn (pNoteOnEvent)) {
		return 0;
	}
	pNoteOffEvent = pNoteOnEvent->m_pNextCombinedEvent;
	if (pNoteOffEvent == NULL) {
		return 0;
	}
	if (!(0x90 <= pNoteOffEvent->m_lKind && pNoteOffEvent->m_lKind <= 0x9F)) {
		return 0;
	}
	if (*(pNoteOffEvent->m_pData + 2) != 0) {
		return 0;
	}
	if (pNoteOffEvent->m_pNextCombinedEvent) {
		return 0;
	}
	return 1;
}

/* ノートオフイベントであるかどうかを調べる */
/* (ノートオンイベントでベロシティ0のものはノートオフイベントとみなす。) */
long __stdcall MIDIEvent_IsNoteOff (MIDIEvent* pEvent) {
	if (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0x8F) {
		return 1;
	}
	if (0x90 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F) {
		if (*(pEvent->m_pData + 2)) {
			return 0;
		}
		return 1;
	}
	return 0;
}

/* ノートオンイベントであるかどうかを調べる */
/* (ノートオンイベントでもベロシティ0のものはノートオフイベントとみなす。) */
long __stdcall MIDIEvent_IsNoteOn (MIDIEvent* pEvent) {
	if (0x90 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F) {
		if (*(pEvent->m_pData + 2)) {
			return 1;
		}
		return 0;
	}
	return 0;
}

/* キーアフタータッチイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsKeyAftertouch (MIDIEvent* pEvent) {
	return (0xA0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xAF) ? 1 : 0;
}

/* コントロールチェンジイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsControlChange (MIDIEvent* pEvent) {
	return (0xB0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xBF) ? 1 : 0;
}

/* プログラムチェンジイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsProgramChange (MIDIEvent* pEvent) {
	return (0xC0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xCF) ? 1 : 0;
}

/* チャンネルアフターイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsChannelAftertouch (MIDIEvent* pEvent) {
	return (0xD0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xDF) ? 1 : 0;
}

/* ピッチベンドイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsPitchBend (MIDIEvent* pEvent) {
	return (0xE0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xEF) ? 1 : 0;
}

/* システムエクスクルーシヴイベントであるかどうかを調べる */
long __stdcall MIDIEvent_IsSysExEvent (MIDIEvent* pEvent) {
	return (pEvent->m_lKind == 0xF0 || pEvent->m_lKind == 0xF7) ? 1 : 0;
}

/* 浮遊イベントであるかどうか調べる */
long __stdcall MIDIEvent_IsFloating (MIDIEvent* pEvent) {
	return (pEvent->m_pParent == NULL ? 1 : 0);
}

/* 結合イベントであるかどうか調べる */
long __stdcall MIDIEvent_IsCombined (MIDIEvent* pEvent) {
	return (pEvent->m_pPrevCombinedEvent || pEvent->m_pNextCombinedEvent) ? 1 : 0;
}

/* RPNチェンジイベントであるかどうかを調べる */
/* CC#101とCC#100とCC#6が同時刻この順にで結合されてなければならない。 */
long __stdcall MIDIEvent_IsRPNChange (MIDIEvent* pEvent) {
	MIDIEvent* pCC101Event = NULL;
	MIDIEvent* pCC100Event = NULL;
	MIDIEvent* pCC006Event = NULL;
	assert (pEvent);
	if (!MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	pCC101Event = MIDIEvent_GetFirstCombinedEvent (pEvent);
	if (pCC101Event == NULL) {
		return 0;
	}
	if (pCC101Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC101Event)) {
		return 0;
	}
	if (*(pCC101Event->m_pData + 1) != 101) {
		return 0;
	}		
	pCC100Event = pCC101Event->m_pNextCombinedEvent;
	if (pCC100Event == NULL) {
		return 0;
	}
	if (pCC100Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC100Event)) {
		return 0;
	}
	if (*(pCC100Event->m_pData + 1) != 100) {
		return 0;
	}
	pCC006Event = pCC100Event->m_pNextCombinedEvent;
	if (pCC006Event == NULL) {
		return 0;
	}
	if (pCC006Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC006Event)) {
		return 0;
	}
	if (pCC006Event->m_pNextCombinedEvent) {
		return 0;
	}
	return 1;
}


/* NRPNチェンジイベントであるかどうかを調べる */
/* CC#99とCC#98とCC#6が同時刻この順番で結合されていなければならない。 */
long __stdcall MIDIEvent_IsNRPNChange (MIDIEvent* pEvent) {
	MIDIEvent* pCC099Event = NULL;
	MIDIEvent* pCC098Event = NULL;
	MIDIEvent* pCC006Event = NULL;
	assert (pEvent);
	if (!MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	pCC099Event = MIDIEvent_GetFirstCombinedEvent (pEvent);
	if (pCC099Event == NULL) {
		return 0;
	}
	if (pCC099Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC099Event)) {
		return 0;
	}
	if (*(pCC099Event->m_pData + 1) != 99) {
		return 0;
	}		
	pCC098Event = pCC099Event->m_pNextCombinedEvent;
	if (pCC098Event == NULL) {
		return 0;
	}
	if (pCC098Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC098Event)) {
		return 0;
	}
	if (*(pCC098Event->m_pData + 1) != 98) {
		return 0;
	}
	pCC006Event = pCC098Event->m_pNextCombinedEvent;
	if (pCC006Event == NULL) {
		return 0;
	}
	if (pCC006Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC006Event)) {
		return 0;
	}
	if (pCC006Event->m_pNextCombinedEvent) {
		return 0;
	}
	return 1;
}


/* パッチチェンジイベントであるかどうかを調べる */
/* CC#0とCC#32とプログラムチェンジが同時刻にこの順番で結合されていなければならない。 */
long __stdcall MIDIEvent_IsPatchChange (MIDIEvent* pEvent) {
	MIDIEvent* pCC000Event = NULL;
	MIDIEvent* pCC032Event = NULL;
	MIDIEvent* pPCEvent = NULL;
	assert (pEvent);
	if (!MIDIEvent_IsCombined (pEvent)) {
		return 0;
	}
	pCC000Event = MIDIEvent_GetFirstCombinedEvent (pEvent);
	if (pCC000Event == NULL) {
		return 0;
	}
	if (pCC000Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC000Event)) {
		return 0;
	}
	if (*(pCC000Event->m_pData + 1) != 0) {
		return 0;
	}		
	pCC032Event = pCC000Event->m_pNextCombinedEvent;
	if (pCC032Event == NULL) {
		return 0;
	}
	if (pCC032Event->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsControlChange (pCC032Event)) {
		return 0;
	}
	if (*(pCC032Event->m_pData + 1) != 32) {
		return 0;
	}
	pPCEvent = pCC032Event->m_pNextCombinedEvent;
	if (pPCEvent == NULL) {
		return 0;
	}
	if (pPCEvent->m_lTime != pEvent->m_lTime) {
		return 0;
	}
	if (!MIDIEvent_IsProgramChange (pPCEvent)) {
		return 0;
	}
	if (pPCEvent->m_pNextCombinedEvent) {
		return 0;
	}
	return 1;
}


/* イベントの種類を取得 */
long __stdcall MIDIEvent_GetKind (MIDIEvent* pEvent) {
	/* TODO MIDIチャンネルイベントの場合の & 0xF0 */
	return (pEvent->m_lKind);
}


long __stdcall MIDIEvent_SetKindSingle (MIDIEvent* pEvent, long lKind) {
	long lLen = 0;
	long lSrcKind;
	assert (pEvent);
	assert (0 <= lKind && lKind <= 0xFF);
	/* MIDIチャンネルイベントへの変更はチャンネル情報無視 */
	if (0x80 <= lKind && lKind <= 0xEF) {
		lKind &= 0xF0;
	}
	/* イベントの種類が変更されないときはリターン0 */
	lSrcKind = pEvent->m_lKind;
	if (0x80 <= lSrcKind && lSrcKind <= 0xEF) {
		lSrcKind &= 0xF0;
	}
	if (lSrcKind == lKind) {
		return 0;
	}
	/* バッファの長さ設定 */
	switch (lKind) {
	case MIDIEVENT_ENDOFTRACK:
		lLen = 0;
		break;
	case MIDIEVENT_CHANNELPREFIX:
	case MIDIEVENT_PORTPREFIX:
		lLen = 1;
		break;
	case MIDIEVENT_SEQUENCENUMBER:
	case MIDIEVENT_CHANNELAFTERTOUCH:
	case MIDIEVENT_PROGRAMCHANGE:
	case MIDIEVENT_KEYSIGNATURE:
	case MIDIEVENT_SYSEXSTART:
	case MIDIEVENT_SYSEXCONTINUE:
		lLen = 2;
		break;
	case MIDIEVENT_NOTEOFF:
	case MIDIEVENT_NOTEON:
	case MIDIEVENT_CONTROLCHANGE:
	case MIDIEVENT_PITCHBEND:
	case MIDIEVENT_KEYAFTERTOUCH:
	case MIDIEVENT_TEMPO:
		lLen = 3;
		break;
	case MIDIEVENT_TIMESIGNATURE:
		lLen = 4;
		break;
	case MIDIEVENT_SMPTEOFFSET:
		lLen = 5;
		break;
	default:
		lLen = pEvent->m_lLen;
		break;
	}
	/* 変更後イベントがMIDIチャンネルイベントの場合 */
	if (0x80 <= lKind && lKind <= 0xEF) {
		/* データ用メモリの解放 */
		if (pEvent->m_pData != (unsigned char*)(&(pEvent->m_lData))) {
			free (pEvent->m_pData);
			pEvent->m_pData = NULL;
			pEvent->m_lLen = 0;
		}
		pEvent->m_pData = (unsigned char*)(&(pEvent->m_lData));
		pEvent->m_lLen = lLen;
		pEvent->m_lKind = (lKind & 0xF0) | (pEvent->m_lKind & 0x0F);
		*(pEvent->m_pData) = (unsigned char)((lKind & 0xF0) | (pEvent->m_lKind & 0x0F));
	}
	/* 変更後イベントがメタイベントかシステムエクスクルーシブイベントの場合 */
	else {
		long lMode = 0;
		long lMaxFrame[4] = {23, 24, 29, 29};
		/* データ用メモリの確保 */
		/* データ部はmallocバッファを指している場合 */
		if (pEvent->m_pData != (unsigned char*)(&(pEvent->m_lData))) {
			if (lLen > 0) {
				if (pEvent->m_lLen != lLen) {
					pEvent->m_pData = realloc (pEvent->m_pData, lLen);
					if (pEvent->m_pData == NULL) {
						pEvent->m_lLen = 0;
						/* TODO:ここに安全措置を記述 */
					}
					else {
						pEvent->m_lLen = lLen;
					}
				}
			}
			/* lLen == 0の場合 */
			else {
				/* データ用メモリはいらない */
				free (pEvent->m_pData);
				pEvent->m_pData = NULL;
				pEvent->m_lLen = 0;
			}
		}
		/* データ部はlongバッファを指している場合 */
		else {
			if (lLen > 0) { /* 20091024条件式追加 */
				/* 4バイトメモリは無視し、データ用メモリを新たに確保 */
				pEvent->m_pData = malloc (lLen);
				if (pEvent->m_pData == NULL) {
					pEvent->m_lLen = 0;
					/* TODO:ここに安全措置を記述 */
				}
				else {
					pEvent->m_lLen = lLen;
				}
			}
			else {
				pEvent->m_pData = NULL;
				pEvent->m_lLen = 0;
			}
		}
		pEvent->m_lKind = lKind;
		/* データ部の正当性確保 */
		switch (lKind) {
		case MIDIEVENT_CHANNELPREFIX:
			*(pEvent->m_pData) = (unsigned char)(CLIP (0, *(pEvent->m_pData), 15));
			break;
		case MIDIEVENT_PORTPREFIX:
			*(pEvent->m_pData) = (unsigned char)(CLIP (0, *(pEvent->m_pData), 255));
			break;
		case MIDIEVENT_SMPTEOFFSET:
			*(pEvent->m_pData) =  (unsigned char)((*(pEvent->m_pData) & 0x60) | 
				CLIP (0, (*(pEvent->m_pData) & 0x1F), 23));
			lMode = (*(pEvent->m_pData) >> 5) & 0x03;
			*(pEvent->m_pData + 1) = (unsigned char)CLIP (0, *(pEvent->m_pData + 1), 59);
			*(pEvent->m_pData + 2) = (unsigned char)CLIP (0, *(pEvent->m_pData + 2), 59);
			*(pEvent->m_pData + 3) = (unsigned char)CLIP (0, *(pEvent->m_pData + 3), lMaxFrame[lMode]);
			*(pEvent->m_pData + 4) = (unsigned char)CLIP (0, *(pEvent->m_pData + 4), 99);
			break;
		case MIDIEVENT_TIMESIGNATURE:
			*(pEvent->m_pData + 0) = (unsigned char)4;
			*(pEvent->m_pData + 1) = (unsigned char)2;
			*(pEvent->m_pData + 2) = (unsigned char)24;
			*(pEvent->m_pData + 3) = (unsigned char)8;
			break;
		case MIDIEVENT_KEYSIGNATURE:
			*(pEvent->m_pData + 0) = (unsigned char)CLIP (-7, *(pEvent->m_pData), +7);
			*(pEvent->m_pData + 1) = (unsigned char)CLIP (0, *(pEvent->m_pData + 1), 1);
			break;
		case MIDIEVENT_SYSEXSTART:
		case MIDIEVENT_SYSEXCONTINUE:
			*(pEvent->m_pData + 0) = (unsigned char)0xF0;
			*(pEvent->m_pData + 1) = (unsigned char)0xF7;
			break;
		}
	}
	/* 前後の同種イベントのポインタのつなぎ替え */
	if (pEvent->m_pPrevSameKindEvent) {
		pEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = 
			MIDIEvent_SearchNextSameKindEvent (pEvent->m_pPrevSameKindEvent);
	}
	if (pEvent->m_pNextSameKindEvent) {
		pEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent =
			MIDIEvent_SearchPrevSameKindEvent (pEvent->m_pNextSameKindEvent);
	}
	/* 前後の同種イベントポインタ設定 */
	pEvent->m_pPrevSameKindEvent = MIDIEvent_SearchPrevSameKindEvent (pEvent);
	if (pEvent->m_pPrevSameKindEvent) {
		pEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = pEvent;
	}
	pEvent->m_pNextSameKindEvent = MIDIEvent_SearchNextSameKindEvent (pEvent);
	if (pEvent->m_pNextSameKindEvent) {
		pEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent = pEvent;
	}
	return 1;
}


/* イベントの種類を設定 */
/* TODO:この関数はまだm_pNextSameKindEventとm_pPrevSameKindの設定がなされていない */
long __stdcall MIDIEvent_SetKind (MIDIEvent* pEvent, long lKind) {
	assert (pEvent);
	/* MIDIチャンネルイベントの場合、チャンネル情報は無視 */
	if (0x0080 <= (lKind & 0x00FF) && (lKind & 0x00FF) <= 0x00EF) {
		lKind &= 0xFFFFFFF0;
	}
	/* イベントの種類に変化がない場合は脱出 */
	if (MIDIEvent_IsCombined (pEvent)) {
		if ((MIDIEvent_IsNoteOnNoteOff (pEvent) && lKind == MIDIEVENT_NOTEONNOTEOFF) ||
			(MIDIEvent_IsNoteOnNoteOn0 (pEvent) && lKind == MIDIEVENT_NOTEONNOTEON0)) {
			return 0;
		}
		else if ((MIDIEvent_IsPatchChange (pEvent) && lKind == MIDIEVENT_PATCHCHANGE) ||
				(MIDIEvent_IsRPNChange (pEvent) && lKind == MIDIEVENT_RPNCHANGE) ||
				(MIDIEvent_IsNRPNChange (pEvent) && lKind == MIDIEVENT_NRPNCHANGE)) {
			return 0;
		}
	}
	/* 2→2 */
	if ((MIDIEvent_IsNoteOnNoteOff (pEvent) && lKind == MIDIEVENT_NOTEONNOTEON0) ||
		(MIDIEvent_IsNoteOnNoteOn0 (pEvent) && lKind == MIDIEVENT_NOTEONNOTEOFF)) {
		MIDIEvent* pEvent1 = MIDIEvent_GetFirstCombinedEvent (pEvent);
		MIDIEvent* pEvent2 = pEvent1->m_pNextCombinedEvent;
		assert (MIDIEvent_GetChannel (pEvent1) == MIDIEvent_GetChannel (pEvent2));
		if (lKind == MIDIEVENT_NOTEONNOTEOFF) {
			pEvent2->m_lKind = (unsigned char)
				(MIDIEVENT_NOTEOFF | ((pEvent2->m_lKind) & 0x0F));
			*(pEvent2->m_pData + 0) = (unsigned char)
				(MIDIEVENT_NOTEOFF | ((pEvent2->m_lKind) & 0x0F));
			*(pEvent2->m_pData + 2) = (unsigned char)
				(*(pEvent1->m_pData + 2));
		}
		else if (lKind == MIDIEVENT_NOTEONNOTEON0) {
			pEvent2->m_lKind = (unsigned char)
				(MIDIEVENT_NOTEON | ((pEvent2->m_lKind) & 0x0F));
			*(pEvent2->m_pData + 0) = (unsigned char)
				(MIDIEVENT_NOTEON | ((pEvent2->m_lKind) & 0x0F));
			*(pEvent2->m_pData + 2) = (unsigned char)(0);
		}
	}
	/* 3→3 */
	else if ((MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent) || 
		MIDIEvent_IsPatchChange (pEvent)) && (lKind == MIDIEVENT_RPNCHANGE || 
		lKind == MIDIEVENT_NRPNCHANGE || lKind == MIDIEVENT_PATCHCHANGE)) {
		MIDIEvent* pEvent1 = MIDIEvent_GetFirstCombinedEvent (pEvent);
		MIDIEvent* pEvent2 = pEvent1->m_pNextCombinedEvent;
		MIDIEvent* pEvent3 = pEvent2->m_pNextCombinedEvent;
		if (lKind == MIDIEVENT_RPNCHANGE) {
			*(pEvent1->m_pData + 1) = 101;
			*(pEvent1->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent1->m_pData + 2), 127);
			*(pEvent2->m_pData + 1) = 100;
			*(pEvent2->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent2->m_pData + 2), 127);
			MIDIEvent_SetKindSingle (pEvent3, 0xB0 | (pEvent->m_lKind & 0x0F));
			*(pEvent3->m_pData + 1) = (unsigned char)6;
			*(pEvent3->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent3->m_pData + 2), 127);
		}
		else if (lKind == MIDIEVENT_NRPNCHANGE) {
			*(pEvent1->m_pData + 1) = 99;
			*(pEvent1->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent1->m_pData + 2), 127);
			*(pEvent2->m_pData + 1) = 98;
			*(pEvent2->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent2->m_pData + 2), 127);
			MIDIEvent_SetKindSingle (pEvent3, 0xB0 | (pEvent->m_lKind & 0x0F));
			*(pEvent3->m_pData + 1) = (unsigned char)6;
			*(pEvent3->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent3->m_pData + 2), 127);
		}
		else if (lKind == MIDIEVENT_PATCHCHANGE) {
			*(pEvent1->m_pData + 1) = 0;
			*(pEvent1->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent1->m_pData + 2), 127);
			*(pEvent2->m_pData + 1) = 32;
			*(pEvent2->m_pData + 2) = (unsigned char)
				CLIP (0, *(pEvent2->m_pData + 2), 127);
			MIDIEvent_SetKindSingle (pEvent3, 0xC0 | (pEvent->m_lKind & 0x0F));
			*(pEvent3->m_pData + 1) = (unsigned char)
				CLIP (0, *(pEvent3->m_pData + 1), 127);
		}
	}
	/* 1→2, 3→2 */
	else if (lKind == MIDIEVENT_NOTEONNOTEOFF || lKind == MIDIEVENT_NOTEONNOTEON0) {
		MIDIEvent* pEvent2 = NULL;
		MIDIEvent_DeleteCombinedEvent (pEvent);
		MIDIEvent_SetKindSingle (pEvent, 0x90 | (pEvent->m_lKind & 0x0F));
		*(pEvent->m_pData + 1) =  (unsigned char)60;
		*(pEvent->m_pData + 2) =  (unsigned char)100;
		if (lKind == MIDIEVENT_NOTEONNOTEOFF) {
			pEvent2 = MIDIEvent_CreateNoteOff 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 60, 100);
		}
		else {
			pEvent2 = MIDIEvent_CreateNoteOn 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 60, 0);
		}
		pEvent->m_pPrevCombinedEvent = NULL;
		pEvent->m_pNextCombinedEvent = pEvent2;
		pEvent2->m_pPrevCombinedEvent = pEvent;
		pEvent2->m_pNextCombinedEvent = NULL;
		if (!MIDIEvent_IsFloating (pEvent)) {
			MIDIEvent_SetNextEvent (pEvent, pEvent2);
		}
	}
	/* 1→3, 2→3 */
	else if (lKind == MIDIEVENT_RPNCHANGE || lKind == MIDIEVENT_NRPNCHANGE ||
		lKind == MIDIEVENT_PATCHCHANGE) {
		MIDIEvent* pEvent2 = NULL;
		MIDIEvent* pEvent3 = NULL;
		MIDIEvent_DeleteCombinedEvent (pEvent);
		if (lKind == MIDIEVENT_NRPNCHANGE) {
			MIDIEvent_SetKindSingle (pEvent, 0xB0 | (pEvent->m_lKind & 0x0F));
			*(pEvent->m_pData + 1) = (unsigned char)99;
			*(pEvent->m_pData + 2) = (unsigned char)0;
			pEvent2 = MIDIEvent_CreateControlChange 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 98, 0);
			pEvent3 = MIDIEvent_CreateControlChange 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 06, 64);
		}
		else if (lKind == MIDIEVENT_RPNCHANGE) {
			MIDIEvent_SetKindSingle (pEvent, 0xB0 | (pEvent->m_lKind & 0x0F));
			*(pEvent->m_pData + 1) = (unsigned char)101;
			*(pEvent->m_pData + 2) = (unsigned char)0;
			pEvent2 = MIDIEvent_CreateControlChange 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 100, 0);
			pEvent3 = MIDIEvent_CreateControlChange 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 06, 64);
		}
		else if (lKind == MIDIEVENT_PATCHCHANGE) {
			MIDIEvent_SetKindSingle (pEvent, 0xC0 | (pEvent->m_lKind & 0x0F));
			*(pEvent->m_pData + 1) = (unsigned char)0;
			*(pEvent->m_pData + 2) = (unsigned char)0;
			pEvent2 = MIDIEvent_CreateControlChange 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 32, 0);
			pEvent3 = MIDIEvent_CreateProgramChange 
				(pEvent->m_lTime, (pEvent->m_lKind & 0x0F), 0);
		}
		pEvent->m_pPrevCombinedEvent = NULL;
		pEvent->m_pNextCombinedEvent = pEvent2;
		pEvent2->m_pPrevCombinedEvent = pEvent;
		pEvent2->m_pNextCombinedEvent = pEvent3;
		pEvent3->m_pPrevCombinedEvent = pEvent2;
		pEvent3->m_pNextCombinedEvent = NULL;
		if (!MIDIEvent_IsFloating (pEvent)) {
			MIDIEvent_SetNextEvent (pEvent, pEvent2);
			MIDIEvent_SetNextEvent (pEvent2, pEvent3);
		}
	}
	/* 1→1, 2→1, 3→1 */
	else {
		MIDIEvent_DeleteCombinedEvent (pEvent);
		MIDIEvent_SetKindSingle (pEvent, (lKind & 0x00FF));
	}
	return 1;
}

/* イベントの長さ取得 */
long __stdcall MIDIEvent_GetLen (MIDIEvent* pEvent) {
	return (pEvent->m_lLen);
}

/* イベントのデータ部を取得 */
long __stdcall MIDIEvent_GetData (MIDIEvent* pEvent, unsigned char* pBuf, long lLen) {
	assert (pEvent);
	assert (pBuf);
	assert (0 <= lLen && lLen < MIDIEVENT_MAXLEN);
	memset (pBuf, 0, lLen);
	if (pEvent->m_pData != NULL && pEvent->m_lLen > 0) { /* 20091024条件式追加 */
		memcpy (pBuf, pEvent->m_pData, MIN (lLen, pEvent->m_lLen));
	}
	return MIN (lLen, pEvent->m_lLen);
}

/* イベントのデータ部を設定(この関数は大変危険です。整合性のチェキはしません) */
long __stdcall MIDIEvent_SetData (MIDIEvent* pEvent, unsigned char* pBuf, long lLen) {
	assert (pEvent);
	assert (pBuf);
	assert (0 <= lLen && lLen < MIDIEVENT_MAXLEN);
	if (MIDIEvent_IsMIDIEvent (pEvent)) {
		if (lLen <= 0 || lLen >= 4) {
			return 0;
		}
		if (pEvent->m_pData) { /* 20091024条件式追加 */
			pEvent->m_lLen = lLen;
			memcpy (pEvent->m_pData, pBuf, lLen);
		}
	}
	else {
		if (lLen > 0) { /* 20091024条件式追加 */
			pEvent->m_pData = realloc (pEvent->m_pData, lLen);
			if (pEvent->m_pData == NULL) {
				pEvent->m_lLen = 0;
				return 0;
			}
			else {
				pEvent->m_lLen = lLen;
				memcpy (pEvent->m_pData, pBuf, lLen);
			}
		}
		else { /* 20091024条件式追加 */
			free (pEvent->m_pData);
			pEvent->m_pData = NULL;
			pEvent->m_lLen = 0;
		}
	}
	return 1;
}

/* 文字列の文字コードを判別(ANSI) */
long GetTextCharCodeA (const char* pData, long lLen) {
	/* データ部に文字コード指定のある場合は、それを返す。 */
	if (pData != NULL) {
		if (lLen >= 11 && _strnicmp (pData, "{@UTF-16LE}", 11) == 0) {
			return MIDIEVENT_UTF16LE;
		}
		else if (lLen >= 11 && _strnicmp (pData, "{@UTF-16BE}", 11) == 0) {
			return MIDIEVENT_UTF16BE;
		}
		else if (lLen >= 8 && _strnicmp (pData, "{@UTF-8}", 8) == 0) {
			return MIDIEVENT_UTF8;
		}
		else if (lLen >= 8 && _strnicmp (pData, "{@LATIN}", 8) == 0) {
			return MIDIEVENT_LATIN;
		}
		else if (lLen >= 5 && _strnicmp (pData, "{@JP}", 5) == 0) {
			return MIDIEVENT_JP;
		}
		else {
			/* データ部に文字コード指定のない場合、MIDIEVENT_NOCHARCODEを返す。 */
			return MIDIEVENT_NOCHARCODE;
		}
	}
	return MIDIEVENT_NOCHARCODE;
}

/* 文字列の文字コードを判別(UNICODE) */
long GetTextCharCodeW (const wchar_t* pData, long lLen) {
	/* データ部に文字コード指定のある場合は、それを返す。 */
	if (pData != NULL) {
		if (lLen >= 11 && _wcsnicmp ((wchar_t*)pData, L"{@UTF-16LE}", 11) == 0) {
			return MIDIEVENT_UTF16LE;
		}
		else if (lLen >= 11 && _wcsnicmp ((wchar_t*)pData, L"{@UTF-16BE}", 11) == 0) {
			return MIDIEVENT_UTF16BE;
		}
		else if (lLen >= 8 && _wcsnicmp ((wchar_t*)pData, L"{@UTF-8}", 8) == 0) {
			return MIDIEVENT_UTF8;
		}
		else if (lLen >= 8 && _wcsnicmp ((wchar_t*)pData, L"{@LATIN}", 8) == 0) {
			return MIDIEVENT_LATIN;
		}
		else if (lLen >= 5 && _wcsnicmp ((wchar_t*)pData, L"{@JP}", 5) == 0) {
			return MIDIEVENT_JP;
		}
		else {
			/* データ部に文字コード指定のない場合、MIDIEVENT_NOCHARCODEを返す。 */
			return MIDIEVENT_NOCHARCODE;
		}
	}
	return MIDIEVENT_NOCHARCODE;
}

/* イベントの文字コードを取得 */
long __stdcall MIDIEvent_GetCharCodeSingle (MIDIEvent* pEvent) {
	unsigned char* pData = NULL;
	long lLen = 0;
	assert (pEvent);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return 0;
	}
	pData = pEvent->m_pData;
	lLen = pEvent->m_lLen;
	/* データ部に文字コード指定のある場合は、それを返す。 */
	if (pData != NULL) {
		if (lLen >= 2 && *((unsigned char*)pData) == 0xFF && *((unsigned char*)pData + 1) == 0xFE) {
			return MIDIEVENT_UTF16LE;
		}
		if (lLen >= 2 && *((unsigned char*)pData) == 0xFE && *((unsigned char*)pData + 1) == 0xFF) {
			return MIDIEVENT_UTF16BE;
		}
		if (lLen >= 3 && *((unsigned char*)pData) == 0xEF && *((unsigned char*)pData + 1) == 0xBB && 
			*((unsigned char*)pData + 2) == 0xBF) {
			return MIDIEVENT_UTF8;
		}
		if (lLen >= 8 && _strnicmp (pData, "{@LATIN}", 8) == 0) {
			return MIDIEVENT_LATIN;
		}
		if (lLen >= 5 && _strnicmp (pData, "{@JP}", 5) == 0) {
			return MIDIEVENT_JP;
		}
	}
	/* データ部に文字コード指定のない場合、MIDIEVENT_NOCHARCODEを返す。 */
	return MIDIEVENT_NOCHARCODE;
}

/* 直近の同種のイベントの文字コードを返す(隠蔽) */
long __stdcall MIDIEvent_FindCharCode (MIDIEvent* pEvent) {
	long lCharCode = 0;
	MIDITrack* pTrack = NULL;
	MIDIEvent* pEvent2 = NULL;
	assert (pEvent);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return 0;
	}
	pEvent2 = pEvent->m_pPrevEvent;
	while (pEvent2) {
		if (pEvent2->m_lKind == pEvent->m_lKind) {
			lCharCode = MIDIEvent_GetCharCodeSingle (pEvent2);
			switch (lCharCode) {
			case MIDIEVENT_LATIN:
				return MIDIEVENT_NOCHARCODELATIN;
			case MIDIEVENT_JP:
				return MIDIEVENT_NOCHARCODEJP;
			case MIDIEVENT_UTF8:
				return MIDIEVENT_NOCHARCODEUTF8;
			case MIDIEVENT_UTF16LE:
				return MIDIEVENT_NOCHARCODEUTF16LE;
			case MIDIEVENT_UTF16BE:
				return MIDIEVENT_NOCHARCODEUTF16BE;
			}
		}
		pEvent2 = pEvent2->m_pPrevEvent;
	}
	return MIDIEVENT_NOCHARCODE;
}

/* イベントの文字コードを取得(不明な場合は直近の同種のイベントより推定) */
long __stdcall MIDIEvent_GetCharCode (MIDIEvent* pEvent) {
	long lCharCode = 0;
	MIDITrack* pTrack = NULL;
	assert (pEvent);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return 0;
	}
	lCharCode = MIDIEvent_GetCharCodeSingle (pEvent);
	/* データ部に文字コード指定のある場合は、それを返す。 */
	if (lCharCode != MIDIEVENT_NOCHARCODE) {
		return lCharCode;
	}
	/* データ部に文字コード指定のない場合、直近の同種のイベントの文字コードを探索する。 */
	return MIDIEvent_FindCharCode (pEvent);
}

/* 2バイトずつバイトオーダーを変換しながらmemcpy */
void* memcpyinv2 (void* pDestData, const void* pSrcData, long lLen) {
	long i;
	unsigned char* pSrc = (unsigned char*)pSrcData;
	unsigned char* pDest = (unsigned char*)pDestData;
	for (i = 0; i < lLen / 2; i ++) {
		*(pDest + 0) = *(pSrc + 1);
		*(pDest + 1) = *(pSrc + 0);
		pDest += 2;
		pSrc += 2;
	}
	return pDestData;
}

/* イベントの文字コードを設定 */
long __stdcall MIDIEvent_SetCharCode (MIDIEvent* pEvent, long lCharCode) {
	long lOldCharCode = 0;
	long lTextLenA = 0;
	long lTextLenW = 0;
	long lTextLenUTF8 = 0;
	long lRet = 0;
	wchar_t wcText[MIDIEVENT_MAXLEN + 1];
	unsigned char byText[MIDIEVENT_MAXLEN + 1];
	unsigned char byTextUTF8[MIDIEVENT_MAXLEN + 1];
	unsigned char byData[MIDIEVENT_MAXLEN + sizeof (wchar_t)];
	long lDataLen = 0;
	assert (pEvent);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return 0;
	}
	memset (wcText, 0, sizeof (wcText));
	memset (byText, 0, sizeof (byText));
	memset (byTextUTF8, 0, sizeof (byTextUTF8));
	memset (byData, 0, sizeof (byData));
	lOldCharCode = MIDIEvent_GetCharCode (pEvent);
	if (lOldCharCode == MIDIEVENT_NOCHARCODE &&
		g_theMIDIDataLib.m_lDefaultCharCode != MIDIEVENT_NOCHARCODE) {
		lOldCharCode = g_theMIDIDataLib.m_lDefaultCharCode | 0x10000;
	}
	if (lOldCharCode == MIDIEVENT_NOCHARCODELATIN && lCharCode == MIDIEVENT_UTF16LE ||
		lOldCharCode == MIDIEVENT_NOCHARCODELATIN && lCharCode == MIDIEVENT_UTF16BE ||
		lOldCharCode == MIDIEVENT_NOCHARCODELATIN && lCharCode == MIDIEVENT_UTF8 ||
		lOldCharCode == MIDIEVENT_LATIN && lCharCode == MIDIEVENT_UTF16LE ||
		lOldCharCode == MIDIEVENT_LATIN && lCharCode == MIDIEVENT_UTF16BE ||
		lOldCharCode == MIDIEVENT_LATIN && lCharCode == MIDIEVENT_UTF8 ||
		lOldCharCode == MIDIEVENT_NOCHARCODEUTF16LE && lCharCode == MIDIEVENT_LATIN ||
		lOldCharCode == MIDIEVENT_NOCHARCODEUTF16BE && lCharCode == MIDIEVENT_LATIN ||
		lOldCharCode == MIDIEVENT_NOCHARCODEUTF8 && lCharCode == MIDIEVENT_LATIN ||
		lOldCharCode == MIDIEVENT_UTF16LE && lCharCode == MIDIEVENT_LATIN ||
		lOldCharCode == MIDIEVENT_UTF16BE && lCharCode == MIDIEVENT_LATIN ||
		lOldCharCode == MIDIEVENT_UTF8 && lCharCode == MIDIEVENT_LATIN) {
		setlocale (LC_CTYPE, ".1252");
	}
	else if (lOldCharCode == MIDIEVENT_NOCHARCODEJP && lCharCode == MIDIEVENT_UTF16LE ||
		lOldCharCode == MIDIEVENT_NOCHARCODEJP && lCharCode == MIDIEVENT_UTF16BE ||
		lOldCharCode == MIDIEVENT_NOCHARCODEJP && lCharCode == MIDIEVENT_UTF8 ||
		lOldCharCode == MIDIEVENT_JP && lCharCode == MIDIEVENT_UTF16LE ||
		lOldCharCode == MIDIEVENT_JP && lCharCode == MIDIEVENT_UTF16BE ||
		lOldCharCode == MIDIEVENT_JP && lCharCode == MIDIEVENT_UTF8 ||
		lOldCharCode == MIDIEVENT_NOCHARCODEUTF16LE && lCharCode == MIDIEVENT_JP ||
		lOldCharCode == MIDIEVENT_NOCHARCODEUTF16BE && lCharCode == MIDIEVENT_JP ||
		lOldCharCode == MIDIEVENT_NOCHARCODEUTF8 && lCharCode == MIDIEVENT_JP ||
		lOldCharCode == MIDIEVENT_UTF16LE && lCharCode == MIDIEVENT_JP ||
		lOldCharCode == MIDIEVENT_UTF16BE && lCharCode == MIDIEVENT_JP ||
		lOldCharCode == MIDIEVENT_UTF8 && lCharCode == MIDIEVENT_JP) {
		setlocale (LC_CTYPE, ".932");
	}
	else {
		char szDefaultLocale[16];
		memset (szDefaultLocale, 0, sizeof (szDefaultLocale));
		if (g_theMIDIDataLib.m_lDefaultCharCode == 0) {
			strcpy (szDefaultLocale, ".ACP");
		}
		else {
			_snprintf (szDefaultLocale, 15, ".%d", g_theMIDIDataLib.m_lDefaultCharCode);
		}
		setlocale (LC_CTYPE, szDefaultLocale);
	}
	/* wcTextとbyTextとbyTextUTF8にテキストを設定 */
	switch (lOldCharCode) {
	case MIDIEVENT_NOCHARCODEUTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpy (wcText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
		lRet = wcstombs (byText, wcText, MIDIEVENT_MAXLEN);
		lRet = wcstoutf8 (byTextUTF8, wcText, MIDIEVENT_MAXLEN);
		break;
	case MIDIEVENT_NOCHARCODEUTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpyinv2 (wcText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
		lRet = wcstombs (byText, wcText, MIDIEVENT_MAXLEN);
		lRet = wcstoutf8 (byTextUTF8, wcText, MIDIEVENT_MAXLEN);
		break;
	case MIDIEVENT_NOCHARCODEUTF8:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpy (byTextUTF8, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
		lRet = utf8towcs (wcText, byText, MIDIEVENT_MAXLEN);
		lRet = wcstombs (byText, wcText, MIDIEVENT_MAXLEN); 
		break;
	case MIDIEVENT_LATIN:
		memcpy (byText, pEvent->m_pData + 8, CLIP (0, pEvent->m_lLen - 8, MIDIEVENT_MAXLEN));
		lRet = mbstowcs (wcText, byText, MIDIEVENT_MAXLEN);
		lRet = wcstoutf8 (byTextUTF8, wcText, MIDIEVENT_MAXLEN);
		break;
	case MIDIEVENT_JP:
		memcpy (byText, pEvent->m_pData + 5, CLIP (0, pEvent->m_lLen - 5, MIDIEVENT_MAXLEN));
		lRet = mbstowcs (wcText, byText, MIDIEVENT_MAXLEN);
		lRet = wcstoutf8 (byTextUTF8, wcText, MIDIEVENT_MAXLEN);
		break;
	case MIDIEVENT_UTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpy (wcText, pEvent->m_pData + 2, CLIP (0, pEvent->m_lLen - 2, MIDIEVENT_MAXLEN));
		lRet = wcstombs (byText, wcText, MIDIEVENT_MAXLEN);
		lRet = wcstoutf8 (byTextUTF8, wcText, MIDIEVENT_MAXLEN);
		break;
	case MIDIEVENT_UTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpyinv2 (wcText, pEvent->m_pData + 2, CLIP (0, pEvent->m_lLen - 2, MIDIEVENT_MAXLEN));
		lRet = wcstombs (byText, wcText, MIDIEVENT_MAXLEN);
		lRet = wcstoutf8 (byTextUTF8, wcText, MIDIEVENT_MAXLEN);
		break;
	case MIDIEVENT_UTF8:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpy (byTextUTF8, pEvent->m_pData + 3, CLIP (0, pEvent->m_lLen - 3, MIDIEVENT_MAXLEN));
		lRet = utf8towcs (wcText, byTextUTF8, MIDIEVENT_MAXLEN);
		lRet = wcstombs (byText, wcText, MIDIEVENT_MAXLEN); 
		break;
	default:
		memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
		lRet = mbstowcs (wcText, byText, MIDIEVENT_MAXLEN);
		lRet = wcstoutf8 (byTextUTF8, wcText, MIDIEVENT_MAXLEN);
		break;
	}
	lTextLenA = strlen (byText);
	lTextLenW = wcslen (wcText);
	lTextLenUTF8 = strlen (byTextUTF8);
	/* 目的の文字列をbyDataに作成 */
	switch (lCharCode) {
	case MIDIEVENT_NOCHARCODEUTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpy (byData, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_NOCHARCODEUTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpyinv2 (byData, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
	case MIDIEVENT_NOCHARCODEUTF8:
		memcpy (byData, byTextUTF8, CLIP (0, lTextLenUTF8, MIDIEVENT_MAXLEN));
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_LATIN:
		memcpy (byData, "{@LATIN}", 8);
		memcpy (byData + 8, byText, CLIP (0, lTextLenA, MIDIEVENT_MAXLEN - 8));
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_JP:
		memcpy (byData, "{@JP}", 5);
		memcpy (byData + 5, byText, CLIP (0, lTextLenA, MIDIEVENT_MAXLEN - 5));
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_UTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		byData[0] = 0xFF;
		byData[1] = 0xFE;
		memcpy (byData + 2, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN - 2));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_UTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		byData[0] = 0xFE;
		byData[1] = 0xFF;
		memcpyinv2 (byData + 2, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN - 2));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_UTF8:
		byData[0] = 0xEF;
		byData[1] = 0xBB;
		byData[2] = 0xBF;
		memcpy (byData + 3, byTextUTF8, CLIP (0, lTextLenUTF8, MIDIEVENT_MAXLEN - 3));
		lDataLen = strlen (byData);
		break;
	default:
		memcpy (byData, byText, CLIP (0, lTextLenA, MIDIEVENT_MAXLEN));
		lDataLen = strlen (byData);
		break;
	}
	return MIDIEvent_SetData (pEvent, byData, lDataLen);
}

/* イベントのテキストを取得(ANSI) */
char* __stdcall MIDIEvent_GetTextA (MIDIEvent* pEvent, char* pBuf, long lLen) {
	long lCharCode = 0;
	long lTextLenA = 0;
	long lTextLenW = 0;
	char szDefaultLocale[16];
	unsigned char byText[MIDIEVENT_MAXLEN + 1];
	wchar_t wcText[MIDIEVENT_MAXLEN + 1];
	assert (pEvent);
	assert (pBuf);
	assert (lLen > 0);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return 0;
	}
	memset (pBuf, 0, lLen);
	memset (wcText, 0, sizeof (wcText));
	memset (byText, 0, sizeof (wcText));
	memset (szDefaultLocale, 0, sizeof (szDefaultLocale));
	if (g_theMIDIDataLib.m_lDefaultCharCode == 0) {
		strcpy (szDefaultLocale, ".ACP");
	}
	else {
		_snprintf (szDefaultLocale, 15, ".%d", g_theMIDIDataLib.m_lDefaultCharCode);
	}
	setlocale (LC_CTYPE, szDefaultLocale);
	lCharCode = MIDIEvent_GetCharCode (pEvent);
	if (lCharCode == MIDIEVENT_NOCHARCODE &&
		g_theMIDIDataLib.m_lDefaultCharCode != MIDIEVENT_NOCHARCODE) {
		lCharCode = g_theMIDIDataLib.m_lDefaultCharCode | 0x10000;
	}
	if (pEvent->m_pData && pEvent->m_lLen > 0) {
		switch (lCharCode) {
		case MIDIEVENT_NOCHARCODEUTF16LE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			memcpy (wcText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			wcstombs (pBuf, wcText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_UTF16LE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			strncpy (pBuf, "{@UTF-16LE}", CLIP (0, lLen - 12, 11));
			memcpy (wcText, pEvent->m_pData + 2, CLIP (0, pEvent->m_lLen - 2, MIDIEVENT_MAXLEN - 2));
			wcstombs (pBuf + 11, wcText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_NOCHARCODEUTF16BE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			memcpyinv2 (wcText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			wcstombs (pBuf, wcText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_UTF16BE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			strncpy (pBuf, "{@UTF-16BE}", CLIP (0, lLen - 12, 11));
			memcpyinv2 (wcText, pEvent->m_pData + 2, CLIP (0, pEvent->m_lLen - 2, MIDIEVENT_MAXLEN - 2));
			wcstombs (pBuf + 11, wcText, MAX (0, lLen - 12));
			break;
		case MIDIEVENT_NOCHARCODEUTF8:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			utf8towcs (wcText, byText, MIDIEVENT_MAXLEN);
			wcstombs (pBuf, wcText, MAX (0, lLen - 1));
		case MIDIEVENT_UTF8:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			strncpy (pBuf, "{@UTF-8}", CLIP (0, lLen - 9, 8));
			memcpy (byText, pEvent->m_pData + 3, CLIP (0, pEvent->m_lLen - 3, MIDIEVENT_MAXLEN));
			utf8towcs (wcText, byText, MIDIEVENT_MAXLEN);
			wcstombs (pBuf + 8, wcText, MAX (0, lLen - 9));
			break;
		default:
			memcpy (pBuf, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MAX(0, lLen - 1)));
			break;
		}
	}
	return pBuf;
}

/* イベントのテキストを取得(UNICODE) */
wchar_t* __stdcall MIDIEvent_GetTextW (MIDIEvent* pEvent, wchar_t* pBuf, long lLen) {
	long lCharCode = 0;
	long lTextLenA = 0;
	long lTextLenW = 0;
	unsigned char byText[MIDIEVENT_MAXLEN + 1];
	wchar_t wcText[MIDIEVENT_MAXLEN + 1];
	char szDefaultLocale[16];
	assert (pEvent);
	assert (pBuf);
	assert (lLen > 0);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return NULL;
	}
	memset (pBuf, 0, lLen * sizeof (wchar_t)); 
	memset (byText, 0, sizeof (byText));
	memset (wcText, 0, sizeof (wcText));
	memset (szDefaultLocale, 0, sizeof (szDefaultLocale));
	if (g_theMIDIDataLib.m_lDefaultCharCode == 0) {
		strcpy (szDefaultLocale, ".ACP");
	}
	else {
		_snprintf (szDefaultLocale, 15, ".%d", g_theMIDIDataLib.m_lDefaultCharCode);
	}
	lCharCode = MIDIEvent_GetCharCode (pEvent);
	if (lCharCode == MIDIEVENT_NOCHARCODE &&
		g_theMIDIDataLib.m_lDefaultCharCode != MIDIEVENT_NOCHARCODE) {
		lCharCode = g_theMIDIDataLib.m_lDefaultCharCode | 0x10000;
	}
	if (pEvent->m_pData && pEvent->m_lLen > 0) {
		switch (lCharCode) {
		case MIDIEVENT_NOCHARCODELATIN:
			setlocale (LC_CTYPE, ".1252");
			memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			mbstowcs (pBuf, byText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_NOCHARCODEJP:
			setlocale (LC_CTYPE, ".932");
			memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			mbstowcs (pBuf, byText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_NOCHARCODEUTF16LE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			memcpy (pBuf, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MAX (0, lLen * (long)sizeof (wchar_t))));
			break;
		case MIDIEVENT_NOCHARCODEUTF16BE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			memcpyinv2 (pBuf, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MAX (0, lLen * (long)sizeof (wchar_t))));
			break;
		case MIDIEVENT_NOCHARCODEUTF8:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			utf8towcs (pBuf, byText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_LATIN:
			setlocale (LC_CTYPE, ".1252");
			memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			mbstowcs (pBuf, byText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_JP:
			setlocale (LC_CTYPE, ".932");
			memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			mbstowcs (pBuf, byText, MAX (0, lLen - 1));
			break;
		case MIDIEVENT_UTF16LE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			wcsncpy (pBuf, L"{@UTF-16LE}", CLIP (0, lLen - 12, 11));
			memcpy (pBuf + 11, pEvent->m_pData + 2, CLIP (0, pEvent->m_lLen - 2, MAX (0, (lLen - 1) * (long)sizeof (wchar_t))));
			break;
		case MIDIEVENT_UTF16BE:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			wcsncpy (pBuf, L"{@UTF-16BE}", CLIP (0, lLen - 12, 11));
			memcpyinv2 (pBuf + 11, pEvent->m_pData + 2, CLIP (0, pEvent->m_lLen - 2, MAX (0, (lLen - 1) * (long)sizeof (wchar_t))));
			break;
		case MIDIEVENT_UTF8:
			/* Windows Only (wchar_t = 2bytes and little endian) */
			wcsncpy (pBuf, L"{@UTF-8}", CLIP (0, lLen - 9, 8));
			memcpy (byText, pEvent->m_pData + 3, CLIP (0, pEvent->m_lLen - 3, MIDIEVENT_MAXLEN));
			utf8towcs (pBuf + 8, byText, MAX (0, lLen - 9));
			break;
		default:
			setlocale (LC_CTYPE, szDefaultLocale);
			memcpy (byText, pEvent->m_pData, CLIP (0, pEvent->m_lLen, MIDIEVENT_MAXLEN));
			mbstowcs (pBuf, byText, MAX(0, lLen - 1));
			break;
		}
	}
	return (wchar_t*)pBuf;
}


/* イベントのテキストを設定(ANSI) */
long __stdcall MIDIEvent_SetTextA (MIDIEvent* pEvent, const char* pszText) {
	long lOldCharCode = 0;
	long lCharCode = 0;
	long lTextCharCode = 0;
	long lTextLenA = 0;
	long lTextLenW = 0;
	unsigned char byData[MIDIEVENT_MAXLEN + sizeof (wchar_t)];
	long lDataLen = 0;
	wchar_t wcText[MIDIEVENT_MAXLEN + 1];
	char szDefaultLocale[16];
	assert (pEvent);
	assert (pszText);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return 0;
	}
	memset (byData, 0, sizeof (byData));
	memset (wcText, 0, sizeof (wcText));
	memset (szDefaultLocale, 0, sizeof (szDefaultLocale));
	if (g_theMIDIDataLib.m_lDefaultCharCode == 0) {
		strcpy (szDefaultLocale, ".ACP");
	}
	else {
		_snprintf (szDefaultLocale, 15, ".%d", g_theMIDIDataLib.m_lDefaultCharCode);
	}
	lOldCharCode = MIDIEvent_FindCharCode (pEvent);
	if (lOldCharCode == MIDIEVENT_NOCHARCODE &&
		g_theMIDIDataLib.m_lDefaultCharCode != MIDIEVENT_NOCHARCODE) {
		lOldCharCode = g_theMIDIDataLib.m_lDefaultCharCode | 0x10000;
	}
	lTextLenA = strlen (pszText);
	lTextCharCode = GetTextCharCodeA (pszText, lTextLenA);
	if (lTextCharCode == MIDIEVENT_NOCHARCODE) {
		lCharCode = lOldCharCode;
	}
	else {
		lCharCode = lTextCharCode;
	}
	switch (lCharCode) {
	case MIDIEVENT_NOCHARCODEUTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		setlocale (LC_CTYPE, szDefaultLocale);
		mbstowcs (wcText, pszText, MIDIEVENT_MAXLEN); 
		lTextLenW = wcslen (wcText);
		memcpy (byData, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_NOCHARCODEUTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		setlocale (LC_CTYPE, szDefaultLocale);
		mbstowcs (wcText, pszText, MIDIEVENT_MAXLEN); 
		lTextLenW = wcslen (wcText);
		memcpyinv2 (byData, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_NOCHARCODEUTF8: /* 20191230追加 */
		/* Windows Only (wchar_t = 2bytes and little endian) */
		setlocale (LC_CTYPE, szDefaultLocale);
		mbstowcs (wcText, pszText, MIDIEVENT_MAXLEN);
		lTextLenW = wcslen (wcText);
		wcstoutf8 (byData, wcText, MIDIEVENT_MAXLEN);
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_LATIN:
		memcpy (byData, pszText, CLIP (0, lTextLenA, MIDIEVENT_MAXLEN));
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_JP:
		memcpy (byData, pszText, CLIP (0, lTextLenA, MIDIEVENT_MAXLEN));
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_UTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		setlocale (LC_CTYPE, szDefaultLocale);
		byData[0] = 0xFF;
		byData[1] = 0xFE;
		mbstowcs (wcText, pszText + 11, MIDIEVENT_MAXLEN); /* 20191230修正 */
		lTextLenW = wcslen (wcText);
		memcpy (byData + 2, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN - 2));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_UTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		setlocale (LC_CTYPE, szDefaultLocale);
		byData[0] = 0xFE;
		byData[1] = 0xFF;
		mbstowcs (wcText, pszText + 11, MIDIEVENT_MAXLEN); /* 20191230修正 */
		lTextLenW = wcslen (wcText);
		memcpyinv2 (byData + 2, wcText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN - 2));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_UTF8: /* 20191230追加 */
		/* Windows Only (wchar_t = 2bytes and little endian) */
		setlocale (LC_CTYPE, szDefaultLocale);
		byData[0] = 0xEF;
		byData[1] = 0xBB;
		byData[2] = 0xBF;
		mbstowcs (wcText, pszText + 8, MIDIEVENT_MAXLEN);
		lTextLenW = wcslen (wcText);
		wcstoutf8 (byData + 3, wcText, MIDIEVENT_MAXLEN - 3);
		lDataLen = strlen (byData);
		break;
	default:
		memcpy (byData, pszText, CLIP (0, lTextLenA, MIDIEVENT_MAXLEN));
		lDataLen = strlen (byData);
		break;
	}
	return MIDIEvent_SetData (pEvent, byData, lDataLen);
}

/* イベントのテキストを設定(UNICODE) */
long __stdcall MIDIEvent_SetTextW (MIDIEvent* pEvent, const wchar_t* pszText) {
	long lCharCode = 0;
	long lOldCharCode = 0;
	long lTextCharCode = 0;
	long lTextLenW = 0;
	unsigned char byData[MIDIEVENT_MAXLEN + sizeof (wchar_t)];
	long lDataLen = 0;
	char szDefaultLocale[16];
	assert (pEvent);
	assert (pszText);
	if (pEvent->m_lKind <= 0x00 || pEvent->m_lKind >= 0x1F) {
		return 0;
	}
	memset (byData, 0, sizeof (byData));
	memset (szDefaultLocale, 0, sizeof (szDefaultLocale));
	if (g_theMIDIDataLib.m_lDefaultCharCode == 0) {
		strcpy (szDefaultLocale, ".ACP");
	}
	else {
		_snprintf (szDefaultLocale, 15, ".%d", g_theMIDIDataLib.m_lDefaultCharCode);
	}
	lOldCharCode = MIDIEvent_FindCharCode (pEvent);
	if (lOldCharCode == MIDIEVENT_NOCHARCODE &&
		g_theMIDIDataLib.m_lDefaultCharCode != MIDIEVENT_NOCHARCODE) {
		lOldCharCode = g_theMIDIDataLib.m_lDefaultCharCode | 0x10000;
	}
	lTextLenW = wcslen (pszText);
	lTextCharCode = GetTextCharCodeW (pszText, lTextLenW);
	if (lTextCharCode == MIDIEVENT_NOCHARCODE) {
		lCharCode = lOldCharCode;
	}
	else {
		lCharCode = lTextCharCode;
	}
	switch (lCharCode) {
	case MIDIEVENT_NOCHARCODELATIN:
		setlocale (LC_CTYPE, ".1252");
		wcstombs (byData, pszText, MIDIEVENT_MAXLEN);
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_NOCHARCODEJP:
		setlocale (LC_CTYPE, ".932");
		wcstombs (byData, pszText, MIDIEVENT_MAXLEN);
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_NOCHARCODEUTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpy (byData, pszText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_NOCHARCODEUTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		memcpyinv2 (byData, (const char*)pszText, CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_NOCHARCODEUTF8: /* 20191230追加 */
		/* Windows Only (wchar_t = 2bytes and little endian) */
		wcstoutf8 (byData, pszText, MIDIEVENT_MAXLEN);
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_LATIN:
		setlocale (LC_CTYPE, ".1252");
		wcstombs (byData, pszText, MIDIEVENT_MAXLEN);
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_JP:
		setlocale (LC_CTYPE, ".932");
		wcstombs (byData, pszText, MIDIEVENT_MAXLEN);
		lDataLen = strlen (byData);
		break;
	case MIDIEVENT_UTF16LE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		byData[0] = 0xFF;
		byData[1] = 0xFE;
		memcpy (byData + 2, (pszText + 11), CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN - 2));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_UTF16BE:
		/* Windows Only (wchar_t = 2bytes and little endian) */
		byData[0] = 0xFE;
		byData[1] = 0xFF;
		memcpyinv2 (byData + 2, (const char*)(pszText + 11), CLIP (0, lTextLenW * sizeof (wchar_t), MIDIEVENT_MAXLEN - 2));
		lDataLen = wcslen ((wchar_t*)byData) * sizeof (wchar_t);
		break;
	case MIDIEVENT_UTF8: /* 20191230追加 */
		/* Windows Only (wchar_t = 2bytes and little endian) */
		byData[0] = 0xEF;
		byData[1] = 0xBB;
		byData[2] = 0xBF;
		wcstoutf8 (byData + 3, pszText + 8, MIDIEVENT_MAXLEN - 3);
		lDataLen = strlen (byData);
		break;
	default:
		setlocale (LC_CTYPE, szDefaultLocale);
		wcstombs (byData, pszText, MIDIEVENT_MAXLEN);
		lDataLen = strlen (byData);
		break;
	}
	return MIDIEvent_SetData (pEvent, byData, lDataLen);
}

/* SMPTEオフセットの取得(SMPTEオフセットイベントのみ) */
long __stdcall MIDIEvent_GetSMPTEOffset 
(MIDIEvent* pEvent, long* pMode, long* pHour, long* pMin, long* pSec, long* pFrame, long* pSubFrame) {
	assert (pEvent);
	assert (pMode);
	assert (pHour);
	assert (pMin);
	assert (pSec);
	assert (pFrame);
	assert (pSubFrame);
	if (pEvent->m_lKind != MIDIEVENT_SMPTEOFFSET) {
		return 0;
	}
	if (pEvent->m_lLen < 5) {
		return 0;
	}
	*pMode =     *(pEvent->m_pData) >> 5;
	*pHour =     *(pEvent->m_pData) & 0x1F;
	*pMin =      *(pEvent->m_pData + 1);
	*pSec =      *(pEvent->m_pData + 2);
	*pFrame =    *(pEvent->m_pData + 3);
	*pSubFrame = *(pEvent->m_pData + 4);
	return 1;
}

/* SMPTEオフセットの設定(SMPTEオフセットイベントのみ) */
long __stdcall MIDIEvent_SetSMPTEOffset 
(MIDIEvent* pEvent, long lMode, long lHour, long lMin, long lSec, long lFrame, long lSubFrame) {
	long lMaxFrame[4] = {23, 24, 29, 29};
	assert (pEvent);
	if (pEvent->m_lKind != MIDIEVENT_SMPTEOFFSET) {
		return 0;
	}
	if (pEvent->m_lLen < 5) {
		return 0;
	}
	*(pEvent->m_pData) = (unsigned char)(((lMode & 0x03) << 5) | (CLIP (0, lHour, 23)));
	*(pEvent->m_pData + 1) = (unsigned char)(CLIP (0, lMin, 59));
	*(pEvent->m_pData + 2) = (unsigned char)(CLIP (0, lSec, 59));
	*(pEvent->m_pData + 3) = (unsigned char)(CLIP (0, lFrame, lMaxFrame[lMode & 0x03]));
	*(pEvent->m_pData + 4) = (unsigned char)(CLIP (0, lSubFrame, 99));
	return 1;
}

/* テンポ取得(テンポイベントのみ) */
long __stdcall MIDIEvent_GetTempo (MIDIEvent* pEvent) {
	assert (pEvent);
	if (pEvent->m_lKind != MIDIEVENT_TEMPO) {
		return 0;
	}
	if (pEvent->m_lLen < 3) {
		return 60000000 / 120;
	}
	return *(pEvent->m_pData) << 16 | *(pEvent->m_pData + 1) << 8 | *(pEvent->m_pData + 2);
}

/* テンポ設定(テンポイベントのみ) */
long __stdcall MIDIEvent_SetTempo (MIDIEvent* pEvent, long lTempo) {
	unsigned char c[3];
	assert (pEvent);
	if (pEvent->m_lKind != MIDIEVENT_TEMPO) {
		return 0;
	}
	if (pEvent->m_lLen < 3) {
		return 0;
	}
	c[0] = (unsigned char)((CLIP (MIDIEVENT_MINTEMPO, lTempo, MIDIEVENT_MAXTEMPO) & 0xFF0000) >> 16);
	c[1] = (unsigned char)((CLIP (MIDIEVENT_MINTEMPO, lTempo, MIDIEVENT_MAXTEMPO) & 0x00FF00) >> 8);
	c[2] = (unsigned char)((CLIP (MIDIEVENT_MINTEMPO, lTempo, MIDIEVENT_MAXTEMPO) & 0x0000FF) >> 0);
	memcpy (pEvent->m_pData, c, 3);
	return 1;
}

/* 拍子取得(拍子記号イベントのみ) */
long __stdcall MIDIEvent_GetTimeSignature (MIDIEvent* pEvent, long* pnn, long* pdd, long* pcc, long* pbb) {
	assert (pEvent);
	assert (pnn);
	assert (pdd);
	assert (pcc);
	assert (pbb);
	if (pEvent->m_lKind != MIDIEVENT_TIMESIGNATURE) {
		return 0;
	}
	if (pEvent->m_lLen < 1) {
		return 0;
	}
	*pnn = 4;
	*pdd = 2;
	*pcc = 24;
	*pbb = 8;
	if (pEvent->m_lLen >= 1) {
		*pnn = (long)(*(pEvent->m_pData));
	}
	if (pEvent->m_lLen >= 2) {
		*pdd = (long)(*(pEvent->m_pData + 1));
	}
	if (pEvent->m_lLen >= 3) {
		*pcc = (long)(*(pEvent->m_pData + 2));
	}
	if (pEvent->m_lLen >= 4) {
		*pbb = (long)(*(pEvent->m_pData + 3));
	}
	return 1;
}

/* 拍子の設定(拍子記号イベントのみ) */
long __stdcall MIDIEvent_SetTimeSignature (MIDIEvent* pEvent, long lnn, long ldd, long lcc, long lbb) {
	assert (pEvent);
	if (pEvent->m_lKind != MIDIEVENT_TIMESIGNATURE) {
		return 0;
	}
	if (pEvent->m_lLen < 1) {
		return 0;
	}
	if (pEvent->m_lLen >= 1) {
		*(pEvent->m_pData) = (unsigned char)lnn;
	}
	if (pEvent->m_lLen >= 2) {
		*(pEvent->m_pData + 1) = (unsigned char)ldd;
	}
	if (pEvent->m_lLen >= 3) {
		*(pEvent->m_pData + 2) = (unsigned char)lcc;
	}
	if (pEvent->m_lLen >= 4) {
		*(pEvent->m_pData + 3) = (unsigned char)lbb;
	}
	return 1;
}

/* 調性記号の取得(調性記号イベントのみ) */
long __stdcall MIDIEvent_GetKeySignature (MIDIEvent* pEvent, long* psf, long* pmi) {
	assert (pEvent);
	assert (psf);
	assert (pmi);
	if (pEvent->m_lKind != MIDIEVENT_KEYSIGNATURE) {
		return 0;
	}
	if (pEvent->m_lLen < 1) {
		return 0;
	}
	*psf = 0;
	*pmi = 0;
	if (pEvent->m_lLen >= 1) {
		*psf = (long)(char)(*(pEvent->m_pData));
	}
	if (pEvent->m_lLen >= 2) {
		*pmi = (long)(*(pEvent->m_pData + 1));
	}
	return 1;
}

/* 調性記号の設定(調性記号イベントのみ) */
long __stdcall MIDIEvent_SetKeySignature (MIDIEvent* pEvent, long lsf, long lmi) {
	assert (pEvent);
	if (pEvent->m_lKind != MIDIEVENT_KEYSIGNATURE) {
		return 0;
	}
	if (pEvent->m_lLen < 1) {
		return 0;
	}
	if (pEvent->m_lLen >= 1) {
		*(pEvent->m_pData) = (unsigned char)CLIP (-7, lsf, 7);
	}
	if (pEvent->m_lLen >= 2) {
		*(pEvent->m_pData + 1) = (unsigned char)CLIP (0, lmi, 1);
	}
	return 1;
}

/* イベントのメッセージ取得(MIDIイベント及びシステムエクスクルーシヴのみ) */
long __stdcall MIDIEvent_GetMIDIMessage (MIDIEvent* pEvent, char* pMessage, long lLen) {
	assert (pEvent);
	assert (pMessage);
	assert (lLen > 0);
	if (MIDIEvent_IsMIDIEvent (pEvent) || MIDIEvent_IsSysExEvent (pEvent)) {
		return MIDIEvent_GetData (pEvent, (unsigned char*)pMessage, lLen);
	}
	return 0;
}

/* イベントのメッセージ設定(MIDIイベント及びシステムエクスクルーシヴのみ) */
long __stdcall MIDIEvent_SetMIDIMessage (MIDIEvent* pEvent, char* pMessage, long lLen) {
	assert (pEvent);
	assert (pMessage);
	assert (lLen > 0);
	if (MIDIEvent_IsMIDIEvent (pEvent) || MIDIEvent_IsSysExEvent (pEvent)) {
		return MIDIEvent_SetData (pEvent, (unsigned char*)pMessage, lLen);
	}
	return 0;
}

/* イベントのチャンネル取得(MIDIイベントのみ) */
long __stdcall MIDIEvent_GetChannel (MIDIEvent* pEvent) {
	assert (pEvent);
	if (MIDIEvent_IsMIDIEvent (pEvent)) {
		assert (pEvent->m_lKind  == *(pEvent->m_pData));
		return pEvent->m_lKind & 0x0F;
	}
	return -1;
}

/* イベントのチャンネル設定(MIDIイベントのみ) */
long __stdcall MIDIEvent_SetChannel (MIDIEvent* pEvent, long lCh) {
	long lCount = 0;
	MIDIEvent* pTempEvent = NULL;
	assert (pEvent);
	assert (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xEF);
	pTempEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	while (pTempEvent) {
		if (MIDIEvent_IsMIDIEvent (pTempEvent))	{
			pTempEvent->m_lKind &= 0xF0;
			pTempEvent->m_lKind |= (unsigned char)(CLIP (0, lCh, 15));
			*(pTempEvent->m_pData) &= 0xF0;
			*(pTempEvent->m_pData) |= (unsigned char)(CLIP (0, lCh, 15));
			assert (pTempEvent->m_lKind == *(pTempEvent->m_pData));
			/* 前後の同種イベントのポインタのつなぎ替え */
			if (pTempEvent->m_pPrevSameKindEvent) {
				pTempEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = 
					MIDIEvent_SearchNextSameKindEvent (pTempEvent->m_pPrevSameKindEvent);
			}
			if (pTempEvent->m_pNextSameKindEvent) {
				pTempEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent =
					MIDIEvent_SearchPrevSameKindEvent (pTempEvent->m_pNextSameKindEvent);
			}
			/* 前後の同種イベントポインタ設定 */
			pTempEvent->m_pPrevSameKindEvent = MIDIEvent_SearchPrevSameKindEvent (pTempEvent);
			if (pTempEvent->m_pPrevSameKindEvent) {
				pTempEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = pTempEvent;
			}
			pTempEvent->m_pNextSameKindEvent = MIDIEvent_SearchNextSameKindEvent (pTempEvent);
			if (pTempEvent->m_pNextSameKindEvent) {
				pTempEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent = pTempEvent;
			}
			lCount++;
		}
		pTempEvent = pTempEvent->m_pNextCombinedEvent;
	};
	return lCount;
}

/* イベントの時刻取得 */
long __stdcall MIDIEvent_GetTime (MIDIEvent* pEvent) {
	assert (pEvent);
	return pEvent->m_lTime;
}






/* 単体イベントの時刻設定 */
/* イベントがリストの要素の場合、ポインタをつなぎ変えて時刻順序を正しく保ちます。 */
long __stdcall MIDIEvent_SetTimeSingle (MIDIEvent* pEvent, long lTime) {
	long lCurrentTime = pEvent->m_lTime;
	MIDITrack* pTrack = (MIDITrack*)(pEvent->m_pParent);
	assert (pEvent);

	/* 浮遊イベントの場合は単純に時刻設定 */
	if (MIDIEvent_IsFloating (pEvent)) {
		pEvent->m_lTime = CLIP (0, lTime, 0x7FFFFFFF);
		return 1;
	}

	/* 以下は浮遊イベントでない場合の処理 */
	/* EOTイベントを動かす場合の特殊処理 */
	if (pEvent->m_lKind == MIDIEVENT_ENDOFTRACK && pEvent->m_pNextEvent == NULL) {
		/* EOTイベントの前に別のイベントがある場合 */
		if (pEvent->m_pPrevEvent) {
			/* EOTイベントはそのイベントより前には移動しない。 */
			if (pEvent->m_pPrevEvent->m_lTime > lTime) {
				pEvent->m_lTime = pEvent->m_pPrevEvent->m_lTime;
			}
			else {
				pEvent->m_lTime = lTime;
			}
		}
		/* EOTイベントの前に別のイベントが無い場合 */
		else {
			/* タイムスタンプ0より前には移動しない。 */
			pEvent->m_lTime = CLIP (0, lTime, 0x7FFFFFFF);
		}
		return 1;
	}

	/* エンドオブトラック以外のイベントの場合 */
	/* 現在のタイムより後方へ動かす場合 */
	if (lTime >= lCurrentTime) {
		/* pTempEventの直前に挿入する。pTempEventがなければ最後に挿入する。 */
		MIDIEvent* pTempEvent = pEvent;
		MIDIEvent* pLastEvent = NULL;
		/* ノートオフイベントの場合 */
		if (MIDIEvent_IsNoteOff (pTempEvent)) {
			MIDIEvent* pNoteOnEvent = pTempEvent->m_pPrevCombinedEvent;
			/* 対応するノートオンイベントがある場合(20090713追加) */
			if (pNoteOnEvent) {
				/* 音長さ=0以下の場合(20090713追加) */
				/* 対応するノートオンイベントの直後に確定 */
				if (lTime <= pNoteOnEvent->m_lTime) {
					lTime = pNoteOnEvent->m_lTime;
					pLastEvent = pNoteOnEvent;
					/* 注:SetTimeSingleから呼ばれた場合とSetDurationから呼ばれた場合でNoteOn-NoteOff順序が異なる。 */
					/* NoteOnを先に移動済みの場合(SetTimeSingle) */
					if (pNoteOnEvent->m_pNextEvent != pEvent) {
						pTempEvent = pNoteOnEvent->m_pNextEvent;
					}
					/* NoteOnを移動していない場合(SetDuration) */
					else {
						pTempEvent = pEvent->m_pNextEvent; /* 20190101:pNoteOnEventをpEventに修正 */
					}
					if (pTempEvent) {
						if (pTempEvent->m_lKind == MIDIEVENT_ENDOFTRACK &&
							pTempEvent->m_pNextEvent == NULL) {
							pTempEvent->m_lTime = lTime;
						}
					}
				}
				/* 音長さ=0以上の場合(20090713追加) */
				else {
					while (pTempEvent) {
						if (pTempEvent->m_lTime > lTime ||
							(pTempEvent->m_lTime == lTime && !MIDIEvent_IsNoteOff (pTempEvent))) {
							break;
						}
						/* EOTよりも後に来る場合はEOTを後ろへ追い込む */
						if (pTempEvent->m_lKind == MIDIEVENT_ENDOFTRACK &&
							pTempEvent->m_pNextEvent == NULL) {
							pTempEvent->m_lTime = lTime;
							break;
						}
						pLastEvent = pTempEvent;
						pTempEvent = pTempEvent->m_pNextEvent;
					}
				}
			}
			/* 対応するノートオンイベントがない場合 */
			else {
				while (pTempEvent) {
					if (pTempEvent->m_lTime > lTime ||
						(pTempEvent->m_lTime == lTime && !MIDIEvent_IsNoteOff (pTempEvent))) {
						break;
					}
					/* EOTよりも後に来る場合はEOTを後ろへ追い込む */
					if (pTempEvent->m_lKind == MIDIEVENT_ENDOFTRACK &&
						pTempEvent->m_pNextEvent == NULL) {
						pTempEvent->m_lTime = lTime;
						break;
					}
					pLastEvent = pTempEvent;
					pTempEvent = pTempEvent->m_pNextEvent;
				}
			}
		}
		/* その他の場合 */
		else {
			while (pTempEvent) {
				if (pTempEvent->m_lTime > lTime) {
					break;
				}
				/* EOTよりも後に来る場合はEOTを後ろへ追い込む */
				if (pTempEvent->m_lKind == MIDIEVENT_ENDOFTRACK &&
					pTempEvent->m_pNextEvent == NULL) {
					pTempEvent->m_lTime = lTime;
					break;
				}
				pLastEvent = pTempEvent;
				pTempEvent = pTempEvent->m_pNextEvent;
			}
		}
		/* pTempEventの直前にpEventを挿入する場合 */
		if (pTempEvent) {
			/* if (pTempEvent != pEvent) { 20080622廃止 */
			/* if (pTempEvent->m_pPrevEvent != pEvent) { /* 20080622修正 */
			if (pTempEvent != pEvent &&
				pTempEvent->m_pPrevEvent != pEvent) { /* 20190407修正 */
				MIDIEvent_SetFloating (pEvent);
				pEvent->m_lTime = lTime;
				MIDIEvent_SetPrevEvent (pTempEvent, pEvent);
			}
			else {
				pEvent->m_lTime = lTime;
			}
		}
		/* リンクリストの最後にpEventを挿入する場合 */
		else if (pLastEvent) {
			/* if (pLastEvent != pEvent) { 20080622廃止 */
			/* if (pLastEvent->m_pNextEvent != pEvent) {  20080622修正 */
			if (pLastEvent != pEvent &&
				pLastEvent->m_pNextEvent != pEvent) { /* 20190407修正 */
				MIDIEvent_SetFloating (pEvent);
				pEvent->m_lTime = lTime;
				MIDIEvent_SetNextEvent (pLastEvent, pEvent);
			}
			else {
				pEvent->m_lTime = lTime;
			}
		}
		/* 空のリストに挿入する場合 */
		else if (pTrack) {
			pEvent->m_lTime = lTime;
			pEvent->m_pParent = pTrack;
			pEvent->m_pNextEvent = NULL;
			pEvent->m_pPrevEvent = NULL;
			pEvent->m_pNextSameKindEvent = NULL;
			pEvent->m_pPrevSameKindEvent = NULL;
			pTrack->m_pFirstEvent = pEvent;
			pTrack->m_pLastEvent = pEvent;
			pTrack->m_lNumEvent ++;
		}

	}
	/* 現在のタイムより前方へ動かす場合 */
	else if (lTime < lCurrentTime) {
		/* pTempEventの直後に挿入する。pTempEventがなければ最初に挿入する。 */
		MIDIEvent* pTempEvent = pEvent;
		MIDIEvent* pFirstEvent = NULL;
		/* ノートオフイベントの場合 */
		if (MIDIEvent_IsNoteOff (pEvent)) {
			MIDIEvent* pNoteOnEvent = pTempEvent->m_pPrevCombinedEvent;
			/* 対応するノートオンイベントがある場合(20090713追加) */
			if (pNoteOnEvent) {
				/* 音長さ=0以下の場合(20090713追加) */
				/* 対応するノートオンイベントの直後に確定 */
				if (lTime <= pNoteOnEvent->m_lTime) {
					lTime = pNoteOnEvent->m_lTime;
					pFirstEvent = NULL;
					pTempEvent = pNoteOnEvent;
				}
				/* 音長さ=0以上の場合(20090713追加) */
				else {
					while (pTempEvent) {
						if (pTempEvent->m_lTime < lTime ||
							(pTempEvent->m_lTime == lTime && MIDIEvent_IsNoteOff (pTempEvent))) {
							break;
						}
						/* 対応するノートオンイベントより前には行かない */
						if (pTempEvent == pNoteOnEvent) {
							break;
						}
						pFirstEvent = pTempEvent;
						pTempEvent = pTempEvent->m_pPrevEvent;
					}
				}
			}
			/* 対応するノートオンイベントがない場合 */
			else {
				while (pTempEvent) {
					if (pTempEvent->m_lTime < lTime ||
						(pTempEvent->m_lTime == lTime && MIDIEvent_IsNoteOff (pTempEvent))) {
						break;
					}
					pFirstEvent = pTempEvent;
					pTempEvent = pTempEvent->m_pPrevEvent;
				}
			}
		}
		/* その他のイベントの場合 */
		else {
			while (pTempEvent) {
				if (pTempEvent->m_lTime <= lTime) {
					break;
				}
				pFirstEvent = pTempEvent;
				pTempEvent = pTempEvent->m_pPrevEvent;
			}
		}
		/* pTempEventの直後にpEventを挿入する場合 */
		if (pTempEvent) {
			if (pTempEvent != pEvent && 
				pTempEvent->m_pNextEvent != pEvent) { /* 20080721修正 */
				MIDIEvent_SetFloating (pEvent);
				pEvent->m_lTime = lTime;
				MIDIEvent_SetNextEvent (pTempEvent, pEvent);
			}
			else {
				pEvent->m_lTime = lTime;
			}
		}
		/* リンクリストの最初にpEventを挿入する場合 */
		else if (pFirstEvent) {
			if (pFirstEvent != pEvent &&
				pFirstEvent->m_pPrevEvent != pEvent) { /* 20080721追加 */
				MIDIEvent_SetFloating (pEvent);
				pEvent->m_lTime = lTime;
				MIDIEvent_SetPrevEvent (pFirstEvent, pEvent);
			}
			else {
				pEvent->m_lTime = lTime;
			}
		}
		/* 空のリストに挿入する場合 */
		else if (pTrack) {
			pEvent->m_lTime = lTime;
			pEvent->m_pParent = pTrack;
			pEvent->m_pNextEvent = NULL;
			pEvent->m_pPrevEvent = NULL;
			pEvent->m_pNextSameKindEvent = NULL;
			pEvent->m_pPrevSameKindEvent = NULL;
			pTrack->m_pFirstEvent = pEvent;
			pTrack->m_pLastEvent = pEvent;
			pTrack->m_lNumEvent ++;
		}
	}
	return 1;
}

/* イベントの時刻設定 */
/* (結合イベントも連動して時刻が相対的に動きます) */
long __stdcall MIDIEvent_SetTime (MIDIEvent* pEvent, long lTime) {
	long nCounter = 0;
	long lTargetTime, lDeltaTime;
	MIDIEvent* pMoveEvent = NULL;
	lTime = CLIP (0, lTime, 0x7FFFFFFF);
	lDeltaTime = lTime - pEvent->m_lTime;
	pMoveEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	while (pMoveEvent) {
		lTargetTime = pMoveEvent->m_lTime + lDeltaTime;
		lTargetTime = CLIP (0, lTargetTime, 0x7FFFFFFF);
		MIDIEvent_SetTimeSingle (pMoveEvent, lTargetTime);
		nCounter++;
		pMoveEvent = pMoveEvent->m_pNextCombinedEvent;
	}
	return nCounter;
}

/* イベントのキー取得(ノートオフ・ノートオン・チャンネルアフターのみ) */
long __stdcall MIDIEvent_GetKey (MIDIEvent* pEvent) {
	assert (pEvent);
	assert (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xAF);
	if (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xAF) {
		return *(pEvent->m_pData + 1);
	}
	return 0;
}

/* イベントのキー設定(ノートオフ・ノートオン・チャンネルアフターのみ) */
long __stdcall MIDIEvent_SetKey (MIDIEvent* pEvent, long lKey) {
	long lCount = 0;
	MIDIEvent* pTempEvent = pEvent;
	assert (pEvent);
	assert (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xAF);
	pTempEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	while (pTempEvent) {
		if (0x80 <= pTempEvent->m_lKind && pTempEvent->m_lKind <= 0xAF) {
			*(pTempEvent->m_pData + 1) = (unsigned char)(CLIP (0, lKey, 127));
			lCount++;
		}
		pTempEvent = pTempEvent->m_pNextCombinedEvent;
	}
	return lCount;
}

/* イベントのベロシティ取得(ノートオフ・ノートオンのみ) */
long __stdcall MIDIEvent_GetVelocity (MIDIEvent* pEvent) {
	assert (pEvent);
	assert (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F);
	if (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F) {
		return *(pEvent->m_pData + 2);
	}
	return 0;
}

/* イベントのベロシティ設定(ノートオフ・ノートオンのみ) */
long __stdcall MIDIEvent_SetVelocity (MIDIEvent* pEvent, long lVel) {
	assert (pEvent);
	assert (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F);
	if (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0x8F) {
		*(pEvent->m_pData + 2) = (unsigned char)(CLIP (0, lVel, 127));
		return 1;
	}
	else if (0x90 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F) {
		if (*(pEvent->m_pData + 2) >= 1) {
			*(pEvent->m_pData + 2) = (unsigned char)(CLIP (1, lVel, 127));
			return 1;
		}
		return 0;
	}
	return 0;
}

/* 結合イベントの音長さ取得(ノートのみ) */
long __stdcall MIDIEvent_GetDuration (MIDIEvent* pEvent) {
	long lDuration = 0;
	MIDIEvent* pNoteOnEvent = NULL;
	MIDIEvent* pNoteOffEvent = NULL;
	assert (pEvent);
	assert (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F);
	if (!MIDIEvent_IsNote (pEvent)) {
		return 0;
	}
	if (MIDIEvent_IsNoteOn (pEvent)) {
		pNoteOnEvent = pEvent;
		pNoteOffEvent = pEvent->m_pNextCombinedEvent;
		lDuration = pNoteOffEvent->m_lTime - pNoteOnEvent->m_lTime;
		assert (lDuration >= 0);
	}
	else if (MIDIEvent_IsNoteOff (pEvent)) {
		pNoteOffEvent = pEvent;
		pNoteOnEvent = pEvent->m_pPrevCombinedEvent;
		lDuration = pNoteOnEvent->m_lTime - pNoteOffEvent->m_lTime;
		assert (lDuration <= 0);
	}
	return lDuration;

}

/* 結合イベントの音長さ設定(ノートのみ) */
long __stdcall MIDIEvent_SetDuration (MIDIEvent* pEvent, long lDuration) {
	long lTime = 0;
	MIDIEvent* pNoteOnEvent = NULL;
	MIDIEvent* pNoteOffEvent = NULL;
	assert (pEvent);
	assert (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0x9F);
	if (!MIDIEvent_IsNote (pEvent)) {
		return 0;
	}
	if (MIDIEvent_IsNoteOn (pEvent)) {
		if (lDuration < 0) {
			return 0;
		}
		pNoteOnEvent = pEvent;
		pNoteOffEvent = pEvent->m_pNextCombinedEvent;
		lTime = CLIP (0, pNoteOnEvent->m_lTime + lDuration, 0x7FFFFFFF);
		return MIDIEvent_SetTimeSingle (pNoteOffEvent, lTime);
	}
	else if (MIDIEvent_IsNoteOff (pEvent)) {
		if (lDuration > 0) {
			return 0;
		}
		pNoteOffEvent = pEvent;
		pNoteOnEvent = pEvent->m_pPrevCombinedEvent;
		lTime = CLIP (0, pNoteOffEvent->m_lTime + lDuration, 0x7FFFFFFF);
		/* TODO:lDuration==0のとき、NoteOnのほうが後に来てしまう。*/
		return MIDIEvent_SetTimeSingle (pNoteOnEvent, lTime);
	}
	return 1;
}


/* 結合イベントのバンク取得(RPNチェンジ・NRPNチェンジ・パッチチェンジのみ) */
long __stdcall MIDIEvent_GetBank (MIDIEvent* pEvent) {
	long lBankMSB = 0; /* バンク上位:CC#0,CC#99,CC#101 */
	long lBankLSB = 0; /* バンク下位:CC#32,CC#98,CC#100 */
	MIDIEvent* pMSBEvent = NULL;
	MIDIEvent* pLSBEvent = NULL;
	assert (pEvent);
	assert (0xB0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xCF);
	if (MIDIEvent_IsPatchChange (pEvent) ||
		MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		pMSBEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
		assert (MIDIEvent_IsControlChange (pMSBEvent));
		lBankMSB = MIDIEvent_GetValue (pMSBEvent);
		pLSBEvent = pMSBEvent->m_pNextCombinedEvent;
		assert (MIDIEvent_IsControlChange (pLSBEvent));
		lBankLSB = MIDIEvent_GetValue (pLSBEvent);
		return (lBankMSB << 7) | (lBankLSB & 0x7F);
	}
	else if (MIDIEvent_IsProgramChange (pEvent)) {
		long lBankMSBFound = 0;
		long lBankLSBFound = 0;
		long lChannel = MIDIEvent_GetChannel (pEvent);
		MIDIEvent* pTempEvent = pEvent->m_pPrevEvent;		
		while (pTempEvent) {
			if (MIDIEvent_IsControlChange (pTempEvent)) {
				if (MIDIEvent_GetChannel (pTempEvent) == lChannel) {
					if (MIDIEvent_GetNumber (pTempEvent) == 0) {
						lBankMSB = MIDIEvent_GetValue (pTempEvent);
						lBankMSBFound++;
					}
					else if (MIDIEvent_GetNumber (pTempEvent) == 32) {
						lBankLSB = MIDIEvent_GetValue (pTempEvent);
						lBankLSBFound++;
					}
					if (lBankMSBFound && lBankLSBFound) {
						break;
					}
				}
			}
			pTempEvent = pTempEvent->m_pPrevEvent;
		}
		return (lBankMSB << 7) | (lBankLSB & 0x7F);
	}
	else if (MIDIEvent_IsControlChange (pEvent)) {
		long lNumber = MIDIEvent_GetNumber (pEvent);
		if (lNumber == 6 || lNumber == 38) {
			long lBankMSBFound = 0;
			long lBankLSBFound = 0;
			long lChannel = MIDIEvent_GetChannel (pEvent);
			MIDIEvent* pTempEvent = pEvent->m_pPrevEvent;
			while (pTempEvent) {
				if (MIDIEvent_IsControlChange (pTempEvent)) {
					if (MIDIEvent_GetChannel (pTempEvent) == lChannel) {
						if (MIDIEvent_GetNumber (pTempEvent) == 99 ||
							MIDIEvent_GetNumber (pTempEvent) == 101) {
							lBankMSB = MIDIEvent_GetValue (pTempEvent);
							lBankMSBFound++;
						}
						else if (MIDIEvent_GetNumber (pTempEvent) == 98 ||
							MIDIEvent_GetNumber (pTempEvent) == 100) {
							lBankLSB = MIDIEvent_GetValue (pTempEvent);
							lBankLSBFound++;
						}
						if (lBankMSBFound && lBankLSBFound) {
							break;
						}
					}
				}
				pTempEvent = pTempEvent->m_pPrevEvent;
			}
		}
		return (lBankMSB << 7) | (lBankLSB & 0x7F);
	}
	return 0;
}

/* 結合イベントのバンク上位(MSB)取得(RPNチェンジ・NRPNチェンジ・パッチチェンジのみ) */
long __stdcall MIDIEvent_GetBankMSB (MIDIEvent* pEvent) {
	long lBankMSB = 0; /* バンク上位:CC#0,CC#99,CC#101 */
	long lBankLSB = 0; /* バンク下位:CC#32,CC#98,CC#100 */
	MIDIEvent* pMSBEvent = NULL;
	MIDIEvent* pLSBEvent = NULL;
	assert (pEvent);
	assert (0xB0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xCF);
	if (MIDIEvent_IsPatchChange (pEvent) ||
		MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		pMSBEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
		assert (MIDIEvent_IsControlChange (pMSBEvent));
		lBankMSB = MIDIEvent_GetValue (pMSBEvent);
		pLSBEvent = pMSBEvent->m_pNextCombinedEvent;
		assert (MIDIEvent_IsControlChange (pLSBEvent));
		lBankLSB = MIDIEvent_GetValue (pLSBEvent);
		return lBankMSB;
	}
	else if (MIDIEvent_IsProgramChange (pEvent)) {
		long lChannel = MIDIEvent_GetChannel (pEvent);
		MIDIEvent* pTempEvent = pEvent->m_pPrevEvent;
		while (pTempEvent) {
			if (MIDIEvent_IsControlChange (pTempEvent)) {
				if (MIDIEvent_GetChannel (pTempEvent) == lChannel) {
					if (MIDIEvent_GetNumber (pTempEvent) == 0) {
						return MIDIEvent_GetValue (pTempEvent);
					}
				}
			}
			pTempEvent = pTempEvent->m_pPrevEvent;
		}
		return 0;
	}
	else if (MIDIEvent_IsControlChange (pEvent)) {
		long lNumber = MIDIEvent_GetNumber (pEvent);
		if (lNumber == 6 || lNumber == 38) {
			long lChannel = MIDIEvent_GetChannel (pEvent);
			MIDIEvent* pTempEvent = pEvent->m_pPrevEvent;
			while (pTempEvent) {
				if (MIDIEvent_IsControlChange (pTempEvent)) {
					if (MIDIEvent_GetChannel (pTempEvent) == lChannel) {
						if (MIDIEvent_GetNumber (pTempEvent) == 99 ||
							MIDIEvent_GetNumber (pTempEvent) == 101) {
							return MIDIEvent_GetValue (pTempEvent);
						}
					}
				}
				pTempEvent = pTempEvent->m_pPrevEvent;
			}
		}
		return 0;
	}
	return 0;
}

/* 結合イベントのバンク下位(LSB)取得(RPNチェンジ・NRPNチェンジ・パッチチェンジのみ) */
long __stdcall MIDIEvent_GetBankLSB (MIDIEvent* pEvent) {
	long lBankMSB = 0; /* バンク上位:CC#0,CC#99,CC#101 */
	long lBankLSB = 0; /* バンク下位:CC#32,CC#98,CC#100 */
	MIDIEvent* pMSBEvent = NULL;
	MIDIEvent* pLSBEvent = NULL;
	assert (pEvent);
	assert (0xB0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xCF);
	if (MIDIEvent_IsPatchChange (pEvent) ||
		MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		pMSBEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
		assert (MIDIEvent_IsControlChange (pMSBEvent));
		lBankMSB = MIDIEvent_GetValue (pMSBEvent);
		pLSBEvent = pMSBEvent->m_pNextCombinedEvent;
		assert (MIDIEvent_IsControlChange (pLSBEvent));
		lBankLSB = MIDIEvent_GetValue (pLSBEvent);
		return lBankLSB;
	}
	else if (MIDIEvent_IsProgramChange (pEvent)) {
		long lChannel = MIDIEvent_GetChannel (pEvent);
		MIDIEvent* pTempEvent = pEvent->m_pPrevEvent;
		while (pTempEvent) {
			if (MIDIEvent_IsControlChange (pTempEvent)) {
				if (MIDIEvent_GetChannel (pTempEvent) == lChannel) {
					if (MIDIEvent_GetNumber (pTempEvent) == 32) {
						return MIDIEvent_GetValue (pTempEvent);
					}
				}
			}
			pTempEvent = pTempEvent->m_pPrevEvent;
		}
		return 0;
	}
	else if (MIDIEvent_IsControlChange (pEvent)) {
		long lNumber = MIDIEvent_GetNumber (pEvent);
		if (lNumber == 6 || lNumber == 38) {
			long lChannel = MIDIEvent_GetChannel (pEvent);
			MIDIEvent* pTempEvent = pEvent->m_pPrevEvent;
			while (pTempEvent) {
				if (MIDIEvent_IsControlChange (pTempEvent)) {
					if (MIDIEvent_GetChannel (pTempEvent) == lChannel) {
						if (MIDIEvent_GetNumber (pTempEvent) == 98 ||
							MIDIEvent_GetNumber (pTempEvent) == 100) {
							return MIDIEvent_GetValue (pTempEvent);
						}
					}
				}
				pTempEvent = pTempEvent->m_pPrevEvent;
			}
		}
		return 0;
	}
	return 0;
}

/* 結合イベントのバンク設定(RPNチェンジ・NRPNチェンジ・パッチチェンジのみ) */
long __stdcall MIDIEvent_SetBank (MIDIEvent* pEvent, long lBank) {
	long lBankMSB = (CLIP(0, lBank, 16383) >> 7); /* バンク上位:CC#0,CC#99,CC#101 */
	long lBankLSB = (CLIP(0, lBank, 16383) & 0x7F); /* バンク下位:CC#32,CC#98,CC#100 */
	MIDIEvent* pMSBEvent = NULL;
	MIDIEvent* pLSBEvent = NULL;
	assert (pEvent);
	assert (0xB0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xCF);
	if (MIDIEvent_IsPatchChange (pEvent) ||
		MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		pMSBEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
		assert (MIDIEvent_IsControlChange (pMSBEvent));
		MIDIEvent_SetValue (pMSBEvent, lBankMSB);
		pLSBEvent = pMSBEvent->m_pNextCombinedEvent;
		assert (MIDIEvent_IsControlChange (pLSBEvent));
		MIDIEvent_SetValue (pLSBEvent, lBankLSB);
		return 2;
	}
	return 0;
}

/* 結合イベントのバンク上位(MSB)設定(RPNチェンジ・NRPNチェンジ・パッチチェンジのみ) */
long __stdcall MIDIEvent_SetBankMSB (MIDIEvent* pEvent, long lBankMSB) {
	MIDIEvent* pMSBEvent = NULL;
	assert (pEvent);
	assert (0xB0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xCF);
	if (MIDIEvent_IsPatchChange (pEvent) ||
		MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		pMSBEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
		assert (MIDIEvent_IsControlChange (pMSBEvent));
		MIDIEvent_SetValue (pMSBEvent, lBankMSB);
		return 1;
	}
	return 0;
}

/* 結合イベントのバンク下位(LSB)設定(RPNチェンジ・NRPNチェンジ・パッチチェンジのみ) */
long __stdcall MIDIEvent_SetBankLSB (MIDIEvent* pEvent, long lBankLSB) {
	MIDIEvent* pMSBEvent = NULL;
	MIDIEvent* pLSBEvent = NULL;
	assert (pEvent);
	assert (0xB0 <= pEvent->m_lKind && pEvent->m_lKind <= 0xCF);
	if (MIDIEvent_IsPatchChange (pEvent) ||
		MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		pMSBEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
		pLSBEvent = pMSBEvent->m_pNextCombinedEvent;
		assert (MIDIEvent_IsControlChange (pLSBEvent));
		MIDIEvent_SetValue (pLSBEvent, lBankLSB);
		return 1;
	}
	return 0;
}

/* 結合イベントのプログラムナンバーを取得(パッチイベントのみ) */
long __stdcall MIDIEvent_GetPatchNum (MIDIEvent* pEvent) {
	assert (pEvent);
	/* パッチチェンジイベントの場合 */
	if (MIDIEvent_IsPatchChange (pEvent)) {
		MIDIEvent* pCC000Event = MIDIEvent_GetFirstCombinedEvent (pEvent);
		MIDIEvent* pCC032Event = pCC000Event->m_pNextCombinedEvent;
		MIDIEvent* pPCEvent = pCC032Event->m_pNextCombinedEvent;
		return *(pPCEvent->m_pData + 1);
	}
	return 0;
}

/* 結合イベントのプログラムナンバーを設定(パッチイベントのみ) */
long __stdcall MIDIEvent_SetPatchNum (MIDIEvent* pEvent, long lNum) {
	assert (pEvent);
	/* パッチチェンジイベントの場合 */
	if (MIDIEvent_IsPatchChange (pEvent)) {
		MIDIEvent* pCC000Event = MIDIEvent_GetFirstCombinedEvent (pEvent);
		MIDIEvent* pCC032Event = pCC000Event->m_pNextCombinedEvent;
		MIDIEvent* pPCEvent = pCC032Event->m_pNextCombinedEvent;
		*(pPCEvent->m_pData + 1) = (unsigned char)CLIP (0, lNum, 127);
		return 1;
	}
	return 0;
}

/* 結合イベントのデータエントリーMSBを取得(RPNチェンジ・NPRNチェンジのみ) */
long __stdcall MIDIEvent_GetDataEntryMSB (MIDIEvent* pEvent) {
	assert (pEvent);
	/* RPNチェンジ・NRPNチェンジの場合 */
	if (MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		MIDIEvent* pCC101Event = MIDIEvent_GetFirstCombinedEvent (pEvent);
		MIDIEvent* pCC100Event = pCC101Event->m_pNextCombinedEvent;
		MIDIEvent* pCC006Event = pCC100Event->m_pNextCombinedEvent;
		return *(pCC006Event->m_pData + 2);
	}
	return 0;
}

/* 結合イベントのデータエントリーMSBを設定(RPNチェンジ・NPRNチェンジのみ) */
long __stdcall MIDIEvent_SetDataEntryMSB (MIDIEvent* pEvent, long lVal) {
	assert (pEvent);
	/* RPNチェンジ・NRPNチェンジの場合 */
	if (MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		MIDIEvent* pCC101Event = MIDIEvent_GetFirstCombinedEvent (pEvent);
		MIDIEvent* pCC100Event = pCC101Event->m_pNextCombinedEvent;
		MIDIEvent* pCC006Event = pCC100Event->m_pNextCombinedEvent;
		*(pCC006Event->m_pData + 2) = (unsigned char)(CLIP (0, lVal, 127));
		return 1;
	}
	return 0;
}

/* イベントの番号取得(シーケンス番号・チャンネルプリフィックス・ポートプリフィックス・ */
/* コントロールチェンジ・プログラムチェンジ) */
long __stdcall MIDIEvent_GetNumber (MIDIEvent* pEvent) {
	assert (pEvent);
	/* シーケンス番号の場合 */
	if (MIDIEvent_IsSequenceNumber (pEvent)) {
		if (pEvent->m_lLen < 2) {
			return 0;
		}
		return (*(pEvent->m_pData) << 8) + *(pEvent->m_pData + 1);
	}
	/* チャンネルプリフィックス、ポートプリフィックスの場合 */
	else if (MIDIEvent_IsChannelPrefix (pEvent) || MIDIEvent_IsPortPrefix (pEvent)) {
		if (pEvent->m_lLen < 1) {
			return 0;
		}
		return *(pEvent->m_pData);
	}
	/* コントロールチェンジの場合 */
	else if (MIDIEvent_IsControlChange (pEvent)) {
		return *(pEvent->m_pData + 1);
	}
	/* プログラムチェンジ */
	else if (MIDIEvent_IsProgramChange (pEvent)) {
		return *(pEvent->m_pData + 1);
	}
	return 0;
}

/* イベントの番号設定(シーケンス番号・チャンネルプリフィックス・ポートプリフィックス・ */
/* コントロールチェンジ・プログラムチェンジ) */
long __stdcall MIDIEvent_SetNumber (MIDIEvent* pEvent, long lNum) {
	assert (pEvent);
	/* RPNチェンジ・NRPNチェンジの場合、CC#を切り替えるのは禁止 */
	if (MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent)) {
		return 0;
	}
	/* シーケンス番号の場合 */
	else if (MIDIEvent_IsSequenceNumber (pEvent)) {
		if (pEvent->m_lLen < 2) {
			return 0;
		}
		*(pEvent->m_pData + 0) = (unsigned char)(CLIP (0, lNum, 65535) >> 8);
		*(pEvent->m_pData + 1) = (unsigned char)(CLIP (0, lNum, 65535) & 0xFF);
		return 1;
	}
	/* チャンネルプリフィックスの場合 */
	else if (MIDIEvent_IsChannelPrefix (pEvent)) {
		if (pEvent->m_lLen < 1) {
			return 0;
		}
		*(pEvent->m_pData + 0) = (unsigned char)(CLIP (0, lNum, 15));
		return 1;
	}
	/* ポートプリフィックスの場合 */
	else if (MIDIEvent_IsPortPrefix (pEvent)) {
		if (pEvent->m_lLen < 1) {
			return 0;
		}
		*(pEvent->m_pData + 0) = (unsigned char)(CLIP (0, lNum, 255));
		return 1;
	}
	/* コントロールチェンジの場合 */
	else if (MIDIEvent_IsControlChange (pEvent)) {
		*(pEvent->m_pData + 1) = (unsigned char)(CLIP (0, lNum, 127));
		return 1;
	}
	/* プログラムチェンジの場合 */
	else if (MIDIEvent_IsProgramChange (pEvent)) {
		*(pEvent->m_pData + 1) = (unsigned char)(CLIP (0, lNum, 127));
		return 1;
	}
	return 0;
}

/* イベントの値取得(シーケンス番号・チャンネルプリフィックス・ポートプリフィックス・ */
/* キーアフター・コントロールチェンジ・プログラムチェンジ・チャンネルアフター・ピッチベンド) */
long __stdcall MIDIEvent_GetValue (MIDIEvent* pEvent) {
	assert (pEvent);
	/* シーケンス番号の場合 */
	if (MIDIEvent_IsSequenceNumber (pEvent)) {
		if (pEvent->m_lLen < 2) {
			return 0;
		}
		return (*(pEvent->m_pData) << 8) + *(pEvent->m_pData + 1);
	}
	/* チャンネルプリフィックス・ポートプリフィックスの場合 */
	else if (MIDIEvent_IsChannelPrefix (pEvent) || MIDIEvent_IsPortPrefix (pEvent)) {
		if (pEvent->m_lLen < 1) {
			return 0;
		}
		return *(pEvent->m_pData);
	}
	/* キーアフタータッチ・コントロールチェンジの場合 */
	else if (MIDIEvent_IsKeyAftertouch (pEvent) || MIDIEvent_IsControlChange (pEvent)) {
		return *(pEvent->m_pData + 2);
	}
	/* プログラムチェンジ・チャンネルアフタータッチの場合 */
	else if (MIDIEvent_IsProgramChange (pEvent) || MIDIEvent_IsChannelAftertouch (pEvent)) {
		return *(pEvent->m_pData + 1);
	}
	/* ピッチベンドの場合 */
	else if (MIDIEvent_IsPitchBend (pEvent)) {
		return *(pEvent->m_pData + 1) + (*(pEvent->m_pData + 2) << 7);
	}
	return 0;
}

/* イベントの値設定(シーケンス番号・チャンネルプリフィックス・ポートプリフィックス・ */
/* キーアフター・コントロールチェンジ・プログラムチェンジ・チャンネルアフター・ピッチベンド) */
long __stdcall MIDIEvent_SetValue (MIDIEvent* pEvent, long lVal) {
	assert (pEvent);
	/* シーケンス番号の場合 */
	if (MIDIEvent_IsSequenceNumber (pEvent)) {
		if (pEvent->m_lLen < 2) {
			return 0;
		}
		*(pEvent->m_pData + 0) = (unsigned char)(CLIP (0, lVal, 65535) >> 8);
		*(pEvent->m_pData + 1) = (unsigned char)(CLIP (0, lVal, 65535) & 0x00FF);
		return 1;
	}
	/* チャンネルプリフィックス */
	else if (MIDIEvent_IsChannelPrefix (pEvent)) {
		if (pEvent->m_lLen < 1) {
			return 0;
		}
		*(pEvent->m_pData + 0) = (unsigned char)(CLIP (0, lVal, 15));
		return 1;
	}
	/* ポートプリフィックスの場合 */
	else if (MIDIEvent_IsPortPrefix (pEvent)) {
		if (pEvent->m_lLen < 1) {
			return 0;
		}
		*(pEvent->m_pData + 0) = (unsigned char)(CLIP (0, lVal, 255));
		return 1;
	}
	/* キーアフタータッチ・コントロールチェンジの場合 */
	else if (MIDIEvent_IsKeyAftertouch (pEvent) || MIDIEvent_IsControlChange (pEvent)) {
		*(pEvent->m_pData + 2) = (unsigned char)(CLIP (0, lVal, 127));
		return 1;
	}
	/* プログラムチェンジ・チャンネルアフタータッチの場合 */
	else if (MIDIEvent_IsProgramChange (pEvent) || MIDIEvent_IsChannelAftertouch (pEvent)) {
		*(pEvent->m_pData + 1) = (unsigned char)(CLIP (0, lVal, 127));
		return 1;
	}
	/* ピッチベンドの場合 */
	else if (MIDIEvent_IsPitchBend (pEvent)) {
		*(pEvent->m_pData + 1) = (unsigned char)(CLIP (0, lVal, 16383) & 0x007F);
		*(pEvent->m_pData + 2) = (unsigned char)(CLIP (0, lVal, 16383) >> 7);
		return 1;
	}
	return 0;
}

/* 次のイベント取得(なければNULL) */
MIDIEvent* __stdcall MIDIEvent_GetNextEvent (MIDIEvent* pEvent) {
	return (pEvent->m_pNextEvent);
}

/* 前のイベント取得(なければNULL) */
MIDIEvent* __stdcall MIDIEvent_GetPrevEvent (MIDIEvent* pEvent) {
	return (pEvent->m_pPrevEvent);
}

/* 次の同じ種類のイベント取得(なければNULL) */
MIDIEvent* __stdcall MIDIEvent_GetNextSameKindEvent (MIDIEvent* pEvent) {
	return (pEvent->m_pNextSameKindEvent);
}

/* 前の同じ種類のイベント取得(なければNULL) */
MIDIEvent* __stdcall MIDIEvent_GetPrevSameKindEvent (MIDIEvent* pEvent) {
	return (pEvent->m_pPrevSameKindEvent);
}

/* 親トラックへのポインタを取得(なければNULL) */
MIDITrack* __stdcall MIDIEvent_GetParent (MIDIEvent* pMIDIEvent) {
	return (MIDITrack*)(pMIDIEvent->m_pParent);
}

/* イベントの種類文字列表現表(メタイベント)(ANSI) */
static char* g_szMetaKindNameA[] = {
	"SequenceNumber", "TextEvent", "CopyrightNotice", "TrackName",
	"InstrumentName", "Lyric", "Marker", "CuePoint",
	"ProgramName", "DeviceName", "", "", "", "", "", "", /* 0x00 ～ 0x0F */
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", /* 0x10 ～ 0x1F */
	"ChannelPrefix", "PortPrefix", "", "", "", "", "", "", 
	"", "", "", "", "", "", "", "EndofTrack", /* 0x20 ～ 0x2F */
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", /* 0x30 ～ 0x3F */
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", /* 0x40 ～ 0x4F */
    "", "Tempo", "", "", "SMPTEOffset", "", "", "",
	"TimeSignature", "KeySignature", "", "", "", "", "", "", /* 0x50 ～ 0x5F */
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", /* 0x60 ～ 0x6F */
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "SequencerSpec", /* 0x70 ～ 0x7F */
	"UnknownMetaEvent"};

/* イベントの種類文字列表現表(メタイベント)(UNICODE) */
static wchar_t* g_szMetaKindNameW[] = {
	L"SequenceNumber", L"TextEvent", L"CopyrightNotice", L"TrackName",
	L"InstrumentName", L"Lyric", L"Marker", L"CuePoint",
	L"ProgramName", L"DeviceName", L"", L"", L"", L"", L"", L"", /* 0x00 ～ 0x0F */
	L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", /* 0x10 ～ 0x1F */
	L"ChannelPrefix", L"PortPrefix", L"", L"", L"", L"", L"", L"", 
	L"", L"", L"", L"", L"", L"", L"", L"EndofTrack", /* 0x20 ～ 0x2F */
	L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", /* 0x30 ～ 0x3F */
	L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", /* 0x40 ～ 0x4F */
    L"", L"Tempo", L"", L"", L"SMPTEOffset", L"", L"", L"",
	L"TimeSignature", L"KeySignature", L"", L"", L"", L"", L"", L"", /* 0x50 ～ 0x5F */
	L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", /* 0x60 ～ 0x6F */
	L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"SequencerSpec", /* 0x70 ～ 0x7F */
	L"UnknownMetaEvent"};

/* イベントの種類文字列表現表(MIDIイベント)(ANSI) */
static char* g_szMIDIKindNameA[] = {
	"NoteOff", "NoteOn", "KeyAftertouch", "ControlChange",
	"ProgramChange", "ChannelAftertouch", "PitchBend"};

/* イベントの種類文字列表現表(MIDIイベント)(UNICODE) */
static wchar_t* g_szMIDIKindNameW[] = {
	L"NoteOff", L"NoteOn", L"KeyAftertouch", L"ControlChange",
	L"ProgramChange", L"ChannelAftertouch", L"PitchBend"};

/* イベントの種類文字列表現表(SYSEXイベント)(ANSI) */
static char* g_szSysExKindNameA[] = {
	"SysExStart", "", "", "", "", "", "", "SysExContinue"
};

/* イベントの種類文字列表現表(SYSEXイベント)(UNICODE) */
static wchar_t* g_szSysExKindNameW[] = {
	L"SysExStart", L"", L"", L"", L"", L"", L"", L"SysExContinue"
};

/* ノートキー文字列表現表(ANSI) */
static char* g_szKeyNameA[] =
		{"C_", "C#", "D_", "D#", "E_", "F_", "F#", "G_", "G#", "A_", "Bb", "B_"};

/* ノートキー文字列表現表(UNICODE) */
static wchar_t* g_szKeyNameW[] =
		{L"C_", L"C#", L"D_", L"D#", L"E_", L"F_", L"F#", L"G_", L"G#", L"A_", L"Bb", L"B_"};

/* MIDIイベントの内容を文字列表現に変換(ANSI) */
char* __stdcall MIDIEvent_ToStringA (MIDIEvent* pEvent, char* pBuf, long lLen) {
	return MIDIEvent_ToStringExA (pEvent, pBuf, lLen, MIDIEVENT_DUMPALL);
}

/* MIDIイベントの内容を文字列表現に変換(UNICODE) */
wchar_t* __stdcall MIDIEvent_ToStringW (MIDIEvent* pEvent, wchar_t* pBuf, long lLen) {
	return MIDIEvent_ToStringExW (pEvent, pBuf, lLen, MIDIEVENT_DUMPALL);
}

/* MIDIイベントの内容を文字列表現に変換(ANSI) */
char* __stdcall MIDIEvent_ToStringExA (MIDIEvent* pEvent, char* pBuf, long lLen, long lFlags) {
	char szText[2048];
	long lColumn = 0;
	long lOutput = 0;
	memset (szText, 0, sizeof (szText));
	memset (pBuf, 0, lLen);
	/* 時刻の出力 */
	if (lFlags & MIDIEVENT_DUMPTIME) {
		MIDITrack* pMIDITrack;
		MIDIData* pMIDIData;
		long lMeasure, lBeat, lTick;
		if (lColumn != 0) {
			lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, " ");
		}
		lColumn++;
		if ((pMIDITrack = (MIDITrack*)(pEvent->m_pParent))) {
			if ((pMIDIData = (MIDIData*)(pMIDITrack->m_pParent))) {
				long lMode = MIDIData_GetTimeMode (pMIDIData);
				long lResolution = MIDIData_GetTimeResolution (pMIDIData);
				if (lMode == MIDIDATA_TPQNBASE) {
					MIDITrack* pSetupTrack = (pMIDIData->m_lFormat == MIDIDATA_FORMAT2) ?
						pMIDITrack : pMIDIData->m_pFirstTrack;
					MIDITrack_BreakTime (pSetupTrack, pEvent->m_lTime, &lMeasure, &lBeat, &lTick);
					lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%05ld:%02ld:%03ld", lMeasure, lBeat, lTick);
				}
				else {
					lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%08ld:%03ld", 
						pEvent->m_lTime / lResolution, pEvent->m_lTime % lResolution);
				}
			}
			else {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%12ld", pEvent->m_lTime);
			}
		}
		else {
			lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%12ld", pEvent->m_lTime);
		}
		//lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%08d", pEvent->m_lTime);
	}
	/* 種類の出力 */
	if (lFlags & MIDIEVENT_DUMPKIND) {
		if (lColumn != 0) {
			lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, " ");
		}
		lColumn++;
		if (MIDIEvent_IsMetaEvent (pEvent)) {
			if (strcmp (g_szMetaKindNameA[pEvent->m_lKind], "") != 0) {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%-17.17s", g_szMetaKindNameA[pEvent->m_lKind]);
			}
			else {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "0x%02lX             ", pEvent->m_lKind);
			}
		}
		else if (MIDIEvent_IsMIDIEvent (pEvent)) {
			long lTempKind = pEvent->m_lKind;
			if (MIDIEvent_IsNoteOff (pEvent)) {
				lTempKind = (pEvent->m_lKind & 0x0F) | 0x80;
			}
			if (strcmp (g_szMIDIKindNameA[(lTempKind - 0x80) >> 4], "") != 0) {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%-17.17s", g_szMIDIKindNameA[(lTempKind - 0x80) >> 4]);
			}
			else {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "0x%02lX             ", lTempKind);
			}
		}
		else if (MIDIEvent_IsSysExEvent (pEvent)) {
			if (strcmp (g_szSysExKindNameA[pEvent->m_lKind - 0xF0], "") != 0) {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%-17.17s", g_szSysExKindNameA[pEvent->m_lKind - 0xF0]);
			}
			else {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "0x%02lX             ", pEvent->m_lKind);
			}
		}
		else {
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "0x%02lX             ", pEvent->m_lKind);
		}
	}
	/* データ部の長さの出力 */
	if (lFlags & MIDIEVENT_DUMPLEN) {
		if (lColumn != 0) {
			lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, " ");
		}
		lColumn++;
		lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "%4ld", pEvent->m_lLen);
	}
	/* データ部の出力 */
	if (lFlags & MIDIEVENT_DUMPDATA) {
		long i;
		long n;
		char* p1;
		unsigned char* p2;
		if (lColumn != 0) {
			lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, " ");
		}
		lColumn++;
		if (MIDIEVENT_TEXTEVENT <= pEvent->m_lKind && 
			pEvent->m_lKind < MIDIEVENT_CHANNELPREFIX) {
			n = CLIP (0, lLen, sizeof (szText) - 64);
			MIDIEvent_GetText (pEvent, szText, n);
			lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{\"%s\"}", szText);
		}
		else if ((MIDIEVENT_CHANNELPREFIX <= pEvent->m_lKind && 
			pEvent->m_lKind < MIDIEVENT_SEQUENCERSPECIFIC) ||
			pEvent->m_lKind >= MIDIEVENT_SYSEXSTART) {
			switch (pEvent->m_lKind) {
			case MIDIEVENT_CHANNELPREFIX:
			case MIDIEVENT_PORTPREFIX:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{%d}", *(pEvent->m_pData));
				break;
			case MIDIEVENT_ENDOFTRACK:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{}");
				break;
			case MIDIEVENT_TEMPO:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{%ld[microsec/beat](%ld[BPM])}", 
					MIDIEvent_GetTempo (pEvent), 60000000 / MIDIEvent_GetTempo (pEvent));
				break;
			case MIDIEVENT_SMPTEOFFSET:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{%d[h] %d[m] %d[s] %d[f] %d[ff]}", 
					*(pEvent->m_pData + 0), *(pEvent->m_pData + 1),
					*(pEvent->m_pData + 2), *(pEvent->m_pData + 3), *(pEvent->m_pData + 4));
				break;
			case MIDIEVENT_TIMESIGNATURE:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{%d / %d %d[clock/beat] %d[32divnote/beat]}", 
					*(pEvent->m_pData + 0), 1 << *(pEvent->m_pData + 1),
					*(pEvent->m_pData + 2), *(pEvent->m_pData + 3));
				break;
			case MIDIEVENT_KEYSIGNATURE:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{%d%s %s}",
					abs (*((char*)(pEvent->m_pData))), 
					*(pEvent->m_pData) == 0 ? "" : (*(pEvent->m_pData) < 128 ? "#" : "b"),
					*(pEvent->m_pData + 1) ? "minor" : "major");
				break;
			default:
				p1 = szText;
				p2 = pEvent->m_pData;
				n = CLIP (0, pEvent->m_lLen, 399);
				n = CLIP (0, pEvent->m_lLen, lLen - 64);
				for (i = 0; i < n; i++) {
					_snprintf (p1, 5, " 0x%02X", *p2++);
					p1 += 5;
				}
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{%s}", szText + 1);
				break;
			}
		}
		else if (MIDIEvent_IsMIDIEvent (pEvent)) {
			switch (pEvent->m_lKind & 0xF0) {
			case MIDIEVENT_NOTEOFF:
			case MIDIEVENT_NOTEON:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{0x%02X  %-2s%d %4d %9ld}",
				*(pEvent->m_pData), 
				g_szKeyNameA[*(pEvent->m_pData + 1) % 12],
				*(pEvent->m_pData + 1) / 12, *(pEvent->m_pData + 2),
				MIDIEvent_IsNote (pEvent) ? MIDIEvent_GetDuration (pEvent) : 0);
				break;
			case MIDIEVENT_KEYAFTERTOUCH:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{0x%02X  %-2s%d %4d}",
				*(pEvent->m_pData), g_szKeyNameA[*(pEvent->m_pData + 1) % 12],
				*(pEvent->m_pData + 1) / 12, *(pEvent->m_pData + 2));
				break;
			case MIDIEVENT_CONTROLCHANGE:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{0x%02X %4d %4d}",
				*(pEvent->m_pData), *(pEvent->m_pData + 1), *(pEvent->m_pData + 2));
				break;
			case MIDIEVENT_PROGRAMCHANGE:
			case MIDIEVENT_CHANNELAFTERTOUCH:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{0x%02X %4d}",
				*(pEvent->m_pData), *(pEvent->m_pData + 1));
				break;
			case MIDIEVENT_PITCHBEND:
				lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{0x%02X %9d}",
				*(pEvent->m_pData), *(pEvent->m_pData + 1) + *(pEvent->m_pData + 2) * 128);
				break;
			}
		}
		else if (MIDIEvent_IsSysExEvent (pEvent)) {
			p1 = szText;
			p2 = pEvent->m_pData;
			n = CLIP (0, pEvent->m_lLen, 399);
			n = CLIP (0, pEvent->m_lLen, lLen - 64);
			for (i = 0; i < n; i++) {
				_snprintf (p1, 5, " 0x%02X", *p2++);
				p1 += 5;
			}
			lOutput += _snprintf (pBuf + lOutput, lLen - lOutput, "{%s}", szText + 1);
		}
	}
	return pBuf;
}

/* MIDIイベントの内容を文字列表現に変換(UNICODE) */
wchar_t* __stdcall MIDIEvent_ToStringExW (MIDIEvent* pEvent, wchar_t* pBuf, long lLen, long lFlags) {
	wchar_t szText[2048];
	long lColumn = 0;
	long lOutput = 0;
	memset (szText, 0, WSIZEOF (szText));
	memset (pBuf, 0, lLen * sizeof (wchar_t));
	/* 時刻の出力 */
	if (lFlags & MIDIEVENT_DUMPTIME) {
		MIDITrack* pMIDITrack;
		MIDIData* pMIDIData;
		long lMeasure, lBeat, lTick;
		if (lColumn != 0) {
			lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L" ");
		}
		lColumn++;
		if ((pMIDITrack = (MIDITrack*)(pEvent->m_pParent))) {
			if ((pMIDIData = (MIDIData*)(pMIDITrack->m_pParent))) {
				long lMode = MIDIData_GetTimeMode (pMIDIData);
				long lResolution = MIDIData_GetTimeResolution (pMIDIData);
				if (lMode == MIDIDATA_TPQNBASE) {
					MIDITrack* pSetupTrack = (pMIDIData->m_lFormat == MIDIDATA_FORMAT2) ?
						pMIDITrack : pMIDIData->m_pFirstTrack;
					MIDITrack_BreakTime (pSetupTrack, pEvent->m_lTime, &lMeasure, &lBeat, &lTick);
					lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%05ld:%02ld:%03ld", lMeasure, lBeat, lTick);
				}
				else {
					lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%08ld:%03ld", 
						pEvent->m_lTime / lResolution, pEvent->m_lTime % lResolution);
				}
			}
			else {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%12ld", pEvent->m_lTime);
			}
		}
		else {
			lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%12ld", pEvent->m_lTime);
		}
		//lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%08d", pEvent->m_lTime);
	}
	/* 種類の出力 */
	if (lFlags & MIDIEVENT_DUMPKIND) {
		if (lColumn != 0) {
			lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L" ");
		}
		lColumn++;
		if (MIDIEvent_IsMetaEvent (pEvent)) {
			if (wcscmp (g_szMetaKindNameW[pEvent->m_lKind], L"") != 0) {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%-17.17s", g_szMetaKindNameW[pEvent->m_lKind]);
			}
			else {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"0x%02lX             ", pEvent->m_lKind);
			}
		}
		else if (MIDIEvent_IsMIDIEvent (pEvent)) {
			long lTempKind = pEvent->m_lKind;
			if (MIDIEvent_IsNoteOff (pEvent)) {
				lTempKind = (pEvent->m_lKind & 0x0F) | 0x80;
			}
			if (wcscmp (g_szMIDIKindNameW[(lTempKind - 0x80) >> 4], L"") != 0) {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%-17.17s", g_szMIDIKindNameW[(lTempKind - 0x80) >> 4]);
			}
			else {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"0x%02lX             ", lTempKind);
			}
		}
		else if (MIDIEvent_IsSysExEvent (pEvent)) {
			if (wcscmp (g_szSysExKindNameW[pEvent->m_lKind - 0xF0], L"") != 0) {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%-17.17s", g_szSysExKindNameW[pEvent->m_lKind - 0xF0]);
			}
			else {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"0x%02lX             ", pEvent->m_lKind);
			}
		}
		else {
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"0x%02lX             ", pEvent->m_lKind);
		}
	}
	/* データ部の長さの出力 */
	if (lFlags & MIDIEVENT_DUMPLEN) {
		if (lColumn != 0) {
			lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L" ");
		}
		lColumn++;
		lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"%4ld", pEvent->m_lLen);
	}
	/* データ部の出力 */
	if (lFlags & MIDIEVENT_DUMPDATA) {
		long i;
		long n;
		wchar_t* p1;
		unsigned char* p2;
		if (lColumn != 0) {
			lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L" ");
		}
		lColumn++;
		if (MIDIEVENT_TEXTEVENT <= pEvent->m_lKind && 
			pEvent->m_lKind < MIDIEVENT_CHANNELPREFIX) {
			n = CLIP (0, lLen, WSIZEOF (szText) - 64);
			MIDIEvent_GetTextW (pEvent, szText, n);
			lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{\"%s\"}", szText);
		}
		else if ((MIDIEVENT_CHANNELPREFIX <= pEvent->m_lKind && 
			pEvent->m_lKind < MIDIEVENT_SEQUENCERSPECIFIC) ||
			pEvent->m_lKind >= MIDIEVENT_SYSEXSTART) {
			switch (pEvent->m_lKind) {
			case MIDIEVENT_CHANNELPREFIX:
			case MIDIEVENT_PORTPREFIX:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{%d}", *(pEvent->m_pData));
				break;
			case MIDIEVENT_ENDOFTRACK:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{}");
				break;
			case MIDIEVENT_TEMPO:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{%ld[microsec/beat](%ld[BPM])}", 
					MIDIEvent_GetTempo (pEvent), 60000000 / MIDIEvent_GetTempo (pEvent));
				break;
			case MIDIEVENT_SMPTEOFFSET:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{%d[h] %d[m] %d[s] %d[f] %d[ff]}", 
					*(pEvent->m_pData + 0), *(pEvent->m_pData + 1),
					*(pEvent->m_pData + 2), *(pEvent->m_pData + 3), *(pEvent->m_pData + 4));
				break;
			case MIDIEVENT_TIMESIGNATURE:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{%d / %d %d[clock/beat] %d[32divnote/beat]}", 
					*(pEvent->m_pData + 0), 1 << *(pEvent->m_pData + 1),
					*(pEvent->m_pData + 2), *(pEvent->m_pData + 3));
				break;
			case MIDIEVENT_KEYSIGNATURE:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{%d%s %s}",
					abs (*((char*)(pEvent->m_pData))), 
					*(pEvent->m_pData) == 0 ? L"" : (*(pEvent->m_pData) < 128 ? L"#" : L"b"),
					*(pEvent->m_pData + 1) ? L"minor" : L"major");
				break;
			default:
				p1 = szText;
				p2 = pEvent->m_pData;
				n = CLIP (0, pEvent->m_lLen, 399);
				n = CLIP (0, pEvent->m_lLen, lLen - 64);
				for (i = 0; i < n; i++) {
					_snwprintf (p1, 5, L" 0x%02X", *p2++);
					p1 += 5;
				}
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{%s}", szText + 1);
				break;
			}
		}
		else if (MIDIEvent_IsMIDIEvent (pEvent)) {
			switch (pEvent->m_lKind & 0xF0) {
			case MIDIEVENT_NOTEOFF:
			case MIDIEVENT_NOTEON:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{0x%02X  %-2s%d %4d %9ld}",
				*(pEvent->m_pData), 
				g_szKeyNameW[*(pEvent->m_pData + 1) % 12],
				*(pEvent->m_pData + 1) / 12, *(pEvent->m_pData + 2),
				MIDIEvent_IsNote (pEvent) ? MIDIEvent_GetDuration (pEvent) : 0);
				break;
			case MIDIEVENT_KEYAFTERTOUCH:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{0x%02X  %-2s%d %4d}",
				*(pEvent->m_pData), g_szKeyNameW[*(pEvent->m_pData + 1) % 12],
				*(pEvent->m_pData + 1) / 12, *(pEvent->m_pData + 2));
				break;
			case MIDIEVENT_CONTROLCHANGE:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{0x%02X %4d %4d}",
				*(pEvent->m_pData), *(pEvent->m_pData + 1), *(pEvent->m_pData + 2));
				break;
			case MIDIEVENT_PROGRAMCHANGE:
			case MIDIEVENT_CHANNELAFTERTOUCH:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{0x%02X %4d}",
				*(pEvent->m_pData), *(pEvent->m_pData + 1));
				break;
			case MIDIEVENT_PITCHBEND:
				lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{0x%02X %9d}",
				*(pEvent->m_pData), *(pEvent->m_pData + 1) + *(pEvent->m_pData + 2) * 128);
				break;
			}
		}
		else if (MIDIEvent_IsSysExEvent (pEvent)) {
			p1 = szText;
			p2 = pEvent->m_pData;
			n = CLIP (0, pEvent->m_lLen, 399);
			n = CLIP (0, pEvent->m_lLen, lLen - 64);
			for (i = 0; i < n; i++) {
				_snwprintf (p1, 5, L" 0x%02X", *p2++);
				p1 += 5;
			}
			lOutput += _snwprintf (pBuf + lOutput, lLen - lOutput, L"{%s}", szText + 1);

		}
	}
	return pBuf;
}


/******************************************************************************/
/*                                                                            */
/*　MIDITrackクラス関数                                                       */
/*                                                                            */
/******************************************************************************/

/* トラック内のイベントの総数を取得 */
long __stdcall MIDITrack_GetNumEvent (MIDITrack* pTrack) {
	return pTrack->m_lNumEvent;
}

/* トラックの最初のイベント取得(なければNULL) */
MIDIEvent* __stdcall MIDITrack_GetFirstEvent (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_pFirstEvent;
}

/* トラックの最後のイベント取得(なければNULL) */
/* 通常、最後のイベントはEOT(エンドオブトラック)である */
MIDIEvent* __stdcall MIDITrack_GetLastEvent (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_pLastEvent;
}

/* トラック内の指定種類の最初のイベント取得(なければNULL) */
MIDIEvent* __stdcall MIDITrack_GetFirstKindEvent (MIDITrack* pTrack, long lKind) {
	MIDIEvent* pEvent = NULL;
	assert (pTrack);
	forEachEvent (pTrack, pEvent) {
		if (pEvent->m_lKind == lKind) {
			return pEvent;
		}
	}
	return NULL;
}

/* トラック内の指定種類の最後のイベント取得(なければNULL) */
MIDIEvent* __stdcall MIDITrack_GetLastKindEvent (MIDITrack* pTrack, long lKind) {
	MIDIEvent* pEvent = NULL;
	assert (pTrack);
	forEachEventInverse (pTrack, pEvent) {
		if (pEvent->m_lKind == lKind) {
			return pEvent;
		}
	}
	return NULL;
}

/* 次のMIDIトラックへのポインタ取得(なければNULL)(20080715追加) */
MIDITrack* __stdcall MIDITrack_GetNextTrack (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_pNextTrack;
}

/* 前のMIDIトラックへのポインタ取得(なければNULL)(20080715追加) */
MIDITrack* __stdcall MIDITrack_GetPrevTrack (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_pPrevTrack;
}


/* 親MIDIデータへのポインタを取得(なければNULL) */
MIDIData* __stdcall MIDITrack_GetParent (MIDITrack* pMIDITrack) {
	return (MIDIData*)(pMIDITrack->m_pParent);
}


/* トラック内のイベント数をカウントし、各イベントのインデックスと総イベント数を更新し、イベント数を返す。 */
long __stdcall MIDITrack_CountEvent (MIDITrack* pTrack) {
	long i = 0;
	MIDIEvent* pEvent;
	assert (pTrack);
	forEachEvent (pTrack, pEvent) {
		pEvent->m_lTempIndex = i;
		i++;
	}
	return pTrack->m_lNumEvent = i;
}

/* トラックの開始時刻(最初のイベントの時刻)[Tick]を取得(20081101追加) */
long __stdcall MIDITrack_GetBeginTime (MIDITrack* pMIDITrack) {
	MIDIEvent* pFirstEvent = pMIDITrack->m_pFirstEvent;
	if (pFirstEvent) {
		return pFirstEvent->m_lTime;
	}
	return 0;
}

/* トラックの終了時刻(最後のイベントの時刻)[Tick]を取得(20081101追加) */
long __stdcall MIDITrack_GetEndTime (MIDITrack* pMIDITrack) {
	MIDIEvent* pLastEvent = pMIDITrack->m_pLastEvent;
	if (pLastEvent) {
		return pLastEvent->m_lTime;
	}
	return 0;
}

/* トラック名取得(トラック名イベントがなければNULL) */
char* __stdcall MIDITrack_GetNameA (MIDITrack* pTrack, char* pBuf, long lLen) {
	MIDIEvent* pEvent;
	memset (pBuf, '\0', lLen);
	forEachEvent (pTrack, pEvent) {
		if (pEvent->m_lKind == MIDIEVENT_TRACKNAME) {
			return MIDIEvent_GetTextA (pEvent, pBuf, lLen);
		}
	}
	return NULL;
}

/* トラック名取得(トラック名イベントがなければNULL) */
wchar_t* __stdcall MIDITrack_GetNameW (MIDITrack* pTrack, wchar_t* pBuf, long lLen) {
	MIDIEvent* pEvent;
	memset (pBuf, '\0', lLen * sizeof (wchar_t));
	forEachEvent (pTrack, pEvent) {
		if (pEvent->m_lKind == MIDIEVENT_TRACKNAME) {
			return MIDIEvent_GetTextW (pEvent, pBuf, lLen);
		}
	}
	return NULL;
}


/* 入力取得(0=OFF, 1=On) */
long __stdcall MIDITrack_GetInputOn (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lInputOn;
}

/* 入力ポート取得(-1=n/a, 0～15=ポート番号) */
long __stdcall MIDITrack_GetInputPort (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lInputPort;
}

/* 入力チャンネル取得(-1=n/a, 0～15=チャンネル番号) */
long __stdcall MIDITrack_GetInputChannel (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lInputChannel;
}

/* 出力取得(0=OFF, 1=On) */
long __stdcall MIDITrack_GetOutputOn (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lOutputOn;
}

/* 出力ポート(-1=n/a, 0～15=ポート番号) */
long __stdcall MIDITrack_GetOutputPort (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lOutputPort;
}

/* 出力チャンネル(-1=n/a, 0～15=チャンネル番号) */
long __stdcall MIDITrack_GetOutputChannel (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lOutputChannel;
}

/* タイム+取得 */
long __stdcall MIDITrack_GetTimePlus (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lTimePlus;
}

/* キー+取得 */
long __stdcall MIDITrack_GetKeyPlus (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lKeyPlus;
}

/* ベロシティ+取得 */
long __stdcall MIDITrack_GetVelocityPlus (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lVelocityPlus;
}

/* 表示モード取得(0=通常、1=ドラム) */
long __stdcall MIDITrack_GetViewMode (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lViewMode;
}

/* 前景色取得 */
long __stdcall MIDITrack_GetForeColor (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lForeColor;
}

/* 背景色取得 */
long __stdcall MIDITrack_GetBackColor (MIDITrack* pTrack) {
	assert (pTrack);
	return pTrack->m_lBackColor;
}


/* トラック名設定(トラック名イベントがなければ自動追加)(ANSI) */
long __stdcall MIDITrack_SetNameA (MIDITrack* pTrack, const char* pszText) {
	MIDIEvent* pEvent;
	assert (pTrack);
	forEachEvent (pTrack, pEvent) {
		if (pEvent->m_lKind == MIDIEVENT_TRACKNAME) {
			MIDIEvent_SetTextA (pEvent, pszText);
			return 1;
		}
	}
	return MIDITrack_InsertTrackNameA (pTrack, 0, pszText);
}

/* トラック名設定(トラック名イベントがなければ自動追加)(UNICODE) */
long __stdcall MIDITrack_SetNameW (MIDITrack* pTrack, const wchar_t* pszText) {
	MIDIEvent* pEvent;
	assert (pTrack);
	forEachEvent (pTrack, pEvent) {
		if (pEvent->m_lKind == MIDIEVENT_TRACKNAME) {
			MIDIEvent_SetTextW (pEvent, pszText);
			return 1;
		}
	}
	return MIDITrack_InsertTrackNameW (pTrack, 0, pszText);
}

/* 入力設定(0=OFF, 1=On) */
long __stdcall MIDITrack_SetInputOn (MIDITrack* pTrack, long lInputOn) {
	assert (pTrack);
	pTrack->m_lInputOn = lInputOn;
	return 1;
}

/* 入力ポート設定(-1=n/a, 0～15=ポート番号) */
long __stdcall MIDITrack_SetInputPort (MIDITrack* pTrack, long lInputPort) {
	assert (pTrack);
	pTrack->m_lInputPort = CLIP (0, lInputPort, 255);
	return 1;
}

/* 入力チャンネル設定(-1=n/a, 0～15=チャンネル番号) */
long __stdcall MIDITrack_SetInputChannel (MIDITrack* pTrack, long lInputChannel) {
	assert (pTrack);
	pTrack->m_lInputChannel = CLIP (-1, lInputChannel, 15);
	return 1;
}

/* 出力設定(0=OFF, 1=On) */
long __stdcall MIDITrack_SetOutputOn (MIDITrack* pTrack, long lOutputOn) {
	assert (pTrack);
	pTrack->m_lOutputOn = lOutputOn;
	return 1;
}

/* 出力ポート(-1=n/a, 0～15=ポート番号) */
long __stdcall MIDITrack_SetOutputPort (MIDITrack* pTrack, long lOutputPort) {
	assert (pTrack);
	pTrack->m_lOutputPort = CLIP (0, lOutputPort, 255);
	return 1;
}

/* 出力チャンネル(-1=n/a, 0～15=チャンネル番号) */
long __stdcall MIDITrack_SetOutputChannel (MIDITrack* pTrack, long lOutputChannel) {
	assert (pTrack);
	pTrack->m_lOutputChannel = CLIP (-1, lOutputChannel, 15);
	return 1;
}

/* タイム+設定 */
long __stdcall MIDITrack_SetTimePlus (MIDITrack* pTrack, long lTimePlus) {
	assert (pTrack);
	pTrack->m_lTimePlus = lTimePlus;
	return 1;
}

/* キー+設定 */
long __stdcall MIDITrack_SetKeyPlus (MIDITrack* pTrack, long lKeyPlus) {
	assert (pTrack);
	pTrack->m_lKeyPlus = lKeyPlus;
	return 1;
}

/* ベロシティ+設定 */
long __stdcall MIDITrack_SetVelocityPlus (MIDITrack* pTrack, long lVelocityPlus) {
	assert (pTrack);
	pTrack->m_lVelocityPlus = lVelocityPlus;
	return 1;
}

/* 表示モード設定(0=通常、1=ドラム) */
long __stdcall MIDITrack_SetViewMode (MIDITrack* pTrack, long lViewMode) {
	assert (pTrack);
	pTrack->m_lViewMode = lViewMode;
	return 1;
}

/* 前景色設定 */
long __stdcall MIDITrack_SetForeColor (MIDITrack* pTrack, long lForeColor) {
	assert (pTrack);
	pTrack->m_lForeColor = lForeColor;
	return 1;
}

/* 背景色設定 */
long __stdcall MIDITrack_SetBackColor (MIDITrack* pTrack, long lBackColor) {
	assert (pTrack);
	pTrack->m_lBackColor = lBackColor;
	return 1;
}

/* XFであるとき、XFのヴァージョンを取得(XFでなければ0) */
long __stdcall MIDITrack_GetXFVersion (MIDITrack* pMIDITrack) {
	MIDIEvent* pMIDIEvent = NULL;
	/* シーケンサ固有のイベントで */
	/* {43 7B 00 58 46 Mj Mn S1 S0} を探し、*/
	/* Mj | (Mn << 8) | (S1 << 16) | (S0 << 24) を返す。*/
	forEachEvent (pMIDITrack, pMIDIEvent) {
		if (MIDIEvent_IsSequencerSpecific (pMIDIEvent)) {
			if (MIDIEvent_GetLen (pMIDIEvent) == 9) {
				if (*(pMIDIEvent->m_pData + 0) == 0x43 &&
					*(pMIDIEvent->m_pData + 1) == 0x7B &&
					*(pMIDIEvent->m_pData + 2) == 0x00 &&
					*(pMIDIEvent->m_pData + 3) == 0x58 &&
					*(pMIDIEvent->m_pData + 4) == 0x46) {
					return 
						(*(pMIDIEvent->m_pData + 5) << 0) |
						(*(pMIDIEvent->m_pData + 6) << 8) |	
						(*(pMIDIEvent->m_pData + 7) << 16) |	
						(*(pMIDIEvent->m_pData + 8) << 24);
				}
			}
		}
	}
	/* XFではない */
	return 0;
}




/* トラックの削除(トラック内に含まれるイベントオブジェクトも削除されます) */
void __stdcall MIDITrack_Delete (MIDITrack* pTrack) {
	MIDIEvent* pEvent;
	MIDIEvent* pNextEvent;
	assert (pTrack);
	/* トラック内のイベント削除 */
	pEvent = MIDITrack_GetFirstEvent (pTrack);
	while (pEvent) {
		pNextEvent = MIDIEvent_GetNextEvent (pEvent);
		MIDIEvent_DeleteSingle (pEvent);
		pEvent = pNextEvent;
	}
	/* 双方向リストポインタのつなぎかえ */
	if (pTrack->m_pNextTrack) {
		pTrack->m_pNextTrack->m_pPrevTrack = pTrack->m_pPrevTrack;
	}
	else if (pTrack->m_pParent) {
		((MIDIData*)(pTrack->m_pParent))->m_pLastTrack = pTrack->m_pPrevTrack;
	}

	if (pTrack->m_pPrevTrack) {
		pTrack->m_pPrevTrack->m_pNextTrack = pTrack->m_pNextTrack;
	}
	else if (pTrack->m_pParent) {
		((MIDIData*)(pTrack->m_pParent))->m_pFirstTrack = pTrack->m_pNextTrack;
	}

	if (pTrack->m_pParent) {
		((MIDIData*)(pTrack->m_pParent))->m_lNumTrack --;
		pTrack->m_pParent = NULL;
	}
	free (pTrack);
}

/* 空のトラックの生成 */
/* エンドオブトラックイベントは手動で付加しなければならない。*/
MIDITrack* __stdcall MIDITrack_Create () {
	MIDITrack* pTrack = NULL;
	pTrack = calloc (1, sizeof (MIDITrack));
	if (pTrack == NULL) {
		return NULL;
	}
	pTrack->m_lNumEvent = 0;
	pTrack->m_pFirstEvent = NULL;
	pTrack->m_pLastEvent = NULL;
	pTrack->m_pNextTrack = NULL;
	pTrack->m_pPrevTrack = NULL;
	pTrack->m_pParent = NULL;
	pTrack->m_lInputOn = 1;
	pTrack->m_lInputPort = 0;
	pTrack->m_lInputChannel = 0;
	pTrack->m_lOutputOn = 1;
	pTrack->m_lOutputPort = 0;
	pTrack->m_lOutputChannel = 0;
	pTrack->m_lTimePlus = 0;
	pTrack->m_lKeyPlus = 0;
	pTrack->m_lVelocityPlus = 0;
	pTrack->m_lViewMode = 0;
	pTrack->m_lForeColor = 0x00000000;
	pTrack->m_lBackColor = 0x00FFFFFF;
	pTrack->m_lUser1 = 0;
	pTrack->m_lUser2 = 0;
	pTrack->m_lUser3 = 0;
	pTrack->m_lUserFlag = 0;
	return pTrack;
}

/* MIDIトラックのクローンを生成 */
MIDITrack* __stdcall MIDITrack_CreateClone (MIDITrack* pTrack) {
	MIDITrack* pCloneTrack = NULL;
	MIDIEvent* pSrcEvent = NULL;
	MIDIEvent* pCloneEvent = NULL;
	long lCount = MIDITrack_CountEvent (pTrack);
	pCloneTrack = MIDITrack_Create ();
	if (pCloneTrack == NULL) {
		return NULL;
	}
	forEachEvent (pTrack, pSrcEvent) {
		if (pSrcEvent->m_pPrevCombinedEvent == NULL) {
			pCloneEvent = MIDIEvent_CreateClone (pSrcEvent);
			if (pCloneEvent == NULL) {
				MIDITrack_Delete (pCloneTrack);
				return NULL;
			}
			MIDITrack_InsertEvent (pCloneTrack, pCloneEvent);
		}
	}
	/* TODO イベントの順序が混乱しているので並べ替えよ(m_lTempIndexの値を用いよ) */
	pCloneTrack->m_lInputOn = pTrack->m_lInputOn;
	pCloneTrack->m_lInputPort = pTrack->m_lInputPort;
	pCloneTrack->m_lInputChannel = pTrack->m_lInputChannel;
	pCloneTrack->m_lOutputOn = pTrack->m_lOutputOn;
	pCloneTrack->m_lOutputPort = pTrack->m_lOutputPort;
	pCloneTrack->m_lOutputChannel = pTrack->m_lOutputChannel;
	pCloneTrack->m_lTimePlus = pTrack->m_lTimePlus;
	pCloneTrack->m_lKeyPlus = pTrack->m_lKeyPlus;
	pCloneTrack->m_lVelocityPlus = pTrack->m_lVelocityPlus;
	pCloneTrack->m_lViewMode = pTrack->m_lViewMode;
	pCloneTrack->m_lForeColor = pTrack->m_lForeColor;
	pCloneTrack->m_lBackColor = pTrack->m_lBackColor;
	pCloneTrack->m_lReserved1 = pTrack->m_lReserved1;
	pCloneTrack->m_lReserved2 = pTrack->m_lReserved2;
	pCloneTrack->m_lReserved3 = pTrack->m_lReserved3;
	pCloneTrack->m_lReserved4 = pTrack->m_lReserved4;
	pCloneTrack->m_lUser1 = pTrack->m_lUser1;
	pCloneTrack->m_lUser2 = pTrack->m_lUser2;
	pCloneTrack->m_lUser3 = pTrack->m_lUser3;
	pCloneTrack->m_lUserFlag = pTrack->m_lUserFlag;
	return pCloneTrack;
}

/* トラックにノートオフイベントを正しく挿入 */
/* 結合しているノートオンイベントは既に挿入済みとする。 */
/* 同時刻にノートオフイベントがある場合はそれらの直前に挿入する */
/* (この関数は内部隠蔽されています。) */
long MIDITrack_InsertNoteOffEventBefore (MIDITrack* pTrack, MIDIEvent* pNoteOffEvent) {
	MIDIEvent* pOldEvent = NULL;
	MIDIEvent* pTempEvent = NULL;
	MIDIEvent* pNoteOnEvent = pNoteOffEvent->m_pPrevCombinedEvent;
	assert (pTrack);
	assert (pNoteOnEvent);
	assert (!MIDIEvent_IsFloating (pNoteOnEvent));
	pOldEvent = pNoteOnEvent;
	pTempEvent = pNoteOnEvent->m_pNextEvent;
	while (pTempEvent) {
		if (pTempEvent->m_lKind == MIDIEVENT_ENDOFTRACK && 
			pTempEvent->m_pNextEvent == NULL) {
			pTempEvent->m_lTime = pNoteOffEvent->m_lTime;
			break;
		}
		else if (pTempEvent->m_lTime >= pNoteOffEvent->m_lTime) {
			break;
		}
		pOldEvent = pTempEvent;
		pTempEvent = pTempEvent->m_pNextEvent;
	}
	return MIDIEvent_SetNextEvent (pOldEvent, pNoteOffEvent);
}


/* トラックにノートオフイベントを正しく挿入 */
/* 結合しているノートオンイベントは既に挿入済みとする。 */
/* 同時刻にノートオフイベントがある場合はそれらの直後に挿入する */
/* (この関数は内部隠蔽されています。) */
long MIDITrack_InsertNoteOffEventAfter (MIDITrack* pTrack, MIDIEvent* pNoteOffEvent) {
	MIDIEvent* pOldEvent = NULL;
	MIDIEvent* pTempEvent = NULL;
	MIDIEvent* pNoteOnEvent = pNoteOffEvent->m_pPrevCombinedEvent;
	assert (pTrack);
	assert (pNoteOnEvent);
	assert (!MIDIEvent_IsFloating (pNoteOnEvent));
	pOldEvent = pNoteOnEvent;
	pTempEvent = pNoteOnEvent->m_pNextEvent;
	while (pTempEvent) {
		if (pTempEvent->m_lKind == MIDIEVENT_ENDOFTRACK && 
			pTempEvent->m_pNextEvent == NULL) {
			pTempEvent->m_lTime = pNoteOffEvent->m_lTime;
			break;
		}
		else if (pTempEvent->m_lTime > pNoteOffEvent->m_lTime ||
			(pTempEvent->m_lTime == pNoteOffEvent->m_lTime &&
			!MIDIEvent_IsNoteOff (pTempEvent))) {
			break;
		}
		pOldEvent = pTempEvent;
		pTempEvent = pTempEvent->m_pNextEvent;
	}
	return MIDIEvent_SetNextEvent (pOldEvent, pNoteOffEvent);
}


/* トラックに単一のイベントを挿入 */
/* pEventをpTargetの直前に入れる。時刻が不正な場合、自動訂正する。*/
/* pTarget==NULLの場合、トラックの最後に入れる。 */
/* (この関数は内部隠蔽されています。) */
long __stdcall MIDITrack_InsertSingleEventBefore 
(MIDITrack* pTrack, MIDIEvent* pEvent, MIDIEvent* pTarget) {
	assert (pTrack);
	assert (pEvent);
	/* イベントが既に他のトラックに属している場合、却下する */
	if (pEvent->m_pParent || pEvent->m_pPrevEvent || pEvent->m_pNextEvent) {
		return 0;
	}
	/* EOTを二重に入れるのを防止 */
	if (pTrack->m_pLastEvent) {
		if (pTrack->m_pLastEvent->m_lKind == MIDIEVENT_ENDOFTRACK &&
			pEvent->m_lKind == MIDIEVENT_ENDOFTRACK) {
			return 0;
		}
	}
	/* SMFフォーマット1の場合 */
	if (pTrack->m_pParent) {
		if (((MIDIData*)(pTrack->m_pParent))->m_lFormat == 1) {
			/* コンダクタートラックにMIDIEventを入れるのを防止 */
			if (((MIDIData*)(pTrack->m_pParent))->m_pFirstTrack == pTrack) {
				if (MIDIEvent_IsMIDIEvent (pEvent)) {
					return 0;
				}
			}
			/* 非コンダクタートラックにテンポ・拍子などを入れるのを防止 */
			else {
				if (pEvent->m_lKind == MIDIEVENT_TEMPO ||
					pEvent->m_lKind == MIDIEVENT_SMPTEOFFSET ||
					pEvent->m_lKind == MIDIEVENT_TIMESIGNATURE ||
					pEvent->m_lKind == MIDIEVENT_KEYSIGNATURE) {
					return 0;
				}
			}
		}
	}
	/* pTargetの直前に挿入する場合 */
	if (pTarget) {
		/* ターゲットが所属トラックが異なる場合却下 */
		if (pTarget->m_pParent != pTrack) {
			assert (0);
			return 0;
		}
		MIDIEvent_SetPrevEvent (pTarget, pEvent);
	}
	/* トラックの最後に挿入する場合(pTarget==NULL) */
	else if (pTrack->m_pLastEvent) {
		/* EOTの後に挿入しようとした場合、EOTを後ろに移動しEOTの直前に挿入 */
		if (pTrack->m_pLastEvent->m_lKind == MIDIEVENT_ENDOFTRACK) {
			/* EOTを正しく移動するため、先に時刻の整合調整 */
			if (pTrack->m_pLastEvent->m_lTime < pEvent->m_lTime) {
				pTrack->m_pLastEvent->m_lTime = pEvent->m_lTime;
			}
			MIDIEvent_SetPrevEvent (pTrack->m_pLastEvent, pEvent);
		}
		/* EOT以外の後に挿入しようとした場合、普通に挿入 */
		else {
			MIDIEvent_SetNextEvent (pTrack->m_pLastEvent, pEvent);
		}
	}
	/* 空トラックに挿入する場合 */
	else {
		pEvent->m_pParent = pTrack;
		pEvent->m_pNextEvent = NULL;
		pEvent->m_pPrevEvent = NULL;
		pEvent->m_pNextSameKindEvent = NULL;
		pEvent->m_pPrevSameKindEvent = NULL;
		pTrack->m_pFirstEvent = pEvent;
		pTrack->m_pLastEvent = pEvent;
		pTrack->m_lNumEvent ++;
	}
	return 1;
}

/* トラックにイベントを挿入(結合イベントにも対応) */
/* pEventをpTargetの直前に入れる。時刻が不正な場合、自動訂正する。*/
/* pTarget==NULLの場合、トラックの最後に入れる。 */
long __stdcall MIDITrack_InsertEventBefore (MIDITrack* pTrack, MIDIEvent* pEvent, MIDIEvent* pTarget) {
	long lRet = 0;
	assert (pTrack);
	assert (pEvent);
	/* 非浮遊イベントは挿入できない。 */
	if (!MIDIEvent_IsFloating (pEvent)) {
		return 0;
	}
	pEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	/* ノートイベント以外の結合イベントの間には挿入できない */
	if (pTarget) {
		if (!MIDIEvent_IsNote (pTarget)) {
			pTarget = MIDIEvent_GetFirstCombinedEvent (pTarget);
		}
	}
	/* 単独のイベントの場合 */
	if (MIDIEvent_IsCombined (pEvent) == 0) {
		lRet = MIDITrack_InsertSingleEventBefore (pTrack, pEvent, pTarget);
		return lRet;
	}
	/* ノートイベントの場合 */
	else if (MIDIEvent_IsNote (pEvent)) {
		lRet = MIDITrack_InsertSingleEventBefore (pTrack, pEvent, pTarget);
		if (lRet == 0) {
			return 0;
		}
		lRet = MIDITrack_InsertNoteOffEventBefore (pTrack, pEvent->m_pNextCombinedEvent);
		if (lRet == 0) {
			MIDITrack_RemoveSingleEvent (pTrack, pEvent);
			return 0;
		}
		return 2;
	}
	/* RPNチェンジ又はNPRNチェンジ又はパッチチェンジの場合 */
	else if (MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent) ||
	MIDIEvent_IsPatchChange (pEvent)) {
		lRet = MIDITrack_InsertSingleEventBefore (pTrack, pEvent, pTarget);
		if (lRet == 0) {
			return 0;
		}
		lRet = MIDITrack_InsertSingleEventBefore (pTrack, pEvent->m_pNextCombinedEvent, pTarget);
		if (lRet == 0) {
			MIDITrack_RemoveSingleEvent (pTrack, pEvent);
			return 0;
		}
		lRet = MIDITrack_InsertSingleEventBefore (pTrack, pEvent->m_pNextCombinedEvent->m_pNextCombinedEvent, pTarget);
		if (lRet == 0) {
			MIDITrack_RemoveSingleEvent (pTrack, pEvent);
			MIDITrack_RemoveSingleEvent (pTrack, pEvent->m_pNextCombinedEvent);
			return 0;
		}
		return 3;
	}
	/* 未定義の結合イベント */
	assert (0);
	return 0;
}



/* トラックにイベントを挿入(イベントはあらかじめ生成しておく) */
/* pEventをpTargetの直後に入れる。時刻が不正な場合、自動訂正する。*/
/* pTarget==NULLの場合、トラックの最初に入れる。 */
long __stdcall MIDITrack_InsertSingleEventAfter
	(MIDITrack* pTrack, MIDIEvent* pEvent, MIDIEvent* pTarget) {
	assert (pTrack);
	assert (pEvent);
	/* イベントが既に他のトラックに属している場合、却下する */
	if (pEvent->m_pParent || pEvent->m_pPrevEvent || pEvent->m_pNextEvent) {
		return 0;
	}
	/* EOTを二重に入れるのを防止 */
	if (pTrack->m_pLastEvent) {
		if (pTrack->m_pLastEvent->m_lKind == MIDIEVENT_ENDOFTRACK &&
			pEvent->m_lKind == MIDIEVENT_ENDOFTRACK) {
			return 0;
		}
	}
	/* SMFフォーマット1の場合 */
	if (pTrack->m_pParent) {
		if (((MIDIData*)(pTrack->m_pParent))->m_lFormat == 1) {
			/* コンダクタートラックにMIDIEventを入れるのを防止 */
			if (((MIDIData*)(pTrack->m_pParent))->m_pFirstTrack == pTrack) {
				if (MIDIEvent_IsMIDIEvent (pEvent)) {
					return 0;
				}
			}
			/* 非コンダクタートラックにテンポ・拍子などを入れるのを防止 */
			else {
				if (pEvent->m_lKind == MIDIEVENT_TEMPO ||
					pEvent->m_lKind == MIDIEVENT_SMPTEOFFSET ||
					pEvent->m_lKind == MIDIEVENT_TIMESIGNATURE ||
					pEvent->m_lKind == MIDIEVENT_KEYSIGNATURE) {
					return 0;
				}
			}
		}
	}
	
	/* pTargetの直後に挿入する場合 */
	if (pTarget) {
		/* ターゲットが所属トラックが異なる場合却下 */
		if (pTarget->m_pParent != pTrack) {
			assert (0);
			return 0;
		}
		/* EOTの直後に挿入しようとした場合、EOTを移動しEOTの直前に挿入 */
		if (pTarget->m_lKind == MIDIEVENT_ENDOFTRACK &&
			pTarget->m_pNextEvent == NULL) {
			/* EOTを正しく移動するため、先に時刻の整合調整 */
			if (pTarget->m_lTime < pEvent->m_lTime) {
				pTarget->m_lTime = pEvent->m_lTime;
			}
			MIDIEvent_SetPrevEvent (pTarget, pEvent);
		}
		/* EOT以外の直後に挿入しようとした場合、時刻の整合さえすれば可能(pTarget==NULL) */
		else {
			if (pTrack->m_pLastEvent->m_lKind == MIDIEVENT_ENDOFTRACK) {
				if (pTrack->m_pLastEvent->m_lTime < pEvent->m_lTime) {
					pTrack->m_pLastEvent->m_lTime = pEvent->m_lTime;
				}
			}
			MIDIEvent_SetNextEvent (pTarget, pEvent);
		}
	}
	/* トラックの最初に挿入する場合(pTarget==NULL) */
	else if (pTrack->m_pFirstEvent) {
		/* EOTの直前となる場合は、EOTの時刻を調整する */
		if (pTrack->m_pFirstEvent->m_lKind == MIDIEVENT_ENDOFTRACK &&
			pTrack->m_pFirstEvent->m_pNextEvent == NULL) {
			if (pTrack->m_pFirstEvent->m_lTime < pEvent->m_lTime) {
				pTrack->m_pFirstEvent->m_lTime = pEvent->m_lTime;
			}
		}
		MIDIEvent_SetPrevEvent (pTrack->m_pFirstEvent, pEvent);
	}
	/* 空トラックに挿入する場合 */
	else {
		pEvent->m_pParent = pTrack;
		pEvent->m_pNextEvent = NULL;
		pEvent->m_pPrevEvent = NULL;
		pEvent->m_pNextSameKindEvent = NULL;
		pEvent->m_pPrevSameKindEvent = NULL;
		pTrack->m_pFirstEvent = pEvent;
		pTrack->m_pLastEvent = pEvent;
		pTrack->m_lNumEvent ++;
	}
	return 1;
}

/* トラックにイベントを挿入(結合イベントにも対応) */
/* pEventをpTargetの直前に入れる。時刻が不正な場合、自動訂正する。*/
/* pTarget==NULLの場合、トラックの最後に入れる。 */
/* (この関数は内部隠蔽されています。) */
long __stdcall MIDITrack_InsertEventAfter (MIDITrack* pTrack, MIDIEvent* pEvent, MIDIEvent* pTarget) {
	long lRet = 0;
	assert (pTrack);
	assert (pEvent);
	/* 非浮遊イベントは挿入できない。 */
	if (!MIDIEvent_IsFloating (pEvent)) {
		return 0;
	}
	pEvent = MIDIEvent_GetLastCombinedEvent (pEvent);
	/* ノートイベント以外の結合イベントの間には挿入できない */
	if (pTarget) {
		if (!MIDIEvent_IsNote (pTarget)) {
			pTarget = MIDIEvent_GetLastCombinedEvent (pTarget);
		}
	}
	/* 単独のイベントの場合 */
	if (!MIDIEvent_IsCombined (pEvent)) {
		lRet = MIDITrack_InsertSingleEventAfter (pTrack, pEvent, pTarget);
		return lRet;
	}
	/* ノートイベントの場合 */
	else if (MIDIEvent_IsNote (pEvent)) {
		lRet = MIDITrack_InsertSingleEventAfter (pTrack, pEvent->m_pPrevCombinedEvent, pTarget);
		if (lRet == 0) {
			return 0;
		}
		lRet = MIDITrack_InsertNoteOffEventAfter (pTrack, pEvent);
		if (lRet == 0) {
			MIDITrack_RemoveSingleEvent (pTrack, pEvent->m_pPrevCombinedEvent);
			return 0;
		}
		return 2;
	}
	/* RPNチェンジ又はNPRNチェンジ又はパッチチェンジの場合 */
	else if (MIDIEvent_IsRPNChange (pEvent) || MIDIEvent_IsNRPNChange (pEvent) ||
	MIDIEvent_IsPatchChange (pEvent)) {
		lRet = MIDITrack_InsertSingleEventAfter (pTrack, pEvent, pTarget);
		if (lRet == 0) {
			return 0;
		}
		lRet = MIDITrack_InsertSingleEventAfter (pTrack, pEvent->m_pPrevCombinedEvent, pTarget);
		if (lRet == 0) {
			MIDITrack_RemoveSingleEvent (pTrack, pEvent);
			return 0;
		}
		lRet = MIDITrack_InsertSingleEventAfter (pTrack, pEvent->m_pPrevCombinedEvent->m_pPrevCombinedEvent, pTarget);
		if (lRet == 0) {
			MIDITrack_RemoveSingleEvent (pTrack, pEvent);
			MIDITrack_RemoveSingleEvent (pTrack, pEvent->m_pPrevCombinedEvent);
			return 0;
		}
		return 3;
	}
	/* 未定義の結合イベント */
	assert (0);
	return 0;
}

/* トラックにイベントを挿入(イベントはあらかじめ生成しておく) */
/* 挿入位置は時刻により決定する。*/
/* 同時刻のイベントがある場合は、それらの最後に挿入される */
long __stdcall MIDITrack_InsertEvent (MIDITrack* pTrack, MIDIEvent* pEvent) {
	long i = 0;
	long lRet = 0;
	MIDIEvent* pInsertEvent = pEvent;
	MIDIData* pMIDIData = NULL;
	assert (pTrack);
	assert (pEvent);
	/* pEventが浮遊状態であることを確認 */
	if (pEvent->m_pParent != NULL || pEvent->m_pPrevEvent != NULL || pEvent->m_pNextEvent != NULL) {
		return 0;
	}
	/* エンドオブトラックの重複挿入の防止 */
	if (pTrack->m_pLastEvent) {
		if (((MIDIEvent*)(pTrack->m_pLastEvent))->m_lKind == MIDIEVENT_ENDOFTRACK && 
			pEvent->m_lKind == MIDIEVENT_ENDOFTRACK) {
			return 0;
		}
	}
	/* フォーマット1のときの場合のイベントの種類整合性チェック */
	pMIDIData = MIDITrack_GetParent (pTrack);
	if (pMIDIData) {
		if (pMIDIData->m_lFormat == MIDIDATA_FORMAT1) {
			/* 最初のトラックにMIDIチャンネルイベントの挿入防止 */
			if (pTrack == pMIDIData->m_pFirstTrack) {
				if (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xEF) {
					return 0;	
				}
			}
			/* 2番目以降のトラックにテンポ・SMPTEオフセット・拍子記号・調性記号の挿入防止 */
			else {
				if (0x50 <= pEvent->m_lKind && pEvent->m_lKind <= 0x5F) {
					return 0;
				}
			}
		}
	}

	/* 各イベントの処理 */
	pInsertEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	while (pInsertEvent) {
		MIDIEvent* pTempEvent = pTrack->m_pLastEvent;
		long lInsertTime = MIDIEvent_GetTime (pInsertEvent);
		/* トラックの後方から挿入位置を探索 */
		while (1) {
			/* トラックにデータがない、又はトラックの先頭入れてよい */
			if (pTempEvent == NULL) {
				lRet += MIDITrack_InsertSingleEventAfter (pTrack, pInsertEvent, NULL);
				break;
			}
			/* pTempEventの直後に入れてよい */
			else {
				/* 挿入するものがノートオフイベントの場合(ベロシティ0のノートオンを含む) */
				if (MIDIEvent_IsNoteOff (pInsertEvent)) {
					/* 対応するノートオンイベントより前には絶対に来れない (20090111追加) */
					if (pTempEvent == pInsertEvent->m_pPrevCombinedEvent) {
						lRet += MIDITrack_InsertSingleEventAfter (pTrack, pInsertEvent, pTempEvent);
						break;
					}
					/* 同時刻のイベントがある場合は同時刻の他のノートオフの直後に挿入 */
					else if (pTempEvent->m_lTime == lInsertTime && MIDIEvent_IsNoteOff (pTempEvent)) {
						lRet += MIDITrack_InsertSingleEventAfter (pTrack, pInsertEvent, pTempEvent);
						break;
					}
					else if (pTempEvent->m_lTime < lInsertTime) {
						lRet += MIDITrack_InsertSingleEventAfter (pTrack, pInsertEvent, pTempEvent);
						break;
					}
				}
				/* その他のイベントの場合 */
				else {
					if (pTempEvent->m_lTime <= lInsertTime) {
						lRet += MIDITrack_InsertSingleEventAfter (pTrack, pInsertEvent, pTempEvent);
						break;
					}
				}
			}
			pTempEvent = pTempEvent->m_pPrevEvent;
		}
		pInsertEvent = pInsertEvent->m_pNextCombinedEvent;
		i++;
	}
	return lRet;
}

/* トラックにシーケンス番号イベントを生成して挿入 */
long __stdcall MIDITrack_InsertSequenceNumber (MIDITrack* pTrack, long lTime, long lNum) { 
	MIDIEvent* pEvent = MIDIEvent_CreateSequenceNumber (lTime, lNum);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにテキストベースイベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertTextBasedEventA 
(MIDITrack* pTrack, long lTime, long lKind, const char* pszText) {
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, lKind, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにテキストベースイベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertTextBasedEventW 
(MIDITrack* pTrack, long lTime, long lKind, const wchar_t* pszText) {
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, lKind, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにテキストベースイベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertTextBasedEventExA 
(MIDITrack* pTrack, long lTime, long lKind, long lCharCode, const char* pszText) {
	long lRet = 0;
	MIDIEvent* pEvent = NULL;
	if (lKind <= 0x00 || lKind >= 0x1F) {
		return 0;
	}
	/* lCharCode==MIDIEVENT_NOCHARCODEの場合でも、*/
	/* 文字コードは直近の同種イベントに基づいてエンコードされるが、*/
	/* この時点では探索ができないため、仮に空の文字列で生成する。 */
	pEvent = MIDIEvent_CreateTextBasedEventExA (lTime, lKind, lCharCode, "");
	if (pEvent == NULL) {
		return 0;
	}
	lRet = MIDITrack_InsertEvent (pTrack, pEvent);
	if (lRet == 0) {
		MIDIEvent_Delete (pEvent);
		return 0;
	}
	/* トラックに挿入されているので、直近の同種イベントを探索できる。*/
	/* lCharCode==MIDIEVENT_NOCHARCODEの場合、
	/* 文字コードは直近の同種イベントに基づきエンコードされる。 */
	lRet = MIDIEvent_SetTextA (pEvent, pszText);
	if (lRet == 0) {
		MIDITrack_RemoveEvent (pTrack, pEvent);
		MIDIEvent_Delete (pEvent);
		return 0;
	}
	return 1;
}

/* トラックにテキストベースイベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertTextBasedEventExW 
(MIDITrack* pTrack, long lTime, long lKind, long lCharCode, const wchar_t* pszText) { 
	long lRet = 0;
	MIDIEvent* pEvent = NULL;
	if (lKind <= 0x00 || lKind >= 0x1F) {
		return 0;
	}
	/* lCharCode==MIDIEVENT_NOCHARCODEの場合でも、*/
	/* 文字コードは直近の同種イベントに基づいてエンコードされるが、*/
	/* この時点では探索ができないため、仮に空の文字列で生成する。 */
	pEvent = MIDIEvent_CreateTextBasedEventExW (lTime, lKind, lCharCode, L"");
	if (pEvent == NULL) {
		return 0;
	}
	lRet = MIDITrack_InsertEvent (pTrack, pEvent);
	if (lRet == 0) {
		MIDIEvent_Delete (pEvent);
		return 0;
	}
	/* トラックに挿入されているので、直近の同種イベントを探索できる。*/
	/* lCharCode==MIDIEVENT_NOCHARCODEの場合、
	/* 文字コードは直近の同種イベントに基づきエンコードされる。 */
	lRet = MIDIEvent_SetTextW (pEvent, pszText);
	if (lRet == 0) {
		MIDITrack_RemoveEvent (pTrack, pEvent);
		MIDIEvent_Delete (pEvent);
		return 0;
	}
	return 1;
}

/* トラックにテキストイベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertTextEventA 
(MIDITrack* pTrack, long lTime, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA 
		(pTrack, lTime, MIDIEVENT_TEXTEVENT, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにテキストイベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertTextEventW 
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW 
		(pTrack, lTime, MIDIEVENT_TEXTEVENT, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにテキストイベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertTextEventExA 
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA 
		(pTrack, lTime, MIDIEVENT_TEXTEVENT, lCharCode, pszText);
}

/* トラックにテキストイベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertTextEventExW 
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) { 
	MIDIEvent* pEvent = MIDIEvent_CreateTextEventExW (lTime, lCharCode, pszText);
	return MIDITrack_InsertTextBasedEventExW 
		(pTrack, lTime, MIDIEVENT_TEXTEVENT, lCharCode, pszText);
}

/* トラックに著作権イベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertCopyrightNoticeA 
(MIDITrack* pTrack, long lTime, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA 
		(pTrack, lTime, MIDIEVENT_COPYRIGHTNOTICE, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックに著作権イベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertCopyrightNoticeW
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW 
		(pTrack, lTime, MIDIEVENT_COPYRIGHTNOTICE, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックに著作権イベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertCopyrightNoticeExA 
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA 
		(pTrack, lTime, MIDIEVENT_COPYRIGHTNOTICE, lCharCode, pszText);
}

/* トラックに著作権イベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertCopyrightNoticeExW
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW 
		(pTrack, lTime, MIDIEVENT_COPYRIGHTNOTICE, lCharCode, pszText);
}

/* トラックにトラック名イベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertTrackNameA
(MIDITrack* pTrack, long lTime, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_TRACKNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにトラック名イベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertTrackNameW
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_TRACKNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにトラック名イベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertTrackNameExA
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_TRACKNAME, lCharCode, pszText);
}

/* トラックにトラック名イベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertTrackNameExW
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_TRACKNAME, lCharCode, pszText);
}

/* トラックにインストゥルメントイベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertInstrumentNameA
(MIDITrack* pTrack, long lTime, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_INSTRUMENTNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにインストゥルメントイベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertInstrumentNameW
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_INSTRUMENTNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにインストゥルメントイベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertInstrumentNameExA
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_INSTRUMENTNAME, lCharCode, pszText);
}

/* トラックにインストゥルメントイベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertInstrumentNameExW
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_INSTRUMENTNAME, lCharCode, pszText);
}

/* トラックに歌詞イベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertLyricA 
(MIDITrack* pTrack, long lTime, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_LYRIC, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックに歌詞イベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertLyricW 
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_LYRIC, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックに歌詞イベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertLyricExA 
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_LYRIC, lCharCode, pszText);
}

/* トラックに歌詞イベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertLyricExW 
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_LYRIC, lCharCode, pszText);
}

/* トラックにマーカーイベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertMarkerA 
(MIDITrack* pTrack, long lTime, const char* pszText) {
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_MARKER, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにマーカーイベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertMarkerW
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) {
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_MARKER, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにマーカーイベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertMarkerExA 
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) {
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_MARKER, lCharCode, pszText);
}

/* トラックにマーカーイベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertMarkerExW
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_MARKER, lCharCode, pszText);
}

/* トラックにキューポイントイベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertCuePointA
(MIDITrack* pTrack, long lTime, const char* pszText) {
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_CUEPOINT, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにキューポイントイベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertCuePointW
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) {
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_CUEPOINT, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにキューポイントイベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertCuePointExA 
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) {
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_CUEPOINT, lCharCode, pszText);
}

/* トラックにキューポイントイベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertCuePointExW
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) {
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_CUEPOINT, lCharCode, pszText);
}

/* トラックにプログラム名イベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertProgramNameA
(MIDITrack* pTrack, long lTime, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_PROGRAMNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにプログラム名イベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertProgramNameW
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_PROGRAMNAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにプログラム名イベントを生成して挿入(文字コード指定あり)(ANSI) */
long __stdcall MIDITrack_InsertProgramNameExA
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_PROGRAMNAME, lCharCode, pszText);
}

/* トラックにプログラム名イベントを生成して挿入(文字コード指定あり)(UNICODE) */
long __stdcall MIDITrack_InsertProgramNameExW
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_PROGRAMNAME, lCharCode, pszText);
}
	
/* トラックにデバイス名イベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertDeviceNameA
(MIDITrack* pTrack, long lTime, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_DEVICENAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにデバイス名イベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertDeviceNameW
(MIDITrack* pTrack, long lTime, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_DEVICENAME, MIDIEVENT_NOCHARCODE, pszText);
}

/* トラックにデバイス名イベントを生成して挿入(ANSI) */
long __stdcall MIDITrack_InsertDeviceNameExA
(MIDITrack* pTrack, long lTime, long lCharCode, const char* pszText) { 
	return MIDITrack_InsertTextBasedEventExA
		(pTrack, lTime, MIDIEVENT_DEVICENAME, lCharCode, pszText);
}

/* トラックにデバイス名イベントを生成して挿入(UNICODE) */
long __stdcall MIDITrack_InsertDeviceNameExW
(MIDITrack* pTrack, long lTime, long lCharCode, const wchar_t* pszText) { 
	return MIDITrack_InsertTextBasedEventExW
		(pTrack, lTime, MIDIEVENT_DEVICENAME, lCharCode, pszText);
}

/* トラックにチャンネルプレフィックスイベントを生成して挿入 */
long __stdcall MIDITrack_InsertChannelPrefix (MIDITrack* pTrack, long lTime, long lCh) { 
	MIDIEvent* pEvent = MIDIEvent_CreateChannelPrefix (lTime, lCh);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにポートプレフィックスイベントを生成して挿入 */
long __stdcall MIDITrack_InsertPortPrefix (MIDITrack* pTrack, long lTime, long lPort) { 
	MIDIEvent* pEvent = MIDIEvent_CreatePortPrefix (lTime, lPort);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにエンドオブトラックイベントを生成して挿入 */
long __stdcall MIDITrack_InsertEndofTrack (MIDITrack* pTrack, long lTime) { 
	MIDIEvent* pEvent = MIDIEvent_CreateEndofTrack (lTime);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにテンポイベントを生成して挿入 */
long __stdcall MIDITrack_InsertTempo (MIDITrack* pTrack, long lTime, long lTempo) { 
	MIDIEvent* pEvent = MIDIEvent_CreateTempo (lTime, lTempo);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにSMPTEオフセットイベントを生成して挿入 */
long MIDITrack_InsertSMPTEOffset 
(MIDITrack* pTrack, long lTime, long lMode, 
 long lHour, long lMin, long lSec, long lFrame, long lSubFrame) {
	MIDIEvent* pEvent = MIDIEvent_CreateSMPTEOffset 
		(lTime, lMode, lHour, lMin, lSec, lFrame, lSubFrame);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックに拍子イベントを生成して挿入 */
long __stdcall MIDITrack_InsertTimeSignature 
(MIDITrack* pTrack, long lTime, long lnn, long ldd, long lcc, long lbb) { 
	MIDIEvent* pEvent = MIDIEvent_CreateTimeSignature (lTime, lnn, ldd, lcc, lbb);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックに調性イベントを生成して挿入 */
long __stdcall MIDITrack_InsertKeySignature (MIDITrack* pTrack, long lTime, long lsf, long lmi) { 
	MIDIEvent* pEvent = MIDIEvent_CreateKeySignature (lTime, lsf, lmi);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにシーケンサ独自のイベントを生成して挿入 */
long __stdcall MIDITrack_InsertSequencerSpecific (MIDITrack* pTrack, long lTime, char* pBuf, long lLen) { 
	MIDIEvent* pEvent = MIDIEvent_CreateSequencerSpecific (lTime, pBuf, lLen);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにノートオフイベントを生成して挿入 */
long __stdcall MIDITrack_InsertNoteOff
	(MIDITrack* pTrack, long lTime, long lCh, long lKey, long lVel) {
	MIDIEvent* pEvent = MIDIEvent_CreateNoteOff (lTime, lCh, lKey, lVel);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにノートオンイベントを生成して挿入 */
long __stdcall MIDITrack_InsertNoteOn
	(MIDITrack* pTrack, long lTime, long lCh, long lKey, long lVel) {
	MIDIEvent* pEvent = MIDIEvent_CreateNoteOn (lTime, lCh, lKey, lVel);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにノートイベントを生成して挿入 */
long __stdcall MIDITrack_InsertNote 
	(MIDITrack* pTrack, long lTime, long lCh, long lKey, long lVel, long lDur) {
	MIDIEvent* pEvent = MIDIEvent_CreateNote (lTime, lCh, lKey, lVel, lDur);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにキーアフターイベントを生成して挿入 */
long __stdcall MIDITrack_InsertKeyAftertouch
	(MIDITrack* pTrack, long lTime, long lCh, long lKey, long lVal) {
	MIDIEvent* pEvent = MIDIEvent_CreateKeyAftertouch (lTime, lCh, lKey, lVal);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにコントローラーイベントを生成して挿入 */
long __stdcall MIDITrack_InsertControlChange
	(MIDITrack* pTrack, long lTime, long lCh, long lNum, long lVal) {
	MIDIEvent* pEvent = MIDIEvent_CreateControlChange (lTime, lCh, lNum, lVal);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにRPNチェンジイベントを生成して挿入 */
long __stdcall MIDITrack_InsertRPNChange 
	(MIDITrack* pTrack, long lTime, long lCh, long lCC101, long lCC100, long lVal) {
	MIDIEvent* pEvent = MIDIEvent_CreateRPNChange (lTime, lCh, lCC101, lCC100, lVal);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにNRPNチェンジイベントを生成して挿入 */
long __stdcall MIDITrack_InsertNRPNChange 
	(MIDITrack* pTrack, long lTime, long lCh, long lCC99, long lCC98, long lVal) {
	MIDIEvent* pEvent = MIDIEvent_CreateNRPNChange (lTime, lCh, lCC99, lCC98, lVal);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにプログラムイベントを生成して挿入 */
long __stdcall MIDITrack_InsertProgramChange
	(MIDITrack* pTrack, long lTime, long lCh, long lNum) {
	MIDIEvent* pEvent = MIDIEvent_CreateProgramChange (lTime, lCh, lNum);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにバンクパッチイベントを生成して挿入 */
long __stdcall MIDITrack_InsertPatchChange 
	(MIDITrack* pTrack, long lTime, long lCh, long lCC0, long lCC32 , long lNum) {
	MIDIEvent* pEvent = MIDIEvent_CreatePatchChange (lTime, lCh, lCC0, lCC32, lNum);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにチャンネルアフターイベントを生成して挿入 */
long __stdcall MIDITrack_InsertChannelAftertouch 
	(MIDITrack* pTrack, long lTime, long lCh, long lVal) {
	MIDIEvent* pEvent = MIDIEvent_CreateChannelAftertouch (lTime, lCh, lVal);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにピッチベンドイベントを生成して挿入 */
long __stdcall MIDITrack_InsertPitchBend
	(MIDITrack* pTrack, long lTime, long lCh, long lVal) {
	MIDIEvent* pEvent = MIDIEvent_CreatePitchBend (lTime, lCh, lVal);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラックにシステムエクスクルーシブイベントを生成して挿入 */
long __stdcall MIDITrack_InsertSysExEvent
	(MIDITrack* pTrack, long lTime, unsigned char* pBuf, long lLen) {
	MIDIEvent* pEvent = MIDIEvent_CreateSysExEvent (lTime, pBuf, lLen);
	if (pEvent == NULL) {
		return 0;
	}
	return MIDITrack_InsertEvent (pTrack, pEvent);
}

/* トラック内のイベントを複製する */
long __stdcall MIDITrack_DuplicateEvent (MIDITrack* pTrack, MIDIEvent* pEvent) {
	long lCount = 0;
	MIDIEvent* pDuplicatedEvent = NULL;
	MIDIEvent* pTargetEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	assert (pTrack);
	assert (pEvent);
	assert (pEvent->m_pParent == pTrack);
	pDuplicatedEvent = MIDIEvent_CreateClone (pTargetEvent);
	if (pDuplicatedEvent == NULL) {
		return 0;
	}
	while (pTargetEvent && pDuplicatedEvent) {
		MIDITrack_InsertSingleEventAfter (pTrack, pDuplicatedEvent, pTargetEvent);
		pTargetEvent = pTargetEvent->m_pNextCombinedEvent;
		pDuplicatedEvent = pDuplicatedEvent->m_pNextCombinedEvent;
		lCount++;
	}
	return lCount;
}

/* トラックからイベントを1つ取り除く(イベントオブジェクトは削除しません) */
long __stdcall MIDITrack_RemoveSingleEvent (MIDITrack* pTrack, MIDIEvent* pEvent) {
	assert (pTrack);
	assert (pEvent);
	assert (pEvent->m_pParent == pTrack);
	return MIDIEvent_SetFloating (pEvent);
}

/* トラックからイベントを取り除く(イベントオブジェクトは削除しません) */
long __stdcall MIDITrack_RemoveEvent (MIDITrack* pTrack, MIDIEvent* pEvent) {
	long lCount = 0;
	MIDIEvent* pRemoveEvent = MIDIEvent_GetFirstCombinedEvent (pEvent);
	assert (pTrack);
	assert (pEvent);
	assert (pEvent->m_pParent == pTrack);
	if (pEvent->m_pParent != pTrack) {
		return 0;
	}
	while (pRemoveEvent) {
		MIDIEvent_SetFloating (pRemoveEvent);
		pRemoveEvent = pRemoveEvent->m_pNextCombinedEvent;
		lCount++;
	}
	return lCount;
}

/* MIDIトラックが浮遊トラックであるかどうかを調べる */
long __stdcall MIDITrack_IsFloating (MIDITrack* pMIDITrack) {
	return (pMIDITrack->m_pParent == NULL ? 1 : 0);
}


/* MIDIトラックがコンダクタートラックとして正しいことを確認する */
/* (コンダクタートラックの条件は、MIDIチャンネルイベントを含まないこと) */
long __stdcall MIDITrack_CheckSetupTrack (MIDITrack* pMIDITrack) {
	MIDIEvent* pMIDIEvent;
	forEachEvent (pMIDITrack, pMIDIEvent) {
		if (0x80 <= pMIDIEvent->m_lKind && pMIDIEvent->m_lKind <= 0xEF) {
			return 0;
		}
	}
	return 1;
}

/* MIDIトラックがノンコンダクタートラックとして正しいことを確認する */
/* (ノンコンダクタートラックには、0x50台のイベント(テンポ・SMPTEオフセット・拍子記号・調性記号など)を */
/*　含んではならない。) */
long __stdcall MIDITrack_CheckNonSetupTrack (MIDITrack* pMIDITrack) {
	MIDIEvent* pMIDIEvent;
	forEachEvent (pMIDITrack, pMIDIEvent) {
		if (0x50 <= pMIDIEvent->m_lKind && pMIDIEvent->m_lKind < 0x60) {
			return 0;
		}
	}
	return 1;
}



/* タイムコードをミリ秒時刻に変換(指定トラック内のテンポイベントを基に計算) */
long __stdcall MIDITrack_TimeToMillisec (MIDITrack* pMIDITrack, long lTime) {
	long lMode = 0;
	long lResolution = 0;
	assert (pMIDITrack);
	assert (lTime >= 0);
	/* 浮遊トラックは認めない */
	if (pMIDITrack->m_pParent == NULL) {
		return 0;
	}
	/* タイムベースを調べる */
	MIDIData_GetTimeBase (((MIDIData*)(pMIDITrack->m_pParent)), &lMode, &lResolution);
	/* TPQNベースのMIDIデータの場合 */
	if (lMode == MIDIDATA_TPQNBASE) {
		long lDeltaTime = 0;
		long lOldTime = 0;
		long lOldTempo = MIDIEVENT_DEFTEMPO;
		int64_t llDeltaResoMicrosec = 0;
		int64_t llSumResoMicrosec = 0;
		/* 最初のテンポイベントを探す */
		MIDIEvent* pEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TEMPO);
		/* テンポイベントを発見するたびにその位置のレゾμ秒を求める */
		while (pEvent) {
			if (pEvent->m_lTime > lTime) {
				break;
			}
			lDeltaTime = pEvent->m_lTime - lOldTime;
			llDeltaResoMicrosec = ((int64_t)lDeltaTime) * ((int64_t)lOldTempo);
			llSumResoMicrosec += llDeltaResoMicrosec;
			assert (llSumResoMicrosec >= 0);
			lOldTime = pEvent->m_lTime;
			lOldTempo = MIDIEvent_GetTempo (pEvent);
			pEvent = pEvent->m_pNextSameKindEvent;
		}
		/* 最後のテンポイベントから指定位置までのレゾμ秒を求める */
		lDeltaTime = lTime - lOldTime;
		llDeltaResoMicrosec = ((int64_t)lDeltaTime) * ((int64_t)lOldTempo);
		llSumResoMicrosec += llDeltaResoMicrosec;
		assert (llSumResoMicrosec >= 0);
		llSumResoMicrosec /= lResolution;
		llSumResoMicrosec /= 1000;
		return (long)llSumResoMicrosec;
	}
	/* SMPTEベースのMIDIデータの場合 */
	else {
		int64_t llTemp;
		// lTime * 1000 / (lMode * lResolution);
		// lMode = MIDIDATA_SMPTE29BASE → lMode = 29.97扱い;
		if (lMode == MIDIDATA_SMPTE29BASE) {
			llTemp = ((int64_t)lTime) * ((int64_t)100000);
			return (long)(llTemp / (int64_t)(2997 * lResolution));
		}
		llTemp = ((int64_t)lTime) * ((int64_t)1000);
		return (long)(llTemp / (int64_t)(lMode * lResolution));
	}
}

/* ミリ秒時刻をタイムコードに変換(指定トラック内のテンポイベントを基に計算) */
long __stdcall MIDITrack_MillisecToTime (MIDITrack* pMIDITrack, long lMillisec) {
	long lMode;
	long lResolution;
	assert (pMIDITrack);
	assert (lMillisec >= 0);
	/* 浮遊トラックは認めない */
	if (pMIDITrack->m_pParent == NULL) {
		return 0;
	}
	/* タイムベースを調べる */
	MIDIData_GetTimeBase (((MIDIData*)(pMIDITrack->m_pParent)), &lMode, &lResolution);
	/* TPQNベースのMIDIデータの場合 */
	if (lMode == MIDIDATA_TPQNBASE) {
		long lDeltaTime = 0;
		long lOldTime = 0;
		long lOldTempo = MIDIEVENT_DEFTEMPO;
		int64_t llDeltaResoMicrosec = 0;
		int64_t llSumResoMicrosec = 0;
		int64_t llResoMicrosec = ((int64_t)lMillisec) * ((int64_t)lResolution) * ((int64_t)1000);
		/* 最初のテンポイベントを探す */
		MIDIEvent* pEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TEMPO);
		/* 最後のテンポイベントのレゾμ秒をlSumResoMicrosecに取得 */
		while (pEvent) {
			lDeltaTime = pEvent->m_lTime - lOldTime;
			llDeltaResoMicrosec = ((int64_t)lDeltaTime) * ((int64_t)lOldTempo);
			if (llSumResoMicrosec + llDeltaResoMicrosec > llResoMicrosec) {
				break;
			}
			llSumResoMicrosec += llDeltaResoMicrosec;
			lOldTime = pEvent->m_lTime;
			lOldTempo = MIDIEvent_GetTempo (pEvent);
			pEvent = pEvent->m_pNextSameKindEvent;
		}
		/* 最後のテンポ時刻から指定時刻までの経過タイム[レゾマイクロ秒]を計算 */
		llDeltaResoMicrosec = llResoMicrosec - llSumResoMicrosec;
		//lDeltaTime = (long)(llDeltaResoMicrosec / (int64_t)lResolution / (int64_t)lOldTempo);//20080920式修正
		lDeltaTime = (long)(llDeltaResoMicrosec / (int64_t)lOldTempo);
		return lOldTime + lDeltaTime;
	}
	/* SMPTEベースのMIDIデータの場合 */
	else {
		int64_t llTemp;
		if (lMode == MIDIDATA_SMPTE29BASE) {
			llTemp = (int64_t)2997 * (int64_t)lResolution * (int64_t)lMillisec;
			return (long)(llTemp / (int64_t)100000);
		}
		llTemp = (int64_t)lMode * (int64_t)lResolution * (int64_t)lMillisec;
		return (long)(llTemp / (int64_t)1000);
	}
}

/* TPQN時刻をSMPTE時刻に変換(指定トラック内のテンポイベントを基に計算) */
/* この関数はSetTimeBaseでTPQNベースをSMPTEベースに変換する時にのみ呼ばれる。 */
long __stdcall MIDITrack_TPQNtoSMPTE (MIDITrack* pMIDITrack, long lTime, long lNewMode, long lNewResolution) {
	long lMode = 0;
	long lResolution = 0;
	assert (pMIDITrack);
	assert (lTime >= 0);
	/* 浮遊トラックは認めない */
	if (pMIDITrack->m_pParent == NULL) {
		return 0;
	}
	/* タイムベースを調べる */
	MIDIData_GetTimeBase (((MIDIData*)(pMIDITrack->m_pParent)), &lMode, &lResolution);
	/* TPQNベースのMIDIデータの場合 */
	if (lMode == MIDIDATA_TPQNBASE) {
		long lDeltaTime = 0;
		long lOldTime = 0;
		long lOldTempo = MIDIEVENT_DEFTEMPO;
		int64_t llDeltaResoMicrosec = 0;
		int64_t llSumResoMicrosec = 0;
		int64_t llTemp = 0;
		/* 最初のテンポイベントを探す */
		MIDIEvent* pEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TEMPO);
		/* テンポイベントを発見するたびにその位置のレゾμ秒を求める */
		while (pEvent) {
			if (pEvent->m_lTime > lTime) {
				break;
			}
			lDeltaTime = pEvent->m_lTime - lOldTime;
			llDeltaResoMicrosec = ((int64_t)lDeltaTime) * ((int64_t)lOldTempo);
			llSumResoMicrosec += llDeltaResoMicrosec;
			assert (llSumResoMicrosec >= 0);
			lOldTime = pEvent->m_lTime;
			lOldTempo = MIDIEvent_GetTempo (pEvent);
			pEvent = pEvent->m_pNextSameKindEvent;
		}
		/* 最後のテンポイベントから指定位置までのレゾμ秒を求める */
		lDeltaTime = lTime - lOldTime;
		llDeltaResoMicrosec = ((int64_t)lDeltaTime) * ((int64_t)lOldTempo);
		llSumResoMicrosec += llDeltaResoMicrosec;
		assert (llSumResoMicrosec >= 0);
		if (lNewMode == MIDIDATA_SMPTE29BASE) {
			llTemp = llSumResoMicrosec * (int64_t)2997 * (int64_t)lNewResolution;
			return (long)(llTemp / 100000000 / (int64_t)lResolution);
		}
		llTemp = llSumResoMicrosec * (int64_t)lNewMode * (int64_t)lNewResolution;
		return (long)(llTemp / 1000000 / (int64_t)lResolution);
		
	}
	return 0;
}



/* SMPTE時刻をTPQN時刻に変換(指定トラック内のテンポイベントを基に計算) */
/* この関数はSetTimeBaseでSMPTEベースをTPQNベースに変換する時にのみ呼ばれる。 */
long __stdcall MIDITrack_SMPTEtoTPQN (MIDITrack* pMIDITrack, long lTime, long lNewResolution) {
	long lMode;
	long lResolution;
	assert (pMIDITrack);
	assert (lTime >= 0);
	assert (lNewResolution > 0);
	/* 浮遊トラックは認めない */
	if (pMIDITrack->m_pParent == NULL) {
		return 0;
	}
	/* タイムベースを調べる */
	MIDIData_GetTimeBase ((MIDIData*)(pMIDITrack->m_pParent), &lMode, &lResolution);
	/* SMPTEベースのMIDIデータの場合 */
	if (lMode != MIDIDATA_TPQNBASE) {
		long lDeltaTime = 0;
		long lOldTime = 0;
		long lOldTempo = MIDIEVENT_DEFTEMPO;
		int64_t llDeltaResoModeTick = 0;
		int64_t llSumResoModeTick = 0;
		/* 最初のテンポイベントを探す */
		MIDIEvent* pEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TEMPO);
		/* 最後のテンポイベントの位置[レゾモードティック]をlSumResoModeTickに取得 */
		while (pEvent) {
			if (pEvent->m_lTime >= lTime) {
				break;
			}
			lDeltaTime = pEvent->m_lTime - lOldTime; /* [SMPTEframe=レゾモード秒] */
			llDeltaResoModeTick = ((int64_t)lDeltaTime * (int64_t)1000000 * (int64_t)lNewResolution / (int64_t)lOldTempo);
			llSumResoModeTick += llDeltaResoModeTick;
			assert (llSumResoModeTick >= 0);
			lOldTime = pEvent->m_lTime;
			lOldTempo = MIDIEvent_GetTempo (pEvent);
			pEvent = pEvent->m_pNextSameKindEvent;
		}
		/* 最後のテンポ時刻から指定時刻までの経過タイムを計算 */
		lDeltaTime = lTime - lOldTime; /* [SMPTEframe=レゾモード秒] */
		llDeltaResoModeTick = ((int64_t)lDeltaTime * (int64_t)1000000 * (int64_t)lNewResolution / (int64_t)lOldTempo);
		llSumResoModeTick += llDeltaResoModeTick;
		assert (llSumResoModeTick >= 0);
		if (lMode == MIDIDATA_SMPTE29BASE) {
			return (long)(llSumResoModeTick * (int64_t)100 / (int64_t)(2997 * lResolution));
		}
		//llSumResoModeTick /= lMode;
		//llSumResoModeTick /= lResolution;
		//return (long)llSumResoModeTick;
		return (long)(llSumResoModeTick / (int64_t)(lMode * lResolution));
	}
	return 0;
}




/* タイムコードを小節：拍：ティックに分解(指定トラック内の拍子記号から計算) */
long __stdcall MIDITrack_BreakTimeEx
(MIDITrack* pMIDITrack, long lTime, long* pMeasure, long* pBeat, long* pTick,
 long* pnn, long* pdd, long* pcc, long* pbb) {
	long lOldTime = 0;
	long lOldnn = 4;
	long lOlddd = 2;
	long lOldcc = 24;
	long lOldbb = 8;
	long lnn, ldd, lcc, lbb;
	long lSumMeasure = 0;
	long lDeltaMeasure;
	long lDeltaTime;
	long lUnitTick;
	long lTimeMode;
	long lTimeResolution;
	MIDIData* pMIDIData = (MIDIData*)(pMIDITrack->m_pParent);
	MIDIEvent* pMIDIEvent;
	/* 浮遊トラックは認めない */
	if (pMIDIData == NULL) {
		return 0;
	}
	/* MIDIデータのタイムモードと分解能を取得 */
	MIDIData_GetTimeBase (pMIDIData, &lTimeMode, &lTimeResolution);

	/* TPQNベースのMIDIデータの場合 */
	if (lTimeMode == MIDIDATA_TPQNBASE) {
		/* 最初の拍子記号を取得 */
		pMIDIEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TIMESIGNATURE);
		/* 拍子記号を発見するたびにその位置の小節番号を求める */
		while (pMIDIEvent) {
			if (pMIDIEvent->m_lTime >= lTime) {
				break;
			}
			MIDIEvent_GetTimeSignature (pMIDIEvent, &lnn, &ldd, &lcc, &lbb);
			lDeltaTime = pMIDIEvent->m_lTime - lOldTime;
			lUnitTick = lTimeResolution * 4 / (1 << lOlddd); /* 分子1あたりのティック数 */
			if (lDeltaTime > 0) {
				lDeltaMeasure = (lDeltaTime - 1) / (lUnitTick * lOldnn) + 1;
			}
			else {
				lDeltaMeasure = 0;
			}
			lSumMeasure += lDeltaMeasure;
			lOldTime = pMIDIEvent->m_lTime;
			lOldnn = lnn;
			lOlddd = ldd;
			lOldcc = lcc;
			lOldbb = lbb;
			pMIDIEvent = pMIDIEvent->m_pNextSameKindEvent;
		}
		/* 最後の拍子記号から指定位置までの増分を求める */
		lDeltaTime = lTime - lOldTime;
		lUnitTick = lTimeResolution * 4 / (1 << lOlddd); /* 分子1あたりのティック数 */
		if (lDeltaTime >= 0) {
			lDeltaMeasure = (lDeltaTime) / (lUnitTick * lOldnn);
		}
		else {
			lDeltaMeasure = 0;
		}
		*pMeasure = lSumMeasure + lDeltaMeasure;
		*pBeat = (lDeltaTime % (lUnitTick * lOldnn)) / lUnitTick;
		*pTick = lDeltaTime % lUnitTick;
		*pnn = lOldnn;
		*pdd = lOlddd;
		*pcc = lOldcc;
		*pbb = lOldbb;
	}
	/* SMPTEベースのMIDIデータの場合 */
	else {
		*pMeasure = lTime / lTimeResolution;
		*pBeat = 0;
		*pTick = lTime % lTimeResolution;
	}
	return 1;
}

/* タイムコードを小節：拍：ティックに分解(指定トラック内の拍子記号から計算) */
long __stdcall MIDITrack_BreakTime 
(MIDITrack* pMIDITrack, long lTime, long* pMeasure, long* pBeat, long* pTick) {
	long lnn, lbb, lcc, ldd;
	return MIDITrack_BreakTimeEx (pMIDITrack, lTime, pMeasure, pBeat, pTick, 
		&lnn, &lbb, &lcc, &ldd);
}

/* 小節：拍：ティックからタイムコードを生成(指定トラック内の拍子記号から計算) */
long __stdcall MIDITrack_MakeTimeEx 
(MIDITrack* pMIDITrack, long lMeasure, long lBeat, long lTick, 
 long* pTime, long* pnn, long* pdd, long* pcc, long* pbb) {
	long lOldTime = 0;
	long lOldnn = 4;
	long lOlddd = 2;
	long lOldcc = 24;
	long lOldbb = 8;
	long lnn, ldd, lcc, lbb;
	long lSumMeasure = 0;
	long lDeltaMeasure;
	long lDeltaTime;
	long lUnitTick;
	long lTimeMode;
	long lTimeResolution;
	MIDIData* pMIDIData = (MIDIData*)(pMIDITrack->m_pParent);
	MIDIEvent* pMIDIEvent;
	/* 浮遊トラックは認めない */
	if (pMIDIData == NULL) {
		return 0;
	}
	/* タイムモードと分解能を取得 */
	MIDIData_GetTimeBase (pMIDIData, &lTimeMode, &lTimeResolution);
	/* TPQNベースのMIDIデータの場合 */
	if (lTimeMode == MIDIDATA_TPQNBASE) {
		/* 最初の拍子記号イベントを取得 */
		pMIDIEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TIMESIGNATURE);
		/* 拍子記号を発見するたびにその位置の小節番号を調べる */
		while (pMIDIEvent) {
			MIDIEvent_GetTimeSignature (pMIDIEvent, &lnn, &ldd, &lcc, &lbb);
			lDeltaTime = pMIDIEvent->m_lTime - lOldTime;
			lUnitTick = lTimeResolution * 4 / (1 << lOlddd); /* 分子1あたりのティック数 */
			if (lDeltaTime > 0) {
				lDeltaMeasure = (lDeltaTime - 1) / (lUnitTick * lOldnn) + 1;
			}
			else {
				lDeltaMeasure = 0;
			}
			if (lSumMeasure + lDeltaMeasure > lMeasure) {
				break;
			}
			lSumMeasure += lDeltaMeasure;
			lOldTime = pMIDIEvent->m_lTime;
			lOldnn = lnn;
			lOlddd = ldd;
			lOldcc = lcc;
			lOlddd = ldd;
			pMIDIEvent = pMIDIEvent->m_pNextSameKindEvent;
		}
		/* 最後の拍子記号からの経過ティック数を加算する */
		lUnitTick = lTimeResolution * 4 / (1 << lOlddd);
		lDeltaMeasure = lMeasure - lSumMeasure;
		*pTime = lOldTime + lUnitTick * lOldnn * lDeltaMeasure + lUnitTick * lBeat+ lTick; 
	
		/* 指定位置の拍子情報を格納 */
		*pnn = lOldnn;
		*pdd = lOlddd;
		*pcc = lOldcc;
		*pbb = lOldbb;
	}
	/* SMPTEベースのMIDIデータの場合 */
	else {
		*pTime = lMeasure * lTimeResolution + lTick;
	}
	return 1;
}

/* 小節：拍：ティックからタイムコードを生成(指定トラック内の拍子記号から計算) */
long __stdcall MIDITrack_MakeTime
(MIDITrack* pMIDITrack, long lMeasure, long lBeat, long lTick, long* pTime) {
	long lnn, lbb, lcc, ldd;
	return MIDITrack_MakeTimeEx (pMIDITrack, lMeasure, lBeat, lTick, pTime, 
		&lnn, &lbb, &lcc, &ldd);
}

/* 指定位置における直前のテンポを取得 */
long __stdcall MIDITrack_FindTempo
(MIDITrack* pMIDITrack, long lTime, long* pTempo) {
	long lTempo = MIDIEVENT_DEFTEMPO;
	MIDIEvent* pMIDIEvent = NULL;
	pMIDIEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TEMPO);
	while (pMIDIEvent) {
		if (pMIDIEvent->m_lTime <= lTime) {
			lTempo = MIDIEvent_GetTempo (pMIDIEvent);
		}
		else {
			break;
		}
		pMIDIEvent = MIDIEvent_GetNextSameKindEvent (pMIDIEvent);
	}
	*pTempo = lTempo;
	return 1;
}

/* 指定位置における直前の拍子記号を取得 */
long __stdcall MIDITrack_FindTimeSignature 
(MIDITrack* pMIDITrack, long lTime, long* pnn, long* pdd, long* pcc, long* pbb) {
	long lnn = 4;
	long ldd = 2;
	long lcc = 24;
	long lbb = 8;
	MIDIEvent* pMIDIEvent = NULL;
	pMIDIEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_TIMESIGNATURE);
	while (pMIDIEvent) {
		if (pMIDIEvent->m_lTime <= lTime) {
			MIDIEvent_GetTimeSignature (pMIDIEvent, &lnn, &ldd, &lcc, &lbb);
		}
		else {
			break;
		}
		pMIDIEvent = MIDIEvent_GetNextSameKindEvent (pMIDIEvent);
	}
	*pnn = lnn;
	*pdd = ldd;
	*pcc = lcc;
	*pbb = lbb;
	return 1;
}

/* 指定位置における直前の調性記号を取得 */
long __stdcall MIDITrack_FindKeySignature 
(MIDITrack* pMIDITrack, long lTime, long* psf, long* pmi) {
	long lsf = 0;
	long lmi = 0;
	MIDIEvent* pMIDIEvent = NULL;
	pMIDIEvent = MIDITrack_GetFirstKindEvent (pMIDITrack, MIDIEVENT_KEYSIGNATURE);
	while (pMIDIEvent) {
		if (pMIDIEvent->m_lTime <= lTime) {
			MIDIEvent_GetKeySignature (pMIDIEvent, &lsf, &lmi);
		}
		else {
			break;
		}
		pMIDIEvent = MIDIEvent_GetNextSameKindEvent (pMIDIEvent);
	}
	*psf = lsf;
	*pmi = lmi;
	return 1;
}







/******************************************************************************/
/*                                                                            */
/*　MIDIDataクラス関数群                                                      */
/*                                                                            */
/******************************************************************************/


/* トラックをpTargetの直前に挿入(トラックは予め生成しておく) */
long __stdcall MIDIData_InsertTrackBefore 
(MIDIData* pMIDIData, MIDITrack* pTrack, MIDITrack* pTarget) {
	assert (pMIDIData);
	assert (pTrack);
	/* トラックが既にMIDIデータに属している場合は追加できない。 */
	if (pTrack->m_pPrevTrack || pTrack->m_pNextTrack || pTrack->m_pParent) {
		return 0;
	}
	/* ターゲットトラックの親とMIDIデータが一致しない場合は却下。 */
	if (pTarget) {
		if (pTarget->m_pParent != pMIDIData) {
			return 0;
		}
	}
	/* SMF0における挿入正当性チェック */
	if (pMIDIData->m_lFormat == 0) {
		/* 通常のMIDIデータの場合 */
		if (MIDIData_GetXFVersion (pMIDIData) == 0 && MIDITrack_GetXFVersion (pTrack) == 0) {
			/* 既に1つ以上のトラックを持っている場合、それ以上追加できない */
			if (pMIDIData->m_pFirstTrack || pMIDIData->m_pLastTrack) {
				return 0;
			}
		}
		/* XFデータの場合は挿入できる */
	}
	/* SMF1における挿入正当性チェックは(コンダクタートラックの判定など)しません */
	/* pTargetの直前に挿入する場合 */
	if (pTarget) {
		pTrack->m_pNextTrack = pTarget;
		pTrack->m_pPrevTrack = pTarget->m_pPrevTrack;
		pTrack->m_pParent = pMIDIData;
		if (pTarget->m_pPrevTrack) {
			pTarget->m_pPrevTrack->m_pNextTrack = pTrack;
		}
		else {
			pMIDIData->m_pFirstTrack = pTrack;
		}
		pTarget->m_pPrevTrack = pTrack;
	}
	/* 最後に挿入する場合(pData==NULL) */
	else if (pMIDIData->m_pLastTrack) {
		pTrack->m_pNextTrack = NULL;
		pTrack->m_pPrevTrack = pMIDIData->m_pLastTrack;
		pTrack->m_pParent = pMIDIData;
		pMIDIData->m_pLastTrack->m_pNextTrack = pTrack;
		pMIDIData->m_pLastTrack = pTrack;
	}
	/* 空のトラックに挿入する場合 */
	else {
		pTrack->m_pNextTrack = NULL;
		pTrack->m_pPrevTrack = NULL;
		pTrack->m_pParent = pMIDIData;
		pMIDIData->m_pFirstTrack = pTrack;
		pMIDIData->m_pLastTrack = pTrack;
	}
	pMIDIData->m_lNumTrack++;
	return 1;
}


/* トラックをpTargetの直後に挿入(トラックは予め生成しておく) */
long __stdcall MIDIData_InsertTrackAfter
(MIDIData* pMIDIData, MIDITrack* pTrack, MIDITrack* pTarget) {
	assert (pMIDIData);
	assert (pTrack);
	/* トラックが既にMIDIデータに属している場合は追加できない。 */
	if (pTrack->m_pPrevTrack || pTrack->m_pNextTrack || pTrack->m_pParent) {
		return 0;
	}
	/* ターゲットトラックの親とMIDIデータが一致しない場合は却下。 */
	if (pTarget) {
		if (pTarget->m_pParent != pMIDIData) {
			return 0;
		}
	}
	/* SMF0における挿入正当性チェック */
	if (pMIDIData->m_lFormat == 0) {
		/* 通常のMIDIデータの場合 */
		if (MIDIData_GetXFVersion (pMIDIData) == 0 && MIDITrack_GetXFVersion (pTrack) == 0) {
			/* 既に1つ以上のトラックを持っている場合、それ以上追加できない */
			if (pMIDIData->m_pFirstTrack || pMIDIData->m_pLastTrack) {
				return 0;
			}
		}
		/* XFデータの場合は挿入できる */
	}
	/* SMF1における挿入正当性チェックは(コンダクタートラックの判定など)しません */
	/* pTargetの直後に挿入する場合 */
	if (pTarget) {
		pTrack->m_pNextTrack = pTarget->m_pNextTrack;
		pTrack->m_pPrevTrack = pTarget;
		pTrack->m_pParent = pMIDIData;
		if (pTarget->m_pNextTrack) {
			pTarget->m_pNextTrack->m_pPrevTrack = pTrack;
		}
		else {
			pMIDIData->m_pLastTrack = pTrack;
		}
		pTarget->m_pNextTrack = pTrack;
	}
	/* 最初に挿入する場合(pData==NULL) */
	else if (pMIDIData->m_pFirstTrack) {
		pTrack->m_pNextTrack = pMIDIData->m_pFirstTrack;
		pTrack->m_pPrevTrack = NULL;
		pTrack->m_pParent = pMIDIData;
		pMIDIData->m_pFirstTrack->m_pPrevTrack = pTrack;
		pMIDIData->m_pFirstTrack = pTrack;
	}
	/* 空のトラックに挿入する場合 */
	else {
		pTrack->m_pNextTrack = NULL;
		pTrack->m_pPrevTrack = NULL;
		pTrack->m_pParent = pMIDIData;
		pMIDIData->m_pFirstTrack = pTrack;
		pMIDIData->m_pLastTrack = pTrack;
	}
	pMIDIData->m_lNumTrack++;
	return 1;
}

/* MIDIデータにトラックを追加(トラックは予め生成しておく) */
long __stdcall MIDIData_AddTrack (MIDIData* pMIDIData, MIDITrack* pTrack) {
	assert (pMIDIData != NULL);
	assert (pTrack != NULL);
	return MIDIData_InsertTrackAfter (pMIDIData, pTrack, pMIDIData->m_pLastTrack);
}

/* MIDIデータに強制的にトラックを追加(トラックは予め生成しておく)(非公開関数) */
long __stdcall MIDIData_AddTrackForce (MIDIData* pMIDIData, MIDITrack* pTrack) {
	assert (pMIDIData != NULL);
	assert (pTrack != NULL);
	if (pTrack->m_pPrevTrack != NULL ||
		pTrack->m_pNextTrack != NULL ||
		pTrack->m_pParent != NULL) {
		return 0;
	}
	pTrack->m_pNextTrack = NULL;
	pTrack->m_pPrevTrack = pMIDIData->m_pLastTrack;
	pTrack->m_pParent = pMIDIData;
	if (pMIDIData->m_pLastTrack) {
		pMIDIData->m_pLastTrack->m_pNextTrack = pTrack;
	}
	else {
		pMIDIData->m_pFirstTrack = pTrack;
	}
	pMIDIData->m_pLastTrack = pTrack;
	pMIDIData->m_lNumTrack++;
	return 1;
}

/* MIDIデータ内のトラックを複製する */
long __stdcall MIDIData_DuplicateTrack (MIDIData* pMIDIData, MIDITrack* pTrack) {
	long lCount = 0;
	MIDITrack* pDuplicatedTrack = NULL;
	assert (pMIDIData);
	assert (pTrack);
	assert (pTrack->m_pParent == pMIDIData);
	/* SMFフォーマット0の場合はトラックの複製はできない */
	if (pMIDIData->m_lFormat == 0) {
		return 0;
	}
	/* SMFフォーマット1の最初のトラックは複製できない */
	if (pMIDIData->m_lFormat == 1 && pTrack == pMIDIData->m_pFirstTrack) {
		return 0;
	}
	/* トラックの複製 */
	pDuplicatedTrack = MIDITrack_CreateClone (pTrack);
	if (pDuplicatedTrack == NULL) {
		return 0;
	}
	MIDIData_InsertTrackAfter (pMIDIData, pDuplicatedTrack, pTrack);
	return 1;
}


/* MIDIData内のトラックを除去(トラック自体及びイベントは削除しない) */
long __stdcall MIDIData_RemoveTrack (MIDIData* pMIDIData, MIDITrack* pTrack) {
	assert (pMIDIData);
	assert (pTrack);
	if (pTrack->m_pParent != pMIDIData) {
		return 0;
	}
	if (pTrack->m_pNextTrack) {
		pTrack->m_pNextTrack->m_pPrevTrack = pTrack->m_pPrevTrack;
	}
	else if (pMIDIData->m_pLastTrack) {
		pMIDIData->m_pLastTrack = pTrack->m_pPrevTrack;
	}
	if (pTrack->m_pPrevTrack) {
		pTrack->m_pPrevTrack->m_pNextTrack = pTrack->m_pNextTrack;
	}
	else if (pMIDIData->m_pFirstTrack) {
		pMIDIData->m_pFirstTrack = pTrack->m_pNextTrack;
	}
	pTrack->m_pNextTrack = NULL;
	pTrack->m_pPrevTrack = NULL;
	pTrack->m_pParent = NULL;
	pMIDIData->m_lNumTrack--;
	return 1;
}

/* MIDIデータの削除(トラック内のイベントもろとも跡形もなく) */
void __stdcall MIDIData_Delete (MIDIData* pMIDIData) {
	MIDITrack* pCurTrack;	
	MIDITrack* pNextTrack;
	if (pMIDIData == NULL) {
		return;
	}
	pCurTrack = pMIDIData->m_pFirstTrack;
	while (pCurTrack) {
		pNextTrack = pCurTrack->m_pNextTrack;
		MIDITrack_Delete (pCurTrack);
		pCurTrack = pNextTrack;
	}
	free (pMIDIData);
	return;
}

/* MIDIデータを生成し、MIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_Create (long lFormat, long lNumTrack, long lTimeMode, long lResolution) {
	MIDIData* pMIDIData = NULL;
	MIDITrack* pTrack = NULL;
	long i;

	/* タイムモードと分解能の整合性チェック */
	if (lTimeMode == MIDIDATA_TPQNBASE) {
		if (lResolution < MIDIDATA_MINTPQNRESOLUTION || lResolution > MIDIDATA_MAXTPQNRESOLUTION) {
			return NULL;
		}
	}
	else if (lTimeMode == MIDIDATA_SMPTE24BASE || lTimeMode == MIDIDATA_SMPTE25BASE || 
		lTimeMode == MIDIDATA_SMPTE29BASE || lTimeMode == MIDIDATA_SMPTE30BASE) {
		if (lResolution < MIDIDATA_MINSMPTERESOLUTION || lResolution > MIDIDATA_MAXSMPTERESOLUTION) {
			return NULL;
		}
	}
	else {
		return NULL;
	}

	/* フォーマットとトラック数の整合性チェック */
	if (lFormat == 0) {
		if (lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
			return NULL;
		}
	}
	else if (lFormat == 1 || lFormat == 2) {
		if (lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
			return NULL;
		}
	}
	else {
		return NULL;
	}
	
	pMIDIData = malloc (sizeof(MIDIData));
	if (pMIDIData == NULL) {
		return NULL;
	}
	pMIDIData->m_lFormat = lFormat;
	pMIDIData->m_lNumTrack = 0;
	if (lTimeMode == MIDIDATA_TPQNBASE) {
		pMIDIData->m_lTimeBase = lResolution;
	}
	else {
		pMIDIData->m_lTimeBase = ((256 - lTimeMode) << 8) | lResolution;
	}
	pMIDIData->m_pFirstTrack = NULL;
	pMIDIData->m_pLastTrack = NULL;
	pMIDIData->m_pNextSeq = NULL;
	pMIDIData->m_pPrevSeq = NULL;
	pMIDIData->m_pParent = NULL;
	pMIDIData->m_lReserved1 = 0;
	pMIDIData->m_lReserved2 = 0;
	pMIDIData->m_lReserved3 = 0;
	pMIDIData->m_lReserved4 = 0;
	pMIDIData->m_lUser1 = 0;
	pMIDIData->m_lUser2 = 0;
	pMIDIData->m_lUser3 = 0;
	for (i = 0; i < lNumTrack; i++) {
		pTrack = MIDITrack_Create ();
		if (pTrack == NULL) {
			break;
		}
		MIDIData_AddTrackForce (pMIDIData, pTrack);
	}
	if (i < lNumTrack) {
		MIDIData_Delete (pMIDIData);
		return NULL;
	}
	return pMIDIData;
}

/* MIDIデータの各トラックの出力ポートを自動設定する(非公開) */
long __stdcall MIDIData_UpdateOutputPort (MIDIData* pMIDIData) {
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	assert (pMIDIData);
	forEachTrack (pMIDIData, pMIDITrack) {
		/* ポートプリフィックスイベントで出力ポートが指定されている場合、 */
		/* このトラックの出力ポートは、そのポート番号とする */
		/* ただしポートプリフィックスの値&0x0F(0～15)を採用値とする。 */
		/* ポートプリフィックスイベントがない場合、 */
		/* このトラックの出力ポートは0を設定する。 */
		MIDITrack_SetOutputPort (pMIDITrack, 0);
		forEachEvent (pMIDITrack, pMIDIEvent) {
			if (MIDIEvent_IsPortPrefix (pMIDIEvent)) {
				long lValue = MIDIEvent_GetValue (pMIDIEvent);
				MIDITrack_SetOutputPort (pMIDITrack, (lValue));
				break;
			}
		}
	}
	return 1;
}

/* MIDIデータの各トラックの出力チャンネルを自動設定する(非公開) */
long __stdcall MIDIData_UpdateOutputChannel (MIDIData* pMIDIData) {
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	assert (pMIDIData);
	forEachTrack (pMIDIData, pMIDITrack) {
		/* このトラック内の全MIDIチャンネルイベントのチャンネル番号が一致している場合、 */
		/* このトラックの出力チャンネルは、そのチャンネル番号とする。 */
		/* このトラック内のMIDIチャンネルイベントのチャンネル番号が さまざまである場合、 */
		/* このトラックの出力チャンネルは、n/aとする。 */
		/* このトラック内にMIDIチャンネルイベントがない場合、 */
		/* このトラックの出力チャンネルは、n/aとする。 */
		long lChannelChanged = -1;
		long lNewChannel = -1;
		long lOldChannel = -1;
		forEachEvent (pMIDITrack, pMIDIEvent) {
			if (MIDIEvent_IsMIDIEvent (pMIDIEvent)) {
				lNewChannel = MIDIEvent_GetChannel (pMIDIEvent);
				assert (0 <= lNewChannel && lNewChannel < 16);
				if (lOldChannel != lNewChannel) {
					lChannelChanged ++;
				}
				// 2種類以上のチャンネルのMIDIチャンネルイベントが混ざっている
				if (lChannelChanged >= 1) {
					MIDITrack_SetOutputChannel (pMIDITrack, -1);
					break;
				}
				lOldChannel = lNewChannel;
			}
		}
		// 1種類のチャンネルのMIDIチャンネルイベントのみで構成されていた
		if (lChannelChanged <= 0) {
			MIDITrack_SetOutputChannel (pMIDITrack, lNewChannel);
		}
	}
	return 1;
}

#define MIDITRACK_MODENATIVE          0x00000000 /* Native */
#define MIDITRACK_MODEGM1             0x7E000001 /* GM1 */
#define MIDITRACK_MODEGMOFF           0x7E000002 /* GMOff(=Native) */
#define MIDITRACK_MODEGM2             0x7E000003 /* GM2 */
#define MIDITRACK_MODEGS              0x41000002 /* GS */
#define MIDITRACK_MODE88              0x41000003 /* 88 */
#define MIDITRACK_MODEXG              0x43000002 /* XG */

/* MIDIデータの各トラックの表示モードが「通常」か「ドラム」かを自動設定する(非公開) */
long __stdcall MIDIData_UpdateViewMode (MIDIData* pMIDIData) {
	/* デバイスIDはなんでもよいものとする。 */
	unsigned char byGMReset[] = 
		{0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7};
	unsigned char byGMOff[] = 
		{0xF0, 0x7E, 0x7F, 0x09, 0x02, 0xF7};
	unsigned char byGM2Reset[] = 
		{0xF0, 0x7E, 0x7F, 0x09, 0x03, 0xF7};
	unsigned char byGSReset[] =
		{0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41, 0xF7};
	unsigned char by88Reset[] =
		{0xF0, 0x41, 0x10, 0x42, 0x12, 0x00, 0x00, 0x7F, 0x00, 0x01, 0xF7};
	unsigned char byXGReset[] = 
		{0xF0, 0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00, 0xF7};
	
	MIDITrack* pTempTrack = NULL;
	MIDIEvent* pTempEvent = NULL;
	long lOutputPortMode[MIDIDATA_MAXNUMPORT];
	long lOutputChannelDrum[MIDIDATA_MAXNUMPORT][16];
	memset (lOutputPortMode, 0, sizeof(long) * MIDIDATA_MAXNUMPORT);
	memset (lOutputChannelDrum, 0, sizeof(long) * MIDIDATA_MAXNUMPORT * 16);
	assert (pMIDIData);

	/* 各出力ポートの音源モード(Native/GM/GS/XG/GM2)を取得する */
	forEachTrack (pMIDIData, pTempTrack) {
		long lTempPort = CLIP (0, MIDITrack_GetOutputPort (pTempTrack), MIDIDATA_MAXNUMPORT - 1);
		long lTempChannel = CLIP (-1, MIDITrack_GetOutputChannel (pTempTrack), 15);
		forEachEvent (pTempTrack, pTempEvent) {
			if (MIDIEvent_IsSysExEvent (pTempEvent)) {
				long lLen = MIDIEvent_GetLen (pTempEvent);
				unsigned char bySysxData[256];
				memset (bySysxData, 0, sizeof (bySysxData));
				MIDIEvent_GetData (pTempEvent, bySysxData, MIN (lLen, 255));
				if (memcmp (bySysxData, byGMReset, 6) == 0) {
					lOutputPortMode[lTempPort] = MIDITRACK_MODEGM1;
					break;
				}
				if (memcmp (bySysxData, byGMOff, 6) == 0) {
					lOutputPortMode[lTempPort] = MIDITRACK_MODEGMOFF;
					break;
				}
				if (memcmp (bySysxData, byGM2Reset, 6) == 0) {
					lOutputPortMode[lTempPort] = MIDITRACK_MODEGM2;
					break;
				}
				if (memcmp (bySysxData, byGSReset, 11) == 0) {
					lOutputPortMode[lTempPort] = MIDITRACK_MODEGS;
					break;
				}
				if (memcmp (bySysxData, by88Reset, 11) == 0) {
					lOutputPortMode[lTempPort] = MIDITRACK_MODE88;
					break;
				}
				if (memcmp (bySysxData, byXGReset, 9) == 0) {
					lOutputPortMode[lTempPort] = MIDITRACK_MODEXG;
					break;
				}
				/* TODO:ここに新型音源のリセットメッセージへ対応してください。 */
			}
		}
	}

	/* 出力ポートのチャンネルごとに通常(0)かドラム(1)かの分類を行う。*/
	forEachTrack (pMIDIData, pTempTrack) {
		long lTempPort = CLIP (0, MIDITrack_GetOutputPort (pTempTrack), MIDIDATA_MAXNUMPORT - 1);
		long lTempChannel = CLIP (-1, MIDITrack_GetOutputChannel (pTempTrack), 15);
		long lTempPortMode = lOutputPortMode[lTempPort];
		/* このトラックの出力ポートの音源初期化モードが */
		switch (lTempPortMode) {
		/* GM2の場合 */
		case MIDITRACK_MODEGM2:
			forEachEvent (pTempTrack, pTempEvent) {
				/* CC#0(Bank Select MSB)の場合 */
				if (MIDIEvent_IsControlChange (pTempEvent)) {
					if (MIDIEvent_GetNumber (pTempEvent) == 0) {
						long lChannel = lTempChannel;
						if (lTempChannel == -1) {
							lChannel = MIDIEvent_GetChannel (pTempEvent);
						}
						if (MIDIEvent_GetValue (pTempEvent) == 120) {
							lOutputChannelDrum[lTempPort][lChannel] = 1;
						}
						else {
							lOutputChannelDrum[lTempPort][lChannel] = 0;
						}
					}
				}
			}
			break;
		/* XGの場合 */
		case MIDITRACK_MODEXG:
			forEachEvent (pTempTrack, pTempEvent) {
				/* CC#0(Bank Select MSB)の場合 */
				if (MIDIEvent_IsControlChange (pTempEvent)) {
					if (MIDIEvent_GetNumber (pTempEvent) == 0) {
						long lChannel = lTempChannel;
						if (lTempChannel == -1) {
							lChannel = MIDIEvent_GetChannel (pTempEvent);
						}
						if (MIDIEvent_GetValue (pTempEvent) == 127) {
							lOutputChannelDrum[lTempPort][lChannel] = 1;
						}
						else {
							lOutputChannelDrum[lTempPort][lChannel] = 0;
						}
					}
				}
			}
			break;
		/* GS/88の場合 */
		case MIDITRACK_MODEGS:
		case MIDITRACK_MODE88:
			lOutputChannelDrum[lTempPort][9] = 1;
			forEachEvent (pTempTrack, pTempEvent) {
				/* システムエクスクルーシブの場合 */
				if (MIDIEvent_IsSysExEvent (pTempEvent)) {
					long lLen = MIDIEvent_GetLen (pTempEvent);
					unsigned char bySysxData[256];
					memset (bySysxData, 0, sizeof (bySysxData));
					MIDIEvent_GetData (pTempEvent, bySysxData, MIN (lLen, 255));
					/* パートモードメッセージの場合 */
					/* {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x15, 0xvv, 0xcs, 0xF7} */
					if (bySysxData[0] == 0xF0 &&
						bySysxData[1] == 0x41 &&
						bySysxData[3] == 0x42 &&
						bySysxData[4] == 0x12 &&
						bySysxData[5] == 0x40 &&
						(bySysxData[6] & 0xF0) == 0x10 &&
						bySysxData[7] == 0x15 &&
						bySysxData[9] == 128 - ((bySysxData[5] + bySysxData[6] + 
										bySysxData[7] + bySysxData[8]) % 128) &&
						bySysxData[10] == 0xF7) {
						long lChannel = 0;
						if (bySysxData[6] == 0x10) {
							lChannel = 9;
						}
						else if (0x11 <= bySysxData[6] && bySysxData[6] <= 0x19) {
							lChannel = bySysxData[6] - 0x11;
						}
						else {
							lChannel = bySysxData[6] - 0x10;
						}
						lOutputChannelDrum[lTempPort][lChannel] = bySysxData[8] > 0 ? 1 : 0;
					}
				}
			}
			break;
		/* Native/GMOff/GMの場合 */
		default:
			/* 出力チャンネル9に設定されている場合のみドラム */
			if (lTempChannel == 9) {
				lOutputChannelDrum[lTempPort][lTempChannel] = 1;
			}
			else if (0 <= lTempChannel && lTempChannel < 16) {
				lOutputChannelDrum[lTempPort][lTempChannel] = 0;
			}
			break;
		}
	}

	/* 各トラックについて、表示モードが「通常」か「ドラム」か設定 */
	forEachTrack (pMIDIData, pTempTrack) {
		long lOutputPort = MIDITrack_GetOutputPort (pTempTrack);
		long lOutputChannel = MIDITrack_GetOutputChannel (pTempTrack);
		if (0 <= lOutputPort && lOutputPort < MIDIDATA_MAXNUMPORT &&
			0 <= lOutputChannel && lOutputChannel < 16) {
			MIDITrack_SetViewMode (pTempTrack, lOutputChannelDrum[lOutputPort][lOutputChannel]);
		}
		else {
			MIDITrack_SetViewMode (pTempTrack, 0);
		}
	}
	return 1;
}


/* フォーマット取得 */
long __stdcall MIDIData_GetFormat (MIDIData* pMIDIData) {
	assert (pMIDIData);
	return pMIDIData->m_lFormat;
}

/* フォーマット設定 */
/* (注意：この関数は未デバッグです) */
long __stdcall MIDIData_SetFormat (MIDIData* pMIDIData, long lFormat) {
	long i, j;
	MIDITrack* pTrack = NULL;
	long lXFVersion = 0;
	assert (pMIDIData);
	assert (0 <= lFormat && lFormat <= 2);
	lXFVersion = MIDIData_GetXFVersion (pMIDIData);

	/* 0→1又は0→2の場合、コンダクタートラックと16個のMIDIトラック(計17トラック)へ分裂 */
	/* (XFデータの場合は、セットアップと16トラックとXFIHとXFKM(計19トラック)へ分裂) */
	if (pMIDIData->m_lFormat == 0 && (lFormat == 1 || lFormat == 2)) {
		long lNumAddTrack = (lXFVersion == 0 ? 17 : 19);
		MIDIEvent* pEvent;
		MIDITrack* pNewTrack[19];

		/* 追加用の新しいトラック作成 */
		for (i = 0; i < lNumAddTrack; i++) {
			pNewTrack[i] = MIDITrack_Create ();
			if (pNewTrack[i] == NULL) {
				for (j = 0; j < i; j++) {
					MIDITrack_Delete (pNewTrack[j]);
				}
				return 0;
			}
		}

		/* 各トラックごとに(トラックが残っている限り) */
		while ((pTrack = pMIDIData->m_pFirstTrack)) {
			/* このトラック内のイベント除外し、新しいトラックに割り振る */
			while ((pEvent = pTrack->m_pFirstEvent)) {
				/* MIDIチャンネルイベントの場合、チャンネルに応じてトラック[1]～[16]へ */
				if (MIDIEvent_IsMIDIEvent (pEvent)) {
					MIDITrack_RemoveEvent (pTrack, pEvent);
					MIDITrack_InsertEvent (pNewTrack[MIDIEvent_GetChannel (pEvent) + 1], pEvent);
				}
				/* エンドオブトラックの場合、それを削除 */
				else if (MIDIEvent_IsEndofTrack (pEvent)) {
					MIDIEvent_Delete (pEvent);
				}
				/* メタイベント・システムエクスクルーシヴイベントの場合 */
				else {
					/* XFデータの場合 */
					if (lXFVersion != 0) {
						/* XFで始まるテキストはトラック[17]へ、その他のテキストはトラック[0]へ */
						if (pEvent->m_lKind == MIDIEVENT_TEXTEVENT) {
							if (pEvent->m_lLen >= 4) {
								if (strncmp ((char*)(pEvent->m_pData), "XF", 2) == 0) {
									MIDITrack_RemoveEvent (pTrack, pEvent);
									MIDITrack_InsertEvent (pNewTrack[17], pEvent);
								}
								else {
									MIDITrack_RemoveEvent (pTrack, pEvent);
									MIDITrack_InsertEvent (pNewTrack[0], pEvent);
								}
							}
							else {
								MIDITrack_RemoveEvent (pTrack, pEvent);
								MIDITrack_InsertEvent (pNewTrack[0], pEvent);
							}
						}
						/* 歌詞はトラック[18]へ */
						else if (pEvent->m_lKind == MIDIEVENT_LYRIC) {
							MIDITrack_RemoveEvent (pTrack, pEvent);
							MIDITrack_InsertEvent (pNewTrack[18], pEvent);
						}
						/* キューポイントはトラック[18]へ */
						else if (pEvent->m_lKind == MIDIEVENT_CUEPOINT) {
							MIDITrack_RemoveEvent (pTrack, pEvent);
							MIDITrack_InsertEvent (pNewTrack[18], pEvent);
						}
						/* その他の場合、トラック[0]へ */
						else {
							MIDITrack_RemoveEvent (pTrack, pEvent);
							MIDITrack_InsertEvent (pNewTrack[0], pEvent);
						}
					}
					/* 通常のMIDIデータの場合、トラック[0]へ */
					else {
						MIDITrack_RemoveEvent (pTrack, pEvent);
						MIDITrack_InsertEvent (pNewTrack[0], pEvent);
					}
				}
			}
			/* このMIDIトラックを削除 */
			MIDITrack_Delete (pTrack);
		}

		/* 追加用の新しいトラックにEOTをつけてMIDIDataに追加 */
		for (i = 0; i < lNumAddTrack; i++) {
			long lLastTime = 0;
			pEvent = pNewTrack[i]->m_pLastEvent;
			if (pEvent) {
				lLastTime = pEvent->m_lTime;
			}
			MIDITrack_InsertEndofTrack (pNewTrack[i], lLastTime);
			MIDIData_AddTrackForce (pMIDIData, pNewTrack[i]);
		}
	}

	/* 1→0又は2→0の場合、全トラックを1トラックに集約 */
	/* (XFの場合、MThdトラック、XFIHトラック、XFKMトラックの3トラックに集約) */
	else if ((pMIDIData->m_lFormat == 1 || pMIDIData->m_lFormat == 2) && lFormat == 0) {
		long lNumAddTrack = (lXFVersion == 0 ? 1 : 3);
		MIDIEvent* pEvent = NULL;
		MIDITrack* pMIDITrack = NULL;
		MIDITrack* pNewTrack[3];
		/* 新しい追加用トラック確保 */
		for (i = 0; i < lNumAddTrack; i++) {
			pNewTrack[i] = MIDITrack_Create ();
			if (pNewTrack[i] == NULL) {
				for (j = 0; j < i; j++) {
					MIDITrack_Delete (pNewTrack[j]);
				}
				return 0;
			}
		}

		/* 各トラックごとに(トラックが残っている限り) */
		while ((pMIDITrack = pMIDIData->m_pFirstTrack)) {
			/* このトラック内のイベント除外し、新しいトラックに割り振る */
			while ((pEvent = pMIDITrack->m_pFirstEvent)) {
				/* トラック名の場合 */
				if (pEvent->m_lKind == MIDIEVENT_TRACKNAME) {
					MIDITrack_RemoveEvent (pMIDITrack, pEvent);
					/* pEvent->m_lKind = MIDIEVENT_TEXTEVENT; 20090112廃止 */
					MIDITrack_InsertEvent (pNewTrack[0], pEvent);
				}
				/* エンドオブトラックの場合、そのイベントを削除 */
				else if (pEvent->m_lKind == MIDIEVENT_ENDOFTRACK) {
					MIDIEvent_Delete (pEvent);
				}
				/* その他のイベントの場合 */
				else {
					/* XFデータの場合 */
					if (lXFVersion != 0) {
						/* XFで始まるテキストイベントはトラック[1]へ、その他のテキストはトラック[0]へ */
						if (pEvent->m_lKind == MIDIEVENT_TEXTEVENT) {
							if (pEvent->m_lLen >= 4) {
								if (strncmp ((char*)(pEvent->m_pData), "XF", 2) == 0) {
									MIDITrack_RemoveEvent (pMIDITrack, pEvent);
									MIDITrack_InsertEvent (pNewTrack[1], pEvent);
								}
								else {
									MIDITrack_RemoveEvent (pMIDITrack, pEvent);
									MIDITrack_InsertEvent (pNewTrack[0], pEvent);
								}
							}
							else {
								MIDITrack_RemoveEvent (pMIDITrack, pEvent);
								MIDITrack_InsertEvent (pNewTrack[0], pEvent);
							}
						}
						/* キューポイントはトラック[2]へ */
						else if (pEvent->m_lKind == MIDIEVENT_CUEPOINT) {
							MIDITrack_RemoveEvent (pMIDITrack, pEvent);
							MIDITrack_InsertEvent (pNewTrack[2], pEvent);
						}
						/* 歌詞はトラック[2]へ */
						else if (pEvent->m_lKind == MIDIEVENT_LYRIC) {
							MIDITrack_RemoveEvent (pMIDITrack, pEvent);
							MIDITrack_InsertEvent (pNewTrack[2], pEvent);
						}
						/* その他はトラック[0]へ */
						else {
							MIDITrack_RemoveEvent (pMIDITrack, pEvent);
							MIDITrack_InsertEvent (pNewTrack[0], pEvent);
						}
					}
					/* 通常のMIDIデータの場合、すべてトラック[0]へ */
					else {
						MIDITrack_RemoveEvent (pMIDITrack, pEvent);
						MIDITrack_InsertEvent (pNewTrack[0], pEvent);
					}
				}
			}
			/* このMIDIトラックを削除 */
			MIDITrack_Delete (pMIDITrack);
		}

		/* 追加用の新しいトラックにEOTをつけてMIDIDataに追加 */
		for (i = 0; i < lNumAddTrack; i++) {
			long lLastTime = 0;
			pEvent = pNewTrack[i]->m_pLastEvent;
			if (pEvent) {
				lLastTime = pEvent->m_lTime;
			}
			MIDITrack_InsertEndofTrack (pNewTrack[i], lLastTime);
			MIDIData_AddTrackForce (pMIDIData, pNewTrack[i]);
		}
	}
	/* その他のフォーマット変換又は変換なし */
	else {
		return 0;
	}

	/* フォーマットの確定 */
	pMIDIData->m_lFormat = lFormat;

	/* 各トラックの出力ポート番号・出力チャンネル・表示モードの自動設定 */
	MIDIData_UpdateOutputPort (pMIDIData);
	MIDIData_UpdateOutputChannel (pMIDIData);
	MIDIData_UpdateViewMode (pMIDIData);

	/* 各トラックの入力ポート番号・入力チャンネルの設定 */
	forEachTrack (pMIDIData, pTrack) {
		pTrack->m_lInputOn = 1;
		pTrack->m_lInputPort = pTrack->m_lOutputPort;
		pTrack->m_lInputChannel = pTrack->m_lOutputChannel;
		pTrack->m_lOutputOn = 1;
	}

	return 1;
}

/* タイムベース取得 */
long __stdcall MIDIData_GetTimeBase (MIDIData* pMIDIData, long* pMode, long* pResolution) {
	assert (pMIDIData);
	if (pMIDIData->m_lTimeBase & 0x00008000) {
		*pMode = 256 - ((pMIDIData->m_lTimeBase & 0x0000FF00) >> 8);
		*pResolution = pMIDIData->m_lTimeBase & 0x000000FF;
	}
	else {
		*pMode = MIDIDATA_TPQNBASE;
		*pResolution = pMIDIData->m_lTimeBase & 0x00007FFF;
	}
	return 1;
}

/* タイムベースのうち、タイムモード取得 */
long __stdcall MIDIData_GetTimeMode (MIDIData* pMIDIData) {
	assert (pMIDIData);
	if (pMIDIData->m_lTimeBase & 0x00008000) {
		return 256 - ((pMIDIData->m_lTimeBase & 0x0000FF00) >> 8);
	}
	else {
		return MIDIDATA_TPQNBASE;
	}
}

/* タイムベースのうち、レゾリューション取得 */
long __stdcall MIDIData_GetTimeResolution (MIDIData* pMIDIData) {
	assert (pMIDIData);
	if (pMIDIData->m_lTimeBase & 0x00008000) {
		return pMIDIData->m_lTimeBase & 0x000000FF;
	}
	else {
		return pMIDIData->m_lTimeBase & 0x00007FFF;
	}
}

/* タイムベース設定 */
long __stdcall MIDIData_SetTimeBase (MIDIData* pMIDIData, long lMode, long lResolution) {
	MIDIEvent* pEvent;
	MIDITrack* pTrack;
	long lOldMode;
	long lOldResolution;
	assert (pMIDIData);
	/* 引数(モードと分解能)の整合性チェック */
	if (lMode == MIDIDATA_TPQNBASE) {
		if (lResolution < MIDIDATA_MINTPQNRESOLUTION || lResolution > MIDIDATA_MAXTPQNRESOLUTION) {
			return 0;
		}
	}
	else if (lMode == MIDIDATA_SMPTE24BASE || lMode == MIDIDATA_SMPTE25BASE || 
		lMode == MIDIDATA_SMPTE29BASE || lMode == MIDIDATA_SMPTE30BASE) {
		if (lResolution < MIDIDATA_MINSMPTERESOLUTION || lResolution > MIDIDATA_MAXSMPTERESOLUTION) {
			return 0;
		}
	}
	else {
		return 0;
	}
	/* 変更前のモードと分解能を得る */
	MIDIData_GetTimeBase (pMIDIData, &lOldMode, &lOldResolution);

	/* TPQNベースからTPQNベースへ変換時 */
	if (lOldMode == 0 && lMode == 0) {
		int64_t llResolution = (int64_t)lResolution;
		int64_t llTimeBase = (int64_t)pMIDIData->m_lTimeBase;
		forEachTrack (pMIDIData, pTrack) {
			forEachEvent (pTrack, pEvent) {
				int64_t llOldTime = pEvent->m_lTime;
				int64_t llNewTime = llOldTime * llResolution / llTimeBase;
				pEvent->m_lTime = (long)(CLIP (0, llNewTime, 0x7FFFFFFF));
			}
		}
		pMIDIData->m_lTimeBase = lResolution;
	}

	/* SMPTEベースからSMPTEベースへ変換時 */
	else if (lOldMode > 0 && lMode > 0) {
		int64_t llOldModeResolution = (int64_t)lOldMode * (int64_t)lOldResolution;
		int64_t llModeResolution = (int64_t)lMode * (int64_t)lResolution;
		forEachTrack (pMIDIData, pTrack) {
			forEachEvent (pTrack, pEvent) {
				int64_t llOldTime = pEvent->m_lTime;
				int64_t llNewTime = pEvent->m_lTime * llModeResolution / llOldModeResolution; 
				pEvent->m_lTime = (long)(CLIP (0, llNewTime, 0x7FFFFFFF));
			}
		}
		pMIDIData->m_lTimeBase = ((256 - lMode) << 8) | lResolution;
	}

	/* TPQNベースからSMPTEベースへ変換時 */
	else if (lOldMode == 0 && lMode > 0) {
		long lModeResolution = lMode * lResolution;
		MIDITrack* pSetupTrack = pMIDIData->m_pFirstTrack;
		/* forループ内でタイムを追うので、テンポマッパーは最後にタイム変換すること。 */
		forEachTrackInverse (pMIDIData, pTrack) {
			if (pMIDIData->m_lFormat == 2) {
				pSetupTrack = pTrack;
			}
			forEachEventInverse (pTrack, pEvent) {
				pEvent->m_lTime = MIDITrack_TPQNtoSMPTE 
					(pSetupTrack, pEvent->m_lTime, lMode, lResolution);
			}
		}
		pMIDIData->m_lTimeBase = ((256 - lMode) << 8) | lResolution;
	}

	/* SMPTEベースからTPQNベースへ変換時 */
	else if (lOldMode > 0 && lMode == 0) {
		long lOldModeResolution = lOldMode * lOldResolution;
		MIDITrack* pSetupTrack = pMIDIData->m_pFirstTrack;
		/* forループ内でタイムを追うので、テンポマッパーは最後にタイム変換すること。 */
		forEachTrackInverse (pMIDIData, pTrack) {
			if (pMIDIData->m_lFormat == 2) {
				pSetupTrack = pTrack;
			}
			forEachEventInverse (pTrack, pEvent) {
				pEvent->m_lTime = MIDITrack_SMPTEtoTPQN (pSetupTrack, pEvent->m_lTime, lResolution); 
			}
		}
		pMIDIData->m_lTimeBase = lResolution;
	}
	return 1;
}

/* トラック数取得 */
long __stdcall MIDIData_GetNumTrack (MIDIData* pMIDIData) {
	assert (pMIDIData);
	return pMIDIData->m_lNumTrack;
}

/* トラック数をカウントし、各トラックのインデックスと総トラック数を更新し、トラック数を返す。 */
long __stdcall MIDIData_CountTrack (MIDIData* pMIDIData) {
	MIDITrack* pTrack;
	long i = 0;
	assert (pMIDIData);
	forEachTrack (pMIDIData, pTrack) {
		pTrack->m_lTempIndex = i;
		i++;
	}
	pMIDIData->m_lNumTrack = i;
	return i;
}

/* XFであるとき、XFのヴァージョンを取得(XFでなければ0) */
long __stdcall MIDIData_GetXFVersion (MIDIData* pMIDIData) {
	/* シーケンサ固有のイベントで */
	/* {43 7B 00 58 46 Mj Mn S1 S0} を探し、*/
	/* Mj | (Mn << 8) | (S1 << 16) | (S0 << 24) を返す。*/
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_GetXFVersion (pMIDIData->m_pFirstTrack);
	}
	return 0;
}

/* 最初のトラックへのポインタ取得 */
MIDITrack* __stdcall MIDIData_GetFirstTrack (MIDIData* pMIDIData) {
	assert (pMIDIData);
	return pMIDIData->m_pFirstTrack;
}

/* 最後のトラックへのポインタ取得 */
MIDITrack* __stdcall MIDIData_GetLastTrack (MIDIData* pMIDIData) {
	assert (pMIDIData);
	return pMIDIData->m_pLastTrack;
}

/* 指定インデックスのMIDIトラックへのポインタを取得する(なければNULL) */
MIDITrack* __stdcall MIDIData_GetTrack (MIDIData* pMIDIData, long lTrackIndex) {
	int i = 0;
	MIDITrack* pMIDITrack = NULL;
	assert (pMIDIData);
	forEachTrack (pMIDIData, pMIDITrack) {
		if (i == lTrackIndex) {
			return pMIDITrack;
		}
		i++;
	}
	return NULL;
}

/* MIDIデータの開始時刻[Tick]を返す。 */
long __stdcall MIDIData_GetBeginTime (MIDIData* pMIDIData) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	long lTime, lBeginTime;
	assert (pMIDIData);
	lBeginTime = LONG_MAX;
	forEachTrack (pMIDIData, pTrack) {
		pEvent = pTrack->m_pFirstEvent;
		if (pEvent) {
			lTime = pEvent->m_lTime;
			if (lBeginTime > lTime) {
				lBeginTime = lTime;	
			}
		}
	}
	return lBeginTime;
}

/* MIDIデータの終了時刻[Tick]を返す。 */
long __stdcall MIDIData_GetEndTime (MIDIData* pMIDIData) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	long lTime, lEndTime;
	assert (pMIDIData);
	lEndTime = LONG_MIN;
	forEachTrack (pMIDIData, pTrack) {
		pEvent = pTrack->m_pLastEvent;
		if (pEvent) {
			lTime = pEvent->m_lTime;
			if (lEndTime < lTime) {
				lEndTime = lTime;	
			}
		}

	}
	return lEndTime;
}

/* MIDIデータのタイトル簡易取得(ANSI) */
char* __stdcall MIDIData_GetTitleA (MIDIData* pMIDIData, char* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	memset (pData, 0, lLen);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return pData;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL) {
		return pData;
	}
	return MIDIEvent_GetTextA (pEvent, pData, lLen);

}

/* MIDIデータのタイトル簡易取得(UNICODE) */
wchar_t* __stdcall MIDIData_GetTitleW (MIDIData* pMIDIData, wchar_t* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	memset (pData, 0, lLen * sizeof (wchar_t));
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return pData;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL) {
		return pData;
	}
	return MIDIEvent_GetTextW (pEvent, pData, lLen);

}


/* MIDIデータのタイトル簡易設定(ANSI) */
long __stdcall MIDIData_SetTitleA (MIDIData* pMIDIData, const char* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertTrackNameA (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	return MIDIEvent_SetTextA (pEvent, pszText);
}

/* MIDIデータのタイトル簡易設定(UNICODE) */
long __stdcall MIDIData_SetTitleW (MIDIData* pMIDIData, const wchar_t* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertTrackNameW (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	return MIDIEvent_SetTextW (pEvent, pszText);
}


/* MIDIデータのサブタイトル簡易取得(ANSI) */
char* __stdcall MIDIData_GetSubTitleA (MIDIData* pMIDIData, char* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	long nCount = 0;
	assert (pMIDIData);
	memset (pData, 0, lLen);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return pData;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {			
			if (nCount >= 1) {
				break;
			}
			nCount++;
		}
	}
	if (pEvent == NULL) {
		return pData;
	}
	return MIDIEvent_GetTextA (pEvent, pData, lLen);
}

/* MIDIデータのサブタイトル簡易取得(UNICODE) */
wchar_t* __stdcall MIDIData_GetSubTitleW (MIDIData* pMIDIData, wchar_t* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	long nCount = 0;
	assert (pMIDIData);
	memset (pData, 0, lLen * sizeof (wchar_t));
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return pData;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {			
			if (nCount >= 1) {
				break;
			}
			nCount++;
		}
	}
	if (pEvent == NULL) {
		return pData;
	}
	return MIDIEvent_GetTextW (pEvent, pData, lLen);
}

/* MIDIデータのサブタイトル簡易設定(ANSI) */
long __stdcall MIDIData_SetSubTitleA (MIDIData* pMIDIData, const char* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	long nCount = 0;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {			
			if (nCount >= 1) {
				break;
			}
			nCount++;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertTrackNameA (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	return MIDIEvent_SetTextA (pEvent, pszText);
}

/* MIDIデータのサブタイトル簡易設定(UNICODE) */
long __stdcall MIDIData_SetSubTitleW (MIDIData* pMIDIData, const wchar_t* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	long nCount = 0;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTrackName (pEvent)) {			
			if (nCount >= 1) {
				break;
			}
			nCount++;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertTrackNameW (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	return MIDIEvent_SetTextW (pEvent, pszText);
}

/* MIDIデータの著作権簡易取得(ANSI) */
char* __stdcall MIDIData_GetCopyrightA (MIDIData* pMIDIData, char* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	memset (pData, 0, lLen);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return pData;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsCopyrightNotice (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL) {
		return pData;
	}
	return MIDIEvent_GetTextA (pEvent, pData, lLen);
}

/* MIDIデータの著作権簡易取得(UNICODE) */
wchar_t* __stdcall MIDIData_GetCopyrightW (MIDIData* pMIDIData, wchar_t* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	memset (pData, 0, lLen * sizeof (wchar_t));
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return pData;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsCopyrightNotice (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL) {
		return pData;
	}
	return MIDIEvent_GetTextW (pEvent, pData, lLen);
}

/* MIDIデータの著作権簡易設定(ANSI) */
long __stdcall MIDIData_SetCopyrightA (MIDIData* pMIDIData, const char* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsCopyrightNotice (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertCopyrightNoticeA (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	else {
		return MIDIEvent_SetTextA (pEvent, pszText);
	}
}

/* MIDIデータの著作権簡易設定(UNICODE) */
long __stdcall MIDIData_SetCopyrightW (MIDIData* pMIDIData, const wchar_t* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsCopyrightNotice (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertCopyrightNoticeW (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	else {
		return MIDIEvent_SetTextW (pEvent, pszText);
	}
}

/* MIDIデータのコメント簡易取得(ANSI) */
char* __stdcall MIDIData_GetCommentA (MIDIData* pMIDIData, char* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	memset (pData, 0, lLen);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTextEvent (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL) {
		return 0;
	}
	return MIDIEvent_GetTextA (pEvent, pData, lLen);
}

/* MIDIデータのコメント簡易取得(UNICODE) */
wchar_t* __stdcall MIDIData_GetCommentW (MIDIData* pMIDIData, wchar_t* pData, long lLen) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	memset (pData, 0, lLen * sizeof (wchar_t));
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTextEvent (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL) {
		return 0;
	}
	return MIDIEvent_GetTextW (pEvent, pData, lLen);
}

/* MIDIデータのコメント簡易設定(ANSI) */
long __stdcall MIDIData_SetCommentA (MIDIData* pMIDIData, const char* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTextEvent (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertTextEventA (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	return MIDIEvent_SetTextA (pEvent, pszText);
}

/* MIDIデータのコメント簡易設定(UNICODE) */
long __stdcall MIDIData_SetCommentW (MIDIData* pMIDIData, const wchar_t* pszText) {
	MIDITrack* pTrack;
	MIDIEvent* pEvent;
	assert (pMIDIData);
	pTrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pTrack == NULL) {
		return 0;
	}
	forEachEvent (pTrack, pEvent) {
		if (MIDIEvent_IsTextEvent (pEvent)) {
			break;
		}
	}
	if (pEvent == NULL && pszText != NULL) {
		return MIDITrack_InsertTextEventW (pTrack, 0, pszText);
	}
	else if (pEvent == NULL && pszText == NULL) {
		return 1;
	}
	else if (pEvent != NULL && pszText == NULL) {
		MIDIEvent_DeleteSingle (pEvent);
		return 1;
	}
	return MIDIEvent_SetTextW (pEvent, pszText);
}

/* タイムスタンプをミリ秒時刻に変換 */
long __stdcall MIDIData_TimeToMillisec (MIDIData* pMIDIData, long lTime) {
	long lTimeMode, lTimeResolution;
	MIDIData_GetTimeBase (pMIDIData, &lTimeMode, &lTimeResolution);
	if (lTimeMode == MIDIDATA_TPQNBASE) {
		if (pMIDIData->m_pFirstTrack) {
			return MIDITrack_TimeToMillisec (pMIDIData->m_pFirstTrack, lTime);
		}
		return 0;
	}
	else {
		switch (lTimeMode) {
		case MIDIDATA_SMPTE24BASE:
			return lTime * 1000 / lTimeResolution / 24;
		case MIDIDATA_SMPTE25BASE:
			return lTime * 1000 / lTimeResolution / 25;
		case MIDIDATA_SMPTE29BASE:
			return lTime * 1000 / lTimeResolution * 1000 / 2997;
		case MIDIDATA_SMPTE30BASE:
			return lTime * 1000 / lTimeResolution / 30;
		default:
			return 0;
		}
	}
}

/* ミリ秒時刻をタイムスタンプに変換 */
long __stdcall MIDIData_MillisecToTime (MIDIData* pMIDIData, long lMillisec) {
	long lTimeMode, lTimeResolution;
	MIDIData_GetTimeBase (pMIDIData, &lTimeMode, &lTimeResolution);
	if (lTimeMode == MIDIDATA_TPQNBASE) {
		if (pMIDIData->m_pFirstTrack) {
			return MIDITrack_MillisecToTime (pMIDIData->m_pFirstTrack, lMillisec);
		}
		return 0;
	}
	else {
		switch (lTimeMode) {
		case MIDIDATA_SMPTE24BASE:
			return lTimeResolution * lMillisec * 24 / 1000;
		case MIDIDATA_SMPTE25BASE:
			return lTimeResolution * lMillisec * 25 / 1000;
		case MIDIDATA_SMPTE29BASE:
			return lTimeResolution * lMillisec * 2997 / 1000 / 1000;
		case MIDIDATA_SMPTE30BASE:
			return lTimeResolution * lMillisec * 30 / 1000;
		default:
			return 0;
		}
	}
}

/* タイムコードを小節：拍：ティックに分解(最初のトラック内の拍子記号から計算) */
long __stdcall MIDIData_BreakTimeEx
(MIDIData* pMIDIData, long lTime, long* pMeasure, long* pBeat, long* pTick,
 long* pnn, long* pdd, long* pcc, long* pbb) {
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_BreakTimeEx (pMIDIData->m_pFirstTrack,
			lTime, pMeasure, pBeat, pTick, pnn, pdd, pcc, pbb);
	}
	return 0;
}

/* タイムコードを小節：拍：ティックに分解(最初のトラック内の拍子記号から計算) */
long __stdcall MIDIData_BreakTime 
(MIDIData* pMIDIData, long lTime, long* pMeasure, long* pBeat, long* pTick) {
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_BreakTime (pMIDIData->m_pFirstTrack, 
			lTime, pMeasure, pBeat, pTick);
	}
	return 0;
}

/* 小節：拍：ティックからタイムコードを生成(最初のトラック内の拍子記号から計算) */
long __stdcall MIDIData_MakeTimeEx 
(MIDIData* pMIDIData, long lMeasure, long lBeat, long lTick, 
 long* pTime, long* pnn, long* pdd, long* pcc, long* pbb) {
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_MakeTimeEx (pMIDIData->m_pFirstTrack,
			lMeasure, lBeat, lTick, pTime, pnn, pdd, pcc, pbb);
	}
	return 0;
}

/* 小節：拍：ティックからタイムコードを生成(最初のトラック内の拍子記号から計算) */
long __stdcall MIDIData_MakeTime 
(MIDIData* pMIDIData, long lMeasure, long lBeat, long lTick, long* pTime) {
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_MakeTime (pMIDIData->m_pFirstTrack, 
			lMeasure, lBeat, lTick, pTime);
	}
	return 0;
}

/* 指定位置の直前のテンポを取得 */
long __stdcall MIDIData_FindTempo
(MIDIData* pMIDIData, long lTime, long* pTempo) {
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_FindTempo (pMIDIData->m_pFirstTrack, lTime, pTempo);
	}
	return 0;
}

/* 指定位置の直前の拍子記号を取得 */
long __stdcall MIDIData_FindTimeSignature
(MIDIData* pMIDIData, long lTime, long* pnn, long* pdd, long* pcc, long* pbb) {
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_FindTimeSignature
			(pMIDIData->m_pFirstTrack, lTime, pnn, pdd, pcc, pbb);
	}
	return 0;
}

/* 指定位置の直前の調性記号を取得 */
long __stdcall MIDIData_FindKeySignature
(MIDIData* pMIDIData, long lTime, long* psf, long* pmi) {
	if (pMIDIData->m_pFirstTrack) {
		return MIDITrack_FindKeySignature 
			(pMIDIData->m_pFirstTrack, lTime, psf, pmi);
	}
	return 0;
}

/* MIDIデータに別のMIDIデータをマージする */
long __stdcall MIDIData_Merge (MIDIData* pMIDIData, MIDIData* pMergeData, 
	long lTime, long lFlags, long* pInsertedEventCount, long* pDeletedEventCount) {
	
	long lNumMergeTrack = MIDIData_CountTrack (pMergeData);
	long lTimeMode, lTimeResolution;
	long ret;
	MIDITrack* pMergeTrack;
	MIDIEvent* pMergeEvent;
	MIDITrack* pMIDITrack = pMIDIData->m_pFirstTrack;
	MIDIEvent* pMIDIEvent;
	*pInsertedEventCount = 0;
	*pDeletedEventCount = 0;
	
	/* マージ用MIDIデータのフォーマットをこのMIDIデータのフォーマットに合わせる。 */
	if (pMIDIData->m_lFormat != pMergeData->m_lFormat) {
		MIDIData_SetFormat (pMergeData, pMIDIData->m_lFormat); 
	}

	/* マージ用MIDIデータのタイムベースをこのMIDIデータのタイムベースに合わせる。 */
	MIDIData_GetTimeBase (pMIDIData, &lTimeMode, &lTimeResolution);
	if (pMIDIData->m_lTimeBase != pMergeData->m_lTimeBase) {
		MIDIData_SetTimeBase (pMergeData, lTimeMode, lTimeResolution);
	}

	/* 挿入先トラックの存在検査 */
	pMIDITrack = pMIDIData->m_pFirstTrack;
	forEachTrack (pMergeData, pMergeTrack) {
		/* トラックが足りない場合は追加する */
		if (pMIDITrack == NULL) {
			MIDITrack* pNewTrack = MIDITrack_Create ();
			MIDIData_AddTrack (pMIDIData, pNewTrack);
			pMIDITrack = pNewTrack;
		}
	}

	/* 重複イベントを削除するフラグが立っているならば */
	if (lFlags == 1) {
		pMIDITrack = pMIDIData->m_pFirstTrack;
		forEachTrack (pMergeData, pMergeTrack) {
			forEachEvent (pMergeTrack, pMergeEvent) {
				/* このMIDIデータに重複イベントがないかチェックし、あれば削除 */			
				pMIDIEvent = pMIDITrack->m_pFirstEvent;
				while (pMIDIEvent) {
					if (MIDIEvent_GetTime (pMergeEvent) + lTime == 
						MIDIEvent_GetTime (pMIDIEvent)) {
						if (MIDIEvent_GetKind (pMergeEvent) ==
							MIDIEvent_GetKind (pMIDIEvent)) {
							if ((MIDIEvent_IsNoteOff (pMergeEvent) &&
								MIDIEvent_IsNoteOff (pMIDIEvent)) ||
								(MIDIEvent_IsNoteOn (pMergeEvent) &&
								MIDIEvent_IsNoteOn (pMIDIEvent)) ||
								(MIDIEvent_IsKeyAftertouch (pMergeEvent) &&
								MIDIEvent_IsKeyAftertouch (pMIDIEvent))) {
								if (MIDIEvent_GetChannel (pMergeEvent) == 
									MIDIEvent_GetChannel (pMIDIEvent) &&
									MIDIEvent_GetKey (pMergeEvent) == 
									MIDIEvent_GetKey (pMIDIEvent)) {
									ret = MIDIEvent_Delete (pMIDIEvent);
									*pDeletedEventCount += ret;
									pMIDIEvent = pMIDITrack->m_pFirstEvent;
									continue;
								}
							}
							else if (MIDIEvent_IsControlChange (pMergeEvent) &&
								MIDIEvent_IsControlChange (pMIDIEvent)) {
								if (MIDIEvent_GetChannel (pMergeEvent) == 
									MIDIEvent_GetChannel (pMIDIEvent) &&
									MIDIEvent_GetNumber (pMergeEvent) == 
									MIDIEvent_GetNumber (pMIDIEvent)) {
									ret = MIDIEvent_Delete (pMIDIEvent);
									*pDeletedEventCount += ret;
									pMIDIEvent = pMIDITrack->m_pFirstEvent;
									continue;
								}
							}
							else if ((MIDIEvent_IsProgramChange (pMergeEvent) &&
								MIDIEvent_IsProgramChange (pMIDIEvent)) ||
								(MIDIEvent_IsChannelAftertouch (pMergeEvent) &&
								MIDIEvent_IsChannelAftertouch (pMIDIEvent)) ||
								(MIDIEvent_IsPitchBend (pMergeEvent) &&
								MIDIEvent_IsPitchBend (pMIDIEvent))) {
								if (MIDIEvent_GetChannel (pMergeEvent) ==
									MIDIEvent_GetChannel (pMIDIEvent)) {
									ret = MIDIEvent_Delete (pMIDIEvent);
									*pDeletedEventCount += ret;
									pMIDIEvent = pMIDITrack->m_pFirstEvent;
									continue;
								}
							}
							else if ((MIDIEvent_IsSysExEvent (pMergeEvent) &&
								MIDIEvent_IsSysExEvent (pMIDIEvent)) ||
								(MIDIEvent_IsSequencerSpecific (pMergeEvent) &&
								MIDIEvent_IsSequencerSpecific (pMIDIEvent))) {
								if (MIDIEvent_GetLen (pMergeEvent) ==
									MIDIEvent_GetLen (pMIDIEvent) &&
									pMergeEvent->m_pData != NULL && /* 20091024条件式追加 */
									pMIDIEvent->m_pData != NULL) { /* 20091024条件式追加 */
									if (memcmp (pMergeEvent->m_pData,
										pMIDIEvent->m_pData, pMergeEvent->m_lLen) == 0) {
										ret = MIDIEvent_Delete (pMIDIEvent);
										*pDeletedEventCount += ret;
										pMIDIEvent = pMIDITrack->m_pFirstEvent;
										continue;
									}
								}									
							}
							else {
								ret = MIDIEvent_Delete (pMIDIEvent);
								*pDeletedEventCount += ret;
								pMIDIEvent = pMIDITrack->m_pFirstEvent;
								continue;
							}
						}
					}
					pMIDIEvent = pMIDIEvent->m_pNextEvent;
				}
			}
			pMIDITrack = pMIDITrack->m_pNextTrack;
		}
	}

	/* マージ用MIDIデータからイベントを除去し、このMIDIデータに挿入 */
	pMIDITrack = pMIDIData->m_pFirstTrack;
	forEachTrack (pMergeData, pMergeTrack) {
		pMergeEvent = pMergeTrack->m_pFirstEvent;
		while (pMergeEvent) {
			ret = MIDITrack_RemoveEvent (pMergeTrack, pMergeEvent);
			ret = MIDIEvent_SetTime (pMergeEvent, pMergeEvent->m_lTime + lTime);
			ret = MIDITrack_InsertEvent (pMIDITrack, pMergeEvent);
			*pInsertedEventCount += ret;
			pMergeEvent = pMergeTrack->m_pFirstEvent;
		}
		pMIDITrack = pMIDITrack->m_pNextTrack;
	}
	return 1;
}


/* 保存・読み込み用関数 */

/* ビッグエンディアンをリトルエンディアンに変換する(4バイト) */
static unsigned long BigToLittle4 (unsigned char* pData) {
	return (*pData << 24) | (*(pData + 1) << 16) |
		(*(pData + 2) << 8) | *(pData + 3);
}

/* ビッグエンディアンをリトルエンディアンに変換する(2バイト) */
static unsigned short BigToLittle2 (unsigned char* pData) {
	return (unsigned short)((*pData << 8) | *(pData + 1));
}

/* リトルエンディアンをビッグエンディアンに変換する(4バイト) */
static void LittleToBig4 (long l, unsigned char* pData) {
	*(pData + 0) = (unsigned char)((l & 0xFF000000) >> 24);
	*(pData + 1) = (unsigned char)((l & 0x00FF0000) >> 16);
	*(pData + 2) = (unsigned char)((l & 0x0000FF00) >> 8);
	*(pData + 3) = (unsigned char)(l & 0x000000FF);
}

/* リトルエンディアンをビッグエンディアンに変換する(2バイト) */
static void LittleToBig2 (short s, unsigned char* pData) {
	*(pData + 0) = (unsigned char)((s & 0x0000FF00) >> 8);
	*(pData + 1) = (unsigned char)(s & 0x000000FF);
}


/* 可変長整数値を4バイトlong型整数値に変換し、読み込んだバイト数を返す。 */
static long VariableToLong (unsigned char* pData, long* pValue) {
	unsigned long ulValue = 0;
	long i = 0;
	while (1) {
		if (*(pData + i) & 0x80) { /* 7ビット目が立っている */
			ulValue = (ulValue << 7) | (*(pData + i) & 0x7F);
			i++;
		}
		else {
			ulValue = (ulValue << 7) | (*(pData + i) & 0x7F);
			break;
		}
	}
	*pValue = (long)ulValue;
	return i + 1;
}

/* 4バイトlong型整数値を可変長整数値に変換し、書き込んだバイト数を返す(最大5バイト)。 */
static long LongToVariable (long lValue, unsigned char* pData) {
	unsigned long ulValue = (unsigned long)lValue;
	if (0 <= ulValue && ulValue < 128) {
		*pData = (unsigned char)ulValue;
		return 1;
	}
	else if (128 <= ulValue && ulValue < 16384) {
		*pData = (unsigned char)(((ulValue & 0x3F80) >> 7) | 0x80);
		*(pData + 1) = (unsigned char)(ulValue & 0x007F);
		return 2;
	}
	else if (16384 <= ulValue && ulValue < 2097152) { 
		*pData = (unsigned char)(((ulValue & 0x1FC000) >> 14) | 0x80);
		*(pData + 1) = (unsigned char)(((ulValue & 0x003F80) >> 7) | 0x80);
		*(pData + 2) = (unsigned char)(ulValue & 0x00007F);
		return 3;
	}
	else if (2097152 <= ulValue && ulValue < 268435456) { 
		*pData = (unsigned char)(((ulValue & 0x0FE00000) >> 21) | 0x80);
		*(pData + 1) = (unsigned char)(((ulValue & 0x001FC000) >> 14) | 0x80);
		*(pData + 2) = (unsigned char)(((ulValue & 0x00003F80) >> 7) | 0x80);
		*(pData + 3) = (unsigned char)(ulValue & 0x0000007F);
		return 4;
	}
	else if (268435456 <= ulValue) { 
		*(pData + 0) = (unsigned char)(((ulValue & 0xF0000000) >> 28) | 0x80);
		*(pData + 1) = (unsigned char)(((ulValue & 0x0FE00000) >> 21) | 0x80);
		*(pData + 2) = (unsigned char)(((ulValue & 0x001FC000) >> 14) | 0x80);
		*(pData + 3) = (unsigned char)(((ulValue & 0x00003F80) >> 7) | 0x80);
		*(pData + 4) = (unsigned char)(ulValue & 0x0000007F);
		return 5;
	}
	*pData = 0;
	return 0;
}

/* トラックに単一のイベントを強制追加(内部隠蔽)(20090712追加) */
/* pEventを強制的にpMIDITrackの最後に挿入する。*/
/* 時刻チェック、正当性チェックは行わない。 */
long __stdcall MIDITrack_AddSingleEventForce (MIDITrack* pTrack, MIDIEvent* pInsertEvent) {
	assert (pTrack);
	assert (pInsertEvent);
	assert (pInsertEvent->m_pParent == NULL);
	/* 最後のイベントの直後に挿入する場合 */
	if (pTrack->m_pLastEvent) {
		MIDIEvent* pLastEvent = pTrack->m_pLastEvent;
		pInsertEvent->m_pParent = pTrack;
		/* 前後のイベントのポインタのつなぎかえ */
		pInsertEvent->m_pNextEvent = NULL;
		pInsertEvent->m_pPrevEvent = pLastEvent;
		pTrack->m_pLastEvent = pInsertEvent;
		pLastEvent->m_pNextEvent = pInsertEvent;
		/* 前後の同種イベントポインタ設定 */
		pInsertEvent->m_pPrevSameKindEvent = MIDIEvent_SearchPrevSameKindEvent (pInsertEvent);
		if (pInsertEvent->m_pPrevSameKindEvent) {
			pInsertEvent->m_pPrevSameKindEvent->m_pNextSameKindEvent = pInsertEvent;
		}
		pInsertEvent->m_pNextSameKindEvent = MIDIEvent_SearchNextSameKindEvent (pInsertEvent);
		if (pInsertEvent->m_pNextSameKindEvent) {
			pInsertEvent->m_pNextSameKindEvent->m_pPrevSameKindEvent = pInsertEvent;
		}
		pTrack->m_lNumEvent ++;
	}
	/* 空トラックに挿入する場合 */
	else {
		pInsertEvent->m_pParent = pTrack;
		pInsertEvent->m_pNextEvent = NULL;
		pInsertEvent->m_pPrevEvent = NULL;
		pInsertEvent->m_pNextSameKindEvent = NULL;
		pInsertEvent->m_pPrevSameKindEvent = NULL;
		pTrack->m_pFirstEvent = pInsertEvent;
		pTrack->m_pLastEvent = pInsertEvent;
		pTrack->m_lNumEvent ++;
	}
	return 1;
}

/* MIDITrackをSMFのメモリブロックから読み込み(非公開) */
MIDITrack* __stdcall MIDITrack_LoadFromSMF (unsigned char* pTrackData, long lTrackLen) {

	unsigned char* p = NULL;
	long lLen = 0;
	long lDeltaTime = 0;
	long lTickCount = 0;
	unsigned char byEventKind = 0;
	unsigned char byMetaKind = 0; 
	unsigned char byOldEventKind = 0;
	MIDITrack* pTrack = NULL;
	MIDIEvent* pEvent = NULL;

	p = pTrackData;

	pTrack = MIDITrack_Create ();
	if (pTrack == NULL) {
		return NULL;
	}

	while (p < pTrackData + lTrackLen) {
		p += VariableToLong (p, &lDeltaTime);
		lTickCount += lDeltaTime;
		byEventKind = *p;
		/* メタイベントの場合 */
		if (byEventKind == 0xFF) {
			p ++; /* イベントタイプ(0xFF)を読み飛ばす */
			byMetaKind = (*p++);
			p += VariableToLong (p, &lLen);
			pEvent = MIDIEvent_Create (lTickCount, byMetaKind, p, lLen);
			p += lLen;
		}
		/* システムエクスクルーシブイベントの場合 */
		else if (byEventKind == 0xF0 || byEventKind == 0xF7) {
			p ++; /* イベントタイプ(0xF0||0xF7)を読み飛ばす */
			p += VariableToLong (p, &lLen);
			pEvent = MIDIEvent_Create (lTickCount, byEventKind, p, lLen);
			p += lLen;
		}
		/* MIDIイベントの場合 */
		else if (0x00 <= byEventKind && byEventKind <= 0xEF) {
			if (0x00 <= byEventKind && byEventKind <= 0x7F) { /* ランニングステータス */
				byEventKind = byOldEventKind;
				lLen = 2;
			}
			else {
				lLen = 3;
			}
			if (0xC0 <= byEventKind && byEventKind <= 0xDF) {
				lLen --;
			}
			pEvent = MIDIEvent_Create (lTickCount, byEventKind, p, lLen);
			p += lLen;
			byOldEventKind = byEventKind;
		}
		if (pEvent == NULL) {
			;
		}
		MIDITrack_AddSingleEventForce (pTrack, pEvent); // 20090712追加
	}

	/* 結合できるイベントは結合する(廃止) */
	/*forEachEvent (pTrack, pEvent) {
		MIDIEvent_Combine (pEvent);
	}*/

	return pTrack;
}

/* MIDIDataをスタンダードMIDIファイル(*.mid)から読み込み、 */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL)(ANSI) */
/* 戻り値：正常終了=1、異常終了=0。 */
MIDIData* __stdcall MIDIData_LoadFromSMFA (const char* pszFileName) {
	MIDIData* pMIDIData = NULL;
	MIDITrack* pTrack = NULL;
	FILE* pFile = NULL;
	long lTrackLen = 0;
	unsigned long lFormat = 0;
	unsigned long lNumTrack = 0;
	unsigned long lTimeBase = 120;
	unsigned long lTimeMode = 0;
	unsigned long lTimeResolution = 0;
	unsigned long i = 0;
	unsigned char* pTrackData = NULL;
	unsigned char byBuf[256];

	pFile = fopen (pszFileName, "rb");
	if (pFile == NULL) {
		return NULL;
	}
	if (fread (byBuf, sizeof(char), 14, pFile) < 14) {
		fclose (pFile);
		return NULL;
	}
	if (memcmp (byBuf, "MThd", 4) != 0) {
		fclose (pFile);
		return NULL; 
	}
	lFormat = (long)BigToLittle2 (byBuf + 8);
	lNumTrack = (long)BigToLittle2 (byBuf + 10);
	lTimeBase = (long)BigToLittle2 (byBuf + 12);
	if (lFormat < 0 || lFormat > 2) {
		fclose (pFile);
		return NULL;
	}
	if (lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase < 0 || lTimeBase > 65535) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase & 0x00008000) {
		lTimeMode = 256 - ((lTimeBase & 0x0000FF00) >> 8);
		if (lTimeMode != MIDIDATA_SMPTE24BASE &&
			lTimeMode != MIDIDATA_SMPTE25BASE &&
			lTimeMode != MIDIDATA_SMPTE29BASE &&
			lTimeMode != MIDIDATA_SMPTE30BASE) {
			fclose (pFile);
			return NULL;
		}
		lTimeResolution = lTimeBase & 0x00FF;
	}
	else {
		lTimeMode = MIDIDATA_TPQNBASE;
		lTimeResolution = lTimeBase & 0x7FFF;
	}

	pMIDIData = MIDIData_Create (lFormat, 0, lTimeMode, lTimeResolution);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}

	while (!feof (pFile)) {

		if (fread (byBuf, sizeof(char), 8, pFile) < 8) {
			break;
		}
		if ((memcmp (byBuf, "MTrk", 4) != 0 && i < lNumTrack) &&
			!(memcmp (byBuf, "XFIH", 4) == 0 && lFormat == 0 && i == 1) &&
			!(memcmp (byBuf, "XFKM", 4) == 0 && lFormat == 0 && i == 2)) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile); /* This is not MIDITrack. */
			return NULL;
		}
		else if ((memcmp (byBuf, "MTrk", 4) != 0 && i >= lNumTrack) &&
			!(memcmp (byBuf, "XFIH", 4) == 0 && lFormat == 0 && i == 1) &&
			!(memcmp (byBuf, "XFKM", 4) == 0 && lFormat == 0 && i == 2)) {
			break;
		}
		lTrackLen = (long)BigToLittle4 (byBuf + 4);
		pTrackData = malloc (lTrackLen);
		if (pTrackData == NULL) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile); /* Out of Memory for TrackData. */
			return NULL;
		}
		if (fread (pTrackData, sizeof (char), 
			lTrackLen, pFile) < (unsigned long)lTrackLen) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile); /* There is too few TrackData. */
			free (pTrackData);
			return NULL;
		}

		pTrack = MIDITrack_LoadFromSMF (pTrackData, lTrackLen);
		if (pTrack == NULL) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		
		MIDIData_AddTrackForce (pMIDIData, pTrack);
	
		free (pTrackData);
		pTrackData = NULL;
		i++;
	}

	pMIDIData->m_lNumTrack = i;
	assert (pMIDIData->m_lTimeBase == lTimeBase);

	/* 各トラックの出力ポート番号・出力チャンネル・表示モードの自動設定 */
	MIDIData_UpdateOutputPort (pMIDIData);
	MIDIData_UpdateOutputChannel (pMIDIData);
	MIDIData_UpdateViewMode (pMIDIData);

	/* 各トラックの入力ポート番号・入力チャンネルの設定 */
	forEachTrack (pMIDIData, pTrack) {
		pTrack->m_lInputOn = 1;
		pTrack->m_lInputPort = pTrack->m_lOutputPort;
		pTrack->m_lInputChannel = pTrack->m_lOutputChannel;
		pTrack->m_lOutputOn = 1;
	}
	
	fclose (pFile);
	pFile = NULL;

	return pMIDIData;

}


/* MIDIDataをスタンダードMIDIファイル(*.mid)から読み込み、 */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL)(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
MIDIData* __stdcall MIDIData_LoadFromSMFW (const wchar_t* pszFileName) {
	MIDIData* pMIDIData = NULL;
	MIDITrack* pTrack = NULL;
	FILE* pFile = NULL;
	long lTrackLen = 0;
	unsigned long lFormat = 0;
	unsigned long lNumTrack = 0;
	unsigned long lTimeBase = 0;
	unsigned long lTimeMode = 0;
	unsigned long lTimeResolution = 0;
	unsigned long i = 0;
	unsigned char* pTrackData = NULL;
	unsigned char byBuf[256];

	pFile = _wfopen (pszFileName, L"rb");
	if (pFile == NULL) {
		return NULL;
	}
	if (fread (byBuf, sizeof(char), 14, pFile) < 14) {
		fclose (pFile);
		return NULL;
	}
	if (memcmp (byBuf, "MThd", 4) != 0) {
		fclose (pFile);
		return NULL; 
	}
	lFormat = (long)BigToLittle2 (byBuf + 8);
	lNumTrack = (long)BigToLittle2 (byBuf + 10);
	lTimeBase = (long)BigToLittle2 (byBuf + 12);
	if (lFormat < 0 || lFormat > 2) {
		fclose (pFile);
		return NULL;
	}
	if (lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase < 0 || lTimeBase > 65535) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase & 0x00008000) {
		lTimeMode = 256 - ((lTimeBase & 0x0000FF00) >> 8);
		if (lTimeMode != MIDIDATA_SMPTE24BASE &&
			lTimeMode != MIDIDATA_SMPTE25BASE &&
			lTimeMode != MIDIDATA_SMPTE29BASE &&
			lTimeMode != MIDIDATA_SMPTE30BASE) {
			fclose (pFile);
			return NULL;
		}
		lTimeResolution = lTimeBase & 0x00FF;
	}
	else {
		lTimeMode = MIDIDATA_TPQNBASE;
		lTimeResolution = lTimeBase & 0x7FFF;
	}

	pMIDIData = MIDIData_Create (lFormat, 0, lTimeMode, lTimeResolution);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}

	while (!feof (pFile)) {

		if (fread (byBuf, sizeof(char), 8, pFile) < 8) {
			break;
		}
		if ((memcmp (byBuf, "MTrk", 4) != 0 && i < lNumTrack) &&
			!(memcmp (byBuf, "XFIH", 4) == 0 && lFormat == 0 && i == 1) &&
			!(memcmp (byBuf, "XFKM", 4) == 0 && lFormat == 0 && i == 2)) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile); /* This is not MIDITrack. */
			return NULL;
		}
		else if ((memcmp (byBuf, "MTrk", 4) != 0 && i >= lNumTrack) &&
			!(memcmp (byBuf, "XFIH", 4) == 0 && lFormat == 0 && i == 1) &&
			!(memcmp (byBuf, "XFKM", 4) == 0 && lFormat == 0 && i == 2)) {
			break;
		}
		lTrackLen = (long)BigToLittle4 (byBuf + 4);
		pTrackData = malloc (lTrackLen);
		if (pTrackData == NULL) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile); /* Out of Memory for TrackData. */
			return NULL;
		}
		if (fread (pTrackData, sizeof (char), 
			lTrackLen, pFile) < (unsigned long)lTrackLen) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile); /* There is too few TrackData. */
			free (pTrackData);
			return NULL;
		}

		pTrack = MIDITrack_LoadFromSMF (pTrackData, lTrackLen);
		if (pTrack == NULL) {
			MIDIData_Delete (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		
		MIDIData_AddTrackForce (pMIDIData, pTrack);
	
		free (pTrackData);
		pTrackData = NULL;
		i++;
	}

	pMIDIData->m_lNumTrack = i;
	assert (pMIDIData->m_lTimeBase == lTimeBase);

	/* 各トラックの出力ポート番号・出力チャンネル・表示モードの自動設定 */
	MIDIData_UpdateOutputPort (pMIDIData);
	MIDIData_UpdateOutputChannel (pMIDIData);
	MIDIData_UpdateViewMode (pMIDIData);

	/* 各トラックの入力ポート番号・入力チャンネルの設定 */
	forEachTrack (pMIDIData, pTrack) {
		pTrack->m_lInputOn = 1;
		pTrack->m_lInputPort = pTrack->m_lOutputPort;
		pTrack->m_lInputChannel = pTrack->m_lOutputChannel;
		pTrack->m_lOutputOn = 1;
	}
	
	fclose (pFile);
	pFile = NULL;

	return pMIDIData;

}


/* long型整数値を可変長整数値に変換する。*/
/* 書き込んだバイト数を返す(最大4バイト)。 */
static long LongToExpand (long lValue, unsigned char* pData) {
	if (0 <= lValue && lValue < 128) {
		*pData = (unsigned char)lValue;
		return 1;
	}
	else if (128 <= lValue && lValue < 16384) {
		*pData = (unsigned char)(((lValue & 0x3F80) >> 7) | 0x80);
		*(pData + 1) = (unsigned char)(lValue & 0x007F);
		return 2;
	}
	else if (16384 <= lValue && lValue < 16384 * 128) { 
		*pData = (unsigned char)(((lValue & 0x1FC000) >> 14) | 0x80);
		*(pData + 1) = (unsigned char)(((lValue & 0x003F80) >> 7) | 0x80);
		*(pData + 2) = (unsigned char)(lValue & 0x00007F);
		return 3;
	}
	else if (16384 * 128 <= lValue && lValue < 16384 * 16384) { 
		*pData = (unsigned char)(((lValue & 0x0FE00000) >> 21) | 0x80);
		*(pData + 1) = (unsigned char)(((lValue & 0x001FC000) >> 14) | 0x80);
		*(pData + 2) = (unsigned char)(((lValue & 0x00003F80) >> 7) | 0x80);
		*(pData + 3) = (unsigned char)(lValue & 0x0000007F);
		return 4;
	}
	*pData = 0;
	return 0;
}

/* MIDITrackをSMFとしてメモリブロック上に保存したときの長さを推定(非公開) */
long __stdcall MIDITrack_GuessTrackDataLenAsSMF (MIDITrack* pMIDITrack) {
	long lLen = 0;
	MIDIEvent* pEvent;
	forEachEvent (pMIDITrack, pEvent) {
		lLen += 4;
		if (MIDIEvent_IsMIDIEvent (pEvent) == 0) {
			lLen += 5;	
		}
		lLen += pEvent->m_lLen;
	}
	return lLen;
}

/* MIDITrackをSMFとしてメモリブロック上に保存(非公開) */
long __stdcall MIDITrack_SaveAsSMF (MIDITrack* pMIDITrack, unsigned char* pBuf) {
	long lLen, lDeltaTime;
	unsigned char* p = pBuf;
	MIDIEvent* pEvent;
	unsigned char cOldEventType = 0xFF;
	forEachEvent (pMIDITrack, pEvent) {
		/* デルタタイム保存 */
		if (pEvent->m_pPrevEvent) {
			lDeltaTime = pEvent->m_lTime - pEvent->m_pPrevEvent->m_lTime;
		}
		/* 20081031:先頭空白は削除しない */
		else {
			lDeltaTime = pEvent->m_lTime;
		}
		lLen = LongToExpand (lDeltaTime, p);
		p += lLen;
		/* メタイベント */
		if (0x00 <= pEvent->m_lKind && pEvent->m_lKind < 0x80) {
			*p++ = 0xFF;
			*p++ = (unsigned char)(pEvent->m_lKind);
			p += LongToExpand (pEvent->m_lLen, p);
			if (pEvent->m_pData && pEvent->m_lLen > 0) { /* 20091024条件式追加 */
				memcpy (p, pEvent->m_pData, pEvent->m_lLen);
			}
			p += pEvent->m_lLen;
			cOldEventType = 0xFF;
		}
		/* MIDIチャンネルイベント */
		else if (0x80 <= pEvent->m_lKind && pEvent->m_lKind <= 0xEF) {
			/* ランニングステータス使用可能(2バイト目以降保存) */
			if (cOldEventType == *(pEvent->m_pData)) {
				memcpy (p, pEvent->m_pData + 1, pEvent->m_lLen - 1);
				p += pEvent->m_lLen - 1;
			}
			/* ランニングステータス使用不能(全バイト保存) */
			else {
				memcpy (p, pEvent->m_pData, pEvent->m_lLen);
				p += pEvent->m_lLen;
			}
			cOldEventType = *(pEvent->m_pData);
		}
		/* Sysxイベント */
		else if (pEvent->m_lKind == 0xF0) {
			*p++ = (unsigned char)0xF0;
			p += LongToExpand (pEvent->m_lLen - 1, p);
			if (pEvent->m_pData != NULL && pEvent->m_lLen > 1) { /* 20091024条件式追加 */
				memcpy (p, pEvent->m_pData + 1, pEvent->m_lLen - 1);
			}
			p += pEvent->m_lLen - 1;
			cOldEventType = 0xF0;
		}
		/* Sysxイベント(続き) */
		else if (pEvent->m_lKind == 0xF7) {
			/* 未デバッグ */
			*p++ = (unsigned char)0xF7;
			p += LongToExpand (pEvent->m_lLen, p);
			if (pEvent->m_pData != NULL && pEvent->m_lLen > 0) { /* 20091024条件式追加 */
				memcpy (p, pEvent->m_pData, pEvent->m_lLen);
			}
			p += pEvent->m_lLen;
			cOldEventType = 0xF7;
		}
	}
	return (p - pBuf);
}

/* MIDIデータをスタンダードMIDIファイル(*.mid)として保存(ANSI) */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsSMFA (MIDIData* pMIDIData, const char* pszFileName) {
	unsigned char* pBuf;
	unsigned char byBuf[256];
	long i = 0;
	long lLen, lWriteLen;
	short sNumTrack;
	MIDITrack* pMIDITrack;
	FILE* pFile;
	long lXFVersion = MIDIData_GetXFVersion (pMIDIData);

	pFile = fopen (pszFileName, "wb");
	if (pFile == NULL) {
		return 0;
	}

	/* MThdヘッダーの保存 */
	memset (byBuf, 0, 256);
	strncpy ((char*)byBuf, "MThd", 4);
	pBuf = byBuf;
	LittleToBig4 (6, pBuf + 4);
	LittleToBig2 ((short)(pMIDIData->m_lFormat), pBuf + 8);
	sNumTrack = pMIDIData->m_lFormat == 0 ? 1 : (short)(pMIDIData->m_lNumTrack);
	LittleToBig2 (sNumTrack, pBuf + 10);
	LittleToBig2 ((short)(pMIDIData->m_lTimeBase), pBuf + 12);
	lWriteLen = fwrite (byBuf, 1, 14, pFile);
	if (lWriteLen < 14) {
		fclose (pFile);
		return 0;
	}

	/* 各トラックの保存 */
	forEachTrack (pMIDIData, pMIDITrack) {
		lLen = MIDITrack_GuessTrackDataLenAsSMF (pMIDITrack);
		pBuf = malloc (8 + lLen);
		if (pBuf == NULL) {
			fclose (pFile);
			return 0;
		}
		lLen = MIDITrack_SaveAsSMF (pMIDITrack, pBuf + 8);
		if (lLen <= 0) {
			free (pBuf);
			fclose (pFile);
			return 0;
		}
		LittleToBig4 (lLen ,pBuf + 4);

		if (pMIDIData->m_lFormat == 0 && lXFVersion != 0 && i == 1) {
			strncpy ((char*)pBuf, "XFIH", 4); 
		}
		else if (pMIDIData->m_lFormat == 0 && lXFVersion != 0 && i == 2) {
			strncpy ((char*)pBuf, "XFKM", 4); 
		}
		else {
			strncpy ((char*)pBuf, "MTrk", 4); 
		}

		lWriteLen = fwrite (pBuf, 1, 8 + lLen, pFile);
		if (lWriteLen < 8 + lLen) {
			free (pBuf);
			fclose (pFile);
			return 0;
		}
		free (pBuf);
		i++;
	}

	fclose (pFile);
	return 1;
}

/* MIDIデータをスタンダードMIDIファイル(*.mid)として保存(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsSMFW (MIDIData* pMIDIData, const wchar_t* pszFileName) {
	unsigned char* pBuf;
	unsigned char byBuf[256];
	long i = 0;
	long lLen, lWriteLen;
	short sNumTrack;
	MIDITrack* pMIDITrack;
	FILE* pFile;
	long lXFVersion = MIDIData_GetXFVersion (pMIDIData);

	pFile = _wfopen (pszFileName, L"wb");
	if (pFile == NULL) {
		return 0;
	}

	/* MThdヘッダーの保存 */
	memset (byBuf, 0, 256);
	strncpy ((char*)byBuf, "MThd", 4);
	pBuf = byBuf;
	LittleToBig4 (6, pBuf + 4);
	LittleToBig2 ((short)(pMIDIData->m_lFormat), pBuf + 8);
	sNumTrack = pMIDIData->m_lFormat == 0 ? 1 : (short)(pMIDIData->m_lNumTrack);
	LittleToBig2 (sNumTrack, pBuf + 10);
	LittleToBig2 ((short)(pMIDIData->m_lTimeBase), pBuf + 12);
	lWriteLen = fwrite (byBuf, 1, 14, pFile);
	if (lWriteLen < 14) {
		fclose (pFile);
		return 0;
	}

	/* 各トラックの保存 */
	forEachTrack (pMIDIData, pMIDITrack) {
		lLen = MIDITrack_GuessTrackDataLenAsSMF (pMIDITrack);
		pBuf = malloc (8 + lLen);
		if (pBuf == NULL) {
			fclose (pFile);
			return 0;
		}
		lLen = MIDITrack_SaveAsSMF (pMIDITrack, pBuf + 8);
		if (lLen <= 0) {
			free (pBuf);
			fclose (pFile);
			return 0;
		}
		LittleToBig4 (lLen ,pBuf + 4);

		if (pMIDIData->m_lFormat == 0 && lXFVersion != 0 && i == 1) {
			strncpy ((char*)pBuf, "XFIH", 4); 
		}
		else if (pMIDIData->m_lFormat == 0 && lXFVersion != 0 && i == 2) {
			strncpy ((char*)pBuf, "XFKM", 4); 
		}
		else {
			strncpy ((char*)pBuf, "MTrk", 4); 
		}

		lWriteLen = fwrite (pBuf, 1, 8 + lLen, pFile);
		if (lWriteLen < 8 + lLen) {
			free (pBuf);
			fclose (pFile);
			return 0;
		}
		free (pBuf);
		i++;
	}

	fclose (pFile);
	return 1;
}



/* MIDIDataをテキストファイル(*.txt)から読み込み、 */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL)(ANSI) */
MIDIData* __stdcall MIDIData_LoadFromTextA (const char* pszFileName) {
	MIDIData* pMIDIData = NULL;
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	long lFormat;
	long lNumTrack;
	long lTimeBase;
	long lTimeMode;
	long lTimeResolution;
	long lNumEvent;
	long lIndex;
	long lTime;
	long lKind;
	long lLen;
	unsigned char ucData[1024];
	long lPrevCombinedEvent;
	long lNextCombinedEvent;
	long lUser1;
	long lUser2;
	long lUser3;
	long lUserFlag;
	char szType[256];
	char szTextLine[2048];
	long i = 0;
	long j = 0;
	long k = 0;

	FILE* pFile = fopen (pszFileName, "rt");
	if (pFile == NULL) {
		return 0;
	}

	memset (szTextLine, 0, sizeof (szTextLine));
	fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
	if (strncmp (szTextLine, "MDat", 4) == 0) {
		fclose (pFile);
		return NULL;
	}
	sscanf (szTextLine, "%s %lX %lX %lX %lX %lX %lX %lX\n",
		szType, &lFormat, &lNumTrack, &lTimeBase, 
		&lUser1, &lUser2, &lUser3, &lUserFlag);
	if (lFormat < 0 || lFormat >= 2) {
		fclose (pFile);
 		return NULL;
	}
	if (lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase < 0 || lTimeBase > 65535) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase & 0x00008000) {
		lTimeMode = 256 - ((lTimeBase & 0x0000FF00) >> 8);
		lTimeResolution = lTimeBase & 0x000000FF;
	}
	else {
		lTimeMode =  MIDIDATA_TPQNBASE;
		lTimeResolution = lTimeBase & 0x00007FFF;
	}
	if (lTimeMode != MIDIDATA_TPQNBASE &&
		lTimeMode != MIDIDATA_SMPTE24BASE &&
		lTimeMode != MIDIDATA_SMPTE25BASE &&
		lTimeMode != MIDIDATA_SMPTE29BASE &&
		lTimeMode != MIDIDATA_SMPTE30BASE) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeResolution <= 0) {
		fclose (pFile);
		return NULL;
	}

	pMIDIData = MIDIData_Create (lFormat, lNumTrack, lTimeMode, lTimeResolution);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}
	pMIDIData->m_lUser1 = lUser1;
	pMIDIData->m_lUser2 = lUser2;
	pMIDIData->m_lUser3 = lUser3;
	pMIDIData->m_lUserFlag = lUserFlag;

	forEachTrack (pMIDIData, pMIDITrack) {
		memset (szTextLine, 0, sizeof (szTextLine));
		fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
		if (strncmp (szTextLine, "MTrk", 9) != 0) {
			fclose (pFile);
			MIDIData_Delete (pMIDIData);
			return NULL;
		}
		sscanf (szTextLine, "%s %lX %lX %lX %lX %lX %lX\n",
			szType, &lIndex, &lNumEvent,
			&lUser1, &lUser2, &lUser3, &lUserFlag);
		pMIDITrack->m_lTempIndex = lIndex;
		pMIDITrack->m_lUser1 = lUser1;
		pMIDITrack->m_lUser2 = lUser2;
		pMIDITrack->m_lUser3 = lUser3;
		pMIDITrack->m_lUserFlag = lUserFlag;
		/* 各MIDIイベントの読み込み */
		for (j = 0; j < lNumEvent; j++) {
			char* p = szTextLine;
			memset (szTextLine, 0, sizeof (szTextLine));
			fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
			if (strncmp (szTextLine, "MEvt", 4) != 0) {
				fclose (pFile);
				MIDIData_Delete (pMIDIData);
				return NULL;
			}
			sscanf (szTextLine, "%s %lX %lX %lX %lX",
				szType, &lIndex, &lTime, &lKind, &lLen);
			memset (ucData, 0, sizeof (ucData));
			for (k = 0; k < 5; k++) {
				p = strchr (p, ' ');
				if (p == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
				while (*p == ' ') {
					p++;
				}
			}
			for (k = 0; k < lLen; k++) {
				sscanf (p, "%X", &ucData[k]);
				p = strchr (p, ' ');
				if (p == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
				while (*p == ' ') {
					p++;
				}
			}

			pMIDIEvent = MIDIEvent_Create (lTime, lKind, ucData, lLen);
			if (pMIDIEvent == NULL) {
				fclose (pFile);
				MIDIData_Delete (pMIDIData);
				return NULL;
			}
			if (MIDITrack_InsertEvent (pMIDITrack, pMIDIEvent) == 0) {
				fclose (pFile);
				MIDIData_Delete (pMIDIData);
				return NULL;
			}
			sscanf (p, "%lX %lX %lX %lX %lX %lX\n", 
				&lPrevCombinedEvent, &lNextCombinedEvent,
				&lUser1, &lUser2, &lUser3, &lUserFlag);
			pMIDIEvent->m_lTempIndex = lIndex;
			pMIDIEvent->m_pPrevCombinedEvent = (MIDIEvent*)lPrevCombinedEvent;
			pMIDIEvent->m_pNextCombinedEvent = (MIDIEvent*)lNextCombinedEvent;
			pMIDIEvent->m_lUser1 = lUser1;
			pMIDIEvent->m_lUser2 = lUser2;
			pMIDIEvent->m_lUser3 = lUser3;
			pMIDIEvent->m_lUserFlag = lUserFlag;
		}
		/* 結合されたイベントの結合状態復元処理 */
		forEachEvent (pMIDITrack, pMIDIEvent) {
			if ((long)(pMIDIEvent->m_pPrevCombinedEvent) == -1) {
				pMIDIEvent->m_pPrevCombinedEvent = NULL;
			}
			else {
				MIDIEvent* pTempEvent;
				forEachEvent (pMIDITrack, pTempEvent) {
					if (pTempEvent->m_lTempIndex ==
						(long)(pMIDIEvent->m_pPrevCombinedEvent)) {
						pMIDIEvent->m_pPrevCombinedEvent = pTempEvent;
						break;
					}
				}
				if (pTempEvent == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
			}
			if ((long)(pMIDIEvent->m_pNextCombinedEvent) == -1) {
				pMIDIEvent->m_pNextCombinedEvent = NULL;
			}
			else {
				MIDIEvent* pTempEvent;
				forEachEvent (pMIDITrack, pTempEvent) {
					if (pTempEvent->m_lTempIndex ==
						(long)(pMIDIEvent->m_pNextCombinedEvent)) {
						pMIDIEvent->m_pNextCombinedEvent = pTempEvent;
						break;
					}
				}
				if (pTempEvent == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
			}
		}
		i++;
	}
	return pMIDIData;
}

/* MIDIDataをテキストファイルから読み込み、 */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL)(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
MIDIData* __stdcall MIDIData_LoadFromTextW (const wchar_t* pszFileName) {
	MIDIData* pMIDIData = NULL;
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	long lFormat;
	long lNumTrack;
	long lTimeBase;
	long lTimeMode;
	long lTimeResolution;
	long lNumEvent;
	long lIndex;
	long lTime;
	long lKind;
	long lLen;
	unsigned char ucData[1024];
	long lPrevCombinedEvent;
	long lNextCombinedEvent;
	long lUser1;
	long lUser2;
	long lUser3;
	long lUserFlag;
	char szType[256];
	char szTextLine[2048];
	long i = 0;
	long j = 0;
	long k = 0;

	FILE* pFile = _wfopen (pszFileName, L"rt");
	if (pFile == NULL) {
		return 0;
	}

	memset (szTextLine, 0, sizeof (szTextLine));
	fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
	if (strncmp (szTextLine, "MDat", 4) == 0) {
		fclose (pFile);
		return NULL;
	}
	sscanf (szTextLine, "%s %lX %lX %lX %lX %lX %lX %lX\n",
		szType, &lFormat, &lNumTrack, &lTimeBase, 
		&lUser1, &lUser2, &lUser3, &lUserFlag);
	if (lFormat < 0 || lFormat >= 2) {
		fclose (pFile);
 		return NULL;
	}
	if (lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase < 0 || lTimeBase > 65535) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase & 0x00008000) {
		lTimeMode = 256 - ((lTimeBase & 0x0000FF00) >> 8);
		lTimeResolution = lTimeBase & 0x000000FF;
	}
	else {
		lTimeMode =  MIDIDATA_TPQNBASE;
		lTimeResolution = lTimeBase & 0x00007FFF;
	}
	if (lTimeMode != MIDIDATA_TPQNBASE &&
		lTimeMode != MIDIDATA_SMPTE24BASE &&
		lTimeMode != MIDIDATA_SMPTE25BASE &&
		lTimeMode != MIDIDATA_SMPTE29BASE &&
		lTimeMode != MIDIDATA_SMPTE30BASE) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeResolution <= 0) {
		fclose (pFile);
		return NULL;
	}

	pMIDIData = MIDIData_Create (lFormat, lNumTrack, lTimeMode, lTimeResolution);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}
	pMIDIData->m_lUser1 = lUser1;
	pMIDIData->m_lUser2 = lUser2;
	pMIDIData->m_lUser3 = lUser3;
	pMIDIData->m_lUserFlag = lUserFlag;

	forEachTrack (pMIDIData, pMIDITrack) {
		memset (szTextLine, 0, sizeof (szTextLine));
		fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
		if (strncmp (szTextLine, "MTrk", 9) != 0) {
			fclose (pFile);
			MIDIData_Delete (pMIDIData);
			return NULL;
		}
		sscanf (szTextLine, "%s %lX %lX %lX %lX %lX %lX\n",
			szType, &lIndex, &lNumEvent,
			&lUser1, &lUser2, &lUser3, &lUserFlag);
		pMIDITrack->m_lTempIndex = lIndex;
		pMIDITrack->m_lUser1 = lUser1;
		pMIDITrack->m_lUser2 = lUser2;
		pMIDITrack->m_lUser3 = lUser3;
		pMIDITrack->m_lUserFlag = lUserFlag;
		/* 各MIDIイベントの読み込み */
		for (j = 0; j < lNumEvent; j++) {
			char* p = szTextLine;
			memset (szTextLine, 0, sizeof (szTextLine));
			fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
			if (strncmp (szTextLine, "MEvt", 4) != 0) {
				fclose (pFile);
				MIDIData_Delete (pMIDIData);
				return NULL;
			}
			sscanf (szTextLine, "%s %lX %lX %lX %lX",
				szType, &lIndex, &lTime, &lKind, &lLen);
			memset (ucData, 0, sizeof (ucData));
			for (k = 0; k < 5; k++) {
				p = strchr (p, ' ');
				if (p == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
				while (*p == ' ') {
					p++;
				}
			}
			for (k = 0; k < lLen; k++) {
				sscanf (p, "%X", &ucData[k]);
				p = strchr (p, ' ');
				if (p == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
				while (*p == ' ') {
					p++;
				}
			}

			pMIDIEvent = MIDIEvent_Create (lTime, lKind, ucData, lLen);
			if (pMIDIEvent == NULL) {
				fclose (pFile);
				MIDIData_Delete (pMIDIData);
				return NULL;
			}
			if (MIDITrack_InsertEvent (pMIDITrack, pMIDIEvent) == 0) {
				fclose (pFile);
				MIDIData_Delete (pMIDIData);
				return NULL;
			}
			sscanf (p, "%lX %lX %lX %lX %lX %lX\n", 
				&lPrevCombinedEvent, &lNextCombinedEvent,
				&lUser1, &lUser2, &lUser3, &lUserFlag);
			pMIDIEvent->m_lTempIndex = lIndex;
			pMIDIEvent->m_pPrevCombinedEvent = (MIDIEvent*)lPrevCombinedEvent;
			pMIDIEvent->m_pNextCombinedEvent = (MIDIEvent*)lNextCombinedEvent;
			pMIDIEvent->m_lUser1 = lUser1;
			pMIDIEvent->m_lUser2 = lUser2;
			pMIDIEvent->m_lUser3 = lUser3;
			pMIDIEvent->m_lUserFlag = lUserFlag;
		}
		/* 結合されたイベントの結合状態復元処理 */
		forEachEvent (pMIDITrack, pMIDIEvent) {
			if ((long)(pMIDIEvent->m_pPrevCombinedEvent) == -1) {
				pMIDIEvent->m_pPrevCombinedEvent = NULL;
			}
			else {
				MIDIEvent* pTempEvent;
				forEachEvent (pMIDITrack, pTempEvent) {
					if (pTempEvent->m_lTempIndex ==
						(long)(pMIDIEvent->m_pPrevCombinedEvent)) {
						pMIDIEvent->m_pPrevCombinedEvent = pTempEvent;
						break;
					}
				}
				if (pTempEvent == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
			}
			if ((long)(pMIDIEvent->m_pNextCombinedEvent) == -1) {
				pMIDIEvent->m_pNextCombinedEvent = NULL;
			}
			else {
				MIDIEvent* pTempEvent;
				forEachEvent (pMIDITrack, pTempEvent) {
					if (pTempEvent->m_lTempIndex ==
						(long)(pMIDIEvent->m_pNextCombinedEvent)) {
						pMIDIEvent->m_pNextCombinedEvent = pTempEvent;
						break;
					}
				}
				if (pTempEvent == NULL) {
					fclose (pFile);
					MIDIData_Delete (pMIDIData);
					return NULL;
				}
			}
		}
		i++;
	}
	return pMIDIData;
}


/* MIDIDataをテキストファイル(*.txt)として保存(デバッグ用隠し関数)(ANSI) */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsTextA (MIDIData* pMIDIData, const char* pszFileName) {
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	long i = 0;
	long j = 0;
	long k = 0;
	FILE* pFile = fopen (pszFileName, "wt");
	if (pFile == NULL) {
		return 0;
	}
	MIDIData_CountTrack (pMIDIData),
	fprintf (pFile, "MDat %lX %lX %lX %lX %lX %lX %lX\n",
		pMIDIData->m_lFormat,
		pMIDIData->m_lNumTrack,
		pMIDIData->m_lTimeBase,
		pMIDIData->m_lUser1,
		pMIDIData->m_lUser2,
		pMIDIData->m_lUser3,
		pMIDIData->m_lUserFlag);
	forEachTrack (pMIDIData, pMIDITrack) {
		MIDITrack_CountEvent (pMIDITrack),
		fprintf (pFile, "MTrk %lX %lX %lX %lX %lX %lX\n",
			pMIDITrack->m_lTempIndex,
			pMIDITrack->m_lNumEvent,
			pMIDITrack->m_lUser1,
			pMIDITrack->m_lUser2,
			pMIDITrack->m_lUser3,
			pMIDITrack->m_lUserFlag);
		j = 0;
		forEachEvent (pMIDITrack, pMIDIEvent) {
			fprintf (pFile, "MEvt %lX %lX %lX %lX",
				pMIDIEvent->m_lTempIndex,
				pMIDIEvent->m_lTime,
				pMIDIEvent->m_lKind,
				pMIDIEvent->m_lLen);
			for (k = 0; k < pMIDIEvent->m_lLen; k++) {
				fprintf (pFile, " %X", *(pMIDIEvent->m_pData + k));
			}
			fprintf (pFile, " %lX %lX %lX %lX %lX %lX\n",
				pMIDIEvent->m_pPrevCombinedEvent ? pMIDIEvent->m_pPrevCombinedEvent->m_lTempIndex : -1,
				pMIDIEvent->m_pNextCombinedEvent ? pMIDIEvent->m_pNextCombinedEvent->m_lTempIndex : -1,
				pMIDIEvent->m_lUser1,
				pMIDIEvent->m_lUser2,
				pMIDIEvent->m_lUser3,
				pMIDIEvent->m_lUserFlag);
			j++;
		}
		i++;
	}
	fclose (pFile);
	return 1;
}

/* MIDIDataをテキストファイル(*.txt)として保存(デバッグ用隠し関数)(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsTextW (MIDIData* pMIDIData, const wchar_t* pszFileName) {
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	long i = 0;
	long j = 0;
	long k = 0;
	FILE* pFile = _wfopen (pszFileName, L"wt");
	if (pFile == NULL) {
		return 0;
	}
	MIDIData_CountTrack (pMIDIData),
	fprintf (pFile, "MDat %lX %lX %lX %lX %lX %lX %lX\n",
		pMIDIData->m_lFormat,
		pMIDIData->m_lNumTrack,
		pMIDIData->m_lTimeBase,
		pMIDIData->m_lUser1,
		pMIDIData->m_lUser2,
		pMIDIData->m_lUser3,
		pMIDIData->m_lUserFlag);
	forEachTrack (pMIDIData, pMIDITrack) {
		MIDITrack_CountEvent (pMIDITrack),
		fprintf (pFile, "MTrk %lX %lX %lX %lX %lX %lX\n",
			pMIDITrack->m_lTempIndex,
			pMIDITrack->m_lNumEvent,
			pMIDITrack->m_lUser1,
			pMIDITrack->m_lUser2,
			pMIDITrack->m_lUser3,
			pMIDITrack->m_lUserFlag);
		j = 0;
		forEachEvent (pMIDITrack, pMIDIEvent) {
			fprintf (pFile, "MEvt %lX %lX %lX %lX",
				pMIDIEvent->m_lTempIndex,
				pMIDIEvent->m_lTime,
				pMIDIEvent->m_lKind,
				pMIDIEvent->m_lLen);
			for (k = 0; k < pMIDIEvent->m_lLen; k++) {
				fprintf (pFile, " %X", *(pMIDIEvent->m_pData + k));
			}
			fprintf (pFile, " %lX %lX %lX %lX %lX %lX\n",
				pMIDIEvent->m_pPrevCombinedEvent ? pMIDIEvent->m_pPrevCombinedEvent->m_lTempIndex : -1,
				pMIDIEvent->m_pNextCombinedEvent ? pMIDIEvent->m_pNextCombinedEvent->m_lTempIndex : -1,
				pMIDIEvent->m_lUser1,
				pMIDIEvent->m_lUser2,
				pMIDIEvent->m_lUser3,
				pMIDIEvent->m_lUserFlag);
			j++;
		}
		i++;
	}
	fclose (pFile);
	return 1;
}

/* MIDIEventをバイナリファイルから読み込み(隠蔽) */
/* 新しいMIDIEventへのポインタを返す(失敗時NULL) */
MIDIEvent* __stdcall MIDIEvent_LoadFromBinary (FILE* pFile) {
	long lRet = 0;
	char szType[4] = {0, 0, 0, 0};
	long lIndex = 0;
	long lTime = 0;
	long lKind = 0;
	long lLen = 0;
	unsigned char ucData[16384];
	long lPrevCombinedEvent = 0;
	long lNextCombinedEvent = 0;
	long lReserved1 = 0;
	long lReserved2 = 0;
	long lReserved3 = 0;
	long lReserved4 = 0;
	long lUser1 = 0;
	long lUser2 = 0;
	long lUser3 = 0;
	long lUserFlag = 0;
	MIDIEvent* pMIDIEvent = NULL;
	/* プロパティ読み込み */
	lRet = fread (szType, 4, 1, pFile);
	if (lRet < 1 || memcmp (szType, "MEvt", 4) != 0) {
		return NULL;
	}
	lRet = fread (&lIndex, 4, 1, pFile);
	if (lRet < 1 || lIndex < 0) {
		return NULL;
	}
	lRet = fread (&lTime, 4, 1, pFile);
	if (lRet < 1 || lTime < 0) {
		return NULL;
	}
	lRet = fread (&lKind, 4, 1, pFile);
	if (lRet < 1 || lKind < 0) {
		return NULL;
	}
	lRet = fread (&lLen, 4, 1, pFile);
	if (lRet < 1 || lLen < 0 || lLen >= sizeof (ucData)) {
		return NULL;
	}
	lRet = fread (ucData, 1, lLen, pFile);
	if (lRet < lLen) {
		return NULL;
	}
	lRet = fread (&lPrevCombinedEvent, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lNextCombinedEvent, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lUser1, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lUser2, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lUser3, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lUserFlag, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	/* MIDIイベントの生成 */
	pMIDIEvent = MIDIEvent_Create (lTime, lKind, ucData, lLen);
	if (pMIDIEvent == NULL) {
		return NULL;
	}
	/* プロパティ設定 */
	pMIDIEvent->m_lTempIndex = lIndex;
	pMIDIEvent->m_pPrevCombinedEvent = (MIDIEvent*)lPrevCombinedEvent;
	pMIDIEvent->m_pNextCombinedEvent = (MIDIEvent*)lNextCombinedEvent;			
	pMIDIEvent->m_lUser1 = lUser1;
	pMIDIEvent->m_lUser2 = lUser2;
	pMIDIEvent->m_lUser3 = lUser3;
	pMIDIEvent->m_lUserFlag = lUserFlag;
	return pMIDIEvent;
}


/* MIDITrackをバイナリファイルから読み込み(隠蔽) */
/* 新しいMIDITrackへのポインタを返す(失敗時NULL) */
MIDITrack* __stdcall MIDITrack_LoadFromBinary (FILE* pFile) {
	long lRet = 0;
	char szType[4] = {0, 0, 0, 0};
	long lIndex = 0;
	long lNumEvent = 0;
	long lInputOn = 1;
	long lInputPort = 0;
	long lInputChannel = 0;
	long lOutputOn = 1;
	long lOutputPort = 0;
	long lOutputChannel = 0;
	long lTimePlus = 0;
	long lKeyPlus = 0;
	long lVelocityPlus = 0;
	long lViewMode = 0;
	long lForeColor = 0x00000000;
	long lBackColor = 0x00FFFFFF;
	long lReserved1 = 0;
	long lReserved2 = 0;
	long lReserved3 = 0;
	long lReserved4 = 0;
	long lUser1 = 0;
	long lUser2 = 0;
	long lUser3 = 0;
	long lUserFlag = 0;
	long j = 0;
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	
	/* プロパティ読み込み・設定 */
	lRet = fread (szType, 4, 1, pFile);
	if (lRet < 1 || (memcmp (szType, "MTrk", 4) != 0 && memcmp (szType, "MTr2", 4) != 0)) {
		return NULL;
	}
	lRet = fread (&lIndex, 4, 1, pFile);
	if (lRet < 1 || lIndex < 0) {
		return NULL;
	}
	lRet = fread (&lNumEvent, 4, 1, pFile);
	if (lRet < 1 || lNumEvent < 0) {
		return NULL;
	}
	/* MIDIDataLib1.4から追加した項目(20081008追加) */
	if (memcmp (szType, "MTr2", 4) == 0) {
		lRet = fread (&lInputOn, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lInputPort, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lInputChannel, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lOutputOn, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lOutputPort, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lOutputChannel, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lTimePlus, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lKeyPlus, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lVelocityPlus, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lViewMode, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lForeColor, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lBackColor, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lReserved1, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lReserved2, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lReserved3, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
		lRet = fread (&lReserved4, 4, 1, pFile);
		if (lRet < 1) {
			return NULL;
		}
	}
	lRet = fread (&lUser1, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lUser2, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lUser3, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	lRet = fread (&lUserFlag, 4, 1, pFile);
	if (lRet < 1) {
		return NULL;
	}
	/* MIDITrackの生成 */
	pMIDITrack = MIDITrack_Create ();
	if (pMIDITrack == NULL) {
		return NULL;
	}
	/* プロパティの設定 */
	pMIDITrack->m_lTempIndex = lIndex;
	pMIDITrack->m_lUser1 = lUser1;
	pMIDITrack->m_lUser2 = lUser2;
	pMIDITrack->m_lUser3 = lUser3;
	pMIDITrack->m_lUserFlag = lUserFlag;
	/* MIDIDataLib1.4から追加した項目(20081008追加) */
	if (memcmp (szType, "MTr2", 4) == 0) {
		pMIDITrack->m_lInputOn = lInputOn;
		pMIDITrack->m_lInputPort = lInputPort;
		pMIDITrack->m_lInputChannel = lInputChannel;
		pMIDITrack->m_lOutputOn = lOutputOn;
		pMIDITrack->m_lOutputPort = lOutputPort;
		pMIDITrack->m_lOutputChannel = lOutputChannel;
		pMIDITrack->m_lTimePlus = lTimePlus;
		pMIDITrack->m_lKeyPlus = lKeyPlus;
		pMIDITrack->m_lVelocityPlus = lVelocityPlus;
		pMIDITrack->m_lViewMode = lViewMode;
		pMIDITrack->m_lForeColor = lForeColor;
		pMIDITrack->m_lBackColor = lBackColor;
		pMIDITrack->m_lReserved1 = lReserved1;
		pMIDITrack->m_lReserved2 = lReserved2;
		pMIDITrack->m_lReserved3 = lReserved3;
		pMIDITrack->m_lReserved4 = lReserved4;
	}
	/* MIDIDataLib1.3以前(20081008追加) */
	else {
		pMIDITrack->m_lInputOn = (lUserFlag & 0x00000010) ? 1 : 0;
		pMIDITrack->m_lInputPort = lUser2 & 0x000000FF;
		pMIDITrack->m_lInputChannel = CLIP (-1, (long)(char)((lUser2 & 0x0000FF00) >> 8), 15);
		pMIDITrack->m_lOutputOn = (lUserFlag & 0x00000020) ? 1 : 0;
		pMIDITrack->m_lOutputPort = (lUser2 & 0x00FF0000) >> 16;
		pMIDITrack->m_lOutputChannel = CLIP (-1, (long)(char)((lUser2 & 0xFF000000) >> 24), 15);
		pMIDITrack->m_lTimePlus = 0;
		pMIDITrack->m_lKeyPlus = 0;
		pMIDITrack->m_lVelocityPlus = 0;
		pMIDITrack->m_lViewMode = (lUserFlag & 0x00000008) ? 1 : 0;
		pMIDITrack->m_lForeColor = lUser1;
		pMIDITrack->m_lBackColor = 0x00FFFFFF;
		pMIDITrack->m_lReserved1 = 0;
		pMIDITrack->m_lReserved2 = 0;
		pMIDITrack->m_lReserved3 = 0;
		pMIDITrack->m_lReserved4 = 0;
	}
	/* このトラック内の各MIDIEventについて */
	for (j = 0; j < lNumEvent; j++) {
		/* MIDIイベントを1つ読み込み */
		pMIDIEvent = MIDIEvent_LoadFromBinary (pFile);
		if (pMIDIEvent == NULL) {
			return NULL;
		}
		/* MIDIトラックにMIDIイベントを挿入 */
		if (MIDITrack_AddSingleEventForce (pMIDITrack, pMIDIEvent) == 0) { // 20090712修正
			return NULL;
		}
	}
	/* 結合されたイベントの結合状態復元処理 */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		if ((long)(pMIDIEvent->m_pPrevCombinedEvent) == -1) {
			pMIDIEvent->m_pPrevCombinedEvent = NULL;
		}
		else {
			MIDIEvent* pTempEvent;
			forEachEvent (pMIDITrack, pTempEvent) {
				if (pTempEvent->m_lTempIndex ==
					(long)(pMIDIEvent->m_pPrevCombinedEvent)) {
					pMIDIEvent->m_pPrevCombinedEvent = pTempEvent;
					break;
				}
			}
			if (pTempEvent == NULL) {
				return NULL;
			}
		}
		if ((long)(pMIDIEvent->m_pNextCombinedEvent) == -1) {
			pMIDIEvent->m_pNextCombinedEvent = NULL;
		}
		else {
			MIDIEvent* pTempEvent;
			forEachEvent (pMIDITrack, pTempEvent) {
				if (pTempEvent->m_lTempIndex ==
					(long)(pMIDIEvent->m_pNextCombinedEvent)) {
					pMIDIEvent->m_pNextCombinedEvent = pTempEvent;
					break;
				}
			}
			if (pTempEvent == NULL) {
				return NULL;
			}
		}
	}
	return pMIDITrack;
}


/* MIDIDataをバイナリファイルから読み込み(ANSI) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromBinaryA (const char* pszFileName) {
	long lRet = 0;
	char szType[4] = {0, 0, 0, 0};
	long lFormat = 0;
	long lNumTrack = 0;
	long lTimeBase = 0;
	long lTimeMode = 0;
	long lTimeResolution = 0;
	long lReserved1 = 0;
	long lReserved2 = 0;
	long lReserved3 = 0;
	long lReserved4 = 0;
	long lUser1 = 0;
	long lUser2 = 0;
	long lUser3 = 0;
	long lUserFlag = 0;
	MIDIData* pMIDIData = NULL;
	long i = 0;

	/* ファイルを開く */
	FILE* pFile = fopen (pszFileName, "rb");
	if (pFile == NULL) {
		return NULL;
	}
	/* MIDIDataについてプロパティ読み込み */
	lRet = fread (szType, 4, 1, pFile);
	if (lRet < 1 || (memcmp (szType, "MDat", 4) != 0 && memcmp (szType, "MDa2", 4) != 0)) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lFormat, 4, 1, pFile);
	if (lRet < 1 || lFormat < 0 || lFormat > 2) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lNumTrack, 4, 1, pFile);
	if (lRet < 1 || lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lTimeBase, 4, 1, pFile);
	if (lRet < 1 || lTimeBase <= 0 || lTimeBase >= 65536) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase & 0x00008000) {
		lTimeMode = 256 - ((lTimeBase & 0x0000FF00) >> 8);
		if (lTimeMode != MIDIDATA_SMPTE24BASE &&
			lTimeMode != MIDIDATA_SMPTE25BASE &&
			lTimeMode != MIDIDATA_SMPTE29BASE &&
			lTimeMode != MIDIDATA_SMPTE30BASE) {
			fclose (pFile);
			return NULL;
		}
		lTimeResolution = lTimeBase & 0x000000FF;
	}
	else {
		lTimeMode =  MIDIDATA_TPQNBASE;
		lTimeResolution = lTimeBase & 0x00007FFF;
	}
	
	/* MIDIDataLib1.4から追加した項目(20081008追加) */
	if (memcmp (szType, "MDa2", 4) == 0) {
		lRet = fread (&lReserved1, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
		lRet = fread (&lReserved2, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
		lRet = fread (&lReserved3, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
		lRet = fread (&lReserved4, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
	}
	lRet = fread (&lUser1, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lUser2, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lUser3, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lUserFlag, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}

	/* MIDIDataの生成及びプロパティ設定 */
	pMIDIData = MIDIData_Create (lFormat, 0, lTimeMode, lTimeResolution);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}
	/* MIDIDataLib1.4から追加した項目(20081008追加) */
	if (memcmp (szType, "MDa2", 4) == 0) {
		pMIDIData->m_lReserved1 = lReserved1;
		pMIDIData->m_lReserved2 = lReserved2;
		pMIDIData->m_lReserved3 = lReserved3;
		pMIDIData->m_lReserved4 = lReserved4;
	}
	/* MIDIDataLib1.3以前(20081008追加) */
	else {
		pMIDIData->m_lReserved1 = 0;
		pMIDIData->m_lReserved2 = 0;
		pMIDIData->m_lReserved3 = 0;
		pMIDIData->m_lReserved4 = 0;
	}
	pMIDIData->m_lUser1 = lUser1;
	pMIDIData->m_lUser2 = lUser2;
	pMIDIData->m_lUser3 = lUser3;
	pMIDIData->m_lUserFlag = lUserFlag;

	/* このファイル内の各MIDITrackについて */
	for (i = 0; i < lNumTrack; i++) {
		/* MIDIトラックをひとつ読み込み */
		MIDITrack* pMIDITrack = MIDITrack_LoadFromBinary (pFile);
		if (pMIDITrack == NULL) {
			fclose (pFile);
			MIDIData_Delete (pMIDIData);
			return NULL;
		}
		/* MIDIデータにMIDIトラックを追加 */
		if (MIDIData_AddTrack (pMIDIData, pMIDITrack) == 0) {
			fclose (pFile);
			MIDIData_Delete (pMIDIData);
			return NULL;
		}
	}
	fclose (pFile);
	return pMIDIData;
}

/* MIDIDataをバイナリファイルから読み込み(UNICODE) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
MIDIData* __stdcall MIDIData_LoadFromBinaryW (const wchar_t* pszFileName) {
	long lRet = 0;
	char szType[4] = {0, 0, 0, 0};
	long lFormat = 0;
	long lNumTrack = 0;
	long lTimeBase = 0;
	long lTimeMode = 0;
	long lTimeResolution = 0;
	long lReserved1 = 0;
	long lReserved2 = 0;
	long lReserved3 = 0;
	long lReserved4 = 0;
	long lUser1 = 0;
	long lUser2 = 0;
	long lUser3 = 0;
	long lUserFlag = 0;
	MIDIData* pMIDIData = NULL;
	long i = 0;

	/* ファイルを開く */
	FILE* pFile = _wfopen (pszFileName, L"rb");
	if (pFile == NULL) {
		return NULL;
	}
	/* MIDIDataについてプロパティ読み込み */
	lRet = fread (szType, 4, 1, pFile);
	if (lRet < 1 || (memcmp (szType, "MDat", 4) != 0 && memcmp (szType, "MDa2", 4) != 0)) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lFormat, 4, 1, pFile);
	if (lRet < 1 || lFormat < 0 || lFormat > 2) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lNumTrack, 4, 1, pFile);
	if (lRet < 1 || lNumTrack < 0 || lNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lTimeBase, 4, 1, pFile);
	if (lRet < 1 || lTimeBase <= 0 || lTimeBase >= 65536) {
		fclose (pFile);
		return NULL;
	}
	if (lTimeBase & 0x00008000) {
		lTimeMode = 256 - ((lTimeBase & 0x0000FF00) >> 8);
		if (lTimeMode != MIDIDATA_SMPTE24BASE &&
			lTimeMode != MIDIDATA_SMPTE25BASE &&
			lTimeMode != MIDIDATA_SMPTE29BASE &&
			lTimeMode != MIDIDATA_SMPTE30BASE) {
			fclose (pFile);
			return NULL;
		}
		lTimeResolution = lTimeBase & 0x000000FF;
	}
	else {
		lTimeMode =  MIDIDATA_TPQNBASE;
		lTimeResolution = lTimeBase & 0x00007FFF;
	}

	/* MIDIDataLib1.4から追加した項目(20081008追加) */
	if (memcmp (szType, "MDa2", 4) == 0) {
		lRet = fread (&lReserved1, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
		lRet = fread (&lReserved2, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
		lRet = fread (&lReserved3, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
		lRet = fread (&lReserved4, 4, 1, pFile);
		if (lRet < 1) {
			fclose (pFile);
			return NULL;
		}
	}
	lRet = fread (&lUser1, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lUser2, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lUser3, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}
	lRet = fread (&lUserFlag, 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return NULL;
	}

	/* MIDIDataの生成及びプロパティ設定 */
	pMIDIData = MIDIData_Create (lFormat, 0, lTimeMode, lTimeResolution);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}
	/* MIDIDataLib1.4から追加した項目(20081008追加) */
	if (memcmp (szType, "MDa2", 4) == 0) {
		pMIDIData->m_lReserved1 = lReserved1;
		pMIDIData->m_lReserved2 = lReserved2;
		pMIDIData->m_lReserved3 = lReserved3;
		pMIDIData->m_lReserved4 = lReserved4;
	}
	/* MIDIDataLib1.3以前(20081008追加) */
	else {
		pMIDIData->m_lReserved1 = 0;
		pMIDIData->m_lReserved2 = 0;
		pMIDIData->m_lReserved3 = 0;
		pMIDIData->m_lReserved4 = 0;
	}
	pMIDIData->m_lUser1 = lUser1;
	pMIDIData->m_lUser2 = lUser2;
	pMIDIData->m_lUser3 = lUser3;
	pMIDIData->m_lUserFlag = lUserFlag;

	/* このファイル内の各MIDITrackについて */
	for (i = 0; i < lNumTrack; i++) {
		/* MIDIトラックをひとつ読み込み */
		MIDITrack* pMIDITrack = MIDITrack_LoadFromBinary (pFile);
		if (pMIDITrack == NULL) {
			fclose (pFile);
			MIDIData_Delete (pMIDIData);
			return NULL;
		}
		/* MIDIデータにMIDIトラックを追加 */
		if (MIDIData_AddTrack (pMIDIData, pMIDITrack) == 0) {
			fclose (pFile);
			MIDIData_Delete (pMIDIData);
			return NULL;
		}
	}
	fclose (pFile);
	return pMIDIData;
}


/* MIDIEventをバイナリファイルに保存(隠蔽) */
long __stdcall MIDIEvent_SaveAsBinary (MIDIEvent* pMIDIEvent, FILE* pFile) {
	long lRet;
	long lNullIndex = -1;
	lRet = fwrite ("MEvt", 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lTempIndex), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lTime), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lKind), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lLen), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	if (pMIDIEvent->m_pData != NULL && pMIDIEvent->m_lLen > 0) { /* 20091024条件式追加 */
		lRet = fwrite (pMIDIEvent->m_pData, 1, pMIDIEvent->m_lLen, pFile);
		if (lRet < pMIDIEvent->m_lLen) {
			return 0;
		}
	}
	lRet = fwrite (pMIDIEvent->m_pPrevCombinedEvent ? 
		&(pMIDIEvent->m_pPrevCombinedEvent->m_lTempIndex) : &lNullIndex, 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (pMIDIEvent->m_pNextCombinedEvent ? 
		&(pMIDIEvent->m_pNextCombinedEvent->m_lTempIndex) : &lNullIndex, 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lUser1), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lUser2), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lUser3), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDIEvent->m_lUserFlag), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	return 1;
}

/* MIDITrackをバイナリファイルに保存(隠蔽) */
long __stdcall MIDITrack_SaveAsBinary (MIDITrack* pMIDITrack, FILE* pFile) {
	long lEventCount = MIDITrack_CountEvent (pMIDITrack);
	long lRet;
	MIDIEvent* pMIDIEvent;
	lRet = fwrite ("MTr2", 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lTempIndex), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lNumEvent), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lInputOn), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lInputPort), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lInputChannel), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lOutputOn), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lOutputPort), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lOutputChannel), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lTimePlus), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lKeyPlus), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lVelocityPlus), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lViewMode), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lForeColor), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lBackColor), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lReserved1), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lReserved2), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lReserved3), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lReserved4), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lUser1), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lUser2), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lUser3), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	lRet = fwrite (&(pMIDITrack->m_lUserFlag), 4, 1, pFile);
	if (lRet < 1) {
		return 0;
	}
	/* このMIDIトラック内の各イベントについて */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		long lRet = MIDIEvent_SaveAsBinary (pMIDIEvent, pFile);
		if (lRet == 0) {
			return 0;
		}
	}
	return 1;
}

/* MIDIDataをバイナリファイルに保存(ANSI) */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsBinaryA (MIDIData* pMIDIData, const char* pszFileName) {
	MIDITrack* pMIDITrack = NULL;
	long lRet = 0;
	long lTrackCount = 0;
	FILE* pFile = fopen (pszFileName, "wb");
	if (pFile == NULL) {
		return 0;
	}
	/* MIDIDataについて */
	lTrackCount = MIDIData_CountTrack (pMIDIData);
	lRet = fwrite ("MDa2", 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lFormat), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lNumTrack), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lTimeBase), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved1), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved2), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved3), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved4), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUser1), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUser2), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUser3), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUserFlag), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	/* このMIDIデータ内の各トラックについて */
	forEachTrack (pMIDIData, pMIDITrack) {
		lRet = MIDITrack_SaveAsBinary (pMIDITrack, pFile);
		if (lRet == 0) {
			fclose (pFile);
			return 0;
		}
	}
	fclose (pFile);
	return 1;
}

/* MIDIDataをバイナリファイルに保存(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsBinaryW (MIDIData* pMIDIData, const wchar_t* pszFileName) {
	MIDITrack* pMIDITrack = NULL;
	long lRet = 0;
	long lTrackCount = 0;
	FILE* pFile = _wfopen (pszFileName, L"wb");
	if (pFile == NULL) {
		return 0;
	}
	/* MIDIDataについて */
	lTrackCount = MIDIData_CountTrack (pMIDIData);
	lRet = fwrite ("MDa2", 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lFormat), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lNumTrack), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lTimeBase), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved1), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved2), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved3), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lReserved4), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUser1), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUser2), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUser3), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	lRet = fwrite (&(pMIDIData->m_lUserFlag), 4, 1, pFile);
	if (lRet < 1) {
		fclose (pFile);
		return 0;
	}
	/* このMIDIデータ内の各トラックについて */
	forEachTrack (pMIDIData, pMIDITrack) {
		lRet = MIDITrack_SaveAsBinary (pMIDITrack, pFile);
		if (lRet == 0) {
			fclose (pFile);
			return 0;
		}
	}
	fclose (pFile);
	return 1;
}


/* MIDITrackをCherry形式のメモリブロックから読み込み(非公開) */
/* 新しいMIDIトラックへのポインタを返す(失敗時NULL) */
MIDITrack* __stdcall MIDITrack_LoadFromCherry (long lTrackIndex, unsigned char*pTrackData, long lTrackLen) {
	MIDITrack* pMIDITrack = NULL;
	unsigned char* p = pTrackData;
	long lOutputPort = 0;
	long lOutputChannel = 0;
	char szTempTrackName[256];
	memset (szTempTrackName, 0, sizeof (szTempTrackName));

	/* 新規MIDIトラックの生成 */
	pMIDITrack = MIDITrack_Create ();
	if (pMIDITrack == NULL) {
		return NULL;
	}

	pMIDITrack->m_lTempIndex = lTrackIndex;

	/* モード(通常／ドラム)の取得 */
	pMIDITrack->m_lViewMode = (long)(*(unsigned char*)(p + 12));

	/* 出力ポート番号の取得 */
	lOutputPort = CLIP (0, (long)(*(char*)(p + 13)), 3);
	pMIDITrack->m_lOutputPort = lOutputPort;

	/* 出力チャンネルの取得 */
	lOutputChannel = CLIP (-1, (long)(*(char*)(p + 14)), 15);
	pMIDITrack->m_lOutputChannel = (lTrackIndex <= 1 ? -1 : lOutputChannel);

	/* キー+の取得 */
	pMIDITrack->m_lKeyPlus = (long)(*(char*)(p + 15));

	/* タイム+の取得 */
	pMIDITrack->m_lTimePlus = (long)(*(char*)(p + 16));

	/* ベロシティ+の取得 */
	pMIDITrack->m_lVelocityPlus = (long)(*(char*)(p + 17));

	/* その他の取得 */
	pMIDITrack->m_lReserved1 = *(p + 28) | (*(p + 29) << 8) | (*(p + 30) << 16) | (*(p + 31) << 24);

	/* */
	pMIDITrack->m_lInputOn = 1;
	pMIDITrack->m_lInputPort = lOutputPort;
	pMIDITrack->m_lInputChannel = (lTrackIndex <= 1 ? -1 : lOutputChannel);

	/* タイトルの取得 */
	memcpy (szTempTrackName, pTrackData + 60, 64);
	MIDITrack_SetNameA (pMIDITrack, szTempTrackName);

	/* 各イベントの追加処理 */
	p = pTrackData + 124;
	while (p < pTrackData + lTrackLen) {
		/* ノート */
		if (0x00 <= *p && *p <= 0x7F) {
			long lTime = (long)(*(long*)(p + 2));
			long lKey = (long)(*p);
			long lVel = (long)(*(unsigned short*)(p + 8));
			long lDur = (long)(*(unsigned short*)(p + 6));
			MIDITrack_InsertNote (pMIDITrack, lTime, lOutputChannel, lKey, lVel, lDur);
			p += 10;
		}
		/* コントロールチェンジ又は仮想コントロールチェンジ(テンポ含む) */
		else if (*p == 0x82) {
			long lTime = (long)(*(long*)(p + 2));
			long lNum = (long)(*(p + 1));
			long lBank = (long)(*(unsigned short*)(p + 6));
			long lVal = (long)(*(unsigned short*)(p + 8));
			unsigned char bySysx[256];
			memset (bySysx, 0, sizeof (bySysx));
			/* コントロールチェンジ */
			if (0 <= lNum && lNum <= 127 && 0 <= lVal && lVal <= 127 && lTrackIndex >= 2) {
				MIDITrack_InsertControlChange (pMIDITrack, lTime, lOutputChannel, lNum, lVal);
			}
			/* ピッチベンドセンシティビティ */
			else if (lNum == 0x80) {
				long lMSB = 0;
				long lLSB = 0;
				MIDITrack_InsertRPNChange (pMIDITrack, lTime, lOutputChannel, lMSB, lLSB, lVal);
			}
			/* ファインチューン(TODO:値は保留:0x0000-0x4000-0x8000) */
			else if (lNum == 0x81) {
				long lMSB = 0;
				long lLSB = 1;
				MIDITrack_InsertRPNChange (pMIDITrack, lTime, lOutputChannel, lMSB, lLSB, lVal);
			}
			/* コースチューン */
			else if (lNum == 0x82) {
				long lMSB = 0;
				long lLSB = 2;
				MIDITrack_InsertRPNChange (pMIDITrack, lTime, lOutputChannel, lMSB, lLSB, lVal);
			}
			/* マスターボリューム */
			else if (lNum == 0x83) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x7F;
				bySysx[2] = 0x7F;
				bySysx[3] = 0x04;
				bySysx[4] = 0x01;
				bySysx[5] = 0x00;
				bySysx[6] = (unsigned char)CLIP (0, lVal, 127);
				bySysx[7] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 8);
			}
			/* V.Rate / V.Dep / V.Del / HPF.Cutoff / E.G.Delay */
			else if (0x8C <= lNum && lNum <= 0x90) {
				long lMSB = 1;
				long lLSBList[5] = {8, 9, 10, 36, 100};
				long lLSB = lLSBList[lNum - 0x8C];
				MIDITrack_InsertNRPNChange (pMIDITrack, lTime, lOutputChannel, lMSB, lLSB, lVal);
			}
			/* PEG InitLev / PEG AttackTime / PEG RelLev / PEG RelTime */
			else if (0x91 <= lNum && lNum <= 0x94) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x43;
				bySysx[2] = 0x10;
				bySysx[3] = 0x4C;
				bySysx[4] = 0x08;
				bySysx[5] = 0x01;
				bySysx[6] = (unsigned char)(lNum - 0x28); /* 0x69～0x6C */
				bySysx[7] = (unsigned char)CLIP (0, lVal, 127);
				bySysx[8] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 9);
			}
			/* キーアフタータッチ */
			else if (lNum == 0x97) {
				MIDITrack_InsertKeyAftertouch (pMIDITrack, lTime, lOutputChannel, 0, lVal);
			}
			/* ピッチベンド */
			else if (lNum == 0x99) {
				MIDITrack_InsertPitchBend (pMIDITrack, lTime, lOutputChannel, lVal);
			}
			/* テンポ */
			else if (lNum == 0x9B) {
				long lTempo = 60000000 / CLIP (1, lVal, 65535);
				MIDITrack_InsertTempo (pMIDITrack, lTime, lTempo);
			}
			/* GM System On */
			else if (lNum == 0xA0) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x7E;
				bySysx[2] = 0x7F;
				bySysx[3] = 0x09;
				bySysx[4] = 0x01;
				bySysx[5] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 6);
			}
			/* XG System On */
			/* GS Reset */
			else if (lNum == 0xA1) {
				unsigned char ucModule = (unsigned char)(pMIDITrack->m_lReserved1 & 0x000000FF);
				if (64 <= ucModule && ucModule < 96) { /* GS */
					bySysx[0] = 0xF0;
					bySysx[1] = 0x41;
					bySysx[2] = 0x10;
					bySysx[3] = 0x42;
					bySysx[4] = 0x12;
					bySysx[5] = 0x40;
					bySysx[6] = 0x00;
					bySysx[7] = 0x7F;
					bySysx[8] = 0x00;
					bySysx[9] = 128 - (Sum (&bySysx[5], 4) % 128);
					bySysx[10] = 0xF7;
					MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 11);
				}
				else { /* XG, others */
					bySysx[0] = 0xF0;
					bySysx[1] = 0x43;
					bySysx[2] = 0x10;
					bySysx[3] = 0x4C;
					bySysx[4] = 0x00;
					bySysx[5] = 0x00;
					bySysx[6] = 0x7E;
					bySysx[7] = 0x00;
					bySysx[8] = 0xF7;
					MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 9);
				}
			}
			/* 88 Reset */
			else if (lNum == 0xA2) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x41;
				bySysx[2] = 0x10;
				bySysx[3] = 0x42;
				bySysx[4] = 0x12;
				bySysx[5] = 0x00;
				bySysx[6] = 0x00;
				bySysx[7] = 0x7F;
				bySysx[8] = (unsigned char)CLIP (0, lVal, 1);
				bySysx[9] = ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F);
				bySysx[10] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 11);
			}
			/* Dry Level */
			else if (lNum == 0xA5) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x43;
				bySysx[2] = 0x10;
				bySysx[3] = 0x4C;
				bySysx[4] = 0x08;
				bySysx[5] = 0x03;
				bySysx[6] = 0x11;
				bySysx[7] = (unsigned char)CLIP (0, lVal, 127);
				bySysx[8] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 9);
			}
			/* MW params */
			else if (0xA6 <= lNum && lNum <= 0xAB) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x43;
				bySysx[2] = 0x10;
				bySysx[3] = 0x4C;
				bySysx[4] = 0x08;
				bySysx[5] = 0x02;
				bySysx[6] = (unsigned char)(lNum - 0xA6 + 0x1D); /* 0x1D～0x22 */
				bySysx[7] = (unsigned char)CLIP (0, lVal, 127);
				bySysx[8] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 9);
			}
			/* GS Reverb / Chorus / Deray Macro */
			else if (0xAC <= lNum && lNum <= 0xAE) {
				unsigned char ucType[3] = {0x30, 0x38, 0x50};
				bySysx[0] = 0xF0;
				bySysx[1] = 0x41;
				bySysx[2] = 0x10;
				bySysx[3] = 0x42;
				bySysx[4] = 0x12;
				bySysx[5] = 0x40;
				bySysx[6] = 0x01;
				bySysx[7] = ucType[lNum - 0xAC];
				bySysx[8] = (unsigned char)CLIP (0, lVal, 7);
				bySysx[9] = ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F);
				bySysx[10] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 11);
			}
			/* EQ */
			else if (0xAF <= lNum && lNum <= 0xB2) {
				unsigned char ucModule = (unsigned char)(pMIDITrack->m_lReserved1 & 0x000000FF);
				if (64 <= ucModule && ucModule < 96) { /* GS */
					bySysx[0] = 0xF0;
					bySysx[1] = 0x41;
					bySysx[2] = 0x10;
					bySysx[3] = 0x42;
					bySysx[4] = 0x12;
					bySysx[5] = 0x40;
					bySysx[6] = 0x02;
					bySysx[7] = (unsigned char)(lNum - 0xAF);
					bySysx[8] = (unsigned char)CLIP (0, lVal, 127);
					bySysx[9] = ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F);
					bySysx[10] = 0xF7;
					MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 11);
				}
				else if (lTrackIndex >= 2) { /* XG, others */
					long lMSB = 1;
					long lLSBList[4] = {48, 49, 52, 53};
					long lLSB = lLSBList[lNum - 0xAF];
					MIDITrack_InsertNRPNChange (pMIDITrack, lTime, lOutputChannel, lMSB, lLSB, lVal); 
				}
			}
			/* DX:AssignPart or Assign Mode Full */
			else if (lNum == 0xB3) {
				unsigned char ucModule = (unsigned char)(pMIDITrack->m_lReserved1 & 0x000000FF);
				if (64 <= ucModule && ucModule < 96) { /* GS */
					bySysx[0] = 0xF0;
					bySysx[1] = 0x41;
					bySysx[2] = 0x10;
					bySysx[3] = 0x42;
					bySysx[4] = 0x12;
					bySysx[5] = 0x40;
					bySysx[6] = 0x17;
					bySysx[7] = 0x14;
					bySysx[8] = (unsigned char)CLIP (0, lVal, 127);
					bySysx[9] = ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F);
					bySysx[10] = 0xF7;
					MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 11);
				}
				else { /* XG, others */
					bySysx[0] = 0xF0;
					bySysx[1] = 0x43;
					bySysx[2] = 0x10;
					bySysx[3] = 0x4C;
					bySysx[4] = 0x70;
					bySysx[5] = 0x02;
					bySysx[6] = 0x00;
					bySysx[7] = 0x02;
					bySysx[8] = 0xF7;
					MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 9);
				}
			}
			/* GS: Use For Drum Norm/Drum1/Drum2 */
			else if (lNum == 0xB4) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x41;
				bySysx[2] = 0x10;
				bySysx[3] = 0x42;
				bySysx[4] = 0x12;
				bySysx[5] = 0x40;
				bySysx[6] = 0x17;
				bySysx[7] = 0x15;
				bySysx[8] = (unsigned char)CLIP (0, lVal, 2);
				bySysx[9] = ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F);
				bySysx[10] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 11);
			}
			/* GS: EQ SW off/on */
			else if (lNum == 0xB5) {
				bySysx[0] = 0xF0;
				bySysx[1] = 0x41;
				bySysx[2] = 0x10;
				bySysx[3] = 0x42;
				bySysx[4] = 0x12;
				bySysx[5] = 0x40;
				bySysx[6] = 0x47;
				bySysx[7] = 0x20;
				bySysx[8] = (unsigned char)CLIP (0, lVal, 1);
				bySysx[9] = ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F);
				bySysx[10] = 0xF7;
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, bySysx, 11);
			}
			/* Drum params */
			else if (0xC8 <= lNum && lNum <= 0xD7 && lTrackIndex >= 2) {
				long lMSBList[16] = {20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 36, 52, 48, 53, 49};
				long lMSB = lMSBList[lNum - 0xC8];
				long lLSB = (unsigned char)(lBank & 0x007F);
				MIDITrack_InsertNRPNChange (pMIDITrack, lTime, lOutputChannel, lMSB, lLSB, lVal); 
			}
			p += 10;
		}
		/* プログラムチェンジ又はパッチチェンジ */
		else if (*p == 0x83) {
			long lTime = (long)(*(long*)(p + 2));
			long lLSB = (long)(*(unsigned char*)(p + 6));
			long lMSB = (long)(*(unsigned char*)(p + 7));
			long lVal = (long)(*(unsigned short*)(p + 8));
			if (0 <= lMSB && lMSB <= 127 && 0 <= lLSB && lLSB <= 127 && 0 <= lVal && lVal <= 127) {
				MIDITrack_InsertPatchChange (pMIDITrack, lTime, lOutputChannel, lMSB, lLSB, lVal);
			}
			else {
				if (0 <= lMSB && lMSB <= 127) {
					MIDITrack_InsertControlChange (pMIDITrack, lTime, lOutputChannel, 0, lMSB);
				}
				if (0 <= lLSB && lLSB <= 127) {
					MIDITrack_InsertControlChange (pMIDITrack, lTime, lOutputChannel, 0, lLSB);
				}
				if (0 <= lVal && lVal <= 127) {
					MIDITrack_InsertProgramChange (pMIDITrack, lTime, lOutputChannel, lVal);
				}
			}
			p += 10;
		}
		/* MIDIポート・チャンネル切替 */
		else if (*p == 0x84) {
			long lTime = (long)(*(long*)(p + 2));
			lOutputPort = (long)(*(unsigned short*)(p + 6));
			lOutputChannel = (long)(*(unsigned short*)(p + 8));
			p += 10;
		}
		/* システムエクスクルーシヴ */
		else if (*p == 0xC0) {
			long lTime = (long)(*(long*)(p + 2));
			long lLen = (long)(*(long*)(p + 10));
			MIDITrack_InsertSysExEvent (pMIDITrack, lTime, p + 14, lLen);
			p += (14 + lLen);
		}
		/* テキストイベント */
		else if (*p == 0xC1) {
			long lTime = (long)(*(long*)(p + 2));
			long lLen = (long)(*(long*)(p + 10));
			char szText[65535];
			memset (szText, 0, sizeof (szText));
			memcpy (szText, p + 14, lLen);
			MIDITrack_InsertTextEventA (pMIDITrack, lTime, szText);
			p += (14 + lLen);
		}
		/* 歌詞 */
		else if (*p == 0xC2) {
			long lTime = (long)(*(long*)(p + 2));
			long lLen = (long)(*(long*)(p + 10));
			char szText[65535];
			memset (szText, 0, sizeof (szText));
			memcpy (szText, p + 14, lLen);
			MIDITrack_InsertLyricA (pMIDITrack, lTime, szText);
			p += (14 + lLen);
		}
		/* エンドオブトラック */
		else if (*p == 0xFF) {
			long lTime = (long)(*(long*)(p + 2));
			MIDITrack_InsertEndofTrack (pMIDITrack, lTime);
			p += 10;
			break;
		}
		/* 未定義のイベント */
		else {
			long lTime = (long)(*(long*)(p + 2));
			MIDITrack_InsertEndofTrack (pMIDITrack, lTime);
			p += 10;
			break;
		}
	}
	return pMIDITrack;
}

/* MIDIDataをCherryシーケンスファイル(*.chy)から読み込み(ANSI) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromCherryA (const char* pszFileName) {
	MIDIData* pMIDIData = NULL;
	MIDITrack* pMIDITrack = NULL;
	FILE* pFile = NULL;
	long lRet = 0;
	long lTrackLen = 0;
	long lTrackIndex = 0;
	unsigned char* pTrackData = NULL;
	unsigned char byBuf[256];
	char szTempTitle[256];
	char szTempCopyright[256];
	long lTempNumTrack = 0;
	long lTempExtendedDataAddress[64];
	long lTempTrackAddress[256];
	memset (szTempTitle, 0, sizeof (szTempTitle));
	memset (szTempCopyright, 0, sizeof (szTempCopyright));
	memset (lTempExtendedDataAddress, 0, sizeof (long) * 64);
	memset (lTempTrackAddress, 0, sizeof (long) * 256);

	pMIDIData = malloc (sizeof (MIDIData));
	memset (pMIDIData, 0, sizeof (MIDIData));
	if (pMIDIData == NULL) {
		return NULL;
	}
	pMIDIData->m_pFirstTrack = NULL;
	pMIDIData->m_pLastTrack = NULL;
	pMIDIData->m_pNextSeq = NULL;
	pMIDIData->m_pPrevSeq = NULL;
	pMIDIData->m_pParent = NULL;
	pMIDIData->m_lUser1 = 0;
	pMIDIData->m_lUser2 = 0;
	pMIDIData->m_lUser3 = 0;

	pFile = fopen (pszFileName, "rb");
	if (pFile == NULL) {
		free (pMIDIData);
		return NULL;
	}
	if (fread (byBuf, sizeof(char), 256, pFile) < 256) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 識別ID読み込み */
	if (memcmp (byBuf, "CHRY0101", 8) != 0) {
		free (pMIDIData);
		fclose (pFile);
		return NULL; 
	}
	pMIDIData->m_lFormat = 1;

	/* タイムベース読み込み(TPQNベース固定、1～32767) */
	pMIDIData->m_lTimeBase = (long)(*(unsigned short*)(byBuf + 8));
	if (pMIDIData->m_lTimeBase < 1 || pMIDIData->m_lTimeBase > MIDIDATA_MAXTPQNRESOLUTION) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}
	
	/* トラック数読み込み */
	lTempNumTrack = (long)(*(unsigned short*)(byBuf + 10));
	if (lTempNumTrack <= 0 || lTempNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* タイトル読み込み */
	if (fread (szTempTitle, sizeof(char), 128, pFile) < 128) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 著作権読み込み */
	if (fread (szTempCopyright, sizeof(char), 128, pFile) < 128) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 拡張データ先頭アドレス配列読み込み */
	if (fread (lTempExtendedDataAddress, sizeof(long), 64, pFile) < 64) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* トラック先頭アドレス配列読み込み */
	if (fread (lTempTrackAddress, sizeof(long), lTempNumTrack, pFile) < (unsigned long)lTempNumTrack) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 各トラック読み込み */
	for (lTrackIndex = 0; lTrackIndex < lTempNumTrack; lTrackIndex++) {

		/* トラックの長さ[バイト]取得 */
		if (fread (&lTrackLen, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		if (lTrackLen < 128 || lTrackLen >= 16777216) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}

		/* 1トラック用のメモリ割り当て */
		pTrackData = malloc (lTrackLen - 4);
		if (pTrackData == NULL) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		
		/* 1トラック読み込み */
		if (fread (pTrackData, sizeof (char), lTrackLen - 4, pFile) < (unsigned long)lTrackLen - 4) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}

		/* トラック内のデータ読み込み関数を呼び出し */
		pMIDITrack = MIDITrack_LoadFromCherry (lTrackIndex, pTrackData, lTrackLen);
		if (pMIDITrack == NULL) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		
		/* 新しく作られたトラックを追加 */
		lRet = MIDIData_AddTrackForce (pMIDIData, pMIDITrack);
		if (lRet == 0) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
		}
	
		free (pTrackData);
		pTrackData = NULL;
	}

	/* 拡張データ */
	pMIDITrack = pMIDIData->m_pFirstTrack;
	assert (pMIDITrack);
	/* 調性拍子マーカーデータ(ある場合のみ) */
           	if (!feof (pFile) && lTempExtendedDataAddress[0] != 0x00000000) {
		long lExtendedLen = 0;
		long lTimeKeySignatureIndex = 0;
		if (fread (&lExtendedLen, sizeof (long), 1, pFile) < 1) {
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		/* 1回につき80バイト */
		for (lTimeKeySignatureIndex = 0; 
			lTimeKeySignatureIndex < lExtendedLen / 80; 
			lTimeKeySignatureIndex++) {
			long lMeasure = 0;
			long lTime = 0;
			long lnn = 0;
			long lddTemp = 0;
			long ldd = 2;
			long lcc = 24;
			long lbb = 8;
			long lsf = 0;
			long lmi = 0;
			long j;
			memset (byBuf, 0, sizeof (byBuf));
			if (fread (byBuf, sizeof (char), 80, pFile) < 80) {
				free (pMIDIData);
				fclose (pFile);
				return NULL;
			}
			lMeasure = (long)(*(unsigned short*)(byBuf));
			lnn = (long)(*(unsigned char*)(byBuf + 2));
			lddTemp = (long)(*(unsigned char*)(byBuf + 3));
			for (j = 0; j < 7; j++) {
				if (lddTemp == (1 << j)) {
					ldd = j;
					break;
				}
			}
 			lsf = (long)(*(unsigned char*)(byBuf + 4));
			if (lsf >= 9) {
				lsf = 8 - lsf;
			}
			byBuf[79] = 0;
			MIDIData_MakeTime (pMIDIData, lMeasure, 0, 0, &lTime);
			/* マーカー挿入 */
			MIDITrack_InsertMarkerA (pMIDITrack, lTime, (char*)(&byBuf[16]));
			/* 調性記号挿入 */
			MIDITrack_InsertKeySignature (pMIDITrack, lTime, lsf, lmi);
			/* 拍子記号挿入 */
			MIDITrack_InsertTimeSignature (pMIDITrack, lTime, lnn, ldd, lcc, lbb);
		}
	}

	fclose (pFile);

	MIDIData_SetTitleA (pMIDIData, szTempTitle);
	MIDIData_SetCopyrightA (pMIDIData, szTempCopyright);
	
	return pMIDIData;
}

/* MIDIDataをCherryシーケンスファイル(*.chy)から読み込み(UNICODE) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
MIDIData* __stdcall MIDIData_LoadFromCherryW (const wchar_t* pszFileName) {
	MIDIData* pMIDIData = NULL;
	MIDITrack* pMIDITrack = NULL;
	FILE* pFile = NULL;
	long lRet = 0;
	long lTrackLen = 0;
	long lTrackIndex = 0;
	unsigned char* pTrackData = NULL;
	unsigned char byBuf[256];
	char szTempTitle[256];
	char szTempCopyright[256];
	long lTempNumTrack = 0;
	long lTempExtendedDataAddress[64];
	long lTempTrackAddress[256];
	memset (szTempTitle, 0, sizeof (szTempTitle));
	memset (szTempCopyright, 0, sizeof (szTempCopyright));
	memset (lTempExtendedDataAddress, 0, sizeof (long) * 64);
	memset (lTempTrackAddress, 0, sizeof (long) * 256);

	pMIDIData = malloc (sizeof (MIDIData));
	memset (pMIDIData, 0, sizeof (MIDIData));
	if (pMIDIData == NULL) {
		return NULL;
	}
	pMIDIData->m_pFirstTrack = NULL;
	pMIDIData->m_pLastTrack = NULL;
	pMIDIData->m_pNextSeq = NULL;
	pMIDIData->m_pPrevSeq = NULL;
	pMIDIData->m_pParent = NULL;
	pMIDIData->m_lUser1 = 0;
	pMIDIData->m_lUser2 = 0;
	pMIDIData->m_lUser3 = 0;

	pFile = _wfopen (pszFileName, L"rb");
	if (pFile == NULL) {
		free (pMIDIData);
		return NULL;
	}
	if (fread (byBuf, sizeof(char), 256, pFile) < 256) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 識別ID読み込み */
	if (memcmp (byBuf, "CHRY0101", 8) != 0) {
		free (pMIDIData);
		fclose (pFile);
		return NULL; 
	}
	pMIDIData->m_lFormat = 1;

	/* タイムベース読み込み(TPQNベース固定、1～32767) */
	pMIDIData->m_lTimeBase = (long)(*(unsigned short*)(byBuf + 8));
	if (pMIDIData->m_lTimeBase < 1 || pMIDIData->m_lTimeBase > MIDIDATA_MAXTPQNRESOLUTION) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}
	
	/* トラック数読み込み */
	lTempNumTrack = (long)(*(unsigned short*)(byBuf + 10));
	if (lTempNumTrack <= 0 || lTempNumTrack > MIDIDATA_MAXMIDITRACKNUM) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* タイトル読み込み */
	if (fread (szTempTitle, sizeof(char), 128, pFile) < 128) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 著作権読み込み */
	if (fread (szTempCopyright, sizeof(char), 128, pFile) < 128) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 拡張データ先頭アドレス配列読み込み */
	if (fread (lTempExtendedDataAddress, sizeof(long), 64, pFile) < 64) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* トラック先頭アドレス配列読み込み */
	if (fread (lTempTrackAddress, sizeof(long), lTempNumTrack, pFile) < (unsigned long)lTempNumTrack) {
		free (pMIDIData);
		fclose (pFile);
		return NULL;
	}

	/* 各トラック読み込み */
	for (lTrackIndex = 0; lTrackIndex < lTempNumTrack; lTrackIndex++) {

		/* トラックの長さ[バイト]取得 */
		if (fread (&lTrackLen, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		if (lTrackLen < 128 || lTrackLen >= 16777216) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}

		/* 1トラック用のメモリ割り当て */
		pTrackData = malloc (lTrackLen - 4);
		if (pTrackData == NULL) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		
		/* 1トラック読み込み */
		if (fread (pTrackData, sizeof (char), lTrackLen - 4, pFile) < (unsigned long)lTrackLen - 4) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}

		/* トラック内のデータ読み込み関数を呼び出し */
		pMIDITrack = MIDITrack_LoadFromCherry (lTrackIndex, pTrackData, lTrackLen);
		if (pMIDITrack == NULL) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		
		/* 新しく作られたトラックを追加 */
		lRet = MIDIData_AddTrackForce (pMIDIData, pMIDITrack);
		if (lRet == 0) {
			free (pTrackData);
			free (pMIDIData);
			fclose (pFile);
		}
	
		free (pTrackData);
		pTrackData = NULL;
	}

	/* 拡張データ */
	pMIDITrack = pMIDIData->m_pFirstTrack;
	assert (pMIDITrack);
	/* 調性拍子マーカーデータ(ある場合のみ) */
           	if (!feof (pFile) && lTempExtendedDataAddress[0] != 0x00000000) {
		long lExtendedLen = 0;
		long lTimeKeySignatureIndex = 0;
		if (fread (&lExtendedLen, sizeof (long), 1, pFile) < 1) {
			free (pMIDIData);
			fclose (pFile);
			return NULL;
		}
		/* 1回につき80バイト */
		for (lTimeKeySignatureIndex = 0; 
			lTimeKeySignatureIndex < lExtendedLen / 80; 
			lTimeKeySignatureIndex++) {
			long lMeasure = 0;
			long lTime = 0;
			long lnn = 0;
			long lddTemp = 0;
			long ldd = 2;
			long lcc = 24;
			long lbb = 8;
			long lsf = 0;
			long lmi = 0;
			long j;
			memset (byBuf, 0, sizeof (byBuf));
			if (fread (byBuf, sizeof (char), 80, pFile) < 80) {
				free (pMIDIData);
				fclose (pFile);
				return NULL;
			}
			lMeasure = (long)(*(unsigned short*)(byBuf));
			lnn = (long)(*(unsigned char*)(byBuf + 2));
			lddTemp = (long)(*(unsigned char*)(byBuf + 3));
			for (j = 0; j < 7; j++) {
				if (lddTemp == (1 << j)) {
					ldd = j;
					break;
				}
			}
 			lsf = (long)(*(unsigned char*)(byBuf + 4));
			if (lsf >= 9) {
				lsf = 8 - lsf;
			}
			byBuf[79] = 0;
			MIDIData_MakeTime (pMIDIData, lMeasure, 0, 0, &lTime);
			/* マーカー挿入 */
			MIDITrack_InsertMarkerA (pMIDITrack, lTime, (char*)(&byBuf[16]));
			/* 調性記号挿入 */
			MIDITrack_InsertKeySignature (pMIDITrack, lTime, lsf, lmi);
			/* 拍子記号挿入 */
			MIDITrack_InsertTimeSignature (pMIDITrack, lTime, lnn, ldd, lcc, lbb);
		}
	}

	fclose (pFile);

	MIDIData_SetTitleA (pMIDIData, szTempTitle);
	MIDIData_SetCopyrightA (pMIDIData, szTempCopyright);
	
	return pMIDIData;
}


/* MIDITrackをCherry形式でメモリブロック上に保存したときの長さを推定(非公開) */
long __stdcall MIDITrack_GuessTrackDataLenAsCherry (MIDITrack* pMIDITrack) {
	MIDIEvent* pMIDIEvent = NULL;
	long lTrackLen = 128;
	assert (pMIDITrack);
	forEachEvent (pMIDITrack, pMIDIEvent) {
		if (MIDIEvent_IsTimeSignature (pMIDIEvent)) {
			lTrackLen += 80;
		}
		else if (MIDIEvent_IsKeySignature (pMIDIEvent)) {
			lTrackLen += 80;
		}
		else if (MIDIEvent_IsMarker (pMIDIEvent)) {
			lTrackLen += 80;
		}
		else if (MIDIEvent_IsMetaEvent (pMIDIEvent)) {
			lTrackLen += (15 + MIDIEvent_GetLen (pMIDIEvent));
		}
		else if (MIDIEvent_IsMIDIEvent (pMIDIEvent)) {
			lTrackLen += 10;
		}
		else if (MIDIEvent_IsSysExEvent (pMIDIEvent)) {
			lTrackLen += (14 + MIDIEvent_GetLen (pMIDIEvent));
		}
	}
	return lTrackLen;
}

/* MIDITrackをCherry形式でメモリブロック上に保存(Cherry上でトラック0)(非公開) */
long __stdcall MIDITrack_SaveAsCherry0 (MIDITrack* pMIDITrack, unsigned char* pTrackData, long lTrackLen) {
	MIDIEvent* pMIDIEvent = NULL;
	unsigned char* p = pTrackData;

	assert (pMIDITrack);
	assert (pTrackData);
	assert (pMIDITrack->m_lTempIndex == 0);
	assert (0 <= lTrackLen && lTrackLen <= 0x7FFFFFFF);

	/* 予備(12byte) */
	memset (p, 0x00, 12);
	p += 12;

	/* 初期化データ(48byte) */
	memset (p, 0xFF, 48);
	*(p + 0) = (unsigned char)(CLIP (0, pMIDITrack->m_lViewMode, 2));
	*(p + 1) = (unsigned char)(CLIP (0, pMIDITrack->m_lOutputPort, 3));
	*(p + 2) = (unsigned char)(CLIP (0, pMIDITrack->m_lOutputChannel, 15));
	*(p + 3) = (unsigned char)(CLIP (-127, pMIDITrack->m_lKeyPlus, 127));
	*(p + 4) = (unsigned char)(CLIP (-127, pMIDITrack->m_lTimePlus, 127));
	*(p + 5) = (unsigned char)(CLIP (-127, pMIDITrack->m_lVelocityPlus, 127));
	if (pMIDITrack->m_lReserved1 != 0x00000000) {
		*(p + 16) = (unsigned char)((pMIDITrack->m_lReserved1 >> 0) & 0xFF);
		*(p + 17) = (unsigned char)((pMIDITrack->m_lReserved1 >> 8) & 0xFF);
		*(p + 18) = (unsigned char)((pMIDITrack->m_lReserved1 >> 16) & 0xFF);
		*(p + 19) = (unsigned char)((pMIDITrack->m_lReserved1 >> 24) & 0xFF);
	}
	p += 48;

	/* トラック名(64byte) */
	memset (p, 0x00, 64);
	strcpy ((char*)p, "Conductor Track");
	p += 64;

	/* テンポイベントのみ書き込み */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		/* テンポ */
		if (MIDIEvent_IsTempo (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usTempoBPM = (
				(unsigned short)CLIP (1, 60000000 / MIDIEvent_GetTempo (pMIDIEvent), 65535));
			*p = 0x82;
			*(p + 1) = 0x9B;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memcpy (p + 8, &usTempoBPM, 2);
			p += 10;
		}
		/* エンドオブトラック */
		else if (MIDIEvent_IsEndofTrack (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			*p = 0xFF;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			p += 10;
		}
	}
	return (p - pTrackData);
}

/* MIDITrackをCherry形式でメモリブロック上に保存(Cherry上でトラック1)(非公開) */
long __stdcall MIDITrack_SaveAsCherry1 (MIDITrack* pMIDITrack, unsigned char* pTrackData, long lTrackLen) {
	MIDIEvent* pMIDIEvent = NULL;
	unsigned char* p = pTrackData;
	unsigned char szTempBuf[256];
	memset (szTempBuf, 0, sizeof (szTempBuf));

	assert (pMIDITrack);
	assert (pTrackData);
	assert (pMIDITrack->m_lTempIndex == 0 || pMIDITrack->m_lTempIndex == 1);
	assert (0 <= lTrackLen && lTrackLen <= 0x7FFFFFFF);

	/* 予備(12byte) */
	memset (p, 0x00, 12);
	p += 12;
		
	/* 初期化データ(48byte) */
	memset (p, 0xFF, 48);
	*(p + 0) = (unsigned char)(CLIP (0, pMIDITrack->m_lViewMode, 2));
	*(p + 1) = (unsigned char)(CLIP (0, pMIDITrack->m_lOutputPort, 3));
	*(p + 2) = (unsigned char)(CLIP (0, pMIDITrack->m_lOutputChannel, 15));
	*(p + 3) = (unsigned char)(CLIP (-127, pMIDITrack->m_lKeyPlus, 127));
	*(p + 4) = (unsigned char)(CLIP (-127, pMIDITrack->m_lTimePlus, 127));
	*(p + 5) = (unsigned char)(CLIP (-127, pMIDITrack->m_lVelocityPlus, 127));
	if (pMIDITrack->m_lReserved1 != 0x00000000) {
		*(p + 16) = (unsigned char)((pMIDITrack->m_lReserved1 >> 0) & 0xFF);
		*(p + 17) = (unsigned char)((pMIDITrack->m_lReserved1 >> 8) & 0xFF);
		*(p + 18) = (unsigned char)((pMIDITrack->m_lReserved1 >> 16) & 0xFF);
		*(p + 19) = (unsigned char)((pMIDITrack->m_lReserved1 >> 24) & 0xFF);
	}
	p += 48;

	/* トラック名(64byte) */
	memset (p, 0x00, 64);
	strcpy ((char*)p, "System Setup");
	p += 64;

	/* Sysx・テキスト・歌詞イベントのみ書き込み */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		/* システムエクスクルーシヴ */
		if (MIDIEvent_IsSysExEvent (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			long lLen = 0;
			memset (szTempBuf, 0, sizeof (szTempBuf));
			lLen = MIDIEvent_GetData (pMIDIEvent, szTempBuf, sizeof (szTempBuf));
			*p = 0xC0;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			memcpy (p + 10, &lLen, 4);
			memcpy (p + 14, szTempBuf, lLen);
			p += (14 + lLen);
		}
		/* テキスト */
		else if (MIDIEvent_IsTextEvent (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			long lLen = 0;
			memset (szTempBuf, 0, sizeof (szTempBuf));
			lLen = MIDIEvent_GetData (pMIDIEvent, szTempBuf, sizeof (szTempBuf) - 1);
			lLen += 1;
			*p = 0xC1;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			memcpy (p + 10, &lLen, 4);
			memcpy (p + 14, szTempBuf, lLen);
			p += (14 + lLen);
		}
		/* 歌詞 */
		else if (MIDIEvent_IsLyric (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			long lLen = 0;
			memset (szTempBuf, 0, sizeof (szTempBuf));
			lLen = MIDIEvent_GetData (pMIDIEvent, szTempBuf, sizeof (szTempBuf) - 1);
			lLen += 1;
			*p = 0xC2;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			memcpy (p + 10, &lLen, 4);
			memcpy (p + 14, szTempBuf, lLen);
			p += (14 + lLen);
		}
		/* エンドオブトラック */
		else if (MIDIEvent_IsEndofTrack (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			*p = 0xFF;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			p += 10;
		}
	}
	return (p - pTrackData);
}

/* MIDITrackをCherry形式でメモリブロック上に保存(Cherry上でトラック1以降)(非公開) */
long __stdcall MIDITrack_SaveAsCherry2 (MIDITrack* pMIDITrack, unsigned char* pTrackData, long lTrackLen) {
	MIDIEvent* pMIDIEvent = NULL;
	unsigned char* p = pTrackData;
	unsigned char szTempBuf[256];
	memset (szTempBuf, 0, sizeof (szTempBuf));
	assert (pMIDITrack);
	assert (pTrackData);
	assert (pMIDITrack->m_lTempIndex >= 1);
	assert (0 <= lTrackLen && lTrackLen <= 0x7FFFFFFF);
		
	/* 予約領域(12byte) */
	memset (p, 0x00, 12);
	p += 12;
	
	/* 初期化データ(48byte) */
	memset (p, 0xFF, 48);
	*(p + 0) = (unsigned char)(CLIP (0, pMIDITrack->m_lViewMode, 2));
	*(p + 1) = (unsigned char)(CLIP (0, pMIDITrack->m_lOutputPort, 3));
	*(p + 2) = (unsigned char)(CLIP (0, pMIDITrack->m_lOutputChannel, 15));
	*(p + 3)= (unsigned char)(CLIP (-127, pMIDITrack->m_lKeyPlus, 127));
	*(p + 4) = (unsigned char)(CLIP (-127, pMIDITrack->m_lTimePlus, 127));
	*(p + 5)= (unsigned char)(CLIP (-127, pMIDITrack->m_lVelocityPlus, 127));
	if (pMIDITrack->m_lReserved1 != 0x00000000) {
		*(p + 16) = (unsigned char)((pMIDITrack->m_lReserved1 >> 0) & 0xFF);
		*(p + 17) = (unsigned char)((pMIDITrack->m_lReserved1 >> 8) & 0xFF);
		*(p + 18) = (unsigned char)((pMIDITrack->m_lReserved1 >> 16) & 0xFF);
		*(p + 19) = (unsigned char)((pMIDITrack->m_lReserved1 >> 24) & 0xFF);
	}
	p += 48;

	/* トラック名(64byte) */
	memset (p, 0x00, 64);
	MIDITrack_GetNameA (pMIDITrack, (char*)p, 63);
	p += 64;

	/* ノートオン・CC#・PC#・Sysx・テキスト・歌詞イベントのみ書き込み */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		/* ノートオン(ノートオフに結合しているもののみ) */
		if (MIDIEvent_IsNoteOn (pMIDIEvent) && pMIDIEvent->m_pNextCombinedEvent) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned char ucKey = (unsigned char)(CLIP (0, MIDIEvent_GetKey (pMIDIEvent), 127));
			unsigned short usDur = (unsigned short)(CLIP (0, MIDIEvent_GetDuration (pMIDIEvent), 65535));
			unsigned short usVel = (unsigned short)(CLIP (0, MIDIEvent_GetVelocity (pMIDIEvent), 127));
			*p = ucKey;
			*(p + 1) = 0;
			memcpy (p + 2, &lTime, 4);
			memcpy (p + 6, &usDur, 2);
			memcpy (p + 8, &usVel, 2);
			p += 10;
		}
		/* パッチチェンジ(CC#0+CC#32+PC#) */
		else if (MIDIEvent_IsPatchChange (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usBankMSB = (unsigned short)MIDIEvent_GetBankMSB (pMIDIEvent);
			unsigned short usBankLSB = (unsigned short)MIDIEvent_GetBankLSB (pMIDIEvent);
			unsigned short usBank16 = (usBankMSB << 8) | usBankLSB;
			unsigned short usValue = (unsigned short)MIDIEvent_GetValue 
				(pMIDIEvent->m_pNextCombinedEvent->m_pNextCombinedEvent);
			*p = 0x83;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memcpy (p + 6, &usBank16, 2);
			memcpy (p + 8, &usValue, 2);
			p += 10;
			pMIDIEvent = MIDIEvent_GetLastCombinedEvent (pMIDIEvent);
		}
		/* RPNチェンジ(CC#101+CC#100+CC#6) */
		else if (MIDIEvent_IsRPNChange (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usBankMSB = (unsigned short)MIDIEvent_GetBankMSB (pMIDIEvent);
			unsigned short usBankLSB = (unsigned short)MIDIEvent_GetBankLSB (pMIDIEvent);
			unsigned short usDummy = 0x0000;
			unsigned short usValue = (unsigned short)MIDIEvent_GetValue 
				(pMIDIEvent->m_pNextCombinedEvent->m_pNextCombinedEvent);
			/* ピッチベンドセンシティビティ */
			/* コースチューン */
			if (usBankMSB == 0 && (usBankLSB == 0 || usBankLSB == 2)) {
				*p = 0x82;
				*(p + 1) = (unsigned char)(0x80 + usBankLSB);
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usValue, 2);
				p += 10;
			}
			/* ファインチューン(TODO:値は保留:0x0000-0x4000-0x8000) */
			else if (usBankMSB == 0 && usBankLSB == 1) {
				usValue = usValue << 8;
				*p = 0x82;
				*(p + 1) = (unsigned char)(0x80 + usBankLSB);
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usValue, 2);
				p += 10;
			}
			/* その他のRPNチェンジ */
			else {
				*p = 0x82;
				*(p + 1) = 101;
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usBankMSB, 2);
				p += 10;
				*p = 0x82;
				*(p + 1) = 101;
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usBankLSB, 2);
				p += 10;
				*p = 0x82;
				*(p + 1) = 6;
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usValue, 2);
				p += 10;
			}
			pMIDIEvent = MIDIEvent_GetLastCombinedEvent (pMIDIEvent);
		}
		/* NRPNチェンジ(CC#99+CC#98+CC#6) */
		else if (MIDIEvent_IsNRPNChange (pMIDIEvent)) {
			long j = 0;
			long lFound = 0;
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usBankMSB = (unsigned short)MIDIEvent_GetBankMSB (pMIDIEvent);
			unsigned short usBankLSB = (unsigned short)MIDIEvent_GetBankLSB (pMIDIEvent);
			unsigned short usDummy = 0x0000;
			unsigned short usValue = (unsigned short)MIDIEvent_GetValue
				(pMIDIEvent->m_pNextCombinedEvent->m_pNextCombinedEvent);
			/* V.Rate / V.Dep / V.Del / HPF.Cutoff / E.G.Delay */
			if (lFound == 0) {
				unsigned char ucBankLSB8C[5] = {8, 9, 10, 36, 100};
				for (j = 0; j < 5; j++) {
					if (usBankMSB == 1 && usBankLSB == ucBankLSB8C[j]) {
						*p = 0x82;
						*(p + 1) = (unsigned char)(0x8C + j); /* 0x8C～0x90 */
						memcpy (p + 2, &lTime, 4);
						memcpy (p + 6, &usDummy, 2);
						memcpy (p + 8, &usValue, 2);
						p += 10;
						lFound++;
						break;
					}
				}
			}
			/* EQ */
			if (lFound == 0) {
				unsigned char ucBankLSBAF[4] = {48, 49, 52, 53};
				for (j = 0; j < 4; j++) {
					if (usBankMSB == 1 && usBankLSB == ucBankLSBAF[j]) {
						*p = 0x82;
						*(p + 1) = (unsigned char)(0xAF + j); /* 0xAF～0xB2*/
						memcpy (p + 2, &lTime, 4);
						memcpy (p + 6, &usDummy, 2);
						memcpy (p + 8, &usValue, 2);
						p += 10;
						lFound++;
						break;
					}
				}
			}
			/* Drum params */
			if (lFound == 0) {
				unsigned char ucBankMSBC8[16] = {20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 36, 52, 48, 53, 49};
				for (j = 0; j < 16; j++) {
					if (usBankMSB == ucBankMSBC8[j]) {
						*p = 0x82;
						*(p + 1) = (unsigned char)(0xC8 + j); /* 0xC8～0xD7 */
						memcpy (p + 2, &lTime, 4);
						memcpy (p + 6, &usBankLSB, 2); /* Drum.Key */
						memcpy (p + 8, &usValue, 2);
						p += 10;
						lFound++;
						break;
					}
				}
			}
			/* その他のNRPNチェンジ */
			if (lFound == 0) {
				*p = 0x82;
				*(p + 1) = 99;
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usBankMSB, 2);
				p += 10;
				*p = 0x82;
				*(p + 1) = 98;
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usBankLSB, 2);
				p += 10;
				*p = 0x82;
				*(p + 1) = 6;
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDummy, 2);
				memcpy (p + 8, &usValue, 2);
				p += 10;
			}
			pMIDIEvent = MIDIEvent_GetLastCombinedEvent (pMIDIEvent);
		}
		/* キーアフタータッチ */
		else if (MIDIEvent_IsKeyAftertouch (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usDummy = 0x0000;
			unsigned short usValue = (unsigned short)MIDIEvent_GetValue (pMIDIEvent);
			*p = 0x82;
			*(p + 1) = 0x97;
			memcpy (p + 2, &lTime, 4);
			memcpy (p + 6, &usDummy, 2);
			memcpy (p + 8, &usValue, 2);
			p += 10;
		}
		/* ピッチベンド */
		else if (MIDIEvent_IsPitchBend (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usDummy = 0x0000;
			unsigned short usValue = (unsigned short)MIDIEvent_GetValue (pMIDIEvent);
			*p = 0x82;
			*(p + 1) = 0x99;
			memcpy (p + 2, &lTime, 4);
			memcpy (p + 6, &usDummy, 2);
			memcpy (p + 8, &usValue, 2);
			p += 10;
		}
		/* コントロールチェンジ */
		else if (MIDIEvent_IsControlChange (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned char ucNum = (unsigned char)(CLIP (0, MIDIEvent_GetNumber (pMIDIEvent), 127));
			unsigned short usVal = (unsigned short)(CLIP (0, MIDIEvent_GetValue (pMIDIEvent), 127));
			*p = 0x82;
			*(p + 1) = ucNum;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memcpy (p + 8, &usVal, 2);
			p += 10;
		}
		/* プログラムチェンジ */
		else if (MIDIEvent_IsProgramChange (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usVal = (unsigned short)(CLIP (0, MIDIEvent_GetValue (pMIDIEvent), 127));
			*p = 0x83;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0xFF, 2);
			memcpy (p + 8, &usVal, 2);
			p += 10;
		}
		/* システムエクスクルーシヴ */
		else if (pMIDIEvent->m_lKind == MIDIEVENT_SYSEXSTART) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			long lLen = MIDIEvent_GetLen (pMIDIEvent);
			unsigned char bySysx[256];
			memset (bySysx, 0, sizeof (bySysx));
			MIDIEvent_GetData (pMIDIEvent, bySysx, MIN (lLen, sizeof (bySysx)));
			/* マスターボリューム */
			if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x7F &&
				bySysx[2] == 0x7F &&
				bySysx[3] == 0x04 &&
				bySysx[4] == 0x01 &&
				bySysx[5] == 0x00 &&
				bySysx[7] == 0xF7) {
				unsigned short usVal = bySysx[6];
				*p = 0x82;
				*(p + 1) = 0x83;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memcpy (p + 8, &usVal, 2);
				p += 10;
			}
			/* PEG InitLev / PEG AttackTime / PEG RelLev / PEG RelTime */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x43 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x4C &&
				bySysx[4] == 0x08 &&
				bySysx[5] == 0x01 &&
				0x69 <= bySysx[6] && bySysx[6] <= 0x6C &&
				bySysx[8] == 0xF7) {
				unsigned char ucNum = 0x28 + bySysx[6]; /* 0x91～0x94 */
				unsigned short usVal = bySysx[7];
				*p = 0x82;
				*(p + 1) = ucNum;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memcpy (p + 8, &usVal, 2);
				p += 10;
			}
			/* GM System On */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x7E &&
				bySysx[2] == 0x7F &&
				bySysx[3] == 0x09 &&
				bySysx[4] == 0x01 &&
				bySysx[5] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xA0;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				p += 10;
			}
			/* XG System On */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x43 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x4C &&
				bySysx[4] == 0x00 &&
				bySysx[5] == 0x00 &&
				bySysx[6] == 0x7E &&
				bySysx[7] == 0x00 &&
				bySysx[8] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xA1;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				p += 10;
			}
			/* GS Reset */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x41 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x42 &&
				bySysx[4] == 0x12 &&
				bySysx[5] == 0x40 &&
				bySysx[6] == 0x00 &&
				bySysx[7] == 0x7F &&
				bySysx[8] == 0x00 &&
				bySysx[9] == 0x41 &&
				bySysx[10] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xA1;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				p += 10;
			}
			/* 88 Reset */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x41 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x42 &&
				bySysx[4] == 0x12 &&
				bySysx[5] == 0x00 &&
				bySysx[6] == 0x00 &&
				bySysx[7] == 0x7F &&
				0x00 <= bySysx[8] && bySysx[8] <= 0x01 &&
				bySysx[9] == ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F) &&
				bySysx[10] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xA2;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				*(p + 8) = bySysx[8];
				p += 10;
			}
			/* Dry Level */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x43 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x4C &&
				bySysx[4] == 0x08 &&
				bySysx[5] == 0x03 &&
				bySysx[6] == 0x11 &&
				bySysx[8] == 0xF7) {
				unsigned short usVal = bySysx[7];
				*p = 0x82;
				*(p + 1) = 0xA5;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memcpy (p + 8, &usVal, 2);
				p += 10;
			}
			/* MW params */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x43 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x4C &&
				bySysx[4] == 0x08 &&
				bySysx[5] == 0x02 &&
				0x1D <= bySysx[6] && bySysx[6] <= 0x22 &&
				bySysx[8] == 0xF7) {
				unsigned char ucNum = bySysx[6] - 0x1D + 0xA6; /* 0xA6～0xAB */
				unsigned short usVal = bySysx[7];
				*p = 0x82;
				*(p + 1) = ucNum;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memcpy (p + 8, &usVal, 2);
				p += 10;
			}
			/* GS: Reverb / Chorus / Delay Macro  */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x41 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x42 &&
				bySysx[4] == 0x12 &&
				bySysx[5] == 0x40 &&
				bySysx[6] == 0x01 &&
				(bySysx[7] == 0x30 || bySysx[7] == 0x38 || bySysx[7] == 0x50) &&
				0x00 <= bySysx[8] && bySysx[8] <= 0x07 &&
				bySysx[9] == ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F) &&
				bySysx[10] == 0xF7) {
				*p = 0x82;
				*(p + 1) = bySysx[7] == 0x30 ? 0xAC : (bySysx[7] == 0x38 ? 0xAD : 0xAE);
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				*(p + 8) = bySysx[8];
				p += 10;
			}
			/* GS: EQ */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x41 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x42 &&
				bySysx[4] == 0x12 &&
				bySysx[5] == 0x40 &&
				bySysx[6] == 0x02 &&
				0x00 <= bySysx[7] && bySysx[7] <= 0x03 &&
				0x00 <= bySysx[8] && bySysx[8] <= 0x7F &&
				bySysx[9] == ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F) &&
				bySysx[10] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xAF + bySysx[7];
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				*(p + 8) = bySysx[8];
				p += 10;
			}
			/* DX:AssignPart */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x43 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x4C &&
				bySysx[4] == 0x70 &&
				bySysx[5] == 0x02 &&
				bySysx[6] == 0x00 &&
				bySysx[7] == 0x02 &&
				bySysx[8] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xB3;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				p += 10;
			}
			/* GS:Assign Mode */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x41 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x42 &&
				bySysx[4] == 0x12 &&
				bySysx[5] == 0x40 &&
				bySysx[6] == 0x17 &&
				bySysx[7] == 0x14 &&
				0x00 <= bySysx[8] && bySysx[8] <= 0x7F &&
				bySysx[9] == ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F) &&
				bySysx[10] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xB3;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				*(p + 8) = bySysx[8];
				p += 10;
			}
			/* GS: Use For Norm/Drum1/Drum2 */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x41 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x42 &&
				bySysx[4] == 0x12 &&
				bySysx[5] == 0x40 &&
				bySysx[6] == 0x17 &&
				bySysx[7] == 0x15 &&
				0x00 <= bySysx[8] && bySysx[8] <= 0x02 &&
				bySysx[9] == ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F) &&
				bySysx[10] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xB4;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				*(p + 8) = bySysx[8];
				p += 10;
			}
			/* GS: EQ SW Off/On */
			else if (bySysx[0] == 0xF0 &&
				bySysx[1] == 0x41 &&
				bySysx[2] == 0x10 &&
				bySysx[3] == 0x42 &&
				bySysx[4] == 0x12 &&
				bySysx[5] == 0x40 &&
				bySysx[6] == 0x47 &&
				bySysx[7] == 0x20 &&
				0x00 <= bySysx[8] && bySysx[8] <= 0x01 &&
				bySysx[9] == ((128 - (Sum (&bySysx[5], 4) % 128)) & 0x7F) &&
				bySysx[10] == 0xF7) {
				*p = 0x82;
				*(p + 1) = 0xB5;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				*(p + 8) = bySysx[8];
				p += 10;
			}
			/* その他のシステムエクスクルーシヴ */
			else {
				*p = 0xC0;
				*(p + 1) = 0x00;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memset (p + 8, 0x00, 2);
				memcpy (p + 10, &lLen, 4);
				memcpy (p + 14, bySysx, lLen);
				p += (14 + lLen);
			}
		}
		/* テキスト */
		else if (pMIDIEvent->m_lKind == MIDIEVENT_TEXTEVENT) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			long lLen = 0;
			memset (szTempBuf, 0, sizeof (szTempBuf));
			lLen = MIDIEvent_GetData (pMIDIEvent, szTempBuf, sizeof (szTempBuf) - 1);
			lLen += 1;
			*p = 0xC1;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			memcpy (p + 10, &lLen, 4);
			memcpy (p + 14, szTempBuf, lLen);
			p += (14 + lLen);
		}
		/* 歌詞 */
		else if (pMIDIEvent->m_lKind == MIDIEVENT_LYRIC) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			long lLen = 0;
			memset (szTempBuf, 0, sizeof (szTempBuf));
			lLen = MIDIEvent_GetData (pMIDIEvent, szTempBuf, sizeof (szTempBuf) - 1);
			lLen += 1;
			*p = 0xC2;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			memcpy (p + 10, &lLen, 4);
			memcpy (p + 14, szTempBuf, lLen);
			p += (14 + lLen);
		}
		/* エンドオブトラック */
		else if (MIDIEvent_IsEndofTrack (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			*p = 0xFF;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			p += 10;
		}
	}
	return (p - pTrackData);
}

/* MIDITrackをCherry形式でメモリブロック上に保存(拍子調性)(非公開) */
long __stdcall MIDITrack_SaveAsCherry3 (MIDITrack* pMIDITrack, unsigned char* pTrackData, long lTrackLen) {
	MIDIEvent* pMIDIEvent = NULL;
	unsigned char* p = pTrackData;
	long lOldTime = -1;
	assert (pMIDITrack);
	assert (pMIDITrack->m_lTempIndex == 0);
	assert (pTrackData);
	assert (0 <= lTrackLen && lTrackLen <= 0x7FFFFFFF);

	/* 拍子記号・調性記号イベントのみ書き込み */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		unsigned char byBuf[64];
		strcpy ((char*)byBuf, "Setup");
		/* マーカー */
		if (MIDIEvent_IsMarker (pMIDIEvent)) {
			long lsf = 0;
			long lmi = 0;
			long lMeasure, lBeat, lTick;
			long lnn, ldd, lcc, lbb;
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usMeasure = 0;
			unsigned char ucnn = 4;
			unsigned char ucdd = 2;
			unsigned char ucsf = 0;
			unsigned char ucmi = 0;
			memset (byBuf, 0, sizeof (byBuf));
			MIDIEvent_GetData (pMIDIEvent, byBuf, sizeof (byBuf));
			MIDITrack_BreakTime (pMIDITrack, lTime, &lMeasure, &lBeat, &lTick);
			MIDITrack_MakeTime (pMIDITrack, lMeasure, 0, 0, &lTime);
			MIDITrack_FindKeySignature (pMIDITrack, lTime, &lsf, &lmi); 
			MIDITrack_FindTimeSignature (pMIDITrack, lTime, &lnn, &ldd, &lcc, &lbb);
			usMeasure = (unsigned short)(CLIP (0, lMeasure, 65535));
			ucnn = (unsigned char)(CLIP (0, lnn, 255));
			ucdd = (unsigned char)(CLIP (0, 1 << ldd, 255));
			ucsf = (unsigned char)(0 <= lsf && lsf <= 8 ? lsf : 8 - lsf);
			ucmi = (unsigned char)(CLIP (0, lmi, 1));
			if (lOldTime == lTime) {
				p -= 80;
			}
			memset (p, 0, 80);
			memcpy (p, &usMeasure, 2);
			*(p + 2) = ucnn;
			*(p + 3) = ucdd;
			*(p + 4) = ucsf;
			*(p + 5) = ucmi;
			strncpy ((char*)(p + 16), (char*)byBuf, 63);
			p += 80;
			lOldTime = lTime;
		}
		/* 拍子記号 */
		else if (MIDIEvent_IsTimeSignature (pMIDIEvent)) {
			long lsf = 0;
			long lmi = 0;
			long lMeasure, lBeat, lTick;
			long lnn, ldd, lcc, lbb;
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usMeasure = 0;
			unsigned char ucnn = 4;
			unsigned char ucdd = 2;
			unsigned char ucsf = 0;
			unsigned char ucmi = 0;
			MIDITrack_BreakTime (pMIDITrack, lTime, &lMeasure, &lBeat, &lTick);
			MIDITrack_MakeTime (pMIDITrack, lMeasure, 0, 0, &lTime);
			MIDITrack_FindKeySignature (pMIDITrack, lTime, &lsf, &lmi); 
			MIDIEvent_GetTimeSignature (pMIDIEvent, &lnn, &ldd, &lcc, &lbb);
			usMeasure = (unsigned short)(CLIP (0, lMeasure, 65535));
			ucnn = (unsigned char)(CLIP (0, lnn, 255));
			ucdd = (unsigned char)(CLIP (0, 1 << ldd, 255));
			ucsf = (unsigned char)(0 <= lsf && lsf <= 8 ? lsf : 8 - lsf);
			ucmi = (unsigned char)(CLIP (0, lmi, 1));
			if (lOldTime == lTime) {
				p -= 80;
			}
			memset (p, 0, 80);
			memcpy (p, &usMeasure, 2);
			*(p + 2) = ucnn;
			*(p + 3) = ucdd;
			*(p + 4) = ucsf;
			*(p + 5) = ucmi;
			strncpy ((char*)(p + 16), (char*)byBuf, 63);
			p += 80;
			lOldTime = lTime;
		}
		/* 調性記号 */
		else if (MIDIEvent_IsKeySignature (pMIDIEvent)) {
			long lsf = 0;
			long lmi = 0;
			long lMeasure, lBeat, lTick;
			long lnn, ldd, lcc, lbb;
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			unsigned short usMeasure = 0;
			unsigned char ucnn = 4;
			unsigned char ucdd = 2;
			unsigned char ucsf = 0;
			unsigned char ucmi = 0;
			MIDITrack_BreakTime (pMIDITrack, lTime, &lMeasure, &lBeat, &lTick);
			MIDITrack_MakeTime (pMIDITrack, lMeasure, 0, 0, &lTime);
			MIDIEvent_GetKeySignature (pMIDIEvent, &lsf, &lmi); 
			MIDITrack_FindTimeSignature (pMIDITrack, lTime, &lnn, &ldd, &lcc, &lbb);
			usMeasure = (unsigned short)(CLIP (0, lMeasure, 65535));
			ucnn = (unsigned char)(CLIP (0, lnn, 255));
			ucdd = (unsigned char)(CLIP (0, 1 << ldd, 255));
			ucsf = (unsigned char)(0 <= lsf && lsf <= 8 ? lsf : 8 - lsf);
			ucmi = (unsigned char)(CLIP (0, lmi, 1));
			if (lOldTime == lTime) {
				p -= 80;
			}
			memset (p, 0, 80);
			memcpy (p, &usMeasure, 2);
			*(p + 2) = ucnn;
			*(p + 3) = ucdd;
			*(p + 4) = ucsf;
			*(p + 5) = ucmi;
			strncpy ((char*)(p + 16), (char*)byBuf, 63);
			p += 80;
			lOldTime = lTime;
		}
	}
	return (p - pTrackData);
}




/* MIDITrackをCherry形式でメモリブロック上に保存(指定MIDIチャンネルイベントのみ)(非公開) */
long __stdcall MIDITrack_SaveAsCherryChannel 
	(MIDITrack* pMIDITrack, long lChannel, unsigned char* pTrackData, long lTrackLen) {
	MIDIEvent* pMIDIEvent = NULL;
	unsigned char* p = pTrackData;
	unsigned char szTempBuf[256];
	memset (szTempBuf, 0, sizeof (szTempBuf));

	assert (pMIDITrack);
	assert (pTrackData);
	assert (pMIDITrack->m_lTempIndex == 0);
	assert (0 <= lChannel && lChannel <= 15);
		
	/* 予約領域(12byte) */
	memset (p, 0x00, 12);
	p += 12;
	
	/* 初期化データ(48byte) */
	memset (p, 0xFF, 48);
	*(p + 0) = (unsigned char)(CLIP (0, pMIDITrack->m_lViewMode, 2));
	*(p + 1) = (unsigned char)(CLIP (0, pMIDITrack->m_lOutputPort, 3));
	*(p + 2) = (unsigned char)(CLIP (0, lChannel, 15));
	*(p + 3) = (unsigned char)(CLIP (-127, pMIDITrack->m_lKeyPlus, 127));
	*(p + 4) = (unsigned char)(CLIP (-127, pMIDITrack->m_lTimePlus, 127));
	*(p + 5) = (unsigned char)(CLIP (-127, pMIDITrack->m_lVelocityPlus, 127));
	if (pMIDITrack->m_lReserved1 != 0x00000000) {
		*(p + 16) = (unsigned char)((pMIDITrack->m_lReserved1 >> 0) & 0xFF);
		*(p + 17) = (unsigned char)((pMIDITrack->m_lReserved1 >> 8) & 0xFF);
		*(p + 18) = (unsigned char)((pMIDITrack->m_lReserved1 >> 16) & 0xFF);
		*(p + 19) = (unsigned char)((pMIDITrack->m_lReserved1 >> 24) & 0xFF);
	}
	p += 48;

	/* トラック名(64byte) */
	memset (p, 0x00, 64);
	MIDITrack_GetNameA (pMIDITrack, (char*)p, 63);
	p += 64;

	/* ノートオン・CC#・PC#・Sysx・テキスト・歌詞イベントのみ書き込み */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		/* ノートオン(ノートオフに結合しているもののみ) */
		if (MIDIEvent_IsNoteOn (pMIDIEvent) && pMIDIEvent->m_pNextCombinedEvent) {
			if (MIDIEvent_GetChannel (pMIDIEvent) == lChannel) {
				long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
				unsigned char ucKey = (unsigned char)(CLIP (0, MIDIEvent_GetKey (pMIDIEvent), 127));
				unsigned short usDur = (unsigned short)(CLIP (0, MIDIEvent_GetDuration (pMIDIEvent), 65535));
				unsigned short usVel = (unsigned short)(CLIP (0, MIDIEvent_GetVelocity (pMIDIEvent), 127));
				*p = ucKey;
				*(p + 1) = 0;
				memcpy (p + 2, &lTime, 4);
				memcpy (p + 6, &usDur, 2);
				memcpy (p + 8, &usVel, 2);
				p += 10;
			}
		}
		/* コントロールチェンジ */
		else if (MIDIEvent_IsControlChange (pMIDIEvent)) {
			if (MIDIEvent_GetChannel (pMIDIEvent) == lChannel) {
				long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
				unsigned char ucNum = (unsigned char)(CLIP (0, MIDIEvent_GetNumber (pMIDIEvent), 127));
				unsigned short usVal = (unsigned short)(CLIP (0, MIDIEvent_GetValue (pMIDIEvent), 127));
				*p = 0x82;
				*(p + 1) = ucNum;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0x00, 2);
				memcpy (p + 8, &usVal, 2);
				p += 10;
			}
		}
		/* プログラムチェンジ */
		else if (MIDIEvent_IsProgramChange (pMIDIEvent)) {
			if (MIDIEvent_GetChannel (pMIDIEvent) == lChannel) {
				long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
				unsigned short usVal = (unsigned short)(CLIP (0, MIDIEvent_GetValue (pMIDIEvent), 127));
				*p = 0x83;
				*(p + 1) = 0x00;
				memcpy (p + 2, &lTime, 4);
				memset (p + 6, 0xFF, 2);
				memcpy (p + 8, &usVal, 2);
				p += 10;
			}
		}
		/* エンドオブトラック */
		else if (MIDIEvent_IsEndofTrack (pMIDIEvent)) {
			long lTime = CLIP (0, MIDIEvent_GetTime (pMIDIEvent), 0x7FFFFFFF);
			*p = 0xFF;
			*(p + 1) = 0x00;
			memcpy (p + 2, &lTime, 4);
			memset (p + 6, 0x00, 2);
			memset (p + 8, 0x00, 2);
			p += 10;
		}
	}
	return (p - pTrackData);
}

/* MIDIトラックがCherryで言うところのシステムセットアップトラックかどうか調べる */
long __stdcall MIDITrack_IsSystemSetupTrack (MIDITrack* pMIDITrack) {
	/* システムセットアップトラックは通常2番目のトラックである。*/
	/* システムセットアップトラックのトラック名は通常"System Setup"である。 */
	/* システムセットアップトラックは基本的にはSysEx専用トラックであるが、*/
	/* 通常のMIDIイベントやメタイベントを設置することも許容されている。 */
	char szText[256];
	MIDIEvent* pMIDIEvent = NULL;
	memset (szText, 0, sizeof (szText));
	/* まず第一にトラック名が"System Setup"だったら、 */
	/* トラックのないように関わらずシステムセットアップトラックとみなす */
	MIDITrack_GetNameA (pMIDITrack, szText, sizeof (szText));
	if (strncmp (szText, "System Setup", 12) == 0) {
		return 1;
	}
	/* そうでない場合でも、SysEx専用トラックであれば、システムセットアップトラックとみなす。 */
	forEachEvent (pMIDITrack, pMIDIEvent) {
		long lKind = MIDIEvent_GetKind (pMIDIEvent);
		/* MIDIチャンネルイベントの禁止 */
		if (0x80 <= lKind && lKind <= 0xEF) {
			return 0;
		}
		/* テンポ・SMPTEオフセット・拍子記号・調性記号の禁止 */
		if (0x50 <= lKind && lKind <= 0x5F) {
			return 0;
		}
		/* シーケンス番号・著作権・マーカー・キューポイントの禁止 */
		if (lKind == MIDIEVENT_SEQUENCENUMBER || lKind == MIDIEVENT_COPYRIGHTNOTICE ||
			lKind == MIDIEVENT_MARKER || lKind == MIDIEVENT_CUEPOINT) {
			return 0;
		}
	}
	return 1;
}

/* MIDIデータをCherryシーケンスファイル(*.chy)に保存(ANSI) */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsCherryA (MIDIData* pMIDIData, const char* pszFileName) {

	MIDITrack* pMIDITrack = NULL;
	FILE* pFile = NULL;
	long lGuessTrackLen = 0;
	long lTrackLen = 0;
	long lTrackLen4 = 0;
	long lTrackIndex = 0;
	unsigned short usFormat = (unsigned short)MIDIData_GetFormat (pMIDIData);
	unsigned short usTimeMode = (unsigned short)MIDIData_GetTimeMode (pMIDIData);
	unsigned short usTimeResolution = (unsigned short)MIDIData_GetTimeResolution (pMIDIData);
	unsigned short usNumTrack = (usFormat == 0 ? 18 : (unsigned short)MIDIData_CountTrack (pMIDIData));
	unsigned char* pTrackData = NULL;
	unsigned char byBuf[256];
	char szTempTitle[256];
	char szTempCopyright[256];
	long lHasSystemSetupTrack = 0;
	long lTempExtendedDataAddress[64];
	long lTempTrackAddress[65536];
	memset (szTempTitle, 0, sizeof (szTempTitle));
	memset (szTempCopyright, 0, sizeof (szTempCopyright));
	memset (lTempExtendedDataAddress, 0, sizeof (long) * 64);
	memset (lTempTrackAddress, 0, sizeof (long) * 65536);

	/* 最終的な保存トラック数の計算 */
	if (usFormat == 0) {
		usNumTrack = 18;
	}
	else {
		usNumTrack = (unsigned short)CLIP (0, MIDIData_CountTrack (pMIDIData), MIDIDATA_MAXMIDITRACKNUM);
		if (usNumTrack >= 2) {
			MIDITrack* pSecondTrack = pMIDIData->m_pFirstTrack->m_pNextTrack;
			/* システムセットアップ専用トラックがない場合、 */
			/* システムセットアップ専用トラックを追加保存するので+1 */
			if (MIDITrack_IsSystemSetupTrack (pSecondTrack) == 0) {
				usNumTrack++;
			}
			/* システムセットアップ専用トラックがある場合 */
			else {
				lHasSystemSetupTrack = 1;
			}
		}
		/* システムセットアップ専用トラックがない場合、 */
		/* システムセットアップ専用トラックを追加保存するので+1 */
		else {
			usNumTrack++;
		}
	}

	/* ファイルオープン */
	pFile = fopen (pszFileName, "wb");
	if (pFile == NULL) {
		return 0;
	}

	/* ヘッダー(256byte) */
	memset (&byBuf[0], 0, sizeof (byBuf));
	strcpy ((char*)(&byBuf[0]), "CHRY0101");
	memcpy (&byBuf[8], &usTimeResolution, 2);
	memcpy (&byBuf[10], &usNumTrack, 2);
	if (fwrite (byBuf, 1, 256, pFile) < 256) {
		fclose (pFile);
		return 0;
	}
	/* 曲名と著作権(128+128=256byte) */
	memset (&byBuf[0], 0, sizeof (byBuf));
	MIDIData_GetTitleA (pMIDIData, (char*)(&byBuf[0]), 127);
	MIDIData_GetCopyrightA (pMIDIData, (char*)(&byBuf[128]), 127);
	if (fwrite (byBuf, 1, 256, pFile) < 256) {
		fclose (pFile);
		return 0;
	}
	/* 拡張データの先頭アドレス(4*64=256byte) */
	memset (lTempExtendedDataAddress, 0, sizeof (long) * 64);
	if (fwrite (lTempExtendedDataAddress, sizeof (long), 64, pFile) < 64) {
		fclose (pFile);
		return 0;
	}
	/* トラックの先頭アドレス(4*usNumTrack byte) */
	memset (lTempTrackAddress, 0, sizeof (long) * 65536);
	if (fwrite (lTempTrackAddress, sizeof (long), usNumTrack, pFile) < usNumTrack) {
		fclose (pFile);
		return 0;
	}

	/* フォーマット0の場合 */
	if (usFormat == 0) {
		long i;
		MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
		/* テンポ専用トラック */
		lTempTrackAddress[lTrackIndex] = ftell (pFile);
		lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
		pTrackData = malloc (lGuessTrackLen);
		if (pTrackData == NULL) {
			fclose (pFile);
			return 0;
		}
		memset (pTrackData, 0, lGuessTrackLen);
		lTrackLen = MIDITrack_SaveAsCherry0 (pMIDITrack, pTrackData, lGuessTrackLen);
		assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
		lTrackLen4 = lTrackLen + 4;
		/* トラック長さ(4byte) */
		if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		/* トラックデータ(lTrackLenbyte) */
		if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		lTrackIndex++;
		/* システムセットアップ用トラック */
		lTempTrackAddress[lTrackIndex] = ftell (pFile);
		memset (pTrackData, 0, lGuessTrackLen);
		lTrackLen = MIDITrack_SaveAsCherry2 (pMIDITrack, pTrackData, lGuessTrackLen);
		assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
		lTrackLen4 = lTrackLen + 4;
		/* トラック長さ(4byte) */
		if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		/* トラックデータ(lTrackLenbyte) */
		if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		lTrackIndex++;
		/* チャンネル1～16用トラック */		
		for (i = 0; i < 16; i++) {
			lTempTrackAddress[lTrackIndex] = ftell (pFile);
			lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
			pTrackData = malloc (lGuessTrackLen);
			if (pTrackData == NULL) {
				fclose (pFile);
				return 0;
			}
			memset (pTrackData, 0, lGuessTrackLen);
			lTrackLen = MIDITrack_SaveAsCherryChannel (pMIDITrack, i, pTrackData, lGuessTrackLen);
			assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
			lTrackLen4 = lTrackLen + 4;
			/* トラック長さ(4byte) */
			if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
				free (pTrackData);
				fclose (pFile);
				return 0;
			}
			/* トラックデータ(lTrackLenbyte) */
			if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
				free (pTrackData);
				fclose (pFile);
				return 0;
			}
			pTrackData = NULL;
			free (pTrackData);
			lTrackIndex++;
		}
	}
	
	/* フォーマット1,2の場合 */
	else {
		/* 各トラックの保存 */
		lTrackIndex = 0;
		forEachTrack (pMIDIData, pMIDITrack) {
			/* トラック0(コンダクタートラック) */
			if (pMIDITrack->m_lTempIndex == 0) {
				/* トラック0のテンポ部のみ保存 */
				lTempTrackAddress[lTrackIndex] = ftell (pFile);
				lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
				pTrackData = malloc (lGuessTrackLen);
				if (pTrackData == NULL) {
					fclose (pFile);
					return 0;
				}
				memset (pTrackData, 0, lGuessTrackLen);
				lTrackLen = MIDITrack_SaveAsCherry0 (pMIDITrack, pTrackData, lGuessTrackLen);
				assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
				lTrackLen4 = lTrackLen + 4;
				/* トラック長さ(4byte) */
				if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				/* トラックデータ(lTrackLenbyte) */
				if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				lTrackIndex++;
				/* システムセットアップ専用トラックがない場合 */
				if (lHasSystemSetupTrack == 0) {
					/* トラック0からシステムセットアップを生成し追加トラックとして保存 */
					lTempTrackAddress[lTrackIndex] = ftell (pFile);
					lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
					pTrackData = malloc (lGuessTrackLen);
					if (pTrackData == NULL) {
						fclose (pFile);
						return 0;
					}
					memset (pTrackData, 0, lGuessTrackLen);
					lTrackLen = MIDITrack_SaveAsCherry1 (pMIDITrack, pTrackData, lGuessTrackLen);
					assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
					lTrackLen4 = lTrackLen + 4;
					/* トラック長さ(4byte) */
					if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
						free (pTrackData);
						fclose (pFile);
						return 0;
					}
					/* トラックデータ(lTrackLenbyte) */
					if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
						free (pTrackData);
						fclose (pFile);
						return 0;
					}
					free (pTrackData);
					pTrackData = NULL;
					lTrackIndex++;
				}
			}
			/* トラック1以降(システムセットアップトラック又は通常トラック) */
			else {
				lTempTrackAddress[lTrackIndex] = ftell (pFile);
				lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
				pTrackData = malloc (lGuessTrackLen);
				if (pTrackData == NULL) {
					fclose (pFile);
					return 0;
				}
				memset (pTrackData, 0, lGuessTrackLen);
				lTrackLen = MIDITrack_SaveAsCherry2 (pMIDITrack, pTrackData, lGuessTrackLen);
				assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
				lTrackLen4 = lTrackLen + 4;
				/* トラック長さ(4byte) */
				if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				/* トラックデータ(lTrackLenbyte) */
				if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				free (pTrackData);
				pTrackData = NULL;
				lTrackIndex++;
			}
		}
	}

	/* 拡張データ */
	/* 拍子調性 */
	if (pMIDIData->m_pFirstTrack) {
		pMIDITrack = pMIDIData->m_pFirstTrack;
		lTempExtendedDataAddress[0] = ftell (pFile);
		lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
		pTrackData = malloc (lGuessTrackLen);
		if (pTrackData == NULL) {
			fclose (pFile);
			return 0;
		}
		memset (pTrackData, 0, lGuessTrackLen);
		lTrackLen = MIDITrack_SaveAsCherry3 (pMIDITrack, pTrackData, lGuessTrackLen);
		assert (0 <= lTrackLen && lTrackLen < lGuessTrackLen);
		/* 拍子調性データの長さ(4byte) */
		if (fwrite (&lTrackLen, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		/* 拍子調性データ(lTrackLenbyte) */
		if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		free (pTrackData);
		pTrackData = NULL;
	}

	/* 拡張データ先頭アドレスの書き込み */
	fseek (pFile, 512, SEEK_SET);
	if (fwrite (lTempExtendedDataAddress, sizeof (long), 64, pFile) < 64) {
		fclose (pFile);
		return 0;
	}

	/* トラック先頭アドレスの再書き込み */
	fseek (pFile, 768, SEEK_SET);
	if (fwrite (lTempTrackAddress, sizeof (long), usNumTrack, pFile) < usNumTrack) {
		fclose (pFile);
		return 0;
	}

	free (pTrackData);
	pTrackData = NULL;

	fclose (pFile);
	pFile = NULL;

	forEachTrack (pMIDIData, pMIDITrack) {
		pMIDITrack->m_lInputOn = 1;
		pMIDITrack->m_lInputPort = pMIDITrack->m_lOutputPort;
		pMIDITrack->m_lInputChannel = pMIDITrack->m_lOutputChannel;
		pMIDITrack->m_lOutputOn = 1;
	}

	return 1;
}

/* MIDIデータをCherryシーケンスファイル(*.chy)に保存(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsCherryW (MIDIData* pMIDIData, const wchar_t* pszFileName) {

	MIDITrack* pMIDITrack = NULL;
	FILE* pFile = NULL;
	long lGuessTrackLen = 0;
	long lTrackLen = 0;
	long lTrackLen4 = 0;
	long lTrackIndex = 0;
	unsigned short usFormat = (unsigned short)MIDIData_GetFormat (pMIDIData);
	unsigned short usTimeMode = (unsigned short)MIDIData_GetTimeMode (pMIDIData);
	unsigned short usTimeResolution = (unsigned short)MIDIData_GetTimeResolution (pMIDIData);
	unsigned short usNumTrack = (usFormat == 0 ? 18 : (unsigned short)MIDIData_CountTrack (pMIDIData));
	unsigned char* pTrackData = NULL;
	unsigned char byBuf[256];
	char szTempTitle[256];
	char szTempCopyright[256];
	long lHasSystemSetupTrack = 0;
	long lTempExtendedDataAddress[64];
	long lTempTrackAddress[65536];
	memset (szTempTitle, 0, sizeof (szTempTitle));
	memset (szTempCopyright, 0, sizeof (szTempCopyright));
	memset (lTempExtendedDataAddress, 0, sizeof (long) * 64);
	memset (lTempTrackAddress, 0, sizeof (long) * 65536);

	/* 最終的な保存トラック数の計算 */
	if (usFormat == 0) {
		usNumTrack = 18;
	}
	else {
		usNumTrack = (unsigned short)CLIP (0, MIDIData_CountTrack (pMIDIData), MIDIDATA_MAXMIDITRACKNUM);
		if (usNumTrack >= 2) {
			MIDITrack* pSecondTrack = pMIDIData->m_pFirstTrack->m_pNextTrack;
			/* システムセットアップ専用トラックがない場合、 */
			/* システムセットアップ専用トラックを追加保存するので+1 */
			if (MIDITrack_IsSystemSetupTrack (pSecondTrack) == 0) {
				usNumTrack++;
			}
			/* システムセットアップ専用トラックがある場合 */
			else {
				lHasSystemSetupTrack = 1;
			}
		}
		/* システムセットアップ専用トラックがない場合、 */
		/* システムセットアップ専用トラックを追加保存するので+1 */
		else {
			usNumTrack++;
		}
	}

	/* ファイルオープン */
	pFile = _wfopen (pszFileName, L"wb");
	if (pFile == NULL) {
		return 0;
	}

	/* ヘッダー(256byte) */
	memset (&byBuf[0], 0, sizeof (byBuf));
	strcpy ((char*)(&byBuf[0]), "CHRY0101");
	memcpy (&byBuf[8], &usTimeResolution, 2);
	memcpy (&byBuf[10], &usNumTrack, 2);
	if (fwrite (byBuf, 1, 256, pFile) < 256) {
		fclose (pFile);
		return 0;
	}
	/* 曲名と著作権(128+128=256byte) */
	memset (&byBuf[0], 0, sizeof (byBuf));
	MIDIData_GetTitleA (pMIDIData, (char*)(&byBuf[0]), 127);
	MIDIData_GetCopyrightA (pMIDIData, (char*)(&byBuf[128]), 127);
	if (fwrite (byBuf, 1, 256, pFile) < 256) {
		fclose (pFile);
		return 0;
	}
	/* 拡張データの先頭アドレス(4*64=256byte) */
	memset (lTempExtendedDataAddress, 0, sizeof (long) * 64);
	if (fwrite (lTempExtendedDataAddress, sizeof (long), 64, pFile) < 64) {
		fclose (pFile);
		return 0;
	}
	/* トラックの先頭アドレス(4*usNumTrack byte) */
	memset (lTempTrackAddress, 0, sizeof (long) * 65536);
	if (fwrite (lTempTrackAddress, sizeof (long), usNumTrack, pFile) < usNumTrack) {
		fclose (pFile);
		return 0;
	}

	/* フォーマット0の場合 */
	if (usFormat == 0) {
		long i;
		MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
		/* テンポ専用トラック */
		lTempTrackAddress[lTrackIndex] = ftell (pFile);
		lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
		pTrackData = malloc (lGuessTrackLen);
		if (pTrackData == NULL) {
			fclose (pFile);
			return 0;
		}
		memset (pTrackData, 0, lGuessTrackLen);
		lTrackLen = MIDITrack_SaveAsCherry0 (pMIDITrack, pTrackData, lGuessTrackLen);
		assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
		lTrackLen4 = lTrackLen + 4;
		/* トラック長さ(4byte) */
		if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		/* トラックデータ(lTrackLenbyte) */
		if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		lTrackIndex++;
		/* システムセットアップ用トラック */
		lTempTrackAddress[lTrackIndex] = ftell (pFile);
		memset (pTrackData, 0, lGuessTrackLen);
		lTrackLen = MIDITrack_SaveAsCherry1 (pMIDITrack, pTrackData, lGuessTrackLen);
		assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
		lTrackLen4 = lTrackLen + 4;
		/* トラック長さ(4byte) */
		if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		/* トラックデータ(lTrackLenbyte) */
		if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		lTrackIndex++;
		/* チャンネル1～16用トラック */		
		for (i = 0; i < 16; i++) {
			lTempTrackAddress[lTrackIndex] = ftell (pFile);
			lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
			pTrackData = malloc (lGuessTrackLen);
			if (pTrackData == NULL) {
				fclose (pFile);
				return 0;
			}
			memset (pTrackData, 0, lGuessTrackLen);
			lTrackLen = MIDITrack_SaveAsCherryChannel (pMIDITrack, i, pTrackData, lGuessTrackLen);
			assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
			lTrackLen4 = lTrackLen + 4;
			/* トラック長さ(4byte) */
			if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
				free (pTrackData);
				fclose (pFile);
				return 0;
			}
			/* トラックデータ(lTrackLenbyte) */
			if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
				free (pTrackData);
				fclose (pFile);
				return 0;
			}
			pTrackData = NULL;
			free (pTrackData);
			lTrackIndex++;
		}
	}
	
	/* フォーマット1,2の場合 */
	else {
		/* 各トラックの保存 */
		lTrackIndex = 0;
		forEachTrack (pMIDIData, pMIDITrack) {
			/* トラック0(コンダクタートラック) */
			if (pMIDITrack->m_lTempIndex == 0) {
				/* トラック0のテンポ部のみ保存 */
				lTempTrackAddress[lTrackIndex] = ftell (pFile);
				lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
				pTrackData = malloc (lGuessTrackLen);
				if (pTrackData == NULL) {
					fclose (pFile);
					return 0;
				}
				memset (pTrackData, 0, lGuessTrackLen);
				lTrackLen = MIDITrack_SaveAsCherry0 (pMIDITrack, pTrackData, lGuessTrackLen);
				assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
				lTrackLen4 = lTrackLen + 4;
				/* トラック長さ(4byte) */
				if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				/* トラックデータ(lTrackLenbyte) */
				if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				lTrackIndex++;
				/* システムセットアップ専用トラックがない場合 */
				if (lHasSystemSetupTrack == 0) {
					/* トラック0からシステムセットアップを生成し追加トラックとして保存 */
					lTempTrackAddress[lTrackIndex] = ftell (pFile);
					lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
					pTrackData = malloc (lGuessTrackLen);
					if (pTrackData == NULL) {
						fclose (pFile);
						return 0;
					}
					memset (pTrackData, 0, lGuessTrackLen);
					lTrackLen = MIDITrack_SaveAsCherry1 (pMIDITrack, pTrackData, lGuessTrackLen);
					assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
					lTrackLen4 = lTrackLen + 4;
					/* トラック長さ(4byte) */
					if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
						free (pTrackData);
						fclose (pFile);
						return 0;
					}
					/* トラックデータ(lTrackLenbyte) */
					if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
						free (pTrackData);
						fclose (pFile);
						return 0;
					}
					free (pTrackData);
					pTrackData = NULL;
					lTrackIndex++;
				}
			}
			/* トラック1以降(システムセットアップトラック又は通常トラック) */
			else {
				lTempTrackAddress[lTrackIndex] = ftell (pFile);
				lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
				pTrackData = malloc (lGuessTrackLen);
				if (pTrackData == NULL) {
					fclose (pFile);
					return 0;
				}
				memset (pTrackData, 0, lGuessTrackLen);
				lTrackLen = MIDITrack_SaveAsCherry2 (pMIDITrack, pTrackData, lGuessTrackLen);
				assert (124 <= lTrackLen && lTrackLen < lGuessTrackLen);
				lTrackLen4 = lTrackLen + 4;
				/* トラック長さ(4byte) */
				if (fwrite (&lTrackLen4, sizeof (long), 1, pFile) < 1) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				/* トラックデータ(lTrackLenbyte) */
				if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
					free (pTrackData);
					fclose (pFile);
					return 0;
				}
				free (pTrackData);
				pTrackData = NULL;
				lTrackIndex++;
			}
		}
	}

	/* 拡張データ */
	/* 拍子調性 */
	if (pMIDIData->m_pFirstTrack) {
		pMIDITrack = pMIDIData->m_pFirstTrack;
		lTempExtendedDataAddress[0] = ftell (pFile);
		lGuessTrackLen = MIDITrack_GuessTrackDataLenAsCherry (pMIDITrack);
		pTrackData = malloc (lGuessTrackLen);
		if (pTrackData == NULL) {
			fclose (pFile);
			return 0;
		}
		memset (pTrackData, 0, lGuessTrackLen);
		lTrackLen = MIDITrack_SaveAsCherry3 (pMIDITrack, pTrackData, lGuessTrackLen);
		assert (0 <= lTrackLen && lTrackLen < lGuessTrackLen);
		/* 拍子調性データの長さ(4byte) */
		if (fwrite (&lTrackLen, sizeof (long), 1, pFile) < 1) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		/* 拍子調性データ(lTrackLenbyte) */
		if (fwrite (pTrackData, 1, lTrackLen, pFile) < (unsigned long)lTrackLen) {
			free (pTrackData);
			fclose (pFile);
			return 0;
		}
		free (pTrackData);
		pTrackData = NULL;
	}

	/* 拡張データ先頭アドレスの書き込み */
	fseek (pFile, 512, SEEK_SET);
	if (fwrite (lTempExtendedDataAddress, sizeof (long), 64, pFile) < 64) {
		fclose (pFile);
		return 0;
	}

	/* トラック先頭アドレスの再書き込み */
	fseek (pFile, 768, SEEK_SET);
	if (fwrite (lTempTrackAddress, sizeof (long), usNumTrack, pFile) < usNumTrack) {
		fclose (pFile);
		return 0;
	}

	free (pTrackData);
	pTrackData = NULL;

	fclose (pFile);
	pFile = NULL;

	forEachTrack (pMIDIData, pMIDITrack) {
		pMIDITrack->m_lInputOn = 1;
		pMIDITrack->m_lInputPort = pMIDITrack->m_lOutputPort;
		pMIDITrack->m_lInputChannel = pMIDITrack->m_lOutputChannel;
		pMIDITrack->m_lOutputOn = 1;
	}

	return 1;
}


/* カンマ区切りの次のトークンの先頭ポインタを取得する */
static char* getnexttoken_01 (char* pText) {
	char* p = pText;
	int bInsideDoubleQuat = 0;
	/* 次のカンマが現れる位置までポインタを進める */
	/* ダブルクォーテーション内(文字列内)のカンマは無視する */
	while (p) { /* 20120108修正 */
		if (*p == '"') {
			bInsideDoubleQuat = !bInsideDoubleQuat;
		}
		if (*p == ',' && !bInsideDoubleQuat) {
			break;
		}
		else if (*p == '\r' || *p == '\n' || *p == '\0') {
			return p;
		}
		if (0x80 <= *p && *p <= 0x9F || 0xE0 <= *p && *p <= 0xFC) { /* 20120108修正 */
			p++;
			p++;
		}
		else {
			p++;
		}
	}
	/* カンマ直後にフォーカスを合わせる */
	p++;
	/* カンマ直後の半角スペースは読み飛ばす */
	while (p) { /* 20120108修正 */
		if (*p != ' ') {
			break;
		}
		else if (*p == '\r' || *p == '\n' || *p == '\0') {
			return p;
		}
		p++;
	}
	return p;
}

/* strncpyの特殊版02(コード無し文字列をコード入り文字列に変換)(隠蔽) */
/* 20140515修正 */
static char* strncpy_ex02 (char* pText1, char* pText2, long lLen1, long lLen2) {
	char* p1 = pText1;
	char* p2 = pText2;
	while (1) {
		/* 連続ダブルクォーテーション表現""""→0x22'\"'(※空文字列の""を除く) */
		if (p2 - pText2 < lLen2 - 1 && *p2 == '"' && *(p2 + 1) == '"' && p2 != pText2) {
			*p1++ = '"';
			p2++;
			p2++;
		}
		/* ダブルクォーテーションのみ(無視) */
		else if (p2 - pText2 < lLen2 && *p2 == '"') {
			p2++;
		}
		/* バックスラッシュ表現"\\"→0x5C'\\' */
		else if (p1 - pText1 < lLen1 && p2 - pText2 < lLen2 - 1 && 
			*p2 == 0x5C && *(p2 + 1) == 0x5C) {
			*p1++ = 0x5C;
			p2++;
			p2++;
		}
		/* \3桁8進表記"\xxx"→Latin-1で表現できない文字 */
		else if (p1 - pText1 < lLen1 && p2 - pText2 < lLen2 - 4 &&
			*p2 == 0x5C &&
			'0' <= *(p2 + 1) && *(p2 + 1) <= '7' &&
			'0' <= *(p2 + 2) && *(p2 + 2) <= '7' &&
			'0' <= *(p2 + 3) && *(p2 + 3) <= '7') {
			*p1++ = ((unsigned char)*(p2 + 1) - '0') * 64 +
					((unsigned char)*(p2 + 2) - '0') * 8 +
					((unsigned char)*(p2 + 3) - '0');
			p2++;
			p2++;
			p2++;
			p2++;
		}
		/* 1バイト文字(単純コピー) */
		else if (p1 - pText1 < lLen1 &&	p2 - pText2 < lLen2) {
			*p1++ = *p2++;
		}
		else {
			break;
		}
	}
	return pText1;
}

/* MIDIデータをMIDICSVファイル(*.csv)から読み込み(ANSI) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromMIDICSVA (const char* pszFileName) {
	FILE* pFile = NULL;
	char szTextLine[2048];
	long lTextLineLen = 0;
	MIDIData* pMIDIData = NULL;
	MIDITrack* pMIDITrack = NULL;
	long lFormat = 0;
	long lNumTrack = 1;
	long lTimeBase = 1;

	/* ファイルを開く */
	pFile = fopen (pszFileName, "rb"); /* 20120108 単体の\nを扱うためrtをrbに変更 */
	if (pFile == NULL) {
		return NULL;
	}

	/* ファイル終端まで1行ずつ読み込む */
	while (!feof (pFile)) {
		char* pTrackIndex = NULL;
		char* pTime = NULL;
		char* pEventKind = NULL;
		long lTrackIndex = 0;
		long lTime = 0;
		long lKind = 0;
		/* 1行読み取り */
		memset (szTextLine, 0, sizeof (szTextLine));
		fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
		/* コメント行の読み飛ばし */
		if (szTextLine[0] == '#' || szTextLine[0] == ';') {
			continue;
		}
		/* 改行コードの除去(20120108追加) */
		lTextLineLen = strlen (szTextLine);
		if (lTextLineLen > 2 &&
			*(szTextLine + lTextLineLen - 2) == '\r' &&
			*(szTextLine + lTextLineLen - 1) == '\n') {
			*(szTextLine + lTextLineLen - 2) = '\0';
			*(szTextLine + lTextLineLen - 1) = '\0';
		}
		else if (lTextLineLen > 1 &&
			*(szTextLine + lTextLineLen - 1) == '\r') {
			*(szTextLine + lTextLineLen - 1) = '\0';
		}
		else if (lTextLineLen > 1 &&
			*(szTextLine + lTextLineLen - 1) == '\n') {
			*(szTextLine + lTextLineLen - 1) = '\0';
		}
		/* トラック番号、タイム、イベントの種類の読み取り */
		pTrackIndex = szTextLine;
		pTime = getnexttoken_01 (pTrackIndex);
		pEventKind = getnexttoken_01 (pTime);
		lTrackIndex = atol (szTextLine);
		lTime = atol (pTime);

		/* MIDIデータヘッダ情報行 */
		if (lTrackIndex == 0 && lTime == 0 && _strnicmp (pEventKind, "Header", 6) == 0) {
			long lTimeMode = MIDIDATA_TPQNBASE;
			long lTimeResolution = 120;
			char* pFormat = getnexttoken_01 (pEventKind);
			char* pNumTrack = getnexttoken_01 (pFormat);
			char* pTimeBase = getnexttoken_01 (pNumTrack);
			lFormat = atol (pFormat);
			lNumTrack = atol (pNumTrack);
			lTimeBase = atol (pTimeBase);
			if (lFormat < 0 || lFormat > 2) {
				fclose (pFile);
				return NULL;
			}
			if (lNumTrack < 1 || lNumTrack >= 65536) {
				fclose (pFile);
				return NULL;
			}
			if (lTimeBase < 1 || lTimeBase >= 65536) {
				fclose (pFile);
				return NULL;
			}
			if (lTimeBase & 0x8000) {
				lTimeMode = (256 - ((lTimeBase & 0xFF00) >> 8));
				if (lTimeMode != MIDIDATA_SMPTE24BASE &&
					lTimeMode != MIDIDATA_SMPTE25BASE &&
					lTimeMode != MIDIDATA_SMPTE29BASE &&
					lTimeMode != MIDIDATA_SMPTE30BASE) {
					fclose (pFile);
					return NULL;
				}
				lTimeResolution = lTimeBase & 0xFF;
			}
			else {
				lTimeMode = MIDIDATA_TPQNBASE;
				lTimeResolution = lTimeBase & 0x7FFF;
			}
			/* 二重にMIDIデータを作ろうとした場合、直ちに読み込み中断 */
			if (pMIDIData != NULL) {
				break;
			}
			/* MIDIデータの作成(SMPTEベースはサポートしない) */
			pMIDIData = MIDIData_Create (lFormat, lNumTrack, lTimeMode, lTimeResolution);
			if (pMIDIData == NULL) {
				return NULL;
			}
		}

		/* スタートオブトラック */
		if (lTime == 0 && _strnicmp (pEventKind, "Start_track", 11) == 0) {
			/* 二重にMIDIデータを作ろうとした場合、直ちに読み込み中断 */
			if (pMIDITrack != NULL) {
				break;
			}
			/* このセクションのMIDIトラックの取得 */
			if (pMIDIData && 1 <= lTrackIndex && lTrackIndex <= lNumTrack) {
				pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrackIndex - 1);
			}
		}

		/* エンドオブトラック */
		if (_strnicmp (pEventKind, "End_track", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				MIDITrack_InsertEndofTrack (pMIDITrack, lTime);
			}
			pMIDITrack = NULL;
		}

		/* シーケンス番号 */
		if (_strnicmp (pEventKind, "Sequence_number", 15) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pNumber = getnexttoken_01 (pEventKind);
				long lNumber = CLIP (0, atol (pNumber), 65535);
				MIDITrack_InsertSequenceNumber (pMIDITrack, lTime, lNumber);
			}
		}

		/* テキスト */
		if (_strnicmp (pEventKind, "Text_t", 6) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertTextEventA (pMIDITrack, lTime, szText);
			}
		}
		
		/* 著作権 */
		if (_strnicmp (pEventKind, "Copyright_t", 11) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertCopyrightNoticeA (pMIDITrack, lTime, szText);
			}
		}

		/* トラック名／シーケンス名 */
		if (_strnicmp (pEventKind, "Title_t", 7) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertTrackNameA (pMIDITrack, lTime, szText);
			}
		}

		/* インストゥルメント名 */
		if (_strnicmp (pEventKind, "Instrument_name_t", 17) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertInstrumentNameA (pMIDITrack, lTime, szText);
			}
		}

		/* 歌詞 */
		if (_strnicmp (pEventKind, "Lyric_t", 7) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertLyricA (pMIDITrack, lTime, szText);
			}
		}

		/* マーカー */
		if (_strnicmp (pEventKind, "Marker_t", 8) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertMarkerA (pMIDITrack, lTime, szText);
			}
		}

		/* キューポイント */
		if (_strnicmp (pEventKind, "Cue_point_t", 11) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertCuePointA (pMIDITrack, lTime, szText);
			}
		}

		/* プログラム名(処理なし) */

		/* デバイス名(処理なし) */

		/* ポートプリフィックス */
		if (_strnicmp (pEventKind, "MIDI_port", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pNumber = getnexttoken_01 (pEventKind);
				long lNumber = CLIP (0, atol (pNumber), 255);
				MIDITrack_InsertPortPrefix (pMIDITrack, lTime, lNumber);
			}
		}

		/* チャンネルプリフィックス */
		if (_strnicmp (pEventKind, "Channel_prefix", 14) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pNumber = getnexttoken_01 (pEventKind);
				long lNumber = CLIP (0, atol (pNumber), 15);
				MIDITrack_InsertChannelPrefix (pMIDITrack, lTime, lNumber);
			}
		}


		/* テンポ */
		if (_strnicmp (pEventKind, "Tempo", 5) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pTempo = getnexttoken_01 (pEventKind);
				long lTempo = CLIP (1, atol (pTempo), 60000000);
				MIDITrack_InsertTempo (pMIDITrack, lTime, lTempo);
			}
		}

		/* SMPTEオフセット */
		if (_strnicmp (pEventKind, "SMPTE_offset", 12) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pHour = getnexttoken_01 (pEventKind);
				char* pMinute = getnexttoken_01 (pHour);
				char* pSecond = getnexttoken_01 (pMinute);
				char* pFrame = getnexttoken_01 (pSecond);
				char* pSubFrame = getnexttoken_01 (pFrame);
				long lRate = CLIP (0, (atol (pHour) & 0x60) >> 5, 3);
				long lHour = CLIP (0, atol (pHour) & 0x1F, 23);
				long lMinute = CLIP (0, atol (pMinute), 59);
				long lSecond = CLIP (0, atol (pSecond), 59);
				long lFrame = CLIP (0, atol (pFrame), 29);
				long lSubFrame = CLIP (0, atol (pSubFrame), 99);
				MIDITrack_InsertSMPTEOffset (pMIDITrack, lTime, lRate, lHour, lMinute, lSecond, lFrame, lSubFrame);
			}
		}

		/* 拍子記号 */
		if (_strnicmp (pEventKind, "Time_signature", 14) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pN = getnexttoken_01 (pEventKind);
				char* pD = getnexttoken_01 (pN);
				char* pC = getnexttoken_01 (pD);
				char* pB = getnexttoken_01 (pC);
				long lN = CLIP (1, atol (pN), 127);
				long lD = CLIP (1, atol (pD), 8);
				long lC = CLIP (1, atol (pC), 127); /* 常に24が正解 */
				long lB = CLIP (1, atol (pB), 127); /* 常に8が正解 */
				MIDITrack_InsertTimeSignature (pMIDITrack, lTime, lN, lD, lC, lB);
			}
		}

		/* 調性記号 */
		if (_strnicmp (pEventKind, "Key_signature", 13) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pTone = getnexttoken_01 (pEventKind);
				char* pMinor = getnexttoken_01 (pTone);
				long lTone = CLIP (-7, atol (pTone), 7);
				long lMinor = _strnicmp (pMinor, "\"minor\"", 7) == 0 ? 1 : 0;
				MIDITrack_InsertKeySignature (pMIDITrack, lTime, lTone, lMinor);
			}
		}

		/* シーケンサー独自のイベント */
		if (_strnicmp (pEventKind, "Sequencer_specific", 18) == 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 0;
				unsigned char ucData[1024];
				char* pLen = getnexttoken_01 (pEventKind);
				char* pData = getnexttoken_01 (pLen);
				long lLen = CLIP (0, atol (pLen), sizeof (ucData));
				memset (ucData, 0, sizeof (ucData));
				while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen) { 
					ucData[k++] = (unsigned char)atoi (pData);
					pData = getnexttoken_01 (pData);
				}
				MIDITrack_InsertSequencerSpecific (pMIDITrack, lTime, ucData, lLen);
			}
		}

		/* 未定義のメタイベント */
		if (_strnicmp (pEventKind, "Unknown_meta_event", 18) == 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 0;
				unsigned char ucData[1024];
				char* pKind = getnexttoken_01 (pEventKind);
				char* pLen = getnexttoken_01 (pKind);
				char* pData = getnexttoken_01 (pLen);
				long lKind = atol (pKind);
				if (0x00 <= lKind && lKind <= 0x7F) {
					MIDIEvent* pMIDIEvent = NULL;
					long lLen = CLIP (0, atol (pLen), sizeof (ucData));
					memset (ucData, 0, sizeof (ucData));
					while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen) { 
						ucData[k++] = (unsigned char)atoi (pData);
						pData = getnexttoken_01 (pData);
					}
					pMIDIEvent = MIDIEvent_Create (lTime, lKind, ucData, lLen);
					if (pMIDIEvent) {
						MIDITrack_InsertEvent (pMIDITrack, pMIDIEvent);
					}
				}
			}
		}

		/* ノートオフ */
		if (_strnicmp (pEventKind, "Note_off_c", 10) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pKey = getnexttoken_01 (pChannel);
				char* pVelocity = getnexttoken_01 (pKey);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lKey = CLIP (0, atol (pKey), 127);
				long lVelocity = CLIP (0, atol (pVelocity), 127);
				MIDITrack_InsertNoteOff (pMIDITrack, lTime, lChannel, lKey, lVelocity);
			}
		}

		/* ノートオン */
		if (_strnicmp (pEventKind, "Note_on_c", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pKey = getnexttoken_01 (pChannel);
				char* pVelocity = getnexttoken_01 (pKey);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lKey = CLIP (0, atol (pKey), 127);
				long lVelocity = CLIP (0, atol (pVelocity), 127);
				MIDITrack_InsertNoteOn (pMIDITrack, lTime, lChannel, lKey, lVelocity);
			}
		}

		/* キーアフタータッチ */
		if (_strnicmp (pEventKind, "Poly_aftertouch_c", 17) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pKey = getnexttoken_01 (pChannel);
				char* pValue = getnexttoken_01 (pKey);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lKey = CLIP (0, atol (pKey), 127);
				long lValue = CLIP (0, atol (pValue), 127);
				MIDITrack_InsertKeyAftertouch (pMIDITrack, lTime, lChannel, lKey, lValue);
			}
		}

		/* コントロールチェンジ */
		if (_strnicmp (pEventKind, "Control_c", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pNumber = getnexttoken_01 (pChannel);
				char* pValue = getnexttoken_01 (pNumber);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lNumber = CLIP (0, atol (pNumber), 127);
				long lValue = CLIP (0, atol (pValue), 127);
				MIDITrack_InsertControlChange (pMIDITrack, lTime, lChannel, lNumber, lValue);
			}
		}

		/* プログラムチェンジ */
		if (_strnicmp (pEventKind, "Program_c", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pNumber = getnexttoken_01 (pChannel);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lNumber = CLIP (0, atol (pNumber), 127);
				MIDITrack_InsertProgramChange (pMIDITrack, lTime, lChannel, lNumber);
			}
		}

		/* チャンネルアフタータッチ */
		if (_strnicmp (pEventKind, "Channel_aftertouch_c", 20) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pValue = getnexttoken_01 (pChannel);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lValue = CLIP (0, atol (pValue), 127);
				MIDITrack_InsertChannelAftertouch (pMIDITrack, lTime, lChannel, lValue);
			}
		}

		/* ピッチベンド */
		if (_strnicmp (pEventKind, "Pitch_bend_c", 12) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pValue = getnexttoken_01 (pChannel);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lValue = CLIP (0, atol (pValue), 16363);
				MIDITrack_InsertPitchBend (pMIDITrack, lTime, lChannel, lValue);
			}
		}

		/* システムエクスクルーシヴ */
		if (_strnicmp (pEventKind, "System_exclusive", 16) == 0 &&
			_strnicmp (pEventKind, "System_exclusive_packet", 23) != 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 1;
				unsigned char ucData[1024];
				char* pLen = getnexttoken_01 (pEventKind);
				char* pData = getnexttoken_01 (pLen);
				long lLen = CLIP (0, atol (pLen), sizeof (ucData));
				memset (ucData, 0, sizeof (ucData));
				ucData[0] = 0xF0;
				while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen + 1) { 
					ucData[k++] = (unsigned char)atoi (pData);
					pData = getnexttoken_01 (pData);
				}
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, ucData, lLen + 1);
			}
		}

		/* システムエクスクルーシヴ(続き) */
		if (_strnicmp (pEventKind, "System_exclusive_packet", 23) == 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 0;
				unsigned char ucData[1024];
				char* pLen = getnexttoken_01 (pEventKind);
				char* pData = getnexttoken_01 (pLen);
				long lLen = CLIP (0, atol (pLen), sizeof (ucData));
				memset (ucData, 0, sizeof (ucData));
				while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen) { 
					ucData[k++] = (unsigned char)atoi (pData);
					pData = getnexttoken_01 (pData);
				}
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, ucData, lLen);
			}
		}
	}

	/* ファイルを閉じる */
	fclose (pFile);

	/* エンドオブトラックを付け忘れているトラックは自動的に付加する */
	if (pMIDIData) {
		forEachTrack (pMIDIData, pMIDITrack) {
			MIDIEvent* pLastEvent = MIDITrack_GetLastEvent (pMIDITrack);
			if (!MIDIEvent_IsEndofTrack (pLastEvent)) {
				MIDITrack_InsertEndofTrack (pMIDITrack, pLastEvent->m_lTime);
			}
		}
	}

	/* 各トラックの出力ポート番号・出力チャンネル・表示モードの自動設定 */
	MIDIData_UpdateOutputPort (pMIDIData);
	MIDIData_UpdateOutputChannel (pMIDIData);
	MIDIData_UpdateViewMode (pMIDIData);

	/* 各トラックの入力ポート番号・入力チャンネルの設定 */
	forEachTrack (pMIDIData, pMIDITrack) {
		pMIDITrack->m_lInputOn = 1;
		pMIDITrack->m_lInputPort = pMIDITrack->m_lOutputPort;
		pMIDITrack->m_lInputChannel = pMIDITrack->m_lOutputChannel;
		pMIDITrack->m_lOutputOn = 1;
	}

	return pMIDIData;
}

/* MIDIデータをMIDICSVファイル(*.csv)から読み込み(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromMIDICSVW (const wchar_t* pszFileName) {
	FILE* pFile = NULL;
	char szTextLine[2048];
	long lTextLineLen = 0;
	MIDIData* pMIDIData = NULL;
	MIDITrack* pMIDITrack = NULL;
	long lFormat = 0;
	long lNumTrack = 1;
	long lTimeBase = 1;

	/* ファイルを開く */
	pFile = _wfopen (pszFileName, L"rb"); /* 20120108 単体の\nを扱うためrtをrbに変更 */
	if (pFile == NULL) {
		return NULL;
	}

	/* ファイル終端まで1行ずつ読み込む */
	while (!feof (pFile)) {
		char* pTrackIndex = NULL;
		char* pTime = NULL;
		char* pEventKind = NULL;
		long lTrackIndex = 0;
		long lTime = 0;
		long lKind = 0;
		/* 1行読み取り */
		memset (szTextLine, 0, sizeof (szTextLine));
		fgets (szTextLine, sizeof (szTextLine) - 1, pFile);
		/* コメント行の読み飛ばし */
		if (szTextLine[0] == '#' || szTextLine[0] == ';') {
			continue;
		}
		/* 改行コードの除去(20120108追加) */
		lTextLineLen = strlen (szTextLine);
		if (lTextLineLen > 2 &&
			*(szTextLine + lTextLineLen - 2) == '\r' &&
			*(szTextLine + lTextLineLen - 1) == '\n') {
			*(szTextLine + lTextLineLen - 2) = '\0';
			*(szTextLine + lTextLineLen - 1) = '\0';
		}
		else if (lTextLineLen > 1 &&
			*(szTextLine + lTextLineLen - 1) == '\r') {
			*(szTextLine + lTextLineLen - 1) = '\0';
		}
		else if (lTextLineLen > 1 &&
			*(szTextLine + lTextLineLen - 1) == '\n') {
			*(szTextLine + lTextLineLen - 1) = '\0';
		}
		/* トラック番号、タイム、イベントの種類の読み取り */
		pTrackIndex = szTextLine;
		pTime = getnexttoken_01 (pTrackIndex);
		pEventKind = getnexttoken_01 (pTime);
		lTrackIndex = atol (szTextLine);
		lTime = atol (pTime);

		/* MIDIデータヘッダ情報行 */
		if (lTrackIndex == 0 && lTime == 0 && _strnicmp (pEventKind, "Header", 6) == 0) {
			long lTimeMode = MIDIDATA_TPQNBASE;
			long lTimeResolution = 120;
			char* pFormat = getnexttoken_01 (pEventKind);
			char* pNumTrack = getnexttoken_01 (pFormat);
			char* pTimeBase = getnexttoken_01 (pNumTrack);
			lFormat = atol (pFormat);
			lNumTrack = atol (pNumTrack);
			lTimeBase = atol (pTimeBase);
			if (lFormat < 0 || lFormat > 2) {
				fclose (pFile);
				return NULL;
			}
			if (lNumTrack < 1 || lNumTrack >= 65536) {
				fclose (pFile);
				return NULL;
			}
			if (lTimeBase < 1 || lTimeBase >= 65536) {
				fclose (pFile);
				return NULL;
			}
			if (lTimeBase & 0x8000) {
				lTimeMode = (256 - ((lTimeBase & 0xFF00) >> 8));
				if (lTimeMode != MIDIDATA_SMPTE24BASE &&
					lTimeMode != MIDIDATA_SMPTE25BASE &&
					lTimeMode != MIDIDATA_SMPTE29BASE &&
					lTimeMode != MIDIDATA_SMPTE30BASE) {
					fclose (pFile);
					return NULL;
				}
				lTimeResolution = lTimeBase & 0xFF;
			}
			else {
				lTimeMode = MIDIDATA_TPQNBASE;
				lTimeResolution = lTimeBase & 0x7FFF;
			}
			/* 二重にMIDIデータを作ろうとした場合、直ちに読み込み中断 */
			if (pMIDIData != NULL) {
				break;
			}
			/* MIDIデータの作成(SMPTEベースはサポートしない) */
			pMIDIData = MIDIData_Create (lFormat, lNumTrack, lTimeMode, lTimeResolution);
			if (pMIDIData == NULL) {
				return NULL;
			}
		}

		/* スタートオブトラック */
		if (lTime == 0 && _strnicmp (pEventKind, "Start_track", 11) == 0) {
			/* 二重にMIDIデータを作ろうとした場合、直ちに読み込み中断 */
			if (pMIDITrack != NULL) {
				break;
			}
			/* このセクションのMIDIトラックの取得 */
			if (pMIDIData && 1 <= lTrackIndex && lTrackIndex <= lNumTrack) {
				pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrackIndex - 1);
			}
		}

		/* エンドオブトラック */
		if (_strnicmp (pEventKind, "End_track", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				MIDITrack_InsertEndofTrack (pMIDITrack, lTime);
			}
			pMIDITrack = NULL;
		}

		/* シーケンス番号 */
		if (_strnicmp (pEventKind, "Sequence_number", 15) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pNumber = getnexttoken_01 (pEventKind);
				long lNumber = CLIP (0, atol (pNumber), 65535);
				MIDITrack_InsertSequenceNumber (pMIDITrack, lTime, lNumber);
			}
		}

		/* テキスト */
		if (_strnicmp (pEventKind, "Text_t", 6) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertTextEventA (pMIDITrack, lTime, szText);
			}
		}
		
		/* 著作権 */
		if (_strnicmp (pEventKind, "Copyright_t", 11) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertCopyrightNoticeA (pMIDITrack, lTime, szText);
			}
		}

		/* トラック名／シーケンス名 */
		if (_strnicmp (pEventKind, "Title_t", 7) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertTrackNameA (pMIDITrack, lTime, szText);
			}
		}

		/* インストゥルメント名 */
		if (_strnicmp (pEventKind, "Instrument_name_t", 17) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertInstrumentNameA (pMIDITrack, lTime, szText);
			}
		}

		/* 歌詞 */
		if (_strnicmp (pEventKind, "Lyric_t", 7) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertLyricA (pMIDITrack, lTime, szText);
			}
		}

		/* マーカー */
		if (_strnicmp (pEventKind, "Marker_t", 8) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertMarkerA (pMIDITrack, lTime, szText);
			}
		}

		/* キューポイント */
		if (_strnicmp (pEventKind, "Cue_point_t", 11) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pText = getnexttoken_01 (pEventKind);
				char szText[1024];
				memset (szText, 0, sizeof (szText));
				strncpy_ex02 (szText, pText, sizeof (szText) - 1, strlen (pText));
				MIDITrack_InsertCuePointA (pMIDITrack, lTime, szText);
			}
		}

		/* プログラム名(処理なし) */

		/* デバイス名(処理なし) */

		/* ポートプリフィックス */
		if (_strnicmp (pEventKind, "MIDI_port", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pNumber = getnexttoken_01 (pEventKind);
				long lNumber = CLIP (0, atol (pNumber), 255);
				MIDITrack_InsertPortPrefix (pMIDITrack, lTime, lNumber);
			}
		}

		/* チャンネルプリフィックス */
		if (_strnicmp (pEventKind, "Channel_prefix", 14) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pNumber = getnexttoken_01 (pEventKind);
				long lNumber = CLIP (0, atol (pNumber), 15);
				MIDITrack_InsertChannelPrefix (pMIDITrack, lTime, lNumber);
			}
		}


		/* テンポ */
		if (_strnicmp (pEventKind, "Tempo", 5) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pTempo = getnexttoken_01 (pEventKind);
				long lTempo = CLIP (1, atol (pTempo), 60000000);
				MIDITrack_InsertTempo (pMIDITrack, lTime, lTempo);
			}
		}

		/* SMPTEオフセット */
		if (_strnicmp (pEventKind, "SMPTE_offset", 12) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pHour = getnexttoken_01 (pEventKind);
				char* pMinute = getnexttoken_01 (pHour);
				char* pSecond = getnexttoken_01 (pMinute);
				char* pFrame = getnexttoken_01 (pSecond);
				char* pSubFrame = getnexttoken_01 (pFrame);
				long lRate = CLIP (0, (atol (pHour) & 0x60) >> 5, 3);
				long lHour = CLIP (0, atol (pHour) & 0x1F, 23);
				long lMinute = CLIP (0, atol (pMinute), 59);
				long lSecond = CLIP (0, atol (pSecond), 59);
				long lFrame = CLIP (0, atol (pFrame), 29);
				long lSubFrame = CLIP (0, atol (pSubFrame), 99);
				MIDITrack_InsertSMPTEOffset (pMIDITrack, lTime, lRate, lHour, lMinute, lSecond, lFrame, lSubFrame);
			}
		}

		/* 拍子記号 */
		if (_strnicmp (pEventKind, "Time_signature", 14) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pN = getnexttoken_01 (pEventKind);
				char* pD = getnexttoken_01 (pN);
				char* pC = getnexttoken_01 (pD);
				char* pB = getnexttoken_01 (pC);
				long lN = CLIP (1, atol (pN), 127);
				long lD = CLIP (1, atol (pD), 8);
				long lC = CLIP (1, atol (pC), 127); /* 常に24が正解 */
				long lB = CLIP (1, atol (pB), 127); /* 常に8が正解 */
				MIDITrack_InsertTimeSignature (pMIDITrack, lTime, lN, lD, lC, lB);
			}
		}

		/* 調性記号 */
		if (_strnicmp (pEventKind, "Key_signature", 13) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pTone = getnexttoken_01 (pEventKind);
				char* pMinor = getnexttoken_01 (pTone);
				long lTone = CLIP (-7, atol (pTone), 7);
				long lMinor = _strnicmp (pMinor, "\"minor\"", 7) == 0 ? 1 : 0;
				MIDITrack_InsertKeySignature (pMIDITrack, lTime, lTone, lMinor);
			}
		}

		/* シーケンサー独自のイベント */
		if (_strnicmp (pEventKind, "Sequencer_specific", 18) == 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 0;
				unsigned char ucData[1024];
				char* pLen = getnexttoken_01 (pEventKind);
				char* pData = getnexttoken_01 (pLen);
				long lLen = CLIP (0, atol (pLen), sizeof (ucData));
				memset (ucData, 0, sizeof (ucData));
				while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen) { 
					ucData[k++] = (unsigned char)atoi (pData);
					pData = getnexttoken_01 (pData);
				}
				MIDITrack_InsertSequencerSpecific (pMIDITrack, lTime, ucData, lLen);
			}
		}

		/* 未定義のメタイベント */
		if (_strnicmp (pEventKind, "Unknown_meta_event", 18) == 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 0;
				unsigned char ucData[1024];
				char* pKind = getnexttoken_01 (pEventKind);
				char* pLen = getnexttoken_01 (pKind);
				char* pData = getnexttoken_01 (pLen);
				long lKind = atol (pKind);
				if (0x00 <= lKind && lKind <= 0x7F) {
					MIDIEvent* pMIDIEvent = NULL;
					long lLen = CLIP (0, atol (pLen), sizeof (ucData));
					memset (ucData, 0, sizeof (ucData));
					while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen) { 
						ucData[k++] = (unsigned char)atoi (pData);
						pData = getnexttoken_01 (pData);
					}
					pMIDIEvent = MIDIEvent_Create (lTime, lKind, ucData, lLen);
					if (pMIDIEvent) {
						MIDITrack_InsertEvent (pMIDITrack, pMIDIEvent);
					}
				}
			}
		}

		/* ノートオフ */
		if (_strnicmp (pEventKind, "Note_off_c", 10) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pKey = getnexttoken_01 (pChannel);
				char* pVelocity = getnexttoken_01 (pKey);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lKey = CLIP (0, atol (pKey), 127);
				long lVelocity = CLIP (0, atol (pVelocity), 127);
				MIDITrack_InsertNoteOff (pMIDITrack, lTime, lChannel, lKey, lVelocity);
			}
		}

		/* ノートオン */
		if (_strnicmp (pEventKind, "Note_on_c", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pKey = getnexttoken_01 (pChannel);
				char* pVelocity = getnexttoken_01 (pKey);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lKey = CLIP (0, atol (pKey), 127);
				long lVelocity = CLIP (0, atol (pVelocity), 127);
				MIDITrack_InsertNoteOn (pMIDITrack, lTime, lChannel, lKey, lVelocity);
			}
		}

		/* キーアフタータッチ */
		if (_strnicmp (pEventKind, "Poly_aftertouch_c", 17) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pKey = getnexttoken_01 (pChannel);
				char* pValue = getnexttoken_01 (pKey);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lKey = CLIP (0, atol (pKey), 127);
				long lValue = CLIP (0, atol (pValue), 127);
				MIDITrack_InsertKeyAftertouch (pMIDITrack, lTime, lChannel, lKey, lValue);
			}
		}

		/* コントロールチェンジ */
		if (_strnicmp (pEventKind, "Control_c", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pNumber = getnexttoken_01 (pChannel);
				char* pValue = getnexttoken_01 (pNumber);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lNumber = CLIP (0, atol (pNumber), 127);
				long lValue = CLIP (0, atol (pValue), 127);
				MIDITrack_InsertControlChange (pMIDITrack, lTime, lChannel, lNumber, lValue);
			}
		}

		/* プログラムチェンジ */
		if (_strnicmp (pEventKind, "Program_c", 9) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pNumber = getnexttoken_01 (pChannel);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lNumber = CLIP (0, atol (pNumber), 127);
				MIDITrack_InsertProgramChange (pMIDITrack, lTime, lChannel, lNumber);
			}
		}

		/* チャンネルアフタータッチ */
		if (_strnicmp (pEventKind, "Channel_aftertouch_c", 20) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pValue = getnexttoken_01 (pChannel);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lValue = CLIP (0, atol (pValue), 127);
				MIDITrack_InsertChannelAftertouch (pMIDITrack, lTime, lChannel, lValue);
			}
		}

		/* ピッチベンド */
		if (_strnicmp (pEventKind, "Pitch_bend_c", 12) == 0) {
			if (pMIDIData && pMIDITrack) {
				char* pChannel = getnexttoken_01 (pEventKind);
				char* pValue = getnexttoken_01 (pChannel);
				long lChannel = CLIP (0, atol (pChannel), 15);
				long lValue = CLIP (0, atol (pValue), 16363);
				MIDITrack_InsertPitchBend (pMIDITrack, lTime, lChannel, lValue);
			}
		}

		/* システムエクスクルーシヴ */
		if (_strnicmp (pEventKind, "System_exclusive", 16) == 0 &&
			_strnicmp (pEventKind, "System_exclusive_packet", 23) != 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 1;
				unsigned char ucData[1024];
				char* pLen = getnexttoken_01 (pEventKind);
				char* pData = getnexttoken_01 (pLen);
				long lLen = CLIP (0, atol (pLen), sizeof (ucData));
				memset (ucData, 0, sizeof (ucData));
				ucData[0] = 0xF0;
				while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen + 1) { 
					ucData[k++] = (unsigned char)atoi (pData);
					pData = getnexttoken_01 (pData);
				}
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, ucData, lLen + 1);
			}
		}

		/* システムエクスクルーシヴ(続き) */
		if (_strnicmp (pEventKind, "System_exclusive_packet", 23) == 0) {
			if (pMIDIData && pMIDITrack) {
				long k = 0;
				unsigned char ucData[1024];
				char* pLen = getnexttoken_01 (pEventKind);
				char* pData = getnexttoken_01 (pLen);
				long lLen = CLIP (0, atol (pLen), sizeof (ucData));
				memset (ucData, 0, sizeof (ucData));
				while (*pData != '\r' && *pData != '\n' && *pData != '\0' && k < lLen) { 
					ucData[k++] = (unsigned char)atoi (pData);
					pData = getnexttoken_01 (pData);
				}
				MIDITrack_InsertSysExEvent (pMIDITrack, lTime, ucData, lLen);
			}
		}
	}

	/* ファイルを閉じる */
	fclose (pFile);

	/* エンドオブトラックを付け忘れているトラックは自動的に付加する */
	if (pMIDIData) {
		forEachTrack (pMIDIData, pMIDITrack) {
			MIDIEvent* pLastEvent = MIDITrack_GetLastEvent (pMIDITrack);
			if (!MIDIEvent_IsEndofTrack (pLastEvent)) {
				MIDITrack_InsertEndofTrack (pMIDITrack, pLastEvent->m_lTime);
			}
		}
	}

	/* 各トラックの出力ポート番号・出力チャンネル・表示モードの自動設定 */
	MIDIData_UpdateOutputPort (pMIDIData);
	MIDIData_UpdateOutputChannel (pMIDIData);
	MIDIData_UpdateViewMode (pMIDIData);

	/* 各トラックの入力ポート番号・入力チャンネルの設定 */
	forEachTrack (pMIDIData, pMIDITrack) {
		pMIDITrack->m_lInputOn = 1;
		pMIDITrack->m_lInputPort = pMIDITrack->m_lOutputPort;
		pMIDITrack->m_lInputChannel = pMIDITrack->m_lOutputChannel;
		pMIDITrack->m_lOutputOn = 1;
	}

	return pMIDIData;
}


/* strncpyの特殊版01(コード入り文字列をコード無し文字列に変換)(隠蔽) */
/* 20140515修正 */
static char* strncpy_ex01 (char* pText1, char* pText2, long lLen1, long lLen2) {
	char* p1 = pText1;
	char* p2 = pText2;
	while (1) {
		/* 0x22'\"'→ダブルクォーテーション表現"""" */
		if (p1 - pText1 < lLen1 - 1 && 
			p2 - pText2 < lLen2 && *p2 == '\"') {
			*p1++ = '\"';
			*p1++ = '\"';
			p2++;
		}
		/* 0x5C'\\'→バックスラッシュ表現"\\" */
		else if (p1 - pText1 < lLen1 - 1 && 
			p2 - pText2 < lLen2 && *p2 == 0x5C) {
			*p1++ = 0x5C;
			*p1++ = 0x5C;
			p2++;
		}
		/* Latin-1で表現できない文字→\3桁8進表記"\xxx" */
		else if (p1 - pText1 < lLen1 - 4 && p2 - pText2 < lLen2 &&
			(0x00 <= (unsigned char)*p2 && (unsigned char)*p2 <= 0x1F || 
			0x7F <= (unsigned char)*p2 && (unsigned char)*p2 <= 0xA0)) {
			*p1++ = 0x5C;
			*p1++ = ((unsigned char)*p2 / 64) + '0';
			*p1++ = (((unsigned char)*p2 / 8) % 8) + '0';
			*p1++ = ((unsigned char)*p2 % 8) + '0';
			p2++;
		}
		/* 1バイト文字(単純コピー) */
		else if (p1 - pText1 < lLen1 && p2 - pText2 < lLen2) {
			*p1++ = *p2++;
		}
		else {
			break;
		}
	}
	return pText1;
}

/* MIDICSV形式でイベントを1つファイルに保存する(隠蔽) */
static long MIDIEvent_SaveAsMIDICSV (MIDIEvent* pMIDIEvent, long lTrackIndex, FILE* pFile) {
	long lWriteLen = 0;
	long lTime = pMIDIEvent->m_lTime;
	long k = 0;
	if (MIDIEvent_IsMetaEvent (pMIDIEvent)) {
		char szText[1024];
		memset (szText, 0, sizeof (szText));
		switch (pMIDIEvent->m_lKind) {
		case MIDIEVENT_SEQUENCENUMBER:
			lWriteLen = fprintf (pFile, "%ld, %ld, Sequence_number, %ld\n", 
				lTrackIndex + 1, lTime,	MIDIEvent_GetNumber (pMIDIEvent));
			break;
		case MIDIEVENT_TEXTEVENT:
			strncpy_ex01 (szText, pMIDIEvent->m_pData, sizeof (szText) - 1, pMIDIEvent->m_lLen);
			lWriteLen = fprintf (pFile, "%ld, %ld, Text_t, \"%s\"\n", 
				lTrackIndex + 1, lTime, szText);
			break;
		case MIDIEVENT_COPYRIGHTNOTICE:
			strncpy_ex01 (szText, pMIDIEvent->m_pData, sizeof (szText) - 1, pMIDIEvent->m_lLen);
			lWriteLen = fprintf (pFile, "%ld, %ld, Copyright_t, \"%s\"\n", 
				lTrackIndex + 1, lTime, szText);
			break;
		case MIDIEVENT_TRACKNAME:
			strncpy_ex01 (szText, pMIDIEvent->m_pData, sizeof (szText) - 1, pMIDIEvent->m_lLen);
			lWriteLen = fprintf (pFile, "%ld, %ld, Title_t, \"%s\"\n", 
				lTrackIndex + 1, lTime, szText);
			break;
		case MIDIEVENT_INSTRUMENTNAME:
			strncpy_ex01 (szText, pMIDIEvent->m_pData, sizeof (szText) - 1, pMIDIEvent->m_lLen);
			lWriteLen = fprintf (pFile, "%ld, %ld, Instrument_name_t, \"%s\"\n", 
				lTrackIndex + 1, lTime, szText);
			break;
		case MIDIEVENT_LYRIC:
			strncpy_ex01 (szText, pMIDIEvent->m_pData, sizeof (szText) - 1, pMIDIEvent->m_lLen);
			lWriteLen = fprintf (pFile, "%ld, %ld, Lyric_t, \"%s\"\n", 
				lTrackIndex + 1, lTime, szText);
			break;
		case MIDIEVENT_MARKER:
			strncpy_ex01 (szText, pMIDIEvent->m_pData, sizeof (szText) - 1, pMIDIEvent->m_lLen);
			lWriteLen = fprintf (pFile, "%ld, %ld, Marker_t, \"%s\"\n", 
				lTrackIndex + 1, lTime, szText);
			break;
		case MIDIEVENT_CUEPOINT:
			strncpy_ex01 (szText, pMIDIEvent->m_pData, sizeof (szText) - 1, pMIDIEvent->m_lLen);
			lWriteLen = fprintf (pFile, "%ld, %ld, Cue_point_t, \"%s\"\n", 
				lTrackIndex + 1, lTime, szText);
			break;
		/*case MIDIEVENT_PROGRAMNAME: */
		/*	break; */
		/*case MIDIEVENT_DEVICENAME: */
		/*	break; */
		case MIDIEVENT_CHANNELPREFIX:
			lWriteLen = fprintf (pFile, "%ld, %ld, Channel_prefix, %ld\n", 
				lTrackIndex + 1, lTime,	MIDIEvent_GetNumber (pMIDIEvent));
			break;
		case MIDIEVENT_PORTPREFIX:
			lWriteLen = fprintf (pFile, "%ld, %ld, MIDI_port, %ld\n", 
				lTrackIndex + 1, lTime, MIDIEvent_GetNumber (pMIDIEvent));
			break;
		case MIDIEVENT_ENDOFTRACK:
			break;
		case MIDIEVENT_TEMPO:
			lWriteLen = fprintf (pFile, "%ld, %ld, Tempo, %ld\n", 
				lTrackIndex + 1, lTime,	MIDIEvent_GetTempo (pMIDIEvent));
			break;
		case MIDIEVENT_SMPTEOFFSET:
			lWriteLen = fprintf (pFile, "%ld, %ld, SMPTE_offset, %ld, %ld, %ld, %ld, %ld\n", 
				lTrackIndex + 1, lTime,
				pMIDIEvent->m_pData[0], pMIDIEvent->m_pData[1], 
				pMIDIEvent->m_pData[2], pMIDIEvent->m_pData[3],
				pMIDIEvent->m_pData[4]);
			break;
		case MIDIEVENT_TIMESIGNATURE:
			lWriteLen = fprintf (pFile, "%ld, %ld, Time_signature, %ld, %ld, %ld, %ld\n", 
				lTrackIndex + 1, lTime,
				pMIDIEvent->m_pData[0], pMIDIEvent->m_pData[1], 
				pMIDIEvent->m_pData[2], pMIDIEvent->m_pData[3]);
			break;
		case MIDIEVENT_KEYSIGNATURE:
			lWriteLen = fprintf (pFile, "%ld, %ld, Key_signature, %ld, \"%s\"\n", 
				lTrackIndex + 1, lTime,
				CLIP (-7, (char)pMIDIEvent->m_pData[0], 7),
				pMIDIEvent->m_pData[1] ? "minor" : "major");
			break;
		case MIDIEVENT_SEQUENCERSPECIFIC:
			lWriteLen = fprintf (pFile, "%ld, %ld, Sequencer_specific, %ld",
				lTrackIndex + 1, lTime, pMIDIEvent->m_lLen);
			for (k = 0; k < pMIDIEvent->m_lLen; k++) {
				lWriteLen = fprintf (pFile, ", %ld", pMIDIEvent->m_pData[k]);
			}
			lWriteLen = fprintf (pFile, "\n");
			break;
		default:
			lWriteLen = fprintf (pFile, "%ld, %ld, Unknown_meta_event, %ld, %ld",
				lTrackIndex + 1, lTime, pMIDIEvent->m_lKind, pMIDIEvent->m_lLen);
			for (k = 0; k < pMIDIEvent->m_lLen; k++) {
				lWriteLen = fprintf (pFile, ", %ld", pMIDIEvent->m_pData[k]);
			}
			lWriteLen = fprintf (pFile, "\n");
			break;
		}
	}
	else if (MIDIEvent_IsMIDIEvent (pMIDIEvent)) {
		switch (pMIDIEvent->m_lKind & 0xF0) {
		case MIDIEVENT_NOTEOFF:
			lWriteLen = fprintf (pFile, "%ld, %ld, Note_off_c, %ld, %ld, %ld\n", 
				lTrackIndex + 1, lTime, pMIDIEvent->m_pData[0] & 0x0F,
				pMIDIEvent->m_pData[1], pMIDIEvent->m_pData[2]);
			break;
		case MIDIEVENT_NOTEON:
			lWriteLen = fprintf (pFile, "%ld, %ld, Note_on_c, %ld, %ld, %ld\n", 
				lTrackIndex + 1, lTime, pMIDIEvent->m_pData[0] & 0x0F,
				pMIDIEvent->m_pData[1], pMIDIEvent->m_pData[2]);
			break;
		case MIDIEVENT_KEYAFTERTOUCH:
			lWriteLen = fprintf (pFile, "%ld, %ld, Poly_aftertouch_c, %ld, %ld, %ld\n", 
				lTrackIndex + 1, lTime, pMIDIEvent->m_pData[0] & 0x0F,
				pMIDIEvent->m_pData[1], pMIDIEvent->m_pData[2]);
			break;
		case MIDIEVENT_CONTROLCHANGE:
			lWriteLen = fprintf (pFile, "%ld, %ld, Control_c, %ld, %ld, %ld\n", 
				lTrackIndex + 1, lTime, pMIDIEvent->m_pData[0] & 0x0F,
				pMIDIEvent->m_pData[1], pMIDIEvent->m_pData[2]);
			break;
		case MIDIEVENT_PROGRAMCHANGE:
			lWriteLen = fprintf (pFile, "%ld, %ld, Program_c, %ld, %ld\n", 
				lTrackIndex + 1, lTime, pMIDIEvent->m_pData[0] & 0x0F,
				pMIDIEvent->m_pData[1], pMIDIEvent->m_pData[2]);
			break;
		case MIDIEVENT_CHANNELAFTERTOUCH:
			lWriteLen = fprintf (pFile, "%ld, %ld, Channel_aftertouch_c, %ld, %ld\n", 
				lTrackIndex + 1, lTime, pMIDIEvent->m_pData[0] & 0x0F,
				pMIDIEvent->m_pData[1], pMIDIEvent->m_pData[2]);
			break;
		case MIDIEVENT_PITCHBEND:
			lWriteLen = fprintf (pFile, "%ld, %ld, Pitch_bend_c, %ld, %ld\n", 
				lTrackIndex + 1, lTime, pMIDIEvent->m_pData[0] & 0x0F,
				pMIDIEvent->m_pData[2] * 128 + pMIDIEvent->m_pData[1]);
			break;
		}
	}
	else if (MIDIEvent_IsSysExEvent (pMIDIEvent)) {
		switch (pMIDIEvent->m_lKind) {
		case MIDIEVENT_SYSEXSTART:
			lWriteLen = fprintf (pFile, "%ld, %ld, System_exclusive, %ld",
				lTrackIndex + 1, lTime, pMIDIEvent->m_lLen - 1);
			for (k = 1; k < pMIDIEvent->m_lLen; k++) {
				lWriteLen = fprintf (pFile, ", %ld", pMIDIEvent->m_pData[k]);
			}
			lWriteLen = fprintf (pFile, "\n");
			break;
		case MIDIEVENT_SYSEXCONTINUE:
			lWriteLen = fprintf (pFile, "%ld, %ld, System_exclusive_packet, %ld",
				lTrackIndex + 1, lTime, pMIDIEvent->m_lLen);
			for (k = 0; k < pMIDIEvent->m_lLen; k++) {
				lWriteLen = fprintf (pFile, ", %ld", pMIDIEvent->m_pData[k]);
			}
			lWriteLen = fprintf (pFile, "\n");
			break;
		}
	}
	return lWriteLen;
}

/* MIDIデータをMIDICSVファイル(*.csv)として保存(ANSI) */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsMIDICSVA (MIDIData* pMIDIData, const char* pszFileName) {
	long lWriteLen = 0;
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	FILE* pFile = NULL;
	long i = 0;
	long lXFVersion = MIDIData_GetXFVersion (pMIDIData);

	/* ファイルを開く */
	pFile = fopen (pszFileName, "wb"); /* 20120108 単体の\nを扱うためwtをwbに変更 */
	if (pFile == NULL) {
		return 0;
	}

	/* ヘッダーの保存 */
	lWriteLen = fprintf (pFile, "0, 0, Header, %d, %d, %d\n",
		(unsigned short)(pMIDIData->m_lFormat),
		(unsigned short)(pMIDIData->m_lNumTrack),
		(short)(pMIDIData->m_lTimeBase));
	if (lWriteLen == 0) {
		fclose (pFile);
		return 0;
	}

	/* 各トラックの保存 */
	i = 0;
	forEachTrack (pMIDIData, pMIDITrack) {
		long lLastTime = 0;
		if (pMIDITrack->m_pLastEvent) {
			lLastTime = pMIDITrack->m_pLastEvent->m_lTime;
		}
		/* トラックヘッダーの保存 */
		lWriteLen = fprintf (pFile, "%ld, 0, Start_track\n", i + 1);
		if (lWriteLen == 0) {
			fclose (pFile);
			return 0;
		}
		/* 各イベントの保存(EndofTrackを除く) */
		forEachEvent (pMIDITrack, pMIDIEvent) {
			lWriteLen = MIDIEvent_SaveAsMIDICSV (pMIDIEvent, i, pFile);
		}
		/* トラックフッターの保存 */
		lWriteLen = fprintf (pFile, "%ld, %ld, End_track\n", i + 1, lLastTime);
		if (lWriteLen == 0) {
			fclose (pFile);
			return 0;
		}
		i++;
	}

	/* フッターの保存 */
	lWriteLen = fprintf (pFile, "0, 0, End_of_file\n");
	if (lWriteLen == 0) {
		fclose (pFile);
		return 0;
	}
	fclose (pFile);
	return 1;
}

/* MIDIデータをMIDICSVファイル(*.csv)として保存(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
/* 戻り値：正常終了=1、異常終了=0。 */
long __stdcall MIDIData_SaveAsMIDICSVW (MIDIData* pMIDIData, const wchar_t* pszFileName) {
	long lWriteLen = 0;
	MIDITrack* pMIDITrack = NULL;
	MIDIEvent* pMIDIEvent = NULL;
	FILE* pFile = NULL;
	long i = 0;
	long lXFVersion = MIDIData_GetXFVersion (pMIDIData);

	/* ファイルを開く */
	pFile = _wfopen (pszFileName, L"wb"); /* 20120108 単体の\nを扱うためwtをwbに変更 */
	if (pFile == NULL) {
		return 0;
	}

	/* ヘッダーの保存 */
	lWriteLen = fprintf (pFile, "0, 0, Header, %d, %d, %d\n",
		(unsigned short)(pMIDIData->m_lFormat),
		(unsigned short)(pMIDIData->m_lNumTrack),
		(short)(pMIDIData->m_lTimeBase));
	if (lWriteLen == 0) {
		fclose (pFile);
		return 0;
	}

	/* 各トラックの保存 */
	i = 0;
	forEachTrack (pMIDIData, pMIDITrack) {
		long lLastTime = 0;
		if (pMIDITrack->m_pLastEvent) {
			lLastTime = pMIDITrack->m_pLastEvent->m_lTime;
		}
		/* トラックヘッダーの保存 */
		lWriteLen = fprintf (pFile, "%ld, 0, Start_track\n", i + 1);
		if (lWriteLen == 0) {
			fclose (pFile);
			return 0;
		}
		/* 各イベントの保存(EndofTrackを除く) */
		forEachEvent (pMIDITrack, pMIDIEvent) {
			lWriteLen = MIDIEvent_SaveAsMIDICSV (pMIDIEvent, i, pFile);
		}
		/* トラックフッターの保存 */
		lWriteLen = fprintf (pFile, "%ld, %ld, End_track\n", i + 1, lLastTime);
		if (lWriteLen == 0) {
			fclose (pFile);
			return 0;
		}
		i++;
	}

	/* フッターの保存 */
	lWriteLen = fprintf (pFile, "0, 0, End_of_file\n");
	if (lWriteLen == 0) {
		fclose (pFile);
		return 0;
	}
	fclose (pFile);
	return 1;
}



long MIDIData_ReadWRKTrackChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	MIDITrack* pMIDITrack = NULL;
	long lTrack, lTrackNameLen1, lTrackNameLen2, lChannel, lKeyPlus, lVelPlus, lPort, lFlags, lMuted;
	BYTE byTrackName1[256];
	BYTE byTrackName2[256];
	memset (byTrackName1, 0, sizeof (byTrackName1));
	memset (byTrackName2, 0, sizeof (byTrackName2));
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lTrackNameLen1 = *p++;
	strncpy (byTrackName1, p, 255);
	p += lTrackNameLen1;
	lTrackNameLen2 = *p++;
	strncpy (byTrackName2, p, 255);
	p += lTrackNameLen2;
	lChannel = (char)(*p++);
	lKeyPlus = (char)(*p++);
	lVelPlus = (char)(*p++);
	lPort = (char)(*p++);
	lFlags = *p++;
	lMuted = ((lFlags & 0x02) != 0);
	pMIDITrack = MIDITrack_Create ();
	if (pMIDITrack == NULL) {
		return 0;
	}
	pMIDITrack->m_lUser1 = lTrack;
	if (lTrackNameLen1 > 0) {
		verify (MIDITrack_InsertTrackNameA (pMIDITrack, 0, byTrackName1));
	}
	if (lTrackNameLen2 > 0) {
		verify (MIDITrack_InsertTrackNameA (pMIDITrack, 0, byTrackName2));
	}
	verify (MIDITrack_SetOutputPort (pMIDITrack, lPort));
	verify (MIDITrack_SetOutputChannel (pMIDITrack, lChannel));
	verify (MIDITrack_SetOutputOn (pMIDITrack, !lMuted));
	verify (MIDITrack_SetKeyPlus (pMIDITrack, lKeyPlus));
	verify (MIDITrack_SetVelocityPlus (pMIDITrack, lVelPlus));
	verify (MIDITrack_InsertEndofTrack (pMIDITrack, 0));
	MIDIData_AddTrack (pMIDIData, pMIDITrack);
	return 1;
}

long MIDIData_ReadWRKNewTrackChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	MIDITrack* pMIDITrack = NULL;
	long lTrack, lTrackNameLen1, lBank, lPatch, lVol, lPan, lKeyPlus, lVelPlus, lPort, lChannel, lMuted;
	BYTE byTrackName1[256];
	memset (byTrackName1, 0, sizeof (byTrackName1));
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lTrackNameLen1 = *p++;
	strncpy (byTrackName1, p, CLIP (0, lTrackNameLen1, 255));
	p += lTrackNameLen1;
	lBank = *(p) | (*(p + 1) << 8);
	p += 2;
	lPatch = *(p) | (*(p + 1) << 8);
	p += 2;
	lVol = (short)(*(p) | (*(p + 1) << 8));
	p += 2;
	lPan = (short)(*(p) | (*(p + 1) << 8));
	p += 2;
	lKeyPlus = (char)(*p++);
	lVelPlus = (char)(*p++);
	p += 7;
	lPort = *p++;
	lChannel = *p++;
	lMuted = (*p++ != 0);
	pMIDITrack = MIDITrack_Create ();
	if (pMIDITrack == NULL) {
		return 0;
	}
	pMIDITrack->m_lUser1 = lTrack;
	if (lTrackNameLen1 > 0) {
		verify (MIDITrack_InsertTrackNameA (pMIDITrack, 0, byTrackName1));
	}
	verify (MIDITrack_SetOutputPort (pMIDITrack, lPort));
	verify (MIDITrack_SetOutputChannel (pMIDITrack, lChannel));
	verify (MIDITrack_SetOutputOn (pMIDITrack, !lMuted));
	verify (MIDITrack_SetKeyPlus (pMIDITrack, lKeyPlus));
	verify (MIDITrack_SetVelocityPlus (pMIDITrack, lVelPlus));
	if (0 <= lBank && lBank <= 16383 && 0 <= lChannel && lChannel <= 15) {
		MIDITrack_InsertControlChange (pMIDITrack, 0, lChannel, 0, lBank >> 7);
		MIDITrack_InsertControlChange (pMIDITrack, 0, lChannel, 32, lBank & 0x0F);
	}
	if (0 <= lPatch && lPatch <= 127 && 0 <= lChannel && lChannel <= 15) {
		MIDITrack_InsertProgramChange (pMIDITrack, 0, lChannel, lPatch);
	}
	verify (MIDITrack_InsertEndofTrack (pMIDITrack, 0));
	MIDIData_AddTrack (pMIDIData, pMIDITrack);
	return 1;
}

long MIDIData_ReadWRKTrackNameChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lLen;
	MIDITrack* pMIDITrack = NULL;
	char szTrackName[256];
	memset (szTrackName, 0, sizeof (szTrackName));
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lLen = *p++;
	strncpy (szTrackName, p, CLIP (0, lLen, 255));
	p += lLen;
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	MIDITrack_InsertTrackName (pMIDITrack, 0, szTrackName);
	return 1;
}
long MIDIData_ReadWRKTrackBankChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lBank, lCh;
	MIDITrack* pMIDITrack = NULL;
	lTrack = *(p) | (*(p + 1) << 8);
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	p += 2;
	lBank = *(p) | (*(p + 1) << 8);
	p += 2;
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	lCh = MIDITrack_GetOutputPort (pMIDITrack);
	MIDITrack_InsertControlChange (pMIDITrack, 0, CLIP (0, lCh, 15), 0, lBank >> 7);
	MIDITrack_InsertControlChange (pMIDITrack, 0, CLIP (0, lCh, 15), 32, lBank & 0x7F);
	return 1;
}

long MIDIData_ReadWRKTrackPatchChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lPatch, lCh;
	MIDITrack* pMIDITrack = NULL;
	lTrack = *(p) | (*(p + 1) << 8);
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	p += 2;
	lPatch = *(p) | (*(p + 1) << 8);
	p += 2;
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	lCh = MIDITrack_GetOutputPort (pMIDITrack);
	MIDITrack_InsertProgramChange (pMIDITrack, 0, CLIP (0, lCh, 15), lPatch);
	return 1;
}

long MIDIData_ReadWRKTrackVolPanChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lVol, lPan, lCh;
	MIDITrack* pMIDITrack = NULL;
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lVol = *(p) | (*(p + 1) << 8);
	p += 2;
	lPan = *(p) | (*(p + 1) << 8);
	p += 2;
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	lCh = MIDITrack_GetOutputPort (pMIDITrack);
	if (0 <= lVol && lVol <= 127) {
		MIDITrack_InsertControlChange (pMIDITrack, 0, CLIP (0, lCh, 15), 7, lVol);
	}
	if (0 <= lPan && lPan <= 127) {
		MIDITrack_InsertControlChange (pMIDITrack, 0, CLIP (0, lCh, 15), 10, lPan);
	}
	return 1;
}

long MIDIData_ReadWRKVariableRecordChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	MIDITrack* pMIDITrack = NULL;
	long lDataLen = CLIP (0, lChunkLen - 32, 255);
	long lTextLen = strlen (p);
	char szName[256];
	BYTE byData[256];
	memset (szName, 0, sizeof (szName));
	memset (byData, 0, sizeof (byData));
	strncpy (szName, p, 32);
	p += 32;
	verify (pMIDITrack = MIDIData_GetFirstTrack (pMIDIData));
	if (strcmp (szName, "Title") == 0 ||
		strcmp (szName, "Subtitle") == 0) {
		strncpy (byData, p, lDataLen);
		if (strlen (byData) > 0) {
			verify (MIDITrack_InsertTrackName (pMIDITrack, 0, byData));
		}
	}
	else if (strcmp (szName, "Copyright") == 0) {
		strncpy (byData, p, lDataLen);
		if (strlen (byData) > 0) {
			verify (MIDITrack_InsertCopyrightNotice (pMIDITrack, 0, byData));
		}
	}
	else if (strcmp (szName, "Keywords") == 0 ||
		strcmp (szName, "Author") == 0 ||
		strcmp (szName, "Instructions") == 0) {
		strncpy (byData, p, lDataLen);
		if (strlen (byData) > 0) {
			verify (MIDITrack_InsertTextEvent (pMIDITrack, 0, byData));
		}
	}
	else if (strcmp (szName, "FILESTATS") == 0) {
		;
	}
	else {
		;
	}
	return 1;
}

long MIDIData_ReadWRKTimeBaseChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTimeBase = *(p) | (*(p + 1) << 8);
	p += 2;
	verify (1 <= lTimeBase && lTimeBase <= 960);
	MIDIData_SetTimeBase (pMIDIData, MIDIDATA_TPQNBASE, lTimeBase);
	return 2;
}

long MIDITrack_ReadWRKEventArray (MIDITrack* pMIDITrack, int nNumEvents, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long i, lTime, lStatus, lKind, lCh, lData1, lData2, lDur, lLen, lCode;
	BYTE byData[65536];
	for (i = 0; i < nNumEvents; i++) {
		lTime = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16);
		p += 3;
		lStatus = *p++;
		lDur = 0;
		if (lStatus >= 0x90) {
			lKind = lStatus & 0xF0;
			lCh = lStatus & 0x0F;
			lData1 = *p++;
			if (lKind == 0x90 || lKind == 0xA0 || lKind == 0xB0 || lKind == 0xE0) {
				lData2 = *p++;
			}
			if (lKind == 0x90) {
				lDur = *(p) | (*(p + 1) << 8);
				p += 2;
			}
			if (lKind == 0x90) {
				MIDITrack_InsertNote (pMIDITrack, lTime, lCh, lData1, lData2, lDur);
			}
			else if (lKind == 0xA0) {
				MIDITrack_InsertKeyAftertouch (pMIDITrack, lTime, lCh, lData1, lData2);
			}
			else if (lKind == 0xB0) {
				MIDITrack_InsertControlChange (pMIDITrack, lTime, lCh, lData1, lData2);
			}
			else if (lKind == 0xC0) {
				MIDITrack_InsertProgramChange (pMIDITrack, lTime, lCh, lData1);
			}
			else if (lKind == 0xD0) {
				MIDITrack_InsertChannelAftertouch (pMIDITrack, lTime, lCh, lData1);
			}
			else if (lKind == 0xE0) {
				long lValue = (lData2 << 7) + lData1;
				MIDITrack_InsertPitchBend (pMIDITrack, lTime, lCh, lValue);
			}
			else if (lKind == 0xF0) {
				_RPTF3 (_CRT_WARN, "lTime=%d, lStatus=%02X, lVal1=%02X\n", lTime, lStatus, lData1);
			}
			else {
				_RPTF3 (_CRT_WARN, "lTime=%d, lStatus=%02X, lVal1=%02X\n", lTime, lStatus, lData1);
			}
		}
		else if (lStatus == 5) { /* Expression */
			lCode = *(p) | (*(p + 1) << 8);
			p += 2;
			lLen =  *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
			p += 4;
		}
		else if (lStatus == 6) { /* Hairpin */
			lCode = *(p) | (*(p + 1) << 8);
			p += 2;
			lDur = *(p) | (*(p + 1) << 8);
			p += 2;
			p += 4;
		}
		else if (lStatus == 7) { /* Chord */
			lLen =  *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
			p += 4;
			memset (byData, 0, sizeof (byData));
			memcpy (byData, p, CLIP (0, lLen, 255));
			p += lLen;
			MIDITrack_InsertTextEvent (pMIDITrack, lTime, byData);
		}
		else if (lStatus == 8) { /* SysEx */
			lLen = *(p) | (*(p + 1) << 8);
			p += 2;
			memset (byData, 0, sizeof (byData));
			memcpy (byData, p, lLen);
			p += lLen;
			MIDITrack_InsertSysExEvent (pMIDITrack, lTime, byData, lLen);
		}
		else { /* Text Related */
			lLen =  *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
			p += 4;
			memset (byData, 0, sizeof (byData));
			memcpy (byData, p, CLIP (0, lLen, sizeof (byData) -1));
			p += lLen;
			MIDITrack_InsertTextEvent (pMIDITrack, lTime, byData);
		}
	}
	return 1;
}

long MIDIData_ReadWRKStreamChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	long i, lTrack, lNumEvents, lTime, lStatus, lData1, lData2, lDur, lKind, lCh;
	MIDITrack* pMIDITrack = NULL;
	BYTE* p = pChunkData;
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lNumEvents = *(p) | (*(p + 1) << 8);
	p += 2;
	for (i = 0; i < lNumEvents; i++) {
		lTime = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16);
		p += 3;
		lStatus = *p++;
		lData1 = *p++;
		lData2 = *p++;
		lDur = *(p) | (*(p + 1) << 8);
		p += 2;
		lKind = lStatus & 0xF0;
		lCh = lStatus & 0x0F;
		if (lKind == 0x90) {
			MIDITrack_InsertNote (pMIDITrack, lTime, lCh, lData1, lData2, lDur);
		}
		else if (lKind == 0xA0) {
			MIDITrack_InsertKeyAftertouch (pMIDITrack, lTime, lCh, lData1, lData2);
		}
		else if (lKind == 0xB0) {
			MIDITrack_InsertControlChange (pMIDITrack, lTime, lCh, lData1, lData2);
		}
		else if (lKind == 0xC0) {
			MIDITrack_InsertProgramChange (pMIDITrack, lTime, lCh, lData1);
		}
		else if (lKind == 0xD0) {
			MIDITrack_InsertChannelAftertouch (pMIDITrack, lTime, lCh, lData1);
		}
		else if (lKind == 0xE0) {
			long lValue = (lData2 << 7) + lData1;
			MIDITrack_InsertPitchBend (pMIDITrack, lTime, lCh, lValue);
		}
		else if (lKind == 0xF0) {
			_RPTF3 (_CRT_WARN, "lTime=%d, lStatus=%0x02X, lVal1=%02X\n", lTime, lStatus, lData1);
		}
		else {
			_RPTF3 (_CRT_WARN, "lTime=%d, lStatus=%0x02X\n lVal1=%02X\n", lTime, lStatus, lData1);
		}
	}
	return 1;
}

long MIDIData_ReadWRKCommentChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lLen;
	BYTE byText[65536];	
	MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pMIDITrack == NULL) {
		return 0;
	}
	lLen = *(p) | (*(p + 1) << 8);
	p += 2;
	memset (byText, 0, sizeof (byText));
	memcpy (byText, p, CLIP (0, lLen, 65535));
	p += lLen;
	MIDITrack_InsertTextEvent (pMIDITrack, 0, byText);
	return 1;
}

long MIDIData_ReadWRKMeterChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long i, lCount;
	MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pMIDITrack == NULL) {
		return 0;
	}
	lCount = *(p) | (*(p + 1) << 8);
	p += 2;
	for (i = 0; i < lCount; i++) {
		long lMeasure, ldd, lnn, lTime;
		p += 4;
		lMeasure = *(p) | (*(p + 1) << 8);
		p += 2;
		lnn = *p++;
		ldd = *p++;
		MIDITrack_MakeTime (pMIDITrack, lMeasure - 1, 0, 0, &lTime);
		MIDITrack_InsertTimeSignature (pMIDITrack, lTime, lnn, ldd, 24, 8);
		p += 4;
	}
	return 1;
}

long MIDIData_ReadWRKMeterKeyChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long i, lCount, lTime, lMeasure, ldd, lnn, lalt; 
	MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
	if (pMIDITrack == NULL) {
		return 0;
	}
	lCount = *(p) | (*(p + 1) << 8);
	p += 2;
	for (i = 0; i < lCount; i++) {
		lMeasure = *(p) | (*(p + 1) << 8);
		p += 2;
		lnn = *p++;
		ldd = *p++;
		lalt = *p++;
		MIDITrack_MakeTime (pMIDITrack, lMeasure - 1, 0, 0, &lTime);
		MIDITrack_InsertTimeSignature (pMIDITrack, lTime, lnn, ldd, 24, 8);
		MIDITrack_InsertKeySignature (pMIDITrack, lTime, lalt, 0);
	}
	return 1;
}

long MIDIData_ReadWRKTempoChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long i, lCount, lTime, lTempo;
	MIDITrack* pMIDITrack = NULL;
	verify (pMIDITrack = MIDIData_GetFirstTrack (pMIDIData));
	lCount = *(p) | (*(p + 1) << 8);
	p += 2;
	for (i = 0; i < lCount; i++) {
		lTime = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
		p += 4;
		p += 4;
		lTempo = *(p) | (*(p + 1) << 8); /* BPM*100の値 */
		p += 2;
		p += 8;
		lTempo = CLIP (100, lTempo, 65535);
		MIDITrack_InsertTempo (pMIDITrack, lTime, (long)((int64_t)6000000000 / (int64_t)lTempo));
	}
	return p - pChunkData;
}

long MIDIData_ReadWRKSysExChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
	long lBank, lLen, lAutoSend, lNameLen;
	lBank = *p++;
	lLen = *(p) | (*(p + 1) << 8);
	p += 2;
	lAutoSend = *p++;
	lNameLen = *p++;
	p += lNameLen;
	assert (pMIDITrack);
	MIDITrack_InsertSysExEvent (pMIDITrack, 0, p, lLen);
	p += lLen;
	return 1;
}

long MIDIData_ReadWRKSysEx2Chunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
	long lBank, lLen, lB, lPort, lAutoSend, lNameLen;
	lBank = *(p) | (*(p + 1) << 8);
	p += 2;
	lLen = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
	p += 4;
	lB = *p++;
	lPort = (lB & 0xF0) >> 4;
	lAutoSend = ((lB & 0x0F) != 0);
	lNameLen = *p++;
	p += lNameLen;
	assert (pMIDITrack);
	MIDITrack_InsertSysExEvent (pMIDITrack, 0, p, lLen);
	p += lLen;
	return 1;
}

long MIDIData_ReadWRKNewSysExChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	MIDITrack* pMIDITrack = MIDIData_GetFirstTrack (pMIDIData);
	long lBank, lLen, lPort, lAutoSend, lNameLen;
	lBank = *(p) | (*(p + 1) << 8);
	p += 2;
	lLen = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
	p += 4;
	lPort = *(p) | (*(p + 1) << 8);
	p += 2;
	lAutoSend = *p++;
	lNameLen = *p++;
	p += lNameLen;
	assert (pMIDITrack);
	verify (MIDITrack_InsertSysExEvent (pMIDITrack, 0, p, lLen));
	p += lLen;
	return 1;
}

long MIDIData_ReadWRKTrackOfsChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lOfs;
	MIDITrack* pMIDITrack = NULL;	
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lOfs = (short)(*(p) | (*(p + 1) << 8));
	p += 2;
	//verify (pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrack + 1));
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	//MIDITrack_SetTimePlus (pMIDITrack, lOfs);
	return 1;
}

long MIDIData_ReadWRKTrackRepsChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lReps, lCh;
	MIDITrack* pMIDITrack = NULL;	
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lReps = (short)(*(p) | (*(p + 1) << 8));
	p += 2;
	//verify (pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrack + 1));
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	lCh = MIDITrack_GetOutputChannel (pMIDITrack);
	MIDITrack_InsertControlChange (pMIDITrack, 0, CLIP (0, lCh, 15), 8, lReps);
	return 1;
}


long MIDIData_ReadWRK29Chunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lReps, lCh;
	MIDITrack* pMIDITrack = NULL;	
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lReps = (short)(*(p) | (*(p + 1) << 8));
	p += 2;
	//verify (pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrack + 1));
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	lCh = MIDITrack_GetOutputChannel (pMIDITrack);
	MIDITrack_InsertControlChange (pMIDITrack, 0, CLIP (0, lCh, 15), 29, lReps);
	return 1;
}


long MIDIData_ReadWRKLyricsChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lNumEvents;
	MIDITrack* pMIDITrack = NULL;
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lNumEvents = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
	p += 4;
	//verify (pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrack + 1));
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	MIDITrack_ReadWRKEventArray (pMIDITrack, lNumEvents, p, p - pChunkData);
	return 1;
}

long MIDIData_ReadWRKSegmentChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lOffset, lLen, lNumEvents;
	MIDITrack* pMIDITrack = NULL;
	BYTE bySegmentName[256];
	memset (bySegmentName, 0, sizeof (bySegmentName));
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lOffset = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
	p += 4;
	p += 8;
	lLen = *p++;
	memcpy (bySegmentName, p, CLIP (0, lLen, 255));
	p += lLen;
	p += 20;
	lNumEvents = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
	p += 4;
	//verify (pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrack + 1));
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	return MIDITrack_ReadWRKEventArray (pMIDITrack, lNumEvents, p, p - pChunkData);
}

long MIDIData_ReadWRKNewStreamChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long lTrack, lLen, lNumEvents;
	MIDITrack* pMIDITrack = NULL;
	BYTE bySegmentName[256];
	memset (bySegmentName, 0, sizeof (bySegmentName));
	lTrack = *(p) | (*(p + 1) << 8);
	p += 2;
	lLen = *p++;
	memcpy (bySegmentName, p, CLIP (0, lLen, 255));
	p += lLen;
	lNumEvents = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
	p += 4;
	//verify (pMIDITrack = MIDIData_GetTrack (pMIDIData, lTrack + 1));
	verify (pMIDITrack = MIDIData_GetLastTrack (pMIDIData));
	return MIDITrack_ReadWRKEventArray (pMIDITrack, lNumEvents, p, p - pChunkData);
}
long MIDIData_ReadWRKMarkerChunk (MIDIData* pMIDIData, BYTE* pChunkData, long lChunkLen) {
	BYTE* p = pChunkData;
	long i, lCount, lTime, lLen;
	MIDITrack* pMIDITrack = NULL;
	BYTE byText[256];
	memset (byText, 0, sizeof (byText));
	verify (pMIDITrack = MIDIData_GetFirstTrack (pMIDIData));
	lCount = *(p) | (*(p + 1) << 8);
	p += 2;
	p += 2;
	for (i = 0; i < lCount; i++) {
		p += 2;
		lTime = *(p) | (*(p + 1) << 8) | (*(p + 2) << 16);
		CLIP (0, lTime, 0x00FFFFFF);
		p += 3;
		p += 5;
		lLen = *p++;
		memset (byText, 0, sizeof (byText));
		memcpy (byText, p, CLIP (0, lLen, 255));
		p += lLen;
		MIDITrack_InsertMarker (pMIDITrack, lTime, byText);
	}
	return 1;
}

long MIDIData_ReadWRKChunk (MIDIData* pMIDIData, long lChunkType, BYTE* pChunkData, long lChunkLen) {
	switch (lChunkType) {
	case 1: /* TRACK_CHUNK */
		_RPTF2 (_CRT_WARN, "%d:WRKTRACKCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTrackChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 2: /* STREAM_CHUNK */
		_RPTF2 (_CRT_WARN, "%d:WRKSTREAMCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKStreamChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 3: /* VARS_CHUNK */
		_RPTF2 (_CRT_WARN, "%d:WRKVARSCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 4: /* TEMPO */
		MIDIData_ReadWRKTempoChunk (pMIDIData, pChunkData, lChunkLen);
		_RPTF2 (_CRT_WARN, "%d:WRKTEMPOCHUNK(%dbytes)", lChunkType, lChunkLen);
		break;
	case 5: /* METER */
		_RPTF2 (_CRT_WARN, "%d:WRKMETERCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKMeterChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 6: /* SYSEX */
		MIDIData_ReadWRKSysExChunk (pMIDIData, pChunkData, lChunkLen);
		_RPTF2 (_CRT_WARN, "%d:WRKSYSEXCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 7: /* MEMRGN */
		_RPTF2 (_CRT_WARN, "%d:WRKMEMRGNCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 8: /* COMMENT */
		_RPTF2 (_CRT_WARN, "%d:WRKCOMMENTCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKCommentChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 9: /* TRACKOFS */
		_RPTF2 (_CRT_WARN, "%d:WRKTRACKOFSCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTrackOfsChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 10: /* TIMEBASE */
		_RPTF2 (_CRT_WARN, "%d:WRKTIMEBASECHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTimeBaseChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 11: /* TIMEFMT */
		_RPTF2 (_CRT_WARN, "%d:WRKTIMEFMTCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 12: /* TRKREPS */
		_RPTF2 (_CRT_WARN, "%d:WRKTRKREPSCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTrackRepsChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 14: /* TRACKPATCH */
		_RPTF2 (_CRT_WARN, "%d:WRKTRACKPATCHCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTrackPatchChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 15: /* NEWTEMPO */
		_RPTF2 (_CRT_WARN, "%d:WRKNEWTEMPOCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTempoChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 16: /* THRU */
		_RPTF2 (_CRT_WARN, "%d:WRKTHRUCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 18: /* LYRICS */
		_RPTF2 (_CRT_WARN, "%d:WRKLYRICSCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKLyricsChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 19: /* TRACKVOLPAN */
		_RPTF2 (_CRT_WARN, "%d:WRKTRACKVOLPANCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTrackVolPanChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 20: /* SYSEX2 */
		_RPTF2 (_CRT_WARN, "%d:WRKSYSEX2CHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKSysEx2Chunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 21: /* MARKER */
		_RPTF2 (_CRT_WARN, "%d:WRKMARKERCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKMarkerChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 22: /* STRTAB */
		_RPTF2 (_CRT_WARN, "%d:WRKSTRTABCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 23: /* METERKEY */
		_RPTF2 (_CRT_WARN, "%d:WRKMETERKEYCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKMeterKeyChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 24: /* TRACKNAME */
		_RPTF2 (_CRT_WARN, "%d:WRKTRACKNAMECHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTrackNameChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 26: /* VARIABLE */
		_RPTF2 (_CRT_WARN, "%d:WRKVARIABLETABLECHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKVariableRecordChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 27: /* NEWTRACKOFS */
		_RPTF2 (_CRT_WARN, "%d:WRKNEWTRACKOFSCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 30: /* TRACKBANK */
		_RPTF2 (_CRT_WARN, "%d:WRKTRACKBANKCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKTrackBankChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 36: /* NEWTRACK */
		MIDIData_ReadWRKNewTrackChunk (pMIDIData, pChunkData, lChunkLen);
		_RPTF2 (_CRT_WARN, "%d:WRKNEWTRACKCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 44: /* NEWSYSEX */
		MIDIData_ReadWRKNewSysExChunk (pMIDIData, pChunkData, lChunkLen);
		_RPTF2 (_CRT_WARN, "%d:WRKNEWSYSEXCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 45: /* NEWSTREAM */
		MIDIData_ReadWRKNewStreamChunk (pMIDIData, pChunkData, lChunkLen);
		_RPTF2 (_CRT_WARN, "%d:WRKNEWSTREAMCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	case 49: /* SEGMENT */
		_RPTF2 (_CRT_WARN, "%d:WRKSEGMENTCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		MIDIData_ReadWRKSegmentChunk (pMIDIData, pChunkData, lChunkLen);
		break;
	case 74: /* SOFTVER */
		_RPTF2 (_CRT_WARN, "%d:WRKSOFTBERCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	default:
		_RPTF2 (_CRT_WARN, "%d:WRKUNKNOWNCCHUNK(%dbytes)\n", lChunkType, lChunkLen);
		break;
	}
	return 1;
}

long MIDIData_PostReadWRK (MIDIData* pMIDIData) {
	long lNumTrack = MIDIData_CountTrack (pMIDIData);
	MIDITrack* pMIDITrack = NULL;
	/* MIDIトラックのバブルソート */
	MIDITrack** ppTrackArray = (MIDITrack**)calloc (lNumTrack, sizeof (MIDITrack*));
	if (ppTrackArray) {
		long i, j;
		for (i = lNumTrack - 1; i >= 1; i--) {
			pMIDITrack = MIDIData_GetLastTrack (pMIDIData);
			MIDIData_RemoveTrack (pMIDIData, pMIDITrack);
			*(ppTrackArray + i) = pMIDITrack;
		}
		for (i = 1; i < lNumTrack; i++) {
			for (j = 1; j < lNumTrack - 1; j++) {
				MIDITrack* pMIDITrack1 = *(ppTrackArray + j);
				MIDITrack* pMIDITrack2 = *(ppTrackArray + j + 1);
				if (pMIDITrack1->m_lUser1 > pMIDITrack2->m_lUser1) {
					*(ppTrackArray + j) = pMIDITrack2;
					*(ppTrackArray + j + 1) = pMIDITrack1;
				}
			}
		}
		for (i = 1; i < lNumTrack; i++) {
			pMIDITrack = *(ppTrackArray + i);
			MIDIData_AddTrack (pMIDIData, pMIDITrack);
			pMIDITrack->m_lUser1 = 0;
		}
		free (ppTrackArray);
	}
	lNumTrack = MIDIData_CountTrack (pMIDIData);
	/* 各トラックのステータス整理 */
	forEachTrack (pMIDIData, pMIDITrack) {
		/* EOTがない場合、EOTを挿入 */
		MIDIEvent* pMIDIEvent = MIDITrack_GetLastEvent (pMIDITrack);
		if (!MIDIEvent_IsEndofTrack (pMIDIEvent)) {
			MIDITrack_InsertEndofTrack (pMIDITrack, MIDIEvent_GetTime (pMIDIEvent));
		}
		/* 出力チャンネル・入力チャンネルの設定 */
		if (pMIDITrack->m_lTempIndex == 0) {
			MIDITrack_SetOutputChannel (pMIDITrack, -1);
		}
		MIDITrack_SetInputChannel  (pMIDITrack, MIDITrack_GetOutputChannel (pMIDITrack));
	}
	return 1;
}

/* MIDIデータを旧Cakewalkシーケンスファイル(*.wrk)から読み込み(ANSI) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromWRKA (const char* pszFileName) {
	FILE* pFile = NULL;
	BYTE byData[2048];
	MIDIData* pMIDIData = NULL;
	BYTE byChunkType = 0;
	long lChunkLen = 0;
	BYTE* pChunkData = NULL;

	/* ファイルを開く */
	pFile = fopen (pszFileName, "rb");
	if (pFile == NULL) {
		return NULL;
	}

	/* 11バイト読み込み */
	if (fread (byData, 1, 11, pFile) < 11) {
		fclose (pFile);
		return NULL;
	}

	if (strncmp (byData, "CAKEWALK", 8) != 0) {
		fclose (pFile);
		return NULL;
	}

	_RPTF2 (_CRT_WARN, "CAKEWALK%d.%d Recognized.\n", byData[10], byData[9]);

	pMIDIData = MIDIData_Create (1, 1, MIDIDATA_TPQNBASE, 120);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}

	while (1) {
		if (fread (&byChunkType, 1, 1, pFile) < 1) {
			fclose (pFile);
			break;
		}
		if (byChunkType == 255) { /* EndofChunk */
			break;
		}
		if (fread (&lChunkLen, 4, 1, pFile) < 1) {
			fclose (pFile);
			break;
		}
		pChunkData = malloc (lChunkLen);
		if (pChunkData == NULL) {
			fclose (pFile);
			break;
		}
		memset (pChunkData, 0, lChunkLen);
		if (fread (pChunkData, 1, lChunkLen, pFile) < lChunkLen) {
			free (pChunkData);
			pChunkData = NULL;
			break;
		}
		if (MIDIData_ReadWRKChunk (pMIDIData, (long)byChunkType, pChunkData, lChunkLen) == 0) {
			free (pChunkData);
			pChunkData = NULL;
			break;
		}
		free (pChunkData);
		pChunkData = NULL;
	}
	fclose (pFile);
	MIDIData_PostReadWRK (pMIDIData);
	return pMIDIData;
}

/* MIDIデータを旧Cakewalkシーケンスファイル(*.wrk)から読み込み(UNICODE) */
/* ファイル名はUNICODEで与えるがファイルの中身は仕様に基づきANSIである。 */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromWRKW (const wchar_t* pszFileName) {
	FILE* pFile = NULL;
	BYTE byData[2048];
	MIDIData* pMIDIData = NULL;
	BYTE byChunkType = 0;
	long lChunkLen = 0;
	BYTE* pChunkData = NULL;

	/* ファイルを開く */
	pFile = _wfopen (pszFileName, L"rb");
	if (pFile == NULL) {
		return NULL;
	}

	/* 11バイト読み込み */
	if (fread (byData, 1, 11, pFile) < 11) {
		fclose (pFile);
		return NULL;
	}

	if (strncmp (byData, "CAKEWALK", 8) != 0) {
		fclose (pFile);
		return NULL;
	}

	_RPTF2 (_CRT_WARN, "CAKEWALK%d.%d Recognized.\n", byData[10], byData[9]);

	pMIDIData = MIDIData_Create (1, 1, MIDIDATA_TPQNBASE, 120);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}

	while (1) {
		if (fread (&byChunkType, 1, 1, pFile) < 1) {
			fclose (pFile);
			break;
		}
		if (byChunkType == 255) { /* EndofChunk */
			break;
		}
		if (fread (&lChunkLen, 4, 1, pFile) < 1) {
			fclose (pFile);
			break;
		}
		pChunkData = malloc (lChunkLen);
		if (pChunkData == NULL) {
			fclose (pFile);
			break;
		}
		memset (pChunkData, 0, lChunkLen);
		if (fread (pChunkData, 1, lChunkLen, pFile) < lChunkLen) {
			free (pChunkData);
			pChunkData = NULL;
			break;
		}
		if (MIDIData_ReadWRKChunk (pMIDIData, (long)byChunkType, pChunkData, lChunkLen) == 0) {
			free (pChunkData);
			pChunkData = NULL;
			break;
		}
		free (pChunkData);
		pChunkData = NULL;
	}
	fclose (pFile);
	MIDIData_PostReadWRK (pMIDIData);
	return pMIDIData;
}










/* マビノギMML文字列中の数値を読み取る(Unicodeのみ) */
long ReadMabiLongW (const wchar_t* pszText, long* pValue) {
	long lTextLen = 0;
	long lLen = 0;
	wchar_t szBuf[256];
	wchar_t* p1 = pszText;
	wchar_t* p2 = &szBuf[0];
	memset (szBuf, 0, 256);
	lTextLen = wcslen (pszText);
	while (*p1 && lLen < 255) {
		if (L'0' <= *p1 && *p1 <= L'9') {
			*p2++ = *p1++;
			lLen++;
		}
		else {
			break;	
		}
	}
	if (lLen > 0) {
		*pValue = _wtol (szBuf);
		return lLen;
	}
	*pValue = 0;
	return 0;
}

/* マビノギMML文字列からMIDIデータへの読み込み(Unicode) */
long MIDIData_ReadMabiMMLW (MIDIData* pMIDIData, const wchar_t* pszText) {
	long lCurTime = 0;
	long lCurTempo = 120;
	long lCurOctave = 4;
	long lCurVolume = 15;
	long lCurLength = 4;
	long lCurCh = 1;
	long lCurNote = 60;
	long lCurDuration = 1;
	long lCurVelocity = 1;
	long lCurTieMode = 0; 
	long lTempLength = 0;
	long lTempDuration = 0;
	MIDITrack* pCurTrack = pMIDIData->m_pLastTrack;
	MIDIEvent* pLastNoteEvent = NULL;
	long lTimeMode = 0;
	long lResolution = 0;
	long lRet = 0;
	long lValue = 0;
	wchar_t* p = pszText;

	MIDIData_GetTimeBase (pMIDIData, &lTimeMode, &lResolution);
	lCurDuration = lResolution * 4 / lCurLength;

	while (*p) {
		switch (*p) {
		/* 各音符(ドレミファソラシ) */
		case L'C':
		case L'c':
		case L'D':
		case L'd':
		case L'E':
		case L'e':
		case L'F':
		case L'f':
		case L'G':
		case L'g':
		case L'A':
		case L'a':
		case L'B':
		case L'b':
			switch (*p) {
			case L'C':
			case L'c':
				lCurNote = (lCurOctave + 1) * 12 + 0;
				break;
			case L'D':
			case L'd':
				lCurNote = (lCurOctave + 1) * 12 + 2;
				break;
			case L'E':
			case L'e':
				lCurNote = (lCurOctave + 1) * 12 + 4;
				break;
			case L'F':
			case L'f':
				lCurNote = (lCurOctave + 1) * 12 + 5;
				break;
			case L'G':
			case L'g':
				lCurNote = (lCurOctave + 1) * 12 + 7;
				break;
			case L'A':
			case L'a':
				lCurNote = (lCurOctave + 1) * 12 + 9;
				break;
			case L'B':
			case L'b':
				lCurNote = (lCurOctave + 1) * 12 + 11;
				break;
			}
			p++;
			if (*p == L'+') {
				lCurNote++;
				p++;
			}
			else if (*p == L'-') {
				lCurNote--;
				p++;
			}
			lRet = ReadMabiLongW (p, &lValue);
			if (lRet > 0) {
				lTempLength = CLIP (1, lValue, 128);
				lTempDuration = lResolution * 4 / lTempLength;
			}
			else {
				lTempLength = lCurLength;
				lTempDuration = lCurDuration;
			}
			p += lRet;
			if (*p == L'.') {
				lTempDuration = lTempDuration * 3 / 2;
				p++;
			}
			if (lCurTieMode == 1 && pLastNoteEvent != NULL && MIDIEvent_GetParent (pLastNoteEvent) == pCurTrack) {
				long lLastDuration = MIDIEvent_GetDuration (pLastNoteEvent);
				MIDIEvent_SetDuration (pLastNoteEvent, lLastDuration + lTempDuration);
			}
			else {
				pLastNoteEvent = MIDIEvent_CreateNote (lCurTime, lCurCh, lCurNote, lCurVelocity, lTempDuration);
				MIDITrack_InsertEvent (pCurTrack, pLastNoteEvent);
			}
			lCurTime += lTempDuration;
			lCurTieMode = 0;
			break;
		/* テンポ指定 */
		case L'T':
		case L't':
			p++;
			lRet = ReadMabiLongW (p, &lValue);
			if (lRet > 0) {
				lCurTempo = CLIP (1, lValue, 255);
			}
			p += lRet;
			MIDITrack_InsertTempo (pMIDIData->m_pFirstTrack, lCurTime, 60000000 / lCurTempo);
			break;
		/* ベロシティ指定 */
		case L'V':
		case L'v':
			p++;
			lRet = ReadMabiLongW (p, &lValue);
			if (lRet > 0) {
				lCurVolume = CLIP (0, lValue, 15);
				lCurVelocity = CLIP (1, lCurVolume * 8, 127);
			}
			p += lRet;
			break;
		/* オクターブ指定 */
		case L'O':
		case L'o':
			p++;
			lRet = ReadMabiLongW (p, &lValue);
			if (lRet > 0) {
				lCurOctave = CLIP (1, lValue, 8);
			}
			p += lRet;
			break;
		/* 音符長さ指定 */
		case L'L':
		case L'l':
			p++;
			lRet = ReadMabiLongW (p, &lValue);
			if (lRet > 0) {
				lCurLength = CLIP (1, lValue, 64);
				lCurDuration = lResolution * 4 / lCurLength;
			}
			p += lRet;
			if (*p == L'.') {
				lCurDuration = lCurDuration * 3 / 2;
				p++;
			}
			break;
		/* オクターブアップ */
		case L'>':
			lCurOctave++;
			p++;
			break;
		/* オクターブダウン */
		case L'<':
			lCurOctave--;
			p++;
			break;
		/* ノート番号指定 */
		case L'N':
		case L'n':
			p++;
			lRet = ReadMabiLongW (p, &lValue);
			if (lRet > 0) {
				if (lValue != lCurNote) {
					lCurTieMode = 0;
				}
				lCurNote = CLIP (0, lValue, 127);
			}
			p += lRet;
			if (lCurTieMode == 1 && pLastNoteEvent != NULL && MIDIEvent_GetParent (pLastNoteEvent) == pCurTrack) {
				long lLastDuration = MIDIEvent_GetDuration (pLastNoteEvent);
				MIDIEvent_SetDuration (pLastNoteEvent, lLastDuration + lCurDuration);
			}
			else {
				pLastNoteEvent = MIDIEvent_CreateNote (lCurTime, lCurCh, lCurNote, lCurVelocity, lCurDuration);
				MIDITrack_InsertEvent (pCurTrack, pLastNoteEvent);
			}
			lCurTime += lCurDuration;
			lCurTieMode = 0;
			break;
		/* タイ */
		case L'&':
			p++;
			if (pLastNoteEvent != NULL) {
				lCurTieMode = 1;
			}
			break;
		/* 休符 */
		case L'R':
		case L'r':
			p++;
			lRet = ReadMabiLongW (p, &lValue);
			if (lRet > 0) {
				lTempLength = CLIP (1, lValue, 128);
				lTempDuration = lResolution * 4 / lTempLength;
			}
			else {
				lTempLength = lCurLength;
				lTempDuration = lCurDuration;
			}
			p += lRet;
			if (*p == L'.') {
				lTempDuration = lTempDuration * 3 / 2;
				p++;
			}
			pLastNoteEvent = NULL;
			lCurTime += lTempDuration;
			lCurTieMode = 0;
			break;
		/* 新しいトラック */
		case L',':
			p++;
			pCurTrack = MIDITrack_Create ();
			if (pCurTrack == NULL) {
				return (p - pszText);
			}
			MIDIData_AddTrack (pMIDIData, pCurTrack);
			lCurTime = 0;
			lCurLength = 4;
			lCurDuration = lResolution * 4 / lCurLength;
			lCurOctave = 4;
			lCurTieMode = 0;
			pLastNoteEvent = NULL;
			break;
		/* 終了 */
		case L';':
			p++;
			return (p - pszText);
		default :
			p++;
			break;
		}
	}
	return (p - pszText);
}

/* マビノギMML文字列からMIDIデータへの読み込み(ANSI) */
/* この関数は、内部的にはUnicodeに変換し、Unicode用の関数で処理する。 */
long MIDIData_ReadMabiMMLA (MIDIData* pMIDIData, const char* pszText) {
	wchar_t* pszText2 = NULL;
	long lTextLen = 0;
	long lTextLen2 = 0;
	long lResult = 0;

	lTextLen = strlen (pszText);
	pszText2 = malloc ((lTextLen + 1) * sizeof (wchar_t));
	if (pszText2 == NULL) {
		return 0;
	}
	memset (pszText2, 0, (lTextLen + 1) * sizeof (wchar_t));
	mbstowcs (pszText2, pszText, lTextLen * sizeof (wchar_t));
	lTextLen2 = wcslen (pszText2);

	lResult = MIDIData_ReadMabiMMLW (pMIDIData, pszText2);
	free (pszText2);
	return lResult;
}

/* マビノギMMLファイル読み込み後の処理 */
long MIDIData_PostReadMabiMML (MIDIData* pMIDIData) {
	MIDITrack* pMIDITrack = NULL;
	/* EndofTrackの挿入 */
	forEachTrack (pMIDIData, pMIDITrack) {
		MIDIEvent* pLastEvent = MIDITrack_GetLastEvent (pMIDITrack);
		if (pLastEvent != NULL && !MIDIEvent_IsEndofTrack (pLastEvent)) {
			MIDITrack_InsertEndofTrack (pMIDITrack, pLastEvent->m_lTime);
		}
		else if (pLastEvent == NULL) {
			MIDITrack_InsertEndofTrack (pMIDITrack, 0);
		}
	}
	return 1;
}

/* MIDIデータをMabinogiMML(*.mmml)から読み込み(ANSI) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromMabiMMLA (const char* pszFileName) {
	FILE* pFile = NULL;
	BYTE byData[2048];
	MIDIData* pMIDIData = NULL;
	long lTextLen = 0;
	BYTE* pszText = NULL;

	/* ファイルを開く */
	pFile = fopen (pszFileName, "rb");
	if (pFile == NULL) {
		return NULL;
	}

	/* 4バイト読み込み */
	if (fread (byData, 1, 4, pFile) < 4) {
		fclose (pFile);
		return NULL;
	}

	/* MML@の読み込み(なくても可) */
	if (strncmp (byData, "MML@", 4) != 0) {
		_RPTF0 (_CRT_WARN, "MML@ Omitted.\n");
		fseek (pFile, 0L, SEEK_END);
		lTextLen = ftell (pFile);
		fseek (pFile, 0L, SEEK_SET);
	}
	else {
		_RPTF0 (_CRT_WARN, "MML@ Recognized.\n");
		fseek (pFile, 0L, SEEK_END);
		lTextLen = ftell (pFile) - 4;
		fseek (pFile, 4L, SEEK_SET);
	}
	
	pMIDIData = MIDIData_Create (1, 2, MIDIDATA_TPQNBASE, 96);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}

	pszText = malloc (lTextLen + 1);
	if (pszText == NULL) {
		MIDIData_Delete (pMIDIData);
		fclose (pFile);
		return NULL;
	}
	memset (pszText, 0, lTextLen + 1);
	fread (pszText, 1, lTextLen, pFile);
	fclose (pFile);

	MIDIData_ReadMabiMMLA (pMIDIData, pszText);

	free (pszText);

	MIDIData_PostReadMabiMML (pMIDIData);

	return pMIDIData;
}

/* MIDIデータをMabinogiMML(*.mmml)から読み込み(ANSI) */
/* 新しいMIDIデータへのポインタを返す(失敗時NULL) */
MIDIData* __stdcall MIDIData_LoadFromMabiMMLW (const wchar_t* pszFileName) {
	FILE* pFile = NULL;
	BYTE byData[2048];
	MIDIData* pMIDIData = NULL;
	long lTextLen = 0;
	BYTE* pszText = NULL;

	/* ファイルを開く */
	pFile = _wfopen (pszFileName, L"rb");
	if (pFile == NULL) {
		return NULL;
	}

	/* 4バイト読み込み */
	if (fread (byData, 1, 4, pFile) < 4) {
		fclose (pFile);
		return NULL;
	}

	/* MML@の読み込み(なくても可) */
	if (strncmp (byData, "MML@", 4) != 0) {
		_RPTF0 (_CRT_WARN, "MML@ Omitted.\n");
		fseek (pFile, 0L, SEEK_END);
		lTextLen = ftell (pFile);
		fseek (pFile, 0L, SEEK_SET);
	}
	else {
		_RPTF0 (_CRT_WARN, "MML@ Recognized.\n");
		fseek (pFile, 0L, SEEK_END);
		lTextLen = ftell (pFile) - 4;
		fseek (pFile, 4L, SEEK_SET);
	}
	
	pMIDIData = MIDIData_Create (1, 2, MIDIDATA_TPQNBASE, 96);
	if (pMIDIData == NULL) {
		fclose (pFile);
		return NULL;
	}

	pszText = malloc (lTextLen + 1);
	if (pszText == NULL) {
		MIDIData_Delete (pMIDIData);
		fclose (pFile);
		return NULL;
	}
	memset (pszText, 0, lTextLen + 1);
	fread (pszText, 1, lTextLen, pFile);
	fclose (pFile);

	MIDIData_ReadMabiMMLA (pMIDIData, pszText);

	free (pszText);

	MIDIData_PostReadMabiMML (pMIDIData);

	return pMIDIData;
}

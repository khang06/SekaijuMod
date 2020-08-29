/******************************************************************************/
/*                                                                            */
/*　MIDIInstrument.c - MIDIInstrumentソースファイル        (C)2007-2009 くず  */
/*                                                                            */
/******************************************************************************/

/* このモジュールは普通のC言語で書かれている。 */
/* このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布される。 */
/* プロジェクトホームページ："http://openmidiproject.sourceforge.jp/index.html" */

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef _WINDOWS
#include <crtdbg.h>
#endif
#include "MIDIInstrument.h"

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

/******************************************************************************/
/*                                                                            */
/*　MIDIPatchNameTableクラス関数群                                            */
/*                                                                            */
/******************************************************************************/

/* MIDIPatchNameTableオブジェクトの生成 */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_Create () {
	int i;
	MIDIPatchNameTable* pMIDIPatchNameTable = calloc (sizeof(MIDIPatchNameTable), 1);
	if (pMIDIPatchNameTable == NULL) {
		return NULL;
	}
	pMIDIPatchNameTable->m_pszTitle = NULL;
	pMIDIPatchNameTable->m_lTitleLen = 0;
	for (i = 0; i < 128; i++) {
		pMIDIPatchNameTable->m_pszName[i] = NULL;
		pMIDIPatchNameTable->m_lNameLen[i] = 0;
	}
	pMIDIPatchNameTable->m_pNextPatchNameTable = NULL;
	pMIDIPatchNameTable->m_pPrevPatchNameTable = NULL;
	pMIDIPatchNameTable->m_pParent = NULL;
	return pMIDIPatchNameTable;
}

/* MIDIPatchNameTableオブジェクトの削除 */
void __stdcall MIDIPatchNameTable_Delete (MIDIPatchNameTable* pMIDIPatchNameTable) {
	int i;
	/* このオブジェクトへのBasedOnによる参照があればその参照を解除 */
	if (pMIDIPatchNameTable->m_pParent) {
		MIDIInstrument* pParentInstrument = (MIDIInstrument*)(pMIDIPatchNameTable->m_pParent);
		MIDIPatchNameTable* pPatchNameTable;
		forEachPatchNameTable (pParentInstrument, pPatchNameTable) {
			if (pPatchNameTable->m_pBasedOn == pMIDIPatchNameTable) {
				pPatchNameTable->m_pBasedOn = NULL;
			}
		}
	}
	/* タイトルの削除 */
	free (pMIDIPatchNameTable->m_pszTitle);
	pMIDIPatchNameTable->m_pszTitle = NULL;
	pMIDIPatchNameTable->m_lTitleLen = 0;
	/* 名前配列の全削除 */
	for (i = 0; i < 128; i++) {
		free (pMIDIPatchNameTable->m_pszName[i]);
		pMIDIPatchNameTable->m_pszName[i] = NULL;
		pMIDIPatchNameTable->m_lNameLen[i] = 0;
	}
	/* 双方向リンクリストのつなぎ替え */
	if (pMIDIPatchNameTable->m_pParent) {
		MIDIInstrument_RemovePatchNameTable 
			(pMIDIPatchNameTable->m_pParent, pMIDIPatchNameTable);
	}
	/* このオブジェクトの削除 */
	free (pMIDIPatchNameTable);
}

/* MIDIPatchNameTableオブジェクトのタイトルを設定(ANSI) */
long __stdcall MIDIPatchNameTable_SetTitleA
(MIDIPatchNameTable* pMIDIPatchNameTable, const char* pszTitle) {
	assert (pMIDIPatchNameTable);
	if (pMIDIPatchNameTable->m_pszTitle) {
		free (pMIDIPatchNameTable->m_pszTitle);
		pMIDIPatchNameTable->m_pszTitle = NULL;
		pMIDIPatchNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = strlen (pszTitle) + 1;
		pMIDIPatchNameTable->m_pszTitle = malloc (lLen);
		if (pMIDIPatchNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIPatchNameTable->m_pszTitle, 0, lLen);
		strcpy (pMIDIPatchNameTable->m_pszTitle, pszTitle);
		pMIDIPatchNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDIPatchNameTableオブジェクトのタイトルを設定(UNICODE) */
long __stdcall MIDIPatchNameTable_SetTitleW
(MIDIPatchNameTable* pMIDIPatchNameTable, const wchar_t* pszTitle) {
	assert (pMIDIPatchNameTable);
	if (pMIDIPatchNameTable->m_pszTitle) {
		free (pMIDIPatchNameTable->m_pszTitle);
		pMIDIPatchNameTable->m_pszTitle = NULL;
		pMIDIPatchNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = wcslen (pszTitle) + 1;
		pMIDIPatchNameTable->m_pszTitle = malloc (lLen * sizeof (wchar_t));
		if (pMIDIPatchNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIPatchNameTable->m_pszTitle, 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDIPatchNameTable->m_pszTitle, pszTitle);
		pMIDIPatchNameTable->m_lTitleLen = lLen;
	}
	return 1;
}


/* MIDIPatchNameTableオブジェクトの各名前を設定(ANSI) */
long __stdcall MIDIPatchNameTable_SetNameA 
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, const char* pszName) {
	long lLen;
	assert (pMIDIPatchNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIPatchNameTable->m_pszName[lNumber]) {
		free (pMIDIPatchNameTable->m_pszName[lNumber]);
		pMIDIPatchNameTable->m_pszName[lNumber] = NULL;
		pMIDIPatchNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = strlen (pszName) + 1;
		pMIDIPatchNameTable->m_pszName[lNumber] = malloc (lLen);
		if (pMIDIPatchNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDIPatchNameTable->m_pszName[lNumber], 0, lLen);
		strcpy (pMIDIPatchNameTable->m_pszName[lNumber], pszName);
		pMIDIPatchNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDIPatchNameTableオブジェクトの各名前を設定(UNICODE) */
long __stdcall MIDIPatchNameTable_SetNameW
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, const wchar_t* pszName) {
	long lLen;
	assert (pMIDIPatchNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIPatchNameTable->m_pszName[lNumber]) {
		free (pMIDIPatchNameTable->m_pszName[lNumber]);
		pMIDIPatchNameTable->m_pszName[lNumber] = NULL;
		pMIDIPatchNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = wcslen (pszName) + 1;
		pMIDIPatchNameTable->m_pszName[lNumber] = malloc (lLen * sizeof (wchar_t));
		if (pMIDIPatchNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDIPatchNameTable->m_pszName[lNumber], 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDIPatchNameTable->m_pszName[lNumber], pszName);
		pMIDIPatchNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDIPatchNameTableオブジェクトのタイトルを取得(ANSI) */
long __stdcall MIDIPatchNameTable_GetTitleA 
(MIDIPatchNameTable* pMIDIPatchNameTable, char* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIPatchNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIPatchNameTable->m_lTitleLen);
		strncpy (pszTitle, pMIDIPatchNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = '\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = '\0';
		return 0;
	}
}

/* MIDIPatchNameTableオブジェクトのタイトルを取得(UNICODE) */
long __stdcall MIDIPatchNameTable_GetTitleW 
(MIDIPatchNameTable* pMIDIPatchNameTable, wchar_t* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIPatchNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIPatchNameTable->m_lTitleLen);
		wcsncpy (pszTitle, pMIDIPatchNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = L'\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = L'\0';
		return 0;
	}
}

/* MIDIPatchNameTableオブジェクトの各名前を取得(ANSI) */
long __stdcall MIDIPatchNameTable_GetNameA 
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, char* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 128);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIPatchNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDIPatchNameTable->m_lNameLen[lNumber]);
		strncpy (pszName, pMIDIPatchNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = '\0';
		return lLenMin - 1;
	}
	else if (pMIDIPatchNameTable->m_pBasedOn) {
		MIDIPatchNameTable* pBasedOn = (MIDIPatchNameTable*)(pMIDIPatchNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			strncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = '\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = '\0';
			return 0;
		}
	}
	else {
		*(pszName) = '\0';
		return 0;
	}
}

/* MIDIPatchNameTableオブジェクトの各名前を取得(UNICODE) */
long __stdcall MIDIPatchNameTable_GetNameW 
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, wchar_t* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 128);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIPatchNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDIPatchNameTable->m_lNameLen[lNumber]);
		wcsncpy (pszName, pMIDIPatchNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = L'\0';
		return lLenMin - 1;
	}
	else if (pMIDIPatchNameTable->m_pBasedOn) {
		MIDIPatchNameTable* pBasedOn = (MIDIPatchNameTable*)(pMIDIPatchNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			wcsncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = L'\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = L'\0';
			return 0;
		}
	}
	else {
		*(pszName) = L'\0';
		return 0;
	}
}

/* 次のMIDIPatchNameTableへのポインタを取得(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_GetNextPatchNameTable 
(MIDIPatchNameTable* pMIDIPatchNameTable) {
	return pMIDIPatchNameTable->m_pNextPatchNameTable;
}

/* 前のMIDIPatchNameTableへのポインタを取得(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_GetPrevPatchNameTable 
(MIDIPatchNameTable* pMIDIPatchNameTable) {
	return pMIDIPatchNameTable->m_pPrevPatchNameTable;
}

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIPatchNameTable_GetParent
(MIDIPatchNameTable* pMIDIPatchNameTable) {
	return pMIDIPatchNameTable->m_pParent;
}








/******************************************************************************/
/*                                                                            */
/*　MIDINoteNameTableクラス関数群                                             */
/*                                                                            */
/******************************************************************************/

/* MIDINoteNameTableオブジェクトの生成 */
MIDINoteNameTable* __stdcall MIDINoteNameTable_Create () {
	int i;
	MIDINoteNameTable* pMIDINoteNameTable = calloc (sizeof(MIDINoteNameTable), 1);
	if (pMIDINoteNameTable == NULL) {
		return NULL;
	}
	pMIDINoteNameTable->m_pszTitle = NULL;
	pMIDINoteNameTable->m_lTitleLen = 0;
	for (i = 0; i < 128; i++) {
		pMIDINoteNameTable->m_pszName[i] = NULL;
		pMIDINoteNameTable->m_lNameLen[i] = 0;
	}
	pMIDINoteNameTable->m_pNextNoteNameTable = NULL;
	pMIDINoteNameTable->m_pPrevNoteNameTable = NULL;
	pMIDINoteNameTable->m_pParent = NULL;
	return pMIDINoteNameTable;
}

/* MIDINoteNameTableオブジェクトの削除 */
void __stdcall MIDINoteNameTable_Delete (MIDINoteNameTable* pMIDINoteNameTable) {
	int i;
	/* このオブジェクトへのBasedOnによる参照があればその参照を解除 */
	if (pMIDINoteNameTable->m_pParent) {
		MIDIInstrument* pParentInstrument = (MIDIInstrument*)(pMIDINoteNameTable->m_pParent);
		MIDINoteNameTable* pNoteNameTable;
		forEachNoteNameTable (pParentInstrument, pNoteNameTable) {
			if (pNoteNameTable->m_pBasedOn == pMIDINoteNameTable) {
				pNoteNameTable->m_pBasedOn = NULL;
			}
		}
	}
	/* タイトルの削除 */
	free (pMIDINoteNameTable->m_pszTitle);
	pMIDINoteNameTable->m_pszTitle = NULL;
	pMIDINoteNameTable->m_lTitleLen = 0;
	/* 名前配列の全削除 */
	for (i = 0; i < 128; i++) {
		free (pMIDINoteNameTable->m_pszName[i]);
		pMIDINoteNameTable->m_pszName[i] = NULL;
		pMIDINoteNameTable->m_lNameLen[i] = 0;
	}
	/* 双方向リンクリストのつなぎ替え */
	if (pMIDINoteNameTable->m_pParent) {
		MIDIInstrument_RemoveNoteNameTable 
			(pMIDINoteNameTable->m_pParent, pMIDINoteNameTable);
	}
	/* このオブジェクトの削除 */
	free (pMIDINoteNameTable);
}

/* MIDINoteNameTableオブジェクトのタイトルを設定(ANSI) */
long __stdcall MIDINoteNameTable_SetTitleA 
(MIDINoteNameTable* pMIDINoteNameTable, const char* pszTitle) {
	assert (pMIDINoteNameTable);
	if (pMIDINoteNameTable->m_pszTitle) {
		free (pMIDINoteNameTable->m_pszTitle);
		pMIDINoteNameTable->m_pszTitle = NULL;
		pMIDINoteNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = strlen (pszTitle) + 1;
		pMIDINoteNameTable->m_pszTitle = malloc (lLen);
		if (pMIDINoteNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDINoteNameTable->m_pszTitle, 0, lLen);
		strcpy (pMIDINoteNameTable->m_pszTitle, pszTitle);
		pMIDINoteNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDINoteNameTableオブジェクトのタイトルを設定(UNICODE) */
long __stdcall MIDINoteNameTable_SetTitleW 
(MIDINoteNameTable* pMIDINoteNameTable, const wchar_t* pszTitle) {
	assert (pMIDINoteNameTable);
	if (pMIDINoteNameTable->m_pszTitle) {
		free (pMIDINoteNameTable->m_pszTitle);
		pMIDINoteNameTable->m_pszTitle = NULL;
		pMIDINoteNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = wcslen (pszTitle) + 1;
		pMIDINoteNameTable->m_pszTitle = malloc (lLen * sizeof (wchar_t));
		if (pMIDINoteNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDINoteNameTable->m_pszTitle, 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDINoteNameTable->m_pszTitle, pszTitle);
		pMIDINoteNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDINoteNameTableオブジェクトの各名前を設定(ANSI) */
long __stdcall MIDINoteNameTable_SetNameA 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, const char* pszName) {
	long lLen;
	assert (pMIDINoteNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDINoteNameTable->m_pszName[lNumber]) {
		free (pMIDINoteNameTable->m_pszName[lNumber]);
		pMIDINoteNameTable->m_pszName[lNumber] = NULL;
		pMIDINoteNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = strlen (pszName) + 1;
		pMIDINoteNameTable->m_pszName[lNumber] = malloc (lLen);
		if (pMIDINoteNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDINoteNameTable->m_pszName[lNumber], 0, lLen);
		strcpy (pMIDINoteNameTable->m_pszName[lNumber], pszName);
		pMIDINoteNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDINoteNameTableオブジェクトの各名前を設定(UNICODE) */
long __stdcall MIDINoteNameTable_SetNameW 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, const wchar_t* pszName) {
	long lLen;
	assert (pMIDINoteNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDINoteNameTable->m_pszName[lNumber]) {
		free (pMIDINoteNameTable->m_pszName[lNumber]);
		pMIDINoteNameTable->m_pszName[lNumber] = NULL;
		pMIDINoteNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = wcslen (pszName) + 1;
		pMIDINoteNameTable->m_pszName[lNumber] = malloc (lLen * sizeof (wchar_t));
		if (pMIDINoteNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDINoteNameTable->m_pszName[lNumber], 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDINoteNameTable->m_pszName[lNumber], pszName);
		pMIDINoteNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDINoteNameTableオブジェクトのタイトルを取得(ANSI) */
long __stdcall MIDINoteNameTable_GetTitleA 
(MIDINoteNameTable* pMIDINoteNameTable, char* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDINoteNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDINoteNameTable->m_lTitleLen);
		strncpy (pszTitle, pMIDINoteNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = '\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = '\0';
		return 0;
	}
}

/* MIDINoteNameTableオブジェクトのタイトルを取得(UNICODE) */
long __stdcall MIDINoteNameTable_GetTitleW 
(MIDINoteNameTable* pMIDINoteNameTable, wchar_t* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDINoteNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDINoteNameTable->m_lTitleLen);
		wcsncpy (pszTitle, pMIDINoteNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = L'\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = L'\0';
		return 0;
	}
}

/* MIDINoteNameTableオブジェクトの各名前を取得(ANSI) */
long __stdcall MIDINoteNameTable_GetNameA 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, char* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 128);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDINoteNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDINoteNameTable->m_lNameLen[lNumber]);
		strncpy (pszName, pMIDINoteNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = '\0';
		return lLenMin - 1;
	}
	else if (pMIDINoteNameTable->m_pBasedOn) {
		MIDINoteNameTable* pBasedOn = (MIDINoteNameTable*)(pMIDINoteNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			strncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = '\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = '\0';
			return 0;
		}
	}
	else {
		*(pszName) = '\0';
		return 0;
	}
}

/* MIDINoteNameTableオブジェクトの各名前を取得(UNICODE) */
long __stdcall MIDINoteNameTable_GetNameW 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, wchar_t* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 128);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDINoteNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDINoteNameTable->m_lNameLen[lNumber]);
		wcsncpy (pszName, pMIDINoteNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = L'\0';
		return lLenMin - 1;
	}
	else if (pMIDINoteNameTable->m_pBasedOn) {
		MIDINoteNameTable* pBasedOn = (MIDINoteNameTable*)(pMIDINoteNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			wcsncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = L'\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = L'\0';
			return 0;
		}
	}
	else {
		*(pszName) = L'\0';
		return 0;
	}
}

/* 次のMIDINoteNameTableへのポインタを取得(なければNULL) */
MIDINoteNameTable* __stdcall MIDINoteNameTable_GetNextNoteNameTable 
(MIDINoteNameTable* pMIDINoteNameTable) {
	return pMIDINoteNameTable->m_pNextNoteNameTable;
}

/* 前のMIDINoteNameTableへのポインタを取得(なければNULL) */
MIDINoteNameTable* __stdcall MIDINoteNameTable_GetPrevNoteNameTable 
(MIDINoteNameTable* pMIDINoteNameTable) {
	return pMIDINoteNameTable->m_pPrevNoteNameTable;
}

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDINoteNameTable_GetParent
(MIDINoteNameTable* pMIDINoteNameTable) {
	return pMIDINoteNameTable->m_pParent;
}




/******************************************************************************/
/*                                                                            */
/*　MIDIControllerNameTableクラス関数群                                       */
/*                                                                            */
/******************************************************************************/

/* MIDIControllerNameTableオブジェクトの生成 */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_Create () {
	int i;
	MIDIControllerNameTable* pMIDIControllerNameTable = calloc (sizeof(MIDIControllerNameTable), 1);
	if (pMIDIControllerNameTable == NULL) {
		return NULL;
	}
	pMIDIControllerNameTable->m_pszTitle = NULL;
	pMIDIControllerNameTable->m_lTitleLen = 0;
	for (i = 0; i < 128; i++) {
		pMIDIControllerNameTable->m_pszName[i] = NULL;
		pMIDIControllerNameTable->m_lNameLen[i] = 0;
	}
	pMIDIControllerNameTable->m_pNextControllerNameTable = NULL;
	pMIDIControllerNameTable->m_pPrevControllerNameTable = NULL;
	pMIDIControllerNameTable->m_pParent = NULL;
	return pMIDIControllerNameTable;
}

/* MIDIControllerNameTableオブジェクトの削除 */
void __stdcall MIDIControllerNameTable_Delete (MIDIControllerNameTable* pMIDIControllerNameTable) {
	int i;
	/* このオブジェクトへのBasedOnによる参照があればその参照を解除 */
	if (pMIDIControllerNameTable->m_pParent) {
		MIDIInstrument* pParentInstrument = (MIDIInstrument*)(pMIDIControllerNameTable->m_pParent);
		MIDIControllerNameTable* pControllerNameTable;
		forEachControllerNameTable (pParentInstrument, pControllerNameTable) {
			if (pControllerNameTable->m_pBasedOn == pMIDIControllerNameTable) {
				pControllerNameTable->m_pBasedOn = NULL;
			}
		}
	}
	/* タイトルの削除 */
	free (pMIDIControllerNameTable->m_pszTitle);
	pMIDIControllerNameTable->m_pszTitle = NULL;
	pMIDIControllerNameTable->m_lTitleLen = 0;
	/* 名前配列の全削除 */
	for (i = 0; i < 128; i++) {
		free (pMIDIControllerNameTable->m_pszName[i]);
		pMIDIControllerNameTable->m_pszName[i] = NULL;
		pMIDIControllerNameTable->m_lNameLen[i] = 0;
	}
	/* 双方向リンクリストのつなぎ替え */
	if (pMIDIControllerNameTable->m_pParent) {
		MIDIInstrument_RemoveControllerNameTable 
			(pMIDIControllerNameTable->m_pParent, pMIDIControllerNameTable);
	}
	/* このオブジェクトの削除 */
	free (pMIDIControllerNameTable);
}

/* MIDIControllerNameTableオブジェクトのタイトルを設定(ANSI) */
long __stdcall MIDIControllerNameTable_SetTitleA 
(MIDIControllerNameTable* pMIDIControllerNameTable, const char* pszTitle) {
	assert (pMIDIControllerNameTable);
	if (pMIDIControllerNameTable->m_pszTitle) {
		free (pMIDIControllerNameTable->m_pszTitle);
		pMIDIControllerNameTable->m_pszTitle = NULL;
		pMIDIControllerNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = strlen (pszTitle) + 1;
		pMIDIControllerNameTable->m_pszTitle = malloc (lLen);
		if (pMIDIControllerNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIControllerNameTable->m_pszTitle, 0, lLen);
		strcpy (pMIDIControllerNameTable->m_pszTitle, pszTitle);
		pMIDIControllerNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDIControllerNameTableオブジェクトのタイトルを設定(UNICODE) */
long __stdcall MIDIControllerNameTable_SetTitleW 
(MIDIControllerNameTable* pMIDIControllerNameTable, const wchar_t* pszTitle) {
	assert (pMIDIControllerNameTable);
	if (pMIDIControllerNameTable->m_pszTitle) {
		free (pMIDIControllerNameTable->m_pszTitle);
		pMIDIControllerNameTable->m_pszTitle = NULL;
		pMIDIControllerNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = wcslen (pszTitle) + 1;
		pMIDIControllerNameTable->m_pszTitle = malloc (lLen * sizeof (wchar_t));
		if (pMIDIControllerNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIControllerNameTable->m_pszTitle, 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDIControllerNameTable->m_pszTitle, pszTitle);
		pMIDIControllerNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDIControllerNameTableオブジェクトの各名前を設定(ANSI) */
long __stdcall MIDIControllerNameTable_SetNameA 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, const char* pszName) {
	long lLen;
	assert (pMIDIControllerNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIControllerNameTable->m_pszName[lNumber]) {
		free (pMIDIControllerNameTable->m_pszName[lNumber]);
		pMIDIControllerNameTable->m_pszName[lNumber] = NULL;
		pMIDIControllerNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = strlen (pszName) + 1;
		pMIDIControllerNameTable->m_pszName[lNumber] = malloc (lLen);
		if (pMIDIControllerNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDIControllerNameTable->m_pszName[lNumber], 0, lLen);
		strcpy (pMIDIControllerNameTable->m_pszName[lNumber], pszName);
		pMIDIControllerNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDIControllerNameTableオブジェクトの各名前を設定(UNICODE) */
long __stdcall MIDIControllerNameTable_SetNameW 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, const wchar_t* pszName) {
	long lLen;
	assert (pMIDIControllerNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIControllerNameTable->m_pszName[lNumber]) {
		free (pMIDIControllerNameTable->m_pszName[lNumber]);
		pMIDIControllerNameTable->m_pszName[lNumber] = NULL;
		pMIDIControllerNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = wcslen (pszName) + 1;
		pMIDIControllerNameTable->m_pszName[lNumber] = malloc (lLen * sizeof (wchar_t));
		if (pMIDIControllerNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDIControllerNameTable->m_pszName[lNumber], 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDIControllerNameTable->m_pszName[lNumber], pszName);
		pMIDIControllerNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}


/* MIDIControllerNameTableオブジェクトのタイトルを取得(ANSI) */
long __stdcall MIDIControllerNameTable_GetTitleA 
(MIDIControllerNameTable* pMIDIControllerNameTable, char* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIControllerNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIControllerNameTable->m_lTitleLen);
		strncpy (pszTitle, pMIDIControllerNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = '\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = '\0';
		return 0;
	}
}

/* MIDIControllerNameTableオブジェクトのタイトルを取得(UNICODE) */
long __stdcall MIDIControllerNameTable_GetTitleW 
(MIDIControllerNameTable* pMIDIControllerNameTable, wchar_t* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIControllerNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIControllerNameTable->m_lTitleLen);
		wcsncpy (pszTitle, pMIDIControllerNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = L'\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = L'\0';
		return 0;
	}
}

/* MIDIControllerNameTableオブジェクトの各名前を取得(ANSI) */
long __stdcall MIDIControllerNameTable_GetNameA 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, char* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 128);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIControllerNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDIControllerNameTable->m_lNameLen[lNumber]);
		strncpy (pszName, pMIDIControllerNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = '\0';
		return lLenMin - 1;
	}
	else if (pMIDIControllerNameTable->m_pBasedOn) {
		MIDIControllerNameTable* pBasedOn = (MIDIControllerNameTable*)(pMIDIControllerNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			strncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = '\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = '\0';
			return 0;
		}
	}
	else {
		*(pszName) = '\0';
		return 0;
	}
}

/* MIDIControllerNameTableオブジェクトの各名前を取得(UNICODE) */
long __stdcall MIDIControllerNameTable_GetNameW 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, wchar_t* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 128);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIControllerNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDIControllerNameTable->m_lNameLen[lNumber]);
		wcsncpy (pszName, pMIDIControllerNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = L'\0';
		return lLenMin - 1;
	}
	else if (pMIDIControllerNameTable->m_pBasedOn) {
		MIDIControllerNameTable* pBasedOn = (MIDIControllerNameTable*)(pMIDIControllerNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			wcsncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = L'\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = L'\0';
			return 0;
		}
	}
	else {
		*(pszName) = L'\0';
		return 0;
	}
}


/* 次のMIDIControllerNameTableへのポインタを取得(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_GetNextControllerNameTable 
(MIDIControllerNameTable* pMIDIControllerNameTable) {
	return pMIDIControllerNameTable->m_pNextControllerNameTable;
}

/* 前のMIDIControllerNameTableへのポインタを取得(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_GetPrevControllerNameTable 
(MIDIControllerNameTable* pMIDIControllerNameTable) {
	return pMIDIControllerNameTable->m_pPrevControllerNameTable;
}

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIControllerNameTable_GetParent
(MIDIControllerNameTable* pMIDIControllerNameTable) {
	return pMIDIControllerNameTable->m_pParent;
}



/******************************************************************************/
/*                                                                            */
/*　MIDIRPNNameTableクラス関数群                                              */
/*                                                                            */
/******************************************************************************/

/* MIDIRPNNameTableオブジェクトの生成 */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_Create () {
	int i;
	MIDIRPNNameTable* pMIDIRPNNameTable = calloc (sizeof(MIDIRPNNameTable), 1);
	if (pMIDIRPNNameTable == NULL) {
		return NULL;
	}
	pMIDIRPNNameTable->m_pszTitle = NULL;
	pMIDIRPNNameTable->m_lTitleLen = 0;
	for (i = 0; i < 16384; i++) {
		pMIDIRPNNameTable->m_pszName[i] = NULL;
		pMIDIRPNNameTable->m_lNameLen[i] = 0;
	}
	pMIDIRPNNameTable->m_pNextRPNNameTable = NULL;
	pMIDIRPNNameTable->m_pPrevRPNNameTable = NULL;
	pMIDIRPNNameTable->m_pParent = NULL;
	return pMIDIRPNNameTable;
}

/* MIDIRPNNameTableオブジェクトの削除 */
void __stdcall MIDIRPNNameTable_Delete (MIDIRPNNameTable* pMIDIRPNNameTable) {
	int i;
	/* このオブジェクトへのBasedOnによる参照があればその参照を解除 */
	if (pMIDIRPNNameTable->m_pParent) {
		MIDIInstrument* pParentInstrument = (MIDIInstrument*)(pMIDIRPNNameTable->m_pParent);
		MIDIRPNNameTable* pRPNNameTable;
		forEachRPNNameTable (pParentInstrument, pRPNNameTable) {
			if (pRPNNameTable->m_pBasedOn == pMIDIRPNNameTable) {
				pRPNNameTable->m_pBasedOn = NULL;
			}
		}
	}
	/* タイトルの削除 */
	free (pMIDIRPNNameTable->m_pszTitle);
	pMIDIRPNNameTable->m_pszTitle = NULL;
	pMIDIRPNNameTable->m_lTitleLen = 0;
	/* 名前配列の全削除 */
	for (i = 0; i < 16384; i++) {
		free (pMIDIRPNNameTable->m_pszName[i]);
		pMIDIRPNNameTable->m_pszName[i] = NULL;
		pMIDIRPNNameTable->m_lNameLen[i] = 0;
	}
	/* 双方向リンクリストのつなぎ替え */
	if (pMIDIRPNNameTable->m_pParent) {
		MIDIInstrument_RemoveRPNNameTable 
			(pMIDIRPNNameTable->m_pParent, pMIDIRPNNameTable);
	}
	/* このオブジェクトの削除 */
	free (pMIDIRPNNameTable);
}

/* MIDIRPNNameTableオブジェクトのタイトルを設定(ANSI) */
long __stdcall MIDIRPNNameTable_SetTitleA 
(MIDIRPNNameTable* pMIDIRPNNameTable, const char* pszTitle) {
	assert (pMIDIRPNNameTable);
	if (pMIDIRPNNameTable->m_pszTitle) {
		free (pMIDIRPNNameTable->m_pszTitle);
		pMIDIRPNNameTable->m_pszTitle = NULL;
		pMIDIRPNNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = strlen (pszTitle) + 1;
		pMIDIRPNNameTable->m_pszTitle = malloc (lLen);
		if (pMIDIRPNNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIRPNNameTable->m_pszTitle, 0, lLen);
		strcpy (pMIDIRPNNameTable->m_pszTitle, pszTitle);
		pMIDIRPNNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDIRPNNameTableオブジェクトのタイトルを設定(UNICODE) */
long __stdcall MIDIRPNNameTable_SetTitleW 
(MIDIRPNNameTable* pMIDIRPNNameTable, const wchar_t* pszTitle) {
	assert (pMIDIRPNNameTable);
	if (pMIDIRPNNameTable->m_pszTitle) {
		free (pMIDIRPNNameTable->m_pszTitle);
		pMIDIRPNNameTable->m_pszTitle = NULL;
		pMIDIRPNNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = wcslen (pszTitle) + 1;
		pMIDIRPNNameTable->m_pszTitle = malloc (lLen * sizeof (wchar_t));
		if (pMIDIRPNNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIRPNNameTable->m_pszTitle, 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDIRPNNameTable->m_pszTitle, pszTitle);
		pMIDIRPNNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDIRPNNameTableオブジェクトの各名前を設定(ANSI) */
long __stdcall MIDIRPNNameTable_SetNameA 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, const char* pszName) {
	long lLen;
	assert (pMIDIRPNNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIRPNNameTable->m_pszName[lNumber]) {
		free (pMIDIRPNNameTable->m_pszName[lNumber]);
		pMIDIRPNNameTable->m_pszName[lNumber] = NULL;
		pMIDIRPNNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = strlen (pszName) + 1;
		pMIDIRPNNameTable->m_pszName[lNumber] = malloc (lLen);
		if (pMIDIRPNNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDIRPNNameTable->m_pszName[lNumber], 0, lLen);
		strcpy (pMIDIRPNNameTable->m_pszName[lNumber], pszName);
		pMIDIRPNNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDIRPNNameTableオブジェクトの各名前を設定(UNICODE) */
long __stdcall MIDIRPNNameTable_SetNameW 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, const wchar_t* pszName) {
	long lLen;
	assert (pMIDIRPNNameTable);
	assert (0 <= lNumber && lNumber < 128);
	if (lNumber < 0 || lNumber >= 128) {
		return 0;	
	}
	if (pMIDIRPNNameTable->m_pszName[lNumber]) {
		free (pMIDIRPNNameTable->m_pszName[lNumber]);
		pMIDIRPNNameTable->m_pszName[lNumber] = NULL;
		pMIDIRPNNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = wcslen (pszName) + 1;
		pMIDIRPNNameTable->m_pszName[lNumber] = malloc (lLen * sizeof (wchar_t));
		if (pMIDIRPNNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDIRPNNameTable->m_pszName[lNumber], 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDIRPNNameTable->m_pszName[lNumber], pszName);
		pMIDIRPNNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}


/* MIDIRPNNameTableオブジェクトのタイトルを取得(ANSI) */
long __stdcall MIDIRPNNameTable_GetTitleA 
(MIDIRPNNameTable* pMIDIRPNNameTable, char* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIRPNNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIRPNNameTable->m_lTitleLen);
		strncpy (pszTitle, pMIDIRPNNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = '\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = '\0';
		return 0;
	}
}

/* MIDIRPNNameTableオブジェクトのタイトルを取得(UNICODE) */
long __stdcall MIDIRPNNameTable_GetTitleW 
(MIDIRPNNameTable* pMIDIRPNNameTable, wchar_t* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIRPNNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIRPNNameTable->m_lTitleLen);
		wcsncpy (pszTitle, pMIDIRPNNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = L'\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = L'\0';
		return 0;
	}
}

/* MIDIRPNNameTableオブジェクトの各名前を取得(ANSI) */
long __stdcall MIDIRPNNameTable_GetNameA 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, char* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 16384);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 16384) {
		return 0;	
	}
	if (pMIDIRPNNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDIRPNNameTable->m_lNameLen[lNumber]);
		strncpy (pszName, pMIDIRPNNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = '\0';
		return lLenMin - 1;
	}
	else if (pMIDIRPNNameTable->m_pBasedOn) {
		MIDIRPNNameTable* pBasedOn = (MIDIRPNNameTable*)(pMIDIRPNNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			strncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = '\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = '\0';
			return 0;
		}
	}
	else {
		*(pszName) = '\0';
		return 0;
	}
}

/* MIDIRPNNameTableオブジェクトの各名前を取得(UNICODE) */
long __stdcall MIDIRPNNameTable_GetNameW 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, wchar_t* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 16384);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 16384) {
		return 0;	
	}
	if (pMIDIRPNNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDIRPNNameTable->m_lNameLen[lNumber]);
		wcsncpy (pszName, pMIDIRPNNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = L'\0';
		return lLenMin - 1;
	}
	else if (pMIDIRPNNameTable->m_pBasedOn) {
		MIDIRPNNameTable* pBasedOn = (MIDIRPNNameTable*)(pMIDIRPNNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			wcsncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = L'\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = L'\0';
			return 0;
		}
	}
	else {
		*(pszName) = L'\0';
		return 0;
	}
}

/* 次のMIDIRPNNameTableへのポインタを取得(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_GetNextRPNNameTable 
(MIDIRPNNameTable* pMIDIRPNNameTable) {
	return pMIDIRPNNameTable->m_pNextRPNNameTable;
}

/* 前のMIDIRPNNameTableへのポインタを取得(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_GetPrevRPNNameTable 
(MIDIRPNNameTable* pMIDIRPNNameTable) {
	return pMIDIRPNNameTable->m_pPrevRPNNameTable;
}

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIRPNNameTable_GetParent
(MIDIRPNNameTable* pMIDIRPNNameTable) {
	return pMIDIRPNNameTable->m_pParent;
}


/******************************************************************************/
/*                                                                            */
/*　MIDINRPNNameTableクラス関数群                                             */
/*                                                                            */
/******************************************************************************/

/* MIDINRPNNameTableオブジェクトの生成 */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_Create () {
	int i;
	MIDINRPNNameTable* pMIDINRPNNameTable = calloc (sizeof(MIDINRPNNameTable), 1);
	if (pMIDINRPNNameTable == NULL) {
		return NULL;
	}
	pMIDINRPNNameTable->m_pszTitle = NULL;
	pMIDINRPNNameTable->m_lTitleLen = 0;
	for (i = 0; i < 16384; i++) {
		pMIDINRPNNameTable->m_pszName[i] = NULL;
		pMIDINRPNNameTable->m_lNameLen[i] = 0;
	}
	pMIDINRPNNameTable->m_pNextNRPNNameTable = NULL;
	pMIDINRPNNameTable->m_pPrevNRPNNameTable = NULL;
	pMIDINRPNNameTable->m_pParent = NULL;
	return pMIDINRPNNameTable;
}

/* MIDINRPNNameTableオブジェクトの削除 */
void __stdcall MIDINRPNNameTable_Delete (MIDINRPNNameTable* pMIDINRPNNameTable) {
	int i;
	/* このオブジェクトへのBasedOnによる参照があればその参照を解除 */
	if (pMIDINRPNNameTable->m_pParent) {
		MIDIInstrument* pParentInstrument = (MIDIInstrument*)(pMIDINRPNNameTable->m_pParent);
		MIDINRPNNameTable* pNRPNNameTable;
		forEachNRPNNameTable (pParentInstrument, pNRPNNameTable) {
			if (pNRPNNameTable->m_pBasedOn == pMIDINRPNNameTable) {
				pNRPNNameTable->m_pBasedOn = NULL;
			}
		}
	}
	/* タイトルの削除 */
	free (pMIDINRPNNameTable->m_pszTitle);
	pMIDINRPNNameTable->m_pszTitle = NULL;
	pMIDINRPNNameTable->m_lTitleLen = 0;
	/* 名前配列の全削除 */
	for (i = 0; i < 16384; i++) {
		free (pMIDINRPNNameTable->m_pszName[i]);
		pMIDINRPNNameTable->m_pszName[i] = NULL;
		pMIDINRPNNameTable->m_lNameLen[i] = 0;
	}
	/* 双方向リンクリストのつなぎ替え */
	if (pMIDINRPNNameTable->m_pParent) {
		MIDIInstrument_RemoveNRPNNameTable 
			(pMIDINRPNNameTable->m_pParent, pMIDINRPNNameTable);
	}
	/* このオブジェクトの削除 */
	free (pMIDINRPNNameTable);
}

/* MIDINRPNNameTableオブジェクトのタイトルを設定(ANSI) */
long __stdcall MIDINRPNNameTable_SetTitleA 
(MIDINRPNNameTable* pMIDINRPNNameTable, const char* pszTitle) {
	assert (pMIDINRPNNameTable);
	if (pMIDINRPNNameTable->m_pszTitle) {
		free (pMIDINRPNNameTable->m_pszTitle);
		pMIDINRPNNameTable->m_pszTitle = NULL;
		pMIDINRPNNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = strlen (pszTitle) + 1;
		pMIDINRPNNameTable->m_pszTitle = malloc (lLen);
		if (pMIDINRPNNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDINRPNNameTable->m_pszTitle, 0, lLen);
		strcpy (pMIDINRPNNameTable->m_pszTitle, pszTitle);
		pMIDINRPNNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDINRPNNameTableオブジェクトのタイトルを設定(UNICODE) */
long __stdcall MIDINRPNNameTable_SetTitleW 
(MIDINRPNNameTable* pMIDINRPNNameTable, const wchar_t* pszTitle) {
	assert (pMIDINRPNNameTable);
	if (pMIDINRPNNameTable->m_pszTitle) {
		free (pMIDINRPNNameTable->m_pszTitle);
		pMIDINRPNNameTable->m_pszTitle = NULL;
		pMIDINRPNNameTable->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = wcslen (pszTitle) + 1;
		pMIDINRPNNameTable->m_pszTitle = malloc (lLen * sizeof (wchar_t));
		if (pMIDINRPNNameTable->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDINRPNNameTable->m_pszTitle, 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDINRPNNameTable->m_pszTitle, pszTitle);
		pMIDINRPNNameTable->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDINRPNNameTableオブジェクトの各名前を設定(ANSI) */
long __stdcall MIDINRPNNameTable_SetNameA 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, const char* pszName) {
	long lLen;
	assert (pMIDINRPNNameTable);
	assert (0 <= lNumber && lNumber < 16384);
	if (lNumber < 0 || lNumber >= 16384) {
		return 0;	
	}
	if (pMIDINRPNNameTable->m_pszName[lNumber]) {
		free (pMIDINRPNNameTable->m_pszName[lNumber]);
		pMIDINRPNNameTable->m_pszName[lNumber] = NULL;
		pMIDINRPNNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = strlen (pszName) + 1;
		pMIDINRPNNameTable->m_pszName[lNumber] = malloc (lLen);
		if (pMIDINRPNNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDINRPNNameTable->m_pszName[lNumber], 0, lLen);
		strcpy (pMIDINRPNNameTable->m_pszName[lNumber], pszName);
		pMIDINRPNNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDINRPNNameTableオブジェクトの各名前を設定(UNICODE) */
long __stdcall MIDINRPNNameTable_SetNameW 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, const wchar_t* pszName) {
	long lLen;
	assert (pMIDINRPNNameTable);
	assert (0 <= lNumber && lNumber < 16384);
	if (lNumber < 0 || lNumber >= 16384) {
		return 0;	
	}
	if (pMIDINRPNNameTable->m_pszName[lNumber]) {
		free (pMIDINRPNNameTable->m_pszName[lNumber]);
		pMIDINRPNNameTable->m_pszName[lNumber] = NULL;
		pMIDINRPNNameTable->m_lNameLen[lNumber] = 0;
	}
	if (pszName) {
		lLen = wcslen (pszName) + 1;
		pMIDINRPNNameTable->m_pszName[lNumber] = malloc (lLen * sizeof (wchar_t));
		if (pMIDINRPNNameTable->m_pszName[lNumber] == NULL) {
			return 0;
		}
		memset (pMIDINRPNNameTable->m_pszName[lNumber], 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDINRPNNameTable->m_pszName[lNumber], pszName);
		pMIDINRPNNameTable->m_lNameLen[lNumber] = lLen;
	}
	return 1;
}

/* MIDINRPNNameTableオブジェクトのタイトルを取得(ANSI) */
long __stdcall MIDINRPNNameTable_GetTitleA
(MIDINRPNNameTable* pMIDINRPNNameTable, char* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDINRPNNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDINRPNNameTable->m_lTitleLen);
		strncpy (pszTitle, pMIDINRPNNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = '\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = '\0';
		return 0;
	}
}

/* MIDINRPNNameTableオブジェクトのタイトルを取得(UNICODE) */
long __stdcall MIDINRPNNameTable_GetTitleW 
(MIDINRPNNameTable* pMIDINRPNNameTable, wchar_t* pszTitle, long lLen) {
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDINRPNNameTable->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDINRPNNameTable->m_lTitleLen);
		wcsncpy (pszTitle, pMIDINRPNNameTable->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = L'\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = L'\0';
		return 0;
	}
}

/* MIDINRPNNameTableオブジェクトの各名前を取得(ANSI) */
long __stdcall MIDINRPNNameTable_GetNameA 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, char* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 16384);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 16384) {
		return 0;	
	}
	if (pMIDINRPNNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDINRPNNameTable->m_lNameLen[lNumber]);
		strncpy (pszName, pMIDINRPNNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = '\0';
		return lLenMin - 1;
	}
	else if (pMIDINRPNNameTable->m_pBasedOn) {
		MIDINRPNNameTable* pBasedOn = (MIDINRPNNameTable*)(pMIDINRPNNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			strncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = '\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = '\0';
			return 0;
		}
	}
	else {
		*(pszName) = '\0';
		return 0;
	}
}

/* MIDINRPNNameTableオブジェクトの各名前を取得(UNICODE) */
long __stdcall MIDINRPNNameTable_GetNameW 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, wchar_t* pszName, long lLen) {
	assert (0 <= lNumber && lNumber < 16384);
	assert (pszName);
	assert (lLen > 0);
	if (lNumber < 0 || lNumber >= 16384) {
		return 0;	
	}
	if (pMIDINRPNNameTable->m_pszName[lNumber]) {
		long lLenMin = MIN (lLen, pMIDINRPNNameTable->m_lNameLen[lNumber]);
		wcsncpy (pszName, pMIDINRPNNameTable->m_pszName[lNumber], lLenMin - 1);
		*(pszName + lLenMin -1) = L'\0';
		return lLenMin - 1;
	}
	else if (pMIDINRPNNameTable->m_pBasedOn) {
		MIDINRPNNameTable* pBasedOn = (MIDINRPNNameTable*)(pMIDINRPNNameTable->m_pBasedOn);
		if (pBasedOn->m_pszName[lNumber]) {
			long lLenMin = MIN (lLen, pBasedOn->m_lNameLen[lNumber]);
			wcsncpy (pszName, pBasedOn->m_pszName[lNumber], lLenMin - 1);
			*(pszName + lLenMin -1) = L'\0';
			return lLenMin - 1;
		}
		else {
			*(pszName) = L'\0';
			return 0;
		}
	}
	else {
		*(pszName) = L'\0';
		return 0;
	}
}

/* 次のMIDINRPNNameTableへのポインタを取得(なければNULL) */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_GetNextNRPNNameTable 
(MIDINRPNNameTable* pMIDINRPNNameTable) {
	return pMIDINRPNNameTable->m_pNextNRPNNameTable;
}

/* 前のMIDINRPNNameTableへのポインタを取得(なければNULL) */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_GetPrevNRPNNameTable 
(MIDINRPNNameTable* pMIDINRPNNameTable) {
	return pMIDINRPNNameTable->m_pPrevNRPNNameTable;
}

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDINRPNNameTable_GetParent
(MIDINRPNNameTable* pMIDINRPNNameTable) {
	return pMIDINRPNNameTable->m_pParent;
}


/******************************************************************************/
/*                                                                            */
/*　MIDIInstrumentDefinitionクラス関数群                                      */
/*                                                                            */
/******************************************************************************/

/* MIDIInstrumentDefinitionオブジェクトの生成 */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_Create () {
	int i = 0;
	MIDIInstrumentDefinition* pMIDIInstrumentDefinition = calloc (sizeof(MIDIInstrumentDefinition), 1);
	if (pMIDIInstrumentDefinition == NULL) {
		return NULL;
	}
	pMIDIInstrumentDefinition->m_pszTitle = NULL;
	pMIDIInstrumentDefinition->m_lTitleLen = 0;
	pMIDIInstrumentDefinition->m_pMIDIControllerNameTable = NULL;
	pMIDIInstrumentDefinition->m_pMIDIRPNNameTable = NULL;
	pMIDIInstrumentDefinition->m_pMIDINRPNNameTable = NULL;
	pMIDIInstrumentDefinition->m_lMIDIPatchNameNum = 0;
	for (i = 0; i < MIDIINSTRUMENTDEFINITION_MAXPATCHNAME; i++) {
		pMIDIInstrumentDefinition->m_lMIDIPatchNameBank[i] = 0;
		pMIDIInstrumentDefinition->m_pMIDIPatchNameTable[i] = NULL;
	}
	pMIDIInstrumentDefinition->m_lMIDINoteNameNum = 0;
	for (i = 0; i < MIDIINSTRUMENTDEFINITION_MAXNOTENAME; i++) {
		pMIDIInstrumentDefinition->m_lMIDINoteNameBank[i] = 0;
		pMIDIInstrumentDefinition->m_lMIDINoteNameProgram[i] = 0;
		pMIDIInstrumentDefinition->m_pMIDINoteNameTable[i] = NULL;
	}
	pMIDIInstrumentDefinition->m_pNextInstrumentDefinition = NULL;
	pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition = NULL;
	pMIDIInstrumentDefinition->m_pParent = NULL;
	return pMIDIInstrumentDefinition;
}

/* MIDIInstrumentDefinitionオブジェクトの削除 */
void __stdcall MIDIInstrumentDefinition_Delete (MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	int i = 0;
	/* タイトルの削除 */
	free (pMIDIInstrumentDefinition->m_pszTitle);
	pMIDIInstrumentDefinition->m_pszTitle = NULL;
	pMIDIInstrumentDefinition->m_lTitleLen = 0;
	/* インストゥルメント定義の切り離し */
	pMIDIInstrumentDefinition->m_pMIDIControllerNameTable = NULL;
	pMIDIInstrumentDefinition->m_pMIDIRPNNameTable = NULL;
	pMIDIInstrumentDefinition->m_pMIDINRPNNameTable = NULL;
	for (i = 0; i <  MIDIINSTRUMENTDEFINITION_MAXPATCHNAME; i++) {
		pMIDIInstrumentDefinition->m_lMIDIPatchNameBank[i] = 0;
		pMIDIInstrumentDefinition->m_pMIDIPatchNameTable[i] = NULL;
	}
	pMIDIInstrumentDefinition->m_lMIDIPatchNameNum = 0;
	for (i = 0; i < MIDIINSTRUMENTDEFINITION_MAXNOTENAME; i++) {
		pMIDIInstrumentDefinition->m_lMIDINoteNameBank[i] = 0;
		pMIDIInstrumentDefinition->m_lMIDINoteNameProgram[i] = 0;
		pMIDIInstrumentDefinition->m_pMIDINoteNameTable[i] = NULL;
	}
	pMIDIInstrumentDefinition->m_lMIDINoteNameNum = 0;
	/* 双方向リンクリストのつなぎ替え */
	if (pMIDIInstrumentDefinition->m_pParent) {
		MIDIInstrument_RemoveInstrumentDefinition 
			(pMIDIInstrumentDefinition->m_pParent, pMIDIInstrumentDefinition);
	}
	/* このオブジェクトの削除 */
	free (pMIDIInstrumentDefinition);
}

/* MIDIInstrumentDefinitionオブジェクトのタイトルを設定(ANSI) */
long __stdcall MIDIInstrumentDefinition_SetTitleA 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, const char* pszTitle) {
	assert (pMIDIInstrumentDefinition);
	if (pMIDIInstrumentDefinition->m_pszTitle) {
		free (pMIDIInstrumentDefinition->m_pszTitle);
		pMIDIInstrumentDefinition->m_pszTitle = NULL;
		pMIDIInstrumentDefinition->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = strlen (pszTitle) + 1;
		pMIDIInstrumentDefinition->m_pszTitle = malloc (lLen);
		if (pMIDIInstrumentDefinition->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIInstrumentDefinition->m_pszTitle, 0, lLen);
		strcpy (pMIDIInstrumentDefinition->m_pszTitle, pszTitle);
		pMIDIInstrumentDefinition->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDIInstrumentDefinitionオブジェクトのタイトルを設定(UNICODE) */
long __stdcall MIDIInstrumentDefinition_SetTitleW 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, const wchar_t* pszTitle) {
	assert (pMIDIInstrumentDefinition);
	if (pMIDIInstrumentDefinition->m_pszTitle) {
		free (pMIDIInstrumentDefinition->m_pszTitle);
		pMIDIInstrumentDefinition->m_pszTitle = NULL;
		pMIDIInstrumentDefinition->m_lTitleLen = 0;
	}
	if (pszTitle) {
		long lLen = wcslen (pszTitle) + 1;
		pMIDIInstrumentDefinition->m_pszTitle = malloc (lLen * sizeof (wchar_t));
		if (pMIDIInstrumentDefinition->m_pszTitle == NULL) {
			return 0;
		}
		memset (pMIDIInstrumentDefinition->m_pszTitle, 0, lLen * sizeof (wchar_t));
		wcscpy (pMIDIInstrumentDefinition->m_pszTitle, pszTitle);
		pMIDIInstrumentDefinition->m_lTitleLen = lLen;
	}
	return 1;
}

/* MIDIInstrumentDefinitionオブジェクトのコントローラー名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetControllerNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIControllerNameTable* pMIDIControllerNameTable) {
	assert (pMIDIInstrumentDefinition);
	pMIDIInstrumentDefinition->m_pMIDIControllerNameTable = pMIDIControllerNameTable;
	return 1;
}

/* MIDIInstrumentDefinitionオブジェクトのRPN名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIRPNNameTable* pMIDIRPNNameTable) {
	assert (pMIDIInstrumentDefinition);
	pMIDIInstrumentDefinition->m_pMIDIRPNNameTable = pMIDIRPNNameTable;
	return 1;
}

/* MIDIInstrumentDefinitionオブジェクトのNRPN名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetNRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDINRPNNameTable* pMIDINRPNNameTable) {
	assert (pMIDIInstrumentDefinition);
	pMIDIInstrumentDefinition->m_pMIDINRPNNameTable = pMIDINRPNNameTable;
	return 1;
}

/* MIDIInstrumentDefinitionオブジェクトのパッチ名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetPatchNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, MIDIPatchNameTable* pMIDIPatchNameTable) {
	int i = 0;
	assert (pMIDIInstrumentDefinition);
	assert (0 <= lBank && lBank < 16384);
	/* 既に同バンク番号のテーブルが登録されていれば、そこのテーブルのポインタを上書きする */
	for (i = 0; i < pMIDIInstrumentDefinition->m_lMIDIPatchNameNum; i++) {
		if (pMIDIInstrumentDefinition->m_lMIDIPatchNameBank[i] == lBank) {
			pMIDIInstrumentDefinition->m_pMIDIPatchNameTable[i] = pMIDIPatchNameTable;
			return 1;
		}
	}
	/* そうでなければ、新しいバンク番号を登録する */
	if (i < MIDIINSTRUMENTDEFINITION_MAXPATCHNAME) {
		pMIDIInstrumentDefinition->m_lMIDIPatchNameBank[i] = lBank;
		pMIDIInstrumentDefinition->m_pMIDIPatchNameTable[i] = pMIDIPatchNameTable;
		pMIDIInstrumentDefinition->m_lMIDIPatchNameNum++;
		return 1;
	}
	/* これ以上登録できない */
	return 0;
}

/* MIDIInstrumentDefinitionオブジェクトのノート名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetNoteNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, long lProgram, MIDINoteNameTable* pMIDINoteNameTable) {
	int i = 0;
	assert (pMIDIInstrumentDefinition);
	assert (0 <= lBank && lBank < 16384);
	assert (0 <= lProgram && lProgram < 128);
	/* 既に同バンク番号・プログラム番号が登録されていれば、そこのテーブルのポインタを上書きする */
	for (i = 0; i < pMIDIInstrumentDefinition->m_lMIDINoteNameNum; i++) {
		if (pMIDIInstrumentDefinition->m_lMIDINoteNameBank[i] == lBank &&
			pMIDIInstrumentDefinition->m_lMIDINoteNameProgram[i] == lProgram) {
			pMIDIInstrumentDefinition->m_pMIDINoteNameTable[i] = pMIDINoteNameTable;
			return 1;
		}
	}
	/* そうでなければ、新しいバンク番号・プログラム番号を登録する */
	if (i < MIDIINSTRUMENTDEFINITION_MAXNOTENAME) {
		pMIDIInstrumentDefinition->m_lMIDINoteNameBank[i] = lBank;
		pMIDIInstrumentDefinition->m_lMIDINoteNameProgram[i] = lProgram;
		pMIDIInstrumentDefinition->m_pMIDINoteNameTable[i] = pMIDINoteNameTable;
		pMIDIInstrumentDefinition->m_lMIDINoteNameNum++;
		return 1;
	}
	/* これ以上登録できない */
	return 0;
}


/* MIDIInstrumentDefinitionオブジェクトのタイトルを取得(ANSI) */
long __stdcall MIDIInstrumentDefinition_GetTitleA 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, char* pszTitle, long lLen) {
	assert (pMIDIInstrumentDefinition);
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIInstrumentDefinition->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIInstrumentDefinition->m_lTitleLen);
		strncpy (pszTitle, pMIDIInstrumentDefinition->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = '\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = '\0';
		return 0;
	}
}

/* MIDIInstrumentDefinitionオブジェクトのタイトルを取得(UNICODE) */
long __stdcall MIDIInstrumentDefinition_GetTitleW 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, wchar_t* pszTitle, long lLen) {
	assert (pMIDIInstrumentDefinition);
	assert (pszTitle);
	assert (lLen > 0);
	if (pMIDIInstrumentDefinition->m_pszTitle) {
		long lLenMin = MIN (lLen, pMIDIInstrumentDefinition->m_lTitleLen);
		wcsncpy (pszTitle, pMIDIInstrumentDefinition->m_pszTitle, lLenMin - 1);
		*(pszTitle + lLenMin - 1) = L'\0';
		return lLenMin - 1;
	}
	else {
		*(pszTitle) = L'\0';
		return 0;
	}
}

/* MIDIInstrumentDefinitionオブジェクトのコントローラー名テーブルを取得 */
MIDIControllerNameTable* __stdcall MIDIInstrumentDefinition_GetControllerNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	assert (pMIDIInstrumentDefinition);
	return pMIDIInstrumentDefinition->m_pMIDIControllerNameTable;
}

/* MIDIInstrumentDefinitionオブジェクトのRPN名テーブルを取得 */
MIDIRPNNameTable* __stdcall MIDIInstrumentDefinition_GetRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	assert (pMIDIInstrumentDefinition);
	return pMIDIInstrumentDefinition->m_pMIDIRPNNameTable;
}

/* MIDIInstrumentDefinitionオブジェクトのNRPN名テーブルを取得 */
MIDINRPNNameTable* __stdcall MIDIInstrumentDefinition_GetNRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	assert (pMIDIInstrumentDefinition);
	return pMIDIInstrumentDefinition->m_pMIDINRPNNameTable;
}

/* MIDIInstrumentDefinitionオブジェクトのパッチ名テーブルを取得 */
MIDIPatchNameTable* __stdcall MIDIInstrumentDefinition_GetPatchNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank) {
	int i = 0;
	assert (pMIDIInstrumentDefinition);
	assert (0 <= lBank && lBank < 16384);
	/* 該当バンク番号が登録されていれば、そこのテーブルのポインタを取得する */
	for (i = 0; i < pMIDIInstrumentDefinition->m_lMIDIPatchNameNum; i++) {
		if (pMIDIInstrumentDefinition->m_lMIDIPatchNameBank[i] == lBank) {
			return pMIDIInstrumentDefinition->m_pMIDIPatchNameTable[i];
		}
	}
	/* そのバンク番号は見つからなかった */
	return NULL;
}

/* MIDIInstrumentDefinitionオブジェクトのノート名テーブルを取得 */
MIDINoteNameTable* __stdcall MIDIInstrumentDefinition_GetNoteNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, long lProgram) {
	int i = 0;
	assert (pMIDIInstrumentDefinition);
	assert (0 <= lBank && lBank < 16384);
	assert (0 <= lProgram && lProgram < 128);
	/* 該当バンク番号・プログラム番号が登録されていれば、そこのテーブルのポインタを取得する */
	for (i = 0; i < pMIDIInstrumentDefinition->m_lMIDINoteNameNum; i++) {
		if (pMIDIInstrumentDefinition->m_lMIDINoteNameBank[i] == lBank &&
			pMIDIInstrumentDefinition->m_lMIDINoteNameProgram[i] == lProgram) {
			return pMIDIInstrumentDefinition->m_pMIDINoteNameTable[i];
		}
	}
	/* そのバンク番号・プログラム番号は見つからなかった */
	return NULL;
}

/* 次のMIDIInstrumentDefinitionへのポインタを取得(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_GetNextInstrumentDefinition 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	return pMIDIInstrumentDefinition->m_pNextInstrumentDefinition;
}

/* 前のMIDIInstrumentDefinitionへのポインタを取得(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_GetPrevInstrumentDefinition 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	return pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition;
}

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIInstrumentDefinition_GetParent
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	return pMIDIInstrumentDefinition->m_pParent;
}

/******************************************************************************/
/*                                                                            */
/*　MIDIInstrumentクラス関数群                                                */
/*                                                                            */
/******************************************************************************/

/* MIDIInstrumentオブジェクトの生成 */
MIDIInstrument* __stdcall MIDIInstrument_Create () {
	MIDIInstrument* pMIDIInstrument = NULL;
	pMIDIInstrument = calloc (sizeof (MIDIInstrument), 1);
	if (pMIDIInstrument == NULL) {
		return NULL;
	}
	pMIDIInstrument->m_pFirstPatchNameTable = NULL;
	pMIDIInstrument->m_pLastPatchNameTable = NULL;
	pMIDIInstrument->m_pFirstNoteNameTable = NULL;
	pMIDIInstrument->m_pLastNoteNameTable = NULL;
	pMIDIInstrument->m_pFirstControllerNameTable = NULL;
	pMIDIInstrument->m_pLastControllerNameTable = NULL;
	pMIDIInstrument->m_pFirstRPNNameTable = NULL;
	pMIDIInstrument->m_pLastRPNNameTable = NULL;
	pMIDIInstrument->m_pFirstNRPNNameTable = NULL;
	pMIDIInstrument->m_pLastNRPNNameTable = NULL;
	pMIDIInstrument->m_pFirstInstrumentDefinition = NULL;
	pMIDIInstrument->m_pLastInstrumentDefinition = NULL;
	return pMIDIInstrument;
}

/* MIDIInstrumentオブジェクトの削除 */
void __stdcall MIDIInstrument_Delete (MIDIInstrument* pMIDIInstrument) {
	MIDIInstrumentDefinition* pMIDIInstrumentDefinition;
	MIDIPatchNameTable* pMIDIPatchNameTable;
	MIDINoteNameTable* pMIDINoteNameTable;
	MIDIControllerNameTable* pMIDIControllerNameTable;
	MIDIRPNNameTable* pMIDIRPNNameTable;
	MIDINRPNNameTable* pMIDINRPNNameTable;
	/* pMIDIInstrumentがNULLの場合は何もしない */
	if (pMIDIInstrument == NULL) {
		return;
	}
	/* 先にインストゥルメント定義テーブルをすべて削除 */
	pMIDIInstrumentDefinition = pMIDIInstrument->m_pFirstInstrumentDefinition;
	while (pMIDIInstrumentDefinition) {
		MIDIInstrumentDefinition* pNextInstrumentDefinition = 
			pMIDIInstrumentDefinition->m_pNextInstrumentDefinition;
		MIDIInstrumentDefinition_Delete (pMIDIInstrumentDefinition);
		pMIDIInstrumentDefinition = pNextInstrumentDefinition;
	}
	/* パッチ名テーブルをすべて削除 */
	pMIDIPatchNameTable = pMIDIInstrument->m_pFirstPatchNameTable;
	while (pMIDIPatchNameTable) {
		MIDIPatchNameTable* pNextPatchNameTable = pMIDIPatchNameTable->m_pNextPatchNameTable;
		MIDIPatchNameTable_Delete (pMIDIPatchNameTable);
		pMIDIPatchNameTable = pNextPatchNameTable;
	}
	/* ノート名テーブルをすべて削除 */
	pMIDINoteNameTable = pMIDIInstrument->m_pFirstNoteNameTable;
	while (pMIDINoteNameTable) {
		MIDINoteNameTable* pNextNoteNameTable = pMIDINoteNameTable->m_pNextNoteNameTable;
		MIDINoteNameTable_Delete (pMIDINoteNameTable);
		pMIDINoteNameTable = pNextNoteNameTable;
	}
	/* コントローラー名テーブルをすべて削除 */
	pMIDIControllerNameTable = pMIDIInstrument->m_pFirstControllerNameTable;
	while (pMIDIControllerNameTable) {
		MIDIControllerNameTable* pNextControllerNameTable = pMIDIControllerNameTable->m_pNextControllerNameTable;
		MIDIControllerNameTable_Delete (pMIDIControllerNameTable);
		pMIDIControllerNameTable = pNextControllerNameTable;
	}
	/* RPN名テーブルをすべて削除 */
	pMIDIRPNNameTable = pMIDIInstrument->m_pFirstRPNNameTable;
	while (pMIDIRPNNameTable) {
		MIDIRPNNameTable* pNextRPNNameTable = pMIDIRPNNameTable->m_pNextRPNNameTable;
		MIDIRPNNameTable_Delete (pMIDIRPNNameTable);
		pMIDIRPNNameTable = pNextRPNNameTable;
	}
	/* NRPN名テーブルをすべて削除 */
	pMIDINRPNNameTable = pMIDIInstrument->m_pFirstNRPNNameTable;
	while (pMIDINRPNNameTable) {
		MIDINRPNNameTable* pNextNRPNNameTable = pMIDINRPNNameTable->m_pNextNRPNNameTable;
		MIDINRPNNameTable_Delete (pMIDINRPNNameTable);
		pMIDINRPNNameTable = pNextNRPNNameTable;
	}
	/* このオブジェクトを削除 */
	free (pMIDIInstrument);
}

/* MIDIInstrumentにMIDIPatchNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertPatchNameTableAfter 
(MIDIInstrument* pMIDIInstrument, 
 MIDIPatchNameTable* pMIDIPatchNameTable, MIDIPatchNameTable* pTargetPatchNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDIPatchNameTable);
	/* 既にどこかのMIDIInstrumentに属している場合は挿入できない。 */
	if (pMIDIPatchNameTable->m_pParent) {
		return 0;
	}
	/* 挿入位置のターゲットはこのMIDIInstrumentのものでなければならない。 */
	if (pTargetPatchNameTable) {
		if ((MIDIInstrument*)(pTargetPatchNameTable->m_pParent) != pMIDIInstrument) {
			return 0;
		}
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	/* pTargetPatchNameTableの直後に挿入する */
	if (pTargetPatchNameTable) {
		pMIDIPatchNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDIPatchNameTable->m_pPrevPatchNameTable = pTargetPatchNameTable;
		pMIDIPatchNameTable->m_pNextPatchNameTable = pTargetPatchNameTable->m_pNextPatchNameTable;
		if (pTargetPatchNameTable->m_pNextPatchNameTable) {
			((MIDIPatchNameTable*)(pTargetPatchNameTable->m_pNextPatchNameTable))
				->m_pPrevPatchNameTable = pMIDIPatchNameTable;
		}
		else {
			pMIDIInstrument->m_pLastPatchNameTable = pMIDIPatchNameTable;
		}
		pTargetPatchNameTable->m_pNextPatchNameTable = pMIDIPatchNameTable;
	}
	/* pTargetPatchNameTableがNULLならば先頭に挿入する */
	else {
		pMIDIPatchNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDIPatchNameTable->m_pPrevPatchNameTable = NULL;
		pMIDIPatchNameTable->m_pNextPatchNameTable = pMIDIInstrument->m_pFirstPatchNameTable;
		if (pMIDIInstrument->m_pFirstPatchNameTable) {
			pMIDIInstrument->m_pFirstPatchNameTable->m_pPrevPatchNameTable = pMIDIPatchNameTable;
		}
		else {
			pMIDIInstrument->m_pLastPatchNameTable = pMIDIPatchNameTable;
		}
		pMIDIInstrument->m_pFirstPatchNameTable = pMIDIPatchNameTable;
	}
	return 1;
}

/* MIDIInstrumentにMIDIPatchNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddPatchNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIPatchNameTable* pMIDIPatchNameTable) {
	assert (pMIDIInstrument);
	return MIDIInstrument_InsertPatchNameTableAfter
		(pMIDIInstrument, pMIDIPatchNameTable, pMIDIInstrument->m_pLastPatchNameTable);
}

/* MIDIInstrumentからMIDIPatchNameTableを除去(MIDIPatchNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemovePatchNameTable
(MIDIInstrument* pMIDIInstrument, MIDIPatchNameTable* pMIDIPatchNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDIPatchNameTable);
	/* 既にどこのMIDIInstrumentにも属していない場合は除去できない。 */
	if (pMIDIPatchNameTable->m_pParent == NULL) {
		return 0;
	}
	/* 除去するMIDIPatchNameTableはこのMIDIInstrumentのものでなければならない。 */
	if ((MIDIInstrument*)(pMIDIPatchNameTable->m_pParent) != pMIDIInstrument) {
		return 0;
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	if (pMIDIPatchNameTable->m_pNextPatchNameTable) {
		((MIDIPatchNameTable*)(pMIDIPatchNameTable->m_pNextPatchNameTable))->m_pPrevPatchNameTable = 
			pMIDIPatchNameTable->m_pPrevPatchNameTable;
	}
	else {
		pMIDIInstrument->m_pLastPatchNameTable = pMIDIPatchNameTable->m_pPrevPatchNameTable;
	}
	if (pMIDIPatchNameTable->m_pPrevPatchNameTable) {
		((MIDIPatchNameTable*)(pMIDIPatchNameTable->m_pPrevPatchNameTable))->m_pNextPatchNameTable = 
			pMIDIPatchNameTable->m_pNextPatchNameTable;
	}
	else {
		pMIDIInstrument->m_pFirstPatchNameTable = pMIDIPatchNameTable->m_pNextPatchNameTable;
	}
	pMIDIPatchNameTable->m_pNextPatchNameTable = NULL;
	pMIDIPatchNameTable->m_pPrevPatchNameTable = NULL;
	pMIDIPatchNameTable->m_pParent = NULL;
	return 1;
}





/* MIDIInstrumentにMIDINoteNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertNoteNameTableAfter 
(MIDIInstrument* pMIDIInstrument, 
 MIDINoteNameTable* pMIDINoteNameTable, MIDINoteNameTable* pTargetNoteNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDINoteNameTable);
	/* 既にどこかのMIDIInstrumentに属している場合は挿入できない。 */
	if (pMIDINoteNameTable->m_pParent) {
		return 0;
	}
	/* 挿入位置のターゲットはこのMIDIInstrumentのものでなければならない。 */
	if (pTargetNoteNameTable) {
		if ((MIDIInstrument*)(pTargetNoteNameTable->m_pParent) != pMIDIInstrument) {
			return 0;
		}
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	/* pTargetNoteNameTableの直後に挿入する */
	if (pTargetNoteNameTable) {
		pMIDINoteNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDINoteNameTable->m_pPrevNoteNameTable = pTargetNoteNameTable;
		pMIDINoteNameTable->m_pNextNoteNameTable = pTargetNoteNameTable->m_pNextNoteNameTable;
		if (pTargetNoteNameTable->m_pNextNoteNameTable) {
			((MIDINoteNameTable*)(pTargetNoteNameTable->m_pNextNoteNameTable))
				->m_pPrevNoteNameTable = pMIDINoteNameTable;
		}
		else {
			pMIDIInstrument->m_pLastNoteNameTable = pMIDINoteNameTable;
		}
		pTargetNoteNameTable->m_pNextNoteNameTable = pMIDINoteNameTable;
	}
	/* pTargetNoteNameTableがNULLならば先頭に挿入する */
	else {
		pMIDINoteNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDINoteNameTable->m_pPrevNoteNameTable = NULL;
		pMIDINoteNameTable->m_pNextNoteNameTable = pMIDIInstrument->m_pFirstNoteNameTable;
		if (pMIDIInstrument->m_pFirstNoteNameTable) {
			pMIDIInstrument->m_pFirstNoteNameTable->m_pPrevNoteNameTable = pMIDINoteNameTable;
		}
		else {
			pMIDIInstrument->m_pLastNoteNameTable = pMIDINoteNameTable;
		}
		pMIDIInstrument->m_pFirstNoteNameTable = pMIDINoteNameTable;
	}
	return 1;
}

/* MIDIInstrumentにMIDINoteNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddNoteNameTable 
(MIDIInstrument* pMIDIInstrument, MIDINoteNameTable* pMIDINoteNameTable) {
	assert (pMIDIInstrument);
	return MIDIInstrument_InsertNoteNameTableAfter
		(pMIDIInstrument, pMIDINoteNameTable, pMIDIInstrument->m_pLastNoteNameTable);
}

/* MIDIInstrumentからMIDINoteNameTableを除去(MIDINoteNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveNoteNameTable
(MIDIInstrument* pMIDIInstrument, MIDINoteNameTable* pMIDINoteNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDINoteNameTable);
	/* 既にどこのMIDIInstrumentにも属していない場合は除去できない。 */
	if (pMIDINoteNameTable->m_pParent == NULL) {
		return 0;
	}
	/* 除去するMIDINoteNameTableはこのMIDIInstrumentのものでなければならない。 */
	if ((MIDIInstrument*)(pMIDINoteNameTable->m_pParent) != pMIDIInstrument) {
		return 0;
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	if (pMIDINoteNameTable->m_pNextNoteNameTable) {
		((MIDINoteNameTable*)(pMIDINoteNameTable->m_pNextNoteNameTable))->m_pPrevNoteNameTable = 
			pMIDINoteNameTable->m_pPrevNoteNameTable;
	}
	else {
		pMIDIInstrument->m_pLastNoteNameTable = pMIDINoteNameTable->m_pPrevNoteNameTable;
	}
	if (pMIDINoteNameTable->m_pPrevNoteNameTable) {
		((MIDINoteNameTable*)(pMIDINoteNameTable->m_pPrevNoteNameTable))->m_pNextNoteNameTable = 
			pMIDINoteNameTable->m_pNextNoteNameTable;
	}
	else {
		pMIDIInstrument->m_pFirstNoteNameTable = pMIDINoteNameTable->m_pNextNoteNameTable;
	}
	pMIDINoteNameTable->m_pNextNoteNameTable = NULL;
	pMIDINoteNameTable->m_pPrevNoteNameTable = NULL;
	pMIDINoteNameTable->m_pParent = NULL;
	return 1;
}



/* MIDIInstrumentにMIDIControllerNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertControllerNameTableAfter 
(MIDIInstrument* pMIDIInstrument, 
 MIDIControllerNameTable* pMIDIControllerNameTable, MIDIControllerNameTable* pTargetControllerNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDIControllerNameTable);
	/* 既にどこかのMIDIInstrumentに属している場合は挿入できない。 */
	if (pMIDIControllerNameTable->m_pParent) {
		return 0;
	}
	/* 挿入位置のターゲットはこのMIDIInstrumentのものでなければならない。 */
	if (pTargetControllerNameTable) {
		if ((MIDIInstrument*)(pTargetControllerNameTable->m_pParent) != pMIDIInstrument) {
			return 0;
		}
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	/* pTargetControllerNameTableの直後に挿入する */
	if (pTargetControllerNameTable) {
		pMIDIControllerNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDIControllerNameTable->m_pPrevControllerNameTable = pTargetControllerNameTable;
		pMIDIControllerNameTable->m_pNextControllerNameTable = pTargetControllerNameTable->m_pNextControllerNameTable;
		if (pTargetControllerNameTable->m_pNextControllerNameTable) {
			((MIDIControllerNameTable*)(pTargetControllerNameTable->m_pNextControllerNameTable))
				->m_pPrevControllerNameTable = pMIDIControllerNameTable;
		}
		else {
			pMIDIInstrument->m_pLastControllerNameTable = pMIDIControllerNameTable;
		}
		pTargetControllerNameTable->m_pNextControllerNameTable = pMIDIControllerNameTable;
	}
	/* pTargetControllerNameTableがNULLならば先頭に挿入する */
	else {
		pMIDIControllerNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDIControllerNameTable->m_pPrevControllerNameTable = NULL;
		pMIDIControllerNameTable->m_pNextControllerNameTable = pMIDIInstrument->m_pFirstControllerNameTable;
		if (pMIDIInstrument->m_pFirstControllerNameTable) {
			pMIDIInstrument->m_pFirstControllerNameTable->m_pPrevControllerNameTable = pMIDIControllerNameTable;
		}
		else {
			pMIDIInstrument->m_pLastControllerNameTable = pMIDIControllerNameTable;
		}
		pMIDIInstrument->m_pFirstControllerNameTable = pMIDIControllerNameTable;
	}
	return 1;
}

/* MIDIInstrumentにMIDIControllerNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddControllerNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIControllerNameTable* pMIDIControllerNameTable) {
	assert (pMIDIInstrument);
	return MIDIInstrument_InsertControllerNameTableAfter
		(pMIDIInstrument, pMIDIControllerNameTable, pMIDIInstrument->m_pLastControllerNameTable);
}

/* MIDIInstrumentからMIDIControllerNameTableを除去(MIDIControllerNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveControllerNameTable
(MIDIInstrument* pMIDIInstrument, MIDIControllerNameTable* pMIDIControllerNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDIControllerNameTable);
	/* 既にどこのMIDIInstrumentにも属していない場合は除去できない。 */
	if (pMIDIControllerNameTable->m_pParent == NULL) {
		return 0;
	}
	/* 除去するMIDIControllerNameTableはこのMIDIInstrumentのものでなければならない。 */
	if ((MIDIInstrument*)(pMIDIControllerNameTable->m_pParent) != pMIDIInstrument) {
		return 0;
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	if (pMIDIControllerNameTable->m_pNextControllerNameTable) {
		((MIDIControllerNameTable*)(pMIDIControllerNameTable->m_pNextControllerNameTable))->m_pPrevControllerNameTable = 
			pMIDIControllerNameTable->m_pPrevControllerNameTable;
	}
	else {
		pMIDIInstrument->m_pLastControllerNameTable = pMIDIControllerNameTable->m_pPrevControllerNameTable;
	}
	if (pMIDIControllerNameTable->m_pPrevControllerNameTable) {
		((MIDIControllerNameTable*)(pMIDIControllerNameTable->m_pPrevControllerNameTable))->m_pNextControllerNameTable = 
			pMIDIControllerNameTable->m_pNextControllerNameTable;
	}
	else {
		pMIDIInstrument->m_pFirstControllerNameTable = pMIDIControllerNameTable->m_pNextControllerNameTable;
	}
	pMIDIControllerNameTable->m_pNextControllerNameTable = NULL;
	pMIDIControllerNameTable->m_pPrevControllerNameTable = NULL;
	pMIDIControllerNameTable->m_pParent = NULL;
	return 1;
}




/* MIDIInstrumentにMIDIRPNNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertRPNNameTableAfter 
(MIDIInstrument* pMIDIInstrument, 
 MIDIRPNNameTable* pMIDIRPNNameTable, MIDIRPNNameTable* pTargetRPNNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDIRPNNameTable);
	/* 既にどこかのMIDIInstrumentに属している場合は挿入できない。 */
	if (pMIDIRPNNameTable->m_pParent) {
		return 0;
	}
	/* 挿入位置のターゲットはこのMIDIInstrumentのものでなければならない。 */
	if (pTargetRPNNameTable) {
		if ((MIDIInstrument*)(pTargetRPNNameTable->m_pParent) != pMIDIInstrument) {
			return 0;
		}
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	/* pTargetRPNNameTableの直後に挿入する */
	if (pTargetRPNNameTable) {
		pMIDIRPNNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDIRPNNameTable->m_pPrevRPNNameTable = pTargetRPNNameTable;
		pMIDIRPNNameTable->m_pNextRPNNameTable = pTargetRPNNameTable->m_pNextRPNNameTable;
		if (pTargetRPNNameTable->m_pNextRPNNameTable) {
			((MIDIRPNNameTable*)(pTargetRPNNameTable->m_pNextRPNNameTable))
				->m_pPrevRPNNameTable = pMIDIRPNNameTable;
		}
		else {
			pMIDIInstrument->m_pLastRPNNameTable = pMIDIRPNNameTable;
		}
		pTargetRPNNameTable->m_pNextRPNNameTable = pMIDIRPNNameTable;
	}
	/* pTargetRPNNameTableがNULLならば先頭に挿入する */
	else {
		pMIDIRPNNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDIRPNNameTable->m_pPrevRPNNameTable = NULL;
		pMIDIRPNNameTable->m_pNextRPNNameTable = pMIDIInstrument->m_pFirstRPNNameTable;
		if (pMIDIInstrument->m_pFirstRPNNameTable) {
			pMIDIInstrument->m_pFirstRPNNameTable->m_pPrevRPNNameTable = pMIDIRPNNameTable;
		}
		else {
			pMIDIInstrument->m_pLastRPNNameTable = pMIDIRPNNameTable;
		}
		pMIDIInstrument->m_pFirstRPNNameTable = pMIDIRPNNameTable;
	}
	return 1;
}

/* MIDIInstrumentにMIDIRPNNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddRPNNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIRPNNameTable* pMIDIRPNNameTable) {
	assert (pMIDIInstrument);
	return MIDIInstrument_InsertRPNNameTableAfter
		(pMIDIInstrument, pMIDIRPNNameTable, pMIDIInstrument->m_pLastRPNNameTable);
}

/* MIDIInstrumentからMIDIRPNNameTableを除去(MIDIRPNNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveRPNNameTable
(MIDIInstrument* pMIDIInstrument, MIDIRPNNameTable* pMIDIRPNNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDIRPNNameTable);
	/* 既にどこのMIDIInstrumentにも属していない場合は除去できない。 */
	if (pMIDIRPNNameTable->m_pParent == NULL) {
		return 0;
	}
	/* 除去するMIDIRPNNameTableはこのMIDIInstrumentのものでなければならない。 */
	if ((MIDIInstrument*)(pMIDIRPNNameTable->m_pParent) != pMIDIInstrument) {
		return 0;
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	if (pMIDIRPNNameTable->m_pNextRPNNameTable) {
		((MIDIRPNNameTable*)(pMIDIRPNNameTable->m_pNextRPNNameTable))->m_pPrevRPNNameTable = 
			pMIDIRPNNameTable->m_pPrevRPNNameTable;
	}
	else {
		pMIDIInstrument->m_pLastRPNNameTable = pMIDIRPNNameTable->m_pPrevRPNNameTable;
	}
	if (pMIDIRPNNameTable->m_pPrevRPNNameTable) {
		((MIDIRPNNameTable*)(pMIDIRPNNameTable->m_pPrevRPNNameTable))->m_pNextRPNNameTable = 
			pMIDIRPNNameTable->m_pNextRPNNameTable;
	}
	else {
		pMIDIInstrument->m_pFirstRPNNameTable = pMIDIRPNNameTable->m_pNextRPNNameTable;
	}
	pMIDIRPNNameTable->m_pNextRPNNameTable = NULL;
	pMIDIRPNNameTable->m_pPrevRPNNameTable = NULL;
	pMIDIRPNNameTable->m_pParent = NULL;
	return 1;
}



/* MIDIInstrumentにMIDINRPNNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertNRPNNameTableAfter 
(MIDIInstrument* pMIDIInstrument, 
 MIDINRPNNameTable* pMIDINRPNNameTable, MIDINRPNNameTable* pTargetNRPNNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDINRPNNameTable);
	/* 既にどこかのMIDIInstrumentに属している場合は挿入できない。 */
	if (pMIDINRPNNameTable->m_pParent) {
		return 0;
	}
	/* 挿入位置のターゲットはこのMIDIInstrumentのものでなければならない。 */
	if (pTargetNRPNNameTable) {
		if ((MIDIInstrument*)(pTargetNRPNNameTable->m_pParent) != pMIDIInstrument) {
			return 0;
		}
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	/* pTargetNRPNNameTableの直後に挿入する */
	if (pTargetNRPNNameTable) {
		pMIDINRPNNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDINRPNNameTable->m_pPrevNRPNNameTable = pTargetNRPNNameTable;
		pMIDINRPNNameTable->m_pNextNRPNNameTable = pTargetNRPNNameTable->m_pNextNRPNNameTable;
		if (pTargetNRPNNameTable->m_pNextNRPNNameTable) {
			((MIDINRPNNameTable*)(pTargetNRPNNameTable->m_pNextNRPNNameTable))
				->m_pPrevNRPNNameTable = pMIDINRPNNameTable;
		}
		else {
			pMIDIInstrument->m_pLastNRPNNameTable = pMIDINRPNNameTable;
		}
		pTargetNRPNNameTable->m_pNextNRPNNameTable = pMIDINRPNNameTable;
	}
	/* pTargetNRPNNameTableがNULLならば先頭に挿入する */
	else {
		pMIDINRPNNameTable->m_pParent = (void*)pMIDIInstrument;
		pMIDINRPNNameTable->m_pPrevNRPNNameTable = NULL;
		pMIDINRPNNameTable->m_pNextNRPNNameTable = pMIDIInstrument->m_pFirstNRPNNameTable;
		if (pMIDIInstrument->m_pFirstNRPNNameTable) {
			pMIDIInstrument->m_pFirstNRPNNameTable->m_pPrevNRPNNameTable = pMIDINRPNNameTable;
		}
		else {
			pMIDIInstrument->m_pLastNRPNNameTable = pMIDINRPNNameTable;
		}
		pMIDIInstrument->m_pFirstNRPNNameTable = pMIDINRPNNameTable;
	}
	return 1;
}

/* MIDIInstrumentにMIDINRPNNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddNRPNNameTable 
(MIDIInstrument* pMIDIInstrument, MIDINRPNNameTable* pMIDINRPNNameTable) {
	assert (pMIDIInstrument);
	return MIDIInstrument_InsertNRPNNameTableAfter
		(pMIDIInstrument, pMIDINRPNNameTable, pMIDIInstrument->m_pLastNRPNNameTable);
}

/* MIDIInstrumentからMIDINRPNNameTableを除去(MIDINRPNNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveNRPNNameTable
(MIDIInstrument* pMIDIInstrument, MIDINRPNNameTable* pMIDINRPNNameTable) {
	assert (pMIDIInstrument);
	assert (pMIDINRPNNameTable);
	/* 既にどこのMIDIInstrumentにも属していない場合は除去できない。 */
	if (pMIDINRPNNameTable->m_pParent == NULL) {
		return 0;
	}
	/* 除去するMIDINRPNNameTableはこのMIDIInstrumentのものでなければならない。 */
	if ((MIDIInstrument*)(pMIDINRPNNameTable->m_pParent) != pMIDIInstrument) {
		return 0;
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	if (pMIDINRPNNameTable->m_pNextNRPNNameTable) {
		((MIDINRPNNameTable*)(pMIDINRPNNameTable->m_pNextNRPNNameTable))->m_pPrevNRPNNameTable = 
			pMIDINRPNNameTable->m_pPrevNRPNNameTable;
	}
	else {
		pMIDIInstrument->m_pLastNRPNNameTable = pMIDINRPNNameTable->m_pPrevNRPNNameTable;
	}
	if (pMIDINRPNNameTable->m_pPrevNRPNNameTable) {
		((MIDINRPNNameTable*)(pMIDINRPNNameTable->m_pPrevNRPNNameTable))->m_pNextNRPNNameTable = 
			pMIDINRPNNameTable->m_pNextNRPNNameTable;
	}
	else {
		pMIDIInstrument->m_pFirstNRPNNameTable = pMIDINRPNNameTable->m_pNextNRPNNameTable;
	}
	pMIDINRPNNameTable->m_pNextNRPNNameTable = NULL;
	pMIDINRPNNameTable->m_pPrevNRPNNameTable = NULL;
	pMIDINRPNNameTable->m_pParent = NULL;
	return 1;
}



/* MIDIInstrumentにMIDIInstrumentDefinitionを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertInstrumentDefinitionAfter 
(MIDIInstrument* pMIDIInstrument, 
 MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIInstrumentDefinition* pTargetInstrumentDefinition) {
	assert (pMIDIInstrument);
	assert (pMIDIInstrumentDefinition);
	/* 既にどこかのMIDIInstrumentに属している場合は挿入できない。 */
	if (pMIDIInstrumentDefinition->m_pParent) {
		return 0;
	}
	/* 挿入位置のターゲットはこのMIDIInstrumentのものでなければならない。 */
	if (pTargetInstrumentDefinition) {
		if ((MIDIInstrument*)(pTargetInstrumentDefinition->m_pParent) != pMIDIInstrument) {
			return 0;
		}
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	/* pTargetInstrumentDefinitionの直後に挿入する */
	if (pTargetInstrumentDefinition) {
		pMIDIInstrumentDefinition->m_pParent = (void*)pMIDIInstrument;
		pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition = pTargetInstrumentDefinition;
		pMIDIInstrumentDefinition->m_pNextInstrumentDefinition = pTargetInstrumentDefinition->m_pNextInstrumentDefinition;
		if (pTargetInstrumentDefinition->m_pNextInstrumentDefinition) {
			((MIDIInstrumentDefinition*)(pTargetInstrumentDefinition->m_pNextInstrumentDefinition))
				->m_pPrevInstrumentDefinition = pMIDIInstrumentDefinition;
		}
		else {
			pMIDIInstrument->m_pLastInstrumentDefinition = pMIDIInstrumentDefinition;
		}
		pTargetInstrumentDefinition->m_pNextInstrumentDefinition = pMIDIInstrumentDefinition;
	}
	/* pTargetInstrumentDefinitionがNULLならば先頭に挿入する */
	else {
		pMIDIInstrumentDefinition->m_pParent = (void*)pMIDIInstrument;
		pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition = NULL;
		pMIDIInstrumentDefinition->m_pNextInstrumentDefinition = pMIDIInstrument->m_pFirstInstrumentDefinition;
		if (pMIDIInstrument->m_pFirstInstrumentDefinition) {
			pMIDIInstrument->m_pFirstInstrumentDefinition->m_pPrevInstrumentDefinition = pMIDIInstrumentDefinition;
		}
		else {
			pMIDIInstrument->m_pLastInstrumentDefinition = pMIDIInstrumentDefinition;
		}
		pMIDIInstrument->m_pFirstInstrumentDefinition = pMIDIInstrumentDefinition;
	}
	return 1;
}

/* MIDIInstrumentにMIDIInstrumentDefinitionを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddInstrumentDefinition 
(MIDIInstrument* pMIDIInstrument, MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	assert (pMIDIInstrument);
	return MIDIInstrument_InsertInstrumentDefinitionAfter
		(pMIDIInstrument, pMIDIInstrumentDefinition, pMIDIInstrument->m_pLastInstrumentDefinition);
}

/* MIDIInstrumentからMIDIInstrumentDefinitionを除去(MIDIInstrumentDefinition自体は削除しない) */
long __stdcall MIDIInstrument_RemoveInstrumentDefinition
(MIDIInstrument* pMIDIInstrument, MIDIInstrumentDefinition* pMIDIInstrumentDefinition) {
	assert (pMIDIInstrument);
	assert (pMIDIInstrumentDefinition);
	/* 既にどこのMIDIInstrumentにも属していない場合は除去できない。 */
	if (pMIDIInstrumentDefinition->m_pParent == NULL) {
		return 0;
	}
	/* 除去するMIDIInstrumentDefinitionはこのMIDIInstrumentのものでなければならない。 */
	if ((MIDIInstrument*)(pMIDIInstrumentDefinition->m_pParent) != pMIDIInstrument) {
		return 0;
	}
	/* 双方向リンクリストのポインタつなぎかえ */
	if (pMIDIInstrumentDefinition->m_pNextInstrumentDefinition) {
		((MIDIInstrumentDefinition*)(pMIDIInstrumentDefinition->m_pNextInstrumentDefinition))->m_pPrevInstrumentDefinition = 
			pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition;
	}
	else {
		pMIDIInstrument->m_pLastInstrumentDefinition = pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition;
	}
	if (pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition) {
		((MIDIInstrumentDefinition*)(pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition))->m_pNextInstrumentDefinition = 
			pMIDIInstrumentDefinition->m_pNextInstrumentDefinition;
	}
	else {
		pMIDIInstrument->m_pFirstInstrumentDefinition = pMIDIInstrumentDefinition->m_pNextInstrumentDefinition;
	}
	pMIDIInstrumentDefinition->m_pNextInstrumentDefinition = NULL;
	pMIDIInstrumentDefinition->m_pPrevInstrumentDefinition = NULL;
	pMIDIInstrumentDefinition->m_pParent = NULL;
	return 1;
}

/* 最初のパッチ名テーブルへのポインタを返す(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetFirstPatchNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pFirstPatchNameTable;
}

/* 最後のパッチ名テーブルへのポインタを返す(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetLastPatchNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pLastPatchNameTable;
}

/* 最初のノート名テーブルへのポインタを返す(なければNULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetFirstNoteNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pFirstNoteNameTable;
}

/* 最後のノート名テーブルへのポインタを返す(なければNULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetLastNoteNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pLastNoteNameTable;
}

/* 最初のコントローラ名テーブルへのポインタを返す(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetFirstControllerNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pFirstControllerNameTable;
}

/* 最後のコントローラ名テーブルへのポインタを返す(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetLastControllerNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pLastControllerNameTable;
}

/* 最初のRPN名テーブルへのポインタを返す(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetFirstRPNNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pFirstRPNNameTable;
}

/* 最後のRPN名テーブルへのポインタを返す(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetLastRPNNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pLastRPNNameTable;
}

/* 最初のNRPN名テーブルへのポインタを返す(なければNULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetFirstNRPNNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pFirstNRPNNameTable;
}

/* 最後のNRPN名テーブルへのポインタを返す(なければNULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetLastNRPNNameTable
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pLastNRPNNameTable;
}

/* 最初のインストゥルメント定義へのポインタを返す(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetFirstInstrumentDefinition
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pFirstInstrumentDefinition;
}

/* 最後のインストゥルメント定義へのポインタを返す(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetLastInstrumentDefinition
(MIDIInstrument* pMIDIInstrument) {
	return pMIDIInstrument->m_pLastInstrumentDefinition;
}

/* タイトルからMIDIPatchNameTableへのポインタを返す(なければNULL)(ANSI) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetPatchNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle) {
	MIDIPatchNameTable* pMIDIPatchNameTable;
	forEachPatchNameTable (pMIDIInstrument, pMIDIPatchNameTable) {
		if (strcmp (pMIDIPatchNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDIPatchNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDIPatchNameTableへのポインタを返す(なければNULL)(UNICODE) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetPatchNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle) {
	MIDIPatchNameTable* pMIDIPatchNameTable;
	forEachPatchNameTable (pMIDIInstrument, pMIDIPatchNameTable) {
		if (wcscmp (pMIDIPatchNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDIPatchNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDINoteNameTableへのポインタを返す(なければNULL)(ANSI) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetNoteNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle) {
	MIDINoteNameTable* pMIDINoteNameTable;
	forEachNoteNameTable (pMIDIInstrument, pMIDINoteNameTable) {
		if (strcmp (pMIDINoteNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDINoteNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDINoteNameTableへのポインタを返す(なければNULL)(UNICODE) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetNoteNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle) {
	MIDINoteNameTable* pMIDINoteNameTable;
	forEachNoteNameTable (pMIDIInstrument, pMIDINoteNameTable) {
		if (wcscmp (pMIDINoteNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDINoteNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDIControllerNameTableへのポインタを返す(なければNULL)(ANSI) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetControllerNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle) {
	MIDIControllerNameTable* pMIDIControllerNameTable;
	forEachControllerNameTable (pMIDIInstrument, pMIDIControllerNameTable) {
		if (strcmp (pMIDIControllerNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDIControllerNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDIControllerNameTableへのポインタを返す(なければNULL)(UNICODE) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetControllerNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle) {
	MIDIControllerNameTable* pMIDIControllerNameTable;
	forEachControllerNameTable (pMIDIInstrument, pMIDIControllerNameTable) {
		if (wcscmp (pMIDIControllerNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDIControllerNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDIRPNNameTableへのポインタを返す(なければNULL)(ANSI) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetRPNNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle) {
	MIDIRPNNameTable* pMIDIRPNNameTable;
	forEachRPNNameTable (pMIDIInstrument, pMIDIRPNNameTable) {
		if (strcmp (pMIDIRPNNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDIRPNNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDIRPNNameTableへのポインタを返す(なければNULL)(UNICODE) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetRPNNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle) {
	MIDIRPNNameTable* pMIDIRPNNameTable;
	forEachRPNNameTable (pMIDIInstrument, pMIDIRPNNameTable) {
		if (wcscmp (pMIDIRPNNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDIRPNNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDINRPNNameTableへのポインタを返す(なければNULL)(ANSI) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetNRPNNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle) {
	MIDINRPNNameTable* pMIDINRPNNameTable;
	forEachNRPNNameTable (pMIDIInstrument, pMIDINRPNNameTable) {
		if (strcmp (pMIDINRPNNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDINRPNNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDINRPNNameTableへのポインタを返す(なければNULL)(UNICODE) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetNRPNNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle) {
	MIDINRPNNameTable* pMIDINRPNNameTable;
	forEachNRPNNameTable (pMIDIInstrument, pMIDINRPNNameTable) {
		if (wcscmp (pMIDINRPNNameTable->m_pszTitle, pszTitle) == 0) {
			return pMIDINRPNNameTable;
		}
	}
	return NULL;
}

/* タイトルからMIDIInstrumentDefinitionへのポインタを返す(なければNULL)(ANSI) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetInstrumentDefinitionFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle) {
	MIDIInstrumentDefinition* pMIDIInstrumentDefinition;
	forEachInstrumentDefinition (pMIDIInstrument, pMIDIInstrumentDefinition) {
		if (strcmp (pMIDIInstrumentDefinition->m_pszTitle, pszTitle) == 0) {
			return pMIDIInstrumentDefinition;
		}
	}
	return NULL;
}

/* タイトルからMIDIInstrumentDefinitionへのポインタを返す(なければNULL)(UNICODE) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetInstrumentDefinitionFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle) {
	MIDIInstrumentDefinition* pMIDIInstrumentDefinition;
	forEachInstrumentDefinition (pMIDIInstrument, pMIDIInstrumentDefinition) {
		if (wcscmp (pMIDIInstrumentDefinition->m_pszTitle, pszTitle) == 0) {
			return pMIDIInstrumentDefinition;
		}
	}
	return NULL;
}

/* Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを読み込み(ANSI) */
MIDIInstrument* __stdcall MIDIInstrument_LoadA (const char* pszFileName) {
	MIDIInstrument* pMIDIInstrument = NULL;
	pMIDIInstrument = MIDIInstrument_Create ();
	if (pMIDIInstrument == NULL) {
		return NULL;
	}
	if (MIDIInstrument_LoadAdditionalA (pMIDIInstrument, pszFileName) == 0) {
		MIDIInstrument_Delete (pMIDIInstrument);
		return NULL;
	}
	return pMIDIInstrument;
}

/* Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを読み込み(UNICODE) */
MIDIInstrument* __stdcall MIDIInstrument_LoadW (const wchar_t* pszFileName) {
	MIDIInstrument* pMIDIInstrument = NULL;
	pMIDIInstrument = MIDIInstrument_Create ();
	if (pMIDIInstrument == NULL) {
		return NULL;
	}
	if (MIDIInstrument_LoadAdditionalW (pMIDIInstrument, pszFileName) == 0) {
		MIDIInstrument_Delete (pMIDIInstrument);
		return NULL;
	}
	return pMIDIInstrument;
}

/* Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを追加読み込み(ANSI) */
long __stdcall MIDIInstrument_LoadAdditionalA 
(MIDIInstrument* pMIDIInstrument, const char* pszFileName) {
	FILE* pFile = NULL;
	MIDIPatchNameTable* pMIDIPatchNameTable = NULL;
	MIDINoteNameTable* pMIDINoteNameTable = NULL;
	MIDIControllerNameTable* pMIDIControllerNameTable = NULL;
	MIDIRPNNameTable* pMIDIRPNNameTable = NULL;
	MIDINRPNNameTable* pMIDINRPNNameTable = NULL;
	MIDIInstrumentDefinition* pMIDIInstrumentDefinition = NULL;
	MIDIPatchNameTable* pTempPatchNameTable = NULL;
	MIDINoteNameTable* pTempNoteNameTable = NULL;
	MIDIControllerNameTable* pTempControllerNameTable = NULL;
	MIDIRPNNameTable* pTempRPNNameTable = NULL;
	MIDINRPNNameTable* pTempNRPNNameTable = NULL;
	long lNumber;
	char* pEqual = NULL;
	char cMode = 0;
	char szTextLine[256];
	char szTitle[256];
	char szName[256];
	memset (szTextLine, 0, sizeof (szTextLine));
	memset (szTitle, 0, sizeof (szTitle));
	memset (szName, 0, sizeof (szName));
	if (pMIDIInstrument == NULL) {
		return 0;
	}
	pFile = fopen (pszFileName, "rt");
	if (pFile == NULL) {
		return 0;
	}
	while (!feof (pFile)) {
		/* 1行読み込み */
		memset (szTextLine, 0, sizeof (szTextLine));
		fgets (szTextLine, 255, pFile);
		/* コメント (例: ";Noland SC-88Ero Instrument Definitions\n" */
		if (szTextLine[0] == ';') {
			continue;
		}
		/* モードセクション (例: ".Patch Name\n") */
		else if (szTextLine[0] == '.') {
			if (strncmp (szTextLine, ".Patch Names", 12) == 0) {
				cMode = 1;
			}
			else if (strncmp (szTextLine, ".Note Names", 11) == 0) {
				cMode = 2;
			}
			else if (strncmp (szTextLine, ".Controller Names", 17) == 0) {
				cMode = 3;
			}
			else if (strncmp (szTextLine, ".RPN Names", 10) == 0) {
				cMode = 4;
			}
			else if (strncmp (szTextLine, ".NRPN Names", 11) == 0) {
				cMode = 5;
			}
			else if (strncmp (szTextLine, ".Instrument Definitions", 23) == 0) {
				cMode = 6;
			}
			else {
				cMode = 0;
			}
		}
		/* タイトルセクション (例: "[Noland SC-88Ero Capital Tones]\n" */
		else if (szTextLine[0] == '[' ) {
			char* p = strchr (szTextLine + 1, ']');
			memset (szTitle, 0, sizeof (szTitle));
			if (p) {
				strncpy (szTitle, szTextLine + 1, p - szTextLine - 1);
			}
			else {
				strncpy (szTitle, szTextLine + 1, 250);
			}
			pMIDIPatchNameTable = NULL;
			pMIDINoteNameTable = NULL;
			pMIDIControllerNameTable = NULL;
			pMIDIRPNNameTable = NULL;
			pMIDINRPNNameTable = NULL;
			pMIDIInstrumentDefinition = NULL;
			switch (cMode) {
			case 1: /* ".Patch Name" */
				pMIDIPatchNameTable = MIDIPatchNameTable_Create ();
				if (pMIDIPatchNameTable) {
					MIDIPatchNameTable_SetTitleA (pMIDIPatchNameTable, szTitle);
					MIDIInstrument_AddPatchNameTable (pMIDIInstrument, pMIDIPatchNameTable);
				}
				break;
			case 2: /* ".Note Name" */
				pMIDINoteNameTable = MIDINoteNameTable_Create ();
				if (pMIDINoteNameTable) {
					MIDINoteNameTable_SetTitleA (pMIDINoteNameTable, szTitle);
					MIDIInstrument_AddNoteNameTable (pMIDIInstrument, pMIDINoteNameTable);
				}
				break;
			case 3: /* ".Controller Name" */
				pMIDIControllerNameTable = MIDIControllerNameTable_Create ();
				if (pMIDIControllerNameTable) {
					MIDIControllerNameTable_SetTitleA (pMIDIControllerNameTable, szTitle);
					MIDIInstrument_AddControllerNameTable (pMIDIInstrument, pMIDIControllerNameTable);
				}
				break;
			case 4: /* ".RPN Name" */
				pMIDIRPNNameTable = MIDIRPNNameTable_Create ();
				if (pMIDIRPNNameTable) {
					MIDIRPNNameTable_SetTitleA (pMIDIRPNNameTable, szTitle);
					MIDIInstrument_AddRPNNameTable (pMIDIInstrument, pMIDIRPNNameTable);
				}
				break;
			case 5: /* ".NRPN Name" */
				pMIDINRPNNameTable = MIDINRPNNameTable_Create ();
				if (pMIDINRPNNameTable) {
					MIDINRPNNameTable_SetTitleA (pMIDINRPNNameTable, szTitle);
					MIDIInstrument_AddNRPNNameTable (pMIDIInstrument, pMIDINRPNNameTable);
				}
				break;
			case 6: /* ".Instrument Definition" */
				pMIDIInstrumentDefinition = MIDIInstrumentDefinition_Create ();
				if (pMIDIInstrumentDefinition) {
					MIDIInstrumentDefinition_SetTitleA (pMIDIInstrumentDefinition, szTitle);
					MIDIInstrument_AddInstrumentDefinition (pMIDIInstrument, pMIDIInstrumentDefinition);
				}
				break;
			}
		}
		/* 数字=名前(例: "0=Piano1\n", "Patch[0]=Noland SC-88Ero Drumsets\n") */
		else if (pEqual = strchr (szTextLine, '=')) {
			switch (cMode) {
			case 1: /* ".Patch Name" */
				if (strncmp (szTextLine, "BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					forEachPatchNameTable (pMIDIInstrument, pTempPatchNameTable) {
						if (strcmp (pTempPatchNameTable->m_pszTitle, szName) == 0) {
							pMIDIPatchNameTable->m_pBasedOn = pTempPatchNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempPatchNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : PatchNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = atoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					if (pMIDIPatchNameTable && 
						0 <= lNumber && lNumber < 128) {
						MIDIPatchNameTable_SetNameA 
							(pMIDIPatchNameTable, lNumber, szName);
					}
				}
				break;
			case 2: /* ".Note Name" */
				if (strncmp (szTextLine, "BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					forEachNoteNameTable (pMIDIInstrument, pTempNoteNameTable) {
						if (strcmp (pTempNoteNameTable->m_pszTitle, szName) == 0) {
							pMIDINoteNameTable->m_pBasedOn = pTempNoteNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempNoteNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : NoteNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = atoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					if (pMIDINoteNameTable &&
						0 <= lNumber && lNumber < 128) {
						MIDINoteNameTable_SetNameA 
							(pMIDINoteNameTable, lNumber, szName);
					}
				}
				break;
			case 3: /* ".Controller Name" */
				if (strncmp (szTextLine, "BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					forEachControllerNameTable (pMIDIInstrument, pTempControllerNameTable) {
						if (strcmp (pTempControllerNameTable->m_pszTitle, szName) == 0) {
							pMIDIControllerNameTable->m_pBasedOn = pTempControllerNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempControllerNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" :ControllerNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = atoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					if (pMIDIControllerNameTable &&
						0 <= lNumber && lNumber < 128) {
						MIDIControllerNameTable_SetNameA 
							(pMIDIControllerNameTable, lNumber, szName);
					}
				}
				break;
			case 4: /* ".RPN Name" */
				if (strncmp (szTextLine, "BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					forEachRPNNameTable (pMIDIInstrument, pTempRPNNameTable) {
						if (strcmp (pTempRPNNameTable->m_pszTitle, szName) == 0) {
							pMIDIRPNNameTable->m_pBasedOn = pTempRPNNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempRPNNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : RPNNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = atoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					if (pMIDIRPNNameTable &&
						0 <= lNumber && lNumber < 16384) {
						MIDIRPNNameTable_SetNameA 
							(pMIDIRPNNameTable, lNumber, szName);
					}
				}
				break;
			case 5: /* ".NRPN Name" */
				if (strncmp (szTextLine, "BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					forEachNRPNNameTable (pMIDIInstrument, pTempNRPNNameTable) {
						if (strcmp (pTempNRPNNameTable->m_pszTitle, szName) == 0) {
							pMIDINRPNNameTable->m_pBasedOn = pTempNRPNNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempNRPNNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : NPRNNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = atoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					strncpy (szName, pEqual + 1, strlen (pEqual + 1) - 1);
					if (pMIDINRPNNameTable &&
						0 <= lNumber && lNumber < 16384) {
						MIDINRPNNameTable_SetNameA 
							(pMIDINRPNNameTable, lNumber, szName);
					}
				}
				break;
			case 6: /* ".Instrument Definition" */
				memset (szTitle, 0, sizeof (szTitle));
				strncpy (szTitle, pEqual + 1, strlen (pEqual + 1) - 1);
				/* "Control=(ControllerNameTable名) " */
				if (strncmp (szTextLine, "Control=", 8) == 0) {
					if (pMIDIInstrumentDefinition) {
						/* 一致するControllerNameTable名を探索 */
						forEachControllerNameTable (pMIDIInstrument, pMIDIControllerNameTable) {
							if (strcmp (pMIDIControllerNameTable->m_pszTitle, szTitle) == 0) {
								pMIDIInstrumentDefinition->m_pMIDIControllerNameTable = pMIDIControllerNameTable;
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDIControllerNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : ControllerNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* "RPN=(RPNNameTable名) " */
				else if (strncmp (szTextLine, "RPN=", 4) == 0) {
					if (pMIDIInstrumentDefinition) {
						/* 一致するRPNNameTable名を探索 */
						forEachRPNNameTable (pMIDIInstrument, pMIDIRPNNameTable) {
							if (strcmp (pMIDIRPNNameTable->m_pszTitle, szTitle) == 0) {
								pMIDIInstrumentDefinition->m_pMIDIRPNNameTable = pMIDIRPNNameTable;
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDIRPNNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : RPNNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* "NRPN=(NRPNNameTable名)" */
				else if (strncmp (szTextLine, "NRPN=", 5) == 0) {
					if (pMIDIInstrumentDefinition) {
						/* 一致するNPRNNameTable名を探索 */
						forEachNRPNNameTable (pMIDIInstrument, pMIDINRPNNameTable) {
							if (strcmp (pMIDINRPNNameTable->m_pszTitle, szTitle) == 0) {
								pMIDIInstrumentDefinition->m_pMIDINRPNNameTable = pMIDINRPNNameTable;
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDINRPNNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : NRPNNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* Patch[*]=(PatchNameTable名) */
				else if (strncmp (szTextLine, "Patch[*", 7) == 0) {
					;
				}
				/* Patch[lBank]=(PatchNameTable名) */
				else if (strncmp (szTextLine, "Patch[", 6) == 0) {
					long lBank = atoi (szTextLine + 6);
					if (pMIDIInstrumentDefinition &&
						0 <= lBank && lBank < 16384) {
						/* 一致するPatchNameTable名を探索 */
						forEachPatchNameTable (pMIDIInstrument, pMIDIPatchNameTable) {
							if (strcmp (pMIDIPatchNameTable->m_pszTitle, szTitle) == 0) {
								MIDIInstrumentDefinition_SetPatchNameTable
									(pMIDIInstrumentDefinition, lBank, pMIDIPatchNameTable);
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDIPatchNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : PatchNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* Key[*,*]=(NoteNameTable名) */
				else if (strncmp (szTextLine, "Key[*", 5) == 0) {
					;
				}
				/* Key[lBank,lProgramChange]=(NoteNameTable名) */
				else if (strncmp (szTextLine, "Key[", 4) == 0) {
					long lBank = atoi (szTextLine + 4);
					long lProgramChange = 0;
					char* p = strchr (szTextLine + 4, ',');
					if (p) { 
						 lProgramChange = atoi (p + 1);
					}
					if (pMIDIInstrumentDefinition &&
						0 <= lBank && lBank < 16384 &&
						0 <= lProgramChange && lProgramChange < 128) {
						/* 一致するNoteNameTable名を探索 */
						forEachNoteNameTable (pMIDIInstrument, pMIDINoteNameTable) {
							if (strcmp (pMIDINoteNameTable->m_pszTitle, szTitle) == 0) {
								MIDIInstrumentDefinition_SetNoteNameTable
									(pMIDIInstrumentDefinition, lBank, lProgramChange, pMIDINoteNameTable);
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDINoteNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : NoteNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				break;
			}
		}
	}
	fclose (pFile);
	pFile = NULL;
	return 1;
}


/* Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを追加読み込み(UNICODE) */
long __stdcall MIDIInstrument_LoadAdditionalW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszFileName) {
	FILE* pFile = NULL;
	MIDIPatchNameTable* pMIDIPatchNameTable = NULL;
	MIDINoteNameTable* pMIDINoteNameTable = NULL;
	MIDIControllerNameTable* pMIDIControllerNameTable = NULL;
	MIDIRPNNameTable* pMIDIRPNNameTable = NULL;
	MIDINRPNNameTable* pMIDINRPNNameTable = NULL;
	MIDIInstrumentDefinition* pMIDIInstrumentDefinition = NULL;
	MIDIPatchNameTable* pTempPatchNameTable = NULL;
	MIDINoteNameTable* pTempNoteNameTable = NULL;
	MIDIControllerNameTable* pTempControllerNameTable = NULL;
	MIDIRPNNameTable* pTempRPNNameTable = NULL;
	MIDINRPNNameTable* pTempNRPNNameTable = NULL;
	long lNumber;
	wchar_t* pEqual = NULL;
	char cMode = 0;
	wchar_t szTextLine[256];
	wchar_t szTitle[256];
	wchar_t szName[256];
	memset (szTextLine, 0, sizeof (szTextLine));
	memset (szTitle, 0, sizeof (szTitle));
	memset (szName, 0, sizeof (szName));
	if (pMIDIInstrument == NULL) {
		return 0;
	}
	pFile = _wfopen (pszFileName, L"rt");
	if (pFile == NULL) {
		return 0;
	}
	while (!feof (pFile)) {
		/* 1行読み込み */
		memset (szTextLine, 0, sizeof (szTextLine));
		fgetws (szTextLine, 255, pFile);
		/* コメント (例: ";Noland SC-88Ero Instrument Definitions\n" */
		if (szTextLine[0] == ';') {
			continue;
		}
		/* モードセクション (例: ".Patch Name\n") */
		else if (szTextLine[0] == L'.') {
			if (wcsncmp (szTextLine, L".Patch Names", 12) == 0) {
				cMode = 1;
			}
			else if (wcsncmp (szTextLine, L".Note Names", 11) == 0) {
				cMode = 2;
			}
			else if (wcsncmp (szTextLine, L".Controller Names", 17) == 0) {
				cMode = 3;
			}
			else if (wcsncmp (szTextLine, L".RPN Names", 10) == 0) {
				cMode = 4;
			}
			else if (wcsncmp (szTextLine, L".NRPN Names", 11) == 0) {
				cMode = 5;
			}
			else if (wcsncmp (szTextLine, L".Instrument Definitions", 23) == 0) {
				cMode = 6;
			}
			else {
				cMode = 0;
			}
		}
		/* タイトルセクション (例: "[Noland SC-88Ero Capital Tones]\n" */
		else if (szTextLine[0] == L'[' ) {
			wchar_t* p = wcschr (szTextLine + 1, L']');
			memset (szTitle, 0, sizeof (szTitle));
			if (p) {
				wcsncpy (szTitle, szTextLine + 1, p - szTextLine - 1);
			}
			else {
				wcsncpy (szTitle, szTextLine + 1, 250);
			}
			pMIDIPatchNameTable = NULL;
			pMIDINoteNameTable = NULL;
			pMIDIControllerNameTable = NULL;
			pMIDIRPNNameTable = NULL;
			pMIDINRPNNameTable = NULL;
			pMIDIInstrumentDefinition = NULL;
			switch (cMode) {
			case 1: /* L".Patch Name" */
				pMIDIPatchNameTable = MIDIPatchNameTable_Create ();
				if (pMIDIPatchNameTable) {
					MIDIPatchNameTable_SetTitleW (pMIDIPatchNameTable, szTitle);
					MIDIInstrument_AddPatchNameTable (pMIDIInstrument, pMIDIPatchNameTable);
				}
				break;
			case 2: /* L".Note Name" */
				pMIDINoteNameTable = MIDINoteNameTable_Create ();
				if (pMIDINoteNameTable) {
					MIDINoteNameTable_SetTitleW (pMIDINoteNameTable, szTitle);
					MIDIInstrument_AddNoteNameTable (pMIDIInstrument, pMIDINoteNameTable);
				}
				break;
			case 3: /* L".Controller Name" */
				pMIDIControllerNameTable = MIDIControllerNameTable_Create ();
				if (pMIDIControllerNameTable) {
					MIDIControllerNameTable_SetTitleW (pMIDIControllerNameTable, szTitle);
					MIDIInstrument_AddControllerNameTable (pMIDIInstrument, pMIDIControllerNameTable);
				}
				break;
			case 4: /* L".RPN Name" */
				pMIDIRPNNameTable = MIDIRPNNameTable_Create ();
				if (pMIDIRPNNameTable) {
					MIDIRPNNameTable_SetTitleW (pMIDIRPNNameTable, szTitle);
					MIDIInstrument_AddRPNNameTable (pMIDIInstrument, pMIDIRPNNameTable);
				}
				break;
			case 5: /* L".NRPN Name" */
				pMIDINRPNNameTable = MIDINRPNNameTable_Create ();
				if (pMIDINRPNNameTable) {
					MIDINRPNNameTable_SetTitleW (pMIDINRPNNameTable, szTitle);
					MIDIInstrument_AddNRPNNameTable (pMIDIInstrument, pMIDINRPNNameTable);
				}
				break;
			case 6: /* L".Instrument Definition" */
				pMIDIInstrumentDefinition = MIDIInstrumentDefinition_Create ();
				if (pMIDIInstrumentDefinition) {
					MIDIInstrumentDefinition_SetTitleW (pMIDIInstrumentDefinition, szTitle);
					MIDIInstrument_AddInstrumentDefinition (pMIDIInstrument, pMIDIInstrumentDefinition);
				}
				break;
			}
		}
		/* 数字=名前(例: L"0=Piano1\n", L"Patch[0]=Noland SC-88Ero Drumsets\n") */
		else if (pEqual = wcschr (szTextLine, L'=')) {
			switch (cMode) {
			case 1: /* ".Patch Name" */
				if (wcsncmp (szTextLine, L"BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					forEachPatchNameTable (pMIDIInstrument, pTempPatchNameTable) {
						if (wcscmp (pTempPatchNameTable->m_pszTitle, szName) == 0) {
							pMIDIPatchNameTable->m_pBasedOn = pTempPatchNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempPatchNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : PatchNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = _wtoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					if (pMIDIPatchNameTable && 
						0 <= lNumber && lNumber < 128) {
						MIDIPatchNameTable_SetNameW 
							(pMIDIPatchNameTable, lNumber, szName);
					}
				}
				break;
			case 2: /* ".Note Name" */
				if (wcsncmp (szTextLine, L"BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					forEachNoteNameTable (pMIDIInstrument, pTempNoteNameTable) {
						if (wcscmp (pTempNoteNameTable->m_pszTitle, szName) == 0) {
							pMIDINoteNameTable->m_pBasedOn = pTempNoteNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempNoteNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : NoteNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = _wtoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					if (pMIDINoteNameTable &&
						0 <= lNumber && lNumber < 128) {
						MIDINoteNameTable_SetNameW 
							(pMIDINoteNameTable, lNumber, szName);
					}
				}
				break;
			case 3: /* ".Controller Name" */
				if (wcsncmp (szTextLine, L"BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					forEachControllerNameTable (pMIDIInstrument, pTempControllerNameTable) {
						if (wcscmp (pTempControllerNameTable->m_pszTitle, szName) == 0) {
							pMIDIControllerNameTable->m_pBasedOn = pTempControllerNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempControllerNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" :ControllerNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = _wtoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					if (pMIDIControllerNameTable &&
						0 <= lNumber && lNumber < 128) {
						MIDIControllerNameTable_SetNameW 
							(pMIDIControllerNameTable, lNumber, szName);
					}
				}
				break;
			case 4: /* ".RPN Name" */
				if (wcsncmp (szTextLine, L"BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					forEachRPNNameTable (pMIDIInstrument, pTempRPNNameTable) {
						if (wcscmp (pTempRPNNameTable->m_pszTitle, szName) == 0) {
							pMIDIRPNNameTable->m_pBasedOn = pTempRPNNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempRPNNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : RPNNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = _wtoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					if (pMIDIRPNNameTable &&
						0 <= lNumber && lNumber < 16384) {
						MIDIRPNNameTable_SetNameW 
							(pMIDIRPNNameTable, lNumber, szName);
					}
				}
				break;
			case 5: /* ".NRPN Name" */
				if (wcsncmp (szTextLine, L"BasedOn", 7) == 0) {
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					forEachNRPNNameTable (pMIDIInstrument, pTempNRPNNameTable) {
						if (wcscmp (pTempNRPNNameTable->m_pszTitle, szName) == 0) {
							pMIDINRPNNameTable->m_pBasedOn = pTempNRPNNameTable;
							break;
						}
					}
#ifdef _WINDOWS
					if (pTempNRPNNameTable == NULL) {
						_RPTF1 (_CRT_WARN, 
							"警告 : \"%s\" : NPRNNameが見つかりません。\n", szName);
					}
#endif
				}
				else {
					lNumber = _wtoi (szTextLine);
					memset (szName, 0, sizeof (szName));
					wcsncpy (szName, pEqual + 1, wcslen (pEqual + 1) - 1);
					if (pMIDINRPNNameTable &&
						0 <= lNumber && lNumber < 16384) {
						MIDINRPNNameTable_SetNameW 
							(pMIDINRPNNameTable, lNumber, szName);
					}
				}
				break;
			case 6: /* ".Instrument Definition" */
				memset (szTitle, 0, sizeof (szTitle));
				wcsncpy (szTitle, pEqual + 1, wcslen (pEqual + 1) - 1);
				/* "Control=(ControllerNameTable名) " */
				if (wcsncmp (szTextLine, L"Control=", 8) == 0) {
					if (pMIDIInstrumentDefinition) {
						/* 一致するControllerNameTable名を探索 */
						forEachControllerNameTable (pMIDIInstrument, pMIDIControllerNameTable) {
							if (wcscmp (pMIDIControllerNameTable->m_pszTitle, szTitle) == 0) {
								pMIDIInstrumentDefinition->m_pMIDIControllerNameTable = pMIDIControllerNameTable;
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDIControllerNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : ControllerNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* "RPN=(RPNNameTable名) " */
				else if (wcsncmp (szTextLine, L"RPN=", 4) == 0) {
					if (pMIDIInstrumentDefinition) {
						/* 一致するRPNNameTable名を探索 */
						forEachRPNNameTable (pMIDIInstrument, pMIDIRPNNameTable) {
							if (wcscmp (pMIDIRPNNameTable->m_pszTitle, szTitle) == 0) {
								pMIDIInstrumentDefinition->m_pMIDIRPNNameTable = pMIDIRPNNameTable;
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDIRPNNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : RPNNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* "NRPN=(NRPNNameTable名)" */
				else if (wcsncmp (szTextLine, L"NRPN=", 5) == 0) {
					if (pMIDIInstrumentDefinition) {
						/* 一致するNPRNNameTable名を探索 */
						forEachNRPNNameTable (pMIDIInstrument, pMIDINRPNNameTable) {
							if (wcscmp (pMIDINRPNNameTable->m_pszTitle, szTitle) == 0) {
								pMIDIInstrumentDefinition->m_pMIDINRPNNameTable = pMIDINRPNNameTable;
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDINRPNNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : NRPNNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* Patch[*]=(PatchNameTable名) */
				else if (wcsncmp (szTextLine, L"Patch[*", 7) == 0) {
					;
				}
				/* Patch[lBank]=(PatchNameTable名) */
				else if (wcsncmp (szTextLine, L"Patch[", 6) == 0) {
					long lBank = _wtoi (szTextLine + 6);
					if (pMIDIInstrumentDefinition &&
						0 <= lBank && lBank < 16384) {
						/* 一致するPatchNameTable名を探索 */
						forEachPatchNameTable (pMIDIInstrument, pMIDIPatchNameTable) {
							if (wcscmp (pMIDIPatchNameTable->m_pszTitle, szTitle) == 0) {
								MIDIInstrumentDefinition_SetPatchNameTable
									(pMIDIInstrumentDefinition, lBank, pMIDIPatchNameTable);
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDIPatchNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : PatchNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				/* Key[*,*]=(NoteNameTable名) */
				else if (wcsncmp (szTextLine, L"Key[*", 5) == 0) {
					;
				}
				/* Key[lBank,lProgramChange]=(NoteNameTable名) */
				else if (wcsncmp (szTextLine, L"Key[", 4) == 0) {
					long lBank = _wtoi (szTextLine + 4);
					long lProgramChange = 0;
					wchar_t* p = wcschr (szTextLine + 4, L',');
					if (p) { 
						 lProgramChange = _wtoi (p + 1);
					}
					if (pMIDIInstrumentDefinition &&
						0 <= lBank && lBank < 16384 &&
						0 <= lProgramChange && lProgramChange < 128) {
						/* 一致するNoteNameTable名を探索 */
						forEachNoteNameTable (pMIDIInstrument, pMIDINoteNameTable) {
							if (wcscmp (pMIDINoteNameTable->m_pszTitle, szTitle) == 0) {
								MIDIInstrumentDefinition_SetNoteNameTable
									(pMIDIInstrumentDefinition, lBank, lProgramChange, pMIDINoteNameTable);
								break;
							}
						}
						/* 見つからなかった */
#ifdef _WINDOWS
						if (pMIDINoteNameTable == NULL) {
							_RPTF1 (_CRT_WARN,
								"警告 : \"%s\" : NoteNameが見つかりません。\n", szTitle);
						}
#endif
					}
				}
				break;
			}
		}
	}
	fclose (pFile);
	pFile = NULL;
	return 1;
}

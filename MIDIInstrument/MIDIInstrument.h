/******************************************************************************/
/*                                                                            */
/*　MIDIInstrument.h - MIDIInstrumentヘッダーファイル      (C)2007-2012 くず  */
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

#ifndef _MIDIINSTRUMENT_H_
#define _MIDIINSTRUMENT_H_

/* C++からも使用可能とする */
#ifdef __cplusplus
extern "C" {
#endif

#define MIDIINSTRUMENTDEFINITION_MAXPATCHNAME 256
#define MIDIINSTRUMENTDEFINITION_MAXNOTENAME 256

/* MIDIPatchNameTable構造体 */
typedef struct tagMIDIPatchNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[128];
	long  m_lNameLen[128];
	struct tagMIDIPatchNameTable* m_pBasedOn;
	struct tagMIDIPatchNameTable* m_pNextPatchNameTable;
	struct tagMIDIPatchNameTable* m_pPrevPatchNameTable;
	void* m_pParent;
} MIDIPatchNameTable;

/* MIDINoteNameTable構造体 */
typedef struct tagMIDINoteNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[128];
	long  m_lNameLen[128];
	struct tagMIDINoteNameTable* m_pBasedOn;
	struct tagMIDINoteNameTable* m_pNextNoteNameTable;
	struct tagMIDINoteNameTable* m_pPrevNoteNameTable;
	void* m_pParent;
} MIDINoteNameTable;

/* MIDIContorollerName構造体 */
typedef struct tagMIDIControllerNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[128];
	long  m_lNameLen[128];
	struct tagMIDIControllerNameTable* m_pBasedOn;
	struct tagMIDIControllerNameTable* m_pNextControllerNameTable;
	struct tagMIDIControllerNameTable* m_pPrevControllerNameTable;
	void* m_pParent;
} MIDIControllerNameTable;

/* MIDIRPNNameTable構造体 */
typedef struct tagMIDIRPNNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[16384];
	long  m_lNameLen[16384];
	struct tagMIDIRPNNameTable* m_pBasedOn;
	struct tagMIDIRPNNameTable* m_pNextRPNNameTable;
	struct tagMIDIRPNNameTable* m_pPrevRPNNameTable;
	void* m_pParent;
} MIDIRPNNameTable;

/* MIDINRPNNameTable構造体 */
typedef struct tagMIDINRPNNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[16384];
	long  m_lNameLen[16384];
	struct tagMIDINRPNNameTable* m_pBasedOn;
	struct tagMIDINRPNNameTable* m_pNextNRPNNameTable;
	struct tagMIDINRPNNameTable* m_pPrevNRPNNameTable;
	void* m_pParent;
} MIDINRPNNameTable;

/* MIDIInstrumentDefinion構造体 */
typedef struct tagMIDIInstrumentDefinition {
	void* m_pszTitle;
	long m_lTitleLen;
	MIDIControllerNameTable* m_pMIDIControllerNameTable;
	MIDIRPNNameTable* m_pMIDIRPNNameTable;
	MIDINRPNNameTable* m_pMIDINRPNNameTable;
	long m_lMIDIPatchNameNum;
	long m_lMIDIPatchNameBank[MIDIINSTRUMENTDEFINITION_MAXPATCHNAME];
	MIDIPatchNameTable* m_pMIDIPatchNameTable[MIDIINSTRUMENTDEFINITION_MAXPATCHNAME];
	long m_lMIDINoteNameNum;
	long m_lMIDINoteNameBank[MIDIINSTRUMENTDEFINITION_MAXNOTENAME];
	long m_lMIDINoteNameProgram[MIDIINSTRUMENTDEFINITION_MAXNOTENAME];
	MIDINoteNameTable* m_pMIDINoteNameTable[MIDIINSTRUMENTDEFINITION_MAXNOTENAME];
	struct tagMIDIInstrumentDefinition* m_pNextInstrumentDefinition;
	struct tagMIDIInstrumentDefinition* m_pPrevInstrumentDefinition;
	void* m_pParent;
} MIDIInstrumentDefinition;

/* MIDIInstrument構造体 */
typedef struct tagMIDIInstrument {
	MIDIPatchNameTable* m_pFirstPatchNameTable;
	MIDIPatchNameTable* m_pLastPatchNameTable;
	MIDINoteNameTable* m_pFirstNoteNameTable;
	MIDINoteNameTable* m_pLastNoteNameTable;
	MIDIControllerNameTable *m_pFirstControllerNameTable;
	MIDIControllerNameTable *m_pLastControllerNameTable;
	MIDIRPNNameTable* m_pFirstRPNNameTable;
	MIDIRPNNameTable* m_pLastRPNNameTable;
	MIDINRPNNameTable* m_pFirstNRPNNameTable;
	MIDINRPNNameTable* m_pLastNRPNNameTable;
	MIDIInstrumentDefinition* m_pFirstInstrumentDefinition;
	MIDIInstrumentDefinition* m_pLastInstrumentDefinition;
} MIDIInstrument;

/******************************************************************************/
/*                                                                            */
/*　MIDIPatchNameTableクラス関数群                                            */
/*                                                                            */
/******************************************************************************/

/* MIDIPatchNameTableオブジェクトの生成 */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_Create ();

/* MIDIPatchNameTableオブジェクトの削除 */
void __stdcall MIDIPatchNameTable_Delete (MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIPatchNameTableオブジェクトのタイトルを設定 */
long __stdcall MIDIPatchNameTable_SetTitleA 
(MIDIPatchNameTable* pMIDIPatchNameTable, const char* pszTitle);
long __stdcall MIDIPatchNameTable_SetTitleW 
(MIDIPatchNameTable* pMIDIPatchNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIPatchNameTable_SetTitle MIDIPatchNameTable_SetTitleW
#else 
#define MIDIPatchNameTable_SetTitle MIDIPatchNameTable_SetTitleA
#endif

/* MIDIPatchNameTableオブジェクトの各名前を設定 */
long __stdcall MIDIPatchNameTable_SetNameA
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, const char* pszName);
long __stdcall MIDIPatchNameTable_SetNameW 
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDIPatchNameTable_SetName MIDIPatchNameTable_SetNameW
#else
#define MIDIPatchNameTable_SetName MIDIPatchNameTable_SetNameA
#endif

/* MIDIPatchNameTableオブジェクトのタイトルを取得 */
long __stdcall MIDIPatchNameTable_GetTitleA 
(MIDIPatchNameTable* pMIDIPatchNameTable, char* pszTitle, long lLen);
long __stdcall MIDIPatchNameTable_GetTitleW 
(MIDIPatchNameTable* pMIDIPatchNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIPatchNameTable_GetTitle MIDIPatchNameTable_GetTitleW
#else
#define MIDIPatchNameTable_GetTitle MIDIPatchNameTable_GetTitleA
#endif

/* MIDIPatchNameTableオブジェクトの各名前を取得 */
long __stdcall MIDIPatchNameTable_GetNameA
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDIPatchNameTable_GetNameW
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDIPatchNameTable_GetName MIDIPatchNameTable_GetNameW
#else
#define MIDIPatchNameTable_GetName MIDIPatchNameTable_GetNameA
#endif
 
/* 次のMIDIPatchNameTableへのポインタを取得(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_GetNextPatchNameTable 
(MIDIPatchNameTable* pMIDIPatchNameTable);

/* 前のMIDIPatchNameTableへのポインタを取得(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_GetPrevPatchNameTable 
(MIDIPatchNameTable* pMIDIPatchNameTable);

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIPatchNameTable_GetParent
(MIDIPatchNameTable* pMIDIPatchNameTable);

/******************************************************************************/
/*                                                                            */
/*　MIDINoteNameTableクラス関数群                                             */
/*                                                                            */
/******************************************************************************/

/* MIDINoteNameTableオブジェクトの生成 */
MIDINoteNameTable* __stdcall MIDINoteNameTable_Create ();

/* MIDINoteNameTableオブジェクトの削除 */
void __stdcall MIDINoteNameTable_Delete (MIDINoteNameTable* pMIDINoteNameTable);

/* MIDINoteNameTableオブジェクトのタイトルを設定 */
long __stdcall MIDINoteNameTable_SetTitleA
(MIDINoteNameTable* pMIDINoteNameTable, const char* pszTitle);
long __stdcall MIDINoteNameTable_SetTitleW
(MIDINoteNameTable* pMIDINoteNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDINoteNameTable_SetTitle MIDINoteNameTable_SetTitleW
#else
#define MIDINoteNameTable_SetTitle MIDINoteNameTable_SetTitleA
#endif

/* MIDINoteNameTableオブジェクトの各名前を設定 */
long __stdcall MIDINoteNameTable_SetNameA 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, const char* pszName);
long __stdcall MIDINoteNameTable_SetNameW 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDINoteNameTable_SetName MIDINoteNameTable_SetNameW
#else
#define MIDINoteNameTable_SetName MIDINoteNameTable_SetNameA
#endif

/* MIDINoteNameTableオブジェクトのタイトルを取得 */
long __stdcall MIDINoteNameTable_GetTitleA 
(MIDINoteNameTable* pMIDINoteNameTable, char* pszTitle, long lLen);
long __stdcall MIDINoteNameTable_GetTitleW
(MIDINoteNameTable* pMIDINoteNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDINoteNameTable_GetTitle MIDINoteNameTable_GetTitleW
#else
#define MIDINoteNameTable_GetTitle MIDINoteNameTable_GetTitleA
#endif

/* MIDINoteNameTableオブジェクトの各名前を取得 */
long __stdcall MIDINoteNameTable_GetNameA 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDINoteNameTable_GetNameW 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDINoteNameTable_GetName MIDINoteNameTable_GetNameW
#else
#define MIDINoteNameTable_GetName MIDINoteNameTable_GetNameA
#endif

/* 次のMIDINoteNameTableへのポインタを取得(なければNULL) */
MIDINoteNameTable* __stdcall MIDINoteNameTable_GetNextNoteNameTable 
(MIDINoteNameTable* pMIDINoteNameTable);

/* 前のMIDINoteNameTableへのポインタを取得(なければNULL) */
MIDINoteNameTable* __stdcall MIDINoteNameTable_GetPrevNoteNameTable 
(MIDINoteNameTable* pMIDINoteNameTable);

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDINoteNameTable_GetParent
(MIDINoteNameTable* pMIDINoteNameTable);

/******************************************************************************/
/*                                                                            */
/*　MIDIControllerNameTableクラス関数群                                       */
/*                                                                            */
/******************************************************************************/

/* MIDIControllerNameTableオブジェクトの生成 */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_Create ();

/* MIDIControllerNameTableオブジェクトの削除 */
void __stdcall MIDIControllerNameTable_Delete (MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIControllerNameTableオブジェクトのタイトルを設定 */
long __stdcall MIDIControllerNameTable_SetTitleA 
(MIDIControllerNameTable* pMIDIControllerNameTable, const char* pszTitle);
long __stdcall MIDIControllerNameTable_SetTitleW 
(MIDIControllerNameTable* pMIDIControllerNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIControllerNameTable_SetTitle MIDIControllerNameTable_SetTitleW
#else
#define MIDIControllerNameTable_SetTitle MIDIControllerNameTable_SetTitleA
#endif

/* MIDIControllerNameTableオブジェクトの各名前を設定 */
long __stdcall MIDIControllerNameTable_SetNameA 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, const char* pszName);
long __stdcall MIDIControllerNameTable_SetNameW 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDIControllerNameTable_SetName MIDIControllerNameTable_SetNameW
#else
#define MIDIControllerNameTable_SetName MIDIControllerNameTable_SetNameA
#endif

/* MIDIControllerNameTableオブジェクトのタイトルを取得 */
long __stdcall MIDIControllerNameTable_GetTitleA 
(MIDIControllerNameTable* pMIDIControllerNameTable, char* pszTitle, long lLen);
long __stdcall MIDIControllerNameTable_GetTitleW 
(MIDIControllerNameTable* pMIDIControllerNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIControllerNameTable_GetTitle MIDIControllerNameTable_GetTitleW
#else
#define MIDIControllerNameTable_GetTitle MIDIControllerNameTable_GetTitleA
#endif

/* MIDIControllerNameTableオブジェクトの各名前を取得 */
long __stdcall MIDIControllerNameTable_GetNameA 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDIControllerNameTable_GetNameW 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDIControllerNameTable_GetName MIDIControllerNameTable_GetNameW
#else
#define MIDIControllerNameTable_GetName MIDIControllerNameTable_GetNameA
#endif
 
/* 次のMIDIControllerNameTableへのポインタを取得(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_GetNextControllerNameTable 
(MIDIControllerNameTable* pMIDIControllerNameTable);

/* 前のMIDIControllerNameTableへのポインタを取得(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_GetPrevControllerNameTable 
(MIDIControllerNameTable* pMIDIControllerNameTable);

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIControllerNameTable_GetParent
(MIDIControllerNameTable* pMIDIControllerNameTable);

/******************************************************************************/
/*                                                                            */
/*　MIDIRPNNameTableクラス関数群                                              */
/*                                                                            */
/******************************************************************************/

/* MIDIRPNNameTableオブジェクトの生成 */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_Create ();

/* MIDIRPNNameTableオブジェクトの削除 */
void __stdcall MIDIRPNNameTable_Delete (MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIRPNNameTableオブジェクトのタイトルを設定 */
long __stdcall MIDIRPNNameTable_SetTitleA 
(MIDIRPNNameTable* pMIDIRPNNameTable, const char* pszTitle);
long __stdcall MIDIRPNNameTable_SetTitleW 
(MIDIRPNNameTable* pMIDIRPNNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIRPNNameTable_SetTitle MIDIRPNNameTable_SetTitleW
#else
#define MIDIRPNNameTable_SetTitle MIDIRPNNameTable_SetTitleA
#endif

/* MIDIRPNNameTableオブジェクトの各名前を設定 */
long __stdcall MIDIRPNNameTable_SetNameA 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, const char* pszName);
long __stdcall MIDIRPNNameTable_SetNameW 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDIRPNNameTable_SetName MIDIRPNNameTable_SetNameW
#else
#define MIDIRPNNameTable_SetName MIDIRPNNameTable_SetNameA
#endif

/* MIDIRPNNameTableオブジェクトのタイトルを取得 */
long __stdcall MIDIRPNNameTable_GetTitleA 
(MIDIRPNNameTable* pMIDIRPNNameTable, char* pszTitle, long lLen);
long __stdcall MIDIRPNNameTable_GetTitleW 
(MIDIRPNNameTable* pMIDIRPNNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIRPNNameTable_GetTitle MIDIRPNNameTable_GetTitleW
#else
#define MIDIRPNNameTable_GetTitle MIDIRPNNameTable_GetTitleA
#endif

/* MIDIRPNNameTableオブジェクトの各名前を取得 */
long __stdcall MIDIRPNNameTable_GetNameA 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDIRPNNameTable_GetNameW 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDIRPNNameTable_GetName MIDIRPNNameTable_GetNameW
#else
#define MIDIRPNNameTable_GetName MIDIRPNNameTable_GetNameA
#endif
 
/* 次のMIDIRPNNameTableへのポインタを取得(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_GetNextRPNNameTable 
(MIDIRPNNameTable* pMIDIRPNNameTable);

/* 前のMIDIRPNNameTableへのポインタを取得(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_GetPrevRPNNameTable 
(MIDIRPNNameTable* pMIDIRPNNameTable);

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIRPNNameTable_GetParent
(MIDIRPNNameTable* pMIDIRPNNameTable);

/******************************************************************************/
/*                                                                            */
/*　MIDINRPNNameTableクラス関数群                                             */
/*                                                                            */
/******************************************************************************/

/* MIDINRPNNameTableオブジェクトの生成 */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_Create ();

/* MIDINRPNNameTableオブジェクトの削除 */
void __stdcall MIDINRPNNameTable_Delete (MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDINRPNNameTableオブジェクトのタイトルを設定 */
long __stdcall MIDINRPNNameTable_SetTitleA 
(MIDINRPNNameTable* pMIDINRPNNameTable, const char* pszTitle);
long __stdcall MIDINRPNNameTable_SetTitleW 
(MIDINRPNNameTable* pMIDINRPNNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDINRPNNameTable_SetTitle MIDINRPNNameTable_SetTitleW
#else
#define MIDINRPNNameTable_SetTitle MIDINRPNNameTable_SetTitleA
#endif

/* MIDIRPNNameTableオブジェクトの各名前を設定 */
long __stdcall MIDINRPNNameTable_SetNameA 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, const char* pszName);
long __stdcall MIDINRPNNameTable_SetNameW 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDINRPNNameTable_SetName MIDINRPNNameTable_SetNameW
#else
#define MIDINRPNNameTable_SetName MIDINRPNNameTable_SetNameA
#endif

/* MIDINRPNNameTableオブジェクトのタイトルを取得 */
long __stdcall MIDINRPNNameTable_GetTitleA 
(MIDINRPNNameTable* pMIDINRPNNameTable, char* pszTitle, long lLen);
long __stdcall MIDINRPNNameTable_GetTitleW 
(MIDINRPNNameTable* pMIDINRPNNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDINRPNNameTable_GetTitle MIDINRPNNameTable_GetTitleW
#else
#define MIDINRPNNameTable_GetTitle MIDINRPNNameTable_GetTitleA
#endif

/* MIDIRPNNameTableオブジェクトの各名前を取得 */
long __stdcall MIDINRPNNameTable_GetNameA 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDINRPNNameTable_GetNameW 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDINRPNNameTable_GetName MIDINRPNNameTable_GetNameW
#else
#define MIDINRPNNameTable_GetName MIDINRPNNameTable_GetNameA
#endif
 
/* 次のMIDINRPNNameTableへのポインタを取得(なければNULL) */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_GetNextNRPNNameTable 
(MIDINRPNNameTable* pMIDINRPNNameTable);

/* 前のMIDINRPNNameTableへのポインタを取得(なければNULL) */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_GetPrevNRPNNameTable 
(MIDINRPNNameTable* pMIDINRPNNameTable);

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDINRPNNameTable_GetParent
(MIDINRPNNameTable* pMIDINRPNNameTable);


/******************************************************************************/
/*                                                                            */
/*　MIDIInstrumentDefinitionクラス関数群                                      */
/*                                                                            */
/******************************************************************************/

/* MIDIInstrumentDefinitionオブジェクトの生成 */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_Create ();

/* MIDIInstrumentDefinitionオブジェクトの削除 */
void __stdcall MIDIInstrumentDefinition_Delete (MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinitionオブジェクトのタイトルを設定 */
long __stdcall MIDIInstrumentDefinition_SetTitleA 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, const char* pszTitle);
long __stdcall MIDIInstrumentDefinition_SetTitleW 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrumentDefinition_SetTitle MIDIInstrumentDefinition_SetTitleW
#else
#define MIDIInstrumentDefinition_SetTitle MIDIInstrumentDefinition_SetTitleA
#endif

/* MIDIInstrumentDefinitionオブジェクトのコントローラー名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetControllerNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIInstrumentDefinitionオブジェクトのRPN名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIInstrumentDefinitionオブジェクトのNRPN名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetNRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDIInstrumentDefinitionオブジェクトのパッチ名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetPatchNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIInstrumentDefinitionオブジェクトのノート名テーブルを設定 */
long __stdcall MIDIInstrumentDefinition_SetNoteNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, long lProgram, MIDINoteNameTable* pMIDINoteNameTable);

/* MIDIInstrumentDefinitionオブジェクトのタイトルを取得 */
long __stdcall MIDIInstrumentDefinition_GetTitleA 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, char* pszTitle, long lLen);
long __stdcall MIDIInstrumentDefinition_GetTitleW 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIInstrumentDefinition_GetTitle MIDIInstrumentDefinition_GetTitleW
#else
#define MIDIInstrumentDefinition_GetTitle MIDIInstrumentDefinition_GetTitleA
#endif

/* MIDIInstrumentDefinitionオブジェクトのコントローラー名テーブルを取得 */
MIDIControllerNameTable* __stdcall MIDIInstrumentDefinition_GetControllerNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinitionオブジェクトのRPN名テーブルを取得 */
MIDIRPNNameTable* __stdcall MIDIInstrumentDefinition_GetRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinitionオブジェクトのNRPN名テーブルを取得 */
MIDINRPNNameTable* __stdcall MIDIInstrumentDefinition_GetNRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinitionオブジェクトのパッチ名テーブルを取得 */
MIDIPatchNameTable* __stdcall MIDIInstrumentDefinition_GetPatchNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank);

/* MIDIInstrumentDefinitionオブジェクトのノート名テーブルを取得 */
MIDINoteNameTable* __stdcall MIDIInstrumentDefinition_GetNoteNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, long lProgram);
 
/* 次のMIDIInstrumentDefinitionへのポインタを取得(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_GetNextInstrumentDefinition 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* 前のMIDIInstrumentDefinitionへのポインタを取得(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_GetPrevInstrumentDefinition 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* 親のMIDIInstrumentへのポインタを取得(なければNULL) */
MIDIInstrument* __stdcall MIDIInstrumentDefinition_GetParent
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);


/*******************************************************************************/
/*                                                                             */
/*　MIDIInstrumentクラス関数群                                                 */
/*                                                                             */
/*******************************************************************************/

/* MIDIInstrumentオブジェクトの生成 */
MIDIInstrument* __stdcall MIDIInstrument_Create ();

/* MIDIInstrumentオブジェクトの削除 */
void __stdcall MIDIInstrument_Delete (MIDIInstrument* pMIDIInstrument);

/* MIDIInstrumentにMIDIPatchNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertPatchNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIPatchNameTable* pMIDIPatchNameTable, MIDIPatchNameTable* pTargetPatchNameTable);

/* MIDIInstrumentにMIDIPatchNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddPatchNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIInstrumentからMIDIPatchNameTableを除去(MIDIPatchNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemovePatchNameTable
(MIDIInstrument* pMIDIInstrument, MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIInstrumentにMIDINoteNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertNoteNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDINoteNameTable* pMIDINoteNameTable, MIDINoteNameTable* pTargetNoteNameTable);

/* MIDIInstrumentにMIDINoteNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddNoteNameTable 
(MIDIInstrument* pMIDIInstrument, MIDINoteNameTable* pMIDINoteNameTable);

/* MIDIInstrumentからMIDINoteNameTableを除去(MIDINoteNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveNoteNameTable
(MIDIInstrument* pMIDIInstrument, MIDINoteNameTable* pMIDINoteNameTable);

/* MIDIInstrumentにMIDIControllerNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertControllerNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIControllerNameTable* pMIDIControllerNameTable, MIDIControllerNameTable* pTargetControllerNameTable);

/* MIDIInstrumentにMIDIControllerNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddControllerNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIInstrumentからMIDIControllerNameTableを除去(MIDIControllerNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveControllerNameTable
(MIDIInstrument* pMIDIInstrument, MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIInstrumentにMIDIRPNNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertRPNNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIRPNNameTable* pMIDIRPNNameTable, MIDIRPNNameTable* pTargetRPNNameTable);

/* MIDIInstrumentにMIDIRPNNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddRPNNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIInstrumentからMIDIRPNNameTableを除去(MIDIRPNNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveRPNNameTable
(MIDIInstrument* pMIDIInstrument, MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIInstrumentにMIDINRPNNameTableを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertNRPNNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDINRPNNameTable* pMIDINRPNNameTable, MIDINRPNNameTable* pTargetNRPNNameTable);

/* MIDIInstrumentにMIDINRPNNameTableを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddNRPNNameTable 
(MIDIInstrument* pMIDIInstrument, MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDIInstrumentからMIDINRPNNameTableを除去(MIDINRPNNameTable自体は削除しない) */
long __stdcall MIDIInstrument_RemoveNRPNNameTable
(MIDIInstrument* pMIDIInstrument, MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDIInstrumentにMIDIInstrumentDefinitionを挿入(挿入位置は指定ターゲットの直後) */
long __stdcall MIDIInstrument_InsertInstrumentDefinitionAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIInstrumentDefinition* pTargetInstrumentDefinition);

/* MIDIInstrumentにMIDIInstrumentDefinitionを追加(最後に挿入) */
long __stdcall MIDIInstrument_AddInstrumentDefinition 
(MIDIInstrument* pMIDIInstrument, MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentからMIDIInstrumentDefinitionを除去(MIDIInstrumentDefinition自体は削除しない) */
long __stdcall MIDIInstrument_RemoveInstrumentDefinition
(MIDIInstrument* pMIDIInstrument, MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* 最初のパッチ名テーブルへのポインタを返す(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetFirstPatchNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最後のパッチ名テーブルへのポインタを返す(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetLastPatchNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最初のノート名テーブルへのポインタを返す(なければNULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetFirstNoteNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最後のノート名テーブルへのポインタを返す(なければNULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetLastNoteNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最初のコントローラ名テーブルへのポインタを返す(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetFirstControllerNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最後のコントローラ名テーブルへのポインタを返す(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetLastControllerNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最初のRPN名テーブルへのポインタを返す(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetFirstRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最後のRPN名テーブルへのポインタを返す(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetLastRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最初のNRPN名テーブルへのポインタを返す(なければNULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetFirstNRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最後のNRPN名テーブルへのポインタを返す(なければNULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetLastNRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* 最初のインストゥルメント定義へのポインタを返す(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetFirstInstrumentDefinition
(MIDIInstrument* pMIDIInstrument);

/* 最後のインストゥルメント定義へのポインタを返す(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetLastInstrumentDefinition
(MIDIInstrument* pMIDIInstrument);

/* タイトルからMIDIPatchNameTableへのポインタを返す(なければNULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetPatchNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIPatchNameTable* __stdcall MIDIInstrument_GetPatchNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetPatchNameTableFromTitle MIDIInstrument_GetPatchNameTableFromTitleW
#else
#define MIDIInstrument_GetPatchNameTableFromTitle MIDIInstrument_GetPatchNameTableFromTitleA
#endif

/* タイトルからMIDINoteNameTableへのポインタを返す(なければNULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetNoteNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDINoteNameTable* __stdcall MIDIInstrument_GetNoteNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetNoteNameTableFromTitle MIDIInstrument_GetNoteNameTableFromTitleW
#else
#define MIDIInstrument_GetNoteNameTableFromTitle MIDIInstrument_GetNoteNameTableFromTitleA
#endif

/* タイトルからMIDIControllerNameTableへのポインタを返す(なければNULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetControllerNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIControllerNameTable* __stdcall MIDIInstrument_GetControllerNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetControllerNameTableFromTitle MIDIInstrument_GetControllerNameTableFromTitleW
#else
#define MIDIInstrument_GetControllerNameTableFromTitle MIDIInstrument_GetControllerNameTableFromTitleA
#endif

/* タイトルからMIDIRPNNameTableへのポインタを返す(なければNULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetRPNNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIRPNNameTable* __stdcall MIDIInstrument_GetRPNNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetRPNNameTableFromTitle MIDIInstrument_GetRPNNameTableFromTitleW
#else
#define MIDIInstrument_GetRPNNameTableFromTitle MIDIInstrument_GetRPNNameTableFromTitleA
#endif

/* タイトルからMIDINRPNNameTableへのポインタを返す(なければNULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetNRPNNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDINRPNNameTable* __stdcall MIDIInstrument_GetNRPNNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetNRPNNameTableFromTitle MIDIInstrument_GetNRPNNameTableFromTitleW
#else
#define MIDIInstrument_GetNRPNNameTableFromTitle MIDIInstrument_GetNRPNNameTableFromTitleA
#endif

/* タイトルからMIDIInstrumentDefinitionへのポインタを返す(なければNULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetInstrumentDefinitionFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetInstrumentDefinitionFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetInstrumentDefinitionFromTitle MIDIInstrument_GetInstrumentDefinitionFromTitleW
#else
#define MIDIInstrument_GetInstrumentDefinitionFromTitle MIDIInstrument_GetInstrumentDefinitionFromTitleA
#endif

/* Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを読み込み */
MIDIInstrument* __stdcall MIDIInstrument_LoadA (const char* pszFileName);
MIDIInstrument* __stdcall MIDIInstrument_LoadW (const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIInstrument_Load MIDIInstrument_LoadW
#else
#define MIDIInstrument_Load MIDIInstrument_LoadA
#endif

/* Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを追加読み込み */
long __stdcall MIDIInstrument_LoadAdditionalA (MIDIInstrument* pMIDIInstrument, const char* pszFileName);
long __stdcall MIDIInstrument_LoadAdditionalW (MIDIInstrument* pMIDIInstrument, const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIInstrument_LoadAdditional MIDIInstrument_LoadAdditionalW
#else
#define MIDIInstrument_LoadAdditional MIDIInstrument_LoadAdditionalA
#endif

/* forEachControllerNameTableマクロ */
#define forEachControllerNameTable(pMIDIInstrument,pMIDIControllerNameTable) \
	for(pMIDIControllerNameTable=pMIDIInstrument->m_pFirstControllerNameTable;\
		pMIDIControllerNameTable;\
		pMIDIControllerNameTable=pMIDIControllerNameTable->m_pNextControllerNameTable)

/* forEachRPNNameTableマクロ */
#define forEachRPNNameTable(pMIDIInstrument,pMIDIRPNNameTable) \
	for(pMIDIRPNNameTable=pMIDIInstrument->m_pFirstRPNNameTable;\
		pMIDIRPNNameTable;\
		pMIDIRPNNameTable=pMIDIRPNNameTable->m_pNextRPNNameTable)

/* forEachNRPNNameTableマクロ */
#define forEachNRPNNameTable(pMIDIInstrument,pMIDINRPNNameTable) \
	for(pMIDINRPNNameTable=pMIDIInstrument->m_pFirstNRPNNameTable;\
		pMIDINRPNNameTable;\
		pMIDINRPNNameTable=pMIDINRPNNameTable->m_pNextNRPNNameTable)

/* forEachPatchNameTableマクロ */
#define forEachPatchNameTable(pMIDIInstrument,pMIDIPatchNameTable) \
	for(pMIDIPatchNameTable=pMIDIInstrument->m_pFirstPatchNameTable;\
		pMIDIPatchNameTable;\
		pMIDIPatchNameTable=pMIDIPatchNameTable->m_pNextPatchNameTable)

/* forEachNoteNameTableマクロ */
#define forEachNoteNameTable(pMIDIInstrument,pMIDINoteNameTable) \
	for(pMIDINoteNameTable=pMIDIInstrument->m_pFirstNoteNameTable;\
		pMIDINoteNameTable;\
		pMIDINoteNameTable=pMIDINoteNameTable->m_pNextNoteNameTable)

/* forEachInstrumentDefinitionマクロ */
#define forEachInstrumentDefinition(pMIDIInstrument,pMIDIInstrumentDefinition) \
	for(pMIDIInstrumentDefinition=pMIDIInstrument->m_pFirstInstrumentDefinition;\
		pMIDIInstrumentDefinition;\
		pMIDIInstrumentDefinition=pMIDIInstrumentDefinition->m_pNextInstrumentDefinition)


#ifdef __cplusplus
}
#endif


#endif

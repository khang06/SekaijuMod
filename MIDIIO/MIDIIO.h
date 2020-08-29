/******************************************************************************/
/*                                                                            */
/*  MIDIIO.h - MIDIIOヘッダーファイル                      (C)2002-2012 くず  */
/*                                                                            */
/******************************************************************************/

/* このモジュールは普通のＣ言語で書かれている。 */
/* このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布される。 */
/* プロジェクトホームページ(仮)："http://openmidiproject.sourceforge.jp/index.html" */
/* MIDI入力オブジェクトのオープン・クローズ・リセット・データ受信(SYSX含む) */
/* MIDI出力オブジェクトのオープン・クローズ・リセット・データ送信(SYSX含む) */

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

#if !defined _MIDIIO_H_
#define _MIDIIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MIDIIO_BUFSIZE 16384
#define MIDIIO_SYSXNUM 4
#define MIDIIO_SYSXSIZE 4096
#define MIDIIO_MODEIN  0x0000
#define MIDIIO_MODEOUT 0x0001

/* 次のマクロを使用することにより、*/
/* 標準C言語のファイル入出力と同じ感覚でMIDI入出力を扱える。 */
#define MIDIIO_NONE "(None)"
#define MIDIIO_NONEJ "(なし)"
#define MIDIIO_DEFAULT "Default"
#define MIDIIO_DEFAULTJ "デフォルト"
#define MIDIIO_MIDIMAPPER "MIDI Mapper"
#define MIDIIO_MIDIMAPPERJ "MIDI マッパー"

#ifdef UNICODE 
#define _MIDIIO_T(x) L ## x
#else
#define _MIDIIO_T(x) (x)
#endif
#define mopen(NAME,MODE) (*(MODE)==_MIDIIO_T('r')?MIDIIn_Open(NAME):MIDIOut_Open(NAME))
#define mclose(MIDI) ((MIDI)->m_lMode==0?MIDIIn_Close(MIDI):MIDIOut_Close(MIDI))
#define mflush(MIDI) ((MIDI)->m_lMode==0?MIDIIn_Reset(MIDI):MIDIOut_Reset(MIDI))
#define mreopen(NAME,MODE,MIDI) (*(MODE)==_MIDIIO_T('r')?MIDIIn_ReOpen((MIDI),(NAME)):MIDIOut_ReOpen((MIDI),(NAME)))
#define mgets(PBUF,LEN,MIDI) (MIDIIn_GetMIDIMessage((MIDI),(PBUF),(LEN)))
#define mputs(PBUF,LEN,MIDI) (MIDIOut_PutMIDIMessage((MIDI),(PBUF),(LEN)))
#define mgetc(MIDI,CHAR) (MIDIIn_GetByte((MIDI),(CHAR)))
#define mputc(MIDI,CHAR) (MIDIOut_PutByte((MIDI),(CHAR)))
#define mread(PBUF,LEN,MIDI) (MIDIIn_GetBytes((MIDI),(PBUF),(LEN)))
#define mwrite(PBUF,LEN,MIDI) (MIDIOut_PutBytes((MIDI),(PBUF),(LEN)))

/* MIDI構造体 */
typedef struct tagMIDI {
	void* m_pDeviceHandle;
	void* m_pDeviceName; /* 20120124型と長さを修正 */
	long  m_lMode;
	void* m_pSysxHeader[MIDIIO_SYSXNUM];
	long  m_bStarting;
	unsigned char* m_pBuf;
	long  m_lBufSize;
	long  m_lReadPosition;
	long  m_lWritePosition;
	long  m_bBufLocked;
	unsigned char m_byRunningStatus;
} MIDI;

/* DLLMain */
//BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

/******************************************************************************/
/*                                                                            */
/*  MIDIOutクラス関数群                                                       */
/*                                                                            */
/******************************************************************************/

#define MIDIOut MIDI

/* MIDI出力デバイスの数を調べる */
long __stdcall MIDIOut_GetDeviceNum ();

/* MIDI出力デバイスの名前を調べる */
long __stdcall MIDIOut_GetDeviceNameA (long lID, char* pszDeviceName, long lLen);
long __stdcall MIDIOut_GetDeviceNameW (long lID, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIOut_GetDeviceName MIDIOut_GetDeviceNameW
#else
#define MIDIOut_GetDeviceName MIDIOut_GetDeviceNameA
#endif

/* MIDI出力デバイスを開く */
MIDIOut* __stdcall MIDIOut_OpenA (const char* pszDeviceName);
MIDIOut* __stdcall MIDIOut_OpenW (const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIOut_Open MIDIOut_OpenW
#else
#define MIDIOut_Open MIDIOut_OpenA
#endif

/* MIDI出力デバイスを閉じる */
long __stdcall MIDIOut_Close (MIDIOut* pMIDIDevice);

/* MIDI出力デバイスを再び開く */
MIDIOut* __stdcall MIDIOut_ReopenA (MIDIOut* pMIDIOut, const char* pszDeviceName);
MIDIOut* __stdcall MIDIOut_ReopenW (MIDIOut* pMIDIOut, const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIOut_Reopen MIDIOut_ReopenW
#else
#define MIDIOut_Reopen MIDIOut_ReopenA
#endif

/* MIDI出力デバイスをリセットする */
long __stdcall MIDIOut_Reset (MIDIOut* pMIDIDevice);

/* MIDI出力デバイスに1メッセージ出力する */
long __stdcall MIDIOut_PutMIDIMessage (MIDIOut* pMIDI, unsigned char* pMessage, long lLen);

/* MIDI出力デバイスに1バイトを出力する */
long __stdcall MIDIOut_PutByte (MIDIOut* pMIDI, unsigned char cByte);

/* MIDI出力デバイスに任意長のバイナリデータを出力する */
long __stdcall MIDIOut_PutBytes (MIDIOut* pMIDI, unsigned char* pBuf, long lLen);

/* このMIDI出力デバイスの名前を取得する(20120415追加) */
long __stdcall MIDIOut_GetThisDeviceNameA (MIDIOut* pMIDIOut, char* pszDeviceName, long lLen);
long __stdcall MIDIOut_GetThisDeviceNameW (MIDIOut* pMIDIOut, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIOut_GetThisDeviceName MIDIOut_GetThisDeviceNameW
#else
#define MIDIOut_GetThisDeviceName MIDIOut_GetThisDeviceNameA
#endif

/******************************************************************************/
/*                                                                            */
/*  MIDIInクラス関数群                                                        */
/*                                                                            */
/******************************************************************************/

#define MIDIIn  MIDI

/* MIDI入力デバイスの数を調べる */
long __stdcall MIDIIn_GetDeviceNum ();

/* MIDI入力デバイスの名前を調べる */
long __stdcall MIDIIn_GetDeviceNameA (long lID, char* pszDeviceName, long lLen);
long __stdcall MIDIIn_GetDeviceNameW (long lID, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIIn_GetDeviceName MIDIIn_GetDeviceNameW
#else
#define MIDIIn_GetDeviceName MIDIIn_GetDeviceNameA
#endif

/* MIDI入力デバイスを開く */
MIDIIn* __stdcall MIDIIn_OpenA (const char* pszDeviceName);
MIDIIn* __stdcall MIDIIn_OpenW (const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIIn_Open MIDIIn_OpenW
#else
#define MIDIIn_Open MIDIIn_OpenA
#endif

/* MIDI入力デバイスを閉じる */
long __stdcall MIDIIn_Close (MIDIIn* pMIDIDevice);

/* MIDI入力デバイスを再び開く */
MIDIIn* __stdcall MIDIIn_ReopenA (MIDIIn* pMIDIIn, const char* pszDeviceName);
MIDIIn* __stdcall MIDIIn_ReopenW (MIDIIn* pMIDIIn, const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIIn_Reopen MIDIIn_ReopenW
#else
#define MIDIIn_Reopen MIDIIn_ReopenA
#endif

/* MIDI入力デバイスをリセットする */
long __stdcall MIDIIn_Reset (MIDIIn* pMIDIDevice);

/* MIDI入力デバイスから1メッセージ入力する */
long __stdcall MIDIIn_GetMIDIMessage (MIDIIn* pMIDIIn, unsigned char* pMessage, long lLen);

/* MIDI入力デバイスから1バイト入力する */
long __stdcall MIDIIn_GetByte (MIDIIn* pMIDIIn, unsigned char* pByte);

/* MIDI入力デバイスから任意長のバイナリデータを入力する */
long __stdcall MIDIIn_GetBytes (MIDIIn* pMIDIIn, unsigned char* pBuf, long lLen);

/* このMIDI入力デバイスの名前を取得する(20120415追加) */
long __stdcall MIDIIn_GetThisDeviceNameA (MIDIIn* pMIDIIn, char* pszDeviceName, long lLen);
long __stdcall MIDIIn_GetThisDeviceNameW (MIDIIn* pMIDIIn, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIIn_GetThisDeviceName MIDIIn_GetThisDeviceNameW
#else
#define MIDIIn_GetThisDeviceName MIDIIn_GetThisDeviceNameA
#endif


#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/
/*                                                                            */
/*  MIDIClock.h - MIDIクロックヘッダーファイル             (C)2002-2012 くず  */
/*                                                                            */
/******************************************************************************/

/* このモジュールは普通のC言語とWin32APIを使って書かれている。 */
/* このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布される。 */
/* プロジェクトホームページ："http://openmidiproject.sourceforge.jp/index.html" */
/* MIDIクロックのスタート・ストップ・リセット */
/* テンポ・タイムベース・経過時刻・経過ティック数の取得・設定 */
/* タイムモード(TPQNBASE、SMPTE24BASE、SMPTE25BASE、SMPTE29BASE、SMPTE30BASE)に対応 */
/* スレーブモード(MIDIタイミングクロック、SMPTE/MTC)に対応 */

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

#if !defined _MIDICLOCK_H_
#define _MIDICLOCK_H_

/* C++からも使用可能とする */
#ifdef __cplusplus
extern "C" {
#endif

/* テンポ */
#define MIDICLOCK_TEMPO(U) (60000000/(U))
#define MIDICLOCK_MAXTEMPO 60000000
#define MIDICLOCK_MINTEMPO 1

/* タイムモード(以下の値のみ利用可能) */
#define MIDICLOCK_TPQNBASE         0 /* TPQNベース */
#define MIDICLOCK_SMPTE24BASE     24 /* SMPTE24ベース(24フレーム／秒) */
#define MIDICLOCK_SMPTE25BASE     25 /* SMPTE25ベース(25フレーム／秒) */
#define MIDICLOCK_SMPTE29BASE     29 /* SMPTE29ベース(29.97フレーム／秒) */
#define MIDICLOCK_SMPTE30BASE     30 /* SMPTE30ベース(30フレーム／秒) */

/* タイムモード(ver0.6以前との互換のために残してある) */
#define MIDICLOCK_MASTERTPQNBASE         0 /* TPQNベース */
#define MIDICLOCK_MASTERSMPTE24BASE     24 /* SMPTE24ベース(24フレーム／秒) */
#define MIDICLOCK_MASTERSMPTE25BASE     25 /* SMPTE25ベース(25フレーム／秒) */
#define MIDICLOCK_MASTERSMPTE29BASE     29 /* SMPTE29ベース(29.97フレーム／秒) */
#define MIDICLOCK_MASTERSMPTE30BASE     30 /* SMPTE30ベース(30フレーム／秒) */

/* MIDI入力同期モード(以下の値のみ利用可能) */
#define MIDICLOCK_MASTER                 0 /* マスター */
#define MIDICLOCK_SLAVEMIDITIMINGCLOCK   1 /* スレーブモード(MIDIタイミングクロック追従) */
#define MIDICLOCK_SLAVESMPTEMTC          2 /* スレーブモード(SMPTE/MTC追従) */

/* スピード[×0.01％] */
#define MIDICLOCK_SPEEDNORMAL        10000 /* スピード=100％ */
#define MIDICLOCK_SPEEDSLOW           5000 /* スピード=50％ */
#define MIDICLOCK_SPEEDFAST          20000 /* スピード=200％ */
#define MIDICLOCK_MINSPEED               0 /* スピード=静止 */
#define MIDICLOCK_MAXSPEED          100000 /* スピード=最大 */

/* MIDIClock構造体 */
typedef struct tagMIDIClock {
	long m_lTimeMode;        /* タイムモード */
	long m_lResolution;      /* 分解能[ティック/4分音符]又は[ティック/1フレーム] */
	long m_lTempo;           /* テンポ[μ秒/4分音符] */
	long m_lDummyTempo;      /* ダミーテンポ[μ秒/4分音符](スレーブモード時の仮保持用) */
	long m_lSpeed;           /* スピード(10000が標準、0=静止、20000が倍速) */
	long m_lMIDIInSyncMode;  /* MIDI入力同期モード(0=マスター、1=MIDIタイミングクロック、2=SMPTE/MTC) */
	long m_lPeriod;          /* コールバック関数呼び出し間隔[ミリ秒] */
	long m_lMillisec;        /* 時刻[ミリ秒] */
	long m_lMillisecMod;     /* 時刻誤差補正用 */
	long m_lOldMillisec;     /* 前回の時刻[ミリ秒]保持用 */
	long m_lDummyMillisec;   /* ダミー時刻[ミリ秒](スレーブモード時の仮保持用) */
	long m_lDummyMillisecMod;/* ダミー時刻誤差補正用(スレーブモード時の仮保持用) */
	long m_lOldDummyMillisec;/* 前回のダミー時刻[ミリ秒](スレーブモード時の仮保持用) */
	long m_lTickCount;       /* ティックカウント[Tick] */
	long m_lTickCountMod;    /* ティックカウント誤差補正用 */
	long m_lOldTickCount;    /* 前回のティックカウント[Tick]保持用 */
	long m_lTimerID;         /* タイマーID */
	unsigned char m_bySMPTE[8]; /* MIDIタイムコードクォーターフレーム保持用 */
	volatile long m_lRunning;   /* 1のとき動作中、0のとき停止中 */
	volatile long m_lLocked;    /* 1のときメンバ変数の操作禁止、0のとき許可 */
} MIDIClock;

/* MIDIクロックオブジェクトの削除 */
void __stdcall MIDIClock_Delete (MIDIClock* pMIDIClock);

/* MIDIクロックオブジェクトの生成 */
MIDIClock* __stdcall MIDIClock_Create (long lTimeMode, long lResolution, long lTempo);

/* タイムベース(タイムモードと分解能[ティック/4分音符]又は[ティック/1フレーム])の取得 */
long __stdcall MIDIClock_GetTimeBase (MIDIClock* pMIDIClock, long* pTimeMode, long* pResolution);

/* タイムベース(タイムモードと分解能[ティック/4分音符]又は[ティック/1フレーム])の設定 */
long __stdcall MIDIClock_SetTimeBase (MIDIClock* pMIDIClock, long lTimeMode, long lResolution);

/* MIDIテンポ[μ秒/4分音符]の取得 */
long __stdcall MIDIClock_GetTempo (MIDIClock* pMIDIClock);

/* MIDIテンポ[μ秒/4分音符]の設定 */
long __stdcall MIDIClock_SetTempo (MIDIClock* pMIDIClock, long lTempo);

/* スピード[×0.01％]の取得 */
long __stdcall MIDIClock_GetSpeed (MIDIClock* pMIDIClock);

/* スピード[×0.01％]の設定 */
long __stdcall MIDIClock_SetSpeed (MIDIClock* pMIDIClock, long lSpeed);

/* MIDI入力同期モードの取得 */
long __stdcall MIDIClock_GetMIDIInSyncMode (MIDIClock* pMIDIClock);

/* MIDI入力同期モードの設定 */
long __stdcall MIDIClock_SetMIDIInSyncMode (MIDIClock* pMIDIClock, long lMIDIInSyncMode);

/* スタート */
long __stdcall MIDIClock_Start (MIDIClock* pMIDIClock);

/* ストップ */
long __stdcall MIDIClock_Stop (MIDIClock* pMIDIClock);

/* リセット */
long __stdcall MIDIClock_Reset (MIDIClock* pMIDIClock);

/* 動作中か調べる */
long __stdcall MIDIClock_IsRunning (MIDIClock* pMIDIClock);

/* 経過時刻[ミリ秒]の取得 */
long __stdcall MIDIClock_GetMillisec (MIDIClock* pMIDIClock);

/* 経過時刻[ミリ秒]の設定 */
long __stdcall MIDIClock_SetMillisec (MIDIClock* pMIDIClock, long lMillisec);

/* 経過ティック数[tick]の取得 */
long __stdcall MIDIClock_GetTickCount (MIDIClock* pMIDIClock);

/* 経過ティック数[tick]の設定 */
long __stdcall MIDIClock_SetTickCount (MIDIClock* pMIDIClock, long lTickCount);


/* MIDIタイミングクロックを認識させる(0xF8) */
long __stdcall MIDIClock_PutMIDITimingClock (MIDIClock* pMIDIClock);

/* システムエクスクルーシヴによるSMPTE/MTCを認識させる */
long __stdcall MIDIClock_PutSysExSMPTEMTC 
	(MIDIClock* pMIDIClock, unsigned char cHour, 
	unsigned char cMinute, unsigned char cSecond, unsigned char cFrame);

/* MIDIタイムコードクォーターフレーム(0xF1)によるSMPTE/MTCを認識させる */
long __stdcall MIDIClock_PutSMPTEMTC (MIDIClock* pMIDIClock, unsigned char cSMPTEMTC);

/* MIDIクロックにMIDIメッセージを認識させる */
long __stdcall MIDIClock_PutMIDIMessage 
	(MIDIClock* pMIDIClock, unsigned char* pMessage, long lLen);


#ifdef __cplusplus
}
#endif

#endif

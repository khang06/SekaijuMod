/******************************************************************************/
/*                                                                            */
/*　MIDIClock.c - MIDIClockソースファイル                  (C)2002-2012 くず  */
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

#include <assert.h>
#include <stdlib.h>
#include <windows.h>
#include <crtdbg.h>
#include <mmsystem.h>
#include "MIDIClock.h"

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

/* MIDIクロックオブジェクトの削除 */
void __stdcall MIDIClock_Delete (MIDIClock* pMIDIClock) {
	if (pMIDIClock) {
		MIDIClock_Stop (pMIDIClock);
	}
	free (pMIDIClock);
	pMIDIClock = NULL;
}

/* MIDIクロックオブジェクトの生成 */
MIDIClock* __stdcall MIDIClock_Create (long lTimeMode, long lResolution, long lTempo) {
	MIDIClock* pMIDIClock;
	/* タイムモードの正当性チェック */
	if (lTimeMode != MIDICLOCK_TPQNBASE &&
		lTimeMode != MIDICLOCK_SMPTE24BASE &&
		lTimeMode != MIDICLOCK_SMPTE25BASE &&
		lTimeMode != MIDICLOCK_SMPTE29BASE &&
		lTimeMode != MIDICLOCK_SMPTE30BASE) {
		return NULL;
	}
	/* 分解能の正当性チェック */
	if (lTimeMode == MIDICLOCK_TPQNBASE) {
		if (lResolution <= 0 || lResolution > 32767) {
			return NULL;
		}
	}
	else {
		if (lResolution <= 0 || lResolution > 255) {
			return NULL;
		}
	}
	/* テンポの正当性チェック */
	if (lTempo < MIDICLOCK_MINTEMPO || lTempo > MIDICLOCK_MAXTEMPO) {
		return NULL;
	}
	/* オブジェクト用のメモリ確保 */
	pMIDIClock = calloc (1, sizeof (MIDIClock));
	if (pMIDIClock == NULL) {
		return NULL;
	}
	/* メンバ変数の値設定 */
	pMIDIClock->m_lTimeMode = lTimeMode;
	pMIDIClock->m_lResolution = lResolution;
	pMIDIClock->m_lTempo = lTempo;
	pMIDIClock->m_lDummyTempo = lTempo;
	pMIDIClock->m_lSpeed = MIDICLOCK_SPEEDNORMAL;
	pMIDIClock->m_lMIDIInSyncMode = MIDICLOCK_MASTER;
	pMIDIClock->m_lMillisec = 0;
	pMIDIClock->m_lMillisecMod = 0;
	pMIDIClock->m_lOldMillisec = 0;
	pMIDIClock->m_lDummyMillisec = 0;
	pMIDIClock->m_lTickCount = 0;
	pMIDIClock->m_lTickCountMod = 0;
	pMIDIClock->m_lOldTickCount = 0;
	pMIDIClock->m_lLocked = 0;
	return pMIDIClock;
}


/* コールバック関数 */
static void CALLBACK TimeProc (UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
	MIDIClock* pMIDIClock = (MIDIClock*)(dwUser);

	/* ロック */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	
	/* MIDI入力同期モードがマスターの場合 */
	if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_MASTER) { // 20090626修正
		long lDeltaMillisec = 0;
		long lDeltaTickCount = 0; 

		/* 経過ミリ秒の計算 */
		pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
		lDeltaMillisec = 
			(pMIDIClock->m_lMillisecMod + pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed) / MIDICLOCK_SPEEDNORMAL; 
		pMIDIClock->m_lMillisecMod += 
			(pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed - lDeltaMillisec * MIDICLOCK_SPEEDNORMAL);
		pMIDIClock->m_lMillisec += lDeltaMillisec;
		pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);

		/* 経過ティックカウントの計算 */
		pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
		switch (pMIDIClock->m_lTimeMode) {
		case MIDICLOCK_TPQNBASE:
			lDeltaTickCount = 
				(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
				pMIDIClock->m_lTempo;
			pMIDIClock->m_lTickCountMod += 
				(lDeltaMillisec * 1000 * pMIDIClock->m_lResolution - lDeltaTickCount * pMIDIClock->m_lTempo);
			pMIDIClock->m_lTickCount += lDeltaTickCount;
			break;
		case MIDICLOCK_SMPTE24BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 24;
			break;
		case MIDICLOCK_SMPTE25BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 25;
			break;	
		case MIDICLOCK_SMPTE29BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 2997 / 100;
			break;
		case MIDICLOCK_SMPTE30BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 30;
			break;
		}
		pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF);		
	}

	/* MIDI入力同期モードがMIDIタイミングクロックによるスレーブの場合 */
	else if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) { //20090701修正
		long lDeltaMillisec = 0;
		/* 経過ミリ秒の計算 */
		pMIDIClock->m_lOldDummyMillisec = pMIDIClock->m_lDummyMillisec;
		lDeltaMillisec = 
			(pMIDIClock->m_lDummyMillisecMod + pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed) / MIDICLOCK_SPEEDNORMAL; 
		pMIDIClock->m_lDummyMillisecMod += 
			(pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed - lDeltaMillisec * MIDICLOCK_SPEEDNORMAL);
		pMIDIClock->m_lDummyMillisec += lDeltaMillisec;
		pMIDIClock->m_lDummyMillisec = CLIP (0, pMIDIClock->m_lDummyMillisec, 0x7FFFFFFF);
	}


	/* ロック解除 */
	pMIDIClock->m_lLocked = 0;

}

/* タイムベース(タイムモードと分解能[ティック/4分音符]又は[ティック/1フレーム])の取得 */
long __stdcall MIDIClock_GetTimeBase (MIDIClock* pMIDIClock, long* pTimeMode, long* pResolution) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	*pTimeMode = pMIDIClock->m_lTimeMode;
	*pResolution = pMIDIClock->m_lResolution;
	return 1;
}

/* タイムベース(タイムモードと分解能[ティック/4分音符]又は[ティック/1フレーム])の設定 */
long __stdcall MIDIClock_SetTimeBase (MIDIClock* pMIDIClock, long lTimeMode, long lResolution) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	/* 動作モードの正当性チェック */
	if (lTimeMode != MIDICLOCK_TPQNBASE &&
		lTimeMode != MIDICLOCK_SMPTE24BASE &&
		lTimeMode != MIDICLOCK_SMPTE25BASE &&
		lTimeMode != MIDICLOCK_SMPTE29BASE &&
		lTimeMode != MIDICLOCK_SMPTE30BASE) {
		return 0;
	}
	/* 分解能の正当性チェック */
	if (lResolution <= 0 || lResolution >= 65536) {
		return 0;
	}
	/* タイムモード又は分解能が変更された時のみ */
	if (lTimeMode != pMIDIClock->m_lTimeMode || lResolution != pMIDIClock->m_lResolution) {
		while (pMIDIClock->m_lLocked);
		pMIDIClock->m_lLocked = 1;
		pMIDIClock->m_lTimeMode = lTimeMode;
		pMIDIClock->m_lResolution = lResolution;
		pMIDIClock->m_lTickCountMod = 0;
		pMIDIClock->m_lMillisecMod = 0;
		pMIDIClock->m_lLocked = 0;
		/* 注意：現在の経過時刻[ミリ秒]と経過ティックカウント[tick]は保持する。 */
	}
	return 1;
}

/* MIDIテンポ[μ秒/4分音符]の取得 */
long __stdcall MIDIClock_GetTempo (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) {
		return pMIDIClock->m_lDummyTempo;
	}
	return pMIDIClock->m_lTempo;
}

/* MIDIテンポ[μ秒/4分音符]の設定 */
long __stdcall MIDIClock_SetTempo (MIDIClock* pMIDIClock, long lTempo) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	if (lTempo < MIDICLOCK_MINTEMPO || lTempo > MIDICLOCK_MAXTEMPO) {
		return 0;
	}	
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	pMIDIClock->m_lTempo = lTempo;
	pMIDIClock->m_lDummyTempo = lTempo;
	pMIDIClock->m_lLocked = 0;
	return 1;
}

/* スピード[×0.01％]の取得 */
long __stdcall MIDIClock_GetSpeed (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	return pMIDIClock->m_lSpeed;
}

/* スピード[×0.01％]の設定 */
long __stdcall MIDIClock_SetSpeed (MIDIClock* pMIDIClock, long lSpeed) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	if (lSpeed < MIDICLOCK_MINSPEED || lSpeed > MIDICLOCK_MAXSPEED) {
		return 0;
	}
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	pMIDIClock->m_lSpeed = lSpeed;
	pMIDIClock->m_lLocked = 0;
	return 1;
}

/* MIDI入力同期モードの取得 */
long __stdcall MIDIClock_GetMIDIInSyncMode (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lMIDIInSyncMode;
}

/* MIDI入力同期モードの設定 */
long __stdcall MIDIClock_SetMIDIInSyncMode (MIDIClock* pMIDIClock, long lMIDIInSyncMode) {
	assert (pMIDIClock);
	if (lMIDIInSyncMode != MIDICLOCK_MASTER &&
		lMIDIInSyncMode != MIDICLOCK_SLAVEMIDITIMINGCLOCK &&
		lMIDIInSyncMode != MIDICLOCK_SLAVESMPTEMTC) {
		return 0;
	}
	/* MIDI入力同期モードが変更された時のみ */
	if (lMIDIInSyncMode != pMIDIClock->m_lMIDIInSyncMode) {
		while (pMIDIClock->m_lLocked);
		pMIDIClock->m_lLocked = 1;
		pMIDIClock->m_lMIDIInSyncMode = lMIDIInSyncMode;
		pMIDIClock->m_lTickCountMod = 0;
		pMIDIClock->m_lMillisecMod = 0;
		pMIDIClock->m_lDummyMillisec = pMIDIClock->m_lMillisec;
		pMIDIClock->m_lDummyMillisecMod = pMIDIClock->m_lMillisecMod;
		pMIDIClock->m_lOldDummyMillisec = pMIDIClock->m_lOldMillisec;
		pMIDIClock->m_lDummyTempo = pMIDIClock->m_lTempo;
		pMIDIClock->m_lLocked = 0;
		/* 注意：現在の経過時刻[ミリ秒]と経過ティックカウント[tick]は保持する。 */
	}
	return 1;
}

/* スタート */
long __stdcall MIDIClock_Start (MIDIClock* pMIDIClock) {
	TIMECAPS tc;
	long lPeriod; /* コールバック関数呼び出し間隔[ミリ秒] */
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* ロック */ 
	/* 注意：timeSetEvent中にコールバック関数内と同時ロックするとデッドロックする */
	//while (pMIDIClock->m_lLocked);
	//pMIDIClock->m_lLocked = 1;

	/* マルチメディアタイマー起動 */
	if (pMIDIClock->m_lTimerID == 0) {
		if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
			pMIDIClock->m_lLocked = 0;
			return 0;
		}
		lPeriod = tc.wPeriodMin;
		timeBeginPeriod (lPeriod);
		 pMIDIClock->m_lTimerID = timeSetEvent 
			(lPeriod, lPeriod, TimeProc, (DWORD)pMIDIClock, TIME_PERIODIC); 
		if(!pMIDIClock->m_lTimerID) {
			pMIDIClock->m_lLocked = 0;
			return 0;
		}
		pMIDIClock->m_lPeriod = lPeriod;
	}

	/* 動作状態に設定 */
	pMIDIClock->m_lRunning = 1;

	/* ロック解除 */
	//pMIDIClock->m_lLocked = 0;
    
	return 1;
}

/* ストップ */
long __stdcall MIDIClock_Stop (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* ロック */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	/* マルチメディアタイマー停止 */
    if (pMIDIClock->m_lTimerID != 0) {
	    timeKillEvent (pMIDIClock->m_lTimerID);
		timeEndPeriod (pMIDIClock->m_lPeriod);
		pMIDIClock->m_lTimerID = 0;
	}

	/* 停止状態に設定 */
	pMIDIClock->m_lRunning = 0;
	
	/* ロック解除 */
	pMIDIClock->m_lLocked = 0;

   	return 1;
}

/* リセット */
long __stdcall MIDIClock_Reset (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	
	/* ロック */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	/* メンバ変数の初期化 */
	pMIDIClock->m_lDummyTempo = pMIDIClock->m_lTempo;
	pMIDIClock->m_lMillisec = 0;
	pMIDIClock->m_lOldMillisec = 0;
	pMIDIClock->m_lMillisecMod = 0;
	pMIDIClock->m_lDummyMillisec = 0;
	pMIDIClock->m_lOldDummyMillisec = 0;
	pMIDIClock->m_lDummyMillisecMod = 0;
	pMIDIClock->m_lTickCount = 0;
	pMIDIClock->m_lTickCountMod = 0;
	pMIDIClock->m_lOldTickCount = 0;
	memset (pMIDIClock->m_bySMPTE, 0, sizeof (pMIDIClock->m_bySMPTE));

	/* ロック解除 */
	pMIDIClock->m_lLocked = 0;
	return 1;
}	

/* 動作中か調べる */
long __stdcall MIDIClock_IsRunning (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lRunning;
}

/* 経過時刻[ミリ秒]の取得 */
long __stdcall MIDIClock_GetMillisec (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lMillisec;
}

/* 経過時刻[ミリ秒]の設定 */
long __stdcall MIDIClock_SetMillisec (MIDIClock* pMIDIClock, long lMillisec) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	
	/* ロック */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	
	pMIDIClock->m_lMillisec = lMillisec;
	pMIDIClock->m_lOldMillisec = lMillisec;
	pMIDIClock->m_lMillisecMod = 0;
	pMIDIClock->m_lDummyMillisec = lMillisec;
	pMIDIClock->m_lOldDummyMillisec = lMillisec;
	pMIDIClock->m_lDummyMillisecMod = 0;

	/* ロック解除 */
	pMIDIClock->m_lLocked = 0;
	return 1;
}

/* 経過ティック数[tick]の取得 */
long __stdcall MIDIClock_GetTickCount (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lTickCount;		
}

/* 経過ティック数[tick]の設定 */
long __stdcall MIDIClock_SetTickCount (MIDIClock* pMIDIClock, long lTickCount) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* ロック */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	pMIDIClock->m_lTickCount = lTickCount;
	pMIDIClock->m_lOldTickCount = lTickCount;
	pMIDIClock->m_lTickCountMod = 0;

	/* ロック解除 */
	pMIDIClock->m_lLocked = 0;
	return 1;
}



/* MIDIタイミングクロックを認識させる(0xF8) */
long __stdcall MIDIClock_PutMIDITimingClock (MIDIClock* pMIDIClock) {
	long lDeltaTickCount = 0;
	long lDeltaMillisec = 0;
	long lOldDummyMillisec = 0;
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* MIDI入力同期モードがMIDIタイミングクロックによるスレーブ以外の場合は無効 */
	if (pMIDIClock->m_lMIDIInSyncMode != MIDICLOCK_SLAVEMIDITIMINGCLOCK) {
		return 0;
	}

	/* ロック */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	/* 経過ティックカウントを計算(20090626方式:分解能24未満にも対応) */
	pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
	lDeltaTickCount =
		(pMIDIClock->m_lTickCountMod + pMIDIClock->m_lResolution) / 24;
	pMIDIClock->m_lTickCountMod +=
		(pMIDIClock->m_lResolution - lDeltaTickCount * 24);
	pMIDIClock->m_lTickCount += lDeltaTickCount;
	pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF); 

	/* 現在のテンポを用いて経過ミリ秒を計算(20090626方式)(廃止) */
	/*pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
	lDeltaMillisec =
		(pMIDIClock->m_lMillisecMod + lDeltaTickCount * pMIDIClock->m_lTempo) / (pMIDIClock->m_lResolution * 1000);
	pMIDIClock->m_lMillisecMod +=
		(lDeltaTickCount * pMIDIClock->m_lTempo - lDeltaMillisec * (pMIDIClock->m_lResolution * 1000));
	pMIDIClock->m_lMillisec += lDeltaMillisec;
	pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);*/

	/* 現在の経過時刻を適用 */
	//lOldDummyMillisec = pMIDIClock->m_lDummyMillisec;
	pMIDIClock->m_lMillisec = pMIDIClock->m_lDummyMillisec;
	lDeltaMillisec = pMIDIClock->m_lMillisec - pMIDIClock->m_lOldMillisec;

	/* ダミーテンポを経過ティックカウントと経過時刻から計算 */
	pMIDIClock->m_lDummyTempo = 
		lDeltaMillisec  * 1000 * pMIDIClock->m_lResolution / lDeltaTickCount;
	pMIDIClock->m_lDummyTempo = CLIP (1, pMIDIClock->m_lDummyTempo, 60000000);

	/* ロック解除 */
	pMIDIClock->m_lLocked = 0;
	
	return 1;
}

/* システムエクスクルーシヴによるSMPTE/MTCを認識させる */
long __stdcall MIDIClock_PutSysExSMPTEMTC 
	(MIDIClock* pMIDIClock, unsigned char cHour, 
	unsigned char cMinute, unsigned char cSecond, unsigned char cFrame) {
	long lDeltaTickCount = 0;
	long lDeltaMillisec = 0;
	double dFrameLen[4] = {41.66666666, 40.00000000, 33.36670003, 33.33333333};
	long lFrameRate = (cHour >> 5) & 0x03;
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* MIDI入力同期モードがSMPTE/MTCによるスレーブ以外の場合は無効 */
	if (pMIDIClock->m_lMIDIInSyncMode != MIDICLOCK_SLAVESMPTEMTC) {
		return 0;
	}

	/* ロック */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	
	/* 経過ミリ秒の計算 */
	cHour = cHour & 0x1F;
	pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
	pMIDIClock->m_lMillisec = cHour * 3600000 + cMinute * 60000 + cSecond * 1000 +
		((long)(dFrameLen[lFrameRate] * cFrame));
	lDeltaMillisec = pMIDIClock->m_lMillisec - pMIDIClock->m_lOldMillisec;
	pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);

	/* 経過ティックカウントの計算 */
	pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
	switch (pMIDIClock->m_lTimeMode) {
	case MIDICLOCK_TPQNBASE:
		if (lDeltaMillisec >= 0) {
			lDeltaTickCount = 
				(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
				pMIDIClock->m_lTempo;
			pMIDIClock->m_lTickCountMod += 
				(lDeltaMillisec * 1000 * pMIDIClock->m_lResolution - lDeltaTickCount * pMIDIClock->m_lTempo);
			pMIDIClock->m_lTickCount += lDeltaTickCount;
		}
		else {
			lDeltaTickCount = 
				(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
				pMIDIClock->m_lTempo;
			pMIDIClock->m_lTickCountMod = 0;
			pMIDIClock->m_lTickCount += lDeltaTickCount;
		}
		break;
	case MIDICLOCK_SMPTE24BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 24;
		break;
	case MIDICLOCK_SMPTE25BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 25;
		break;	
	case MIDICLOCK_SMPTE29BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 2997 / 100;
		break;
	case MIDICLOCK_SMPTE30BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 30;
		break;
	}
	pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF); 

	/* ロック解除 */
	pMIDIClock->m_lLocked = 0;

	return 1;
}

/* MIDIタイムコードクォーターフレーム(0xF1)によるSMPTE/MTCを認識させる */
long __stdcall MIDIClock_PutSMPTEMTC (MIDIClock* pMIDIClock, unsigned char cSMPTEMTC) {
	long k;
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* MIDI入力同期モードがSMPTE/MTCによるスレーブ以外の場合は無効 */
	if (pMIDIClock->m_lMIDIInSyncMode != MIDICLOCK_SLAVESMPTEMTC) {
		return 0;
	}

	k = (cSMPTEMTC >> 4) & 0x07;
	pMIDIClock->m_bySMPTE[k] = cSMPTEMTC;

	/* 8番目のメッセージを受信した */
	if (k == 7) {
		long lDeltaTickCount = 0;
		long lDeltaMillisec = 0;
		double dFrameLen[4] = {41.66666666, 40.00000000, 33.36670003, 33.33333333};
		long lFrameRate = (pMIDIClock->m_bySMPTE[7] & 0x0E) >> 1;
		long lHour =   ((pMIDIClock->m_bySMPTE[7] & 0x01) << 4) | (pMIDIClock->m_bySMPTE[6] & 0x0F);
		long lMinute = ((pMIDIClock->m_bySMPTE[5] & 0x0F) << 4) | (pMIDIClock->m_bySMPTE[4] & 0x0F);
		long lSecond = ((pMIDIClock->m_bySMPTE[3] & 0x0F) << 4) | (pMIDIClock->m_bySMPTE[2] & 0x0F);
		long lFrame =  ((pMIDIClock->m_bySMPTE[1] & 0x0F) << 4) | (pMIDIClock->m_bySMPTE[0] & 0x0F);

		/* ロック */
		while (pMIDIClock->m_lLocked);
		pMIDIClock->m_lLocked = 1;
		
		/* 経過ミリ秒の計算 */
		pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
		pMIDIClock->m_lMillisec = lHour * 3600000 + lMinute * 60000 + lSecond * 1000 +
			((long)(dFrameLen[lFrameRate] * lFrame));
		lDeltaMillisec = pMIDIClock->m_lMillisec - pMIDIClock->m_lOldMillisec;
		pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);

		/* 経過ティックカウントの計算 */
		pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
		switch (pMIDIClock->m_lTimeMode) {
		case MIDICLOCK_TPQNBASE:
			if (lDeltaTickCount >= 0) {
				lDeltaTickCount = 
					(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
					pMIDIClock->m_lTempo;
				pMIDIClock->m_lTickCountMod += 
					(lDeltaMillisec * 1000 * pMIDIClock->m_lResolution - lDeltaTickCount * pMIDIClock->m_lTempo);
				pMIDIClock->m_lTickCount += lDeltaTickCount;
			}
			else {
				lDeltaTickCount = 
					(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
					pMIDIClock->m_lTempo;
				pMIDIClock->m_lTickCountMod = 0;
				pMIDIClock->m_lTickCount += lDeltaTickCount;
			}
			break;
		case MIDICLOCK_SMPTE24BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 24;
			break;
		case MIDICLOCK_SMPTE25BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 25;
			break;	
		case MIDICLOCK_SMPTE29BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 2997 / 100;
			break;
		case MIDICLOCK_SMPTE30BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 30;
			break;
		}
		pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF); 

		/* ロック解除 */
		pMIDIClock->m_lLocked = 0;

	
	}
	return 1;
}

/* MIDIクロックにMIDIメッセージを認識させる */
long __stdcall MIDIClock_PutMIDIMessage 
(MIDIClock* pMIDIClock, unsigned char* pMIDIMessage, long lLen) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	
	/* システムエクスクルーシヴメッセージ(0xF0) */
	if (*pMIDIMessage == 0xF0) {
		unsigned char bySysExSMPTEMTC[10] = 
			{0xF0, 0x7F, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF7};
		/* システムエクスクルーシヴによるSMPTE/MTC */
		if (memcmp (pMIDIMessage, bySysExSMPTEMTC, 5) == 0) {
			/* MIDI入力同期モードがSMPTE/MTCによるスレーブの場合のみ */
			if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVESMPTEMTC) {
				return MIDIClock_PutSysExSMPTEMTC (pMIDIClock, *(pMIDIMessage + 5), 
					*(pMIDIMessage + 6), *(pMIDIMessage + 7), *(pMIDIMessage + 8));
			}
		}
	}

	/* MIDIタイムコードクォーターフレーム(0xF1) */
	else if (*pMIDIMessage == 0xF1) {
		/* MIDI入力同期モードがSMPTE/MTCによるスレーブの場合のみ */
		if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVESMPTEMTC) {
			return MIDIClock_PutSMPTEMTC (pMIDIClock, *(pMIDIMessage + 1));
		}
	}

	/* ソングポジションセレクタ(0xF2) */
	else if (*pMIDIMessage == 0xF2) {
		/* MIDI入力同期モードがMIDIタイミングクロックによるスレーブの場合のみ */
		if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) { // TODO:保留
			long lVal = ((pMIDIMessage[2] & 0x7F) << 7) | (pMIDIMessage[1] & 0x7F);
			long lTickCount = lVal * pMIDIClock->m_lResolution / 4;
			MIDIClock_SetTickCount (pMIDIClock, lTickCount);
		}
	}

	/* MIDIタイミングクロック(0xF8) */
	else if (*pMIDIMessage == 0xF8) {
		/* MIDI入力同期モードがMIDIタイミングクロックによるスレーブの場合のみ */
		if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) {
			if (pMIDIClock->m_lRunning) {
				return MIDIClock_PutMIDITimingClock (pMIDIClock);
			}
		}
	}

	/* スタート(0xFA) */
	else if (*pMIDIMessage == 0xFA) {
		MIDIClock_SetTickCount (pMIDIClock, 0);
		MIDIClock_SetMillisec (pMIDIClock, 0);
		MIDIClock_Start (pMIDIClock);
	}

	/* コンティニュー(0xFB) */
	else if (*pMIDIMessage == 0xFB) {
		MIDIClock_Start (pMIDIClock);
	}

	/* ストップ(0xFC) */
	else if (*pMIDIMessage == 0xFC) {
		MIDIClock_Stop (pMIDIClock);
	}

	return 1;
}

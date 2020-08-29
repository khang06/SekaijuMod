/******************************************************************************/
/*                                                                            */
/*　MIDIStatus.h - MIDIStatusヘッダーファイル              (C)2002-2013 くず  */
/*                                                                            */
/******************************************************************************/

/* このモジュールは普通のＣ言語で書かれている。 */
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

#ifndef _MIDISTATUS_H_
#define _MIDISTATUS_H_

#define MIDISTATUS_MAXMIDIPART      128
#define MIDISTATUS_MAXMIDIDRUMSETUP 128

#define MIDISTATUS_MODENATIVE 0x00000000
#define MIDISTATUS_MODEGM     0x7E000001
#define MIDISTATUS_MODEGM2    0x7E000003
#define MIDISTATUS_MODEGS     0x41000002
#define MIDISTATUS_MODE88     0x41000003
#define MIDISTATUS_MODEXG     0x43000002
	/* #define MIDISTATUS_MODE????を追加する場合は、 */
	/* 上位24ビットはMMAから割り当てられたメーカーコードとし、 */
	/* 下位8ビットは各メーカー内の自由な識別コードとする。 */

/* C++からも使用可能とする */
#ifdef __cplusplus
extern "C" {
#endif

/* MIDIPart構造体 */
/* コメントの< >は一般的なデフォルト値を示す */
typedef struct tagMIDIPart {
	void* m_pParent;
	long m_lPartMode;             /* 0=ノーマル,1=ドラム1,2=ドラム2,3=ドラム3,4=ドラム4 */
	long m_lOmniMonoPolyMode;     /* 1=OmniOn_Poly、2=OmniOn_Mono、<3>=OmniOff_Poly、4=OmniOff_Mono */
	long m_lChannelFineTuning;    /* 0～<8192>～16383 100/8192cent単位 RPN#1で設定 */
	long m_lChannelCoarseTuning;  /* 40～<64>～88 半音単位 チャンネルキーシフトとも言う RPN#2で設定 */
	long m_lPitchBendSensitivity; /* 0～<2>～24 半音単位 RPN#0で設定 */
	long m_lModulationDepthRange; /* 0～???? RPN#5で設定 */
	long m_lReceiveChannel;       /* 0～15,16 デフォルト値はパート番号に一致 SysExで設定(GS/88/XGのみ) */
	long m_lVelocitySenseDepth;   /* 0～<64>～127 SysExで設定(GS/88/XGのみ) */
	long m_lVelocitySenseOffset;  /* 0～<64>～127 SysExで設定(GS/88/XGのみ) */
	long m_lKeyboardRangeLow;     /* ノートリミットローとも言う <0>～127 SysExで設定(GS/88/XGのみ) */
	long m_lKeyboardRangeHigh;    /* ノートリミットハイとも言う 0～<127> SysExで設定(GS/88/XGのみ) */
	long m_lAssignableControler1Num; /* アサイナブルコントローラー1に使用するコントロールナンバー */
	long m_lAssignableControler2Num; /* アサイナブルコントローラー2に使用するコントロールナンバー */
	long m_lReserved1;            /* 将来拡張用1(勝手に使ってはならない) */
	long m_lReserved2;            /* 将来拡張用2(勝手に使ってはならない) */
	long m_lScaleOctaveTuning[12]; /* 0～<64>～127 0.1cent単位 */
	/* コントローラーディスティネーションセッティング */
	unsigned char m_cModulationDest[16];           /* モジュレーションを使った時の効果定義 */
	unsigned char m_cPitchBendDest[16];            /* ピッチベンドを使った時の効果定義 */
	unsigned char m_cChannelAfterTouchDest[16];    /* チャンネルアフタータッチを使った時の効果定義 */
	unsigned char m_cKeyAfterTouchDest[16];        /* キーアフタータッチを使った時の効果定義 */
	unsigned char m_cAssignableControler1Dest[16]; /* アサイナブルコントローラー1を使った時の効果定義 */
	unsigned char m_cAssignableControler2Dest[16]; /* アサイナブルコントローラー2を使った時の効果定義 */
	/* チャンネルボイス領域 */
	unsigned char m_cNote[128];          /* 各々<0>～127 値は打鍵時のベロシティ */
	unsigned char m_cNoteKeep[128];      /* 各々<0>～127 値は打鍵時のベロシティ */
	unsigned char m_cKeyAfterTouch[128]; /* 各々<0>～127 */
	unsigned char m_cControlChange[128]; /* 各々0～127 デフォルト値はCC#による。 */
	long m_lProgramChange;        /* <0>(AcousticGrandPiano)～127(Gunshot) */
	long m_lChannelAfterTouch;    /* <0>～127 */
	long m_lPitchBend;            /* 0～<8192>～16383 */
	/* ユーザー用拡張領域 */
	long m_lUser1;
	long m_lUser2;
	long m_lUser3;
	long m_lUserFlag;
	/* RPN/NRPN完全保存領域(普通は保存しない) */
#ifdef MIDISTATUS_RECORDNRPN
	unsigned char m_cNRPNMSB[128][128];
	unsigned char m_cNRPNLSB[128][128];
#endif
#ifdef MIDISTATUS_RECORDRPN
	unsigned char m_cRPNMSB[128][128];
	unsigned char m_cRPNLSB[128][128];
#endif
} MIDIPart;


/* MIDIDrumSetup構造体 */
/* コメントの< >は一般的なデフォルト値を示す */
typedef struct tagMIDIDrumSetup {
	void* m_pParent;
	unsigned char m_cDrumCutoffFrequency[128]; /* 各々0～<64>～127 NRPN#(20*128)台又はSysExで設定(XG) */
	unsigned char m_cDrumResonance[128];       /* 各々0～<64>～127 NRPN#(21*128)台又はSysExで設定(XG) */
	unsigned char m_cDrumAttackTime[128];      /* 各々0～<64>～127 NRPN#(22*128)台又はSysExで設定(XG) */
	unsigned char m_cDrumDecay1Time[128];      /* 各々0～<64>～127 NRPN#(23*128)台又はSysExで設定(XG) */
	unsigned char m_cDrumDecay2Time[128];      /* 各々0～<64>～127 NRPN#(23*128)台又はSysExで設定(XG) */
	unsigned char m_cDrumPitchCoarse[128];     /* 各々0～<64>～127 NRPN#(24*128)台又はSysExで設定(GS/88/XG) */
	unsigned char m_cDrumPitchFine[128];       /* 各々0～<64>～127 NRPN#(25*128)台又はSysExで設定(XG) */
	unsigned char m_cDrumVolume[128];          /* 各々0～<64>～127 NRPN#(26*128)台又はSysExで設定(GS/88/XG/GM2) */
	unsigned char m_cDrumPan[128];             /* 各々0～<64>～127 NRPN#(28*128)台又はSysExで設定(GS/88/XG/GM2) */
	unsigned char m_cDrumReverb[128];          /* 各々0～<64>～127 NRPN#(29*128)台又はSysExで設定(GS/88/XG/GM2) */
	unsigned char m_cDrumChorus[128];          /* 各々0～<64>～127 NRPN#(30*128)台又はSysExで設定(GS/88/XG/GM2) */
	unsigned char m_cDrumDelay[128];           /* 各々0～<64>～127 NRPN#(31*128)台又はSysExで設定(GS/88/XG) */
	long m_lUser1;
	long m_lUser2;
	long m_lUser3;
	long m_lUserFlag;
} MIDIDrumSetup;



/* MIDIStatus構造体 */
/* コメントの< >は一般的なデフォルト値を示す */
typedef struct tagMIDIStatus {
	long m_lModuleMode;           /* 0x00000000=NATIVE, 0x7E000002=GM,   0x7E000003=GM2, */
	                              /* 0x41000002=GS,   0x41000003=88,   0x43000002=XG */
	long m_lMasterFineTuning;     /* 24～<1024>～2024(GS), 0～<1024>～2048(XG), 0～<8192>～16383(その他) */
	long m_lMasterCoarseTuning;   /* 40～<64>～88 半音単位 マスターキーシフトとも言う */
	long m_lMasterBalance;        /* この値は使われない。 */
	long m_lMasterVolume;         /* 0～<127> SysExで設定 */
	long m_lMasterPan;            /* 1～<64>～127 SysExで設定 */
	long m_lMasterReverb[32];     /* 各々の意味と範囲はm_lModeの値により異なる(公式ガイドブック参照) */
	long m_lMasterChorus[32];     /* 各々の意味と範囲はm_lModeの値により異なる(公式ガイドブック参照) */
	long m_lMasterDelay[32];      /* 各々の意味と範囲はm_lModeの値により異なる(公式ガイドブック参照) */
	long m_lMasterEqualizer[32];  /* 各々の意味と範囲はm_lModeの値により異なる(公式ガイドブック参照) */
	long m_lMasterInsertion[32];  /* この値は使われない。 */
	long m_lNumMIDIPart;          /* MIDIPartの数(=16) */
	long m_lNumMIDIDrumSetup;     /* MIDIDrumSetupの数(=2or4) */
	MIDIPart* m_pMIDIPart[MIDISTATUS_MAXMIDIPART];
	MIDIDrumSetup* m_pMIDIDrumSetup[MIDISTATUS_MAXMIDIDRUMSETUP];
	long m_lUser1;
	long m_lUser2;
	long m_lUser3;
	long m_lUserFlag;
	long m_lRunningStatus;
} MIDIStatus;




/******************************************************************************/
/*                                                                            */
/* MIDIPart関数群                                                             */
/*                                                                            */
/******************************************************************************/

/* MIDIPartオブジェクトを削除する。 */
int __stdcall MIDIPart_Delete (MIDIPart* pMIDIPart);

/* MIDIPartオブジェクトを生成する。 */
MIDIPart* __stdcall MIDIPart_Create (MIDIStatus* pParent);

/* MIDIPart_Get系関数 */

/* 現在のパートモードを取得 */
long __stdcall MIDIPart_GetPartMode (MIDIPart* pMIDIPart);

/* 現在のオムニモノポリモードを取得 */
long __stdcall MIDIPart_GetOmniMonoPolyMode (MIDIPart* pMIDIPart);

/* 現在のチャンネルファインチューニングの値を取得 */
long __stdcall MIDIPart_GetChannelFineTuning (MIDIPart* pMIDIPart);

/* 現在のチャンネルコースチューニングの値を取得 */
long __stdcall MIDIPart_GetChannelCoarseTuning (MIDIPart* pMIDIPart);

/* 現在のピッチベンドセンシティビティの値を取得 */
long __stdcall MIDIPart_GetPitchBendSensitivity (MIDIPart* pMIDIPart);

/* 現在のモジュレーションデプスレンジの値を取得 */
long __stdcall MIDIPart_GetModulationDepthRange (MIDIPart* pMIDIPart);

/* 現在のレシーブチャンネルの値を取得 */
long __stdcall MIDIPart_GetReceiveChannel (MIDIPart* pMIDIPart);

/* 現在のベロシティセンスデプスの値を取得 */
long __stdcall MIDIPart_GetVelocitySenseDepth (MIDIPart* pMIDIPart);

/* 現在のベロシティセンスオフセットの値を取得 */
long __stdcall MIDIPart_GetVelocitySenseOffset (MIDIPart* pMIDIPart);

/* 現在のキーボードレンジローの値を取得 */
long __stdcall MIDIPart_GetKeyboardRangeLow (MIDIPart* pMIDIPart);

/* 現在のキーボードレンジハイの値を取得 */
long __stdcall MIDIPart_GetKeyboardRangeHigh (MIDIPart* pMIDIPart);

/* 現在の鍵盤の押され具合を取得する。 */
/* 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ) */
long __stdcall MIDIPart_GetNote (MIDIPart* pMIDIPart, long lKey);

/* 現在の鍵盤の押され具合をまとめて取得する。 */
/* 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ) */
long __stdcall MIDIPart_GetNoteEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* 現在の鍵盤の押され具合を取得する。 */
/* 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ) */
long __stdcall MIDIPart_GetNoteKeep (MIDIPart* pMIDIPart, long lKey);

/* 現在の鍵盤の押され具合(ホールド持続含む)をまとめて取得する。 */
/* 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ) */
long __stdcall MIDIPart_GetNoteKeepEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* 現在のキーアフタータッチの値を取得 */
long __stdcall MIDIPart_GetKeyAfterTouch (MIDIPart* pMIDIPart, long lKey);

/* 現在のキーアフタータッチの値をまとめて取得 */
long __stdcall MIDIPart_GetKeyAfterTouchEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* 現在のコントロールチェンジの値を取得 */
long __stdcall MIDIPart_GetControlChange (MIDIPart* pMIDIPart, long lNum);

/* 現在のコントロールチェンジの値をまとめて取得 */
long __stdcall MIDIPart_GetControlChangeEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* 現在のRPNMSBの値を取得 */
long __stdcall MIDIPart_GetRPNMSB (MIDIPart* pMIDIPart, long lCC101, long lCC100);

/* 現在のRPNLSBの値を取得 */
long __stdcall MIDIPart_GetRPNLSB (MIDIPart* pMIDIPart, long lCC101, long lCC100);

/* 現在のNRPNMSBの値を取得 */
long __stdcall MIDIPart_GetNRPNMSB (MIDIPart* pMIDIPart, long lCC99, long lCC98);

/* 現在のNRPNLSBの値を取得 */
long __stdcall MIDIPart_GetNRPNLSB (MIDIPart* pMIDIPart, long lCC99, long lCC98);

/* 現在のプログラムチェンジの値を取得 */
long __stdcall MIDIPart_GetProgramChange (MIDIPart* pMIDIPart);

/* 現在のチャンネルアフタータッチの値を取得 */
long __stdcall MIDIPart_GetChannelAfterTouch (MIDIPart* pMIDIPart);

/* 現在のピッチベンドの値を取得 */
long __stdcall MIDIPart_GetPitchBend (MIDIPart* pMIDIPart);

/* MIDIPart_Get系関数(ユニークなもの) */

/* 現在押されている鍵盤の数を返す。 */
long __stdcall MIDIPart_GetNumNote (MIDIPart* pMIDIPart);

/* 現在押されている鍵盤の数(ホールド持続含む)を返す。 */
long __stdcall MIDIPart_GetNumNoteKeep (MIDIPart* pMIDIPart);

/* 現在押されている鍵盤で最も低い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetHighestNote (MIDIPart* pMIDIPart);

/* 現在押されている鍵盤(ホールド持続含む)で最も低い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetHighestNoteKeep (MIDIPart* pMIDIPart);

/* 現在押されている鍵盤で最も高い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetLowestNote (MIDIPart* pMIDIPart);

/* 現在押されている鍵盤(ホールド持続含む)で最も高い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetLowestNoteKeep (MIDIPart* pMIDIPart);

/* MIDIPart_Set系関数 */

/* 現在のパートモードを設定 */
long __stdcall MIDIPart_SetPartMode (MIDIPart* pMIDIPart, long lPartMode);

/* 現在のオムニON/OFF、モノ/ポリモードを設定 */
long __stdcall MIDIPart_SetOmniMonoPolyMode (MIDIPart* pMIDIPart, long lOmniMonoPolyMode);

/* 現在のチャンネルファインチューニングの値を設定 */
long __stdcall MIDIPart_SetChannelFineTuning (MIDIPart* pMIDIPart, long lChannelFineTuning);

/* 現在のチャンネルコースチューニングの値を設定 */
long __stdcall MIDIPart_SetChannelCoarseTuning (MIDIPart* pMIDIPart, long lChannelCoarseTuning);

/* 現在のピッチベンドセンシティビティの値を設定 */
long __stdcall MIDIPart_SetPitchBendSensitivity (MIDIPart* pMIDIPart, long lPitchBendSensitivity);

/* 現在のモジュレーションデプスレンジの値を設定 */
long __stdcall MIDIPart_SetModulationDepthRange (MIDIPart* pMIDIPart, long lModulationDepthRange);

/* 現在のレシーブチャンネルの値を設定 */
long __stdcall MIDIPart_SetReseiveChannel (MIDIPart* pMIDIPart, long lReceiveChannel);

/* 現在のベロシティセンスデプスの値を設定 */
long __stdcall MIDIPart_SetVelocitySenseDepth (MIDIPart* pMIDIPart, long lVelocitySenseDepth);

/* 現在のベロシティセンスオフセットの値を設定 */
long __stdcall MIDIPart_SetVelocitySenseOffset (MIDIPart* pMIDIPart, long lVelocitySenseOffset);

/* 現在のキーボードレンジローの値を設定 */
long __stdcall MIDIPart_SetKeyboardRangeLow (MIDIPart* pMIDIPart, long lKeyboardRangeLow);

/* 現在のキーボードレンジハイの値を設定 */
long __stdcall MIDIPart_SetKeyboardRangeHigh (MIDIPart* pMIDIPart, long lKeyboardRangeHigh);

/* 現在の鍵盤の押され具合を設定 */
long __stdcall MIDIPart_SetNote (MIDIPart* pMIDIPart, long lKey, long lVel);

/* 現在のキーアフタータッチの値を設定 */
long __stdcall MIDIPart_SetKeyAfterTouch (MIDIPart* pMIDIPart, long lKey, long lVal);

/* 現在のコントロールチェンジの値を設定 */
long __stdcall MIDIPart_SetControlChange (MIDIPart* pMIDIPart, long lNum, long lVal);

/* 現在のRPNMSBの値を設定 */
long __stdcall MIDIPart_SetRPNMSB (MIDIPart* pMIDIPart, long lCC101, long lCC100, long lVal);

/* 現在のRPNLSBの値を設定 */
long __stdcall MIDIPart_SetRPNLSB (MIDIPart* pMIDIPart, long lCC101, long lCC100, long lVal);

/* 現在のNRPNMSBの値を設定 */
long __stdcall MIDIPart_SetNRPNMSB (MIDIPart* pMIDIPart, long lCC99, long lCC98, long lVal);

/* 現在のNRPNLSBの値を設定 */
long __stdcall MIDIPart_SetNRPNLSB (MIDIPart* pMIDIPart, long lCC99, long lCC98, long lVal);

/* 現在のプログラムチェンジの値を設定 */
long __stdcall MIDIPart_SetProgramChange (MIDIPart* pMIDIPart, long lNum);

/* 現在のチャンネルアフタータッチの値を設定 */
long __stdcall MIDIPart_SetChannelAfterTouch (MIDIPart* pMIDIPart, long lVal);

/* 現在のピッチベンドの値を設定 */
long __stdcall MIDIPart_SetPitchBend (MIDIPart* pMIDIPart, long lVal);


/* MIDIPartの書き込み(外部隠蔽、この関数はMIDIStatus_Writeから呼び出される) */
long __stdcall MIDIPart_Write (MIDIPart* pMIDIPart, FILE* pFile, long lVersion);

/* MIDIPartの読み込み(外部隠蔽、この関数はMIDIStatus_Readから呼び出される) */
long __stdcall MIDIPart_Read (MIDIPart* pMIDIPart, FILE* pFile, long lVersion);



/******************************************************************************/
/*                                                                            */
/* MIDIDrumSetup関数群                                                        */
/*                                                                            */
/******************************************************************************/

/* MIDIDrumSetupオブジェクトを削除する。 */
int __stdcall MIDIDrumSetup_Delete (MIDIDrumSetup* pMIDIDrumSetup);

/* MIDIDrumSetupオブジェクトを生成する。 */
MIDIDrumSetup* __stdcall MIDIDrumSetup_Create (MIDIStatus* pParent);

/* MIDIDrumSetup_Get系関数 */

/* 現在の楽器ごとのカットオフフリーケンシーの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumCutoffFrequency (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのカットオフフリーケンシーの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumCutoffFrequencyEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのレゾナンスの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumResonance (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのレゾナンスの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumResonanceEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのアタックタイムの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumAttackTime (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのアタックタイムの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumAttackTimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのディケイタイム1の値を取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay1Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのディケイタイム1の値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay1TimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのディケイタイム2の値を取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay2Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのディケイタイム2の値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay2TimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのピッチコースの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchCoarse (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのピッチコースの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchCoarseEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのピッチファインの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchFine (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのピッチファインの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchFineEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのボリュームの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumVolume (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのボリュームの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumVolumeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのパンの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumPan (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのパンの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumPanEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのリバーブの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumReverb (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのリバーブの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumReverbEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのコーラスの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumChorus (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのコーラスの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumChorusEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* 現在の楽器ごとのディレイの値を取得(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIDrumSetup_GetDrumDelay (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* 現在の楽器ごとのディレイの値をまとめて取得(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIDrumSetup_GetDrumDelayEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* MIDIDrumSetup_Set系関数 */

/* 現在の楽器ごとのカットオフフリーケンシーの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumCutoffFrequency (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのレゾナンスの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumResonance (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのアタックタイムの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumAttackTime (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのディケイタイム1の値を設定 */
long __stdcall MIDIDrumSetup_SetDrumDecay1Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのディケイタイム2の値を設定 */
long __stdcall MIDIDrumSetup_SetDrumDecay2Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのピッチコースの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumPitchCoarse (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのピッチファインの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumPitchFine (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのボリュームの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumVolume (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのパンの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumPan (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのリバーブの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumReverb (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのコーラスの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumChorus (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* 現在の楽器ごとのディレイの値を設定(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIDrumSetup_SetDrumDelay (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);


/* MIDIDrumSetupの書き込み(外部隠蔽、この関数はMIDIStatus_Writeから呼び出される) */
long __stdcall MIDIDrumSetup_Write (MIDIDrumSetup* pMIDIDrumSetup, FILE* pFile, long lVersion);

/* MIDIDrumSetupの読み込み(外部隠蔽、この関数はMIDIStatus_Readから呼び出される) */
long __stdcall MIDIDrumSetup_Read (MIDIDrumSetup* pMIDIDrumSetup, FILE* pFile, long lVersion);



/******************************************************************************/
/*                                                                            */
/* MIDIStatus関数群                                                           */
/*                                                                            */
/******************************************************************************/

/* MIDIStatusオブジェクトを削除する。 */
void __stdcall MIDIStatus_Delete (MIDIStatus* pMIDIStatus);

/* MIDIStatusオブジェクトを生成する。 */
MIDIStatus* __stdcall MIDIStatus_Create (long lModuleMode, long lNumMIDIPart, long lNumMIDIDrumSetup);

/* MIDIStatus_Get系関数 */

/* 現在のモジュールモード(NATIVE/GM/GM2/GS/88/XG)を取得 */
long __stdcall MIDIStatus_GetModuleMode (MIDIStatus* pMIDIStatus);

/* 現在のマスターチューンの値を取得 */
long __stdcall MIDIStatus_GetMasterFineTuning (MIDIStatus* pMIDIStatus);

/* 現在のマスターキーシフトの値を取得 */
long __stdcall MIDIStatus_GetMasterCoarseTuning (MIDIStatus* pMIDIStatus);

/* 現在のマスターボリュームの値を取得 */
long __stdcall MIDIStatus_GetMasterVolume (MIDIStatus* pMIDIStatus);

/* 現在のマスターパンの値を取得 */
long __stdcall MIDIStatus_GetMasterPan (MIDIStatus* pMIDIStatus);

/* 現在のマスターリバーブの値を1つ取得 */
long __stdcall MIDIStatus_GetMasterReverb (MIDIStatus* pMIDIStatus, long lNum);

/* 現在のマスターリバーブの値をまとめて取得 */
long __stdcall MIDIStatus_GetMasterReverbEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* 現在のマスターコーラスの値を1つ取得 */
long __stdcall MIDIStatus_GetMasterChorus (MIDIStatus* pMIDIStatus, long lNum);

/* 現在のマスターコーラスの値をまとめて取得 */
long __stdcall MIDIStatus_GetMasterChorusEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* 現在のマスターディレイの値を1つ取得(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIStatus_GetMasterDelay (MIDIStatus* pMIDIStatus, long lNum);

/* 現在のマスターディレイの値をまとめて取得(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIStatus_GetMasterDelayEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* 現在のマスターイコライザの値を1つ取得 */
long __stdcall MIDIStatus_GetMasterEqualizer (MIDIStatus* pMIDIStatus, long lNum);

/* 現在のマスターイコライザの値をまとめて取得 */
long __stdcall MIDIStatus_GetMasterEqualizerEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* このMIDIStatusオブジェクトに含まれるMIDIPartの数を取得 */
long __stdcall MIDIStatus_GetNumMIDIPart (MIDIStatus* pMIDIStatus);

/* このMIDIStatusオブジェクトに含まれるMIDIDrumSetupの数を取得 */
long __stdcall MIDIStatus_GetNumMIDIDrumSetup (MIDIStatus* pMIDIStatus);

/* 各MIDIPartへのポインタを取得 */
MIDIPart* __stdcall MIDIStatus_GetMIDIPart (MIDIStatus* pMIDIStatus, long lIndex);

/* 各MIDIDrumSetupへのポインタを取得 */
MIDIDrumSetup* __stdcall MIDIStatus_GetMIDIDrumSetup (MIDIStatus* pMIDIStatus, long lIndex);

/* MIDIStatus_Set系関数 */

/* モジュールモード(NATIVE/GM/GM2/GS/88/XG)を設定する。 */
/* モジュールモードの設定に従いすべてのパラメータをデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetModuleMode (MIDIStatus* pMIDIStatus, long lModuleMode);

/* 現在のマスターファインチューニングの値を設定 */
long __stdcall MIDIStatus_SetMasterFineTuning (MIDIStatus* pMIDIStatus, long lMasterFineTuning);

/* 現在のマスターコースチューニングの値を設定 */
long __stdcall MIDIStatus_SetMasterCoarseTuning (MIDIStatus* pMIDIStatus, long lMasterCoarseTuning);

/* 現在のマスターボリュームの値を設定 */
long __stdcall MIDIStatus_SetMasterVolume (MIDIStatus* pMIDIStatus, long lMasterVolume);

/* 現在のマスターパンの値を設定 */
long __stdcall MIDIStatus_SetMasterPan (MIDIStatus* pMIDIStatus, long lMasterPan);

/* 現在のマスターリバーブの値を1つ設定 */
long __stdcall MIDIStatus_SetMasterReverb (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* マスターリバーブのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetMasterReverbType (MIDIStatus* pMIDIStatus, long lMasterReverbType);

/* 現在のマスターコーラスの値を1つ設定 */
long __stdcall MIDIStatus_SetMasterChorus (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* マスターコーラスのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetMasterChorusType (MIDIStatus* pMIDIStatus, long lMasterChorusType);

/* 現在のマスターディレイの値を1つ設定(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIStatus_SetMasterDelay (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* マスターディレイのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
/* (XGではマスターヴァリエーションとなる) */
long __stdcall MIDIStatus_SetMasterDelayType (MIDIStatus* pMIDIStatus, long lMasterDelayType);

/* 現在のマスターイコライザの値を1つ設定 */
long __stdcall MIDIStatus_SetMasterEqualizer (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* マスターイコライザのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetMasterEqualizerType (MIDIStatus* pMIDIStatus, long lMasterEqualizerType);

/* MIDIStatus_Put系関数 */

/* リセット */
long __stdcall MIDIStatus_PutReset (MIDIStatus* pMIDIStatus);

/* GMリセット */
long __stdcall MIDIStatus_PutGMReset (MIDIStatus* pMIDIStatus);

/* GM2リセット */
long __stdcall MIDIStatus_PutGM2Reset (MIDIStatus* pMIDIStatus);

/* GSリセット */
long __stdcall MIDIStatus_PutGSReset (MIDIStatus* pMIDIStatus);

/* 88リセット */
long __stdcall MIDIStatus_Put88Reset (MIDIStatus* pMIDIStatus);

/* XGリセット */
long __stdcall MIDIStatus_PutXGReset (MIDIStatus* pMIDIStatus);

/* MIDIメッセージを与えることによりパラメータを変更する。 */
long __stdcall MIDIStatus_PutMIDIMessage (MIDIStatus* pMIDIStatus, unsigned char* pMIDIMessage, long lLen);



/* MIDIStatusの書き込み(外部隠蔽、この関数はMIDIStatus_Saveから呼び出される) */
long __stdcall MIDIStatus_Write (MIDIStatus* pMIDIStatus, FILE* pFile, long lVersion);

/* MIDIStatusの保存 */
long __stdcall MIDIStatus_SaveA (MIDIStatus* pMIDIStatus, const char* pszFileName);
long __stdcall MIDIStatus_SaveW (MIDIStatus* pMIDIStatus, const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIStatus_Save MIDIStatus_SaveW
#else
#define MIDIStatus_Save MIDIStatus_SaveA
#endif

/* MIDIStatusの読み込み(外部隠蔽、この関数はMIDIStatus_Loadから呼び出される) */
long __stdcall MIDIStatus_Read (MIDIStatus* pMIDIStatus, FILE* pFile, long lVersion);

/* MIDIStatusのロード */
MIDIStatus* __stdcall MIDIStatus_LoadA (const char* pszFileName);
MIDIStatus* __stdcall MIDIStatus_LoadW (const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIStatus_Load MIDIStatus_LoadW
#else
#define MIDIStatus_Load MIDIStatus_LoadA
#endif

#ifdef __cplusplus
}
#endif

#endif

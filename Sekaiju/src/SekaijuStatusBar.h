﻿//******************************************************************************
// MIDIシーケンサーソフト『世界樹』
// 世界樹ステータスバークラス
// (C)2002-2017 おーぷんMIDIぷろじぇくと／くず
//******************************************************************************

/* This Source Code Form is subject to the terms of the Mozilla Public */
/* License, v. 2.0. If a copy of the MPL was not distributed with this */
/* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SEKAIJUSTATUSBAR_H_
#define _SEKAIJUSTATUSBAR_H_


class CSekaijuStatusBar : public CStatusBar {
	DECLARE_DYNAMIC (CSekaijuStatusBar)

	//--------------------------------------------------------------------------
	// 構築と破壊
	//--------------------------------------------------------------------------
public:
	CSekaijuStatusBar ();                 // コンストラクタ
	virtual ~CSekaijuStatusBar ();        // デストラクタ

	//--------------------------------------------------------------------------
	// オペレーション
	//--------------------------------------------------------------------------
public:

	//--------------------------------------------------------------------------
	// オーバーライド
	//--------------------------------------------------------------------------
protected:
	//--------------------------------------------------------------------------
	// メッセージマップ
	//--------------------------------------------------------------------------
protected:
	DECLARE_MESSAGE_MAP ()
	afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
};

#endif
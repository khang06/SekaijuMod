//******************************************************************************
// MIDIシーケンサーソフト『世界樹』
// 音色の選択ダイアログクラス
// (C)2002-2019 おーぷんMIDIぷろじぇくと／くず
//******************************************************************************

/* This Source Code Form is subject to the terms of the Mozilla Public */
/* License, v. 2.0. If a copy of the MPL was not distributed with this */
/* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SELECTVOICEDLG_H_
#define _SELECTVOICEDLG_H_

class CSelectVoiceDlg : public CDialog {
	//--------------------------------------------------------------------------
	// アトリビュート
	//--------------------------------------------------------------------------
public:
	CString m_strSearchString;               // 検索用文字列
	BOOL m_bCaseSensitive;                   // 大文字と小文字を区別する
	BOOL m_bExactMatch;                      // 一致する文字列だけを検索する
	int m_nListIndex;                        // 検索結果リストのカレントインデックス

	void* m_pMIDIInstrumentDefinition;       // MIDIインストゥルメント定義へのポインタ

	int m_nCC0;
	int m_nCC32;
	int m_nPC;
	void* m_pCC0Event;
	void* m_pCC32Event;
	void* m_pPCEvent;

	//--------------------------------------------------------------------------
	// 構築と破壊
	//--------------------------------------------------------------------------
public:
	CSelectVoiceDlg ();                      // コンストラクタ
	enum {IDD = IDD_SELECTVOICE};

	//--------------------------------------------------------------------------
	// オペレーション
	//--------------------------------------------------------------------------
public:
	int TStrInclude (TCHAR* psz1, const TCHAR* psz2);
	BOOL Search ();
	CDocument* GetDocument ();

	//--------------------------------------------------------------------------
	// オーバーライド
	//--------------------------------------------------------------------------
protected:
	virtual void DoDataExchange (CDataExchange* pDX);    // DDX/DDV のサポート
	virtual BOOL OnInitDialog ();            // ダイアログの初期化
	virtual void OnCancel ();

	//--------------------------------------------------------------------------
	// メッセージマップ
	//--------------------------------------------------------------------------
protected:
	DECLARE_MESSAGE_MAP ();
	afx_msg void OnSearchStringChange ();
	afx_msg void OnCaseSensitiveClick ();
	afx_msg void OnExactMatchClick ();
	afx_msg void OnResetClick ();
	afx_msg void OnListSelChange ();
	afx_msg void OnApplyClick ();
};

#endif
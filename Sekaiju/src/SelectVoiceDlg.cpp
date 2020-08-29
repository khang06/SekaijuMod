//******************************************************************************
// MIDIシーケンサーソフト『世界樹』
// 音色の選択ダイアログクラス
// (C)2002-2019 おーぷんMIDIぷろじぇくと／くず
//******************************************************************************

/* This Source Code Form is subject to the terms of the Mozilla Public */
/* License, v. 2.0. If a copy of the MPL was not distributed with this */
/* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "winver.h"
#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include <afxmt.h>
#include "resource.h"
#include "..\\..\\MIDIIO\\MIDIIO.h"
#include "..\\..\\MIDIData\\MIDIData.h"
#include "..\\..\\MIDIClock\\MIDIClock.h"
#include "..\\..\\MIDIStatus\\MIDIStatus.h"
#include "..\\..\\MIDIInstrument\\MIDIInstrument.h"
#include "HistoryRecord.h"
#include "HistoryUnit.h"
#include "SekaijuApp.h"
#include "SekaijuDocTemplate.h"
#include "SekaijuDoc.h"
#include "SelectVoiceDlg.h"


//******************************************************************************
// 構築と破壊
//******************************************************************************

// コンストラクタ
CSelectVoiceDlg::CSelectVoiceDlg () : CDialog (CSelectVoiceDlg::IDD) {
	m_strSearchString = _T("");
	m_bCaseSensitive = FALSE;
	m_bExactMatch = FALSE;
	m_nListIndex = 0;
	m_pMIDIInstrumentDefinition = NULL;
	m_nCC0 = -1;
	m_nCC32 = -1;
	m_nPC = -1;
	m_pCC0Event = NULL;
	m_pCC32Event = NULL;
	m_pPCEvent = NULL;
}

//******************************************************************************
// オペレーション
//******************************************************************************

// 文字列psz1にpsz2を構成する文字が何文字含まれているか返す(文字の発生順序を考慮する)
int CSelectVoiceDlg::TStrInclude (TCHAR* psz1, const TCHAR* psz2) {
	TCHAR* p = psz1;
	int nRet = 0;
	int nLen1 = _tcslen (psz1);
	int nLen2 = _tcslen (psz2);
	int i;
	for (i = 0; i < nLen2; i++) {
		TCHAR* pRet = _tcschr (p, psz2[i]);
		if (pRet) {
			nRet ++;
			p = pRet + 1;
		}
		if (p - psz1 >= nLen1) {
			break;
		}
	}
	return nRet;
}

// リストボックスに検索結果を表示させる関数
int CSelectVoiceDlg::Search () {
	int i, j, nRet;
	TCHAR szBuf1[256];
	TCHAR szBuf2[256];
	TCHAR szBuf[256];

	MIDIInstrumentDefinition* pInstrumentDefinition = (MIDIInstrumentDefinition*)m_pMIDIInstrumentDefinition;
	CListBox* pWndList = (CListBox*)GetDlgItem (IDC_SELECTVOICE_LIST);
	memset (szBuf1, 0, sizeof (szBuf1));
	pWndList->ResetContent ();
	if (pInstrumentDefinition == NULL) {
		return -2;
	}
	// 検索テキスト、大文字小文字の区別、完全一致を取得
	GetDlgItem (IDC_SELECTVOICE_SEARCHSTRING)->GetWindowText (szBuf1, 255);
	int nCaseSensitive = ((CButton*)GetDlgItem (IDC_SELECTVOICE_CASESENSITIVE))->GetCheck ();
	int nExactMatch = ((CButton*)GetDlgItem (IDC_SELECTVOICE_EXACTMATCH))->GetCheck ();
	// 検索テキストの正当性のチェック
	int nBuf1Len = _tcslen (szBuf1);
	for (i = 0; i < nBuf1Len; i++) {
		if (szBuf1[i] < 32 || szBuf1[i] > 127) {
			break;
		}
	}
	if (nBuf1Len > 12 || i < nBuf1Len) {
		return -1;
	}
	// 大文字と小文字を区別しない場合、検索テキストを小文字化
	if (!nCaseSensitive) {
		_tcslwr (szBuf1);
	}
	// リストボックスのちらつき防止
	pWndList->SendMessage (WM_SETREDRAW, FALSE, 0);
	// リストボックスに項目を追加
	for (i = 0; i < 16384; i++) {
		MIDIPatchNameTable* pPatchNameTable = 
			MIDIInstrumentDefinition_GetPatchNameTable (pInstrumentDefinition, i);
		if (pPatchNameTable == NULL) {
			continue;
		}
		for (j = 0; j < 128; j++) {
			long lRet;
			TCHAR szName[256];
			memset (szName, 0, sizeof (szName));
			lRet = MIDIPatchNameTable_GetName (pPatchNameTable, j, szName, TSIZEOF (szName) - 1);
			if (lRet) {
				memset (szBuf2, 0, sizeof (szBuf2));
				_tcsncpy (szBuf2, szName, TSIZEOF (szBuf2) - 1);
				if (nBuf1Len > 0) {
					if (!nCaseSensitive) {
						_tcslwr (szBuf2);
					}
					if (nExactMatch) {
						nRet = (int)_tcsstr (szBuf2, szBuf1);
					}
					else {
						nRet = (TStrInclude (szBuf2, szBuf1) >= nBuf1Len);
					}
				}
				if (nBuf1Len == 0 || nBuf1Len > 0 && nRet) {
					_sntprintf (szBuf, TSIZEOF (szBuf) - 1, _T("%3d   %3d   %3d%c%s\0"),
						i >> 7, (i & 0x7F), j,
						i > 0 ? _T('-') : _T('-'), szName);
					pWndList->AddString (CString (szBuf));
				}
			}
		}
	}
	// リストボックスのちらつき防止
	pWndList->SendMessage (WM_SETREDRAW, TRUE, 0);
	return 1;
}

// ドキュメント取得用関数
CDocument* CSelectVoiceDlg::GetDocument () {
	CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd ();
	if (pMDIFrameWnd == NULL) {
		return NULL;
	}
	CMDIChildWnd* pMDIChildWnd = (CMDIChildWnd*)pMDIFrameWnd->GetActiveFrame ();
	if (pMDIChildWnd == NULL) {
		//AfxMessageBox ("ｱｸﾃｨﾌﾞなｳｨﾝﾄﾞｳがありません。", MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}
	CDocument* pDoc = pMDIChildWnd->GetActiveDocument();
	if (pDoc == NULL) {
		//AfxMessageBox ("ｱｸﾃｨﾌﾞなﾄﾞｷｭﾒﾝﾄがありません。", MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}
	return pDoc;
}



//******************************************************************************
// オーバーライド
//******************************************************************************

// データエクスチェンジ
void CSelectVoiceDlg::DoDataExchange (CDataExchange* pDX) {
	CDialog::DoDataExchange (pDX);
	DDX_Text (pDX, IDC_SELECTVOICE_SEARCHSTRING, m_strSearchString);
	DDX_Check (pDX, IDC_SELECTVOICE_CASESENSITIVE, m_bCaseSensitive);
	DDX_Check (pDX, IDC_SELECTVOICE_EXACTMATCH, m_bExactMatch);
	DDX_LBIndex (pDX, IDC_SELECTVOICE_LIST, m_nListIndex);
}

// ダイアログ初期化
BOOL CSelectVoiceDlg::OnInitDialog () {
	BOOL bRet = CDialog::OnInitDialog ();

	GetDlgItem (IDC_SELECTVOICE_SEARCHSTRING)->SetWindowText (m_strSearchString);
	((CButton*)GetDlgItem (IDC_SELECTVOICE_CASESENSITIVE))->SetCheck (m_bCaseSensitive);
	((CButton*)GetDlgItem (IDC_SELECTVOICE_EXACTMATCH))->SetCheck (m_bExactMatch);
	if (!Search ()) {
		GetDlgItem (IDC_SELECTVOICE_SEARCHSTRING)->SetFocus ();
	}
	else {
		int nListCount = ((CListBox*)GetDlgItem (IDC_SELECTVOICE_LIST))->GetCurSel ();
		if (nListCount > 0 && 0 <= m_nListIndex && m_nListIndex < nListCount) {
			((CListBox*)GetDlgItem (IDC_SELECTVOICE_LIST))->SetCurSel (m_nListIndex);
			GetDlgItem (IDC_SELECTVOICE_LIST)->SetFocus ();
		}
		else {
			GetDlgItem (IDC_SELECTVOICE_SEARCHSTRING)->SetFocus ();	
		}
	}
	return bRet;
}

void CSelectVoiceDlg::OnCancel () {
	UpdateData (TRUE);
	CDialog::OnCancel ();
}


//******************************************************************************
// メッセージマップ
//******************************************************************************

BEGIN_MESSAGE_MAP (CSelectVoiceDlg, CDialog)
	ON_EN_CHANGE (IDC_SELECTVOICE_SEARCHSTRING, OnSearchStringChange)
	ON_BN_CLICKED (IDC_SELECTVOICE_CASESENSITIVE, OnCaseSensitiveClick)
	ON_BN_CLICKED (IDC_SELECTVOICE_EXACTMATCH, OnExactMatchClick)
	ON_BN_CLICKED (IDC_SELECTVOICE_RESET, OnResetClick)
	ON_BN_CLICKED (IDC_SELECTVOICE_APPLY, OnApplyClick)
	ON_LBN_SELCHANGE (IDC_SELECTVOICE_LIST, OnListSelChange)
END_MESSAGE_MAP ()

// テキストボックスが更新された
void CSelectVoiceDlg::OnSearchStringChange () {
	Search ();
}

// 『大文字小文字を区別する』がクリックされた 
void CSelectVoiceDlg::OnCaseSensitiveClick () {
	Search ();
}

// 『一致する文字列だけを検索』
void CSelectVoiceDlg::OnExactMatchClick () {
	Search ();
}

// 『リセット(&R)』
void CSelectVoiceDlg::OnResetClick () {
	int nListCount;
	GetDlgItem (IDC_SELECTVOICE_SEARCHSTRING)->SetWindowText(_T(""));
	Search ();
	nListCount = ((CListBox*)GetDlgItem (IDC_SELECTVOICE_LIST))->GetCount ();
	if (nListCount > 0) {
		GetDlgItem (IDC_SELECTVOICE_LIST)->SetFocus ();
	}
	else {
		GetDlgItem (IDC_SELECTVOICE_SEARCHSTRING)->SetFocus ();
	}
}

// リストボックスのカレントセルが変更された
void CSelectVoiceDlg::OnListSelChange () {
	TCHAR szBuf[64];
	memset (szBuf, 0, sizeof (szBuf));
	CSekaijuDoc* pSekaijuDoc = (CSekaijuDoc*)GetDocument ();
	CListBox* pWndList = (CListBox*)GetDlgItem (IDC_SELECTVOICE_LIST);
	int nListIndex = pWndList->GetCurSel ();
	// リスト項目が何も選択されていない場合
	if (nListIndex == LB_ERR) {
		m_nCC0 = -1;
		m_nCC32 = -1;
		m_nPC = -1;
		return;
	}
	// リスト項目が選択されている場合
	pWndList->SendMessage 
		(LB_GETTEXT, nListIndex, (LPARAM)szBuf);
	m_nCC0 = CLIP (0, _ttoi (&szBuf[0]), 127);
	m_nCC32 = CLIP (0, _ttoi (&szBuf[6]), 127);
	m_nPC = CLIP (0, _ttoi (&szBuf[12]), 127);
	pSekaijuDoc->ApplySelectVoiceDlg (this);
}

// 『適用』ボタンが押された
void CSelectVoiceDlg::OnApplyClick () {
	CSekaijuDoc* pSekaijuDoc = (CSekaijuDoc*)GetDocument ();
	if (pSekaijuDoc == NULL) {
		AfxMessageBox (_T("ﾄﾞｷｭﾒﾝﾄの取得ができません。"), MB_OK | MB_ICONSTOP);
		return;
	}
	pSekaijuDoc->ApplySelectVoiceDlg (this);
}
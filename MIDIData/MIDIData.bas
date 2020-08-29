'******************************************************************************
'*                                                                            *
'*　MIDIData.bas - MIDIDataモジュール(VB4,5,6用)            (C)2002-2015 くず *
'*                                                                            *
'******************************************************************************

'This library is free software; you can redistribute it and/or
'modify it under the terms of the GNU Lesser General Public
'License as published by the Free Software Foundation; either
'version 2.1 of the License, or (at your option) any later version.

'This library is distributed in the hope that it will be useful,
'but WITHOUT ANY WARRANTY; without even the implied warranty of
'MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
'Lesser General Public License for more details.

'You should have received a copy of the GNU Lesser General Public
'License along with this library; if not, write to the Free Software
'Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

'※このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布されます。
'※プロジェクトホームページ："http://openmidiproject.sourceforge.jp/index.html"

' ★使用例(4分音符の「ド」1音だけのMIDIデータを作成)
'Private Sub Command1_Click()
'    Dim pMIDIData As Long
'    Dim pMIDITrack As Long
'    pMIDIData = MIDIData_Create(MIDIDATA_FORMAT0, 1, MIDIDATA_TPQNBASE, 120)
'    pMIDITrack = MIDIData_GetFirstTrack(pMIDIData)
'    Call MIDITrack_InsertNote(pMIDITrack, 0, 0, 60, 100, 120)
'    Call MIDITrack_InsertEndofTrack(pMIDITrack, 120)
'    Call MIDIData_SaveAsSMF(pMIDIData, "Test1.mid")
'    Call MIDIData_Delete(pMIDIData)
'End Sub

Option Explicit

' ★重要：以下の構造体(ユーザー定義型)をVBで直接使ってはなりません。
' 　　　　MIDIEvent, MIDITrack, MIDIDataは、必ずLong型のポインタで指してください。

' MIDIEvent構造体(参考)
' 双方向ダブルリンクリスト構造
' ノード順序は絶対時刻で決定されます
Type MIDIEvent
    m_lTempIndex As Long
    m_lTime As Long                     ' 絶対時刻
    m_lKind As Long                     ' MIDIイベントの種類(= &H00～= &HFF)
    m_lLen As Long                      ' MIDIイベントのデータ長さ[バイト]
    m_pData As Long                     ' MIDIイベントのデータバッファへのポインタ
    m_lData As Long                     ' MIDIチャンネルイベントの場合のデータ格納場所
    m_pNextEvent As Long                ' 次のイベントへのポインタ(なければNULL)
    m_pPrevEvent As Long                ' 前のイベントへのポインタ(なければNULL)
    m_pNextSameKindEvent As Long        ' 次の同種のイベントへのポインタ(なければNULL)
    m_pPrevSameKindEvent As Long        ' 前の同種のイベントへのポインタ(なければNULL)
    m_pNextCombinedEvent As Long        ' 次の結合しているイベントへのポインタ(なければNULL)
    m_pPrevCombinedEvent As Long        ' 前の結合しているイベントへのポインタ(なければNULL)
    m_pParent As Long                   ' 親(MIDITrackオブジェクト)へのポインタ
    m_lUser1 As Long                    ' ユーザー用領域１(未使用)
    m_lUser2 As Long                    ' ユーザー用領域２(未使用)
    m_lUser3 As Long                    ' ユーザー用領域３(未使用)
    m_lUserFlag As Long                 ' ユーザー用フラグ(未使用)
End Type

' MIDITrack構造体(参考)
' 双方向リンクリスト構造
Type MIDITrack
    m_lTempIndex As Long
    m_lNumEvent As Long                 ' トラック内のイベント数
    m_pFirstEvent As Long               ' 最初のイベントへのポインタ(なければNULL)
    m_pLastEvent As Long                ' 最後のイベントへのポインタ(なければNULL)
    m_pPrevTrack As Long                ' 前のトラックへのポインタ(なければNULL)
    m_pNextTrack As Long                ' 次のトラックへのポインタ(なければNULL)
    m_pParent As Long                   ' 親(MIDIDataオブジェクト)へのポインタ
    m_lInputOn As Long                  ' 入力(0=OFF,1=ON)
    m_lInputPort As Long                ' 入力ポート(0～255)
    m_lInputChannel As Long             ' 入力チャンネル(-1=n/a, 0～15)
    m_lOutputOn As Long                 ' 出力(0=OFF, 1=ON)
    m_lOutputPort As Long               ' 出力ポート(0～255)
    m_lOutputChannel As Long            ' 出力チャンネル(-1=n/a, 0～15)
    m_lTimePlus As Long                 ' タイム+
    m_lKeyPlus As Long                  ' キー+
    m_lVelPlus As Long                  ' ベロシティ+
    m_lViewMode As Long                 ' 表示モード(0=通常,1=ドラム)
    m_lForeColor As Long                ' 前景色
    m_lBackColor As Long                ' 背景色
    m_lUser1 As Long                    ' ユーザー用自由領域１(未使用)
    m_lUser2 As Long                    ' ユーザー用自由領域２(未使用)
    m_lUser3 As Long                    ' ユーザー用領域３(未使用)
    m_lUserFlag As Long                 ' ユーザー用フラグ(未使用)
End Type

' MIDIData構造体(参考)
' 双方向リンクリスト構造
Type MIDIData
    m_lFormat As Long                   ' SMFフォーマット(0/1/2)
    m_lNumTrack As Long                 ' トラック数(1～65535)
    m_lTimeBase As Long                 ' タイムベース(例：120)
    m_pFirstTrack As Long               ' 最初のトラックへのポインタ(なければNULL)
    m_pLastTrack As Long                ' 最後のトラックへのポインタ(なければNULL)
    m_pNextSeq As Long                  ' 次のシーケンスへのポインタ(なければNULL)
    m_pPrevSeq As Long                  ' 前のシーケンスへのポインタ(なければNULL)
    m_pParent As Long                   ' 親(常にNULL)
    m_lUser1 As Long                    ' ユーザー用自由領域１(未使用)
    m_lUser2 As Long                    ' ユーザー用自由領域２(未使用)
    m_lUser3 As Long                    ' ユーザー用領域３(未使用)
    m_lUserFlag As Long                 ' ユーザー用フラグ(未使用)
End Type

' ロケールに関するマクロ
Public Const LC_ALL = 0

' その他のマクロ
Public Const MIDIEVENT_MAXLEN = 65536

' フォーマットに関するマクロ
Public Const MIDIDATA_FORMAT0 = 0
Public Const MIDIDATA_FORMAT1 = 1
Public Const MIDIDATA_FORMAT2 = 2

' トラック数に関するマクロ
Public Const MIDIDATA_MAXMIDITRACKNUM = 65535

' テンポに関するマクロ
' ★重要：テンポの単位はすべて[μ秒/4分音符]とする。
Public Const MIDIEVENT_MINTEMPO = 1
Public Const MIDIEVENT_MAXTEMPO = 60000000
Public Const MIDIEVENT_DEFTEMPO = 60000000 / 120

' タイムベース(タイムモード)に関するマクロ
Public Const MIDIDATA_TPQNBASE = 0
Public Const MIDIDATA_SMPTE24BASE = 24 '24フレーム/秒
Public Const MIDIDATA_SMPTE25BASE = 25 '25フレーム/秒
Public Const MIDIDATA_SMPTE29BASE = 29 '29.97フレーム/秒
Public Const MIDIDATA_SMPTE30BASE = 30 '30フレーム/秒

' タイムベース(レゾリューション)(分解能)に関するマクロ
' TPQNベースの場合：4分音符あたりの分解能
' ★注意：普通TPQNの分解能は、48,72,96,120,144,168,192,216,240,360,384,480,960である
Public Const MIDIDATA_MINTPQNRESOLUTION = 1
Public Const MIDIDATA_MAXTPQNRESOLUTION = 32767
Public Const MIDIDATA_DEFTPQNRESOLUTION = 120

' SMPTEベースの場合：1フレームあたりの分解能
' ★注意：普通SMPTEの分解能は、10,40,80などが代表的である。
Public Const MIDIDATA_MINSMPTERESOLUTION = 1
Public Const MIDIDATA_MAXSMPTERESOLUTION = 255
Public Const MIDIDATA_DEFSMPTERESOLUTION = 10

' MIDIEVENT_KINDマクロ (コメントのカッコ内はデータ部の長さを示す)
Public Const MIDIEVENT_SEQUENCENUMBER = &H0     ' シーケンス番号(2バイト)
Public Const MIDIEVENT_TEXTEVENT = &H1          ' テキスト(可変長文字列)
Public Const MIDIEVENT_COPYRIGHTNOTICE = &H2    ' 著作権(可変長文字列)
Public Const MIDIEVENT_TRACKNAME = &H3          ' トラック名(可変長文字列)
Public Const MIDIEVENT_INSTRUMENTNAME = &H4     ' インストゥルメント(可変長文字列)
Public Const MIDIEVENT_LYRIC = &H5              ' 歌詞(可変長文字列)
Public Const MIDIEVENT_MARKER = &H6             ' マーカー(可変長文字列)
Public Const MIDIEVENT_CUEPOINT = &H7           ' キューポイント(可変長文字列)
Public Const MIDIEVENT_PROGRAMNAME = &H8        ' プログラム名(可変長文字列)
Public Const MIDIEVENT_DEVICENAME = &H9         ' デバイス名(可変長文字列)
Public Const MIDIEVENT_CHANNELPREFIX = &H20     ' チャンネルプリフィックス(1バイト)
Public Const MIDIEVENT_PORTPREFIX = &H21        ' ポートプリフィックス(1バイト)
Public Const MIDIEVENT_ENDOFTRACK = &H2F        ' エンドオブトラック(なし)
Public Const MIDIEVENT_TEMPO = &H51             ' テンポ(3バイト)
Public Const MIDIEVENT_SMPTEOFFSET = &H54       ' SMPTEオフセット(5バイト)
Public Const MIDIEVENT_TIMESIGNATURE = &H58     ' 拍子記号(4バイト)
Public Const MIDIEVENT_KEYSIGNATURE = &H59      ' 調性記号(2バイト)
Public Const MIDIEVENT_SEQUENCERSPECIFIC = &H79 ' シーケンサー独自のイベント(可変長バイナリ)
Public Const MIDIEVENT_NOTEOFF = &H80           ' ノートオフ(3バイト)
Public Const MIDIEVENT_NOTEON = &H90            ' ノートオン(3バイト)
Public Const MIDIEVENT_KEYAFTERTOUCH = &HA0     ' キーアフター(3バイト)
Public Const MIDIEVENT_CONTROLCHANGE = &HB0     ' コントローラー(3バイト)
Public Const MIDIEVENT_PROGRAMCHANGE = &HC0     ' パッチチェンジ(2バイト)
Public Const MIDIEVENT_CHANNELAFTERTOUCH = &HD0 ' チャンネルアフター(2バイト)
Public Const MIDIEVENT_PITCHBEND = &HE0         ' ピッチベンド(3バイト)
Public Const MIDIEVENT_SYSEXSTART = &HF0        ' システムエクスクルーシヴ開始(可変長バイナリ)
Public Const MIDIEVENT_SYSEXCONTINUE = &HF0     ' システムエクスクルーシヴの続き(可変長バイナリ)

'*********************************************************************/
'*                                                                   */
'*　MIDIDataLibクラス関数                                            */
'*                                                                   */
'*********************************************************************/

Declare Function MIDIDataLib_SetDefaultCharCode Lib "MIDIData.dll" _
    (ByVal lCharCode As Long) As Long

'*********************************************************************/
'*                                                                   */
'*　MIDIEventクラス関数                                              */
'*                                                                   */
'*********************************************************************/

'★特記なき限り、正常終了時0以外、異常終了時0を返すものとする。
'★Is系関数は、特記なき限り、「はい」は0以外、「いいえ」は0を返すものとする。


' 結合イベントの最初のイベントを返す。
' 結合イベントでない場合、pEvent自身を返す。
Declare Function MIDIEvent_GetFirstCombinedEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 結合イベントの最後のイベントを返す。
' 結合イベントでない場合、pEvent自身を返す。
Declare Function MIDIEvent_GetLastCombinedEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 単体イベントを結合する
Declare Function MIDIEvent_Combine Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 結合イベントを切り離す
Declare Function MIDIEvent_Chop Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' MIDIイベントの削除(結合している場合でも単一のMIDIイベントを削除)
Declare Sub MIDIEvent_DeleteSingle Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)

' MIDIイベントの削除(結合している場合、結合しているMIDIイベントも削除)
Declare Sub MIDIEvent_Delete Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)

' MIDIイベント(任意)を生成し、MIDIイベントへのポインタを返す(失敗時NULL、以下同様)
Declare Function MIDIEvent_Create Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lKind As Long, _
    ByRef pData As Byte, ByVal lLen As Long) As Long

' 指定イベントと同じMIDIイベントを生成し、MIDIイベントへのポインタを返す(失敗時NULL、以下同様)
Declare Function MIDIEvent_CreateClone Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' シーケンス番号イベントの生成
Declare Function MIDIEvent_CreateSequenceNumber Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lNumber As Long) As Long

' テキストイベントの生成
Declare Function MIDIEvent_CreateTextEvent Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateTextEventA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' 著作権イベントの生成
Declare Function MIDIEvent_CreateCopyrightNotice Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateCopyrightNoticeA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' トラック名イベントの生成
Declare Function MIDIEvent_CreateTrackName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateTrackNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' インストゥルメント名イベントの生成
Declare Function MIDIEvent_CreateInstrumentName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateInstrumentNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' 歌詞イベントの生成
Declare Function MIDIEvent_CreateLyric Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateLyricA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' マーカーイベントの生成
Declare Function MIDIEvent_CreateMarker Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateMarkerA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' キューポイントイベントの生成
Declare Function MIDIEvent_CreateCuePoint Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateCuePointA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' プログラム名イベントの生成
Declare Function MIDIEvent_CreateProgramName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateProgramNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' デバイス名イベントの生成
Declare Function MIDIEvent_CreateDeviceName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateDeviceNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' チャンネルプリフィックスイベントの生成
Declare Function MIDIEvent_CreateChannelPrefix Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lChannel As Long) As Long

' ポートプリフィックスイベントの生成
Declare Function MIDIEvent_CreatePortPrefix Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lPort As Long) As Long

' エンドオブトラックイベントの生成
Declare Function MIDIEvent_CreateEndofTrack Lib "MIDIData.dll" _
    (ByVal lTime As Long) As Long

' テンポイベントの生成
Declare Function MIDIEvent_CreateTempo Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lTempo As Long) As Long

' SMPTEオフセットイベントの生成
Declare Function MIDIEvent_CreateSMPTEOffset Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lMode As Long, ByVal lHour As Long, _
    ByVal lMin As Long, ByVal lSec As Long, ByVal lFrame As Long, _
    ByVal lSubFrame As Long) As Long

' 拍子記号イベントの生成
Declare Function MIDIEvent_CreateTimeSignature Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lnn As Long, ByVal ldd As Long, _
    ByVal lcc As Long, ByVal lbb As Long) As Long

' 調性記号イベントの生成
Declare Function MIDIEvent_CreateKeySignature Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lsf As Long, ByVal lmi As Long) As Long

' ノートオフイベントの生成
Declare Function MIDIEvent_CreateNoteOff Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long) As Long

' ノートオンイベントの生成
Declare Function MIDIEvent_CreateNoteOn Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long) As Long

' ノートイベントの生成(MIDIEvent_CreateNoteOnNoteOn0と同じ)
Declare Function MIDIEvent_CreateNote Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long, ByVal lDur As Long) As Long

' ノートイベントの生成(0x8n離鍵型)
' (ノートオン(0x9n)・ノートオフ(0x8n)の2イベントを生成し、
' NoteOnへのポインタを返す)
Declare Function MIDIEvent_CreateNoteOnNoteOff Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
    ByVal lVel1 As Long, ByVal lVel2 As Long, ByVal lDur As Long) _
    As Long

' ノートイベントの生成(0x9n離鍵型)
' (ノートオン(0x9n)・ノートオン(0x9n(vel==0))の2イベントを生成し、
' NoteOnへのポインタを返す)
Declare Function MIDIEvent_CreateNoteOnNoteOn0 Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long, ByVal lDur As Long) As Long

' キーアフタータッチイベントの生成
Declare Function MIDIEvent_CreateKeyAftertouch Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
    ByVal lVal As Long) As Long

' コントロールチェンジイベントの生成
Declare Function MIDIEvent_CreateControlChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lNum As Long, _
    ByVal lVal As Long) As Long

' RPNチェンジイベントの生成
' (CC#101+CC#100+CC#6の3イベントを生成し、CC#101へのポインタを返す)
Declare Function MIDIEvent_CreateRPNChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lCC101 As Long, _
    ByVal lCC100 As Long, ByVal lVal As Long) As Long

' NRPNチェンジイベントの生成
' (CC#99+CC#98+CC#6の3イベントを生成し、CC#99へのポインタを返す)
Declare Function MIDIEvent_CreateNRPNChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lCC99 As Long, _
    ByVal lCC98 As Long, ByVal lVal As Long) As Long

' プログラムチェンジイベントの生成
Declare Function MIDIEvent_CreateProgramChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lVal As Long) As Long

' パッチチェンジイベントの生成
' (CC#32+CC#0+PCの3イベントを生成し、CC#0へのポインタを返す)
Declare Function MIDIEvent_CreatePatchChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lCC0 As Long, _
    ByVal lCC32 As Long, ByVal lNum As Long) As Long

' チャンネルアフタータッチイベントの生成
Declare Function MIDIEvent_CreateChannelAftertouch Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lVal As Long) As Long

' ピッチベンドイベントの生成
Declare Function MIDIEvent_CreatePitchBend Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal nCh As Long, ByVal lVal As Long) As Long

' システムエクスクルーシヴイベントの生成
Declare Function MIDIEvent_CreateSysExEvent Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByRef pData As Byte, ByVal lLen As Long) As Long


' メタイベントであるかどうかを調べる
Declare Function MIDIEvent_IsMetaEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' シーケンス番号イベントであるかどうかを調べる
Declare Function MIDIEvent_IsSequenceNumber Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' テキストイベントであるかどうかを調べる
Declare Function MIDIEvent_IsTextEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 著作権イベントであるかどうかを調べる
Declare Function MIDIEvent_IsCopyrightNotice Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' トラック名イベントであるかどうかを調べる
Declare Function MIDIEvent_IsTrackName Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 歌詞イベントであるかどうかを調べる
Declare Function MIDIEvent_IsLyric Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' マーカーイベントであるかどうかを調べる
Declare Function MIDIEvent_IsMarker Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' キューポイントイベントであるかどうかを調べる
Declare Function MIDIEvent_IsCuePoint Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' プログラム名イベントであるかどうかを調べる
Declare Function MIDIEvent_IsProgramName Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' デバイス名イベントであるかどうかを調べる
Declare Function MIDIEvent_IsDeviceName Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' チャンネルプレフィックスイベントであるかどうかを調べる
Declare Function MIDIEvent_IsChannelPrefix Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' ポートプレフィックスイベントであるかどうかを調べる
Declare Function MIDIEvent_IsPortPrefix Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' エンドオブトラックイベントであるかどうかを調べる
Declare Function MIDIEvent_IsEndofTrack Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' テンポイベントであるかどうかを調べる
Declare Function MIDIEvent_IsTempo Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' SMPTEオフセットイベントであるかどうかを調べる
Declare Function MIDIEvent_IsSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' 拍子記号イベントであるかどうかを調べる
Declare Function MIDIEvent_IsTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' 調性記号イベントであるかどうかを調べる
Declare Function MIDIEvent_IsKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' シーケンサ独自のイベントであるかどうか調べる
Declare Function MIDIEvnet_IsSequencerSpecific Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    

' MIDIイベントであるかどうかを調べる
Declare Function MIDIEvent_IsMIDIEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' ノートオンイベントであるかどうかを調べる
' (ノートオンでもベロシティが0ものはノートオフとみなす。以下同様)
Declare Function MIDIEvent_IsNoteOn Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' ノートオフイベントであるかどうかを調べる
Declare Function MIDIEvent_IsNoteOff Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' ノートイベントであるかどうかを調べる
Declare Function MIDIEvent_IsNote Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long


' NOTEONOTEOFFイベントであるかどうかを調べる
' これはノートオン(0x9n)とノートオフ(0x8n)が結合イベントしたイベントでなければならない。
Declare Function MIDIEvent_IsNoteOnNoteOff Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' NOTEONNOTEON0イベントであるかどうかを調べる
' これはノートオン(0x9n)とノートオフ(0x9n,vel==0)が結合イベントしたイベントでなければならない。
Declare Function MIDIEvent_IsNoteOnNoteOn0 Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' キーアフタータッチイベントであるかどうかを調べる
Declare Function MIDIEvent_IsKeyAftertouch Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' コントロールチェンジイベントであるかどうかを調べる
Declare Function MIDIEvent_IsControlChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' RPNチェンジイベントであるかどうかを調べる
Declare Function MIDIEvent_IsRPNChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' NRPNチェンジイベントであるかどうかを調べる
Declare Function MIDIEvent_IsNRPNChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' プログラムチェンジイベントであるかどうかを調べる
Declare Function MIDIEvent_IsProgramChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' パッチチェンジイベントであるかどうかを調べる
Declare Function MIDIEvent_IsPatchChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' チャンネルアフタータッチイベントであるかどうかを調べる
Declare Function MIDIEvent_IsChannelAftertouch Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' ピッチベンドイベントであるかどうかを調べる
Declare Function MIDIEvent_IsPitchBend Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' システムエクスクルーシヴイベントであるかどうかを調べる
Declare Function MIDIEvent_IsSysExEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 浮遊イベントであるかどうか調べる
Declare Function MIDIEvent_IsFloating Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)
    
' 結合イベントであるかどうか調べる
Declare Function MIDIEvent_IsCombined Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)
    

' イベントの種類を取得
Declare Function MIDIEvent_GetKind Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントの種類を設定
Declare Function MIDIEvent_SetKind Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lKind As Long) As Long

' イベントの長さ取得
Declare Function MIDIEvent_GetLen Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントのデータ部を取得
Declare Function MIDIEvent_GetData Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' イベントのデータ部を設定(この関数は大変危険です。整合性チェキはしません)
Declare Function MIDIEvent_SetData Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' イベントのテキストを取得(テキスト・著作権・トラック名・インストゥルメント名・
' 歌詞・マーカー・キューポイント・プログラム名・デバイス名のみ)
Declare Function MIDIEvent_GetText Lib "MIDIData.dll" _
    Alias "MIDIEvent_GetTextA" _
    (ByVal pEvent As Long, ByVal strBuf As String, ByVal lLen As Long) As Long

' イベントのテキストを設定(テキスト・著作権・トラック名・インストゥルメント名・
' 歌詞・マーカー・キューポイント・プログラム名・デバイス名のみ)
Declare Function MIDIEvent_SetText Lib "MIDIData.dll" _
    Alias "MIDIEvent_SetTextA" _
    (ByVal pEvent As Long, ByVal strText As String) As Long

' SMPTEオフセット取得(SMPTEオフセットイベントのみ)
Declare Function MIDIEvent_GetSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pMode As Long, ByRef pHour As Long, _
    ByRef pMin As Long, ByRef pSec As Long, ByRef pFrame As Long, _
    ByRef pSubFrame As Long) As Long

' SMPTEオフセット設定(SMPTEオフセットイベントのみ)
Declare Function MIDIEvent_SetSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lMode As Long, ByVal lHour As Long, _
    ByVal lMin As Long, ByVal lSec As Long, ByVal lFrame As Long, _
    ByVal lSubFrame As Long) As Long

' テンポ取得(テンポイベントのみ)
Declare Function MIDIEvent_GetTempo Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' テンポ設定(テンポイベントのみ)
Declare Function MIDIEvent_SetTempo Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lTempo As Long) As Long

' 拍子記号取得(拍子記号イベントのみ)
Declare Function MIDIEvent_GetTimeSignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pnn As Long, ByRef pdd As Long, _
     ByRef pcc As Long, ByRef pbb As Long) As Long

' 拍子記号の設定(拍子記号イベントのみ)
Declare Function MIDIEvent_SetTimeSignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lnn As Long, ByVal ldd As Long, _
     ByVal lcc As Long, ByVal lbb As Long) As Long

' 調性記号の取得(調性記号イベントのみ)
Declare Function MIDIEvent_GetKeySignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef psf As Long, ByRef pmi As Long) As Long

' 調性記号の設定(調性記号イベントのみ)
Declare Function MIDIEvent_SetKeySignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lsf As Long, ByVal lmi As Long) As Long

' イベントのメッセージ取得(MIDIチャンネルイベント及びシステムエクスクルーシヴのみ)
Declare Function MIDIEvent_GetMIDIMessage Lib "MIDIData.dll" _
(ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' イベントのメッセージ設定(MIDIチャンネルイベント及びシステムエクスクルーシヴのみ)
Declare Function MIDIEvent_SetMIDIMessage Lib "MIDIData.dll" _
(ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' イベントのチャンネル取得(MIDIチャンネルイベントのみ)
Declare Function MIDIEvent_GetChannel Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントのチャンネル設定(MIDIチャンネルイベントのみ)
Declare Function MIDIEvent_SetChannel Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lChannel As Long) As Long

' イベントの時刻取得
Declare Function MIDIEvent_GetTime Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントの時刻設定
Declare Function MIDIEvent_SetTime Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lTime As Long) As Long

' イベントのキー取得(ノートオフ・ノートオン・チャンネルアフターのみ)
Declare Function MIDIEvent_GetKey Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントのキー設定(ノートオフ・ノートオン・チャンネルアフターのみ)
Declare Function MIDIEvent_SetKey Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lKey As Long) As Long

' イベントのベロシティ取得(ノートオフ・ノートオンのみ)
Declare Function MIDIEvent_GetVelocity Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントのベロシティ設定(ノートオフ・ノートオンのみ)
Declare Function MIDIEvent_SetVelocity Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lVel As Long) As Long

' 結合イベントの音長さ取得(ノートのみ)
Declare Function MIDIEvent_GetDuration Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' 結合イベントの音長さ設定(ノートのみ)
Declare Function MIDIEvent_SetDuration Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' 結合イベントのバンク取得(パッチチェンジ・RPN/NRPNチェンジのみ)
Declare Function MIDIEvent_GetBank Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' 結合イベントのバンク上位(MSB)取得(パッチチェンジ・RPN/NRPNチェンジのみ)
Declare Function MIDIEvent_GetBankMSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' 結合イベントのバンク下位(LSB)取得(パッチチェンジ・RPN/NRPNチェンジのみ)
Declare Function MIDIEvent_GetBankLSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' 結合イベントのバンク設定(パッチチェンジ・RPN/NRPNチェンジのみ)
Declare Function MIDIEvent_SetBank Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lBank As Long) As Long

' 結合イベントのバンク上位(MSB)設定(パッチチェンジ・RPN/NRPNチェンジのみ)
Declare Function MIDIEvent_SetBankMSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lBank As Long) As Long

' 結合イベントのバンク下位(LSB)設定(パッチチェンジ・RPN/NRPNチェンジのみ)
Declare Function MIDIEvent_SetBankLSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lBank As Long) As Long

' イベントの番号取得(コントロールチェンジ・プログラムチェンジなどのみ)
Declare Function MIDIEvent_GetNumber Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントの番号設定(コントロールチェンジ・プログラムチェンジなどのみ)
Declare Function MIDIEvent_SetNumber Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lNumber As Long) As Long

' イベントの値取得(キーアフター・コントローラー・チャンネルアフター・ピッチベンド)
Declare Function MIDIEvent_GetValue Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' イベントの値設定(キーアフター・コントローラー・チャンネルアフター・ピッチベンド)
Declare Function MIDIEvent_SetValue Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lValue As Long) As Long

' 次のイベントへのポインタを取得(なければNULLを返す)
Declare Function MIDIEvent_GetNextEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 前のイベントへのポインタを取得(なければNULLを返す)
Declare Function MIDIEvent_GetPrevEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 次の同種のイベントへのポインタを取得(なければNULLを返す)
Declare Function MIDIEvent_GetNextSameKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 前の同種のイベントへのポインタを取得(なければNULLを返す)
Declare Function MIDIEvent_GetPrevSameKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 親トラックへのポインタを取得(なければNULL)
Declare Function MIDIEvnet_GetParent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' 文字列表現に変換(仮。この関数の仕様は頻繁に変更されます)
Declare Function MIDIEvent_ToString Lib "MIDIData.dll" _
    Alias "MIDIEvent_ToStringA" _
    (ByVal pEvent As Long, ByVal pBuf As String, ByVal lLen As Long) As Long

'*********************************************************************/
'*                                                                   */
'*　MIDITrackクラス関数                                               */
'*                                                                   */
'*********************************************************************/

' トラック内のイベントの総数を取得
Declare Function MIDITrack_GetNumEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの最初のイベント取得(なければNULLを返す)
Declare Function MIDITrack_GetFirstEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの最後のイベント取得(なければNULLを返す)
Declare Function MIDITrack_GetLastEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの指定種類の最初のイベント取得(なければNULLを返す)
Declare Function MIDITrack_GetFirstKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lKind As Long) As Long

' トラックの指定種類の最後のイベント取得(なければNULLを返す)
Declare Function MIDITrack_GetLastKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lKind As Long) As Long

' 次のMIDIトラックへのポインタ取得(なければNULL)(20080715追加)
Declare Function MIDITrack_GetNextTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' 前のMIDIトラックへのポインタ取得(なければNULL)(20080715追加)
Declare Function MIDITrack_GetPrevTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの親MIDIデータへのポインタを取得(なければNULL)
Declare Function MIDITrack_GetParent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックのイベント数をカウントして取得
Declare Function MIDITrack_CountNumEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの開始時刻(最初のイベントの時刻)[Tick]を取得(20081101追加)
Declare Function MIDITrack_GetBeginTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの終了時刻(最後のイベントの時刻)[Tick]を取得(20081101追加)
Declare Function MIDITrack_GetEndTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの名前を簡易取得
Declare Function MIDITrack_GetName Lib "MIDIData.dll" _
    Alias "MIDITrack_GetNameA" _
    (ByVal pMIDITrack As Long, ByVal pBuf As String, ByVal lLen As Long) As Long

' トラックの入力を取得(0=OFF, 1=ON)
Declare Function MIDITrack_GetInputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの入力ポートを取得(0～255)
Declare Function MIDITrack_GetInputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの入力チャンネルを取得(-1=n/a, 0～15)
Declare Function MIDITrack_GetInputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの出力を取得(0=OFF, 1=ON)
Declare Function MIDITrack_GetOutputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの出力ポートを取得(0～255)
Declare Function MIDITrack_GetOutputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの出力チャンネルを取得(-1=n/a, 0～15)
Declare Function MIDITrack_GetOutputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックのタイム+を取得
Declare Function MIDITrack_GetTimePlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックのキー+を取得
Declare Function MIDITrack_GetKeyPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックのベロシティ+を取得
Declare Function MIDITrack_GetVelPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの表示モードを取得
Declare Function MIDITrack_GetViewMode Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの前景色を取得
Declare Function MIDITrack_GetForeColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの背景色を取得
Declare Function MIDITrack_GetBackColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックの名前を簡易設定
Declare Function MIDITrack_SetName Lib "MIDIData.dll" _
    Alias "MIDITrack_SetNameA" _
    (ByVal pMIDITrack As Long, ByVal pszText As String) As Long

' トラックの入力を設定(0=OFF, 1=ON)
Declare Function MIDITrack_SetInputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lInputOn As Long) As Long

' トラックの入力ポートを設定(0～255)
Declare Function MIDITrack_SetInputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lInputPort As Long) As Long

' トラックの入力チャンネルを設定(-1=n/a, 0～15)
Declare Function MIDITrack_SetInputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lInputChannel As Long) As Long

' トラックの出力を設定(0=OFF, 1=ON)
Declare Function MIDITrack_SetOutputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lOutputOn As Long) As Long

' トラックの出力ポートを設定(0～255)
Declare Function MIDITrack_SetOutputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lOutputPort As Long) As Long

' トラックの出力チャンネルを設定(-1=n/a, 0～15)
Declare Function MIDITrack_SetOutputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lOutputChannel As Long) As Long

' トラックのタイム+を設定
Declare Function MIDITrack_SetTimePlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTimePlus As Long) As Long

' トラックのキー+を設定
Declare Function MIDITrack_SetKeyPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lKeyPlus As Long) As Long

' トラックのベロシティ+を設定
Declare Function MIDITrack_SetVelPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lVelPlus As Long) As Long

' トラックの表示モードを設定
Declare Function MIDITrack_SetViewMode Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lViewMode As Long) As Long

' トラックの前景色を設定
Declare Function MIDITrack_SetForeColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lForeColor As Long) As Long

' トラックの背景色を設定
Declare Function MIDITrack_SetBackColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lBackColor As Long) As Long

' XFであるとき、XFのヴァージョンを取得(XFでなければ0)
Declare Function MIDITrack_GetXFVersion Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long
    
' トラックの削除(トラック内に含まれるイベントオブジェクトも削除されます)
Declare Function MIDITrack_Delete Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックを生成しトラックへのポインタを返す(失敗時NULLを返す)
Declare Function MIDITrack_Create Lib "MIDIData.dll" () As Long

' 指定トラックと同じMIDIトラックを生成し、MIDIトラックへのポインタを返す(失敗時NULLを返す)
Declare Function MIDITrack_CreateClone Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' トラックにイベントを挿入(イベントはあらかじめ生成しておく)
Declare Function MIDITrack_InsertEventAfter Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long, ByVal pTarget As Long) As Long

' トラックにイベントを挿入(イベントはあらかじめ生成しておく)
Declare Function MIDITrack_InsertEventBefore Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long, ByVal pTarget As Long) As Long

' トラックにイベントを挿入(イベントはあらかじめ生成しておく)
Declare Function MIDITrack_InsertEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long) As Long

' トラックにシーケンス番号イベントを生成して挿入
Declare Function MIDITrack_InsertSequenceNumber Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lNumber As Long) As Long

' トラックにテキストイベントを生成して挿入
Declare Function MIDITrack_InsertTextEvent Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertTextEventA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックに著作権イベントを生成して挿入
Declare Function MIDITrack_InsertCopyrightNotice Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertCopyrightNoticeA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックにトラック名イベントを生成して挿入
Declare Function MIDITrack_InsertTrackName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertTrackNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックにインストゥルメント名イベントを生成して挿入
Declare Function MIDITrack_InsertInstrumentName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertInstrumentNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックに歌詞イベントを生成して挿入
Declare Function MIDITrack_InsertLyric Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertLyricA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックにマーカーイベントを生成して挿入
Declare Function MIDITrack_InsertMarker Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertMarkerA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックにキューポイントイベントを生成して挿入
Declare Function MIDITrack_InsertCuePoint Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertCuePointA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックにプログラム名イベントを生成して挿入
Declare Function MIDITrack_InsertProgramName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertProgramNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックにデバイス名イベントを生成して挿入
Declare Function MIDITrack_InsertDeviceName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertDeviceNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' トラックにチャンネルプレフィックスを生成して挿入
Declare Function MIDITrack_InsertChannelPrefix Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lChannel As Long) As Long

' トラックにポートプレフィックスを生成して挿入
Declare Function MIDITrack_InsertPortPrefix Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lPort As Long) As Long

' トラックにエンドオブトラックイベントを生成して挿入
Declare Function MIDITrack_InsertEndofTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long) As Long

' トラックにテンポイベントを生成して挿入
Declare Function MIDITrack_InsertTempo Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lTempo As Long) As Long

' トラックにSMPTEオフセットイベントを生成して挿入
Declare Function MIDITrack_InsertSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByVal lMode As Long, ByVal lHour As Long, ByVal lMin As Long, ByVal lSec As Long, _
    ByVal lFrame As Long, ByVal lSubFrame As Long) As Long

' トラックに拍子記号イベントを生成して挿入
Declare Function MIDITrack_InsertTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByVal lnn As Long, ByVal ldd As Long, _
     ByVal lcc As Long, ByVal lbb As Long) As Long

' トラックに調性記号イベントを生成して挿入
Declare Function MIDITrack_InsertKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByVal lsf As Long, ByVal lmi As Long) As Long

' トラックにシーケンサー独自のイベントを生成して挿入
Declare Function MIDITrack_InsertSequencerSpecific Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByRef pBuf As Byte, ByVal lLen As Long) As Long
     
' トラックにノートオフイベントを生成して挿入
Declare Function MIDITrack_InsertNoteOff Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVel As Long) As Long
     
' トラックにノートオンイベントを生成して挿入
Declare Function MIDITrack_InsertNoteOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVel As Long) As Long

' トラックにノートイベントを生成して挿入
Declare Function MIDITrack_InsertNote Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVel As Long, ByVal lDur As Long) As Long

' トラックにキーアフタータッチイベントを生成して挿入
Declare Function MIDITrack_InsertKeyAftertouch Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVal As Long) As Long

' トラックにコントロールチェンジイベントを生成して挿入
Declare Function MIDITrack_InsertControlChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lNum As Long, ByVal lVal As Long) As Long

' トラックにRPNイベントを生成して挿入
Declare Function MIDITrack_InsertRPNChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lCC101 As Long, ByVal lCC100 As Long, ByVal lVal As Long) As Long

' トラックにNRPNイベントを生成して挿入
Declare Function MIDITrack_InsertNRPNChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lCC99 As Long, ByVal lCC98 As Long, ByVal lVal As Long) As Long

' トラックにプログラムチェンジイベントを生成して挿入
Declare Function MIDITrack_InsertProgramChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lNum As Long) As Long

' トラックにパッチチェンジイベントを生成して挿入
Declare Function MIDITrack_InsertPatchChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lCC0 As Long, ByVal lCC32 As Long, ByVal lNum As Long) As Long

' トラックにチャンネルアフタータッチイベントを生成して挿入
Declare Function MIDITrack_InsertChannelAftertouch Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lVal As Long) As Long

' トラックにピッチベンドイベントを生成して挿入
Declare Function MIDITrack_InsertPitchBend Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lVal As Long) As Long

' トラックにシステムエクスクルーシヴイベントを生成して挿入
Declare Function MIDITrack_InsertSysExEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByRef pBuf As Byte, _
     ByVal lLen As Long) As Long

' トラックからイベントを1つ取り除く(イベントオブジェクトは削除しません) */
Declare Function MIDITrack_RemoveSingleEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long) As Long

' トラックからイベントを取り除く(イベントオブジェクトは削除しません)
Declare Function MIDITrack_RemoveEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long) As Long

' MIDIトラックが浮遊トラックであるかどうかを調べる
Declare Function MIDITrack_IsFloating Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' MIDIトラックがセットアップトラックとして正しいことを確認する
Declare Function MIDITrack_CheckSetupTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' MIDIトラックがノンセットアップトラックとして正しいことを確認する
Declare Function MIDITrack_CheckNonSetupTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' タイムコードをミリ秒時刻に変換(指定トラック内のテンポイベントを基に計算)
Declare Function MIDITrack_TimeToMillisec Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long) As Long

' ミリ秒時刻をタイムコードに変換(指定トラック内のテンポイベントを基に計算)
Declare Function MIDITrack_MillisecToTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lMillisec As Long) As Long

' タイムコードを小節：拍：ティックに分解(指定トラック内の拍子記号を基に計算)
Declare Function MIDITrack_BreakTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' タイムコードを小節：拍：ティックに分解(指定トラック内の拍子記号から計算)
Declare Function MIDITrack_BreakTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long) As Long

' 小節：拍：ティックからタイムコードを生成(指定トラック内の拍子記号から計算)
Declare Function MIDITrack_MakeTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lMeasure As Long, ByVal lBeat As Long, _
    ByVal lTick As Long, ByRef pTime As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' 小節：拍：ティックからタイムコードを生成(指定トラック内の拍子記号から計算)
Declare Function MIDITrack_MakeTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pMeasure As Long, ByVal pBeat As Long, _
    ByVal pTick As Long, ByRef pTime As Long) As Long

' 指定時刻におけるテンポを取得
Declare Function MIDITrack_FindTempo Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByRef pTempo As Long) As Long

' 指定時刻における拍子記号を取得
Declare Function MIDITrack_FindTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' 指定時刻における調性記号を取得
Declare Function MIDITrack_FindKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByRef psf As Long, ByRef pmi As Long) As Long


'*********************************************************************/
'*                                                                   */
'*　MIDIDataクラス関数                                                */
'*                                                                   */
'*********************************************************************/

' MIDIデータの指定トラックの直前にトラックを挿入
Declare Function MIDIData_InsertTrackBefore Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long, ByVal pTarget As Long) As Long

' MIDIデータの指定トラックの直後にトラックを挿入
Declare Function MIDIData_InsertTrackAfter Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long, ByVal pTarget As Long) As Long

' MIDIデータにトラックを追加(トラックは予め作成しておく)
Declare Function MIDIData_AddTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long) As Long

' MIDIデータからトラックを除去(トラック自体及びトラック内のイベントは削除しない)
Declare Function MIDIData_RemoveTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long) As Long

' MIDIデータの削除(含まれるトラック及びイベントもすべて削除)
Declare Function MIDIData_Delete Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータを生成し、MIDIデータへのポインタを返す(失敗時NULL)
Declare Function MIDIData_Create Lib "MIDIData.dll" _
    (ByVal lFormat As Long, ByVal lNumTrack As Long, _
     ByVal lTimeBase As Long, ByVal lTimeResolution As Long) As Long

' MIDIデータのフォーマット0/1/2を取得
Declare Function MIDIData_GetFormat Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータのフォーマット0/1/2を設定(変更時コンバート機能を含む)
Declare Function MIDIData_SetFormat Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lFormat As Long) As Long

' MIDIデータのタイムベースを取得
Declare Function MIDIData_GetTimeBase Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByRef lTimeMode As Long, _
     ByRef lTimeResolution As Long) As Long

' MIDIデータのタイムモード取得
Declare Function MIDIData_GetTimeMode Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータのタイムレゾリューション取得
Declare Function MIDIData_GetTimeResolution Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータのタイムベース設定
Declare Function MIDIData_SetTimeBase Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTimeMode As Long, _
     ByVal lTimeResolution As Long) As Long

' MIDIデータのトラック数取得
Declare Function MIDIData_GetNumTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータのトラック数をカウントして取得
Declare Function MIDIData_CountTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' XFであるとき、XFのヴァージョンを取得(XFでなければ0)
Declare Function MIDIData_GetXFVersion Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータの最初のトラックへのポインタを取得(なければNULL)
Declare Function MIDIData_GetFirstTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータの最後のトラックへのポインタを取得(なければNULL)
Declare Function MIDIData_GetLastTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータの指定番号のトラックへのポインタを取得(なければNULL)
Declare Function MIDIData_GetTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTrackIndex As Long) As Long

' MIDIデータの開始時刻[tick]の取得
Declare Function MIDIData_GetBeginTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータの終了時刻[tick]の取得
Declare Function MIDIData_GetEndTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDIデータのタイトルの簡易取得
Declare Function MIDIData_GetTitle Lib "MIDIData.dll" _
    Alias "MIDIData_GetTitleA" _
    (ByVal pMIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDIデータのタイトルの簡易設定
Declare Function MIDIData_SetTitle Lib "MIDIData.dll" _
    Alias "MIDIData_SetTitleA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' MIDIデータのサブタイトルの簡易取得
Declare Function MIDIData_GetSubTitle Lib "MIDIData.dll" _
    Alias "MIDIData_GetSubTitleA" _
    (ByVal pMIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDIデータのサブタイトルの簡易設定
Declare Function MIDIData_SetSubTitle Lib "MIDIData.dll" _
    Alias "MIDIData_SetSubTitleA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' MIDIデータの著作権の簡易取得
Declare Function MIDIData_GetCopyright Lib "MIDIData.dll" _
    Alias "MIDIData_GetCopyrightA" _
    (ByVal MIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDIデータの著作権の簡易設定
Declare Function MIDIData_SetCopyright Lib "MIDIData.dll" _
    Alias "MIDIData_SetCopyrightA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' MIDIデータのコメントの簡易取得
Declare Function MIDIData_GetComment Lib "MIDIData.dll" _
    Alias "MIDIData_GetCommentA" _
    (ByVal pMIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDIデータのコメントの簡易設定
Declare Function MIDIData_SetComment Lib "MIDIData.dll" _
    Alias "MIDIData_SetCommentA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' タイムコードをミリ秒に変換(フォーマット0/1の場合のみ)
Declare Function MIDIData_TimeToMillisec Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long) As Long

' ミリ秒をタイムコードに変換(フォーマット0/1の場合のみ)
Declare Function MIDIData_MillisecToTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lMillisec As Long) As Long

' タイムコードを小節：拍：ティックに分解(最初のトラック内の拍子記号を基に計算)
Declare Function MIDIData_BreakTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' タイムコードを小節：拍：ティックに分解(最初のトラック内の拍子記号から計算)
Declare Function MIDIData_BreakTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long) As Long

' 小節：拍：ティックからタイムコードを生成(最初のトラック内の拍子記号から計算)
Declare Function MIDIData_MakeTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lMeasure As Long, ByVal lBeat As Long, _
    ByVal lTick As Long, ByRef pTime As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' 小節：拍：ティックからタイムコードを生成(最初のトラック内の拍子記号から計算)
Declare Function MIDIData_MakeTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMeasure As Long, ByVal pBeat As Long, _
    ByVal pTick As Long, ByRef pTime As Long) As Long

' 指定時刻におけるテンポを取得
Declare Function MIDIData_FindTempo Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, ByRef pTempo As Long) As Long

' 指定時刻における拍子記号を取得
Declare Function MIDIData_FindTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
     ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' 指定時刻における調性記号を取得
Declare Function MIDIData_FindKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
    ByRef psf As Long, ByRef pmi As Long) As Long

' このMIDIデータに別のMIDIデータをマージする(20080715廃止)
'Declare Function MIDIData_Merge Lib "MIDIData.dll" _
'    (ByVal pMIDIData As Long, ByVal pMergeData As Long, _
'     ByVal lTime As Long, ByVal lFlags As Long, _
'     ByRef pInsertedEventCount As Long, ByRef pDeletedEventCount As Long) As Long

' 保存・読み込み用関数

' MIDIDataをスタンダードMIDIファイル(SMF)から読み込み、
' 新しいMIDIデータへのポインタを返す(失敗時NULL)
Declare Function MIDIData_LoadFromSMF Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromSMFA" _
    (ByVal strFileName As String) As Long

' MIDIデータをスタンダードMIDIファイル(SMF)として保存
Declare Function MIDIData_SaveAsSMF Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsSMFA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long

' MIDIデータをバイナリファイルより読み込み、
' 新しいMIDIデータへのポインタを返す(失敗時NULL)
Declare Function MIDIData_LoadFromBinary Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromBinaryA" _
    (ByVal strFileName As String) As Long

' MIDIデータをバイナリファイルへ保存
Declare Function MIDIData_SaveAsBinary Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsBinaryA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long

' MIDIデータをCherryファイル(*.chy)より読み込み、
' 新しいMIDIデータへのポインタを返す(失敗時NULL)
Declare Function MIDIData_LoadFromCherry Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromCherryA" _
    (ByVal strFileName As String) As Long

' MIDIデータをCherryファイル(*.chy)へ保存
Declare Function MIDIData_SaveAsCherry Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsCherryA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long

' MIDIDataをMIDICSVファイル(*.csv)から読み込み、
' 新しいMIDIデータへのポインタを返す(失敗時NULL)
Declare Function MIDIData_LoadFromMIDICSV Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromMIDICSVA" _
    (ByVal strFileName As String) As Long

' MIDIデータをMIDICSVファイル(*.csv)として保存
Declare Function MIDIData_SaveAsMIDICSV Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsMIDICSVA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long



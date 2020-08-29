'******************************************************************************
'*                                                                            *
'* MIDIClock.bas - MIDIClockモジュール(VB4,5,6用)           (C)2002-2009 くず *
'*                                                                            *
'******************************************************************************

' このモジュールは普通のC言語とWin32APIを使って書かれています。
' MIDIクロックのスタート・ストップ・リセット
' テンポ・タイムベース・経過時刻・経過ティック数の取得・設定
' マスターモード(TPQNBASE、SMPTE24BASE、SMPTE25BASE、SMPTE29BASE、SMPTE30BASE)に対応
' スレーブモード(MIDIタイミングクロック、SMPTE/MTC)に対応
' このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布されます。
' プロジェクトホームページ："http://openmidiproject.sourceforge.jp/index.html"

' This library is free software; you can redistribute it and/or
' modify it under the terms of the GNU Lesser General Public
' License as published by the Free Software Foundation; either
' version 2.1 of the License, or (at your option) any later version.

' This library is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
' Lesser General Public License for more details.

' You should have received a copy of the GNU Lesser General Public
' License along with this library; if not, write to the Free Software
' Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Option Explicit

Public Const MIDICLOCK_MAXTEMPO = 60000000
Public Const MIDICLOCK_MINTEMPO = 1
Public Const MIDICLOCK_MINSPEED = 0
Public Const MIDICLOCK_MAXSPEED = 100000
Public Const MIDICLOCK_SPEEDSLOW = 5000
Public Const MIDICLOCK_SPEEDNORMAL = 10000
Public Const MIDICLOCK_SPEEDFAST = 20000

'タイムモード(以下の値のみ利用可能)
Public Const MIDICLOCK_TPQNBASE = 0  'TPQNベース
Public Const MIDICLOCK_SMPTE24BASE = 24 'SMPTE24ベース(24フレーム/秒)
Public Const MIDICLOCK_SMPTE25BASE = 25 'SMPTE25ベース(25フレーム/秒)
Public Const MIDICLOCK_SMPTE29BASE = 29 'SMPTE29ベース(29.97フレーム/秒)
Public Const MIDICLOCK_SMPTE30BASE = 30 'SMPTE30ベース(30フレーム/秒)

'タイムモード(ver0.6以前との互換のために残してある)
Public Const MIDICLOCK_MASTERTPQNBASE = 0  'TPQNベース
Public Const MIDICLOCK_MASTERSMPTE24BASE = 24 'SMPTE24ベース(24フレーム/秒)
Public Const MIDICLOCK_MASTERSMPTE25BASE = 25 'SMPTE25ベース(25フレーム/秒)
Public Const MIDICLOCK_MASTERSMPTE29BASE = 29 'SMPTE29ベース(29.97フレーム/秒)
Public Const MIDICLOCK_MASTERSMPTE30BASE = 30 'SMPTE30ベース(30フレーム/秒)

'入力同期モード(以下の値のみ利用可能)
Public Const MIDICLOCK_MASTER = 0 'マスター
Public Const MIDICLOCK_SLAVEMIDITIMINGCLOCK = 1  'スレーブモード(MIDIタイミングクロックに追従)
Public Const MIDICLOCK_SLAVESMPTEMTC = 2  ' スレーブモード(SMPTE/MTCに追従)


' MIDIクロックオブジェクトの削除
Declare Sub MIDIClock_Delete Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long)

' MIDIクロックオブジェクトの生成
Declare Function MIDIClock_Create Lib "MIDIClock.dll" _
        (ByVal lTimeMode As Long, ByVal lResolution As Long, ByVal lTempo As Long) As Long

' タイムモードと分解能[tick/4分音符]又は[tick/1フレーム]の取得
Declare Function MIDIClock_GetTimeBase Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByRef pTimeMode As Long, ByRef pResolution As Long) As Long

' タイムモードと分解能[tick/4分音符]又は[tick/1フレーム]の設定
Declare Function MIDIClock_SetTimeBase Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lTimeMode As Long, ByVal lResolution As Long) As Long

' MIDIテンポ[μ秒/4分音符]の取得
Declare Function MIDIClock_GetTempo Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' MIDIテンポ[μ秒/4分音符]の設定
Declare Function MIDIClock_SetTempo Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lTempo As Long) As Long

' スピード[×0.01％]の取得
Declare Function MIDIClock_GetSpeed Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' スピード[×0.01％]の設定
Declare Function MIDIClock_SetSpeed Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lSpeed As Long) As Long

' 同期モードの取得
Declare Function MIDIClock_GetMIDIInSyncMode Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' 同期モードの設定
Declare Function MIDIClock_SetMIDIInSyncMode Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lMIDIInSyncMode As Long) As Long


' スタート
Declare Function MIDIClock_Start Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' ストップ
Declare Function MIDIClock_Stop Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' リセット
Declare Function MIDIClock_Reset Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' 動作中か調べる
Declare Function MIDIClock_IsRunning Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' 経過時刻[ミリ秒]の取得
Declare Function MIDIClock_GetMillisec Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' 経過時刻[ミリ秒]の設定
Declare Function MIDIClock_SetMillisec Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lMillisec As Long) As Long

' 経過ティック数[tick]の取得
Declare Function MIDIClock_GetTickCount Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' 経過ティック数[tick]の設定
Declare Function MIDIClock_SetTickCount Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lTickCount As Long) As Long


' MIDIタイミングクロックを認識させる(0xF8)
Declare Function MIDIClock_PutMIDITimingClock Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' システムエクスクルーシヴによるSMPTE/MTCを認識させる
Declare Function MIDIClock_PutSysExSMPTEMTC Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal cHour As Byte, _
    ByVal cMinute As Byte, ByVal cSecond As Byte, ByVal cFrame As Byte) As Long

' MIDIタイムコードクォーターフレーム(0xF1)によるSMPTE/MTCを認識させる
Declare Function MIDIClock_PutSMPTEMTC Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal cSMPTEMTC As Byte) As Long

' MIDIクロックにMIDIメッセージを認識させる
Declare Function MIDIClock_PutMIDIMessage Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long


'******************************************************************************
'*                                                                            *
'*　MIDIIO.bas - MIDIIOモジュール(VB4,5,6用)               (C)2002-2012 くず  *
'*                                                                            *
'******************************************************************************

' MIDI入力オブジェクトのオープン・クローズ・リセット・データ受信(SYSX含む)
' MIDI出力オブジェクトのオープン・クローズ・リセット・データ送信(SYSX含む)
' このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布されます。
' プロジェクトホームページ："http://openmidiproject.sourceforge.jp/index.html"

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

Option Explicit

Public Const MIDIIO_BUFSIZE = 1024
Public Const MIDIIO_SYSXNUM = 4
Public Const MIDIIO_SYSXBUF = 256

Public Const MIDIIO_NONE = ""
Public Const MIDIIO_DEFAULT = "default"
Public Const MIDIIO_MIDIMAPPER = "MIDI Mapper"


' MIDI出力デバイスの数を調べる
Declare Function MIDIOut_GetDeviceNum Lib "MIDIIO.dll" () As Long

' MIDI出力デバイスの名前を調べる
Declare Function MIDIOut_GetDeviceName Lib "MIDIIO.dll" Alias "MIDIOut_GetDeviceNameA" _
    (ByVal lID As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long

' MIDI出力デバイスを開く
Declare Function MIDIOut_Open Lib "MIDIIO.dll" Alias "MIDIOut_OpenA" _
    (ByVal pszDeviceName As String) As Long

' MIDI出力デバイスを閉じる
Declare Function MIDIOut_Close Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long) As Long

' MIDI出力デバイスを再び開く
Declare Function MIDIOut_Reopen Lib "MIDIIO.dll" Alias "MIDIOut_ReopenA" _
    (ByVal pMIDIOut As Long, ByVal pszDeviceName As String) As Long

' MIDI出力デバイスをリセットする
Declare Function MIDIOut_Reset Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long) As Long

' MIDI出力デバイスにメッセージを1つ出力する
Declare Function MIDIOut_PutMIDIMessage Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long

' MIDI出力デバイスに1バイト出力する
Declare Function MIDIOut_PutByte Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long, ByVal bByte As Byte) As Long

' MIDI出力デバイスに任意長のバイナリデータを出力する
Declare Function MIDIOut_PutBytes Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long, ByRef pData As Byte, ByVal lLen As Long) As Long

' このMIDI出力デバイスの名前を取得する(20120415追加)
Declare Function MIDIOut_GetThisDeviceName Lib "MIDIIO.dll" Alias "MIDIOut_GetThisDeviceNameA" _
    (ByVal pMIDIOut As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long



' MIDI入力デバイスの数を調べる
Declare Function MIDIIn_GetDeviceNum Lib "MIDIIO.dll" () As Long

' MIDI入力デバイスの名前を調べる
Declare Function MIDIIn_GetDeviceName Lib "MIDIIO.dll" Alias "MIDIIn_GetDeviceNameA" _
    (ByVal lID As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long

' MIDI入力デバイスを開く
Declare Function MIDIIn_Open Lib "MIDIIO.dll" Alias "MIDIIn_OpenA" _
    (ByVal pDeviceName As String) As Long

' MIDI入力デバイスを閉じる
Declare Function MIDIIn_Close Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long) As Long

' MIDI入力デバイスを再び開く
Declare Function MIDIIn_Reopen Lib "MIDIIO.dll" Alias "MIDIIn_ReopenA" _
    (ByVal pMIDIIn As Long, ByVal pszDeviceName As String) As Long

' MIDI入力デバイスをリセットする
Declare Function MIDIIn_Reset Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long) As Long

' MIDI入力デバイスからメッセージを1つ入力する
Declare Function MIDIIn_GetMIDIMessage Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long

' MIDI入力デバイスから1バイト入力する
Declare Function MIDIIn_GetByte Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long, ByRef pByte As Byte) As Long

' MIDI入力デバイスから任意長のバイナリデータを入力する
Declare Function MIDIIn_GetBytes Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long, ByRef pData As Byte, ByVal lLen As Long) As Long

' このMIDI入力デバイスの名前を取得する(20120415追加)
Declare Function MIDIIn_GetThisDeviceName Lib "MIDIIO.dll" Alias "MIDIIn_GetThisDeviceNameA" _
    (ByVal pMIDIIn As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long


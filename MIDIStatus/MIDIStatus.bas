Attribute VB_Name = "MIDIStatus"
'******************************************************************************
'*                                                                            *
'*　MIDIStatus.bas - MIDIStatusモジュール(VB4,5,6用)       (C)2002-2012 くず  *
'*                                                                            *
'******************************************************************************

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

Public Const MIDISTATUS_MAXMIDIPART = 128
Public Const MIDISTATUS_MAXMIDIDRUMSETUP = 128

Public Const MIDISTATUS_MODENATIVE = &H0
Public Const MIDISTATUS_MODEGM = &H7E000001
Public Const MIDISTATUS_MODEGM2 = &H7E000003
Public Const MIDISTATUS_MODEGS = &H41000002
Public Const MIDISTATUS_MODE88 = &H41000003
Public Const MIDISTATUS_MODEXG = &H43000002

'******************************************************************************
'*                                                                            *
'* MIDIPart関数群                                                             *
'*                                                                            *
'******************************************************************************

' MIDIPartオブジェクトを削除する。
Declare Sub MIDIPart_Delete Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long)

' MIDIPartオブジェクトを生成する。
Declare Function MIDIPart_Create Lib "MIDIStatus.dll" _
    (ByVal pParent As Long) As Long

' MIDIPart_Get系関数

' 現在のパートモードを取得
Declare Function MIDIPart_GetPartMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のオムニモノポリモードを取得
Declare Function MIDIPart_GetOmniMonoPolyMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のチャンネルファインチューニングの値を取得
Declare Function MIDIPart_GetChannelFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のチャンネルコースチューニングの値を取得
Declare Function MIDIPart_GetChannelCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のピッチベンドセンシティビティの値を取得
Declare Function MIDIPart_GetPitchBendSensitivity Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のモジュレーションデプスレンジの値を取得
Declare Function MIDIPart_GetModulationDepthRange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のレシーブチャンネルの値を取得
Declare Function MIDIPart_GetReceiveChannel Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のベロシティセンスデプスの値を取得
Declare Function MIDIPart_GetVelocitySenseDepth Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のベロシティセンスオフセットの値を取得
Declare Function MIDIPart_GetVelocitySenseOffset Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のキーボードレンジローの値を取得
Declare Function MIDIPart_GetKeyboardRangeLow Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のキーボードレンジハイの値を取得
Declare Function MIDIPart_GetKeyboardRangeHigh Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在の鍵盤の押され具合を取得する。
' 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ)
Declare Function MIDIPart_GetNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long) As Long

' 現在の鍵盤の押され具合をまとめて取得する。
' 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ)
Declare Function MIDIPart_GetNoteEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の鍵盤の押され具合を取得する。
' 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ)
Declare Function MIDIPart_GetNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long) As Long

' 現在の鍵盤の押され具合(ホールド持続含む)をまとめて取得する。
' 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ)
Declare Function MIDIPart_GetNoteKeepEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在のキーアフタータッチの値を取得
Declare Function MIDIPart_GetKeyAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long) As Long

' 現在のキーアフタータッチの値をまとめて取得
Declare Function MIDIPart_GetKeyAfterTouchEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在のコントロールチェンジの値を取得
Declare Function MIDIPart_GetControlChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lNum As Long) As Long

' 現在のコントロールチェンジの値をまとめて取得
Declare Function MIDIPart_GetControlChangeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在のRPNMSBの値を取得
Declare Function MIDIPart_GetRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long) As Long

' 現在のRPNLSBの値を取得
Declare Function MIDIPart_GetRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long) As Long

' 現在のNRPNMSBの値を取得
Declare Function MIDIPart_GetNRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long) As Long

' 現在のNRPNLSBの値を取得
Declare Function MIDIPart_GetNRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long) As Long

' 現在のプログラムチェンジの値を取得
Declare Function MIDIPart_GetProgramChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のチャンネルアフタータッチの値を取得
Declare Function MIDIPart_GetChannelAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在のピッチベンドの値を取得
Declare Function MIDIPart_GetPitchBend Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' MIDIPart_Get系関数(ユニークなもの)

' 現在押されている鍵盤の数を返す。
Declare Function MIDIPart_GetNumNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在押されている鍵盤の数(ホールド持続含む)を返す。
Declare Function MIDIPart_GetNumNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在押されている鍵盤で最も低い音階を返す(ない場合-1)。
Declare Function MIDIPart_GetHighestNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在押されている鍵盤(ホールド持続含む)で最も低い音階を返す(ない場合-1)。
Declare Function MIDIPart_GetHighestNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在押されている鍵盤で最も高い音階を返す(ない場合-1)。
Declare Function MIDIPart_GetLowestNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' 現在押されている鍵盤(ホールド持続含む)で最も高い音階を返す(ない場合-1)。
Declare Function MIDIPart_GetLowestNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' MIDIPart_Set系関数

' 現在のパートモードを設定
Declare Function MIDIPart_SetPartMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lPartMode As Long) As Long

' 現在のオムニON/OFF、モノ/ポリモードを設定
Declare Function MIDIPart_SetOmniMonoPolyMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lOmniMonoPolyMode As Long) As Long

' 現在のチャンネルファインチューニングの値を設定
Declare Function MIDIPart_SetChannelFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lChannelFineTuning As Long) As Long

' 現在のチャンネルコースチューニングの値を設定
Declare Function MIDIPart_SetChannelCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lChannelCoarseTuning As Long) As Long

' 現在のピッチベンドセンシティビティの値を設定
Declare Function MIDIPart_SetPitchBendSensitivity Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lPitchBendSensitivity As Long) As Long

' 現在のモジュレーションデプスレンジの値を設定
Declare Function MIDIPart_SetModulationDepthRange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lModulationDepthRange As Long)

' 現在のレシーブチャンネルの値を設定
Declare Function MIDIPart_SetReseiveChannel Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lReceiveChannel As Long) As Long

' 現在のベロシティセンスデプスの値を設定
Declare Function MIDIPart_SetVelocitySenseDepth Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVelocitySenseDepth As Long) As Long

' 現在のベロシティセンスオフセットの値を設定
Declare Function MIDIPart_SetVelocitySenseOffset Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVelocitySenseOffset As Long) As Long

' 現在のキーボードレンジローの値を設定
Declare Function MIDIPart_SetKeyboardRangeLow Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKeyboardRangeLow As Long) As Long

' 現在のキーボードレンジハイの値を設定
Declare Function MIDIPart_SetKeyboardRangeHigh Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKeyboardRangeHigh As Long) As Long

' 現在の鍵盤の押され具合を設定
Declare Function MIDIPart_SetNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long, ByVal lVel As Long) As Long

' 現在のキーアフタータッチの値を設定
Declare Function MIDIPart_SetKeyAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在のコントロールチェンジの値を設定
Declare Function MIDIPart_SetControlChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' 現在のRPNMSBの値を設定
Declare Function MIDIPart_SetRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long, ByVal lVal As Long) As Long

' 現在のRPNLSBの値を設定
Declare Function MIDIPart_SetRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long, ByVal lVal As Long) As Long

' 現在のNRPNMSBの値を設定
Declare Function MIDIPart_SetNRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long, ByVal lVal As Long) As Long

' 現在のNRPNLSBの値を設定
Declare Function MIDIPart_SetNRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long, ByVal lVal As Long) As Long

' 現在のプログラムチェンジの値を設定
Declare Function MIDIPart_SetProgramChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lNum As Long) As Long

' 現在のチャンネルアフタータッチの値を設定
Declare Function MIDIPart_SetChannelAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVal As Long) As Long

' 現在のピッチベンドの値を設定
Declare Function MIDIPart_SetPitchBend Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVal As Long) As Long


' MIDIPartの書き込み(外部隠蔽、この関数はMIDIStatus_Writeから呼び出される)
Declare Function MIDIPart_Write Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long

' MIDIPartの読み込み(外部隠蔽、この関数はMIDIStatus_Readから呼び出される)
Declare Function MIDIPart_Read Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long







'******************************************************************************
'*                                                                            *
'* MIDIDrumSetup関数群                                                        *
'*                                                                            *
'******************************************************************************

' MIDIDrumSetupオブジェクトを削除する。
Declare Sub MIDIDrumSetup_Delete Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long)

' MIDIDrumSetupオブジェクトを生成する。
Declare Function MIDIDrumSetup_Create Lib "MIDIStatus.dll" _
    (ByVal pParent As Long) As Long

' MIDIDrumSetup_Get系関数

' 現在の楽器ごとのカットオフフリーケンシーの値を取得
Declare Function MIDIDrumSetup_GetDrumCutoffFrequency Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのカットオフフリーケンシーの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumCutoffFrequencyEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのレゾナンスの値を取得
Declare Function MIDIDrumSetup_GetDrumResonance Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのレゾナンスの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumResonanceEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのアタックタイムの値を取得
Declare Function MIDIDrumSetup_GetDrumAttackTime Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのアタックタイムの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumAttackTimeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのディケイタイム1の値を取得
Declare Function MIDIDrumSetup_GetDrumDecay1Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのディケイタイム1の値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumDecay1TimeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのディケイタイム2の値を取得
Declare Function MIDIDrumSetup_GetDrumDecay2Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのディケイタイム2の値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumDecay2TimeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのピッチコースの値を取得
Declare Function MIDIDrumSetup_GetDrumPitchCoarse Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのピッチコースの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumPitchCoarseEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのピッチファインの値を取得
Declare Function MIDIDrumSetup_GetDrumPitchFine Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのピッチファインの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumPitchFineEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのボリュームの値を取得
Declare Function MIDIDrumSetup_GetDrumVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのボリュームの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumVolumeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのパンの値を取得
Declare Function MIDIDrumSetup_GetDrumPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのパンの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumPanEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのリバーブの値を取得
Declare Function MIDIDrumSetup_GetDrumReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのリバーブの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumReverbEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのコーラスの値を取得
Declare Function MIDIDrumSetup_GetDrumChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのコーラスの値をまとめて取得
Declare Function MIDIDrumSetup_GetDrumChorusEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' 現在の楽器ごとのディレイの値を取得(XGではディレイはヴァリエーションと読み替える)
Declare Function MIDIDrumSetup_GetDrumDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' 現在の楽器ごとのディレイの値をまとめて取得(XGではディレイはヴァリエーションと読み替える)
Declare Function MIDIDrumSetup_GetDrumDelayEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' MIDIDrumSetup_Set系関数

' 現在の楽器ごとのカットオフフリーケンシーの値を設定
Declare Function MIDIDrumSetup_SetDrumCutoffFrequency Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのレゾナンスの値を設定
Declare Function MIDIDrumSetup_SetDrumResonance Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのアタックタイムの値を設定
Declare Function MIDIDrumSetup_SetDrumAttackTime Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのディケイタイム1の値を設定
Declare Function MIDIDrumSetup_SetDrumDecay1Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのディケイタイム2の値を設定
Declare Function MIDIDrumSetup_SetDrumDecay2Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのピッチコースの値を設定
Declare Function MIDIDrumSetup_SetDrumPitchCoarse Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのピッチファインの値を設定
Declare Function MIDIDrumSetup_SetDrumPitchFine Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのボリュームの値を設定
Declare Function MIDIDrumSetup_SetDrumVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのパンの値を設定
Declare Function MIDIDrumSetup_SetDrumPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのリバーブの値を設定
Declare Function MIDIDrumSetup_SetDrumReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのコーラスの値を設定
Declare Function MIDIDrumSetup_SetDrumChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' 現在の楽器ごとのディレイの値を設定(XGではディレイはヴァリエーションと読み替える)
Declare Function MIDIDrumSetup_SetDrumDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long


' MIDIDrumSetupの書き込み(外部隠蔽、この関数はMIDIStatus_Writeから呼び出される)
Declare Function MIDIDrumSetup_Write Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long

' MIDIDrumSetupの読み込み(外部隠蔽、この関数はMIDIStatus_Readから呼び出される)
Declare Function MIDIDrumSetup_Read Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long



'******************************************************************************
'*                                                                            *
'* MIDIStatus関数群                                                           *
'*                                                                            *
'******************************************************************************

' MIDIStatusオブジェクトを削除する。
Declare Sub MIDIStatus_Delete Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long)

' MIDIStatusオブジェクトを生成する。
Declare Function MIDIStatus_Create Lib "MIDIStatus.dll" _
    (ByVal lModuleMode As Long, ByVal lNumMIDIPart As Long, ByVal lNumMIDIDrumSetup As Long) As Long

' MIDIStatus_Get系関数

' 現在のモジュールモード(NATIVE/GM/GM2/GS/88/XG)を取得
Declare Function MIDIStatus_GetModuleMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 現在のマスターチューンの値を取得
Declare Function MIDIStatus_GetMasterFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 現在のマスターキーシフトの値を取得
Declare Function MIDIStatus_GetMasterCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 現在のマスターボリュームの値を取得
Declare Function MIDIStatus_GetMasterVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 現在のマスターパンの値を取得
Declare Function MIDIStatus_GetMasterPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 現在のマスターリバーブの値を1つ取得
Declare Function MIDIStatus_GetMasterReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' 現在のマスターリバーブの値をまとめて取得
Declare Function MIDIStatus_GetMasterReverbEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' 現在のマスターコーラスの値を1つ取得
Declare Function MIDIStatus_GetMasterChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' 現在のマスターコーラスの値をまとめて取得
Declare Function MIDIStatus_GetMasterChorusEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' 現在のマスターディレイの値を1つ取得(XGではディレイはヴァリエーションと読み替える)
Declare Function MIDIStatus_GetMasterDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' 現在のマスターディレイの値をまとめて取得(XGではディレイはヴァリエーションと読み替える)
Declare Function MIDIStatus_GetMasterDelayEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' 現在のマスターイコライザの値を1つ取得
Declare Function MIDIStatus_GetMasterEqualizer Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' 現在のマスターイコライザの値をまとめて取得
Declare Function MIDIStatus_GetMasterEqualizerEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' このMIDIStatusオブジェクトに含まれるMIDIPartの数を取得
Declare Function MIDIStatus_GetNumMIDIPart Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' このMIDIStatusオブジェクトに含まれるMIDIDrumSetupの数を取得
Declare Function MIDIStatus_GetNumMIDIDrumSetup Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 各MIDIPartへのポインタを取得
Declare Function MIDIStatus_GetMIDIPart Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lIndex) As Long

' 各MIDIDrumSetupへのポインタを取得
Declare Function MIDIStatus_GetMIDIDrumSetup Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lIndex) As Long

' MIDIStatus_Set系関数

' モジュールモード(NATIVE/GM/GM2/GS/88/XG)を設定する。
' モジュールモードの設定に従いすべてのパラメータをデフォルト値に設定する。
Declare Function MIDIStatus_SetModuleMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lModuleMode As Long) As Long

' 現在のマスターファインチューニングの値を設定
Declare Function MIDIStatus_SetMasterFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterFineTuning As Long) As Long

' 現在のマスターコースチューニングの値を設定
Declare Function MIDIStatus_SetMasterCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterCoarseTuning As Long) As Long

' 現在のマスターボリュームの値を設定
Declare Function MIDIStatus_SetMasterVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterVolume As Long) As Long

' 現在のマスターパンの値を設定
Declare Function MIDIStatus_SetMasterPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterPan As Long) As Long

' 現在のマスターリバーブの値を1つ設定
Declare Function MIDIStatus_SetMasterReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' マスターリバーブのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。
Declare Function MIDIStatus_SetMasterReverbType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterReverbType As Long) As Long

' 現在のマスターコーラスの値を1つ設定
Declare Function MIDIStatus_SetMasterChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum, ByVal lVal) As Long

' マスターコーラスのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。
Declare Function MIDIStatus_SetMasterChorusType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterChorusType As Long) As Long

' 現在のマスターディレイの値を1つ設定(XGではディレイはヴァリエーションと読み替える)
Declare Function MIDIStatus_SetMasterDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' マスターディレイのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。
' (XGではマスターヴァリエーションとなる)
Declare Function MIDIStatus_SetMasterDelayType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterDelayType As Long) As Long

' 現在のマスターイコライザの値を1つ設定
Declare Function MIDIStatus_SetMasterEqualizer Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' マスターイコライザのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。
Declare Function MIDIStatus_SetMasterEqualizerType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterEqualizerType As Long) As Long

' MIDIStatus_Put系関数

' リセット
Declare Function MIDIStatus_PutReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' GMリセット
Declare Function MIDIStatus_PutGMReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' GM2リセット
Declare Function MIDIStatus_PutGM2Reset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' GSリセット
Declare Function MIDIStatus_PutGSReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 88リセット
Declare Function MIDIStatus_Put88Reset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' XGリセット
Declare Function MIDIStatus_PutXGReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' MIDIメッセージを与えることによりパラメータを変更する。
Declare Function MIDIStatus_PutMIDIMessage Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long



' MIDIStatusの書き込み(外部隠蔽、この関数はMIDIStatus_Saveから呼び出される)
Declare Function MIDIStatus_Write Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long

' MIDIStatusの保存
Declare Function MIDIStatus_Save Lib "MIDIStatus.dll" Alias "MIDIStatus_SaveA" _
    (ByVal pMIDIStatus As Long, ByVal pszFileName As String) As Long

' MIDIStatusの読み込み(外部隠蔽、この関数はMIDIStatus_Loadから呼び出される)
Declare Function MIDIStatus_Read Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal pFile, ByVal lVersion As Long) As Long

' MIDIStatusのロード
Declare Function MIDIStatus_Load Lib "MIDIStatus.dll" Alias "MIDIStatus_LoadA" _
    (ByVal pszFileName As String) As Long





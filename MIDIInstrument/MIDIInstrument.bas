'****************************************************************************
'
'　MIDIInstrument.bas - VB4,5,6用      (C)2007-2012 くず
'
'****************************************************************************

' このモジュールは普通のC言語で書かれている。
' このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布される。
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

'****************************************************************************
'
'　MIDIPatchNameTableクラス関数群
'
'****************************************************************************

' MIDIPatchNameTableオブジェクトの生成
Declare Function MIDIPatchNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIPatchNameTableオブジェクトの削除
Declare Sub MIDIPatchNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long)

' MIDIPatchNameTableオブジェクトのタイトルを設定
Declare Function MIDIPatchNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_SetTitleA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal pszTitle As String) As Long

' MIDIPatchNameTableオブジェクトの各名前を設定
Declare Function MIDIPatchNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_SetNameA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDIPatchNameTableオブジェクトのタイトルを取得
Declare Function MIDIPatchNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_GetTitleA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIPatchNameTableオブジェクトの各名前を取得
Declare Function MIDIPatchNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_GetNameA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' 次のMIDIPatchNameTableへのポインタを取得(なければNULL)
Declare Function MIDIPatchNameTable_GetNextPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long) As Long

' 前のMIDIPatchNameTableへのポインタを取得(なければNULL)
Declare Function MIDIPatchNameTable_GetPrevPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long) As Long

' 親のMIDIInstrumentへのポインタを取得(なければNULL)
Declare Function MIDIPatchNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long) As Long

'****************************************************************************
'
'　MIDINoteNameTableクラス関数群
'
'****************************************************************************

' MIDINoteNameTableオブジェクトの生成
Declare Function MIDINoteNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDINoteNameTableオブジェクトの削除
Declare Sub MIDINoteNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long)

' MIDINoteNameTableオブジェクトのタイトルを設定
Declare Function MIDINoteNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_SetTitleA" _
    (ByVal pMIDINoteNameTable As Long, ByVal pszTitle As String) As Long

' MIDINoteNameTableオブジェクトの各名前を設定
Declare Function MIDINoteNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_SetNameA" _
    (ByVal pMIDINoteNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDINoteNameTableオブジェクトのタイトルを取得
Declare Function MIDINoteNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_GetTitleA" _
    (ByVal pMIDINoteNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDINoteNameTableオブジェクトの各名前を取得
Declare Function MIDINoteNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_GetNameA" _
    (ByVal pMIDINoteNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' 次のMIDINoteNameTableへのポインタを取得(なければNULL)
Declare Function MIDINoteNameTable_GetNextNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long) As Long

' 前のMIDINoteNameTableへのポインタを取得(なければNULL)
Declare Function MIDINoteNameTable_GetPrevNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long) As Long

' 親のMIDIInstrumentへのポインタを取得(なければNULL)
Declare Function MIDINoteNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long) As Long

'****************************************************************************
'
'　MIDIControllerNameTableクラス関数群
'
'****************************************************************************

' MIDIControllerNameTableオブジェクトの生成
Declare Function MIDIControllerNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIControllerNameTableオブジェクトの削除
Declare Sub MIDIControllerNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long)

' MIDIControllerNameTableオブジェクトのタイトルを設定
Declare Function MIDIControllerNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_SetTitleA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal pszTitle As String) As Long

' MIDIControllerNameTableオブジェクトの各名前を設定
Declare Function MIDIControllerNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_SetNameA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDIControllerNameTableオブジェクトのタイトルを取得
Declare Function MIDIControllerNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_GetTitleA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIControllerNameTableオブジェクトの各名前を取得
Declare Function MIDIControllerNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_GetNameA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' 次のMIDIControllerNameTableへのポインタを取得(なければNULL)
Declare Function MIDIControllerNameTable_GetNextControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long) As Long

' 前のMIDIControllerNameTableへのポインタを取得(なければNULL)
Declare Function MIDIControllerNameTable_GetPrevControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long) As Long

' 親のMIDIInstrumentへのポインタを取得(なければNULL)
Declare Function MIDIControllerNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long) As Long

'****************************************************************************
'
'　MIDIRPNNameTableクラス関数群
'
'****************************************************************************

' MIDIRPNNameTableオブジェクトの生成
Declare Function MIDIRPNNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIRPNNameTableオブジェクトの削除
Declare Sub MIDIRPNNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long)

' MIDIRPNNameTableオブジェクトのタイトルを設定
Declare Function MIDIRPNNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_SetTitleA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal pszTitle As String) As Long

' MIDIRPNNameTableオブジェクトの各名前を設定
Declare Function MIDIRPNNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_SetNameA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDIRPNNameTableオブジェクトのタイトルを取得
Declare Function MIDIRPNNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_GetTitleA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIRPNNameTableオブジェクトの各名前を取得
Declare Function MIDIRPNNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_GetNameA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' 次のMIDIRPNNameTableへのポインタを取得(なければNULL)
Declare Function MIDIRPNNameTable_GetNextRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long) As Long

' 前のMIDIRPNNameTableへのポインタを取得(なければNULL)
Declare Function MIDIRPNNameTable_GetPrevRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long) As Long

' 親のMIDIInstrumentへのポインタを取得(なければNULL)
Declare Function MIDIRPNNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long) As Long

'****************************************************************************
'
'　MIDINRPNNameTableクラス関数群
'
'****************************************************************************

' MIDINRPNNameTableオブジェクトの生成
Declare Function MIDINRPNNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDINRPNNameTableオブジェクトの削除
Declare Sub MIDINRPNNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long)

' MIDINRPNNameTableオブジェクトのタイトルを設定
Declare Function MIDINRPNNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_SetTitleA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal pszTitle As String) As Long

' MIDIRPNNameTableオブジェクトの各名前を設定
Declare Function MIDINRPNNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_SetNameA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDINRPNNameTableオブジェクトのタイトルを取得
Declare Function MIDINRPNNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_GetTitleA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIRPNNameTableオブジェクトの各名前を取得
Declare Function MIDINRPNNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_GetNameA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' 次のMIDINRPNNameTableへのポインタを取得(なければNULL)
Declare Function MIDINRPNNameTable_GetNextNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long) As Long

' 前のMIDINRPNNameTableへのポインタを取得(なければNULL)
Declare Function MIDINRPNNameTable_GetPrevNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long) As Long

' 親のMIDIInstrumentへのポインタを取得(なければNULL)
Declare Function MIDINRPNNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long) As Long


'****************************************************************************
'
'　MIDIInstrumentDefinitionクラス関数群
'
'****************************************************************************

' MIDIInstrumentDefinitionオブジェクトの生成
Declare Function MIDIInstrumentDefinition_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIInstrumentDefinitionオブジェクトの削除
Declare Sub MIDIInstrumentDefinition_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long)

' MIDIInstrumentDefinitionオブジェクトのタイトルを設定
Declare Function MIDIInstrumentDefinition_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrumentDefinition_SetTitleA" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pszTitle As String) As Long

' MIDIInstrumentDefinitionオブジェクトのコントローラー名テーブルを設定
Declare Function MIDIInstrumentDefinition_SetControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pMIDIControllerNameTable As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのRPN名テーブルを設定
Declare Function MIDIInstrumentDefinition_SetRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのNRPN名テーブルを設定
Declare Function MIDIInstrumentDefinition_SetNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pMIDINRPNNameTable As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのパッチ名テーブルを設定
Declare Function MIDIInstrumentDefinition_SetPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long, _
    ByVal pMIDIPatchNameTable As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのノート名テーブルを設定
Declare Function MIDIInstrumentDefinition_SetNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long, ByVal j As Long, _
    ByVal pMIDINoteNameTable As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのタイトルを取得
Declare Function MIDIInstrumentDefinition_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrumentDefinition_GetTitleA" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのコントローラー名テーブルを取得
Declare Function MIDIInstrumentDefinition_GetControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのRPN名テーブルを取得
Declare Function MIDIInstrumentDefinition_GetRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのNRPN名テーブルを取得
Declare Function MIDIInstrumentDefinition_GetNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのパッチ名テーブルを取得
Declare Function MIDIInstrumentDefinition_GetPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long) As Long

' MIDIInstrumentDefinitionオブジェクトのノート名テーブルを取得
Declare Function MIDIInstrumentDefinition_GetNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long, ByVal j As Long) As Long
 
' 次のMIDIInstrumentDefinitionへのポインタを取得(なければNULL)
Declare Function MIDIInstrumentDefinition_GetNextInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' 前のMIDIInstrumentDefinitionへのポインタを取得(なければNULL)
Declare Function MIDIInstrumentDefinition_GetPrevInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' 親のMIDIInstrumentへのポインタを取得(なければNULL)
Declare Function MIDIInstrumentDefinition_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long


'*****************************************************************************
'
'　MIDIInstrumentクラス関数群
'
'*****************************************************************************

' MIDIInstrumentオブジェクトの生成
Declare Function MIDIInstrument_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIInstrumentオブジェクトの削除
Declare Sub MIDIInstrument_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long)

' MIDIInstrumentにMIDIPatchNameTableを挿入(挿入位置は指定ターゲットの直後)
Declare Function MIDIInstrument_InsertPatchNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDIPatchNameTable As Long, ByVal pTargetPatchNameTable As Long) As Long

' MIDIInstrumentにMIDIPatchNameTableを追加(最後に挿入)
Declare Function MIDIInstrument_AddPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIPatchNameTable As Long) As Long

' MIDIInstrumentからMIDIPatchNameTableを除去(MIDIPatchNameTable自体は削除しない)
Declare Function MIDIInstrument_RemovePatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIPatchNameTable As Long) As Long

' MIDIInstrumentにMIDINoteNameTableを挿入(挿入位置は指定ターゲットの直後)
Declare Function MIDIInstrument_InsertNoteNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDINoteNameTable As Long, ByVal pTargetNoteNameTable As Long) As Long

' MIDIInstrumentにMIDINoteNameTableを追加(最後に挿入)
Declare Function MIDIInstrument_AddNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINoteNameTable As Long) As Long

' MIDIInstrumentからMIDINoteNameTableを除去(MIDINoteNameTable自体は削除しない)
Declare Function MIDIInstrument_RemoveNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINoteNameTable As Long) As Long

' MIDIInstrumentにMIDIControllerNameTableを挿入(挿入位置は指定ターゲットの直後)
Declare Function MIDIInstrument_InsertControllerNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDIControllerNameTable As Long, ByVal pTargetControllerNameTable As Long) As Long

' MIDIInstrumentにMIDIControllerNameTableを追加(最後に挿入)
Declare Function MIDIInstrument_AddControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIControllerNameTable As Long) As Long

' MIDIInstrumentからMIDIControllerNameTableを除去(MIDIControllerNameTable自体は削除しない)
Declare Function MIDIInstrument_RemoveControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIControllerNameTable As Long) As Long

' MIDIInstrumentにMIDIRPNNameTableを挿入(挿入位置は指定ターゲットの直後)
Declare Function MIDIInstrument_InsertRPNNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDIRPNNameTable As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrumentにMIDIRPNNameTableを追加(最後に挿入)
Declare Function MIDIInstrument_AddRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrumentからMIDIRPNNameTableを除去(MIDIRPNNameTable自体は削除しない)
Declare Function MIDIInstrument_RemoveRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrumentにMIDINRPNNameTableを挿入(挿入位置は指定ターゲットの直後)
Declare Function MIDIInstrument_InsertNRPNNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
     ByVal pMIDINRPNNameTable As Long, ByVal pTargetNRPNNameTable As Long) As Long

' MIDIInstrumentにMIDINRPNNameTableを追加(最後に挿入)
Declare Function MIDIInstrument_AddNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINRPNNameTable As Long) As Long

' MIDIInstrumentからMIDINRPNNameTableを除去(MIDINRPNNameTable自体は削除しない)
Declare Function MIDIInstrument_RemoveNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINRPNNameTable As Long) As Long

' MIDIInstrumentにMIDIInstrumentDefinitionを挿入(挿入位置は指定ターゲットの直後)
Declare Function MIDIInstrument_InsertInstrumentDefinitionAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
     ByVal pMIDIInstrumentDefinition As Long, ByVal pTargetInstrumentDefinition As Long) As Long

' MIDIInstrumentにMIDIInstrumentDefinitionを追加(最後に挿入)
Declare Function MIDIInstrument_AddInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrumentからMIDIInstrumentDefinitionを除去(MIDIInstrumentDefinition自体は削除しない)
Declare Function MIDIInstrument_RemoveInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIInstrumentDefinition As Long) As Long

' 最初のパッチ名テーブルへのポインタを返す(なければNULL) */
Declare Function MIDIInstrument_GetFirstPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最後のパッチ名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetLastPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最初のノート名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetFirstNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最後のノート名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetLastNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最初のコントローラ名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetFirstControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最後のコントローラ名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetLastControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最初のRPN名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetFirstRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最後のRPN名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetLastRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最初のNRPN名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetFirstNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最後のNRPN名テーブルへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetLastNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最初のインストゥルメント定義へのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetFirstInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' 最後のインストゥルメント定義へのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetLastInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' タイトルからMIDIPatchNameTableへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetPatchNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetPatchNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' タイトルからMIDINoteNameTableへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetNoteNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetNoteNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' タイトルからMIDIControllerNameTableへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetControllerNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetControllerNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' タイトルからMIDIRPNNameTableへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetRPNNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetRPNNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' タイトルからMIDINRPNNameTableへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetNRPNNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetNRPNNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' タイトルからMIDIInstrumentDefinitionへのポインタを返す(なければNULL)
Declare Function MIDIInstrument_GetInstrumentDefinitionFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetInstrumentDefinitionFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを読み込み
Declare Function MIDIInstrument_Load Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_LoadA" _
    (ByVal pszFileName As String) As Long

' Cakewalkのインストゥルメント定義ファイル(*.ins)からMIDIインストゥルメントを追加読み込み
Declare Function MIDIInstrument_LoadAdditional Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_LoadAdditionalA" _
    (ByVal pMIDIInstrument As Long, ByVal pszFileName As String) As Long


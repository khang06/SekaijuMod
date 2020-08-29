'****************************************************************************
'
'�@MIDIInstrument.bas - VB4,5,6�p      (C)2007-2012 ����
'
'****************************************************************************

' ���̃��W���[���͕��ʂ�C����ŏ�����Ă���B
' ���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��z�z�����B
' �v���W�F�N�g�z�[���y�[�W�F"http://openmidiproject.sourceforge.jp/index.html"

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
'�@MIDIPatchNameTable�N���X�֐��Q
'
'****************************************************************************

' MIDIPatchNameTable�I�u�W�F�N�g�̐���
Declare Function MIDIPatchNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIPatchNameTable�I�u�W�F�N�g�̍폜
Declare Sub MIDIPatchNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long)

' MIDIPatchNameTable�I�u�W�F�N�g�̃^�C�g����ݒ�
Declare Function MIDIPatchNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_SetTitleA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal pszTitle As String) As Long

' MIDIPatchNameTable�I�u�W�F�N�g�̊e���O��ݒ�
Declare Function MIDIPatchNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_SetNameA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDIPatchNameTable�I�u�W�F�N�g�̃^�C�g�����擾
Declare Function MIDIPatchNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_GetTitleA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIPatchNameTable�I�u�W�F�N�g�̊e���O���擾
Declare Function MIDIPatchNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDIPatchNameTable_GetNameA" _
    (ByVal pMIDIPatchNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' ����MIDIPatchNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIPatchNameTable_GetNextPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long) As Long

' �O��MIDIPatchNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIPatchNameTable_GetPrevPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long) As Long

' �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIPatchNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIPatchNameTable As Long) As Long

'****************************************************************************
'
'�@MIDINoteNameTable�N���X�֐��Q
'
'****************************************************************************

' MIDINoteNameTable�I�u�W�F�N�g�̐���
Declare Function MIDINoteNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDINoteNameTable�I�u�W�F�N�g�̍폜
Declare Sub MIDINoteNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long)

' MIDINoteNameTable�I�u�W�F�N�g�̃^�C�g����ݒ�
Declare Function MIDINoteNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_SetTitleA" _
    (ByVal pMIDINoteNameTable As Long, ByVal pszTitle As String) As Long

' MIDINoteNameTable�I�u�W�F�N�g�̊e���O��ݒ�
Declare Function MIDINoteNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_SetNameA" _
    (ByVal pMIDINoteNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDINoteNameTable�I�u�W�F�N�g�̃^�C�g�����擾
Declare Function MIDINoteNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_GetTitleA" _
    (ByVal pMIDINoteNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDINoteNameTable�I�u�W�F�N�g�̊e���O���擾
Declare Function MIDINoteNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDINoteNameTable_GetNameA" _
    (ByVal pMIDINoteNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' ����MIDINoteNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDINoteNameTable_GetNextNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long) As Long

' �O��MIDINoteNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDINoteNameTable_GetPrevNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long) As Long

' �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDINoteNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDINoteNameTable As Long) As Long

'****************************************************************************
'
'�@MIDIControllerNameTable�N���X�֐��Q
'
'****************************************************************************

' MIDIControllerNameTable�I�u�W�F�N�g�̐���
Declare Function MIDIControllerNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIControllerNameTable�I�u�W�F�N�g�̍폜
Declare Sub MIDIControllerNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long)

' MIDIControllerNameTable�I�u�W�F�N�g�̃^�C�g����ݒ�
Declare Function MIDIControllerNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_SetTitleA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal pszTitle As String) As Long

' MIDIControllerNameTable�I�u�W�F�N�g�̊e���O��ݒ�
Declare Function MIDIControllerNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_SetNameA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDIControllerNameTable�I�u�W�F�N�g�̃^�C�g�����擾
Declare Function MIDIControllerNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_GetTitleA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIControllerNameTable�I�u�W�F�N�g�̊e���O���擾
Declare Function MIDIControllerNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDIControllerNameTable_GetNameA" _
    (ByVal pMIDIControllerNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' ����MIDIControllerNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIControllerNameTable_GetNextControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long) As Long

' �O��MIDIControllerNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIControllerNameTable_GetPrevControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long) As Long

' �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIControllerNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIControllerNameTable As Long) As Long

'****************************************************************************
'
'�@MIDIRPNNameTable�N���X�֐��Q
'
'****************************************************************************

' MIDIRPNNameTable�I�u�W�F�N�g�̐���
Declare Function MIDIRPNNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIRPNNameTable�I�u�W�F�N�g�̍폜
Declare Sub MIDIRPNNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long)

' MIDIRPNNameTable�I�u�W�F�N�g�̃^�C�g����ݒ�
Declare Function MIDIRPNNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_SetTitleA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal pszTitle As String) As Long

' MIDIRPNNameTable�I�u�W�F�N�g�̊e���O��ݒ�
Declare Function MIDIRPNNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_SetNameA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDIRPNNameTable�I�u�W�F�N�g�̃^�C�g�����擾
Declare Function MIDIRPNNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_GetTitleA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIRPNNameTable�I�u�W�F�N�g�̊e���O���擾
Declare Function MIDIRPNNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDIRPNNameTable_GetNameA" _
    (ByVal pMIDIRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' ����MIDIRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIRPNNameTable_GetNextRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long) As Long

' �O��MIDIRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIRPNNameTable_GetPrevRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long) As Long

' �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIRPNNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIRPNNameTable As Long) As Long

'****************************************************************************
'
'�@MIDINRPNNameTable�N���X�֐��Q
'
'****************************************************************************

' MIDINRPNNameTable�I�u�W�F�N�g�̐���
Declare Function MIDINRPNNameTable_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDINRPNNameTable�I�u�W�F�N�g�̍폜
Declare Sub MIDINRPNNameTable_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long)

' MIDINRPNNameTable�I�u�W�F�N�g�̃^�C�g����ݒ�
Declare Function MIDINRPNNameTable_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_SetTitleA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal pszTitle As String) As Long

' MIDIRPNNameTable�I�u�W�F�N�g�̊e���O��ݒ�
Declare Function MIDINRPNNameTable_SetName Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_SetNameA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String) As Long

' MIDINRPNNameTable�I�u�W�F�N�g�̃^�C�g�����擾
Declare Function MIDINRPNNameTable_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_GetTitleA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIRPNNameTable�I�u�W�F�N�g�̊e���O���擾
Declare Function MIDINRPNNameTable_GetName Lib "MIDIInstrument.dll" _
    Alias "MIDINRPNNameTable_GetNameA" _
    (ByVal pMIDINRPNNameTable As Long, ByVal lNumber As Long, _
    ByVal pszName As String, ByVal lLen As Long) As Long
 
' ����MIDINRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDINRPNNameTable_GetNextNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long) As Long

' �O��MIDINRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDINRPNNameTable_GetPrevNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long) As Long

' �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDINRPNNameTable_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDINRPNNameTable As Long) As Long


'****************************************************************************
'
'�@MIDIInstrumentDefinition�N���X�֐��Q
'
'****************************************************************************

' MIDIInstrumentDefinition�I�u�W�F�N�g�̐���
Declare Function MIDIInstrumentDefinition_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̍폜
Declare Sub MIDIInstrumentDefinition_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long)

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃^�C�g����ݒ�
Declare Function MIDIInstrumentDefinition_SetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrumentDefinition_SetTitleA" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pszTitle As String) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃R���g���[���[���e�[�u����ݒ�
Declare Function MIDIInstrumentDefinition_SetControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pMIDIControllerNameTable As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g��RPN���e�[�u����ݒ�
Declare Function MIDIInstrumentDefinition_SetRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g��NRPN���e�[�u����ݒ�
Declare Function MIDIInstrumentDefinition_SetNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pMIDINRPNNameTable As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃p�b�`���e�[�u����ݒ�
Declare Function MIDIInstrumentDefinition_SetPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long, _
    ByVal pMIDIPatchNameTable As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃m�[�g���e�[�u����ݒ�
Declare Function MIDIInstrumentDefinition_SetNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long, ByVal j As Long, _
    ByVal pMIDINoteNameTable As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃^�C�g�����擾
Declare Function MIDIInstrumentDefinition_GetTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrumentDefinition_GetTitleA" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal pszTitle As String, _
    ByVal lLen As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃R���g���[���[���e�[�u�����擾
Declare Function MIDIInstrumentDefinition_GetControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g��RPN���e�[�u�����擾
Declare Function MIDIInstrumentDefinition_GetRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g��NRPN���e�[�u�����擾
Declare Function MIDIInstrumentDefinition_GetNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃p�b�`���e�[�u�����擾
Declare Function MIDIInstrumentDefinition_GetPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long) As Long

' MIDIInstrumentDefinition�I�u�W�F�N�g�̃m�[�g���e�[�u�����擾
Declare Function MIDIInstrumentDefinition_GetNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long, ByVal i As Long, ByVal j As Long) As Long
 
' ����MIDIInstrumentDefinition�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIInstrumentDefinition_GetNextInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' �O��MIDIInstrumentDefinition�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIInstrumentDefinition_GetPrevInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long

' �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIInstrumentDefinition_GetParent Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrumentDefinition As Long) As Long


'*****************************************************************************
'
'�@MIDIInstrument�N���X�֐��Q
'
'*****************************************************************************

' MIDIInstrument�I�u�W�F�N�g�̐���
Declare Function MIDIInstrument_Create Lib "MIDIInstrument.dll" _
    () As Long

' MIDIInstrument�I�u�W�F�N�g�̍폜
Declare Sub MIDIInstrument_Delete Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long)

' MIDIInstrument��MIDIPatchNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���)
Declare Function MIDIInstrument_InsertPatchNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDIPatchNameTable As Long, ByVal pTargetPatchNameTable As Long) As Long

' MIDIInstrument��MIDIPatchNameTable��ǉ�(�Ō�ɑ}��)
Declare Function MIDIInstrument_AddPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIPatchNameTable As Long) As Long

' MIDIInstrument����MIDIPatchNameTable������(MIDIPatchNameTable���͍̂폜���Ȃ�)
Declare Function MIDIInstrument_RemovePatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIPatchNameTable As Long) As Long

' MIDIInstrument��MIDINoteNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���)
Declare Function MIDIInstrument_InsertNoteNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDINoteNameTable As Long, ByVal pTargetNoteNameTable As Long) As Long

' MIDIInstrument��MIDINoteNameTable��ǉ�(�Ō�ɑ}��)
Declare Function MIDIInstrument_AddNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINoteNameTable As Long) As Long

' MIDIInstrument����MIDINoteNameTable������(MIDINoteNameTable���͍̂폜���Ȃ�)
Declare Function MIDIInstrument_RemoveNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINoteNameTable As Long) As Long

' MIDIInstrument��MIDIControllerNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���)
Declare Function MIDIInstrument_InsertControllerNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDIControllerNameTable As Long, ByVal pTargetControllerNameTable As Long) As Long

' MIDIInstrument��MIDIControllerNameTable��ǉ�(�Ō�ɑ}��)
Declare Function MIDIInstrument_AddControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIControllerNameTable As Long) As Long

' MIDIInstrument����MIDIControllerNameTable������(MIDIControllerNameTable���͍̂폜���Ȃ�)
Declare Function MIDIInstrument_RemoveControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIControllerNameTable As Long) As Long

' MIDIInstrument��MIDIRPNNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���)
Declare Function MIDIInstrument_InsertRPNNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
    ByVal pMIDIRPNNameTable As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrument��MIDIRPNNameTable��ǉ�(�Ō�ɑ}��)
Declare Function MIDIInstrument_AddRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrument����MIDIRPNNameTable������(MIDIRPNNameTable���͍̂폜���Ȃ�)
Declare Function MIDIInstrument_RemoveRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIRPNNameTable As Long) As Long

' MIDIInstrument��MIDINRPNNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���)
Declare Function MIDIInstrument_InsertNRPNNameTableAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
     ByVal pMIDINRPNNameTable As Long, ByVal pTargetNRPNNameTable As Long) As Long

' MIDIInstrument��MIDINRPNNameTable��ǉ�(�Ō�ɑ}��)
Declare Function MIDIInstrument_AddNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINRPNNameTable As Long) As Long

' MIDIInstrument����MIDINRPNNameTable������(MIDINRPNNameTable���͍̂폜���Ȃ�)
Declare Function MIDIInstrument_RemoveNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDINRPNNameTable As Long) As Long

' MIDIInstrument��MIDIInstrumentDefinition��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���)
Declare Function MIDIInstrument_InsertInstrumentDefinitionAfter Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, _
     ByVal pMIDIInstrumentDefinition As Long, ByVal pTargetInstrumentDefinition As Long) As Long

' MIDIInstrument��MIDIInstrumentDefinition��ǉ�(�Ō�ɑ}��)
Declare Function MIDIInstrument_AddInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIInstrumentDefinition As Long) As Long

' MIDIInstrument����MIDIInstrumentDefinition������(MIDIInstrumentDefinition���͍̂폜���Ȃ�)
Declare Function MIDIInstrument_RemoveInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long, ByVal pMIDIInstrumentDefinition As Long) As Long

' �ŏ��̃p�b�`���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
Declare Function MIDIInstrument_GetFirstPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �Ō�̃p�b�`���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetLastPatchNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �ŏ��̃m�[�g���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetFirstNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �Ō�̃m�[�g���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetLastNoteNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �ŏ��̃R���g���[�����e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetFirstControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �Ō�̃R���g���[�����e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetLastControllerNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �ŏ���RPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetFirstRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �Ō��RPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetLastRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �ŏ���NRPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetFirstNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �Ō��NRPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetLastNRPNNameTable Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �ŏ��̃C���X�g�D�������g��`�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetFirstInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �Ō�̃C���X�g�D�������g��`�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetLastInstrumentDefinition Lib "MIDIInstrument.dll" _
    (ByVal pMIDIInstrument As Long) As Long

' �^�C�g������MIDIPatchNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetPatchNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetPatchNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' �^�C�g������MIDINoteNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetNoteNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetNoteNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' �^�C�g������MIDIControllerNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetControllerNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetControllerNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' �^�C�g������MIDIRPNNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetRPNNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetRPNNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' �^�C�g������MIDINRPNNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetNRPNNameTableFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetNRPNNameTableFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' �^�C�g������MIDIInstrumentDefinition�ւ̃|�C���^��Ԃ�(�Ȃ����NULL)
Declare Function MIDIInstrument_GetInstrumentDefinitionFromTitle Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_GetInstrumentDefinitionFromTitleA" _
    (ByVal pMIDIInstrument As Long, ByVal pszTitle As String) As Long

' Cakewalk�̃C���X�g�D�������g��`�t�@�C��(*.ins)����MIDI�C���X�g�D�������g��ǂݍ���
Declare Function MIDIInstrument_Load Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_LoadA" _
    (ByVal pszFileName As String) As Long

' Cakewalk�̃C���X�g�D�������g��`�t�@�C��(*.ins)����MIDI�C���X�g�D�������g��ǉ��ǂݍ���
Declare Function MIDIInstrument_LoadAdditional Lib "MIDIInstrument.dll" _
    Alias "MIDIInstrument_LoadAdditionalA" _
    (ByVal pMIDIInstrument As Long, ByVal pszFileName As String) As Long


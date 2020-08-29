'******************************************************************************
'*                                                                            *
'*�@MIDIIO.bas - MIDIIO���W���[��(VB4,5,6�p)               (C)2002-2012 ����  *
'*                                                                            *
'******************************************************************************

' MIDI���̓I�u�W�F�N�g�̃I�[�v���E�N���[�Y�E���Z�b�g�E�f�[�^��M(SYSX�܂�)
' MIDI�o�̓I�u�W�F�N�g�̃I�[�v���E�N���[�Y�E���Z�b�g�E�f�[�^���M(SYSX�܂�)
' ���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��z�z����܂��B
' �v���W�F�N�g�z�[���y�[�W�F"http://openmidiproject.sourceforge.jp/index.html"

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


' MIDI�o�̓f�o�C�X�̐��𒲂ׂ�
Declare Function MIDIOut_GetDeviceNum Lib "MIDIIO.dll" () As Long

' MIDI�o�̓f�o�C�X�̖��O�𒲂ׂ�
Declare Function MIDIOut_GetDeviceName Lib "MIDIIO.dll" Alias "MIDIOut_GetDeviceNameA" _
    (ByVal lID As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long

' MIDI�o�̓f�o�C�X���J��
Declare Function MIDIOut_Open Lib "MIDIIO.dll" Alias "MIDIOut_OpenA" _
    (ByVal pszDeviceName As String) As Long

' MIDI�o�̓f�o�C�X�����
Declare Function MIDIOut_Close Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long) As Long

' MIDI�o�̓f�o�C�X���ĂъJ��
Declare Function MIDIOut_Reopen Lib "MIDIIO.dll" Alias "MIDIOut_ReopenA" _
    (ByVal pMIDIOut As Long, ByVal pszDeviceName As String) As Long

' MIDI�o�̓f�o�C�X�����Z�b�g����
Declare Function MIDIOut_Reset Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long) As Long

' MIDI�o�̓f�o�C�X�Ƀ��b�Z�[�W��1�o�͂���
Declare Function MIDIOut_PutMIDIMessage Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long

' MIDI�o�̓f�o�C�X��1�o�C�g�o�͂���
Declare Function MIDIOut_PutByte Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long, ByVal bByte As Byte) As Long

' MIDI�o�̓f�o�C�X�ɔC�Ӓ��̃o�C�i���f�[�^���o�͂���
Declare Function MIDIOut_PutBytes Lib "MIDIIO.dll" _
    (ByVal pMIDIOut As Long, ByRef pData As Byte, ByVal lLen As Long) As Long

' ����MIDI�o�̓f�o�C�X�̖��O���擾����(20120415�ǉ�)
Declare Function MIDIOut_GetThisDeviceName Lib "MIDIIO.dll" Alias "MIDIOut_GetThisDeviceNameA" _
    (ByVal pMIDIOut As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long



' MIDI���̓f�o�C�X�̐��𒲂ׂ�
Declare Function MIDIIn_GetDeviceNum Lib "MIDIIO.dll" () As Long

' MIDI���̓f�o�C�X�̖��O�𒲂ׂ�
Declare Function MIDIIn_GetDeviceName Lib "MIDIIO.dll" Alias "MIDIIn_GetDeviceNameA" _
    (ByVal lID As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long

' MIDI���̓f�o�C�X���J��
Declare Function MIDIIn_Open Lib "MIDIIO.dll" Alias "MIDIIn_OpenA" _
    (ByVal pDeviceName As String) As Long

' MIDI���̓f�o�C�X�����
Declare Function MIDIIn_Close Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long) As Long

' MIDI���̓f�o�C�X���ĂъJ��
Declare Function MIDIIn_Reopen Lib "MIDIIO.dll" Alias "MIDIIn_ReopenA" _
    (ByVal pMIDIIn As Long, ByVal pszDeviceName As String) As Long

' MIDI���̓f�o�C�X�����Z�b�g����
Declare Function MIDIIn_Reset Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long) As Long

' MIDI���̓f�o�C�X���烁�b�Z�[�W��1���͂���
Declare Function MIDIIn_GetMIDIMessage Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long

' MIDI���̓f�o�C�X����1�o�C�g���͂���
Declare Function MIDIIn_GetByte Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long, ByRef pByte As Byte) As Long

' MIDI���̓f�o�C�X����C�Ӓ��̃o�C�i���f�[�^����͂���
Declare Function MIDIIn_GetBytes Lib "MIDIIO.dll" _
    (ByVal pMIDIIn As Long, ByRef pData As Byte, ByVal lLen As Long) As Long

' ����MIDI���̓f�o�C�X�̖��O���擾����(20120415�ǉ�)
Declare Function MIDIIn_GetThisDeviceName Lib "MIDIIO.dll" Alias "MIDIIn_GetThisDeviceNameA" _
    (ByVal pMIDIIn As Long, ByVal pszDeviceName As String, ByVal lLen As Long) As Long


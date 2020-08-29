'******************************************************************************
'*                                                                            *
'* MIDIClock.bas - MIDIClock���W���[��(VB4,5,6�p)           (C)2002-2009 ���� *
'*                                                                            *
'******************************************************************************

' ���̃��W���[���͕��ʂ�C�����Win32API���g���ď�����Ă��܂��B
' MIDI�N���b�N�̃X�^�[�g�E�X�g�b�v�E���Z�b�g
' �e���|�E�^�C���x�[�X�E�o�ߎ����E�o�߃e�B�b�N���̎擾�E�ݒ�
' �}�X�^�[���[�h(TPQNBASE�ASMPTE24BASE�ASMPTE25BASE�ASMPTE29BASE�ASMPTE30BASE)�ɑΉ�
' �X���[�u���[�h(MIDI�^�C�~���O�N���b�N�ASMPTE/MTC)�ɑΉ�
' ���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��z�z����܂��B
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

Public Const MIDICLOCK_MAXTEMPO = 60000000
Public Const MIDICLOCK_MINTEMPO = 1
Public Const MIDICLOCK_MINSPEED = 0
Public Const MIDICLOCK_MAXSPEED = 100000
Public Const MIDICLOCK_SPEEDSLOW = 5000
Public Const MIDICLOCK_SPEEDNORMAL = 10000
Public Const MIDICLOCK_SPEEDFAST = 20000

'�^�C�����[�h(�ȉ��̒l�̂ݗ��p�\)
Public Const MIDICLOCK_TPQNBASE = 0  'TPQN�x�[�X
Public Const MIDICLOCK_SMPTE24BASE = 24 'SMPTE24�x�[�X(24�t���[��/�b)
Public Const MIDICLOCK_SMPTE25BASE = 25 'SMPTE25�x�[�X(25�t���[��/�b)
Public Const MIDICLOCK_SMPTE29BASE = 29 'SMPTE29�x�[�X(29.97�t���[��/�b)
Public Const MIDICLOCK_SMPTE30BASE = 30 'SMPTE30�x�[�X(30�t���[��/�b)

'�^�C�����[�h(ver0.6�ȑO�Ƃ̌݊��̂��߂Ɏc���Ă���)
Public Const MIDICLOCK_MASTERTPQNBASE = 0  'TPQN�x�[�X
Public Const MIDICLOCK_MASTERSMPTE24BASE = 24 'SMPTE24�x�[�X(24�t���[��/�b)
Public Const MIDICLOCK_MASTERSMPTE25BASE = 25 'SMPTE25�x�[�X(25�t���[��/�b)
Public Const MIDICLOCK_MASTERSMPTE29BASE = 29 'SMPTE29�x�[�X(29.97�t���[��/�b)
Public Const MIDICLOCK_MASTERSMPTE30BASE = 30 'SMPTE30�x�[�X(30�t���[��/�b)

'���͓������[�h(�ȉ��̒l�̂ݗ��p�\)
Public Const MIDICLOCK_MASTER = 0 '�}�X�^�[
Public Const MIDICLOCK_SLAVEMIDITIMINGCLOCK = 1  '�X���[�u���[�h(MIDI�^�C�~���O�N���b�N�ɒǏ])
Public Const MIDICLOCK_SLAVESMPTEMTC = 2  ' �X���[�u���[�h(SMPTE/MTC�ɒǏ])


' MIDI�N���b�N�I�u�W�F�N�g�̍폜
Declare Sub MIDIClock_Delete Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long)

' MIDI�N���b�N�I�u�W�F�N�g�̐���
Declare Function MIDIClock_Create Lib "MIDIClock.dll" _
        (ByVal lTimeMode As Long, ByVal lResolution As Long, ByVal lTempo As Long) As Long

' �^�C�����[�h�ƕ���\[tick/4������]����[tick/1�t���[��]�̎擾
Declare Function MIDIClock_GetTimeBase Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByRef pTimeMode As Long, ByRef pResolution As Long) As Long

' �^�C�����[�h�ƕ���\[tick/4������]����[tick/1�t���[��]�̐ݒ�
Declare Function MIDIClock_SetTimeBase Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lTimeMode As Long, ByVal lResolution As Long) As Long

' MIDI�e���|[�ʕb/4������]�̎擾
Declare Function MIDIClock_GetTempo Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' MIDI�e���|[�ʕb/4������]�̐ݒ�
Declare Function MIDIClock_SetTempo Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lTempo As Long) As Long

' �X�s�[�h[�~0.01��]�̎擾
Declare Function MIDIClock_GetSpeed Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' �X�s�[�h[�~0.01��]�̐ݒ�
Declare Function MIDIClock_SetSpeed Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lSpeed As Long) As Long

' �������[�h�̎擾
Declare Function MIDIClock_GetMIDIInSyncMode Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' �������[�h�̐ݒ�
Declare Function MIDIClock_SetMIDIInSyncMode Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lMIDIInSyncMode As Long) As Long


' �X�^�[�g
Declare Function MIDIClock_Start Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' �X�g�b�v
Declare Function MIDIClock_Stop Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' ���Z�b�g
Declare Function MIDIClock_Reset Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' ���쒆�����ׂ�
Declare Function MIDIClock_IsRunning Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' �o�ߎ���[�~���b]�̎擾
Declare Function MIDIClock_GetMillisec Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' �o�ߎ���[�~���b]�̐ݒ�
Declare Function MIDIClock_SetMillisec Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lMillisec As Long) As Long

' �o�߃e�B�b�N��[tick]�̎擾
Declare Function MIDIClock_GetTickCount Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' �o�߃e�B�b�N��[tick]�̐ݒ�
Declare Function MIDIClock_SetTickCount Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal lTickCount As Long) As Long


' MIDI�^�C�~���O�N���b�N��F��������(0xF8)
Declare Function MIDIClock_PutMIDITimingClock Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long) As Long

' �V�X�e���G�N�X�N���[�V���ɂ��SMPTE/MTC��F��������
Declare Function MIDIClock_PutSysExSMPTEMTC Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal cHour As Byte, _
    ByVal cMinute As Byte, ByVal cSecond As Byte, ByVal cFrame As Byte) As Long

' MIDI�^�C���R�[�h�N�H�[�^�[�t���[��(0xF1)�ɂ��SMPTE/MTC��F��������
Declare Function MIDIClock_PutSMPTEMTC Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByVal cSMPTEMTC As Byte) As Long

' MIDI�N���b�N��MIDI���b�Z�[�W��F��������
Declare Function MIDIClock_PutMIDIMessage Lib "MIDIClock.dll" _
        (ByVal pMIDIClock As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long


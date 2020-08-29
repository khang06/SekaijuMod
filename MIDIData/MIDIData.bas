'******************************************************************************
'*                                                                            *
'*�@MIDIData.bas - MIDIData���W���[��(VB4,5,6�p)            (C)2002-2015 ���� *
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

'�����̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��z�z����܂��B
'���v���W�F�N�g�z�[���y�[�W�F"http://openmidiproject.sourceforge.jp/index.html"

' ���g�p��(4�������́u�h�v1��������MIDI�f�[�^���쐬)
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

' ���d�v�F�ȉ��̍\����(���[�U�[��`�^)��VB�Œ��ڎg���Ă͂Ȃ�܂���B
' �@�@�@�@MIDIEvent, MIDITrack, MIDIData�́A�K��Long�^�̃|�C���^�Ŏw���Ă��������B

' MIDIEvent�\����(�Q�l)
' �o�����_�u�������N���X�g�\��
' �m�[�h�����͐�Ύ����Ō��肳��܂�
Type MIDIEvent
    m_lTempIndex As Long
    m_lTime As Long                     ' ��Ύ���
    m_lKind As Long                     ' MIDI�C�x���g�̎��(= &H00�`= &HFF)
    m_lLen As Long                      ' MIDI�C�x���g�̃f�[�^����[�o�C�g]
    m_pData As Long                     ' MIDI�C�x���g�̃f�[�^�o�b�t�@�ւ̃|�C���^
    m_lData As Long                     ' MIDI�`�����l���C�x���g�̏ꍇ�̃f�[�^�i�[�ꏊ
    m_pNextEvent As Long                ' ���̃C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pPrevEvent As Long                ' �O�̃C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pNextSameKindEvent As Long        ' ���̓���̃C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pPrevSameKindEvent As Long        ' �O�̓���̃C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pNextCombinedEvent As Long        ' ���̌������Ă���C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pPrevCombinedEvent As Long        ' �O�̌������Ă���C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pParent As Long                   ' �e(MIDITrack�I�u�W�F�N�g)�ւ̃|�C���^
    m_lUser1 As Long                    ' ���[�U�[�p�̈�P(���g�p)
    m_lUser2 As Long                    ' ���[�U�[�p�̈�Q(���g�p)
    m_lUser3 As Long                    ' ���[�U�[�p�̈�R(���g�p)
    m_lUserFlag As Long                 ' ���[�U�[�p�t���O(���g�p)
End Type

' MIDITrack�\����(�Q�l)
' �o���������N���X�g�\��
Type MIDITrack
    m_lTempIndex As Long
    m_lNumEvent As Long                 ' �g���b�N���̃C�x���g��
    m_pFirstEvent As Long               ' �ŏ��̃C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pLastEvent As Long                ' �Ō�̃C�x���g�ւ̃|�C���^(�Ȃ����NULL)
    m_pPrevTrack As Long                ' �O�̃g���b�N�ւ̃|�C���^(�Ȃ����NULL)
    m_pNextTrack As Long                ' ���̃g���b�N�ւ̃|�C���^(�Ȃ����NULL)
    m_pParent As Long                   ' �e(MIDIData�I�u�W�F�N�g)�ւ̃|�C���^
    m_lInputOn As Long                  ' ����(0=OFF,1=ON)
    m_lInputPort As Long                ' ���̓|�[�g(0�`255)
    m_lInputChannel As Long             ' ���̓`�����l��(-1=n/a, 0�`15)
    m_lOutputOn As Long                 ' �o��(0=OFF, 1=ON)
    m_lOutputPort As Long               ' �o�̓|�[�g(0�`255)
    m_lOutputChannel As Long            ' �o�̓`�����l��(-1=n/a, 0�`15)
    m_lTimePlus As Long                 ' �^�C��+
    m_lKeyPlus As Long                  ' �L�[+
    m_lVelPlus As Long                  ' �x���V�e�B+
    m_lViewMode As Long                 ' �\�����[�h(0=�ʏ�,1=�h����)
    m_lForeColor As Long                ' �O�i�F
    m_lBackColor As Long                ' �w�i�F
    m_lUser1 As Long                    ' ���[�U�[�p���R�̈�P(���g�p)
    m_lUser2 As Long                    ' ���[�U�[�p���R�̈�Q(���g�p)
    m_lUser3 As Long                    ' ���[�U�[�p�̈�R(���g�p)
    m_lUserFlag As Long                 ' ���[�U�[�p�t���O(���g�p)
End Type

' MIDIData�\����(�Q�l)
' �o���������N���X�g�\��
Type MIDIData
    m_lFormat As Long                   ' SMF�t�H�[�}�b�g(0/1/2)
    m_lNumTrack As Long                 ' �g���b�N��(1�`65535)
    m_lTimeBase As Long                 ' �^�C���x�[�X(��F120)
    m_pFirstTrack As Long               ' �ŏ��̃g���b�N�ւ̃|�C���^(�Ȃ����NULL)
    m_pLastTrack As Long                ' �Ō�̃g���b�N�ւ̃|�C���^(�Ȃ����NULL)
    m_pNextSeq As Long                  ' ���̃V�[�P���X�ւ̃|�C���^(�Ȃ����NULL)
    m_pPrevSeq As Long                  ' �O�̃V�[�P���X�ւ̃|�C���^(�Ȃ����NULL)
    m_pParent As Long                   ' �e(���NULL)
    m_lUser1 As Long                    ' ���[�U�[�p���R�̈�P(���g�p)
    m_lUser2 As Long                    ' ���[�U�[�p���R�̈�Q(���g�p)
    m_lUser3 As Long                    ' ���[�U�[�p�̈�R(���g�p)
    m_lUserFlag As Long                 ' ���[�U�[�p�t���O(���g�p)
End Type

' ���P�[���Ɋւ���}�N��
Public Const LC_ALL = 0

' ���̑��̃}�N��
Public Const MIDIEVENT_MAXLEN = 65536

' �t�H�[�}�b�g�Ɋւ���}�N��
Public Const MIDIDATA_FORMAT0 = 0
Public Const MIDIDATA_FORMAT1 = 1
Public Const MIDIDATA_FORMAT2 = 2

' �g���b�N���Ɋւ���}�N��
Public Const MIDIDATA_MAXMIDITRACKNUM = 65535

' �e���|�Ɋւ���}�N��
' ���d�v�F�e���|�̒P�ʂ͂��ׂ�[�ʕb/4������]�Ƃ���B
Public Const MIDIEVENT_MINTEMPO = 1
Public Const MIDIEVENT_MAXTEMPO = 60000000
Public Const MIDIEVENT_DEFTEMPO = 60000000 / 120

' �^�C���x�[�X(�^�C�����[�h)�Ɋւ���}�N��
Public Const MIDIDATA_TPQNBASE = 0
Public Const MIDIDATA_SMPTE24BASE = 24 '24�t���[��/�b
Public Const MIDIDATA_SMPTE25BASE = 25 '25�t���[��/�b
Public Const MIDIDATA_SMPTE29BASE = 29 '29.97�t���[��/�b
Public Const MIDIDATA_SMPTE30BASE = 30 '30�t���[��/�b

' �^�C���x�[�X(���]�����[�V����)(����\)�Ɋւ���}�N��
' TPQN�x�[�X�̏ꍇ�F4������������̕���\
' �����ӁF����TPQN�̕���\�́A48,72,96,120,144,168,192,216,240,360,384,480,960�ł���
Public Const MIDIDATA_MINTPQNRESOLUTION = 1
Public Const MIDIDATA_MAXTPQNRESOLUTION = 32767
Public Const MIDIDATA_DEFTPQNRESOLUTION = 120

' SMPTE�x�[�X�̏ꍇ�F1�t���[��������̕���\
' �����ӁF����SMPTE�̕���\�́A10,40,80�Ȃǂ���\�I�ł���B
Public Const MIDIDATA_MINSMPTERESOLUTION = 1
Public Const MIDIDATA_MAXSMPTERESOLUTION = 255
Public Const MIDIDATA_DEFSMPTERESOLUTION = 10

' MIDIEVENT_KIND�}�N�� (�R�����g�̃J�b�R���̓f�[�^���̒���������)
Public Const MIDIEVENT_SEQUENCENUMBER = &H0     ' �V�[�P���X�ԍ�(2�o�C�g)
Public Const MIDIEVENT_TEXTEVENT = &H1          ' �e�L�X�g(�ϒ�������)
Public Const MIDIEVENT_COPYRIGHTNOTICE = &H2    ' ���쌠(�ϒ�������)
Public Const MIDIEVENT_TRACKNAME = &H3          ' �g���b�N��(�ϒ�������)
Public Const MIDIEVENT_INSTRUMENTNAME = &H4     ' �C���X�g�D�������g(�ϒ�������)
Public Const MIDIEVENT_LYRIC = &H5              ' �̎�(�ϒ�������)
Public Const MIDIEVENT_MARKER = &H6             ' �}�[�J�[(�ϒ�������)
Public Const MIDIEVENT_CUEPOINT = &H7           ' �L���[�|�C���g(�ϒ�������)
Public Const MIDIEVENT_PROGRAMNAME = &H8        ' �v���O������(�ϒ�������)
Public Const MIDIEVENT_DEVICENAME = &H9         ' �f�o�C�X��(�ϒ�������)
Public Const MIDIEVENT_CHANNELPREFIX = &H20     ' �`�����l���v���t�B�b�N�X(1�o�C�g)
Public Const MIDIEVENT_PORTPREFIX = &H21        ' �|�[�g�v���t�B�b�N�X(1�o�C�g)
Public Const MIDIEVENT_ENDOFTRACK = &H2F        ' �G���h�I�u�g���b�N(�Ȃ�)
Public Const MIDIEVENT_TEMPO = &H51             ' �e���|(3�o�C�g)
Public Const MIDIEVENT_SMPTEOFFSET = &H54       ' SMPTE�I�t�Z�b�g(5�o�C�g)
Public Const MIDIEVENT_TIMESIGNATURE = &H58     ' ���q�L��(4�o�C�g)
Public Const MIDIEVENT_KEYSIGNATURE = &H59      ' �����L��(2�o�C�g)
Public Const MIDIEVENT_SEQUENCERSPECIFIC = &H79 ' �V�[�P���T�[�Ǝ��̃C�x���g(�ϒ��o�C�i��)
Public Const MIDIEVENT_NOTEOFF = &H80           ' �m�[�g�I�t(3�o�C�g)
Public Const MIDIEVENT_NOTEON = &H90            ' �m�[�g�I��(3�o�C�g)
Public Const MIDIEVENT_KEYAFTERTOUCH = &HA0     ' �L�[�A�t�^�[(3�o�C�g)
Public Const MIDIEVENT_CONTROLCHANGE = &HB0     ' �R���g���[���[(3�o�C�g)
Public Const MIDIEVENT_PROGRAMCHANGE = &HC0     ' �p�b�`�`�F���W(2�o�C�g)
Public Const MIDIEVENT_CHANNELAFTERTOUCH = &HD0 ' �`�����l���A�t�^�[(2�o�C�g)
Public Const MIDIEVENT_PITCHBEND = &HE0         ' �s�b�`�x���h(3�o�C�g)
Public Const MIDIEVENT_SYSEXSTART = &HF0        ' �V�X�e���G�N�X�N���[�V���J�n(�ϒ��o�C�i��)
Public Const MIDIEVENT_SYSEXCONTINUE = &HF0     ' �V�X�e���G�N�X�N���[�V���̑���(�ϒ��o�C�i��)

'*********************************************************************/
'*                                                                   */
'*�@MIDIDataLib�N���X�֐�                                            */
'*                                                                   */
'*********************************************************************/

Declare Function MIDIDataLib_SetDefaultCharCode Lib "MIDIData.dll" _
    (ByVal lCharCode As Long) As Long

'*********************************************************************/
'*                                                                   */
'*�@MIDIEvent�N���X�֐�                                              */
'*                                                                   */
'*********************************************************************/

'�����L�Ȃ�����A����I����0�ȊO�A�ُ�I����0��Ԃ����̂Ƃ���B
'��Is�n�֐��́A���L�Ȃ�����A�u�͂��v��0�ȊO�A�u�������v��0��Ԃ����̂Ƃ���B


' �����C�x���g�̍ŏ��̃C�x���g��Ԃ��B
' �����C�x���g�łȂ��ꍇ�ApEvent���g��Ԃ��B
Declare Function MIDIEvent_GetFirstCombinedEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �����C�x���g�̍Ō�̃C�x���g��Ԃ��B
' �����C�x���g�łȂ��ꍇ�ApEvent���g��Ԃ��B
Declare Function MIDIEvent_GetLastCombinedEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �P�̃C�x���g����������
Declare Function MIDIEvent_Combine Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �����C�x���g��؂藣��
Declare Function MIDIEvent_Chop Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' MIDI�C�x���g�̍폜(�������Ă���ꍇ�ł��P���MIDI�C�x���g���폜)
Declare Sub MIDIEvent_DeleteSingle Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)

' MIDI�C�x���g�̍폜(�������Ă���ꍇ�A�������Ă���MIDI�C�x���g���폜)
Declare Sub MIDIEvent_Delete Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)

' MIDI�C�x���g(�C��)�𐶐����AMIDI�C�x���g�ւ̃|�C���^��Ԃ�(���s��NULL�A�ȉ����l)
Declare Function MIDIEvent_Create Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lKind As Long, _
    ByRef pData As Byte, ByVal lLen As Long) As Long

' �w��C�x���g�Ɠ���MIDI�C�x���g�𐶐����AMIDI�C�x���g�ւ̃|�C���^��Ԃ�(���s��NULL�A�ȉ����l)
Declare Function MIDIEvent_CreateClone Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �V�[�P���X�ԍ��C�x���g�̐���
Declare Function MIDIEvent_CreateSequenceNumber Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lNumber As Long) As Long

' �e�L�X�g�C�x���g�̐���
Declare Function MIDIEvent_CreateTextEvent Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateTextEventA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' ���쌠�C�x���g�̐���
Declare Function MIDIEvent_CreateCopyrightNotice Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateCopyrightNoticeA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N���C�x���g�̐���
Declare Function MIDIEvent_CreateTrackName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateTrackNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �C���X�g�D�������g���C�x���g�̐���
Declare Function MIDIEvent_CreateInstrumentName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateInstrumentNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �̎��C�x���g�̐���
Declare Function MIDIEvent_CreateLyric Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateLyricA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �}�[�J�[�C�x���g�̐���
Declare Function MIDIEvent_CreateMarker Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateMarkerA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �L���[�|�C���g�C�x���g�̐���
Declare Function MIDIEvent_CreateCuePoint Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateCuePointA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �v���O�������C�x���g�̐���
Declare Function MIDIEvent_CreateProgramName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateProgramNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �f�o�C�X���C�x���g�̐���
Declare Function MIDIEvent_CreateDeviceName Lib "MIDIData.dll" _
    Alias "MIDIEvent_CreateDeviceNameA" _
    (ByVal lTime As Long, ByVal strText As String) As Long

' �`�����l���v���t�B�b�N�X�C�x���g�̐���
Declare Function MIDIEvent_CreateChannelPrefix Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lChannel As Long) As Long

' �|�[�g�v���t�B�b�N�X�C�x���g�̐���
Declare Function MIDIEvent_CreatePortPrefix Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lPort As Long) As Long

' �G���h�I�u�g���b�N�C�x���g�̐���
Declare Function MIDIEvent_CreateEndofTrack Lib "MIDIData.dll" _
    (ByVal lTime As Long) As Long

' �e���|�C�x���g�̐���
Declare Function MIDIEvent_CreateTempo Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lTempo As Long) As Long

' SMPTE�I�t�Z�b�g�C�x���g�̐���
Declare Function MIDIEvent_CreateSMPTEOffset Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lMode As Long, ByVal lHour As Long, _
    ByVal lMin As Long, ByVal lSec As Long, ByVal lFrame As Long, _
    ByVal lSubFrame As Long) As Long

' ���q�L���C�x���g�̐���
Declare Function MIDIEvent_CreateTimeSignature Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lnn As Long, ByVal ldd As Long, _
    ByVal lcc As Long, ByVal lbb As Long) As Long

' �����L���C�x���g�̐���
Declare Function MIDIEvent_CreateKeySignature Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lsf As Long, ByVal lmi As Long) As Long

' �m�[�g�I�t�C�x���g�̐���
Declare Function MIDIEvent_CreateNoteOff Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long) As Long

' �m�[�g�I���C�x���g�̐���
Declare Function MIDIEvent_CreateNoteOn Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long) As Long

' �m�[�g�C�x���g�̐���(MIDIEvent_CreateNoteOnNoteOn0�Ɠ���)
Declare Function MIDIEvent_CreateNote Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long, ByVal lDur As Long) As Long

' �m�[�g�C�x���g�̐���(0x8n�����^)
' (�m�[�g�I��(0x9n)�E�m�[�g�I�t(0x8n)��2�C�x���g�𐶐����A
' NoteOn�ւ̃|�C���^��Ԃ�)
Declare Function MIDIEvent_CreateNoteOnNoteOff Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
    ByVal lVel1 As Long, ByVal lVel2 As Long, ByVal lDur As Long) _
    As Long

' �m�[�g�C�x���g�̐���(0x9n�����^)
' (�m�[�g�I��(0x9n)�E�m�[�g�I��(0x9n(vel==0))��2�C�x���g�𐶐����A
' NoteOn�ւ̃|�C���^��Ԃ�)
Declare Function MIDIEvent_CreateNoteOnNoteOn0 Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
     ByVal lVel As Long, ByVal lDur As Long) As Long

' �L�[�A�t�^�[�^�b�`�C�x���g�̐���
Declare Function MIDIEvent_CreateKeyAftertouch Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lKey As Long, _
    ByVal lVal As Long) As Long

' �R���g���[���`�F���W�C�x���g�̐���
Declare Function MIDIEvent_CreateControlChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lNum As Long, _
    ByVal lVal As Long) As Long

' RPN�`�F���W�C�x���g�̐���
' (CC#101+CC#100+CC#6��3�C�x���g�𐶐����ACC#101�ւ̃|�C���^��Ԃ�)
Declare Function MIDIEvent_CreateRPNChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lCC101 As Long, _
    ByVal lCC100 As Long, ByVal lVal As Long) As Long

' NRPN�`�F���W�C�x���g�̐���
' (CC#99+CC#98+CC#6��3�C�x���g�𐶐����ACC#99�ւ̃|�C���^��Ԃ�)
Declare Function MIDIEvent_CreateNRPNChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lCC99 As Long, _
    ByVal lCC98 As Long, ByVal lVal As Long) As Long

' �v���O�����`�F���W�C�x���g�̐���
Declare Function MIDIEvent_CreateProgramChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lVal As Long) As Long

' �p�b�`�`�F���W�C�x���g�̐���
' (CC#32+CC#0+PC��3�C�x���g�𐶐����ACC#0�ւ̃|�C���^��Ԃ�)
Declare Function MIDIEvent_CreatePatchChange Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lCC0 As Long, _
    ByVal lCC32 As Long, ByVal lNum As Long) As Long

' �`�����l���A�t�^�[�^�b�`�C�x���g�̐���
Declare Function MIDIEvent_CreateChannelAftertouch Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal lCh As Long, ByVal lVal As Long) As Long

' �s�b�`�x���h�C�x���g�̐���
Declare Function MIDIEvent_CreatePitchBend Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByVal nCh As Long, ByVal lVal As Long) As Long

' �V�X�e���G�N�X�N���[�V���C�x���g�̐���
Declare Function MIDIEvent_CreateSysExEvent Lib "MIDIData.dll" _
    (ByVal lTime As Long, ByRef pData As Byte, ByVal lLen As Long) As Long


' ���^�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsMetaEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �V�[�P���X�ԍ��C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsSequenceNumber Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �e�L�X�g�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsTextEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' ���쌠�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsCopyrightNotice Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �g���b�N���C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsTrackName Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �̎��C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsLyric Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �}�[�J�[�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsMarker Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �L���[�|�C���g�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsCuePoint Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �v���O�������C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsProgramName Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' �f�o�C�X���C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsDeviceName Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �`�����l���v���t�B�b�N�X�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsChannelPrefix Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' �|�[�g�v���t�B�b�N�X�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsPortPrefix Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �G���h�I�u�g���b�N�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsEndofTrack Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' �e���|�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsTempo Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' SMPTE�I�t�Z�b�g�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' ���q�L���C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' �����L���C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    
' �V�[�P���T�Ǝ��̃C�x���g�ł��邩�ǂ������ׂ�
Declare Function MIDIEvnet_IsSequencerSpecific Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long
    

' MIDI�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsMIDIEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �m�[�g�I���C�x���g�ł��邩�ǂ����𒲂ׂ�
' (�m�[�g�I���ł��x���V�e�B��0���̂̓m�[�g�I�t�Ƃ݂Ȃ��B�ȉ����l)
Declare Function MIDIEvent_IsNoteOn Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �m�[�g�I�t�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsNoteOff Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �m�[�g�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsNote Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long


' NOTEONOTEOFF�C�x���g�ł��邩�ǂ����𒲂ׂ�
' ����̓m�[�g�I��(0x9n)�ƃm�[�g�I�t(0x8n)�������C�x���g�����C�x���g�łȂ���΂Ȃ�Ȃ��B
Declare Function MIDIEvent_IsNoteOnNoteOff Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' NOTEONNOTEON0�C�x���g�ł��邩�ǂ����𒲂ׂ�
' ����̓m�[�g�I��(0x9n)�ƃm�[�g�I�t(0x9n,vel==0)�������C�x���g�����C�x���g�łȂ���΂Ȃ�Ȃ��B
Declare Function MIDIEvent_IsNoteOnNoteOn0 Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �L�[�A�t�^�[�^�b�`�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsKeyAftertouch Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �R���g���[���`�F���W�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsControlChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' RPN�`�F���W�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsRPNChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' NRPN�`�F���W�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsNRPNChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �v���O�����`�F���W�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsProgramChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �p�b�`�`�F���W�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsPatchChange Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �`�����l���A�t�^�[�^�b�`�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsChannelAftertouch Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �s�b�`�x���h�C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsPitchBend Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �V�X�e���G�N�X�N���[�V���C�x���g�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDIEvent_IsSysExEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' ���V�C�x���g�ł��邩�ǂ������ׂ�
Declare Function MIDIEvent_IsFloating Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)
    
' �����C�x���g�ł��邩�ǂ������ׂ�
Declare Function MIDIEvent_IsCombined Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long)
    

' �C�x���g�̎�ނ��擾
Declare Function MIDIEvent_GetKind Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̎�ނ�ݒ�
Declare Function MIDIEvent_SetKind Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lKind As Long) As Long

' �C�x���g�̒����擾
Declare Function MIDIEvent_GetLen Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̃f�[�^�����擾
Declare Function MIDIEvent_GetData Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' �C�x���g�̃f�[�^����ݒ�(���̊֐��͑�ϊ댯�ł��B�������`�F�L�͂��܂���)
Declare Function MIDIEvent_SetData Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' �C�x���g�̃e�L�X�g���擾(�e�L�X�g�E���쌠�E�g���b�N���E�C���X�g�D�������g���E
' �̎��E�}�[�J�[�E�L���[�|�C���g�E�v���O�������E�f�o�C�X���̂�)
Declare Function MIDIEvent_GetText Lib "MIDIData.dll" _
    Alias "MIDIEvent_GetTextA" _
    (ByVal pEvent As Long, ByVal strBuf As String, ByVal lLen As Long) As Long

' �C�x���g�̃e�L�X�g��ݒ�(�e�L�X�g�E���쌠�E�g���b�N���E�C���X�g�D�������g���E
' �̎��E�}�[�J�[�E�L���[�|�C���g�E�v���O�������E�f�o�C�X���̂�)
Declare Function MIDIEvent_SetText Lib "MIDIData.dll" _
    Alias "MIDIEvent_SetTextA" _
    (ByVal pEvent As Long, ByVal strText As String) As Long

' SMPTE�I�t�Z�b�g�擾(SMPTE�I�t�Z�b�g�C�x���g�̂�)
Declare Function MIDIEvent_GetSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pMode As Long, ByRef pHour As Long, _
    ByRef pMin As Long, ByRef pSec As Long, ByRef pFrame As Long, _
    ByRef pSubFrame As Long) As Long

' SMPTE�I�t�Z�b�g�ݒ�(SMPTE�I�t�Z�b�g�C�x���g�̂�)
Declare Function MIDIEvent_SetSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lMode As Long, ByVal lHour As Long, _
    ByVal lMin As Long, ByVal lSec As Long, ByVal lFrame As Long, _
    ByVal lSubFrame As Long) As Long

' �e���|�擾(�e���|�C�x���g�̂�)
Declare Function MIDIEvent_GetTempo Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �e���|�ݒ�(�e���|�C�x���g�̂�)
Declare Function MIDIEvent_SetTempo Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lTempo As Long) As Long

' ���q�L���擾(���q�L���C�x���g�̂�)
Declare Function MIDIEvent_GetTimeSignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef pnn As Long, ByRef pdd As Long, _
     ByRef pcc As Long, ByRef pbb As Long) As Long

' ���q�L���̐ݒ�(���q�L���C�x���g�̂�)
Declare Function MIDIEvent_SetTimeSignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lnn As Long, ByVal ldd As Long, _
     ByVal lcc As Long, ByVal lbb As Long) As Long

' �����L���̎擾(�����L���C�x���g�̂�)
Declare Function MIDIEvent_GetKeySignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByRef psf As Long, ByRef pmi As Long) As Long

' �����L���̐ݒ�(�����L���C�x���g�̂�)
Declare Function MIDIEvent_SetKeySignature Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lsf As Long, ByVal lmi As Long) As Long

' �C�x���g�̃��b�Z�[�W�擾(MIDI�`�����l���C�x���g�y�уV�X�e���G�N�X�N���[�V���̂�)
Declare Function MIDIEvent_GetMIDIMessage Lib "MIDIData.dll" _
(ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' �C�x���g�̃��b�Z�[�W�ݒ�(MIDI�`�����l���C�x���g�y�уV�X�e���G�N�X�N���[�V���̂�)
Declare Function MIDIEvent_SetMIDIMessage Lib "MIDIData.dll" _
(ByVal pEvent As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' �C�x���g�̃`�����l���擾(MIDI�`�����l���C�x���g�̂�)
Declare Function MIDIEvent_GetChannel Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̃`�����l���ݒ�(MIDI�`�����l���C�x���g�̂�)
Declare Function MIDIEvent_SetChannel Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lChannel As Long) As Long

' �C�x���g�̎����擾
Declare Function MIDIEvent_GetTime Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̎����ݒ�
Declare Function MIDIEvent_SetTime Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lTime As Long) As Long

' �C�x���g�̃L�[�擾(�m�[�g�I�t�E�m�[�g�I���E�`�����l���A�t�^�[�̂�)
Declare Function MIDIEvent_GetKey Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̃L�[�ݒ�(�m�[�g�I�t�E�m�[�g�I���E�`�����l���A�t�^�[�̂�)
Declare Function MIDIEvent_SetKey Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lKey As Long) As Long

' �C�x���g�̃x���V�e�B�擾(�m�[�g�I�t�E�m�[�g�I���̂�)
Declare Function MIDIEvent_GetVelocity Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̃x���V�e�B�ݒ�(�m�[�g�I�t�E�m�[�g�I���̂�)
Declare Function MIDIEvent_SetVelocity Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lVel As Long) As Long

' �����C�x���g�̉������擾(�m�[�g�̂�)
Declare Function MIDIEvent_GetDuration Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �����C�x���g�̉������ݒ�(�m�[�g�̂�)
Declare Function MIDIEvent_SetDuration Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �����C�x���g�̃o���N�擾(�p�b�`�`�F���W�ERPN/NRPN�`�F���W�̂�)
Declare Function MIDIEvent_GetBank Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �����C�x���g�̃o���N���(MSB)�擾(�p�b�`�`�F���W�ERPN/NRPN�`�F���W�̂�)
Declare Function MIDIEvent_GetBankMSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �����C�x���g�̃o���N����(LSB)�擾(�p�b�`�`�F���W�ERPN/NRPN�`�F���W�̂�)
Declare Function MIDIEvent_GetBankLSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �����C�x���g�̃o���N�ݒ�(�p�b�`�`�F���W�ERPN/NRPN�`�F���W�̂�)
Declare Function MIDIEvent_SetBank Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lBank As Long) As Long

' �����C�x���g�̃o���N���(MSB)�ݒ�(�p�b�`�`�F���W�ERPN/NRPN�`�F���W�̂�)
Declare Function MIDIEvent_SetBankMSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lBank As Long) As Long

' �����C�x���g�̃o���N����(LSB)�ݒ�(�p�b�`�`�F���W�ERPN/NRPN�`�F���W�̂�)
Declare Function MIDIEvent_SetBankLSB Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lBank As Long) As Long

' �C�x���g�̔ԍ��擾(�R���g���[���`�F���W�E�v���O�����`�F���W�Ȃǂ̂�)
Declare Function MIDIEvent_GetNumber Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̔ԍ��ݒ�(�R���g���[���`�F���W�E�v���O�����`�F���W�Ȃǂ̂�)
Declare Function MIDIEvent_SetNumber Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lNumber As Long) As Long

' �C�x���g�̒l�擾(�L�[�A�t�^�[�E�R���g���[���[�E�`�����l���A�t�^�[�E�s�b�`�x���h)
Declare Function MIDIEvent_GetValue Lib "MIDIData.dll" _
    (ByVal pEvent As Long) As Long

' �C�x���g�̒l�ݒ�(�L�[�A�t�^�[�E�R���g���[���[�E�`�����l���A�t�^�[�E�s�b�`�x���h)
Declare Function MIDIEvent_SetValue Lib "MIDIData.dll" _
    (ByVal pEvent As Long, ByVal lValue As Long) As Long

' ���̃C�x���g�ւ̃|�C���^���擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDIEvent_GetNextEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �O�̃C�x���g�ւ̃|�C���^���擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDIEvent_GetPrevEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' ���̓���̃C�x���g�ւ̃|�C���^���擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDIEvent_GetNextSameKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �O�̓���̃C�x���g�ւ̃|�C���^���擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDIEvent_GetPrevSameKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' �e�g���b�N�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIEvnet_GetParent Lib "MIDIData.dll" _
    (ByVal pMIDIEvent As Long) As Long

' ������\���ɕϊ�(���B���̊֐��̎d�l�͕p�ɂɕύX����܂�)
Declare Function MIDIEvent_ToString Lib "MIDIData.dll" _
    Alias "MIDIEvent_ToStringA" _
    (ByVal pEvent As Long, ByVal pBuf As String, ByVal lLen As Long) As Long

'*********************************************************************/
'*                                                                   */
'*�@MIDITrack�N���X�֐�                                               */
'*                                                                   */
'*********************************************************************/

' �g���b�N���̃C�x���g�̑������擾
Declare Function MIDITrack_GetNumEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̍ŏ��̃C�x���g�擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDITrack_GetFirstEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̍Ō�̃C�x���g�擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDITrack_GetLastEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̎w���ނ̍ŏ��̃C�x���g�擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDITrack_GetFirstKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lKind As Long) As Long

' �g���b�N�̎w���ނ̍Ō�̃C�x���g�擾(�Ȃ����NULL��Ԃ�)
Declare Function MIDITrack_GetLastKindEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lKind As Long) As Long

' ����MIDI�g���b�N�ւ̃|�C���^�擾(�Ȃ����NULL)(20080715�ǉ�)
Declare Function MIDITrack_GetNextTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �O��MIDI�g���b�N�ւ̃|�C���^�擾(�Ȃ����NULL)(20080715�ǉ�)
Declare Function MIDITrack_GetPrevTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̐eMIDI�f�[�^�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDITrack_GetParent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̃C�x���g�����J�E���g���Ď擾
Declare Function MIDITrack_CountNumEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̊J�n����(�ŏ��̃C�x���g�̎���)[Tick]���擾(20081101�ǉ�)
Declare Function MIDITrack_GetBeginTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̏I������(�Ō�̃C�x���g�̎���)[Tick]���擾(20081101�ǉ�)
Declare Function MIDITrack_GetEndTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̖��O���ȈՎ擾
Declare Function MIDITrack_GetName Lib "MIDIData.dll" _
    Alias "MIDITrack_GetNameA" _
    (ByVal pMIDITrack As Long, ByVal pBuf As String, ByVal lLen As Long) As Long

' �g���b�N�̓��͂��擾(0=OFF, 1=ON)
Declare Function MIDITrack_GetInputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̓��̓|�[�g���擾(0�`255)
Declare Function MIDITrack_GetInputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̓��̓`�����l�����擾(-1=n/a, 0�`15)
Declare Function MIDITrack_GetInputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̏o�͂��擾(0=OFF, 1=ON)
Declare Function MIDITrack_GetOutputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̏o�̓|�[�g���擾(0�`255)
Declare Function MIDITrack_GetOutputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̏o�̓`�����l�����擾(-1=n/a, 0�`15)
Declare Function MIDITrack_GetOutputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̃^�C��+���擾
Declare Function MIDITrack_GetTimePlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̃L�[+���擾
Declare Function MIDITrack_GetKeyPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̃x���V�e�B+���擾
Declare Function MIDITrack_GetVelPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̕\�����[�h���擾
Declare Function MIDITrack_GetViewMode Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̑O�i�F���擾
Declare Function MIDITrack_GetForeColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̔w�i�F���擾
Declare Function MIDITrack_GetBackColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�̖��O���ȈՐݒ�
Declare Function MIDITrack_SetName Lib "MIDIData.dll" _
    Alias "MIDITrack_SetNameA" _
    (ByVal pMIDITrack As Long, ByVal pszText As String) As Long

' �g���b�N�̓��͂�ݒ�(0=OFF, 1=ON)
Declare Function MIDITrack_SetInputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lInputOn As Long) As Long

' �g���b�N�̓��̓|�[�g��ݒ�(0�`255)
Declare Function MIDITrack_SetInputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lInputPort As Long) As Long

' �g���b�N�̓��̓`�����l����ݒ�(-1=n/a, 0�`15)
Declare Function MIDITrack_SetInputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lInputChannel As Long) As Long

' �g���b�N�̏o�͂�ݒ�(0=OFF, 1=ON)
Declare Function MIDITrack_SetOutputOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lOutputOn As Long) As Long

' �g���b�N�̏o�̓|�[�g��ݒ�(0�`255)
Declare Function MIDITrack_SetOutputPort Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lOutputPort As Long) As Long

' �g���b�N�̏o�̓`�����l����ݒ�(-1=n/a, 0�`15)
Declare Function MIDITrack_SetOutputChannel Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lOutputChannel As Long) As Long

' �g���b�N�̃^�C��+��ݒ�
Declare Function MIDITrack_SetTimePlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTimePlus As Long) As Long

' �g���b�N�̃L�[+��ݒ�
Declare Function MIDITrack_SetKeyPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lKeyPlus As Long) As Long

' �g���b�N�̃x���V�e�B+��ݒ�
Declare Function MIDITrack_SetVelPlus Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lVelPlus As Long) As Long

' �g���b�N�̕\�����[�h��ݒ�
Declare Function MIDITrack_SetViewMode Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lViewMode As Long) As Long

' �g���b�N�̑O�i�F��ݒ�
Declare Function MIDITrack_SetForeColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lForeColor As Long) As Long

' �g���b�N�̔w�i�F��ݒ�
Declare Function MIDITrack_SetBackColor Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lBackColor As Long) As Long

' XF�ł���Ƃ��AXF�̃��@�[�W�������擾(XF�łȂ����0)
Declare Function MIDITrack_GetXFVersion Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long
    
' �g���b�N�̍폜(�g���b�N���Ɋ܂܂��C�x���g�I�u�W�F�N�g���폜����܂�)
Declare Function MIDITrack_Delete Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�𐶐����g���b�N�ւ̃|�C���^��Ԃ�(���s��NULL��Ԃ�)
Declare Function MIDITrack_Create Lib "MIDIData.dll" () As Long

' �w��g���b�N�Ɠ���MIDI�g���b�N�𐶐����AMIDI�g���b�N�ւ̃|�C���^��Ԃ�(���s��NULL��Ԃ�)
Declare Function MIDITrack_CreateClone Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �g���b�N�ɃC�x���g��}��(�C�x���g�͂��炩���ߐ������Ă���)
Declare Function MIDITrack_InsertEventAfter Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long, ByVal pTarget As Long) As Long

' �g���b�N�ɃC�x���g��}��(�C�x���g�͂��炩���ߐ������Ă���)
Declare Function MIDITrack_InsertEventBefore Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long, ByVal pTarget As Long) As Long

' �g���b�N�ɃC�x���g��}��(�C�x���g�͂��炩���ߐ������Ă���)
Declare Function MIDITrack_InsertEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long) As Long

' �g���b�N�ɃV�[�P���X�ԍ��C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertSequenceNumber Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lNumber As Long) As Long

' �g���b�N�Ƀe�L�X�g�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertTextEvent Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertTextEventA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�ɒ��쌠�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertCopyrightNotice Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertCopyrightNoticeA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�Ƀg���b�N���C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertTrackName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertTrackNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�ɃC���X�g�D�������g���C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertInstrumentName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertInstrumentNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�ɉ̎��C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertLyric Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertLyricA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�Ƀ}�[�J�[�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertMarker Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertMarkerA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�ɃL���[�|�C���g�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertCuePoint Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertCuePointA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�Ƀv���O�������C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertProgramName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertProgramNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�Ƀf�o�C�X���C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertDeviceName Lib "MIDIData.dll" _
    Alias "MIDITrack_InsertDeviceNameA" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal strText As String) As Long

' �g���b�N�Ƀ`�����l���v���t�B�b�N�X�𐶐����đ}��
Declare Function MIDITrack_InsertChannelPrefix Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lChannel As Long) As Long

' �g���b�N�Ƀ|�[�g�v���t�B�b�N�X�𐶐����đ}��
Declare Function MIDITrack_InsertPortPrefix Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lPort As Long) As Long

' �g���b�N�ɃG���h�I�u�g���b�N�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertEndofTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long) As Long

' �g���b�N�Ƀe���|�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertTempo Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lTempo As Long) As Long

' �g���b�N��SMPTE�I�t�Z�b�g�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertSMPTEOffset Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByVal lMode As Long, ByVal lHour As Long, ByVal lMin As Long, ByVal lSec As Long, _
    ByVal lFrame As Long, ByVal lSubFrame As Long) As Long

' �g���b�N�ɔ��q�L���C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByVal lnn As Long, ByVal ldd As Long, _
     ByVal lcc As Long, ByVal lbb As Long) As Long

' �g���b�N�ɒ����L���C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByVal lsf As Long, ByVal lmi As Long) As Long

' �g���b�N�ɃV�[�P���T�[�Ǝ��̃C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertSequencerSpecific Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByRef pBuf As Byte, ByVal lLen As Long) As Long
     
' �g���b�N�Ƀm�[�g�I�t�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertNoteOff Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVel As Long) As Long
     
' �g���b�N�Ƀm�[�g�I���C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertNoteOn Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVel As Long) As Long

' �g���b�N�Ƀm�[�g�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertNote Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVel As Long, ByVal lDur As Long) As Long

' �g���b�N�ɃL�[�A�t�^�[�^�b�`�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertKeyAftertouch Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lKey As Long, ByVal lVal As Long) As Long

' �g���b�N�ɃR���g���[���`�F���W�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertControlChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lNum As Long, ByVal lVal As Long) As Long

' �g���b�N��RPN�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertRPNChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lCC101 As Long, ByVal lCC100 As Long, ByVal lVal As Long) As Long

' �g���b�N��NRPN�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertNRPNChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lCC99 As Long, ByVal lCC98 As Long, ByVal lVal As Long) As Long

' �g���b�N�Ƀv���O�����`�F���W�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertProgramChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lNum As Long) As Long

' �g���b�N�Ƀp�b�`�`�F���W�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertPatchChange Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lCC0 As Long, ByVal lCC32 As Long, ByVal lNum As Long) As Long

' �g���b�N�Ƀ`�����l���A�t�^�[�^�b�`�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertChannelAftertouch Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lVal As Long) As Long

' �g���b�N�Ƀs�b�`�x���h�C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertPitchBend Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByVal lCh As Long, _
     ByVal lVal As Long) As Long

' �g���b�N�ɃV�X�e���G�N�X�N���[�V���C�x���g�𐶐����đ}��
Declare Function MIDITrack_InsertSysExEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByRef pBuf As Byte, _
     ByVal lLen As Long) As Long

' �g���b�N����C�x���g��1��菜��(�C�x���g�I�u�W�F�N�g�͍폜���܂���) */
Declare Function MIDITrack_RemoveSingleEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long) As Long

' �g���b�N����C�x���g����菜��(�C�x���g�I�u�W�F�N�g�͍폜���܂���)
Declare Function MIDITrack_RemoveEvent Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pEvent As Long) As Long

' MIDI�g���b�N�����V�g���b�N�ł��邩�ǂ����𒲂ׂ�
Declare Function MIDITrack_IsFloating Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' MIDI�g���b�N���Z�b�g�A�b�v�g���b�N�Ƃ��Đ��������Ƃ��m�F����
Declare Function MIDITrack_CheckSetupTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' MIDI�g���b�N���m���Z�b�g�A�b�v�g���b�N�Ƃ��Đ��������Ƃ��m�F����
Declare Function MIDITrack_CheckNonSetupTrack Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long) As Long

' �^�C���R�[�h���~���b�����ɕϊ�(�w��g���b�N���̃e���|�C�x���g����Ɍv�Z)
Declare Function MIDITrack_TimeToMillisec Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long) As Long

' �~���b�������^�C���R�[�h�ɕϊ�(�w��g���b�N���̃e���|�C�x���g����Ɍv�Z)
Declare Function MIDITrack_MillisecToTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lMillisec As Long) As Long

' �^�C���R�[�h�����߁F���F�e�B�b�N�ɕ���(�w��g���b�N���̔��q�L������Ɍv�Z)
Declare Function MIDITrack_BreakTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' �^�C���R�[�h�����߁F���F�e�B�b�N�ɕ���(�w��g���b�N���̔��q�L������v�Z)
Declare Function MIDITrack_BreakTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long) As Long

' ���߁F���F�e�B�b�N����^�C���R�[�h�𐶐�(�w��g���b�N���̔��q�L������v�Z)
Declare Function MIDITrack_MakeTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lMeasure As Long, ByVal lBeat As Long, _
    ByVal lTick As Long, ByRef pTime As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' ���߁F���F�e�B�b�N����^�C���R�[�h�𐶐�(�w��g���b�N���̔��q�L������v�Z)
Declare Function MIDITrack_MakeTime Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal pMeasure As Long, ByVal pBeat As Long, _
    ByVal pTick As Long, ByRef pTime As Long) As Long

' �w�莞���ɂ�����e���|���擾
Declare Function MIDITrack_FindTempo Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, ByRef pTempo As Long) As Long

' �w�莞���ɂ����锏�q�L�����擾
Declare Function MIDITrack_FindTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
     ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' �w�莞���ɂ����钲���L�����擾
Declare Function MIDITrack_FindKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDITrack As Long, ByVal lTime As Long, _
    ByRef psf As Long, ByRef pmi As Long) As Long


'*********************************************************************/
'*                                                                   */
'*�@MIDIData�N���X�֐�                                                */
'*                                                                   */
'*********************************************************************/

' MIDI�f�[�^�̎w��g���b�N�̒��O�Ƀg���b�N��}��
Declare Function MIDIData_InsertTrackBefore Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long, ByVal pTarget As Long) As Long

' MIDI�f�[�^�̎w��g���b�N�̒���Ƀg���b�N��}��
Declare Function MIDIData_InsertTrackAfter Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long, ByVal pTarget As Long) As Long

' MIDI�f�[�^�Ƀg���b�N��ǉ�(�g���b�N�͗\�ߍ쐬���Ă���)
Declare Function MIDIData_AddTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long) As Long

' MIDI�f�[�^����g���b�N������(�g���b�N���̋y�уg���b�N���̃C�x���g�͍폜���Ȃ�)
Declare Function MIDIData_RemoveTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMIDITrack As Long) As Long

' MIDI�f�[�^�̍폜(�܂܂��g���b�N�y�уC�x���g�����ׂč폜)
Declare Function MIDIData_Delete Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�𐶐����AMIDI�f�[�^�ւ̃|�C���^��Ԃ�(���s��NULL)
Declare Function MIDIData_Create Lib "MIDIData.dll" _
    (ByVal lFormat As Long, ByVal lNumTrack As Long, _
     ByVal lTimeBase As Long, ByVal lTimeResolution As Long) As Long

' MIDI�f�[�^�̃t�H�[�}�b�g0/1/2���擾
Declare Function MIDIData_GetFormat Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̃t�H�[�}�b�g0/1/2��ݒ�(�ύX���R���o�[�g�@�\���܂�)
Declare Function MIDIData_SetFormat Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lFormat As Long) As Long

' MIDI�f�[�^�̃^�C���x�[�X���擾
Declare Function MIDIData_GetTimeBase Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByRef lTimeMode As Long, _
     ByRef lTimeResolution As Long) As Long

' MIDI�f�[�^�̃^�C�����[�h�擾
Declare Function MIDIData_GetTimeMode Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̃^�C�����]�����[�V�����擾
Declare Function MIDIData_GetTimeResolution Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̃^�C���x�[�X�ݒ�
Declare Function MIDIData_SetTimeBase Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTimeMode As Long, _
     ByVal lTimeResolution As Long) As Long

' MIDI�f�[�^�̃g���b�N���擾
Declare Function MIDIData_GetNumTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̃g���b�N�����J�E���g���Ď擾
Declare Function MIDIData_CountTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' XF�ł���Ƃ��AXF�̃��@�[�W�������擾(XF�łȂ����0)
Declare Function MIDIData_GetXFVersion Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̍ŏ��̃g���b�N�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIData_GetFirstTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̍Ō�̃g���b�N�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIData_GetLastTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̎w��ԍ��̃g���b�N�ւ̃|�C���^���擾(�Ȃ����NULL)
Declare Function MIDIData_GetTrack Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTrackIndex As Long) As Long

' MIDI�f�[�^�̊J�n����[tick]�̎擾
Declare Function MIDIData_GetBeginTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̏I������[tick]�̎擾
Declare Function MIDIData_GetEndTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long) As Long

' MIDI�f�[�^�̃^�C�g���̊ȈՎ擾
Declare Function MIDIData_GetTitle Lib "MIDIData.dll" _
    Alias "MIDIData_GetTitleA" _
    (ByVal pMIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDI�f�[�^�̃^�C�g���̊ȈՐݒ�
Declare Function MIDIData_SetTitle Lib "MIDIData.dll" _
    Alias "MIDIData_SetTitleA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' MIDI�f�[�^�̃T�u�^�C�g���̊ȈՎ擾
Declare Function MIDIData_GetSubTitle Lib "MIDIData.dll" _
    Alias "MIDIData_GetSubTitleA" _
    (ByVal pMIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDI�f�[�^�̃T�u�^�C�g���̊ȈՐݒ�
Declare Function MIDIData_SetSubTitle Lib "MIDIData.dll" _
    Alias "MIDIData_SetSubTitleA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' MIDI�f�[�^�̒��쌠�̊ȈՎ擾
Declare Function MIDIData_GetCopyright Lib "MIDIData.dll" _
    Alias "MIDIData_GetCopyrightA" _
    (ByVal MIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDI�f�[�^�̒��쌠�̊ȈՐݒ�
Declare Function MIDIData_SetCopyright Lib "MIDIData.dll" _
    Alias "MIDIData_SetCopyrightA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' MIDI�f�[�^�̃R�����g�̊ȈՎ擾
Declare Function MIDIData_GetComment Lib "MIDIData.dll" _
    Alias "MIDIData_GetCommentA" _
    (ByVal pMIDIData As Long, ByVal pData As String, ByVal lLen As Long) As Long

' MIDI�f�[�^�̃R�����g�̊ȈՐݒ�
Declare Function MIDIData_SetComment Lib "MIDIData.dll" _
    Alias "MIDIData_SetCommentA" _
    (ByVal pMIDIData As Long, ByVal strText As String) As Long

' �^�C���R�[�h���~���b�ɕϊ�(�t�H�[�}�b�g0/1�̏ꍇ�̂�)
Declare Function MIDIData_TimeToMillisec Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long) As Long

' �~���b���^�C���R�[�h�ɕϊ�(�t�H�[�}�b�g0/1�̏ꍇ�̂�)
Declare Function MIDIData_MillisecToTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lMillisec As Long) As Long

' �^�C���R�[�h�����߁F���F�e�B�b�N�ɕ���(�ŏ��̃g���b�N���̔��q�L������Ɍv�Z)
Declare Function MIDIData_BreakTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' �^�C���R�[�h�����߁F���F�e�B�b�N�ɕ���(�ŏ��̃g���b�N���̔��q�L������v�Z)
Declare Function MIDIData_BreakTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
    ByRef pMeasure As Long, ByRef pBeat As Long, ByRef pTick As Long) As Long

' ���߁F���F�e�B�b�N����^�C���R�[�h�𐶐�(�ŏ��̃g���b�N���̔��q�L������v�Z)
Declare Function MIDIData_MakeTimeEx Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lMeasure As Long, ByVal lBeat As Long, _
    ByVal lTick As Long, ByRef pTime As Long, _
    ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' ���߁F���F�e�B�b�N����^�C���R�[�h�𐶐�(�ŏ��̃g���b�N���̔��q�L������v�Z)
Declare Function MIDIData_MakeTime Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal pMeasure As Long, ByVal pBeat As Long, _
    ByVal pTick As Long, ByRef pTime As Long) As Long

' �w�莞���ɂ�����e���|���擾
Declare Function MIDIData_FindTempo Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, ByRef pTempo As Long) As Long

' �w�莞���ɂ����锏�q�L�����擾
Declare Function MIDIData_FindTimeSignature Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
     ByRef pnn As Long, ByRef pdd As Long, ByRef pcc As Long, ByRef pbb As Long) As Long

' �w�莞���ɂ����钲���L�����擾
Declare Function MIDIData_FindKeySignature Lib "MIDIData.dll" _
    (ByVal pMIDIData As Long, ByVal lTime As Long, _
    ByRef psf As Long, ByRef pmi As Long) As Long

' ����MIDI�f�[�^�ɕʂ�MIDI�f�[�^���}�[�W����(20080715�p�~)
'Declare Function MIDIData_Merge Lib "MIDIData.dll" _
'    (ByVal pMIDIData As Long, ByVal pMergeData As Long, _
'     ByVal lTime As Long, ByVal lFlags As Long, _
'     ByRef pInsertedEventCount As Long, ByRef pDeletedEventCount As Long) As Long

' �ۑ��E�ǂݍ��ݗp�֐�

' MIDIData���X�^���_�[�hMIDI�t�@�C��(SMF)����ǂݍ��݁A
' �V����MIDI�f�[�^�ւ̃|�C���^��Ԃ�(���s��NULL)
Declare Function MIDIData_LoadFromSMF Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromSMFA" _
    (ByVal strFileName As String) As Long

' MIDI�f�[�^���X�^���_�[�hMIDI�t�@�C��(SMF)�Ƃ��ĕۑ�
Declare Function MIDIData_SaveAsSMF Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsSMFA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long

' MIDI�f�[�^���o�C�i���t�@�C�����ǂݍ��݁A
' �V����MIDI�f�[�^�ւ̃|�C���^��Ԃ�(���s��NULL)
Declare Function MIDIData_LoadFromBinary Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromBinaryA" _
    (ByVal strFileName As String) As Long

' MIDI�f�[�^���o�C�i���t�@�C���֕ۑ�
Declare Function MIDIData_SaveAsBinary Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsBinaryA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long

' MIDI�f�[�^��Cherry�t�@�C��(*.chy)���ǂݍ��݁A
' �V����MIDI�f�[�^�ւ̃|�C���^��Ԃ�(���s��NULL)
Declare Function MIDIData_LoadFromCherry Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromCherryA" _
    (ByVal strFileName As String) As Long

' MIDI�f�[�^��Cherry�t�@�C��(*.chy)�֕ۑ�
Declare Function MIDIData_SaveAsCherry Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsCherryA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long

' MIDIData��MIDICSV�t�@�C��(*.csv)����ǂݍ��݁A
' �V����MIDI�f�[�^�ւ̃|�C���^��Ԃ�(���s��NULL)
Declare Function MIDIData_LoadFromMIDICSV Lib "MIDIData.dll" _
    Alias "MIDIData_LoadFromMIDICSVA" _
    (ByVal strFileName As String) As Long

' MIDI�f�[�^��MIDICSV�t�@�C��(*.csv)�Ƃ��ĕۑ�
Declare Function MIDIData_SaveAsMIDICSV Lib "MIDIData.dll" _
    Alias "MIDIData_SaveAsMIDICSVA" _
    (ByVal pMIDIData As Long, ByVal strFileName As String) As Long



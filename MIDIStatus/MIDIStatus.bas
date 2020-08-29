Attribute VB_Name = "MIDIStatus"
'******************************************************************************
'*                                                                            *
'*�@MIDIStatus.bas - MIDIStatus���W���[��(VB4,5,6�p)       (C)2002-2012 ����  *
'*                                                                            *
'******************************************************************************

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
'* MIDIPart�֐��Q                                                             *
'*                                                                            *
'******************************************************************************

' MIDIPart�I�u�W�F�N�g���폜����B
Declare Sub MIDIPart_Delete Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long)

' MIDIPart�I�u�W�F�N�g�𐶐�����B
Declare Function MIDIPart_Create Lib "MIDIStatus.dll" _
    (ByVal pParent As Long) As Long

' MIDIPart_Get�n�֐�

' ���݂̃p�[�g���[�h���擾
Declare Function MIDIPart_GetPartMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃I���j���m�|�����[�h���擾
Declare Function MIDIPart_GetOmniMonoPolyMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃`�����l���t�@�C���`���[�j���O�̒l���擾
Declare Function MIDIPart_GetChannelFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃`�����l���R�[�X�`���[�j���O�̒l���擾
Declare Function MIDIPart_GetChannelCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃s�b�`�x���h�Z���V�e�B�r�e�B�̒l���擾
Declare Function MIDIPart_GetPitchBendSensitivity Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃��W�����[�V�����f�v�X�����W�̒l���擾
Declare Function MIDIPart_GetModulationDepthRange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃��V�[�u�`�����l���̒l���擾
Declare Function MIDIPart_GetReceiveChannel Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃x���V�e�B�Z���X�f�v�X�̒l���擾
Declare Function MIDIPart_GetVelocitySenseDepth Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃x���V�e�B�Z���X�I�t�Z�b�g�̒l���擾
Declare Function MIDIPart_GetVelocitySenseOffset Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃L�[�{�[�h�����W���[�̒l���擾
Declare Function MIDIPart_GetKeyboardRangeLow Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃L�[�{�[�h�����W�n�C�̒l���擾
Declare Function MIDIPart_GetKeyboardRangeHigh Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̌��Ղ̉��������擾����B
' �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B)
Declare Function MIDIPart_GetNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long) As Long

' ���݂̌��Ղ̉��������܂Ƃ߂Ď擾����B
' �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B)
Declare Function MIDIPart_GetNoteEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̌��Ղ̉��������擾����B
' �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B)
Declare Function MIDIPart_GetNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long) As Long

' ���݂̌��Ղ̉�����(�z�[���h�����܂�)���܂Ƃ߂Ď擾����B
' �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B)
Declare Function MIDIPart_GetNoteKeepEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̃L�[�A�t�^�[�^�b�`�̒l���擾
Declare Function MIDIPart_GetKeyAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long) As Long

' ���݂̃L�[�A�t�^�[�^�b�`�̒l���܂Ƃ߂Ď擾
Declare Function MIDIPart_GetKeyAfterTouchEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̃R���g���[���`�F���W�̒l���擾
Declare Function MIDIPart_GetControlChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lNum As Long) As Long

' ���݂̃R���g���[���`�F���W�̒l���܂Ƃ߂Ď擾
Declare Function MIDIPart_GetControlChangeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂�RPNMSB�̒l���擾
Declare Function MIDIPart_GetRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long) As Long

' ���݂�RPNLSB�̒l���擾
Declare Function MIDIPart_GetRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long) As Long

' ���݂�NRPNMSB�̒l���擾
Declare Function MIDIPart_GetNRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long) As Long

' ���݂�NRPNLSB�̒l���擾
Declare Function MIDIPart_GetNRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long) As Long

' ���݂̃v���O�����`�F���W�̒l���擾
Declare Function MIDIPart_GetProgramChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃`�����l���A�t�^�[�^�b�`�̒l���擾
Declare Function MIDIPart_GetChannelAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݂̃s�b�`�x���h�̒l���擾
Declare Function MIDIPart_GetPitchBend Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' MIDIPart_Get�n�֐�(���j�[�N�Ȃ���)

' ���݉�����Ă��錮�Ղ̐���Ԃ��B
Declare Function MIDIPart_GetNumNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݉�����Ă��錮�Ղ̐�(�z�[���h�����܂�)��Ԃ��B
Declare Function MIDIPart_GetNumNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݉�����Ă��錮�Ղōł��Ⴂ���K��Ԃ�(�Ȃ��ꍇ-1)�B
Declare Function MIDIPart_GetHighestNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݉�����Ă��錮��(�z�[���h�����܂�)�ōł��Ⴂ���K��Ԃ�(�Ȃ��ꍇ-1)�B
Declare Function MIDIPart_GetHighestNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݉�����Ă��錮�Ղōł��������K��Ԃ�(�Ȃ��ꍇ-1)�B
Declare Function MIDIPart_GetLowestNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' ���݉�����Ă��錮��(�z�[���h�����܂�)�ōł��������K��Ԃ�(�Ȃ��ꍇ-1)�B
Declare Function MIDIPart_GetLowestNoteKeep Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long) As Long

' MIDIPart_Set�n�֐�

' ���݂̃p�[�g���[�h��ݒ�
Declare Function MIDIPart_SetPartMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lPartMode As Long) As Long

' ���݂̃I���jON/OFF�A���m/�|�����[�h��ݒ�
Declare Function MIDIPart_SetOmniMonoPolyMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lOmniMonoPolyMode As Long) As Long

' ���݂̃`�����l���t�@�C���`���[�j���O�̒l��ݒ�
Declare Function MIDIPart_SetChannelFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lChannelFineTuning As Long) As Long

' ���݂̃`�����l���R�[�X�`���[�j���O�̒l��ݒ�
Declare Function MIDIPart_SetChannelCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lChannelCoarseTuning As Long) As Long

' ���݂̃s�b�`�x���h�Z���V�e�B�r�e�B�̒l��ݒ�
Declare Function MIDIPart_SetPitchBendSensitivity Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lPitchBendSensitivity As Long) As Long

' ���݂̃��W�����[�V�����f�v�X�����W�̒l��ݒ�
Declare Function MIDIPart_SetModulationDepthRange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lModulationDepthRange As Long)

' ���݂̃��V�[�u�`�����l���̒l��ݒ�
Declare Function MIDIPart_SetReseiveChannel Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lReceiveChannel As Long) As Long

' ���݂̃x���V�e�B�Z���X�f�v�X�̒l��ݒ�
Declare Function MIDIPart_SetVelocitySenseDepth Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVelocitySenseDepth As Long) As Long

' ���݂̃x���V�e�B�Z���X�I�t�Z�b�g�̒l��ݒ�
Declare Function MIDIPart_SetVelocitySenseOffset Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVelocitySenseOffset As Long) As Long

' ���݂̃L�[�{�[�h�����W���[�̒l��ݒ�
Declare Function MIDIPart_SetKeyboardRangeLow Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKeyboardRangeLow As Long) As Long

' ���݂̃L�[�{�[�h�����W�n�C�̒l��ݒ�
Declare Function MIDIPart_SetKeyboardRangeHigh Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKeyboardRangeHigh As Long) As Long

' ���݂̌��Ղ̉�������ݒ�
Declare Function MIDIPart_SetNote Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long, ByVal lVel As Long) As Long

' ���݂̃L�[�A�t�^�[�^�b�`�̒l��ݒ�
Declare Function MIDIPart_SetKeyAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̃R���g���[���`�F���W�̒l��ݒ�
Declare Function MIDIPart_SetControlChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' ���݂�RPNMSB�̒l��ݒ�
Declare Function MIDIPart_SetRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long, ByVal lVal As Long) As Long

' ���݂�RPNLSB�̒l��ݒ�
Declare Function MIDIPart_SetRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC101 As Long, ByVal lCC100 As Long, ByVal lVal As Long) As Long

' ���݂�NRPNMSB�̒l��ݒ�
Declare Function MIDIPart_SetNRPNMSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long, ByVal lVal As Long) As Long

' ���݂�NRPNLSB�̒l��ݒ�
Declare Function MIDIPart_SetNRPNLSB Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lCC99 As Long, ByVal lCC98 As Long, ByVal lVal As Long) As Long

' ���݂̃v���O�����`�F���W�̒l��ݒ�
Declare Function MIDIPart_SetProgramChange Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lNum As Long) As Long

' ���݂̃`�����l���A�t�^�[�^�b�`�̒l��ݒ�
Declare Function MIDIPart_SetChannelAfterTouch Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVal As Long) As Long

' ���݂̃s�b�`�x���h�̒l��ݒ�
Declare Function MIDIPart_SetPitchBend Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal lVal As Long) As Long


' MIDIPart�̏�������(�O���B���A���̊֐���MIDIStatus_Write����Ăяo�����)
Declare Function MIDIPart_Write Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long

' MIDIPart�̓ǂݍ���(�O���B���A���̊֐���MIDIStatus_Read����Ăяo�����)
Declare Function MIDIPart_Read Lib "MIDIStatus.dll" _
    (ByVal pMIDIPart As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long







'******************************************************************************
'*                                                                            *
'* MIDIDrumSetup�֐��Q                                                        *
'*                                                                            *
'******************************************************************************

' MIDIDrumSetup�I�u�W�F�N�g���폜����B
Declare Sub MIDIDrumSetup_Delete Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long)

' MIDIDrumSetup�I�u�W�F�N�g�𐶐�����B
Declare Function MIDIDrumSetup_Create Lib "MIDIStatus.dll" _
    (ByVal pParent As Long) As Long

' MIDIDrumSetup_Get�n�֐�

' ���݂̊y�킲�Ƃ̃J�b�g�I�t�t���[�P���V�[�̒l���擾
Declare Function MIDIDrumSetup_GetDrumCutoffFrequency Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃J�b�g�I�t�t���[�P���V�[�̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumCutoffFrequencyEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃��]�i���X�̒l���擾
Declare Function MIDIDrumSetup_GetDrumResonance Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃��]�i���X�̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumResonanceEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃A�^�b�N�^�C���̒l���擾
Declare Function MIDIDrumSetup_GetDrumAttackTime Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃A�^�b�N�^�C���̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumAttackTimeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��1�̒l���擾
Declare Function MIDIDrumSetup_GetDrumDecay1Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��1�̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumDecay1TimeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��2�̒l���擾
Declare Function MIDIDrumSetup_GetDrumDecay2Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��2�̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumDecay2TimeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃s�b�`�R�[�X�̒l���擾
Declare Function MIDIDrumSetup_GetDrumPitchCoarse Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃s�b�`�R�[�X�̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumPitchCoarseEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃s�b�`�t�@�C���̒l���擾
Declare Function MIDIDrumSetup_GetDrumPitchFine Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃s�b�`�t�@�C���̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumPitchFineEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃{�����[���̒l���擾
Declare Function MIDIDrumSetup_GetDrumVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃{�����[���̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumVolumeEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃p���̒l���擾
Declare Function MIDIDrumSetup_GetDrumPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃p���̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumPanEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃��o�[�u�̒l���擾
Declare Function MIDIDrumSetup_GetDrumReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃��o�[�u�̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumReverbEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃R�[���X�̒l���擾
Declare Function MIDIDrumSetup_GetDrumChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃R�[���X�̒l���܂Ƃ߂Ď擾
Declare Function MIDIDrumSetup_GetDrumChorusEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B���C�̒l���擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���)
Declare Function MIDIDrumSetup_GetDrumDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B���C�̒l���܂Ƃ߂Ď擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���)
Declare Function MIDIDrumSetup_GetDrumDelayEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByRef pBuf As Byte, ByVal lLen As Long) As Long

' MIDIDrumSetup_Set�n�֐�

' ���݂̊y�킲�Ƃ̃J�b�g�I�t�t���[�P���V�[�̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumCutoffFrequency Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃��]�i���X�̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumResonance Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃A�^�b�N�^�C���̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumAttackTime Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��1�̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumDecay1Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��2�̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumDecay2Time Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃s�b�`�R�[�X�̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumPitchCoarse Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃s�b�`�t�@�C���̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumPitchFine Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃{�����[���̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃p���̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃��o�[�u�̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃R�[���X�̒l��ݒ�
Declare Function MIDIDrumSetup_SetDrumChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long

' ���݂̊y�킲�Ƃ̃f�B���C�̒l��ݒ�(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���)
Declare Function MIDIDrumSetup_SetDrumDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal lKey As Long, ByVal lVal As Long) As Long


' MIDIDrumSetup�̏�������(�O���B���A���̊֐���MIDIStatus_Write����Ăяo�����)
Declare Function MIDIDrumSetup_Write Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long

' MIDIDrumSetup�̓ǂݍ���(�O���B���A���̊֐���MIDIStatus_Read����Ăяo�����)
Declare Function MIDIDrumSetup_Read Lib "MIDIStatus.dll" _
    (ByVal pMIDIDrumSetup As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long



'******************************************************************************
'*                                                                            *
'* MIDIStatus�֐��Q                                                           *
'*                                                                            *
'******************************************************************************

' MIDIStatus�I�u�W�F�N�g���폜����B
Declare Sub MIDIStatus_Delete Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long)

' MIDIStatus�I�u�W�F�N�g�𐶐�����B
Declare Function MIDIStatus_Create Lib "MIDIStatus.dll" _
    (ByVal lModuleMode As Long, ByVal lNumMIDIPart As Long, ByVal lNumMIDIDrumSetup As Long) As Long

' MIDIStatus_Get�n�֐�

' ���݂̃��W���[�����[�h(NATIVE/GM/GM2/GS/88/XG)���擾
Declare Function MIDIStatus_GetModuleMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' ���݂̃}�X�^�[�`���[���̒l���擾
Declare Function MIDIStatus_GetMasterFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' ���݂̃}�X�^�[�L�[�V�t�g�̒l���擾
Declare Function MIDIStatus_GetMasterCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' ���݂̃}�X�^�[�{�����[���̒l���擾
Declare Function MIDIStatus_GetMasterVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' ���݂̃}�X�^�[�p���̒l���擾
Declare Function MIDIStatus_GetMasterPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' ���݂̃}�X�^�[���o�[�u�̒l��1�擾
Declare Function MIDIStatus_GetMasterReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' ���݂̃}�X�^�[���o�[�u�̒l���܂Ƃ߂Ď擾
Declare Function MIDIStatus_GetMasterReverbEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' ���݂̃}�X�^�[�R�[���X�̒l��1�擾
Declare Function MIDIStatus_GetMasterChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' ���݂̃}�X�^�[�R�[���X�̒l���܂Ƃ߂Ď擾
Declare Function MIDIStatus_GetMasterChorusEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' ���݂̃}�X�^�[�f�B���C�̒l��1�擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���)
Declare Function MIDIStatus_GetMasterDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' ���݂̃}�X�^�[�f�B���C�̒l���܂Ƃ߂Ď擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���)
Declare Function MIDIStatus_GetMasterDelayEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' ���݂̃}�X�^�[�C�R���C�U�̒l��1�擾
Declare Function MIDIStatus_GetMasterEqualizer Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long) As Long

' ���݂̃}�X�^�[�C�R���C�U�̒l���܂Ƃ߂Ď擾
Declare Function MIDIStatus_GetMasterEqualizerEx Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pBuf As Long, ByVal lLen As Long) As Long

' ����MIDIStatus�I�u�W�F�N�g�Ɋ܂܂��MIDIPart�̐����擾
Declare Function MIDIStatus_GetNumMIDIPart Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' ����MIDIStatus�I�u�W�F�N�g�Ɋ܂܂��MIDIDrumSetup�̐����擾
Declare Function MIDIStatus_GetNumMIDIDrumSetup Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' �eMIDIPart�ւ̃|�C���^���擾
Declare Function MIDIStatus_GetMIDIPart Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lIndex) As Long

' �eMIDIDrumSetup�ւ̃|�C���^���擾
Declare Function MIDIStatus_GetMIDIDrumSetup Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lIndex) As Long

' MIDIStatus_Set�n�֐�

' ���W���[�����[�h(NATIVE/GM/GM2/GS/88/XG)��ݒ肷��B
' ���W���[�����[�h�̐ݒ�ɏ]�����ׂẴp�����[�^���f�t�H���g�l�ɐݒ肷��B
Declare Function MIDIStatus_SetModuleMode Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lModuleMode As Long) As Long

' ���݂̃}�X�^�[�t�@�C���`���[�j���O�̒l��ݒ�
Declare Function MIDIStatus_SetMasterFineTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterFineTuning As Long) As Long

' ���݂̃}�X�^�[�R�[�X�`���[�j���O�̒l��ݒ�
Declare Function MIDIStatus_SetMasterCoarseTuning Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterCoarseTuning As Long) As Long

' ���݂̃}�X�^�[�{�����[���̒l��ݒ�
Declare Function MIDIStatus_SetMasterVolume Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterVolume As Long) As Long

' ���݂̃}�X�^�[�p���̒l��ݒ�
Declare Function MIDIStatus_SetMasterPan Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterPan As Long) As Long

' ���݂̃}�X�^�[���o�[�u�̒l��1�ݒ�
Declare Function MIDIStatus_SetMasterReverb Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' �}�X�^�[���o�[�u�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B
Declare Function MIDIStatus_SetMasterReverbType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterReverbType As Long) As Long

' ���݂̃}�X�^�[�R�[���X�̒l��1�ݒ�
Declare Function MIDIStatus_SetMasterChorus Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum, ByVal lVal) As Long

' �}�X�^�[�R�[���X�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B
Declare Function MIDIStatus_SetMasterChorusType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterChorusType As Long) As Long

' ���݂̃}�X�^�[�f�B���C�̒l��1�ݒ�(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���)
Declare Function MIDIStatus_SetMasterDelay Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' �}�X�^�[�f�B���C�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B
' (XG�ł̓}�X�^�[���@���G�[�V�����ƂȂ�)
Declare Function MIDIStatus_SetMasterDelayType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterDelayType As Long) As Long

' ���݂̃}�X�^�[�C�R���C�U�̒l��1�ݒ�
Declare Function MIDIStatus_SetMasterEqualizer Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lNum As Long, ByVal lVal As Long) As Long

' �}�X�^�[�C�R���C�U�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B
Declare Function MIDIStatus_SetMasterEqualizerType Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal lMasterEqualizerType As Long) As Long

' MIDIStatus_Put�n�֐�

' ���Z�b�g
Declare Function MIDIStatus_PutReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' GM���Z�b�g
Declare Function MIDIStatus_PutGMReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' GM2���Z�b�g
Declare Function MIDIStatus_PutGM2Reset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' GS���Z�b�g
Declare Function MIDIStatus_PutGSReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' 88���Z�b�g
Declare Function MIDIStatus_Put88Reset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' XG���Z�b�g
Declare Function MIDIStatus_PutXGReset Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long) As Long

' MIDI���b�Z�[�W��^���邱�Ƃɂ��p�����[�^��ύX����B
Declare Function MIDIStatus_PutMIDIMessage Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByRef pMIDIMessage As Byte, ByVal lLen As Long) As Long



' MIDIStatus�̏�������(�O���B���A���̊֐���MIDIStatus_Save����Ăяo�����)
Declare Function MIDIStatus_Write Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal pFile As Long, ByVal lVersion As Long) As Long

' MIDIStatus�̕ۑ�
Declare Function MIDIStatus_Save Lib "MIDIStatus.dll" Alias "MIDIStatus_SaveA" _
    (ByVal pMIDIStatus As Long, ByVal pszFileName As String) As Long

' MIDIStatus�̓ǂݍ���(�O���B���A���̊֐���MIDIStatus_Load����Ăяo�����)
Declare Function MIDIStatus_Read Lib "MIDIStatus.dll" _
    (ByVal pMIDIStatus As Long, ByVal pFile, ByVal lVersion As Long) As Long

' MIDIStatus�̃��[�h
Declare Function MIDIStatus_Load Lib "MIDIStatus.dll" Alias "MIDIStatus_LoadA" _
    (ByVal pszFileName As String) As Long





/******************************************************************************/
/*                                                                            */
/*�@MIDIStatus.h - MIDIStatus�w�b�_�[�t�@�C��              (C)2002-2013 ����  */
/*                                                                            */
/******************************************************************************/

/* ���̃��W���[���͕��ʂ̂b����ŏ�����Ă���B */
/* ���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��z�z�����B */
/* �v���W�F�N�g�z�[���y�[�W�F"http://openmidiproject.sourceforge.jp/index.html" */

/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef _MIDISTATUS_H_
#define _MIDISTATUS_H_

#define MIDISTATUS_MAXMIDIPART      128
#define MIDISTATUS_MAXMIDIDRUMSETUP 128

#define MIDISTATUS_MODENATIVE 0x00000000
#define MIDISTATUS_MODEGM     0x7E000001
#define MIDISTATUS_MODEGM2    0x7E000003
#define MIDISTATUS_MODEGS     0x41000002
#define MIDISTATUS_MODE88     0x41000003
#define MIDISTATUS_MODEXG     0x43000002
	/* #define MIDISTATUS_MODE????��ǉ�����ꍇ�́A */
	/* ���24�r�b�g��MMA���犄�蓖�Ă�ꂽ���[�J�[�R�[�h�Ƃ��A */
	/* ����8�r�b�g�͊e���[�J�[���̎��R�Ȏ��ʃR�[�h�Ƃ���B */

/* C++������g�p�\�Ƃ��� */
#ifdef __cplusplus
extern "C" {
#endif

/* MIDIPart�\���� */
/* �R�����g��< >�͈�ʓI�ȃf�t�H���g�l������ */
typedef struct tagMIDIPart {
	void* m_pParent;
	long m_lPartMode;             /* 0=�m�[�}��,1=�h����1,2=�h����2,3=�h����3,4=�h����4 */
	long m_lOmniMonoPolyMode;     /* 1=OmniOn_Poly�A2=OmniOn_Mono�A<3>=OmniOff_Poly�A4=OmniOff_Mono */
	long m_lChannelFineTuning;    /* 0�`<8192>�`16383 100/8192cent�P�� RPN#1�Őݒ� */
	long m_lChannelCoarseTuning;  /* 40�`<64>�`88 �����P�� �`�����l���L�[�V�t�g�Ƃ����� RPN#2�Őݒ� */
	long m_lPitchBendSensitivity; /* 0�`<2>�`24 �����P�� RPN#0�Őݒ� */
	long m_lModulationDepthRange; /* 0�`???? RPN#5�Őݒ� */
	long m_lReceiveChannel;       /* 0�`15,16 �f�t�H���g�l�̓p�[�g�ԍ��Ɉ�v SysEx�Őݒ�(GS/88/XG�̂�) */
	long m_lVelocitySenseDepth;   /* 0�`<64>�`127 SysEx�Őݒ�(GS/88/XG�̂�) */
	long m_lVelocitySenseOffset;  /* 0�`<64>�`127 SysEx�Őݒ�(GS/88/XG�̂�) */
	long m_lKeyboardRangeLow;     /* �m�[�g���~�b�g���[�Ƃ����� <0>�`127 SysEx�Őݒ�(GS/88/XG�̂�) */
	long m_lKeyboardRangeHigh;    /* �m�[�g���~�b�g�n�C�Ƃ����� 0�`<127> SysEx�Őݒ�(GS/88/XG�̂�) */
	long m_lAssignableControler1Num; /* �A�T�C�i�u���R���g���[���[1�Ɏg�p����R���g���[���i���o�[ */
	long m_lAssignableControler2Num; /* �A�T�C�i�u���R���g���[���[2�Ɏg�p����R���g���[���i���o�[ */
	long m_lReserved1;            /* �����g���p1(����Ɏg���Ă͂Ȃ�Ȃ�) */
	long m_lReserved2;            /* �����g���p2(����Ɏg���Ă͂Ȃ�Ȃ�) */
	long m_lScaleOctaveTuning[12]; /* 0�`<64>�`127 0.1cent�P�� */
	/* �R���g���[���[�f�B�X�e�B�l�[�V�����Z�b�e�B���O */
	unsigned char m_cModulationDest[16];           /* ���W�����[�V�������g�������̌��ʒ�` */
	unsigned char m_cPitchBendDest[16];            /* �s�b�`�x���h���g�������̌��ʒ�` */
	unsigned char m_cChannelAfterTouchDest[16];    /* �`�����l���A�t�^�[�^�b�`���g�������̌��ʒ�` */
	unsigned char m_cKeyAfterTouchDest[16];        /* �L�[�A�t�^�[�^�b�`���g�������̌��ʒ�` */
	unsigned char m_cAssignableControler1Dest[16]; /* �A�T�C�i�u���R���g���[���[1���g�������̌��ʒ�` */
	unsigned char m_cAssignableControler2Dest[16]; /* �A�T�C�i�u���R���g���[���[2���g�������̌��ʒ�` */
	/* �`�����l���{�C�X�̈� */
	unsigned char m_cNote[128];          /* �e�X<0>�`127 �l�͑Ō����̃x���V�e�B */
	unsigned char m_cNoteKeep[128];      /* �e�X<0>�`127 �l�͑Ō����̃x���V�e�B */
	unsigned char m_cKeyAfterTouch[128]; /* �e�X<0>�`127 */
	unsigned char m_cControlChange[128]; /* �e�X0�`127 �f�t�H���g�l��CC#�ɂ��B */
	long m_lProgramChange;        /* <0>(AcousticGrandPiano)�`127(Gunshot) */
	long m_lChannelAfterTouch;    /* <0>�`127 */
	long m_lPitchBend;            /* 0�`<8192>�`16383 */
	/* ���[�U�[�p�g���̈� */
	long m_lUser1;
	long m_lUser2;
	long m_lUser3;
	long m_lUserFlag;
	/* RPN/NRPN���S�ۑ��̈�(���ʂ͕ۑ����Ȃ�) */
#ifdef MIDISTATUS_RECORDNRPN
	unsigned char m_cNRPNMSB[128][128];
	unsigned char m_cNRPNLSB[128][128];
#endif
#ifdef MIDISTATUS_RECORDRPN
	unsigned char m_cRPNMSB[128][128];
	unsigned char m_cRPNLSB[128][128];
#endif
} MIDIPart;


/* MIDIDrumSetup�\���� */
/* �R�����g��< >�͈�ʓI�ȃf�t�H���g�l������ */
typedef struct tagMIDIDrumSetup {
	void* m_pParent;
	unsigned char m_cDrumCutoffFrequency[128]; /* �e�X0�`<64>�`127 NRPN#(20*128)�䖔��SysEx�Őݒ�(XG) */
	unsigned char m_cDrumResonance[128];       /* �e�X0�`<64>�`127 NRPN#(21*128)�䖔��SysEx�Őݒ�(XG) */
	unsigned char m_cDrumAttackTime[128];      /* �e�X0�`<64>�`127 NRPN#(22*128)�䖔��SysEx�Őݒ�(XG) */
	unsigned char m_cDrumDecay1Time[128];      /* �e�X0�`<64>�`127 NRPN#(23*128)�䖔��SysEx�Őݒ�(XG) */
	unsigned char m_cDrumDecay2Time[128];      /* �e�X0�`<64>�`127 NRPN#(23*128)�䖔��SysEx�Őݒ�(XG) */
	unsigned char m_cDrumPitchCoarse[128];     /* �e�X0�`<64>�`127 NRPN#(24*128)�䖔��SysEx�Őݒ�(GS/88/XG) */
	unsigned char m_cDrumPitchFine[128];       /* �e�X0�`<64>�`127 NRPN#(25*128)�䖔��SysEx�Őݒ�(XG) */
	unsigned char m_cDrumVolume[128];          /* �e�X0�`<64>�`127 NRPN#(26*128)�䖔��SysEx�Őݒ�(GS/88/XG/GM2) */
	unsigned char m_cDrumPan[128];             /* �e�X0�`<64>�`127 NRPN#(28*128)�䖔��SysEx�Őݒ�(GS/88/XG/GM2) */
	unsigned char m_cDrumReverb[128];          /* �e�X0�`<64>�`127 NRPN#(29*128)�䖔��SysEx�Őݒ�(GS/88/XG/GM2) */
	unsigned char m_cDrumChorus[128];          /* �e�X0�`<64>�`127 NRPN#(30*128)�䖔��SysEx�Őݒ�(GS/88/XG/GM2) */
	unsigned char m_cDrumDelay[128];           /* �e�X0�`<64>�`127 NRPN#(31*128)�䖔��SysEx�Őݒ�(GS/88/XG) */
	long m_lUser1;
	long m_lUser2;
	long m_lUser3;
	long m_lUserFlag;
} MIDIDrumSetup;



/* MIDIStatus�\���� */
/* �R�����g��< >�͈�ʓI�ȃf�t�H���g�l������ */
typedef struct tagMIDIStatus {
	long m_lModuleMode;           /* 0x00000000=NATIVE, 0x7E000002=GM,   0x7E000003=GM2, */
	                              /* 0x41000002=GS,   0x41000003=88,   0x43000002=XG */
	long m_lMasterFineTuning;     /* 24�`<1024>�`2024(GS), 0�`<1024>�`2048(XG), 0�`<8192>�`16383(���̑�) */
	long m_lMasterCoarseTuning;   /* 40�`<64>�`88 �����P�� �}�X�^�[�L�[�V�t�g�Ƃ����� */
	long m_lMasterBalance;        /* ���̒l�͎g���Ȃ��B */
	long m_lMasterVolume;         /* 0�`<127> SysEx�Őݒ� */
	long m_lMasterPan;            /* 1�`<64>�`127 SysEx�Őݒ� */
	long m_lMasterReverb[32];     /* �e�X�̈Ӗ��Ɣ͈͂�m_lMode�̒l�ɂ��قȂ�(�����K�C�h�u�b�N�Q��) */
	long m_lMasterChorus[32];     /* �e�X�̈Ӗ��Ɣ͈͂�m_lMode�̒l�ɂ��قȂ�(�����K�C�h�u�b�N�Q��) */
	long m_lMasterDelay[32];      /* �e�X�̈Ӗ��Ɣ͈͂�m_lMode�̒l�ɂ��قȂ�(�����K�C�h�u�b�N�Q��) */
	long m_lMasterEqualizer[32];  /* �e�X�̈Ӗ��Ɣ͈͂�m_lMode�̒l�ɂ��قȂ�(�����K�C�h�u�b�N�Q��) */
	long m_lMasterInsertion[32];  /* ���̒l�͎g���Ȃ��B */
	long m_lNumMIDIPart;          /* MIDIPart�̐�(=16) */
	long m_lNumMIDIDrumSetup;     /* MIDIDrumSetup�̐�(=2or4) */
	MIDIPart* m_pMIDIPart[MIDISTATUS_MAXMIDIPART];
	MIDIDrumSetup* m_pMIDIDrumSetup[MIDISTATUS_MAXMIDIDRUMSETUP];
	long m_lUser1;
	long m_lUser2;
	long m_lUser3;
	long m_lUserFlag;
	long m_lRunningStatus;
} MIDIStatus;




/******************************************************************************/
/*                                                                            */
/* MIDIPart�֐��Q                                                             */
/*                                                                            */
/******************************************************************************/

/* MIDIPart�I�u�W�F�N�g���폜����B */
int __stdcall MIDIPart_Delete (MIDIPart* pMIDIPart);

/* MIDIPart�I�u�W�F�N�g�𐶐�����B */
MIDIPart* __stdcall MIDIPart_Create (MIDIStatus* pParent);

/* MIDIPart_Get�n�֐� */

/* ���݂̃p�[�g���[�h���擾 */
long __stdcall MIDIPart_GetPartMode (MIDIPart* pMIDIPart);

/* ���݂̃I���j���m�|�����[�h���擾 */
long __stdcall MIDIPart_GetOmniMonoPolyMode (MIDIPart* pMIDIPart);

/* ���݂̃`�����l���t�@�C���`���[�j���O�̒l���擾 */
long __stdcall MIDIPart_GetChannelFineTuning (MIDIPart* pMIDIPart);

/* ���݂̃`�����l���R�[�X�`���[�j���O�̒l���擾 */
long __stdcall MIDIPart_GetChannelCoarseTuning (MIDIPart* pMIDIPart);

/* ���݂̃s�b�`�x���h�Z���V�e�B�r�e�B�̒l���擾 */
long __stdcall MIDIPart_GetPitchBendSensitivity (MIDIPart* pMIDIPart);

/* ���݂̃��W�����[�V�����f�v�X�����W�̒l���擾 */
long __stdcall MIDIPart_GetModulationDepthRange (MIDIPart* pMIDIPart);

/* ���݂̃��V�[�u�`�����l���̒l���擾 */
long __stdcall MIDIPart_GetReceiveChannel (MIDIPart* pMIDIPart);

/* ���݂̃x���V�e�B�Z���X�f�v�X�̒l���擾 */
long __stdcall MIDIPart_GetVelocitySenseDepth (MIDIPart* pMIDIPart);

/* ���݂̃x���V�e�B�Z���X�I�t�Z�b�g�̒l���擾 */
long __stdcall MIDIPart_GetVelocitySenseOffset (MIDIPart* pMIDIPart);

/* ���݂̃L�[�{�[�h�����W���[�̒l���擾 */
long __stdcall MIDIPart_GetKeyboardRangeLow (MIDIPart* pMIDIPart);

/* ���݂̃L�[�{�[�h�����W�n�C�̒l���擾 */
long __stdcall MIDIPart_GetKeyboardRangeHigh (MIDIPart* pMIDIPart);

/* ���݂̌��Ղ̉��������擾����B */
/* �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B) */
long __stdcall MIDIPart_GetNote (MIDIPart* pMIDIPart, long lKey);

/* ���݂̌��Ղ̉��������܂Ƃ߂Ď擾����B */
/* �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B) */
long __stdcall MIDIPart_GetNoteEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* ���݂̌��Ղ̉��������擾����B */
/* �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B) */
long __stdcall MIDIPart_GetNoteKeep (MIDIPart* pMIDIPart, long lKey);

/* ���݂̌��Ղ̉�����(�z�[���h�����܂�)���܂Ƃ߂Ď擾����B */
/* �e���K���A0=������Ă��Ȃ��A1�`127=������Ă���(�������Ƃ��̃x���V�e�B) */
long __stdcall MIDIPart_GetNoteKeepEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* ���݂̃L�[�A�t�^�[�^�b�`�̒l���擾 */
long __stdcall MIDIPart_GetKeyAfterTouch (MIDIPart* pMIDIPart, long lKey);

/* ���݂̃L�[�A�t�^�[�^�b�`�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIPart_GetKeyAfterTouchEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* ���݂̃R���g���[���`�F���W�̒l���擾 */
long __stdcall MIDIPart_GetControlChange (MIDIPart* pMIDIPart, long lNum);

/* ���݂̃R���g���[���`�F���W�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIPart_GetControlChangeEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen);

/* ���݂�RPNMSB�̒l���擾 */
long __stdcall MIDIPart_GetRPNMSB (MIDIPart* pMIDIPart, long lCC101, long lCC100);

/* ���݂�RPNLSB�̒l���擾 */
long __stdcall MIDIPart_GetRPNLSB (MIDIPart* pMIDIPart, long lCC101, long lCC100);

/* ���݂�NRPNMSB�̒l���擾 */
long __stdcall MIDIPart_GetNRPNMSB (MIDIPart* pMIDIPart, long lCC99, long lCC98);

/* ���݂�NRPNLSB�̒l���擾 */
long __stdcall MIDIPart_GetNRPNLSB (MIDIPart* pMIDIPart, long lCC99, long lCC98);

/* ���݂̃v���O�����`�F���W�̒l���擾 */
long __stdcall MIDIPart_GetProgramChange (MIDIPart* pMIDIPart);

/* ���݂̃`�����l���A�t�^�[�^�b�`�̒l���擾 */
long __stdcall MIDIPart_GetChannelAfterTouch (MIDIPart* pMIDIPart);

/* ���݂̃s�b�`�x���h�̒l���擾 */
long __stdcall MIDIPart_GetPitchBend (MIDIPart* pMIDIPart);

/* MIDIPart_Get�n�֐�(���j�[�N�Ȃ���) */

/* ���݉�����Ă��錮�Ղ̐���Ԃ��B */
long __stdcall MIDIPart_GetNumNote (MIDIPart* pMIDIPart);

/* ���݉�����Ă��錮�Ղ̐�(�z�[���h�����܂�)��Ԃ��B */
long __stdcall MIDIPart_GetNumNoteKeep (MIDIPart* pMIDIPart);

/* ���݉�����Ă��錮�Ղōł��Ⴂ���K��Ԃ�(�Ȃ��ꍇ-1)�B */
long __stdcall MIDIPart_GetHighestNote (MIDIPart* pMIDIPart);

/* ���݉�����Ă��錮��(�z�[���h�����܂�)�ōł��Ⴂ���K��Ԃ�(�Ȃ��ꍇ-1)�B */
long __stdcall MIDIPart_GetHighestNoteKeep (MIDIPart* pMIDIPart);

/* ���݉�����Ă��錮�Ղōł��������K��Ԃ�(�Ȃ��ꍇ-1)�B */
long __stdcall MIDIPart_GetLowestNote (MIDIPart* pMIDIPart);

/* ���݉�����Ă��錮��(�z�[���h�����܂�)�ōł��������K��Ԃ�(�Ȃ��ꍇ-1)�B */
long __stdcall MIDIPart_GetLowestNoteKeep (MIDIPart* pMIDIPart);

/* MIDIPart_Set�n�֐� */

/* ���݂̃p�[�g���[�h��ݒ� */
long __stdcall MIDIPart_SetPartMode (MIDIPart* pMIDIPart, long lPartMode);

/* ���݂̃I���jON/OFF�A���m/�|�����[�h��ݒ� */
long __stdcall MIDIPart_SetOmniMonoPolyMode (MIDIPart* pMIDIPart, long lOmniMonoPolyMode);

/* ���݂̃`�����l���t�@�C���`���[�j���O�̒l��ݒ� */
long __stdcall MIDIPart_SetChannelFineTuning (MIDIPart* pMIDIPart, long lChannelFineTuning);

/* ���݂̃`�����l���R�[�X�`���[�j���O�̒l��ݒ� */
long __stdcall MIDIPart_SetChannelCoarseTuning (MIDIPart* pMIDIPart, long lChannelCoarseTuning);

/* ���݂̃s�b�`�x���h�Z���V�e�B�r�e�B�̒l��ݒ� */
long __stdcall MIDIPart_SetPitchBendSensitivity (MIDIPart* pMIDIPart, long lPitchBendSensitivity);

/* ���݂̃��W�����[�V�����f�v�X�����W�̒l��ݒ� */
long __stdcall MIDIPart_SetModulationDepthRange (MIDIPart* pMIDIPart, long lModulationDepthRange);

/* ���݂̃��V�[�u�`�����l���̒l��ݒ� */
long __stdcall MIDIPart_SetReseiveChannel (MIDIPart* pMIDIPart, long lReceiveChannel);

/* ���݂̃x���V�e�B�Z���X�f�v�X�̒l��ݒ� */
long __stdcall MIDIPart_SetVelocitySenseDepth (MIDIPart* pMIDIPart, long lVelocitySenseDepth);

/* ���݂̃x���V�e�B�Z���X�I�t�Z�b�g�̒l��ݒ� */
long __stdcall MIDIPart_SetVelocitySenseOffset (MIDIPart* pMIDIPart, long lVelocitySenseOffset);

/* ���݂̃L�[�{�[�h�����W���[�̒l��ݒ� */
long __stdcall MIDIPart_SetKeyboardRangeLow (MIDIPart* pMIDIPart, long lKeyboardRangeLow);

/* ���݂̃L�[�{�[�h�����W�n�C�̒l��ݒ� */
long __stdcall MIDIPart_SetKeyboardRangeHigh (MIDIPart* pMIDIPart, long lKeyboardRangeHigh);

/* ���݂̌��Ղ̉�������ݒ� */
long __stdcall MIDIPart_SetNote (MIDIPart* pMIDIPart, long lKey, long lVel);

/* ���݂̃L�[�A�t�^�[�^�b�`�̒l��ݒ� */
long __stdcall MIDIPart_SetKeyAfterTouch (MIDIPart* pMIDIPart, long lKey, long lVal);

/* ���݂̃R���g���[���`�F���W�̒l��ݒ� */
long __stdcall MIDIPart_SetControlChange (MIDIPart* pMIDIPart, long lNum, long lVal);

/* ���݂�RPNMSB�̒l��ݒ� */
long __stdcall MIDIPart_SetRPNMSB (MIDIPart* pMIDIPart, long lCC101, long lCC100, long lVal);

/* ���݂�RPNLSB�̒l��ݒ� */
long __stdcall MIDIPart_SetRPNLSB (MIDIPart* pMIDIPart, long lCC101, long lCC100, long lVal);

/* ���݂�NRPNMSB�̒l��ݒ� */
long __stdcall MIDIPart_SetNRPNMSB (MIDIPart* pMIDIPart, long lCC99, long lCC98, long lVal);

/* ���݂�NRPNLSB�̒l��ݒ� */
long __stdcall MIDIPart_SetNRPNLSB (MIDIPart* pMIDIPart, long lCC99, long lCC98, long lVal);

/* ���݂̃v���O�����`�F���W�̒l��ݒ� */
long __stdcall MIDIPart_SetProgramChange (MIDIPart* pMIDIPart, long lNum);

/* ���݂̃`�����l���A�t�^�[�^�b�`�̒l��ݒ� */
long __stdcall MIDIPart_SetChannelAfterTouch (MIDIPart* pMIDIPart, long lVal);

/* ���݂̃s�b�`�x���h�̒l��ݒ� */
long __stdcall MIDIPart_SetPitchBend (MIDIPart* pMIDIPart, long lVal);


/* MIDIPart�̏�������(�O���B���A���̊֐���MIDIStatus_Write����Ăяo�����) */
long __stdcall MIDIPart_Write (MIDIPart* pMIDIPart, FILE* pFile, long lVersion);

/* MIDIPart�̓ǂݍ���(�O���B���A���̊֐���MIDIStatus_Read����Ăяo�����) */
long __stdcall MIDIPart_Read (MIDIPart* pMIDIPart, FILE* pFile, long lVersion);



/******************************************************************************/
/*                                                                            */
/* MIDIDrumSetup�֐��Q                                                        */
/*                                                                            */
/******************************************************************************/

/* MIDIDrumSetup�I�u�W�F�N�g���폜����B */
int __stdcall MIDIDrumSetup_Delete (MIDIDrumSetup* pMIDIDrumSetup);

/* MIDIDrumSetup�I�u�W�F�N�g�𐶐�����B */
MIDIDrumSetup* __stdcall MIDIDrumSetup_Create (MIDIStatus* pParent);

/* MIDIDrumSetup_Get�n�֐� */

/* ���݂̊y�킲�Ƃ̃J�b�g�I�t�t���[�P���V�[�̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumCutoffFrequency (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃J�b�g�I�t�t���[�P���V�[�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumCutoffFrequencyEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃��]�i���X�̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumResonance (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃��]�i���X�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumResonanceEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃A�^�b�N�^�C���̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumAttackTime (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃A�^�b�N�^�C���̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumAttackTimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��1�̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumDecay1Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��1�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumDecay1TimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��2�̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumDecay2Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��2�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumDecay2TimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃s�b�`�R�[�X�̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumPitchCoarse (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃s�b�`�R�[�X�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumPitchCoarseEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃s�b�`�t�@�C���̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumPitchFine (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃s�b�`�t�@�C���̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumPitchFineEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃{�����[���̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumVolume (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃{�����[���̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumVolumeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃p���̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumPan (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃p���̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumPanEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃��o�[�u�̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumReverb (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃��o�[�u�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumReverbEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃R�[���X�̒l���擾 */
long __stdcall MIDIDrumSetup_GetDrumChorus (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃R�[���X�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIDrumSetup_GetDrumChorusEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* ���݂̊y�킲�Ƃ̃f�B���C�̒l���擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���) */
long __stdcall MIDIDrumSetup_GetDrumDelay (MIDIDrumSetup* pMIDIDrumSetup, long lKey);

/* ���݂̊y�킲�Ƃ̃f�B���C�̒l���܂Ƃ߂Ď擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���) */
long __stdcall MIDIDrumSetup_GetDrumDelayEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen);

/* MIDIDrumSetup_Set�n�֐� */

/* ���݂̊y�킲�Ƃ̃J�b�g�I�t�t���[�P���V�[�̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumCutoffFrequency (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃��]�i���X�̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumResonance (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃A�^�b�N�^�C���̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumAttackTime (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��1�̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumDecay1Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃f�B�P�C�^�C��2�̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumDecay2Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃s�b�`�R�[�X�̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumPitchCoarse (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃s�b�`�t�@�C���̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumPitchFine (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃{�����[���̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumVolume (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃p���̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumPan (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃��o�[�u�̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumReverb (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃R�[���X�̒l��ݒ� */
long __stdcall MIDIDrumSetup_SetDrumChorus (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);

/* ���݂̊y�킲�Ƃ̃f�B���C�̒l��ݒ�(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���) */
long __stdcall MIDIDrumSetup_SetDrumDelay (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal);


/* MIDIDrumSetup�̏�������(�O���B���A���̊֐���MIDIStatus_Write����Ăяo�����) */
long __stdcall MIDIDrumSetup_Write (MIDIDrumSetup* pMIDIDrumSetup, FILE* pFile, long lVersion);

/* MIDIDrumSetup�̓ǂݍ���(�O���B���A���̊֐���MIDIStatus_Read����Ăяo�����) */
long __stdcall MIDIDrumSetup_Read (MIDIDrumSetup* pMIDIDrumSetup, FILE* pFile, long lVersion);



/******************************************************************************/
/*                                                                            */
/* MIDIStatus�֐��Q                                                           */
/*                                                                            */
/******************************************************************************/

/* MIDIStatus�I�u�W�F�N�g���폜����B */
void __stdcall MIDIStatus_Delete (MIDIStatus* pMIDIStatus);

/* MIDIStatus�I�u�W�F�N�g�𐶐�����B */
MIDIStatus* __stdcall MIDIStatus_Create (long lModuleMode, long lNumMIDIPart, long lNumMIDIDrumSetup);

/* MIDIStatus_Get�n�֐� */

/* ���݂̃��W���[�����[�h(NATIVE/GM/GM2/GS/88/XG)���擾 */
long __stdcall MIDIStatus_GetModuleMode (MIDIStatus* pMIDIStatus);

/* ���݂̃}�X�^�[�`���[���̒l���擾 */
long __stdcall MIDIStatus_GetMasterFineTuning (MIDIStatus* pMIDIStatus);

/* ���݂̃}�X�^�[�L�[�V�t�g�̒l���擾 */
long __stdcall MIDIStatus_GetMasterCoarseTuning (MIDIStatus* pMIDIStatus);

/* ���݂̃}�X�^�[�{�����[���̒l���擾 */
long __stdcall MIDIStatus_GetMasterVolume (MIDIStatus* pMIDIStatus);

/* ���݂̃}�X�^�[�p���̒l���擾 */
long __stdcall MIDIStatus_GetMasterPan (MIDIStatus* pMIDIStatus);

/* ���݂̃}�X�^�[���o�[�u�̒l��1�擾 */
long __stdcall MIDIStatus_GetMasterReverb (MIDIStatus* pMIDIStatus, long lNum);

/* ���݂̃}�X�^�[���o�[�u�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIStatus_GetMasterReverbEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* ���݂̃}�X�^�[�R�[���X�̒l��1�擾 */
long __stdcall MIDIStatus_GetMasterChorus (MIDIStatus* pMIDIStatus, long lNum);

/* ���݂̃}�X�^�[�R�[���X�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIStatus_GetMasterChorusEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* ���݂̃}�X�^�[�f�B���C�̒l��1�擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���) */
long __stdcall MIDIStatus_GetMasterDelay (MIDIStatus* pMIDIStatus, long lNum);

/* ���݂̃}�X�^�[�f�B���C�̒l���܂Ƃ߂Ď擾(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���) */
long __stdcall MIDIStatus_GetMasterDelayEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* ���݂̃}�X�^�[�C�R���C�U�̒l��1�擾 */
long __stdcall MIDIStatus_GetMasterEqualizer (MIDIStatus* pMIDIStatus, long lNum);

/* ���݂̃}�X�^�[�C�R���C�U�̒l���܂Ƃ߂Ď擾 */
long __stdcall MIDIStatus_GetMasterEqualizerEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen);

/* ����MIDIStatus�I�u�W�F�N�g�Ɋ܂܂��MIDIPart�̐����擾 */
long __stdcall MIDIStatus_GetNumMIDIPart (MIDIStatus* pMIDIStatus);

/* ����MIDIStatus�I�u�W�F�N�g�Ɋ܂܂��MIDIDrumSetup�̐����擾 */
long __stdcall MIDIStatus_GetNumMIDIDrumSetup (MIDIStatus* pMIDIStatus);

/* �eMIDIPart�ւ̃|�C���^���擾 */
MIDIPart* __stdcall MIDIStatus_GetMIDIPart (MIDIStatus* pMIDIStatus, long lIndex);

/* �eMIDIDrumSetup�ւ̃|�C���^���擾 */
MIDIDrumSetup* __stdcall MIDIStatus_GetMIDIDrumSetup (MIDIStatus* pMIDIStatus, long lIndex);

/* MIDIStatus_Set�n�֐� */

/* ���W���[�����[�h(NATIVE/GM/GM2/GS/88/XG)��ݒ肷��B */
/* ���W���[�����[�h�̐ݒ�ɏ]�����ׂẴp�����[�^���f�t�H���g�l�ɐݒ肷��B */
long __stdcall MIDIStatus_SetModuleMode (MIDIStatus* pMIDIStatus, long lModuleMode);

/* ���݂̃}�X�^�[�t�@�C���`���[�j���O�̒l��ݒ� */
long __stdcall MIDIStatus_SetMasterFineTuning (MIDIStatus* pMIDIStatus, long lMasterFineTuning);

/* ���݂̃}�X�^�[�R�[�X�`���[�j���O�̒l��ݒ� */
long __stdcall MIDIStatus_SetMasterCoarseTuning (MIDIStatus* pMIDIStatus, long lMasterCoarseTuning);

/* ���݂̃}�X�^�[�{�����[���̒l��ݒ� */
long __stdcall MIDIStatus_SetMasterVolume (MIDIStatus* pMIDIStatus, long lMasterVolume);

/* ���݂̃}�X�^�[�p���̒l��ݒ� */
long __stdcall MIDIStatus_SetMasterPan (MIDIStatus* pMIDIStatus, long lMasterPan);

/* ���݂̃}�X�^�[���o�[�u�̒l��1�ݒ� */
long __stdcall MIDIStatus_SetMasterReverb (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* �}�X�^�[���o�[�u�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B */
long __stdcall MIDIStatus_SetMasterReverbType (MIDIStatus* pMIDIStatus, long lMasterReverbType);

/* ���݂̃}�X�^�[�R�[���X�̒l��1�ݒ� */
long __stdcall MIDIStatus_SetMasterChorus (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* �}�X�^�[�R�[���X�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B */
long __stdcall MIDIStatus_SetMasterChorusType (MIDIStatus* pMIDIStatus, long lMasterChorusType);

/* ���݂̃}�X�^�[�f�B���C�̒l��1�ݒ�(XG�ł̓f�B���C�̓��@���G�[�V�����Ɠǂݑւ���) */
long __stdcall MIDIStatus_SetMasterDelay (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* �}�X�^�[�f�B���C�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B */
/* (XG�ł̓}�X�^�[���@���G�[�V�����ƂȂ�) */
long __stdcall MIDIStatus_SetMasterDelayType (MIDIStatus* pMIDIStatus, long lMasterDelayType);

/* ���݂̃}�X�^�[�C�R���C�U�̒l��1�ݒ� */
long __stdcall MIDIStatus_SetMasterEqualizer (MIDIStatus* pMIDIStatus, long lNum, long lVal);

/* �}�X�^�[�C�R���C�U�̃^�C�v��ݒ肵�A����ɔ����e�p�����[�^���f�t�H���g�l�ɐݒ肷��B */
long __stdcall MIDIStatus_SetMasterEqualizerType (MIDIStatus* pMIDIStatus, long lMasterEqualizerType);

/* MIDIStatus_Put�n�֐� */

/* ���Z�b�g */
long __stdcall MIDIStatus_PutReset (MIDIStatus* pMIDIStatus);

/* GM���Z�b�g */
long __stdcall MIDIStatus_PutGMReset (MIDIStatus* pMIDIStatus);

/* GM2���Z�b�g */
long __stdcall MIDIStatus_PutGM2Reset (MIDIStatus* pMIDIStatus);

/* GS���Z�b�g */
long __stdcall MIDIStatus_PutGSReset (MIDIStatus* pMIDIStatus);

/* 88���Z�b�g */
long __stdcall MIDIStatus_Put88Reset (MIDIStatus* pMIDIStatus);

/* XG���Z�b�g */
long __stdcall MIDIStatus_PutXGReset (MIDIStatus* pMIDIStatus);

/* MIDI���b�Z�[�W��^���邱�Ƃɂ��p�����[�^��ύX����B */
long __stdcall MIDIStatus_PutMIDIMessage (MIDIStatus* pMIDIStatus, unsigned char* pMIDIMessage, long lLen);



/* MIDIStatus�̏�������(�O���B���A���̊֐���MIDIStatus_Save����Ăяo�����) */
long __stdcall MIDIStatus_Write (MIDIStatus* pMIDIStatus, FILE* pFile, long lVersion);

/* MIDIStatus�̕ۑ� */
long __stdcall MIDIStatus_SaveA (MIDIStatus* pMIDIStatus, const char* pszFileName);
long __stdcall MIDIStatus_SaveW (MIDIStatus* pMIDIStatus, const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIStatus_Save MIDIStatus_SaveW
#else
#define MIDIStatus_Save MIDIStatus_SaveA
#endif

/* MIDIStatus�̓ǂݍ���(�O���B���A���̊֐���MIDIStatus_Load����Ăяo�����) */
long __stdcall MIDIStatus_Read (MIDIStatus* pMIDIStatus, FILE* pFile, long lVersion);

/* MIDIStatus�̃��[�h */
MIDIStatus* __stdcall MIDIStatus_LoadA (const char* pszFileName);
MIDIStatus* __stdcall MIDIStatus_LoadW (const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIStatus_Load MIDIStatus_LoadW
#else
#define MIDIStatus_Load MIDIStatus_LoadA
#endif

#ifdef __cplusplus
}
#endif

#endif

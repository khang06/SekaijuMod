/******************************************************************************/
/*                                                                            */
/*  MIDIClock.h - MIDI�N���b�N�w�b�_�[�t�@�C��             (C)2002-2012 ����  */
/*                                                                            */
/******************************************************************************/

/* ���̃��W���[���͕��ʂ�C�����Win32API���g���ď�����Ă���B */
/* ���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��z�z�����B */
/* �v���W�F�N�g�z�[���y�[�W�F"http://openmidiproject.sourceforge.jp/index.html" */
/* MIDI�N���b�N�̃X�^�[�g�E�X�g�b�v�E���Z�b�g */
/* �e���|�E�^�C���x�[�X�E�o�ߎ����E�o�߃e�B�b�N���̎擾�E�ݒ� */
/* �^�C�����[�h(TPQNBASE�ASMPTE24BASE�ASMPTE25BASE�ASMPTE29BASE�ASMPTE30BASE)�ɑΉ� */
/* �X���[�u���[�h(MIDI�^�C�~���O�N���b�N�ASMPTE/MTC)�ɑΉ� */

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

#if !defined _MIDICLOCK_H_
#define _MIDICLOCK_H_

/* C++������g�p�\�Ƃ��� */
#ifdef __cplusplus
extern "C" {
#endif

/* �e���| */
#define MIDICLOCK_TEMPO(U) (60000000/(U))
#define MIDICLOCK_MAXTEMPO 60000000
#define MIDICLOCK_MINTEMPO 1

/* �^�C�����[�h(�ȉ��̒l�̂ݗ��p�\) */
#define MIDICLOCK_TPQNBASE         0 /* TPQN�x�[�X */
#define MIDICLOCK_SMPTE24BASE     24 /* SMPTE24�x�[�X(24�t���[���^�b) */
#define MIDICLOCK_SMPTE25BASE     25 /* SMPTE25�x�[�X(25�t���[���^�b) */
#define MIDICLOCK_SMPTE29BASE     29 /* SMPTE29�x�[�X(29.97�t���[���^�b) */
#define MIDICLOCK_SMPTE30BASE     30 /* SMPTE30�x�[�X(30�t���[���^�b) */

/* �^�C�����[�h(ver0.6�ȑO�Ƃ̌݊��̂��߂Ɏc���Ă���) */
#define MIDICLOCK_MASTERTPQNBASE         0 /* TPQN�x�[�X */
#define MIDICLOCK_MASTERSMPTE24BASE     24 /* SMPTE24�x�[�X(24�t���[���^�b) */
#define MIDICLOCK_MASTERSMPTE25BASE     25 /* SMPTE25�x�[�X(25�t���[���^�b) */
#define MIDICLOCK_MASTERSMPTE29BASE     29 /* SMPTE29�x�[�X(29.97�t���[���^�b) */
#define MIDICLOCK_MASTERSMPTE30BASE     30 /* SMPTE30�x�[�X(30�t���[���^�b) */

/* MIDI���͓������[�h(�ȉ��̒l�̂ݗ��p�\) */
#define MIDICLOCK_MASTER                 0 /* �}�X�^�[ */
#define MIDICLOCK_SLAVEMIDITIMINGCLOCK   1 /* �X���[�u���[�h(MIDI�^�C�~���O�N���b�N�Ǐ]) */
#define MIDICLOCK_SLAVESMPTEMTC          2 /* �X���[�u���[�h(SMPTE/MTC�Ǐ]) */

/* �X�s�[�h[�~0.01��] */
#define MIDICLOCK_SPEEDNORMAL        10000 /* �X�s�[�h=100�� */
#define MIDICLOCK_SPEEDSLOW           5000 /* �X�s�[�h=50�� */
#define MIDICLOCK_SPEEDFAST          20000 /* �X�s�[�h=200�� */
#define MIDICLOCK_MINSPEED               0 /* �X�s�[�h=�Î~ */
#define MIDICLOCK_MAXSPEED          100000 /* �X�s�[�h=�ő� */

/* MIDIClock�\���� */
typedef struct tagMIDIClock {
	long m_lTimeMode;        /* �^�C�����[�h */
	long m_lResolution;      /* ����\[�e�B�b�N/4������]����[�e�B�b�N/1�t���[��] */
	long m_lTempo;           /* �e���|[�ʕb/4������] */
	long m_lDummyTempo;      /* �_�~�[�e���|[�ʕb/4������](�X���[�u���[�h���̉��ێ��p) */
	long m_lSpeed;           /* �X�s�[�h(10000���W���A0=�Î~�A20000���{��) */
	long m_lMIDIInSyncMode;  /* MIDI���͓������[�h(0=�}�X�^�[�A1=MIDI�^�C�~���O�N���b�N�A2=SMPTE/MTC) */
	long m_lPeriod;          /* �R�[���o�b�N�֐��Ăяo���Ԋu[�~���b] */
	long m_lMillisec;        /* ����[�~���b] */
	long m_lMillisecMod;     /* �����덷�␳�p */
	long m_lOldMillisec;     /* �O��̎���[�~���b]�ێ��p */
	long m_lDummyMillisec;   /* �_�~�[����[�~���b](�X���[�u���[�h���̉��ێ��p) */
	long m_lDummyMillisecMod;/* �_�~�[�����덷�␳�p(�X���[�u���[�h���̉��ێ��p) */
	long m_lOldDummyMillisec;/* �O��̃_�~�[����[�~���b](�X���[�u���[�h���̉��ێ��p) */
	long m_lTickCount;       /* �e�B�b�N�J�E���g[Tick] */
	long m_lTickCountMod;    /* �e�B�b�N�J�E���g�덷�␳�p */
	long m_lOldTickCount;    /* �O��̃e�B�b�N�J�E���g[Tick]�ێ��p */
	long m_lTimerID;         /* �^�C�}�[ID */
	unsigned char m_bySMPTE[8]; /* MIDI�^�C���R�[�h�N�H�[�^�[�t���[���ێ��p */
	volatile long m_lRunning;   /* 1�̂Ƃ����쒆�A0�̂Ƃ���~�� */
	volatile long m_lLocked;    /* 1�̂Ƃ������o�ϐ��̑���֎~�A0�̂Ƃ����� */
} MIDIClock;

/* MIDI�N���b�N�I�u�W�F�N�g�̍폜 */
void __stdcall MIDIClock_Delete (MIDIClock* pMIDIClock);

/* MIDI�N���b�N�I�u�W�F�N�g�̐��� */
MIDIClock* __stdcall MIDIClock_Create (long lTimeMode, long lResolution, long lTempo);

/* �^�C���x�[�X(�^�C�����[�h�ƕ���\[�e�B�b�N/4������]����[�e�B�b�N/1�t���[��])�̎擾 */
long __stdcall MIDIClock_GetTimeBase (MIDIClock* pMIDIClock, long* pTimeMode, long* pResolution);

/* �^�C���x�[�X(�^�C�����[�h�ƕ���\[�e�B�b�N/4������]����[�e�B�b�N/1�t���[��])�̐ݒ� */
long __stdcall MIDIClock_SetTimeBase (MIDIClock* pMIDIClock, long lTimeMode, long lResolution);

/* MIDI�e���|[�ʕb/4������]�̎擾 */
long __stdcall MIDIClock_GetTempo (MIDIClock* pMIDIClock);

/* MIDI�e���|[�ʕb/4������]�̐ݒ� */
long __stdcall MIDIClock_SetTempo (MIDIClock* pMIDIClock, long lTempo);

/* �X�s�[�h[�~0.01��]�̎擾 */
long __stdcall MIDIClock_GetSpeed (MIDIClock* pMIDIClock);

/* �X�s�[�h[�~0.01��]�̐ݒ� */
long __stdcall MIDIClock_SetSpeed (MIDIClock* pMIDIClock, long lSpeed);

/* MIDI���͓������[�h�̎擾 */
long __stdcall MIDIClock_GetMIDIInSyncMode (MIDIClock* pMIDIClock);

/* MIDI���͓������[�h�̐ݒ� */
long __stdcall MIDIClock_SetMIDIInSyncMode (MIDIClock* pMIDIClock, long lMIDIInSyncMode);

/* �X�^�[�g */
long __stdcall MIDIClock_Start (MIDIClock* pMIDIClock);

/* �X�g�b�v */
long __stdcall MIDIClock_Stop (MIDIClock* pMIDIClock);

/* ���Z�b�g */
long __stdcall MIDIClock_Reset (MIDIClock* pMIDIClock);

/* ���쒆�����ׂ� */
long __stdcall MIDIClock_IsRunning (MIDIClock* pMIDIClock);

/* �o�ߎ���[�~���b]�̎擾 */
long __stdcall MIDIClock_GetMillisec (MIDIClock* pMIDIClock);

/* �o�ߎ���[�~���b]�̐ݒ� */
long __stdcall MIDIClock_SetMillisec (MIDIClock* pMIDIClock, long lMillisec);

/* �o�߃e�B�b�N��[tick]�̎擾 */
long __stdcall MIDIClock_GetTickCount (MIDIClock* pMIDIClock);

/* �o�߃e�B�b�N��[tick]�̐ݒ� */
long __stdcall MIDIClock_SetTickCount (MIDIClock* pMIDIClock, long lTickCount);


/* MIDI�^�C�~���O�N���b�N��F��������(0xF8) */
long __stdcall MIDIClock_PutMIDITimingClock (MIDIClock* pMIDIClock);

/* �V�X�e���G�N�X�N���[�V���ɂ��SMPTE/MTC��F�������� */
long __stdcall MIDIClock_PutSysExSMPTEMTC 
	(MIDIClock* pMIDIClock, unsigned char cHour, 
	unsigned char cMinute, unsigned char cSecond, unsigned char cFrame);

/* MIDI�^�C���R�[�h�N�H�[�^�[�t���[��(0xF1)�ɂ��SMPTE/MTC��F�������� */
long __stdcall MIDIClock_PutSMPTEMTC (MIDIClock* pMIDIClock, unsigned char cSMPTEMTC);

/* MIDI�N���b�N��MIDI���b�Z�[�W��F�������� */
long __stdcall MIDIClock_PutMIDIMessage 
	(MIDIClock* pMIDIClock, unsigned char* pMessage, long lLen);


#ifdef __cplusplus
}
#endif

#endif

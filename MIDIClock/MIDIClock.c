/******************************************************************************/
/*                                                                            */
/*�@MIDIClock.c - MIDIClock�\�[�X�t�@�C��                  (C)2002-2012 ����  */
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

#include <assert.h>
#include <stdlib.h>
#include <windows.h>
#include <crtdbg.h>
#include <mmsystem.h>
#include "MIDIClock.h"

/* �ėp�}�N��(�ŏ��A�ő�A���ݍ���) */
#ifndef MIN
#define MIN(A,B) ((A)>(B)?(B):(A))
#endif
#ifndef MAX
#define MAX(A,B) ((A)>(B)?(A):(B))
#endif
#ifndef CLIP
#define CLIP(A,B,C) ((A)>(B)?(A):((B)>(C)?(C):(B)))
#endif

/* MIDI�N���b�N�I�u�W�F�N�g�̍폜 */
void __stdcall MIDIClock_Delete (MIDIClock* pMIDIClock) {
	if (pMIDIClock) {
		MIDIClock_Stop (pMIDIClock);
	}
	free (pMIDIClock);
	pMIDIClock = NULL;
}

/* MIDI�N���b�N�I�u�W�F�N�g�̐��� */
MIDIClock* __stdcall MIDIClock_Create (long lTimeMode, long lResolution, long lTempo) {
	MIDIClock* pMIDIClock;
	/* �^�C�����[�h�̐������`�F�b�N */
	if (lTimeMode != MIDICLOCK_TPQNBASE &&
		lTimeMode != MIDICLOCK_SMPTE24BASE &&
		lTimeMode != MIDICLOCK_SMPTE25BASE &&
		lTimeMode != MIDICLOCK_SMPTE29BASE &&
		lTimeMode != MIDICLOCK_SMPTE30BASE) {
		return NULL;
	}
	/* ����\�̐������`�F�b�N */
	if (lTimeMode == MIDICLOCK_TPQNBASE) {
		if (lResolution <= 0 || lResolution > 32767) {
			return NULL;
		}
	}
	else {
		if (lResolution <= 0 || lResolution > 255) {
			return NULL;
		}
	}
	/* �e���|�̐������`�F�b�N */
	if (lTempo < MIDICLOCK_MINTEMPO || lTempo > MIDICLOCK_MAXTEMPO) {
		return NULL;
	}
	/* �I�u�W�F�N�g�p�̃������m�� */
	pMIDIClock = calloc (1, sizeof (MIDIClock));
	if (pMIDIClock == NULL) {
		return NULL;
	}
	/* �����o�ϐ��̒l�ݒ� */
	pMIDIClock->m_lTimeMode = lTimeMode;
	pMIDIClock->m_lResolution = lResolution;
	pMIDIClock->m_lTempo = lTempo;
	pMIDIClock->m_lDummyTempo = lTempo;
	pMIDIClock->m_lSpeed = MIDICLOCK_SPEEDNORMAL;
	pMIDIClock->m_lMIDIInSyncMode = MIDICLOCK_MASTER;
	pMIDIClock->m_lMillisec = 0;
	pMIDIClock->m_lMillisecMod = 0;
	pMIDIClock->m_lOldMillisec = 0;
	pMIDIClock->m_lDummyMillisec = 0;
	pMIDIClock->m_lTickCount = 0;
	pMIDIClock->m_lTickCountMod = 0;
	pMIDIClock->m_lOldTickCount = 0;
	pMIDIClock->m_lLocked = 0;
	return pMIDIClock;
}


/* �R�[���o�b�N�֐� */
static void CALLBACK TimeProc (UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
	MIDIClock* pMIDIClock = (MIDIClock*)(dwUser);

	/* ���b�N */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	
	/* MIDI���͓������[�h���}�X�^�[�̏ꍇ */
	if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_MASTER) { // 20090626�C��
		long lDeltaMillisec = 0;
		long lDeltaTickCount = 0; 

		/* �o�߃~���b�̌v�Z */
		pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
		lDeltaMillisec = 
			(pMIDIClock->m_lMillisecMod + pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed) / MIDICLOCK_SPEEDNORMAL; 
		pMIDIClock->m_lMillisecMod += 
			(pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed - lDeltaMillisec * MIDICLOCK_SPEEDNORMAL);
		pMIDIClock->m_lMillisec += lDeltaMillisec;
		pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);

		/* �o�߃e�B�b�N�J�E���g�̌v�Z */
		pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
		switch (pMIDIClock->m_lTimeMode) {
		case MIDICLOCK_TPQNBASE:
			lDeltaTickCount = 
				(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
				pMIDIClock->m_lTempo;
			pMIDIClock->m_lTickCountMod += 
				(lDeltaMillisec * 1000 * pMIDIClock->m_lResolution - lDeltaTickCount * pMIDIClock->m_lTempo);
			pMIDIClock->m_lTickCount += lDeltaTickCount;
			break;
		case MIDICLOCK_SMPTE24BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 24;
			break;
		case MIDICLOCK_SMPTE25BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 25;
			break;	
		case MIDICLOCK_SMPTE29BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 2997 / 100;
			break;
		case MIDICLOCK_SMPTE30BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 30;
			break;
		}
		pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF);		
	}

	/* MIDI���͓������[�h��MIDI�^�C�~���O�N���b�N�ɂ��X���[�u�̏ꍇ */
	else if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) { //20090701�C��
		long lDeltaMillisec = 0;
		/* �o�߃~���b�̌v�Z */
		pMIDIClock->m_lOldDummyMillisec = pMIDIClock->m_lDummyMillisec;
		lDeltaMillisec = 
			(pMIDIClock->m_lDummyMillisecMod + pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed) / MIDICLOCK_SPEEDNORMAL; 
		pMIDIClock->m_lDummyMillisecMod += 
			(pMIDIClock->m_lPeriod * pMIDIClock->m_lSpeed - lDeltaMillisec * MIDICLOCK_SPEEDNORMAL);
		pMIDIClock->m_lDummyMillisec += lDeltaMillisec;
		pMIDIClock->m_lDummyMillisec = CLIP (0, pMIDIClock->m_lDummyMillisec, 0x7FFFFFFF);
	}


	/* ���b�N���� */
	pMIDIClock->m_lLocked = 0;

}

/* �^�C���x�[�X(�^�C�����[�h�ƕ���\[�e�B�b�N/4������]����[�e�B�b�N/1�t���[��])�̎擾 */
long __stdcall MIDIClock_GetTimeBase (MIDIClock* pMIDIClock, long* pTimeMode, long* pResolution) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	*pTimeMode = pMIDIClock->m_lTimeMode;
	*pResolution = pMIDIClock->m_lResolution;
	return 1;
}

/* �^�C���x�[�X(�^�C�����[�h�ƕ���\[�e�B�b�N/4������]����[�e�B�b�N/1�t���[��])�̐ݒ� */
long __stdcall MIDIClock_SetTimeBase (MIDIClock* pMIDIClock, long lTimeMode, long lResolution) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	/* ���샂�[�h�̐������`�F�b�N */
	if (lTimeMode != MIDICLOCK_TPQNBASE &&
		lTimeMode != MIDICLOCK_SMPTE24BASE &&
		lTimeMode != MIDICLOCK_SMPTE25BASE &&
		lTimeMode != MIDICLOCK_SMPTE29BASE &&
		lTimeMode != MIDICLOCK_SMPTE30BASE) {
		return 0;
	}
	/* ����\�̐������`�F�b�N */
	if (lResolution <= 0 || lResolution >= 65536) {
		return 0;
	}
	/* �^�C�����[�h���͕���\���ύX���ꂽ���̂� */
	if (lTimeMode != pMIDIClock->m_lTimeMode || lResolution != pMIDIClock->m_lResolution) {
		while (pMIDIClock->m_lLocked);
		pMIDIClock->m_lLocked = 1;
		pMIDIClock->m_lTimeMode = lTimeMode;
		pMIDIClock->m_lResolution = lResolution;
		pMIDIClock->m_lTickCountMod = 0;
		pMIDIClock->m_lMillisecMod = 0;
		pMIDIClock->m_lLocked = 0;
		/* ���ӁF���݂̌o�ߎ���[�~���b]�ƌo�߃e�B�b�N�J�E���g[tick]�͕ێ�����B */
	}
	return 1;
}

/* MIDI�e���|[�ʕb/4������]�̎擾 */
long __stdcall MIDIClock_GetTempo (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) {
		return pMIDIClock->m_lDummyTempo;
	}
	return pMIDIClock->m_lTempo;
}

/* MIDI�e���|[�ʕb/4������]�̐ݒ� */
long __stdcall MIDIClock_SetTempo (MIDIClock* pMIDIClock, long lTempo) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	if (lTempo < MIDICLOCK_MINTEMPO || lTempo > MIDICLOCK_MAXTEMPO) {
		return 0;
	}	
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	pMIDIClock->m_lTempo = lTempo;
	pMIDIClock->m_lDummyTempo = lTempo;
	pMIDIClock->m_lLocked = 0;
	return 1;
}

/* �X�s�[�h[�~0.01��]�̎擾 */
long __stdcall MIDIClock_GetSpeed (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	return pMIDIClock->m_lSpeed;
}

/* �X�s�[�h[�~0.01��]�̐ݒ� */
long __stdcall MIDIClock_SetSpeed (MIDIClock* pMIDIClock, long lSpeed) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	if (lSpeed < MIDICLOCK_MINSPEED || lSpeed > MIDICLOCK_MAXSPEED) {
		return 0;
	}
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	pMIDIClock->m_lSpeed = lSpeed;
	pMIDIClock->m_lLocked = 0;
	return 1;
}

/* MIDI���͓������[�h�̎擾 */
long __stdcall MIDIClock_GetMIDIInSyncMode (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lMIDIInSyncMode;
}

/* MIDI���͓������[�h�̐ݒ� */
long __stdcall MIDIClock_SetMIDIInSyncMode (MIDIClock* pMIDIClock, long lMIDIInSyncMode) {
	assert (pMIDIClock);
	if (lMIDIInSyncMode != MIDICLOCK_MASTER &&
		lMIDIInSyncMode != MIDICLOCK_SLAVEMIDITIMINGCLOCK &&
		lMIDIInSyncMode != MIDICLOCK_SLAVESMPTEMTC) {
		return 0;
	}
	/* MIDI���͓������[�h���ύX���ꂽ���̂� */
	if (lMIDIInSyncMode != pMIDIClock->m_lMIDIInSyncMode) {
		while (pMIDIClock->m_lLocked);
		pMIDIClock->m_lLocked = 1;
		pMIDIClock->m_lMIDIInSyncMode = lMIDIInSyncMode;
		pMIDIClock->m_lTickCountMod = 0;
		pMIDIClock->m_lMillisecMod = 0;
		pMIDIClock->m_lDummyMillisec = pMIDIClock->m_lMillisec;
		pMIDIClock->m_lDummyMillisecMod = pMIDIClock->m_lMillisecMod;
		pMIDIClock->m_lOldDummyMillisec = pMIDIClock->m_lOldMillisec;
		pMIDIClock->m_lDummyTempo = pMIDIClock->m_lTempo;
		pMIDIClock->m_lLocked = 0;
		/* ���ӁF���݂̌o�ߎ���[�~���b]�ƌo�߃e�B�b�N�J�E���g[tick]�͕ێ�����B */
	}
	return 1;
}

/* �X�^�[�g */
long __stdcall MIDIClock_Start (MIDIClock* pMIDIClock) {
	TIMECAPS tc;
	long lPeriod; /* �R�[���o�b�N�֐��Ăяo���Ԋu[�~���b] */
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* ���b�N */ 
	/* ���ӁFtimeSetEvent���ɃR�[���o�b�N�֐����Ɠ������b�N����ƃf�b�h���b�N���� */
	//while (pMIDIClock->m_lLocked);
	//pMIDIClock->m_lLocked = 1;

	/* �}���`���f�B�A�^�C�}�[�N�� */
	if (pMIDIClock->m_lTimerID == 0) {
		if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
			pMIDIClock->m_lLocked = 0;
			return 0;
		}
		lPeriod = tc.wPeriodMin;
		timeBeginPeriod (lPeriod);
		 pMIDIClock->m_lTimerID = timeSetEvent 
			(lPeriod, lPeriod, TimeProc, (DWORD)pMIDIClock, TIME_PERIODIC); 
		if(!pMIDIClock->m_lTimerID) {
			pMIDIClock->m_lLocked = 0;
			return 0;
		}
		pMIDIClock->m_lPeriod = lPeriod;
	}

	/* �����Ԃɐݒ� */
	pMIDIClock->m_lRunning = 1;

	/* ���b�N���� */
	//pMIDIClock->m_lLocked = 0;
    
	return 1;
}

/* �X�g�b�v */
long __stdcall MIDIClock_Stop (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* ���b�N */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	/* �}���`���f�B�A�^�C�}�[��~ */
    if (pMIDIClock->m_lTimerID != 0) {
	    timeKillEvent (pMIDIClock->m_lTimerID);
		timeEndPeriod (pMIDIClock->m_lPeriod);
		pMIDIClock->m_lTimerID = 0;
	}

	/* ��~��Ԃɐݒ� */
	pMIDIClock->m_lRunning = 0;
	
	/* ���b�N���� */
	pMIDIClock->m_lLocked = 0;

   	return 1;
}

/* ���Z�b�g */
long __stdcall MIDIClock_Reset (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	
	/* ���b�N */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	/* �����o�ϐ��̏����� */
	pMIDIClock->m_lDummyTempo = pMIDIClock->m_lTempo;
	pMIDIClock->m_lMillisec = 0;
	pMIDIClock->m_lOldMillisec = 0;
	pMIDIClock->m_lMillisecMod = 0;
	pMIDIClock->m_lDummyMillisec = 0;
	pMIDIClock->m_lOldDummyMillisec = 0;
	pMIDIClock->m_lDummyMillisecMod = 0;
	pMIDIClock->m_lTickCount = 0;
	pMIDIClock->m_lTickCountMod = 0;
	pMIDIClock->m_lOldTickCount = 0;
	memset (pMIDIClock->m_bySMPTE, 0, sizeof (pMIDIClock->m_bySMPTE));

	/* ���b�N���� */
	pMIDIClock->m_lLocked = 0;
	return 1;
}	

/* ���쒆�����ׂ� */
long __stdcall MIDIClock_IsRunning (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lRunning;
}

/* �o�ߎ���[�~���b]�̎擾 */
long __stdcall MIDIClock_GetMillisec (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lMillisec;
}

/* �o�ߎ���[�~���b]�̐ݒ� */
long __stdcall MIDIClock_SetMillisec (MIDIClock* pMIDIClock, long lMillisec) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	
	/* ���b�N */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	
	pMIDIClock->m_lMillisec = lMillisec;
	pMIDIClock->m_lOldMillisec = lMillisec;
	pMIDIClock->m_lMillisecMod = 0;
	pMIDIClock->m_lDummyMillisec = lMillisec;
	pMIDIClock->m_lOldDummyMillisec = lMillisec;
	pMIDIClock->m_lDummyMillisecMod = 0;

	/* ���b�N���� */
	pMIDIClock->m_lLocked = 0;
	return 1;
}

/* �o�߃e�B�b�N��[tick]�̎擾 */
long __stdcall MIDIClock_GetTickCount (MIDIClock* pMIDIClock) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	return pMIDIClock->m_lTickCount;		
}

/* �o�߃e�B�b�N��[tick]�̐ݒ� */
long __stdcall MIDIClock_SetTickCount (MIDIClock* pMIDIClock, long lTickCount) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* ���b�N */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	pMIDIClock->m_lTickCount = lTickCount;
	pMIDIClock->m_lOldTickCount = lTickCount;
	pMIDIClock->m_lTickCountMod = 0;

	/* ���b�N���� */
	pMIDIClock->m_lLocked = 0;
	return 1;
}



/* MIDI�^�C�~���O�N���b�N��F��������(0xF8) */
long __stdcall MIDIClock_PutMIDITimingClock (MIDIClock* pMIDIClock) {
	long lDeltaTickCount = 0;
	long lDeltaMillisec = 0;
	long lOldDummyMillisec = 0;
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* MIDI���͓������[�h��MIDI�^�C�~���O�N���b�N�ɂ��X���[�u�ȊO�̏ꍇ�͖��� */
	if (pMIDIClock->m_lMIDIInSyncMode != MIDICLOCK_SLAVEMIDITIMINGCLOCK) {
		return 0;
	}

	/* ���b�N */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;

	/* �o�߃e�B�b�N�J�E���g���v�Z(20090626����:����\24�����ɂ��Ή�) */
	pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
	lDeltaTickCount =
		(pMIDIClock->m_lTickCountMod + pMIDIClock->m_lResolution) / 24;
	pMIDIClock->m_lTickCountMod +=
		(pMIDIClock->m_lResolution - lDeltaTickCount * 24);
	pMIDIClock->m_lTickCount += lDeltaTickCount;
	pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF); 

	/* ���݂̃e���|��p���Čo�߃~���b���v�Z(20090626����)(�p�~) */
	/*pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
	lDeltaMillisec =
		(pMIDIClock->m_lMillisecMod + lDeltaTickCount * pMIDIClock->m_lTempo) / (pMIDIClock->m_lResolution * 1000);
	pMIDIClock->m_lMillisecMod +=
		(lDeltaTickCount * pMIDIClock->m_lTempo - lDeltaMillisec * (pMIDIClock->m_lResolution * 1000));
	pMIDIClock->m_lMillisec += lDeltaMillisec;
	pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);*/

	/* ���݂̌o�ߎ�����K�p */
	//lOldDummyMillisec = pMIDIClock->m_lDummyMillisec;
	pMIDIClock->m_lMillisec = pMIDIClock->m_lDummyMillisec;
	lDeltaMillisec = pMIDIClock->m_lMillisec - pMIDIClock->m_lOldMillisec;

	/* �_�~�[�e���|���o�߃e�B�b�N�J�E���g�ƌo�ߎ�������v�Z */
	pMIDIClock->m_lDummyTempo = 
		lDeltaMillisec  * 1000 * pMIDIClock->m_lResolution / lDeltaTickCount;
	pMIDIClock->m_lDummyTempo = CLIP (1, pMIDIClock->m_lDummyTempo, 60000000);

	/* ���b�N���� */
	pMIDIClock->m_lLocked = 0;
	
	return 1;
}

/* �V�X�e���G�N�X�N���[�V���ɂ��SMPTE/MTC��F�������� */
long __stdcall MIDIClock_PutSysExSMPTEMTC 
	(MIDIClock* pMIDIClock, unsigned char cHour, 
	unsigned char cMinute, unsigned char cSecond, unsigned char cFrame) {
	long lDeltaTickCount = 0;
	long lDeltaMillisec = 0;
	double dFrameLen[4] = {41.66666666, 40.00000000, 33.36670003, 33.33333333};
	long lFrameRate = (cHour >> 5) & 0x03;
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* MIDI���͓������[�h��SMPTE/MTC�ɂ��X���[�u�ȊO�̏ꍇ�͖��� */
	if (pMIDIClock->m_lMIDIInSyncMode != MIDICLOCK_SLAVESMPTEMTC) {
		return 0;
	}

	/* ���b�N */
	while (pMIDIClock->m_lLocked);
	pMIDIClock->m_lLocked = 1;
	
	/* �o�߃~���b�̌v�Z */
	cHour = cHour & 0x1F;
	pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
	pMIDIClock->m_lMillisec = cHour * 3600000 + cMinute * 60000 + cSecond * 1000 +
		((long)(dFrameLen[lFrameRate] * cFrame));
	lDeltaMillisec = pMIDIClock->m_lMillisec - pMIDIClock->m_lOldMillisec;
	pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);

	/* �o�߃e�B�b�N�J�E���g�̌v�Z */
	pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
	switch (pMIDIClock->m_lTimeMode) {
	case MIDICLOCK_TPQNBASE:
		if (lDeltaMillisec >= 0) {
			lDeltaTickCount = 
				(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
				pMIDIClock->m_lTempo;
			pMIDIClock->m_lTickCountMod += 
				(lDeltaMillisec * 1000 * pMIDIClock->m_lResolution - lDeltaTickCount * pMIDIClock->m_lTempo);
			pMIDIClock->m_lTickCount += lDeltaTickCount;
		}
		else {
			lDeltaTickCount = 
				(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
				pMIDIClock->m_lTempo;
			pMIDIClock->m_lTickCountMod = 0;
			pMIDIClock->m_lTickCount += lDeltaTickCount;
		}
		break;
	case MIDICLOCK_SMPTE24BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 24;
		break;
	case MIDICLOCK_SMPTE25BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 25;
		break;	
	case MIDICLOCK_SMPTE29BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 2997 / 100;
		break;
	case MIDICLOCK_SMPTE30BASE:
		pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 30;
		break;
	}
	pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF); 

	/* ���b�N���� */
	pMIDIClock->m_lLocked = 0;

	return 1;
}

/* MIDI�^�C���R�[�h�N�H�[�^�[�t���[��(0xF1)�ɂ��SMPTE/MTC��F�������� */
long __stdcall MIDIClock_PutSMPTEMTC (MIDIClock* pMIDIClock, unsigned char cSMPTEMTC) {
	long k;
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}

	/* MIDI���͓������[�h��SMPTE/MTC�ɂ��X���[�u�ȊO�̏ꍇ�͖��� */
	if (pMIDIClock->m_lMIDIInSyncMode != MIDICLOCK_SLAVESMPTEMTC) {
		return 0;
	}

	k = (cSMPTEMTC >> 4) & 0x07;
	pMIDIClock->m_bySMPTE[k] = cSMPTEMTC;

	/* 8�Ԗڂ̃��b�Z�[�W����M���� */
	if (k == 7) {
		long lDeltaTickCount = 0;
		long lDeltaMillisec = 0;
		double dFrameLen[4] = {41.66666666, 40.00000000, 33.36670003, 33.33333333};
		long lFrameRate = (pMIDIClock->m_bySMPTE[7] & 0x0E) >> 1;
		long lHour =   ((pMIDIClock->m_bySMPTE[7] & 0x01) << 4) | (pMIDIClock->m_bySMPTE[6] & 0x0F);
		long lMinute = ((pMIDIClock->m_bySMPTE[5] & 0x0F) << 4) | (pMIDIClock->m_bySMPTE[4] & 0x0F);
		long lSecond = ((pMIDIClock->m_bySMPTE[3] & 0x0F) << 4) | (pMIDIClock->m_bySMPTE[2] & 0x0F);
		long lFrame =  ((pMIDIClock->m_bySMPTE[1] & 0x0F) << 4) | (pMIDIClock->m_bySMPTE[0] & 0x0F);

		/* ���b�N */
		while (pMIDIClock->m_lLocked);
		pMIDIClock->m_lLocked = 1;
		
		/* �o�߃~���b�̌v�Z */
		pMIDIClock->m_lOldMillisec = pMIDIClock->m_lMillisec;
		pMIDIClock->m_lMillisec = lHour * 3600000 + lMinute * 60000 + lSecond * 1000 +
			((long)(dFrameLen[lFrameRate] * lFrame));
		lDeltaMillisec = pMIDIClock->m_lMillisec - pMIDIClock->m_lOldMillisec;
		pMIDIClock->m_lMillisec = CLIP (0, pMIDIClock->m_lMillisec, 0x7FFFFFFF);

		/* �o�߃e�B�b�N�J�E���g�̌v�Z */
		pMIDIClock->m_lOldTickCount = pMIDIClock->m_lTickCount;
		switch (pMIDIClock->m_lTimeMode) {
		case MIDICLOCK_TPQNBASE:
			if (lDeltaTickCount >= 0) {
				lDeltaTickCount = 
					(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
					pMIDIClock->m_lTempo;
				pMIDIClock->m_lTickCountMod += 
					(lDeltaMillisec * 1000 * pMIDIClock->m_lResolution - lDeltaTickCount * pMIDIClock->m_lTempo);
				pMIDIClock->m_lTickCount += lDeltaTickCount;
			}
			else {
				lDeltaTickCount = 
					(pMIDIClock->m_lTickCountMod + lDeltaMillisec * 1000 * pMIDIClock->m_lResolution) / 
					pMIDIClock->m_lTempo;
				pMIDIClock->m_lTickCountMod = 0;
				pMIDIClock->m_lTickCount += lDeltaTickCount;
			}
			break;
		case MIDICLOCK_SMPTE24BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 24;
			break;
		case MIDICLOCK_SMPTE25BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 25;
			break;	
		case MIDICLOCK_SMPTE29BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 2997 / 100;
			break;
		case MIDICLOCK_SMPTE30BASE:
			pMIDIClock->m_lTickCount = pMIDIClock->m_lResolution * pMIDIClock->m_lMillisec / 1000 * 30;
			break;
		}
		pMIDIClock->m_lTickCount = CLIP (0, pMIDIClock->m_lTickCount, 0x7FFFFFFF); 

		/* ���b�N���� */
		pMIDIClock->m_lLocked = 0;

	
	}
	return 1;
}

/* MIDI�N���b�N��MIDI���b�Z�[�W��F�������� */
long __stdcall MIDIClock_PutMIDIMessage 
(MIDIClock* pMIDIClock, unsigned char* pMIDIMessage, long lLen) {
	assert (pMIDIClock);
	if (pMIDIClock == NULL) {
		return 0;
	}
	
	/* �V�X�e���G�N�X�N���[�V�����b�Z�[�W(0xF0) */
	if (*pMIDIMessage == 0xF0) {
		unsigned char bySysExSMPTEMTC[10] = 
			{0xF0, 0x7F, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF7};
		/* �V�X�e���G�N�X�N���[�V���ɂ��SMPTE/MTC */
		if (memcmp (pMIDIMessage, bySysExSMPTEMTC, 5) == 0) {
			/* MIDI���͓������[�h��SMPTE/MTC�ɂ��X���[�u�̏ꍇ�̂� */
			if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVESMPTEMTC) {
				return MIDIClock_PutSysExSMPTEMTC (pMIDIClock, *(pMIDIMessage + 5), 
					*(pMIDIMessage + 6), *(pMIDIMessage + 7), *(pMIDIMessage + 8));
			}
		}
	}

	/* MIDI�^�C���R�[�h�N�H�[�^�[�t���[��(0xF1) */
	else if (*pMIDIMessage == 0xF1) {
		/* MIDI���͓������[�h��SMPTE/MTC�ɂ��X���[�u�̏ꍇ�̂� */
		if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVESMPTEMTC) {
			return MIDIClock_PutSMPTEMTC (pMIDIClock, *(pMIDIMessage + 1));
		}
	}

	/* �\���O�|�W�V�����Z���N�^(0xF2) */
	else if (*pMIDIMessage == 0xF2) {
		/* MIDI���͓������[�h��MIDI�^�C�~���O�N���b�N�ɂ��X���[�u�̏ꍇ�̂� */
		if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) { // TODO:�ۗ�
			long lVal = ((pMIDIMessage[2] & 0x7F) << 7) | (pMIDIMessage[1] & 0x7F);
			long lTickCount = lVal * pMIDIClock->m_lResolution / 4;
			MIDIClock_SetTickCount (pMIDIClock, lTickCount);
		}
	}

	/* MIDI�^�C�~���O�N���b�N(0xF8) */
	else if (*pMIDIMessage == 0xF8) {
		/* MIDI���͓������[�h��MIDI�^�C�~���O�N���b�N�ɂ��X���[�u�̏ꍇ�̂� */
		if (pMIDIClock->m_lMIDIInSyncMode == MIDICLOCK_SLAVEMIDITIMINGCLOCK) {
			if (pMIDIClock->m_lRunning) {
				return MIDIClock_PutMIDITimingClock (pMIDIClock);
			}
		}
	}

	/* �X�^�[�g(0xFA) */
	else if (*pMIDIMessage == 0xFA) {
		MIDIClock_SetTickCount (pMIDIClock, 0);
		MIDIClock_SetMillisec (pMIDIClock, 0);
		MIDIClock_Start (pMIDIClock);
	}

	/* �R���e�B�j���[(0xFB) */
	else if (*pMIDIMessage == 0xFB) {
		MIDIClock_Start (pMIDIClock);
	}

	/* �X�g�b�v(0xFC) */
	else if (*pMIDIMessage == 0xFC) {
		MIDIClock_Stop (pMIDIClock);
	}

	return 1;
}

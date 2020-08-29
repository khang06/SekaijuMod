/******************************************************************************/
/*                                                                            */
/*  MIDIIO.c - MIDI���o�͗p���W���[��(Win32�p)             (C)2002-2016 ����  */
/*                                                                            */
/******************************************************************************/

/* ���̃��W���[���͕��ʂ̂b����ŏ�����Ă���B */
/* ���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��z�z�����B */
/* �v���W�F�N�g�z�[���y�[�W(��)�F"http://openmidiproject.sourceforge.jp/index.html" */
/* MIDI���̓I�u�W�F�N�g�̃I�[�v���E�N���[�Y�E���Z�b�g�E�f�[�^��M(SYSX�܂�) */
/* MIDI�o�̓I�u�W�F�N�g�̃I�[�v���E�N���[�Y�E���Z�b�g�E�f�[�^���M(SYSX�܂�) */

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

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <crtdbg.h>
#include <mmsystem.h>
#include "MIDIIO.h"

/* �ėp�}�N��(�ŏ��A�ő�A���ݍ���) ******************************************/
#ifndef MIN
#define MIN(A,B) ((A)>(B)?(B):(A))
#endif
#ifndef MAX
#define MAX(A,B) ((A)>(B)?(A):(B))
#endif
#ifndef CLIP
#define CLIP(A,B,C) ((A)>(B)?(A):((B)>(C)?(C):(B)))
#endif

/* WSIZEOF�}�N�� **************************************************************/
#define WSIZEOF(STRING) (sizeof(STRING)/sizeof(wchar_t))

/* DLLMain�@*******************************************************************/
BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	/* Windows10�΍� */
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		CoInitializeEx (NULL, COINIT_MULTITHREADED);
		break;
	case DLL_PROCESS_DETACH:
		CoUninitialize ();
	}
	return TRUE;
}

/* MIDI���o�͗��p�֐� *********************************************************/

/* �o�b�t�@�ɑ΂��ĊȈՃN���e�B�J���Z�N�V�����𒣂� */
static void MIDIIO_LockBuf (MIDI* pMIDI) {
	_ASSERT (pMIDI);
	while (pMIDI->m_bBufLocked) {
		Sleep (1);
	}
	pMIDI->m_bBufLocked = 1;
}

/* �o�b�t�@�ɑ΂��ĊȈՃN���e�B�J���Z�N�V�����𔲂��� */
static void MIDIIO_UnLockBuf (MIDI* pMIDI) {
	_ASSERT (pMIDI);
	pMIDI->m_bBufLocked = 0;
}

/* �������u���b�N���e�L�X�g�\������ */
static void MIDIIO_Bin2Txt 
(unsigned char* pBinData, long lBinDataLen, char* pTextData, long lTextDataLen) {
	char szMsg1[256];
	int i;
	int iMax = MIN (lBinDataLen, lTextDataLen / 3);
	memset (pTextData, 0, lTextDataLen);
	for (i = 0; i < iMax; i++) {
		sprintf (szMsg1, "%02X", *(pBinData + i));
		strcat (pTextData, szMsg1);
		if (i != iMax - 1) {
			strcat (pTextData, " ");
		}
	}
}

/******************************************************************************/
/*                                                                            */
/*  MIDIOut�N���X�֐��Q                                                       */
/*                                                                            */
/******************************************************************************/

/* MIDI�o�̓f�o�C�X���J���ꂽ�Ƃ��ɌĂяo�����B(�B��) */
static void MIDIOut_OnMOMOpen 
(HMIDIOUT hMIDIOut, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	;
}

/* MIDI�o�̓f�o�C�X������ꂽ�Ƃ��ɌĂяo�����B(�B��) */
static void MIDIOut_OnMOMClose 
(HMIDIOUT hMIDIOut, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	;
}

/* SYSX�̑��M���I�������Ƃ��ɌĂяo�����B(�B��) */
static void MIDIOut_OnMOMDone 
(HMIDIOUT hMIDIOut, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	MIDIHDR* pMIDIHdr = (MIDIHDR*)(dwParam1);
	MIDIOut* pMIDIOut = (MIDIOut*)(dwInstance);
	int i;
	int nRet;
	if (pMIDIOut == NULL) {
		return;
	}
	/* ���Ԗڂ�SYSX�w�b�_�[�����M�������������o���� */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		if (pMIDIOut->m_pSysxHeader[i] == pMIDIHdr) {
			break;
		}
	}
	if (i >= MIDIIO_SYSXNUM) {
		_RPTF0 (_CRT_WARN, "�x���FMIDIOut_OnMOMDone���ŏo���s����Sysx�f�[�^�����o���܂����B\n");
		return;
	}
	/* �g�p�ς�SYSX�w�b�_�[�̕s����(20070106) */
	nRet = midiOutUnprepareHeader 
		(pMIDIOut->m_pDeviceHandle, ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i])), sizeof(MIDIHDR));
	if (nRet != MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_OnMOMDone����midiOutUnprepareHeader��%d��Ԃ��܂����B\n", nRet);
	}
	/* �g�p�ς�SYSX�w�b�_�[�̉��(20070520) */
	HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]))->lpData);
	HeapFree (GetProcessHeap (), 0, (MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]));
	pMIDIOut->m_pSysxHeader[i] = NULL;
	
	/* TODO �{���R�[���o�b�N�֐����ŃV�X�e���R�[���͋֎~�ł��邪�A */
	/* ��LmidiOutUnprepareHeader��HeapFree�Ɍ���A���̂Ƃ�����͔������Ă��Ȃ��B */
}

/* MIDI�o�̓f�o�C�X���烁�b�Z�[�W���󂯎��R�[���o�b�N�֐��B(�B��) */
static void CALLBACK MidiOutProc(HMIDIOUT hMIDIOut, UINT wMsg,         
  DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	switch (wMsg) {
	case MOM_OPEN:
		_RPTF3 (_CRT_WARN, "MOM_OPEN, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIOut_OnMOMOpen (hMIDIOut, dwInstance, dwParam1, dwParam2);
		break;
	case MOM_CLOSE:
		_RPTF3 (_CRT_WARN, "MOM_CLOSE, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIOut_OnMOMClose (hMIDIOut, dwInstance, dwParam1, dwParam2);
		break;
	case MOM_DONE:
		_RPTF3 (_CRT_WARN, "MOM_DONE, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIOut_OnMOMDone (hMIDIOut, dwInstance, dwParam1, dwParam2);
		break;
	}
}


/* MIDI�o�̓f�o�C�X�̐��𒲂ׂ� */
long __stdcall MIDIOut_GetDeviceNum () {
	return midiOutGetNumDevs ();
}

/* MIDI�o�̓f�o�C�X�̖��O�𒲂ׂ�(ANSI) */
long __stdcall MIDIOut_GetDeviceNameA (long lID, char* pszDeviceName, long lLen) {
	int nRet;
	MIDIOUTCAPSA tagMIDIOutCaps;
	memset (&tagMIDIOutCaps, 0, sizeof (MIDIOUTCAPSA));
	nRet = midiOutGetDevCapsA (lID, &tagMIDIOutCaps, sizeof(MIDIOUTCAPSA));
	if (nRet != MMSYSERR_NOERROR) {
		return 0;
	}
	memset (pszDeviceName, 0, lLen);
	lLen = MIN (lLen - 1, (long)strlen (tagMIDIOutCaps.szPname));
	strncpy (pszDeviceName, tagMIDIOutCaps.szPname, lLen);
	return lLen;
}

/* MIDI�o�̓f�o�C�X�̖��O�𒲂ׂ�(UNICODE) */
long __stdcall MIDIOut_GetDeviceNameW (long lID, wchar_t* pszDeviceName, long lLen) {
	int nRet;
	MIDIOUTCAPSW tagMIDIOutCaps;
	memset (&tagMIDIOutCaps, 0, sizeof (MIDIOUTCAPSW));
	nRet = midiOutGetDevCapsW (lID, &tagMIDIOutCaps, sizeof(MIDIOUTCAPSW));
	if (nRet != MMSYSERR_NOERROR) {
		return 0;
	}
	memset (pszDeviceName, 0, lLen * sizeof (wchar_t));
	lLen = MIN (lLen - 1, (long)wcslen (tagMIDIOutCaps.szPname));
	wcsncpy (pszDeviceName, tagMIDIOutCaps.szPname, lLen);
	return lLen;
}


/* MIDI�o�̓f�o�C�X���J��(ANSI) */
MIDIOut* __stdcall MIDIOut_OpenA (const char* pszDeviceName) {
	MIDIOUTCAPSA tagMIDIOutCaps;
	MIDI* pMIDIOut;
	int nNumDevices, i, nRet;
	if (pszDeviceName == NULL) {
		return NULL;
	}
	/* �f�o�C�X�����󕶎��񖔂�"None"����"�Ȃ�"�̏ꍇ�͉����J���Ȃ� */
	if (*pszDeviceName == 0) {
		return NULL;
	}
	else if (strcmp (pszDeviceName, MIDIIO_NONE) == 0 ||
		strcmp (pszDeviceName, MIDIIO_NONEJ) == 0) {
		return NULL;
	}
	/* �f�o�C�X����"default"��"�f�t�H���g"��"MIDI Mapper"��"MIDI �}�b�p�["�ł���ꍇ�� */
	/* �����I��MIDI�}�b�p�[(-1)�Ƃ���B */
	else if (strcmp (pszDeviceName, MIDIIO_DEFAULT) == 0 ||
		strcmp (pszDeviceName, MIDIIO_DEFAULTJ) == 0 ||
		strcmp (pszDeviceName, MIDIIO_MIDIMAPPER) == 0 ||
		strcmp (pszDeviceName, MIDIIO_MIDIMAPPERJ) == 0) {
		i = -1;
		strncpy (tagMIDIOutCaps.szPname, pszDeviceName, 32);
	}
	/* �f�o�C�X������͂����Ԗڂ�MIDI�o�̓f�o�C�X������ */
	else {
		nNumDevices = midiOutGetNumDevs ();
		for (i = 0; i < nNumDevices; i++) {
			memset (&tagMIDIOutCaps, 0, sizeof (MIDIOUTCAPSA));
			nRet = midiOutGetDevCapsA (i, &tagMIDIOutCaps, sizeof(MIDIOUTCAPSA));
			if (strncmp (pszDeviceName, tagMIDIOutCaps.szPname, 32) == 0) {
				break;
			}
		}
		if (i >= nNumDevices) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_OpenA����MIDI�o�̓f�o�C�X��������܂���B-%s\n", pszDeviceName);
			return NULL;
		}
	}
	/* MIDIOut�\���̂̊m�� */
	pMIDIOut = calloc (1, sizeof (MIDI));
	if (pMIDIOut == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_OpenA����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	pMIDIOut->m_pDeviceName = calloc (strlen (pszDeviceName) + 1, sizeof (char));
	if (pMIDIOut->m_pDeviceName == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_OpenA����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	strncpy ((char*)(pMIDIOut->m_pDeviceName), pszDeviceName, strlen (pszDeviceName) + 1);
	pMIDIOut->m_lMode = MIDIIO_MODEOUT;
	/* MIDI�o�̓f�o�C�X�̃I�[�v�� */
	nRet = midiOutOpen ((HMIDIOUT*)&(pMIDIOut->m_pDeviceHandle), i, 
		(unsigned long)MidiOutProc, (DWORD)pMIDIOut, CALLBACK_FUNCTION);
	if (nRet != 0) {
		free (pMIDIOut->m_pBuf);
		free (pMIDIOut);
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_OpenA����midiOutOpen��%d��Ԃ��܂����B\n", nRet);
		return NULL;
	}
	/* SYSX�w�b�_�[�̗̈�ݒ�(20070520) */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		pMIDIOut->m_pSysxHeader[i] = NULL;
	}
	return pMIDIOut;
}

/* MIDI�o�̓f�o�C�X���J��(UNICODE) */
MIDIOut* __stdcall MIDIOut_OpenW (const wchar_t* pszDeviceName) {
	MIDIOUTCAPSW tagMIDIOutCaps;
	MIDI* pMIDIOut;
	int nNumDevices, i, nRet;
	if (pszDeviceName == NULL) {
		return NULL;
	}
	/* �f�o�C�X�����󕶎��񖔂�"None"����"�Ȃ�"�̏ꍇ�͉����J���Ȃ� */
	if (*pszDeviceName == 0) {
		return NULL;
	}
	else if (wcscmp (pszDeviceName, L"(None)") == 0 ||
		wcscmp (pszDeviceName, L"(�Ȃ�)") == 0) {
		return NULL;
	}
	/* �f�o�C�X����"default"��"�f�t�H���g"��"MIDI Mapper"��"MIDI �}�b�p�["�ł���ꍇ�� */
	/* �����I��MIDI�}�b�p�[(-1)�Ƃ���B */
	else if (wcscmp (pszDeviceName, L"Default") == 0 ||
		wcscmp (pszDeviceName, L"�f�t�H���g") == 0 ||
		wcscmp (pszDeviceName, L"MIDI Mapper") == 0 ||
		wcscmp (pszDeviceName, L"MIDI �}�b�p�[") == 0) {
		i = -1;
		wcsncpy (tagMIDIOutCaps.szPname, pszDeviceName, 32);
	}
	/* �f�o�C�X������͂����Ԗڂ�MIDI�o�̓f�o�C�X������ */
	else {
		nNumDevices = midiOutGetNumDevs ();
		for (i = 0; i < nNumDevices; i++) {
			memset (&tagMIDIOutCaps, 0, sizeof (MIDIOUTCAPSW));
			nRet = midiOutGetDevCapsW (i, &tagMIDIOutCaps, sizeof(MIDIOUTCAPSW));
			if (wcsncmp (pszDeviceName, tagMIDIOutCaps.szPname, 32) == 0) {
				break;
			}
		}
		if (i >= nNumDevices) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_OpenW����MIDI�o�̓f�o�C�X��������܂���B-%s\n", pszDeviceName);
			return NULL;
		}
	}
	/* MIDIOut�\���̂̊m�� */
	pMIDIOut = calloc (1, sizeof (MIDI));
	if (pMIDIOut == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_OpenW����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	pMIDIOut->m_pDeviceName = calloc (wcslen (pszDeviceName) + 1, sizeof (wchar_t));
	if (pMIDIOut->m_pDeviceName == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_OpenW����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	wcsncpy ((wchar_t*)(pMIDIOut->m_pDeviceName), pszDeviceName, wcslen (pszDeviceName) + 1);
	pMIDIOut->m_lMode = MIDIIO_MODEOUT;
	/* MIDI�o�̓f�o�C�X�̃I�[�v�� */
	nRet = midiOutOpen ((HMIDIOUT*)&(pMIDIOut->m_pDeviceHandle), i, 
		MidiOutProc, pMIDIOut, CALLBACK_FUNCTION);
	if (nRet != 0) {
		free (pMIDIOut->m_pBuf);
		free (pMIDIOut);
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_OpenW����midiOutOpen��%d��Ԃ��܂����B\n", nRet);
		return NULL;
	}
	/* SYSX�w�b�_�[�̗̈�ݒ�(20070520) */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		pMIDIOut->m_pSysxHeader[i] = NULL;
	}
	return pMIDIOut;
}


/* MIDI�o�̓f�o�C�X����� */
long __stdcall MIDIOut_Close (MIDIOut* pMIDIOut) {
	int nRet;
	int i, t;
	if (pMIDIOut == NULL) {
		return 1;
	}
	_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
	/* MIDI�o�̓��Z�b�g */
	midiOutReset (pMIDIOut->m_pDeviceHandle);
	/* ���ׂĂ�SYSX�w�b�_�[�ɑ΂���MOM_DONE��҂�(100�񎎍s) */
	for (t = 0; t < 100; t++) {
		for (i = 0; i < MIDIIO_SYSXNUM; i++) {
			if (pMIDIOut->m_pSysxHeader[i] != NULL) {
				break;
			}
		}
		if (i >= MIDIIO_SYSXNUM) {
			break;
		}
		Sleep (10);
	}
	if (t == 100) {
		_RPTF0 (_CRT_WARN, "�x���FMIDIOut_Close����MOM_DONE���R�[���o�b�N����Ă��Ȃ��o�b�t�@������܂��B\n");
	}
	/* MIDI�o�̓f�o�C�X����� */
	nRet = midiOutClose (pMIDIOut->m_pDeviceHandle);
	if (nRet != MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_Close����midiOutClose��%d��Ԃ��܂����B\n", nRet);
		return 0;
	}
	/* ���̃I�u�W�F�N�g�̉�� */
	free (pMIDIOut->m_pBuf);
	pMIDIOut->m_pBuf = NULL;
	free (pMIDIOut->m_pDeviceName);
	pMIDIOut->m_pDeviceName = NULL;
	free (pMIDIOut);
	return 1;
}

/* MIDI�o�̓f�o�C�X���ĂъJ��(ANSI) */
MIDIOut* __stdcall MIDIOut_ReopenA (MIDIOut* pMIDIOut, const char* pszDeviceName) {
	int nRet = 0;
	if (pMIDIOut) {
		_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
		nRet = MIDIOut_Close (pMIDIOut);
		if (nRet == 0) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_ReopenA����MIDIOut_Close��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}
	}
	return MIDIOut_OpenA (pszDeviceName);
}

/* MIDI�o�̓f�o�C�X���ĂъJ��(UNICODE) */
MIDIOut* __stdcall MIDIOut_ReopenW (MIDIOut* pMIDIOut, const wchar_t* pszDeviceName) {
	int nRet = 0;
	if (pMIDIOut) {
		_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
		nRet = MIDIOut_Close (pMIDIOut);
		if (nRet == 0) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_ReopenW����MIDIOut_Close��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}
	}
	return MIDIOut_OpenW (pszDeviceName);
}

/* MIDI�o�̓f�o�C�X�����Z�b�g���� */
long __stdcall MIDIOut_Reset (MIDIOut* pMIDIOut) {
	int nRet = 0;
	_ASSERT (pMIDIOut);
	_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
	nRet = midiOutReset (pMIDIOut->m_pDeviceHandle);
	if (nRet != MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_Reset����midiOutReset��%d��Ԃ��܂����B\n", nRet);
		return 0;
	}
	return 1;
}

/* MIDI�o�̓f�o�C�X��MIDI���b�Z�[�W��1�o�͂��� */
long __stdcall MIDIOut_PutMIDIMessage (MIDIOut* pMIDIOut, unsigned char* pMessage, long lLen) {
	_ASSERT (pMIDIOut);
	_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
	_ASSERT (pMessage);
	_ASSERT (1 <= lLen && lLen < 256);
	/* �V�X�e���G�N�X�N���[�V�����b�Z�[�W */
	if (lLen >= 1 && *pMessage == 0xF0) {
		int nRet;
		int i;
		/* �󂢂Ă���SysxHeader��T���B */
		for (i = 0; i < MIDIIO_SYSXNUM; i++) {
			if (pMIDIOut->m_pSysxHeader[i] == NULL) {
				break;
			}
		}
		if (i >= MIDIIO_SYSXNUM) {
			_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_PutMIDIMessage�Ŏg�p�\��SYSX�w�b�_�[�����͂₠��܂���B\n");
			return 0;
		}
		/* SysxHeader�̃o�b�t�@�m�� */
		(MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]) = 
			(MIDIHDR*)HeapAlloc(GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof (MIDIHDR));
		if ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]) == NULL) {
			_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_PutMIDIMessage��HeapAlloc��NULL��Ԃ��܂����B\n");
			return 0;
		}
		((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]))->lpData = 
			(char*)HeapAlloc (GetProcessHeap (), HEAP_NO_SERIALIZE, MIN (lLen, MIDIIO_SYSXSIZE));
		if (((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]))->lpData == NULL) {
			_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_PutMIDIMessage��HeapAlloc��NULL��Ԃ��܂����B\n");
			return 0;
		}
		/* SYSX�w�b�_�[�����̃f�[�^�p�̈��MIDI���b�Z�[�W���R�s�[ */
		((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]))->dwBufferLength = MIN (lLen, MIDIIO_SYSXSIZE);
		memcpy (((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]))->lpData, pMessage, MIN (lLen, MIDIIO_SYSXSIZE));
		((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]))->dwBytesRecorded = MIN (lLen, MIDIIO_SYSXSIZE);
		/* midiOutPrepareHeader */
		nRet = midiOutPrepareHeader 
			(pMIDIOut->m_pDeviceHandle, ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i])), sizeof (MIDIHDR));
		if (nRet != MMSYSERR_NOERROR) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_PutMIDIMessage����midiOutPrepareHeader��%d��Ԃ��܂����B\n", nRet);
			return 0;
		}
		/* midiOutLongMsg...���̊֐����s���ォ�����MOM_DONE���R�[���o�b�N����� */
		nRet = midiOutLongMsg 
			(pMIDIOut->m_pDeviceHandle, ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i])), sizeof(MIDIHDR));
		if (nRet != MMSYSERR_NOERROR) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIOut_PutMIDIMessage����midiOutLongMsg��%d��Ԃ��܂����B\n", nRet);
			nRet = midiOutUnprepareHeader 
				(pMIDIOut->m_pDeviceHandle, ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i])), sizeof (MIDIHDR));
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i]))->lpData);
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIOut->m_pSysxHeader[i])));
			pMIDIOut->m_pSysxHeader[i] = NULL;
			return 0;
		}
		return lLen;
	}
	/* �ʏ��MIDI���b�Z�[�W���̓V�X�e�����A���^�C�����b�Z�[�W���̓V�X�e���R�������b�Z�[�W */
	else if (1 <= lLen && lLen <= 3) {
		unsigned long lMsg = 0;
		switch (lLen) {
		case 1:
			lMsg = *(pMessage) ;
			break;
		case 2:
			lMsg = *(pMessage) + (*(pMessage + 1) << 8);
			break;
		case 3:
			lMsg = *(pMessage) + (*(pMessage + 1) << 8) + (*(pMessage + 2) << 16);
			break;
		}
		midiOutShortMsg (pMIDIOut->m_pDeviceHandle, lMsg);
		return lLen;
	}
	/* �ُ��MIDI���b�Z�[�W */
	else {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_PutMIDIMessage���ňُ��MIDI���b�Z�[�W�����o����܂����B");
		return 0;
	}
}

/* MIDI�o�̓f�o�C�X��1�o�C�g�o�͂��� */
long __stdcall MIDIOut_PutByte (MIDIOut* pMIDIOut, unsigned char c) {
	_ASSERT (pMIDIOut);
	_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
	midiOutShortMsg (pMIDIOut->m_pDeviceHandle, (unsigned long)c);
	return 1;
}

/* MIDI�o�̓f�o�C�X�ɔC�Ӓ��̃f�[�^���o�͂��� */
long __stdcall MIDIOut_PutBytes (MIDIOut* pMIDIOut, unsigned char* pData, long lLen) {
	unsigned char cRunningStatus = 0;
	long lRet = 0;
	long lLen2 = 0;
	char* p1 = pData;
	_ASSERT (pMIDIOut);
	_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
	if (pMIDIOut == NULL) {
		return 0;
	}
	if (pMIDIOut->m_lMode != MIDIIO_MODEOUT) {
		return 0;
	}
	while (p1 < pData + lLen) {
		if (*p1 < 0x80) {
			if (0x80 <= cRunningStatus && cRunningStatus <= 0xBF ||
				0xE0 <= cRunningStatus && cRunningStatus <= 0xEF) { 
				lLen2 = MIN (2, pData + lLen - p1);
				lRet += MIDIOut_PutMIDIMessage (pMIDIOut, p1, lLen2);
				p1 += lLen2;
			}
			else if (0xC0 <= cRunningStatus && cRunningStatus <= 0xDF) {
				lLen2 = MIN (1, pData + lLen - p1);
				lRet += MIDIOut_PutMIDIMessage (pMIDIOut, p1, lLen2);
				p1 += lLen2;
			}
			else if (cRunningStatus == 0xF0) {
				char* p2 = p1;
				lLen2 = 0;
				cRunningStatus = 0xF0;
				while (*p2++ != 0xF7 && lLen2 < MIDIIO_SYSXSIZE) {
					lLen2++;
				}	
				lRet += MIDIOut_PutMIDIMessage (pMIDIOut, p1, (lLen2 + 1));
				p1 += (lLen2 + 1);
			}
			else {
				while (*p1 < 0x80 || p1 < pData + lLen) {
					p1++;	
				}
				lLen2 = 0;
				_RPTF0 (_CRT_WARN, "�G���[�FMIDIOut_PutBytes���ňُ��MIDI���b�Z�[�W�����o���܂����B\n");
			}
		}
		else if (*p1 == 0xF0) {
			char* p2 = p1;
			lLen2 = 0;
			cRunningStatus = 0xF0;
			while (*p2++ != 0xF7 && lLen2 < MIDIIO_SYSXSIZE) {
				lLen2++;
			}
			lRet += MIDIOut_PutMIDIMessage (pMIDIOut, p1, (lLen2 + 1));
			p1 += (lLen2 + 1);
		}
		else {
			if (0x80 <= *p1 && *p1 <= 0xBF ||
				0xE0 <= *p1 && *p1 <= 0xEF || *p1 == 0xF2) {
				lLen2 = MIN (3, pData + lLen - p1);
			}
			else if (0xC0 <= *p1 && *p1 <= 0xDF ||
				0xF1 == *p1 || *p1 == 0xF3) {
				lLen2 = MIN (2, pData + lLen - p1);
			}
			else if (0xF4 <= *p1 && *p1 <= 0xFF) {
				lLen2 = MIN (1, pData + lLen - p1);
			}
			lRet += MIDIOut_PutMIDIMessage (pMIDIOut, p1, lLen2);
			cRunningStatus = *p1;
			p1 += lLen2;
		}
	}
	return lRet;
}

/* ����MIDI���̓f�o�C�X�̖��O���擾����(ANSI)(20120415�ǉ�) */
long __stdcall MIDIOut_GetThisDeviceNameA (MIDIOut* pMIDIOut, char* pszDeviceName, long lLen) {
	_ASSERT (pMIDIOut);
	_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
	memset (pszDeviceName, 0, lLen);
	lLen = CLIP (0, (long)strlen (pMIDIOut->m_pDeviceName), lLen - 1);
	strncpy (pszDeviceName, pMIDIOut->m_pDeviceName, lLen);
	return lLen;
}

/* ����MIDI���̓f�o�C�X�̖��O���擾����(UNICODE)(20120415�ǉ�) */
long __stdcall MIDIOut_GetThisDeviceNameW (MIDIOut* pMIDIOut, wchar_t* pszDeviceName, long lLen) {
	_ASSERT (pMIDIOut);
	_ASSERT (pMIDIOut->m_lMode == MIDIIO_MODEOUT);
	memset (pszDeviceName, 0, lLen * sizeof (wchar_t));
	lLen = CLIP (0, (long)wcslen (pMIDIOut->m_pDeviceName), lLen - 1);
	wcsncpy (pszDeviceName, pMIDIOut->m_pDeviceName, lLen);
	return lLen;
}

/******************************************************************************/
/*                                                                            */
/*  MIDIIn�N���X�֐��Q                                                        */
/*                                                                            */
/******************************************************************************/


/* MIDI���̓f�o�C�X���J���ꂽ�Ƃ��ɌĂяo�����B(�B��) */
static void MIDIIn_OnMIMOpen 
(HMIDIIN hMIDIn, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	;
}

/* MIDI���̓f�o�C�X������ꂽ�Ƃ��ɌĂяo�����B(�B��) */
static void MIDIIn_OnMIMClose 
(HMIDIIN hMIDIIn, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	;
}

/* MIDI�f�[�^(SYSX����)�����͂��ꂽ�Ƃ��ɌĂяo�����B(�B��) */
static void MIDIIn_OnMIMData 
(HMIDIIN hMIDIIn, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	MIDI* pMIDIIn = (MIDI*)dwInstance;
	unsigned char cData[4];
	long lLen = 0;
	cData[0] = (unsigned char)(dwParam1 & 0x000000FF);
	cData[1] = (unsigned char)((dwParam1 & 0x0000FF00) >> 8);
	cData[2] = (unsigned char)((dwParam1 & 0x00FF0000) >> 16);
	cData[3] = (unsigned char)((dwParam1 & 0xFF000000) >> 24);

	if (0x80 <= cData[0] && cData[0] <= 0xBF ||
		0xE0 <= cData[0] && cData[0] <= 0xEF || cData[0] == 0xF2) { // 20090627�C��
		lLen = 3;
	}
	else if (0xC0 <= cData[0] && cData[0] <= 0xDF ||
		cData[0] == 0xF1 || cData[0] == 0xF3) { // 20090626�C��
		lLen = 2;
	}
	else if (cData[0] == 0xF6 || 0xF8 <= cData[0] && cData[0] <= 0xFF) { // 20090626�C��
		lLen = 1;
	}
	/* SYSX��M��MIM_LONGDATA�̎d���ł��邽�߁A�����ł̓G���[�Ƃ݂Ȃ��B */
	else if (cData[0]== 0xF0) { // 20090626�C��
		lLen = 0;
		_RPTF0 (_CRT_WARN, "�x���FMIDIIn_OnMIMData����Sysx�����o���܂����B\n");
	}
	else { // 20090626�ǉ�
		lLen = 0;
		_RPTF0 (_CRT_WARN, "�x���FMIDIIn_OnMIMData���ŕs���Ȏ�ނ�MIDI���b�Z�[�W�����o���܂����B\n");
	}

	if (pMIDIIn->m_lWritePosition + lLen < pMIDIIn->m_lBufSize) {
		memcpy (pMIDIIn->m_pBuf + pMIDIIn->m_lWritePosition, &cData[0], lLen);
		pMIDIIn->m_lWritePosition += lLen;
	}
	else if (lLen < pMIDIIn->m_lBufSize) {
		memcpy (pMIDIIn->m_pBuf + pMIDIIn->m_lWritePosition, &cData[0], 
			pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition);
		memcpy (pMIDIIn->m_pBuf, &cData[pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition],
			lLen - (pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition));
		pMIDIIn->m_lWritePosition = lLen - (pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition);
	}
	else {
		_RPTF0 (_CRT_WARN, "�x���FMIDIIn_OnMIMData���Ńo�b�t�@�T�C�Y��蒷���f�[�^�����o���܂����B\n");
	}
	/* TODO �{���R�[���o�b�N�֐����ŃV�X�e���R�[���͋֎~�ł��邪�A */
	/* ��Lmemcpy�Ɍ���A���̂Ƃ�����͔������Ă��Ȃ��B */
}

/* MIDI�f�[�^(SYSX)�����͂��ꂽ�Ƃ��ɌĂяo����� */
static void MIDIIn_OnMIMLongData 
(HMIDIIN hMIDIIn, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	MIDI* pMIDIIn = (MIDI*)dwInstance;
	MIDIHDR* pMIDIHeader = (MIDIHDR*)dwParam1;
	unsigned char* pData = pMIDIHeader->lpData;
	long lLen = pMIDIHeader->dwBytesRecorded;
	unsigned long nRet;
	int i;
	/* ���Ԗڂ�SYSX�w�b�_�[����M�������������o���� */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		if (pMIDIIn->m_pSysxHeader[i] == pMIDIHeader) {
			break;
		}
	}
	if (i >= MIDIIO_SYSXNUM) {
		_RPTF0 (_CRT_WARN, "�x���FMIDIIn_OnMIMLongData���ŏo���s����Sysx�f�[�^�����o���܂����B\n");
	}
	/* ���̓f�[�^���o�b�t�@�փR�s�[ */
	MIDIIO_LockBuf (pMIDIIn);
#ifdef _DEBUG
	{
		char szMsg[16384];
		MIDIIO_Bin2Txt (pData, MIN (lLen, MIDIIO_SYSXSIZE), szMsg, sizeof (szMsg));
		_RPTF1 (_CRT_WARN, "���FMIDIIn_OnMIMLongData����Sysx����M���܂����B{%s}\n", szMsg);
	}
#endif
	if (pMIDIIn->m_lWritePosition + lLen < pMIDIIn->m_lBufSize) {
		memcpy (pMIDIIn->m_pBuf + pMIDIIn->m_lWritePosition, pData, lLen);
		pMIDIIn->m_lWritePosition += lLen;
	}
	else if (lLen < pMIDIIn->m_lBufSize) {
		memcpy (pMIDIIn->m_pBuf + pMIDIIn->m_lWritePosition, pData, 
			pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition);
		memcpy (pMIDIIn->m_pBuf, pData + pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition,
			lLen - (pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition));
		pMIDIIn->m_lWritePosition = lLen - (pMIDIIn->m_lBufSize - pMIDIIn->m_lWritePosition);
	}
	else {
		_RPTF0 (_CRT_WARN, "�x���FMIDIIn_OnMIMLongData���Ńo�b�t�@�T�C�Y��蒷��Sysx�f�[�^�����o���܂����B\n");
	}
	MIDIIO_UnLockBuf (pMIDIIn);

	/* midiInStart����midiInUnprepareHeader�ł��Ȃ� */
	
	/* ���łɓ��͒�~���Ă���ꍇ��return */
	if (pMIDIIn->m_bStarting == 0) {
		return;
	}
	/* ���͌p�����̏ꍇ�͎g�p�ς�SYSX�w�b�_�[���ė��p���� */
	/* �g�p�ς�SYSX�w�b�_�[�̍ď��� */
	nRet = midiInPrepareHeader (hMIDIIn, pMIDIHeader, sizeof (MIDIHDR));
	if (nRet != MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OnMIMLongData����midiInPrepareHeader��%d��Ԃ��܂����B\n", nRet);
		return;
	}
	/* �g�p�ς�SYSX�w�b�_�[�̍ēo�^ */
	midiInAddBuffer (hMIDIIn, pMIDIHeader, sizeof (MIDIHDR));
	if (nRet != MMSYSERR_NOERROR) {
		midiInUnprepareHeader (pMIDIIn->m_pDeviceHandle, pMIDIHeader, sizeof (MIDIHDR));
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OnMIMLongData����midiInAddBuffer��%d��Ԃ��܂����B\n", nRet);
	}
	/* TODO �{���R�[���o�b�N�֐����ŃV�X�e���R�[���͋֎~�ł��邪�A */
	/* ��Lmemcpy,midiInPrepareHeader,midiInAddBuffer�Ɍ���A���̂Ƃ�����͔������Ă��Ȃ��B */

}

static void MIDIIn_OnMIMLongError 
(HMIDIIN hMIDIIn, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {

}

static void MIDIIn_OnMIMError
(HMIDIIN hMIDIIn, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {

}

/* MIDI���̓f�o�C�X���烁�b�Z�[�W���󂯎��R�[���o�b�N�֐��B(�B��) */
static void CALLBACK MidiInProc (HMIDIIN hMIDIIn, UINT wMsg,         
  DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	switch (wMsg) {
	case MIM_OPEN:
		_RPTF3 (_CRT_WARN, "MIM_OPEN, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIIn_OnMIMOpen (hMIDIIn, dwInstance, dwParam1, dwParam2);
		break;
	case MIM_CLOSE:
		_RPTF3 (_CRT_WARN, "MIM_CLOSE, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIIn_OnMIMClose (hMIDIIn, dwInstance, dwParam1, dwParam2);
		break;
	case MIM_DATA:
		//_RPTF3 (_CRT_WARN, "MIM_DATA, 0x%08X, 0x%08X, 0x%08X\n", 
		//	dwInstance, dwParam1, dwParam2);
		MIDIIn_OnMIMData (hMIDIIn, dwInstance, dwParam1, dwParam2);
		break;
	case MIM_LONGDATA:
		_RPTF3 (_CRT_WARN, "MIM_LONGDATA, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIIn_OnMIMLongData (hMIDIIn, dwInstance, dwParam1, dwParam2);
		break;
	case MIM_LONGERROR:
		_RPTF3 (_CRT_WARN, "MIM_LONGERROR, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIIn_OnMIMLongError (hMIDIIn, dwInstance, dwParam1, dwParam2);
		break;
	case MIM_ERROR:
		_RPTF3 (_CRT_WARN, "MIM_ERROR, 0x%08X, 0x%08X, 0x%08X\n", 
			dwInstance, dwParam1, dwParam2);
		MIDIIn_OnMIMError (hMIDIIn, dwInstance, dwParam1, dwParam2);
		break;
	}
}

/* MIDI���̓f�o�C�X�̐��𒲂ׂ� */
long __stdcall MIDIIn_GetDeviceNum () {
	return midiInGetNumDevs ();
}

/* MIDI���̓f�o�C�X�̖��O�𒲂ׂ�(ANSI) */
long __stdcall MIDIIn_GetDeviceNameA (long lID, char* pszDeviceName, long lLen) {
	int nRet;
	MIDIINCAPSA tagMIDIInCaps;
	memset (&tagMIDIInCaps, 0, sizeof (MIDIINCAPSA));
	nRet = midiInGetDevCapsA (lID, &tagMIDIInCaps, sizeof(MIDIINCAPSA));
	if (nRet != MMSYSERR_NOERROR) {
		return 0;
	}
	memset (pszDeviceName, 0, lLen);
	lLen = MIN (lLen - 1, (long)strlen (tagMIDIInCaps.szPname));
	strncpy (pszDeviceName, tagMIDIInCaps.szPname, lLen);
	return lLen;
}

/* MIDI���̓f�o�C�X�̖��O�𒲂ׂ�(UNICODE) */
long __stdcall MIDIIn_GetDeviceNameW (long lID, wchar_t* pszDeviceName, long lLen) {
	int nRet;
	MIDIINCAPSW tagMIDIInCaps;
	memset (&tagMIDIInCaps, 0, sizeof (MIDIINCAPSW));
	nRet = midiInGetDevCapsW (lID, &tagMIDIInCaps, sizeof(MIDIINCAPSW));
	if (nRet != MMSYSERR_NOERROR) {
		return 0;
	}
	memset (pszDeviceName, 0, lLen * sizeof (wchar_t));
	lLen = MIN (lLen - 1, (long)wcslen (tagMIDIInCaps.szPname));
	wcsncpy (pszDeviceName, tagMIDIInCaps.szPname, lLen);
	return lLen;
}


/* MIDI���̓f�o�C�X���J��(ANSI) */
MIDIIn* __stdcall MIDIIn_OpenA (const char* pszDeviceName) {
	MIDIINCAPSA tagMIDIInCaps;
	MIDI* pMIDIIn = NULL;
	MIDIHDR* pMIDIHdr = NULL;
	int nNumDevices, i, nRet;
	if (pszDeviceName == NULL) {
		return NULL;
	}
	/* �f�o�C�X�����󕶎��񖔂�"None"����"�Ȃ�"�̏ꍇ�͉����J���Ȃ� */
	if (*pszDeviceName == 0) {
		return NULL;
	}
	else if (strcmp (pszDeviceName, MIDIIO_NONE) == 0 ||
		strcmp (pszDeviceName, MIDIIO_NONEJ) == 0) {
		return NULL;
	}
	/* �f�o�C�X������͂����Ԗڂ�MIDI�o�̓f�o�C�X������ */
	nNumDevices = midiInGetNumDevs ();
	for (i = 0; i < nNumDevices; i++) {
		memset (&tagMIDIInCaps, 0, sizeof (MIDIINCAPSA));
		nRet = midiInGetDevCapsA (i, &tagMIDIInCaps, sizeof(MIDIINCAPSA));
		if (strncmp (pszDeviceName, tagMIDIInCaps.szPname, 32) == 0)
			break;
	}
	if (i >= nNumDevices) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����MIDI���̓f�o�C�X��������܂���B-%s\n", pszDeviceName);
		return NULL;
	}
	/* MIDIIn�\���̂̊m�� */
	pMIDIIn = calloc (1, sizeof (MIDI));
	if (pMIDIIn == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	pMIDIIn->m_pDeviceName = calloc (strlen (pszDeviceName) + 1, sizeof (char));
	if (pMIDIIn->m_pDeviceName == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	strncpy ((char*)(pMIDIIn->m_pDeviceName), pszDeviceName, strlen (pszDeviceName) + 1);
	pMIDIIn->m_lMode = MIDIIO_MODEIN;

	MIDIIO_LockBuf (pMIDIIn);
	pMIDIIn->m_pBuf = malloc (MIDIIO_BUFSIZE);
	if (pMIDIIn->m_pBuf == NULL) {
		free (pMIDIIn);
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����malloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	pMIDIIn->m_lBufSize = MIDIIO_BUFSIZE;
	MIDIIO_UnLockBuf (pMIDIIn);
	/* MIDI���̓f�o�C�X�̃I�[�v�� */
	nRet = midiInOpen ((HMIDIIN*)&(pMIDIIn->m_pDeviceHandle), i, 
		(unsigned long)MidiInProc, (DWORD)(pMIDIIn), CALLBACK_FUNCTION);
	if (nRet != MMSYSERR_NOERROR) {
		free (pMIDIIn->m_pBuf);
		free (pMIDIIn);
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����midiInOpen��%d��Ԃ��܂����B\n", nRet);
		return NULL;
	}
	/* SYSX�w�b�_�[���̗̈���m�ۂ��� */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		pMIDIIn->m_pSysxHeader[i] = (MIDIHDR*)HeapAlloc (GetProcessHeap (),
			HEAP_ZERO_MEMORY, sizeof (MIDIHDR));
		if (pMIDIIn->m_pSysxHeader[i] == NULL) {
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����HeapAlloc��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}

		((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData = 
			(char*)HeapAlloc (GetProcessHeap (), HEAP_NO_SERIALIZE, MIDIIO_SYSXSIZE);
		if (((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData == NULL) {
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����HeapAlloc��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}
		((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->dwBufferLength = MIDIIO_SYSXSIZE;
	}
	/* SYSX�w�b�_�[�����ׂđҋ@��Ԃɂ���(�X�^�[�g�O����) */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		nRet = midiInPrepareHeader (pMIDIIn->m_pDeviceHandle, pMIDIIn->m_pSysxHeader[i], sizeof (MIDIHDR));
		if (nRet != MMSYSERR_NOERROR) {
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData);
			HeapFree (GetProcessHeap (), 0, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]));		
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����midiInPrepareHeader��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}

		nRet = midiInAddBuffer (pMIDIIn->m_pDeviceHandle, 
			(MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]), sizeof (MIDIHDR));
		if (nRet != MMSYSERR_NOERROR) {
			midiInUnprepareHeader (pMIDIIn->m_pDeviceHandle, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]), sizeof (MIDIHDR));
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData);
			HeapFree (GetProcessHeap (), 0, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]));		
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����midiInAddBuffer��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}
	}
	/* ���͂̊J�n(�X�^�[�g) */
	nRet = midiInStart (pMIDIIn->m_pDeviceHandle);
	if (nRet != MMSYSERR_NOERROR) {
		for (i = 0; i < MIDIIO_SYSXNUM; i++) {
			midiInUnprepareHeader (pMIDIIn->m_pDeviceHandle, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]), sizeof (MIDIHDR));
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData);
			HeapFree (GetProcessHeap (), 0, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]));	
		}
		free (pMIDIIn->m_pBuf);
		free (pMIDIIn);
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenA����midiInStart��%d��Ԃ��܂����B\n", nRet);
		return NULL;
	}
	pMIDIIn->m_bStarting = 1;

	return pMIDIIn;
}


/* MIDI���̓f�o�C�X���J��(UNICODE) */
MIDIIn* __stdcall MIDIIn_OpenW (const wchar_t* pszDeviceName) {
	MIDIINCAPSW tagMIDIInCaps;
	MIDI* pMIDIIn = NULL;
	MIDIHDR* pMIDIHdr = NULL;
	int nNumDevices, i, nRet;
	if (pszDeviceName == NULL) {
		return NULL;
	}
	/* �f�o�C�X�����󕶎��񖔂�"None"����"�Ȃ�"�̏ꍇ�͉����J���Ȃ� */
	if (*pszDeviceName == 0) {
		return NULL;
	}
	else if (wcscmp (pszDeviceName, L"(None)") == 0 ||
		wcscmp (pszDeviceName, L"(�Ȃ�)") == 0) {
		return NULL;
	}
	/* �f�o�C�X������͂����Ԗڂ�MIDI�o�̓f�o�C�X������ */
	nNumDevices = midiInGetNumDevs ();
	for (i = 0; i < nNumDevices; i++) {
		memset (&tagMIDIInCaps, 0, sizeof (MIDIINCAPSW));
		nRet = midiInGetDevCapsW (i, &tagMIDIInCaps, sizeof(MIDIINCAPSW));
		if (wcsncmp (pszDeviceName, tagMIDIInCaps.szPname, 32) == 0)
			break;
	}
	if (i >= nNumDevices) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����MIDI���̓f�o�C�X��������܂���B-%s\n", pszDeviceName);
		return NULL;
	}
	/* MIDIIn�\���̂̊m�� */
	pMIDIIn = calloc (1, sizeof (MIDI));
	if (pMIDIIn == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	pMIDIIn->m_pDeviceName = calloc (wcslen (pszDeviceName) + 1, sizeof (wchar_t));
	if (pMIDIIn->m_pDeviceName == NULL) {
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����calloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	wcsncpy ((wchar_t*)(pMIDIIn->m_pDeviceName), pszDeviceName, wcslen (pszDeviceName) + 1);
	pMIDIIn->m_lMode = MIDIIO_MODEIN;

	MIDIIO_LockBuf (pMIDIIn);
	pMIDIIn->m_pBuf = malloc (MIDIIO_BUFSIZE);
	if (pMIDIIn->m_pBuf == NULL) {
		free (pMIDIIn);
		_RPTF0 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����malloc��NULL��Ԃ��܂����B\n");
		return NULL;
	}
	pMIDIIn->m_lBufSize = MIDIIO_BUFSIZE;
	MIDIIO_UnLockBuf (pMIDIIn);
	/* MIDI���̓f�o�C�X�̃I�[�v�� */
	nRet = midiInOpen ((HMIDIIN*)&(pMIDIIn->m_pDeviceHandle), i, 
		(unsigned long)MidiInProc, (DWORD)(pMIDIIn), CALLBACK_FUNCTION);
	if (nRet != MMSYSERR_NOERROR) {
		free (pMIDIIn->m_pBuf);
		free (pMIDIIn);
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����midiInOpen��%d��Ԃ��܂����B\n", nRet);
		return NULL;
	}
	/* SYSX�w�b�_�[���̗̈���m�ۂ��� */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		pMIDIIn->m_pSysxHeader[i] = (MIDIHDR*)HeapAlloc (GetProcessHeap (),
			HEAP_ZERO_MEMORY, sizeof (MIDIHDR));
		if (pMIDIIn->m_pSysxHeader[i] == NULL) {
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����HeapAlloc��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}

		((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData = 
			(char*)HeapAlloc (GetProcessHeap (), HEAP_NO_SERIALIZE, MIDIIO_SYSXSIZE);
		if (((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData == NULL) {
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����HeapAlloc��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}
		((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->dwBufferLength = MIDIIO_SYSXSIZE;
	}
	/* SYSX�w�b�_�[�����ׂđҋ@��Ԃɂ���(�X�^�[�g�O����) */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		nRet = midiInPrepareHeader (pMIDIIn->m_pDeviceHandle, pMIDIIn->m_pSysxHeader[i], sizeof (MIDIHDR));
		if (nRet != MMSYSERR_NOERROR) {
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData);
			HeapFree (GetProcessHeap (), 0, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]));		
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����midiInPrepareHeader��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}

		nRet = midiInAddBuffer (pMIDIIn->m_pDeviceHandle, 
			(MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]), sizeof (MIDIHDR));
		if (nRet != MMSYSERR_NOERROR) {
			midiInUnprepareHeader (pMIDIIn->m_pDeviceHandle, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]), sizeof (MIDIHDR));
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData);
			HeapFree (GetProcessHeap (), 0, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]));		
			free (pMIDIIn->m_pBuf);
			free (pMIDIIn);
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����midiInAddBuffer��%d��Ԃ��܂����B\n", nRet);
			return NULL;
		}
	}
	/* ���͂̊J�n(�X�^�[�g) */
	nRet = midiInStart (pMIDIIn->m_pDeviceHandle);
	if (nRet != MMSYSERR_NOERROR) {
		for (i = 0; i < MIDIIO_SYSXNUM; i++) {
			midiInUnprepareHeader (pMIDIIn->m_pDeviceHandle, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]), sizeof (MIDIHDR));
			HeapFree (GetProcessHeap (), 0, ((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData);
			HeapFree (GetProcessHeap (), 0, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]));	
		}
		free (pMIDIIn->m_pBuf);
		free (pMIDIIn);
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_OpenW����midiInStart��%d��Ԃ��܂����B\n", nRet);
		return NULL;
	}
	pMIDIIn->m_bStarting = 1;

	return pMIDIIn;
}


/* MIDI���̓f�o�C�X�����B */
long __stdcall MIDIIn_Close (MIDIIn* pMIDIIn) {
	int nRet = 0;
	int i, t;
	if (pMIDIIn == NULL) {
		return 1;
	}
	_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
	/* ���͂̒�~ */
	nRet = midiInStop  (pMIDIIn->m_pDeviceHandle);
	if (nRet != MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_Close����midiInStop��%d��Ԃ��܂����B\n", nRet);
		return 0;
	}
	pMIDIIn->m_bStarting = 0;
	/* MIDI���͂̃��Z�b�g(�����Ŗ����͂�SYSX�o�b�t�@�̓R�[���o�b�N�ɕԂ���MHDR_DONE�t���O���Z�b�g����� */
	nRet = midiInReset (pMIDIIn->m_pDeviceHandle);
	if (nRet != MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_Close����midiInReset��%d��Ԃ��܂����B\n", nRet);
		return 0;
	}
	/* ���ׂĂ�SYSX�w�b�_�[�ɑ΂���MIM_LONGDATA��҂�(100�񎎍s) */
	for (t = 0; t < 100; t++) {
		for (i = 0; i < MIDIIO_SYSXNUM; i++) {
			if ((((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->dwFlags & MHDR_DONE) == 0) {
				break;
			}
		}
		if (i >= MIDIIO_SYSXNUM) {
			break;
		}
		Sleep (10);
	}
	if (t == 100) {
		_RPTF0 (_CRT_WARN, "�x���FMIDIIn_Close����MIM_LONGDATA���R�[���o�b�N����Ă��Ȃ��o�b�t�@������܂��B\n");
	}
	/* SYSX�w�b�_�[�̕s����(�X�g�b�v�㏈��) */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		nRet = midiInUnprepareHeader (pMIDIIn->m_pDeviceHandle, pMIDIIn->m_pSysxHeader[i], sizeof(MIDIHDR));
		if (nRet != MMSYSERR_NOERROR) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_Close����midiInUnprepareHeader��%d��Ԃ��܂����B\n", nRet);
			return 0;
		}
	}
	/* SYSX�w�b�_�[�̉��(�N���[�Y�O����) */
	for (i = 0; i < MIDIIO_SYSXNUM; i++) {
		HeapFree (GetProcessHeap(), 0, ((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData);
		((MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]))->lpData = NULL;
		HeapFree (GetProcessHeap(), 0, (MIDIHDR*)(pMIDIIn->m_pSysxHeader[i]));
		pMIDIIn->m_pSysxHeader[i] = NULL;
	}
	/* MIDI���̓f�o�C�X����� */
	nRet = midiInClose (pMIDIIn->m_pDeviceHandle);
	if (nRet != MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_Close����midiInClose��%d��Ԃ��܂����B\n", nRet);
		return 0;
	}
	/* MIDI���͍\���̂̉�� */
	MIDIIO_LockBuf (pMIDIIn);
	free (pMIDIIn->m_pBuf);
	pMIDIIn->m_pBuf = NULL;
	pMIDIIn->m_lBufSize = 0;
	free (pMIDIIn->m_pDeviceName);
	pMIDIIn->m_pDeviceName = NULL;
	MIDIIO_UnLockBuf (pMIDIIn);
	free (pMIDIIn);
	return 1;
}

/* MIDI���̓f�o�C�X���ĂъJ��(ANSI)�B */
MIDIIn* __stdcall MIDIIn_ReopenA (MIDIIn* pMIDIIn, const char* pszDeviceName) {
	int nRet = 0;
	if (pMIDIIn) {
		_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
		nRet = MIDIIn_Close (pMIDIIn);
		if (nRet == 0) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_ReopenA����MIDIIn_Close��%d��Ԃ��܂����B\n", nRet);
			return 0;
		}
		pMIDIIn = NULL;
	}
	return MIDIIn_OpenA (pszDeviceName);
}

/* MIDI���̓f�o�C�X���ĂъJ��(UNICODE)�B */
MIDIIn* __stdcall MIDIIn_ReopenW (MIDIIn* pMIDIIn, const wchar_t* pszDeviceName) {
	int nRet = 0;
	if (pMIDIIn) {
		_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
		nRet = MIDIIn_Close (pMIDIIn);
		if (nRet == 0) {
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_ReopenW����MIDIIn_Close��%d��Ԃ��܂����B\n", nRet);
			return 0;
		}
		pMIDIIn = NULL;
	}
	return MIDIIn_OpenW (pszDeviceName);
}


/* MIDI���̓f�o�C�X�����Z�b�g����B */
long __stdcall MIDIIn_Reset (MIDIIn* pMIDIIn) {
	int nRet = 0;
	_ASSERT (pMIDIIn);
	_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
	nRet = midiInReset (pMIDIIn->m_pDeviceHandle);
	if (nRet == MMSYSERR_NOERROR) {
		_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_Reset����midiInReset��%d��Ԃ��܂����B\n", nRet);
		return 0;
	}
	return 1;
}

/* MIDI���̓f�o�C�X����MIDI���b�Z�[�W��1���͂��� */
long __stdcall MIDIIn_GetMIDIMessage (MIDIIn* pMIDIIn, unsigned char* pMessage, long lLen) {
	unsigned char cType;
	long lMessageLen, lCopyLen;
	unsigned char* p;
	_ASSERT (pMIDIIn);
	_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
	_ASSERT (pMessage);
	/* ��荞�ރf�[�^���o�b�t�@��ɂ��� */
	if (pMIDIIn->m_lReadPosition != pMIDIIn->m_lWritePosition) {
		cType = *(pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition);
		/* �m�[�g�E�`�����l���A�t�^�[�E�R���g���[���E�s�b�`�x���h */	
		if (0x80 <= cType && cType <= 0xBF ||
			0xE0 <= cType && cType <= 0xEF) {
			pMIDIIn->m_byRunningStatus = cType; // 20090627�ǉ�
			lMessageLen = 3;
		}
		/* �v���O�����E�L�[�A�t�^�[�^�b�` */
		else if (0xC0 <= cType && cType <= 0xDF) {
			pMIDIIn->m_byRunningStatus = cType; // 20090627�ǉ�
			lMessageLen = 2;
		}
		/* �V�X�e�����A���^�C�����b�Z�[�W */
		else if (0xF8 <= cType && cType <= 0xFF) {
			lMessageLen = 1;
		}
		/* �V�X�e���G�N�X�N���[�V�����b�Z�[�W */
		else if (cType == 0xF0) { // 20090627�C��
			lMessageLen = 0;
			p = pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition;
			do {
				p++;
				lMessageLen++;
				if (p >= pMIDIIn->m_pBuf + pMIDIIn->m_lBufSize) {
					p = pMIDIIn->m_pBuf;
				}
			} while (*p <= 0x7F);
			lMessageLen++;
		}
		/* MIDI�^�C���R�[�h�N�H�[�^�[�t���[���E�\���O�Z���N�g */
		else if (cType == 0xF1 || cType == 0xF3) { // 20090627�ǉ�
			lMessageLen = 2;
		}
		/* �\���O�|�W�V�����Z���N�^ */
		else if (cType == 0xF2) { // 20090627�ǉ�
			lMessageLen = 3;
		}
		/* �`���[�����N�G�X�g */
		else if (cType == 0xF6) { // 20090627�ǉ�
			lMessageLen = 1;
		}
		/* ��`����Ă��Ȃ��V�X�e���R�������b�Z�[�W */
		else if ((cType & 0xF0) == 0xF0) { // 20090627�ǉ�
			_RPTF1 (_CRT_WARN, "�G���[�FMIDIIn_GetMIDIMessage���Œ�`����Ă��Ȃ�"
				"MIDI���b�Z�[�W�����o���܂���(%d)�B\n", cType);
			lMessageLen = 1;
		}
		/* �����j���O�X�e�[�^�X���ȗ�����Ă���ꍇ(0x00<=cType<=0x7F) */
		else {
			if ((pMIDIIn->m_byRunningStatus & 0xF0) == 0xC0 || // 20090627�ǉ�
				(pMIDIIn->m_byRunningStatus & 0xF0) == 0xD0) {
				lMessageLen = 1;
			}
			else {
				lMessageLen = 2;
			}
		}
		lCopyLen = MIN (lMessageLen, lLen);
		MIDIIO_LockBuf (pMIDIIn);
		if (pMIDIIn->m_lReadPosition + lMessageLen < pMIDIIn->m_lBufSize) {
			memcpy (pMessage, pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition, lCopyLen);
			pMIDIIn->m_lReadPosition += lMessageLen;
		}
		else {
			memcpy (pMessage, pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition, 
				pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition); 
			memcpy (pMessage + (pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition),
				pMIDIIn->m_pBuf,
				lCopyLen - (pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition));
			pMIDIIn->m_lReadPosition = lMessageLen - 
				(pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition);
		}
		MIDIIO_UnLockBuf (pMIDIIn);
		return lCopyLen;
	}
	/* ��荞�ރf�[�^���o�b�t�@��ɂȂ� */
	else {
		return 0;
	}
}

/* MIDI���̓f�o�C�X����1�o�C�g���͂��� */
long __stdcall MIDIIn_GetByte (MIDIIn* pMIDIIn, unsigned char* p) {
	long lRet;
	_ASSERT (pMIDIIn);
	_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
	_ASSERT (p);
	MIDIIO_LockBuf (pMIDIIn);
	if (pMIDIIn->m_lReadPosition != pMIDIIn->m_lWritePosition) {
		*p = *(pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition);
		pMIDIIn->m_lReadPosition++;
		if (pMIDIIn->m_lReadPosition >= pMIDIIn->m_lBufSize) {
			pMIDIIn->m_lReadPosition = 0;
		}
		lRet = 1;
	}
	else {
		lRet = 0;
	}
	MIDIIO_UnLockBuf (pMIDIIn);
	return lRet;
}

/* MIDI���̓f�o�C�X����C�Ӓ��̃f�[�^����͂��� */
long __stdcall MIDIIn_GetBytes (MIDIIn* pMIDIIn, unsigned char* pData, long lLen) {
	unsigned char cType;
	long lLen2, lCopyLen;
	_ASSERT (pMIDIIn);
	_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
	_ASSERT (pData);
	/* ��荞�ރf�[�^���o�b�t�@��ɂ��� */
	if (pMIDIIn->m_lReadPosition != pMIDIIn->m_lWritePosition) {
		cType = *(pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition);
		lLen2 = pMIDIIn->m_lWritePosition - pMIDIIn->m_lReadPosition;
		if (lLen2 < 0) {
			lLen2 += MIDIIO_BUFSIZE;
		}
		lCopyLen = MIN (lLen, lLen2);
		MIDIIO_LockBuf (pMIDIIn);
		if (pMIDIIn->m_lReadPosition + lCopyLen < pMIDIIn->m_lBufSize) {
			memcpy (pData, pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition, lCopyLen);
			pMIDIIn->m_lReadPosition += lCopyLen;
		}
		else {
			memcpy (pData, pMIDIIn->m_pBuf + pMIDIIn->m_lReadPosition, 
				pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition); 
			memcpy (pData + (pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition),
				pMIDIIn->m_pBuf,
				lCopyLen - (pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition));
			pMIDIIn->m_lReadPosition = lCopyLen - 
				(pMIDIIn->m_lBufSize - pMIDIIn->m_lReadPosition);
		}
		MIDIIO_UnLockBuf (pMIDIIn);
		return lCopyLen;
	}
	/* ��荞�ރf�[�^���o�b�t�@��ɂȂ� */
	else {
		return 0;
	}
}

/* ����MIDI���̓f�o�C�X�̖��O���擾����(ANSI)(20120415�ǉ�) */
long __stdcall MIDIIn_GetThisDeviceNameA (MIDIIn* pMIDIIn, char* pszDeviceName, long lLen) {
	_ASSERT (pMIDIIn);
	_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
	memset (pszDeviceName, 0, lLen);
	lLen = CLIP (0, (long)strlen (pMIDIIn->m_pDeviceName), lLen - 1);
	strncpy (pszDeviceName, pMIDIIn->m_pDeviceName, lLen);
	return lLen;
}

/* ����MIDI���̓f�o�C�X�̖��O���擾����(UNICODE)(20120415�ǉ�) */
long __stdcall MIDIIn_GetThisDeviceNameW (MIDIIn* pMIDIIn, wchar_t* pszDeviceName, long lLen) {
	_ASSERT (pMIDIIn);
	_ASSERT (pMIDIIn->m_lMode == MIDIIO_MODEIN);
	memset (pszDeviceName, 0, lLen * sizeof (wchar_t));
	lLen = CLIP (0, (long)wcslen (pMIDIIn->m_pDeviceName), lLen - 1);
	wcsncpy (pszDeviceName, pMIDIIn->m_pDeviceName, lLen);
	return lLen;
}

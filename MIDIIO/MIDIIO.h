/******************************************************************************/
/*                                                                            */
/*  MIDIIO.h - MIDIIO�w�b�_�[�t�@�C��                      (C)2002-2012 ����  */
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

#if !defined _MIDIIO_H_
#define _MIDIIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MIDIIO_BUFSIZE 16384
#define MIDIIO_SYSXNUM 4
#define MIDIIO_SYSXSIZE 4096
#define MIDIIO_MODEIN  0x0000
#define MIDIIO_MODEOUT 0x0001

/* ���̃}�N�����g�p���邱�Ƃɂ��A*/
/* �W��C����̃t�@�C�����o�͂Ɠ������o��MIDI���o�͂�������B */
#define MIDIIO_NONE "(None)"
#define MIDIIO_NONEJ "(�Ȃ�)"
#define MIDIIO_DEFAULT "Default"
#define MIDIIO_DEFAULTJ "�f�t�H���g"
#define MIDIIO_MIDIMAPPER "MIDI Mapper"
#define MIDIIO_MIDIMAPPERJ "MIDI �}�b�p�["

#ifdef UNICODE 
#define _MIDIIO_T(x) L ## x
#else
#define _MIDIIO_T(x) (x)
#endif
#define mopen(NAME,MODE) (*(MODE)==_MIDIIO_T('r')?MIDIIn_Open(NAME):MIDIOut_Open(NAME))
#define mclose(MIDI) ((MIDI)->m_lMode==0?MIDIIn_Close(MIDI):MIDIOut_Close(MIDI))
#define mflush(MIDI) ((MIDI)->m_lMode==0?MIDIIn_Reset(MIDI):MIDIOut_Reset(MIDI))
#define mreopen(NAME,MODE,MIDI) (*(MODE)==_MIDIIO_T('r')?MIDIIn_ReOpen((MIDI),(NAME)):MIDIOut_ReOpen((MIDI),(NAME)))
#define mgets(PBUF,LEN,MIDI) (MIDIIn_GetMIDIMessage((MIDI),(PBUF),(LEN)))
#define mputs(PBUF,LEN,MIDI) (MIDIOut_PutMIDIMessage((MIDI),(PBUF),(LEN)))
#define mgetc(MIDI,CHAR) (MIDIIn_GetByte((MIDI),(CHAR)))
#define mputc(MIDI,CHAR) (MIDIOut_PutByte((MIDI),(CHAR)))
#define mread(PBUF,LEN,MIDI) (MIDIIn_GetBytes((MIDI),(PBUF),(LEN)))
#define mwrite(PBUF,LEN,MIDI) (MIDIOut_PutBytes((MIDI),(PBUF),(LEN)))

/* MIDI�\���� */
typedef struct tagMIDI {
	void* m_pDeviceHandle;
	void* m_pDeviceName; /* 20120124�^�ƒ������C�� */
	long  m_lMode;
	void* m_pSysxHeader[MIDIIO_SYSXNUM];
	long  m_bStarting;
	unsigned char* m_pBuf;
	long  m_lBufSize;
	long  m_lReadPosition;
	long  m_lWritePosition;
	long  m_bBufLocked;
	unsigned char m_byRunningStatus;
} MIDI;

/* DLLMain */
//BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

/******************************************************************************/
/*                                                                            */
/*  MIDIOut�N���X�֐��Q                                                       */
/*                                                                            */
/******************************************************************************/

#define MIDIOut MIDI

/* MIDI�o�̓f�o�C�X�̐��𒲂ׂ� */
long __stdcall MIDIOut_GetDeviceNum ();

/* MIDI�o�̓f�o�C�X�̖��O�𒲂ׂ� */
long __stdcall MIDIOut_GetDeviceNameA (long lID, char* pszDeviceName, long lLen);
long __stdcall MIDIOut_GetDeviceNameW (long lID, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIOut_GetDeviceName MIDIOut_GetDeviceNameW
#else
#define MIDIOut_GetDeviceName MIDIOut_GetDeviceNameA
#endif

/* MIDI�o�̓f�o�C�X���J�� */
MIDIOut* __stdcall MIDIOut_OpenA (const char* pszDeviceName);
MIDIOut* __stdcall MIDIOut_OpenW (const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIOut_Open MIDIOut_OpenW
#else
#define MIDIOut_Open MIDIOut_OpenA
#endif

/* MIDI�o�̓f�o�C�X����� */
long __stdcall MIDIOut_Close (MIDIOut* pMIDIDevice);

/* MIDI�o�̓f�o�C�X���ĂъJ�� */
MIDIOut* __stdcall MIDIOut_ReopenA (MIDIOut* pMIDIOut, const char* pszDeviceName);
MIDIOut* __stdcall MIDIOut_ReopenW (MIDIOut* pMIDIOut, const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIOut_Reopen MIDIOut_ReopenW
#else
#define MIDIOut_Reopen MIDIOut_ReopenA
#endif

/* MIDI�o�̓f�o�C�X�����Z�b�g���� */
long __stdcall MIDIOut_Reset (MIDIOut* pMIDIDevice);

/* MIDI�o�̓f�o�C�X��1���b�Z�[�W�o�͂��� */
long __stdcall MIDIOut_PutMIDIMessage (MIDIOut* pMIDI, unsigned char* pMessage, long lLen);

/* MIDI�o�̓f�o�C�X��1�o�C�g���o�͂��� */
long __stdcall MIDIOut_PutByte (MIDIOut* pMIDI, unsigned char cByte);

/* MIDI�o�̓f�o�C�X�ɔC�Ӓ��̃o�C�i���f�[�^���o�͂��� */
long __stdcall MIDIOut_PutBytes (MIDIOut* pMIDI, unsigned char* pBuf, long lLen);

/* ����MIDI�o�̓f�o�C�X�̖��O���擾����(20120415�ǉ�) */
long __stdcall MIDIOut_GetThisDeviceNameA (MIDIOut* pMIDIOut, char* pszDeviceName, long lLen);
long __stdcall MIDIOut_GetThisDeviceNameW (MIDIOut* pMIDIOut, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIOut_GetThisDeviceName MIDIOut_GetThisDeviceNameW
#else
#define MIDIOut_GetThisDeviceName MIDIOut_GetThisDeviceNameA
#endif

/******************************************************************************/
/*                                                                            */
/*  MIDIIn�N���X�֐��Q                                                        */
/*                                                                            */
/******************************************************************************/

#define MIDIIn  MIDI

/* MIDI���̓f�o�C�X�̐��𒲂ׂ� */
long __stdcall MIDIIn_GetDeviceNum ();

/* MIDI���̓f�o�C�X�̖��O�𒲂ׂ� */
long __stdcall MIDIIn_GetDeviceNameA (long lID, char* pszDeviceName, long lLen);
long __stdcall MIDIIn_GetDeviceNameW (long lID, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIIn_GetDeviceName MIDIIn_GetDeviceNameW
#else
#define MIDIIn_GetDeviceName MIDIIn_GetDeviceNameA
#endif

/* MIDI���̓f�o�C�X���J�� */
MIDIIn* __stdcall MIDIIn_OpenA (const char* pszDeviceName);
MIDIIn* __stdcall MIDIIn_OpenW (const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIIn_Open MIDIIn_OpenW
#else
#define MIDIIn_Open MIDIIn_OpenA
#endif

/* MIDI���̓f�o�C�X����� */
long __stdcall MIDIIn_Close (MIDIIn* pMIDIDevice);

/* MIDI���̓f�o�C�X���ĂъJ�� */
MIDIIn* __stdcall MIDIIn_ReopenA (MIDIIn* pMIDIIn, const char* pszDeviceName);
MIDIIn* __stdcall MIDIIn_ReopenW (MIDIIn* pMIDIIn, const wchar_t* pszDeviceName);
#ifdef UNICODE
#define MIDIIn_Reopen MIDIIn_ReopenW
#else
#define MIDIIn_Reopen MIDIIn_ReopenA
#endif

/* MIDI���̓f�o�C�X�����Z�b�g���� */
long __stdcall MIDIIn_Reset (MIDIIn* pMIDIDevice);

/* MIDI���̓f�o�C�X����1���b�Z�[�W���͂��� */
long __stdcall MIDIIn_GetMIDIMessage (MIDIIn* pMIDIIn, unsigned char* pMessage, long lLen);

/* MIDI���̓f�o�C�X����1�o�C�g���͂��� */
long __stdcall MIDIIn_GetByte (MIDIIn* pMIDIIn, unsigned char* pByte);

/* MIDI���̓f�o�C�X����C�Ӓ��̃o�C�i���f�[�^����͂��� */
long __stdcall MIDIIn_GetBytes (MIDIIn* pMIDIIn, unsigned char* pBuf, long lLen);

/* ����MIDI���̓f�o�C�X�̖��O���擾����(20120415�ǉ�) */
long __stdcall MIDIIn_GetThisDeviceNameA (MIDIIn* pMIDIIn, char* pszDeviceName, long lLen);
long __stdcall MIDIIn_GetThisDeviceNameW (MIDIIn* pMIDIIn, wchar_t* pszDeviceName, long lLen);
#ifdef UNICODE
#define MIDIIn_GetThisDeviceName MIDIIn_GetThisDeviceNameW
#else
#define MIDIIn_GetThisDeviceName MIDIIn_GetThisDeviceNameA
#endif


#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/
/*                                                                            */
/*�@MIDIInstrument.h - MIDIInstrument�w�b�_�[�t�@�C��      (C)2007-2012 ����  */
/*                                                                            */
/******************************************************************************/

/* ���̃��W���[���͕��ʂ�C����ŏ�����Ă���B */
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

#ifndef _MIDIINSTRUMENT_H_
#define _MIDIINSTRUMENT_H_

/* C++������g�p�\�Ƃ��� */
#ifdef __cplusplus
extern "C" {
#endif

#define MIDIINSTRUMENTDEFINITION_MAXPATCHNAME 256
#define MIDIINSTRUMENTDEFINITION_MAXNOTENAME 256

/* MIDIPatchNameTable�\���� */
typedef struct tagMIDIPatchNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[128];
	long  m_lNameLen[128];
	struct tagMIDIPatchNameTable* m_pBasedOn;
	struct tagMIDIPatchNameTable* m_pNextPatchNameTable;
	struct tagMIDIPatchNameTable* m_pPrevPatchNameTable;
	void* m_pParent;
} MIDIPatchNameTable;

/* MIDINoteNameTable�\���� */
typedef struct tagMIDINoteNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[128];
	long  m_lNameLen[128];
	struct tagMIDINoteNameTable* m_pBasedOn;
	struct tagMIDINoteNameTable* m_pNextNoteNameTable;
	struct tagMIDINoteNameTable* m_pPrevNoteNameTable;
	void* m_pParent;
} MIDINoteNameTable;

/* MIDIContorollerName�\���� */
typedef struct tagMIDIControllerNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[128];
	long  m_lNameLen[128];
	struct tagMIDIControllerNameTable* m_pBasedOn;
	struct tagMIDIControllerNameTable* m_pNextControllerNameTable;
	struct tagMIDIControllerNameTable* m_pPrevControllerNameTable;
	void* m_pParent;
} MIDIControllerNameTable;

/* MIDIRPNNameTable�\���� */
typedef struct tagMIDIRPNNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[16384];
	long  m_lNameLen[16384];
	struct tagMIDIRPNNameTable* m_pBasedOn;
	struct tagMIDIRPNNameTable* m_pNextRPNNameTable;
	struct tagMIDIRPNNameTable* m_pPrevRPNNameTable;
	void* m_pParent;
} MIDIRPNNameTable;

/* MIDINRPNNameTable�\���� */
typedef struct tagMIDINRPNNameTable {
	void* m_pszTitle;
	long  m_lTitleLen;
	void* m_pszName[16384];
	long  m_lNameLen[16384];
	struct tagMIDINRPNNameTable* m_pBasedOn;
	struct tagMIDINRPNNameTable* m_pNextNRPNNameTable;
	struct tagMIDINRPNNameTable* m_pPrevNRPNNameTable;
	void* m_pParent;
} MIDINRPNNameTable;

/* MIDIInstrumentDefinion�\���� */
typedef struct tagMIDIInstrumentDefinition {
	void* m_pszTitle;
	long m_lTitleLen;
	MIDIControllerNameTable* m_pMIDIControllerNameTable;
	MIDIRPNNameTable* m_pMIDIRPNNameTable;
	MIDINRPNNameTable* m_pMIDINRPNNameTable;
	long m_lMIDIPatchNameNum;
	long m_lMIDIPatchNameBank[MIDIINSTRUMENTDEFINITION_MAXPATCHNAME];
	MIDIPatchNameTable* m_pMIDIPatchNameTable[MIDIINSTRUMENTDEFINITION_MAXPATCHNAME];
	long m_lMIDINoteNameNum;
	long m_lMIDINoteNameBank[MIDIINSTRUMENTDEFINITION_MAXNOTENAME];
	long m_lMIDINoteNameProgram[MIDIINSTRUMENTDEFINITION_MAXNOTENAME];
	MIDINoteNameTable* m_pMIDINoteNameTable[MIDIINSTRUMENTDEFINITION_MAXNOTENAME];
	struct tagMIDIInstrumentDefinition* m_pNextInstrumentDefinition;
	struct tagMIDIInstrumentDefinition* m_pPrevInstrumentDefinition;
	void* m_pParent;
} MIDIInstrumentDefinition;

/* MIDIInstrument�\���� */
typedef struct tagMIDIInstrument {
	MIDIPatchNameTable* m_pFirstPatchNameTable;
	MIDIPatchNameTable* m_pLastPatchNameTable;
	MIDINoteNameTable* m_pFirstNoteNameTable;
	MIDINoteNameTable* m_pLastNoteNameTable;
	MIDIControllerNameTable *m_pFirstControllerNameTable;
	MIDIControllerNameTable *m_pLastControllerNameTable;
	MIDIRPNNameTable* m_pFirstRPNNameTable;
	MIDIRPNNameTable* m_pLastRPNNameTable;
	MIDINRPNNameTable* m_pFirstNRPNNameTable;
	MIDINRPNNameTable* m_pLastNRPNNameTable;
	MIDIInstrumentDefinition* m_pFirstInstrumentDefinition;
	MIDIInstrumentDefinition* m_pLastInstrumentDefinition;
} MIDIInstrument;

/******************************************************************************/
/*                                                                            */
/*�@MIDIPatchNameTable�N���X�֐��Q                                            */
/*                                                                            */
/******************************************************************************/

/* MIDIPatchNameTable�I�u�W�F�N�g�̐��� */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_Create ();

/* MIDIPatchNameTable�I�u�W�F�N�g�̍폜 */
void __stdcall MIDIPatchNameTable_Delete (MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIPatchNameTable�I�u�W�F�N�g�̃^�C�g����ݒ� */
long __stdcall MIDIPatchNameTable_SetTitleA 
(MIDIPatchNameTable* pMIDIPatchNameTable, const char* pszTitle);
long __stdcall MIDIPatchNameTable_SetTitleW 
(MIDIPatchNameTable* pMIDIPatchNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIPatchNameTable_SetTitle MIDIPatchNameTable_SetTitleW
#else 
#define MIDIPatchNameTable_SetTitle MIDIPatchNameTable_SetTitleA
#endif

/* MIDIPatchNameTable�I�u�W�F�N�g�̊e���O��ݒ� */
long __stdcall MIDIPatchNameTable_SetNameA
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, const char* pszName);
long __stdcall MIDIPatchNameTable_SetNameW 
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDIPatchNameTable_SetName MIDIPatchNameTable_SetNameW
#else
#define MIDIPatchNameTable_SetName MIDIPatchNameTable_SetNameA
#endif

/* MIDIPatchNameTable�I�u�W�F�N�g�̃^�C�g�����擾 */
long __stdcall MIDIPatchNameTable_GetTitleA 
(MIDIPatchNameTable* pMIDIPatchNameTable, char* pszTitle, long lLen);
long __stdcall MIDIPatchNameTable_GetTitleW 
(MIDIPatchNameTable* pMIDIPatchNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIPatchNameTable_GetTitle MIDIPatchNameTable_GetTitleW
#else
#define MIDIPatchNameTable_GetTitle MIDIPatchNameTable_GetTitleA
#endif

/* MIDIPatchNameTable�I�u�W�F�N�g�̊e���O���擾 */
long __stdcall MIDIPatchNameTable_GetNameA
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDIPatchNameTable_GetNameW
(MIDIPatchNameTable* pMIDIPatchNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDIPatchNameTable_GetName MIDIPatchNameTable_GetNameW
#else
#define MIDIPatchNameTable_GetName MIDIPatchNameTable_GetNameA
#endif
 
/* ����MIDIPatchNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_GetNextPatchNameTable 
(MIDIPatchNameTable* pMIDIPatchNameTable);

/* �O��MIDIPatchNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIPatchNameTable* __stdcall MIDIPatchNameTable_GetPrevPatchNameTable 
(MIDIPatchNameTable* pMIDIPatchNameTable);

/* �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrument* __stdcall MIDIPatchNameTable_GetParent
(MIDIPatchNameTable* pMIDIPatchNameTable);

/******************************************************************************/
/*                                                                            */
/*�@MIDINoteNameTable�N���X�֐��Q                                             */
/*                                                                            */
/******************************************************************************/

/* MIDINoteNameTable�I�u�W�F�N�g�̐��� */
MIDINoteNameTable* __stdcall MIDINoteNameTable_Create ();

/* MIDINoteNameTable�I�u�W�F�N�g�̍폜 */
void __stdcall MIDINoteNameTable_Delete (MIDINoteNameTable* pMIDINoteNameTable);

/* MIDINoteNameTable�I�u�W�F�N�g�̃^�C�g����ݒ� */
long __stdcall MIDINoteNameTable_SetTitleA
(MIDINoteNameTable* pMIDINoteNameTable, const char* pszTitle);
long __stdcall MIDINoteNameTable_SetTitleW
(MIDINoteNameTable* pMIDINoteNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDINoteNameTable_SetTitle MIDINoteNameTable_SetTitleW
#else
#define MIDINoteNameTable_SetTitle MIDINoteNameTable_SetTitleA
#endif

/* MIDINoteNameTable�I�u�W�F�N�g�̊e���O��ݒ� */
long __stdcall MIDINoteNameTable_SetNameA 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, const char* pszName);
long __stdcall MIDINoteNameTable_SetNameW 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDINoteNameTable_SetName MIDINoteNameTable_SetNameW
#else
#define MIDINoteNameTable_SetName MIDINoteNameTable_SetNameA
#endif

/* MIDINoteNameTable�I�u�W�F�N�g�̃^�C�g�����擾 */
long __stdcall MIDINoteNameTable_GetTitleA 
(MIDINoteNameTable* pMIDINoteNameTable, char* pszTitle, long lLen);
long __stdcall MIDINoteNameTable_GetTitleW
(MIDINoteNameTable* pMIDINoteNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDINoteNameTable_GetTitle MIDINoteNameTable_GetTitleW
#else
#define MIDINoteNameTable_GetTitle MIDINoteNameTable_GetTitleA
#endif

/* MIDINoteNameTable�I�u�W�F�N�g�̊e���O���擾 */
long __stdcall MIDINoteNameTable_GetNameA 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDINoteNameTable_GetNameW 
(MIDINoteNameTable* pMIDINoteNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDINoteNameTable_GetName MIDINoteNameTable_GetNameW
#else
#define MIDINoteNameTable_GetName MIDINoteNameTable_GetNameA
#endif

/* ����MIDINoteNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDINoteNameTable* __stdcall MIDINoteNameTable_GetNextNoteNameTable 
(MIDINoteNameTable* pMIDINoteNameTable);

/* �O��MIDINoteNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDINoteNameTable* __stdcall MIDINoteNameTable_GetPrevNoteNameTable 
(MIDINoteNameTable* pMIDINoteNameTable);

/* �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrument* __stdcall MIDINoteNameTable_GetParent
(MIDINoteNameTable* pMIDINoteNameTable);

/******************************************************************************/
/*                                                                            */
/*�@MIDIControllerNameTable�N���X�֐��Q                                       */
/*                                                                            */
/******************************************************************************/

/* MIDIControllerNameTable�I�u�W�F�N�g�̐��� */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_Create ();

/* MIDIControllerNameTable�I�u�W�F�N�g�̍폜 */
void __stdcall MIDIControllerNameTable_Delete (MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIControllerNameTable�I�u�W�F�N�g�̃^�C�g����ݒ� */
long __stdcall MIDIControllerNameTable_SetTitleA 
(MIDIControllerNameTable* pMIDIControllerNameTable, const char* pszTitle);
long __stdcall MIDIControllerNameTable_SetTitleW 
(MIDIControllerNameTable* pMIDIControllerNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIControllerNameTable_SetTitle MIDIControllerNameTable_SetTitleW
#else
#define MIDIControllerNameTable_SetTitle MIDIControllerNameTable_SetTitleA
#endif

/* MIDIControllerNameTable�I�u�W�F�N�g�̊e���O��ݒ� */
long __stdcall MIDIControllerNameTable_SetNameA 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, const char* pszName);
long __stdcall MIDIControllerNameTable_SetNameW 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDIControllerNameTable_SetName MIDIControllerNameTable_SetNameW
#else
#define MIDIControllerNameTable_SetName MIDIControllerNameTable_SetNameA
#endif

/* MIDIControllerNameTable�I�u�W�F�N�g�̃^�C�g�����擾 */
long __stdcall MIDIControllerNameTable_GetTitleA 
(MIDIControllerNameTable* pMIDIControllerNameTable, char* pszTitle, long lLen);
long __stdcall MIDIControllerNameTable_GetTitleW 
(MIDIControllerNameTable* pMIDIControllerNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIControllerNameTable_GetTitle MIDIControllerNameTable_GetTitleW
#else
#define MIDIControllerNameTable_GetTitle MIDIControllerNameTable_GetTitleA
#endif

/* MIDIControllerNameTable�I�u�W�F�N�g�̊e���O���擾 */
long __stdcall MIDIControllerNameTable_GetNameA 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDIControllerNameTable_GetNameW 
(MIDIControllerNameTable* pMIDIControllerNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDIControllerNameTable_GetName MIDIControllerNameTable_GetNameW
#else
#define MIDIControllerNameTable_GetName MIDIControllerNameTable_GetNameA
#endif
 
/* ����MIDIControllerNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_GetNextControllerNameTable 
(MIDIControllerNameTable* pMIDIControllerNameTable);

/* �O��MIDIControllerNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIControllerNameTable* __stdcall MIDIControllerNameTable_GetPrevControllerNameTable 
(MIDIControllerNameTable* pMIDIControllerNameTable);

/* �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrument* __stdcall MIDIControllerNameTable_GetParent
(MIDIControllerNameTable* pMIDIControllerNameTable);

/******************************************************************************/
/*                                                                            */
/*�@MIDIRPNNameTable�N���X�֐��Q                                              */
/*                                                                            */
/******************************************************************************/

/* MIDIRPNNameTable�I�u�W�F�N�g�̐��� */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_Create ();

/* MIDIRPNNameTable�I�u�W�F�N�g�̍폜 */
void __stdcall MIDIRPNNameTable_Delete (MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIRPNNameTable�I�u�W�F�N�g�̃^�C�g����ݒ� */
long __stdcall MIDIRPNNameTable_SetTitleA 
(MIDIRPNNameTable* pMIDIRPNNameTable, const char* pszTitle);
long __stdcall MIDIRPNNameTable_SetTitleW 
(MIDIRPNNameTable* pMIDIRPNNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIRPNNameTable_SetTitle MIDIRPNNameTable_SetTitleW
#else
#define MIDIRPNNameTable_SetTitle MIDIRPNNameTable_SetTitleA
#endif

/* MIDIRPNNameTable�I�u�W�F�N�g�̊e���O��ݒ� */
long __stdcall MIDIRPNNameTable_SetNameA 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, const char* pszName);
long __stdcall MIDIRPNNameTable_SetNameW 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDIRPNNameTable_SetName MIDIRPNNameTable_SetNameW
#else
#define MIDIRPNNameTable_SetName MIDIRPNNameTable_SetNameA
#endif

/* MIDIRPNNameTable�I�u�W�F�N�g�̃^�C�g�����擾 */
long __stdcall MIDIRPNNameTable_GetTitleA 
(MIDIRPNNameTable* pMIDIRPNNameTable, char* pszTitle, long lLen);
long __stdcall MIDIRPNNameTable_GetTitleW 
(MIDIRPNNameTable* pMIDIRPNNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIRPNNameTable_GetTitle MIDIRPNNameTable_GetTitleW
#else
#define MIDIRPNNameTable_GetTitle MIDIRPNNameTable_GetTitleA
#endif

/* MIDIRPNNameTable�I�u�W�F�N�g�̊e���O���擾 */
long __stdcall MIDIRPNNameTable_GetNameA 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDIRPNNameTable_GetNameW 
(MIDIRPNNameTable* pMIDIRPNNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDIRPNNameTable_GetName MIDIRPNNameTable_GetNameW
#else
#define MIDIRPNNameTable_GetName MIDIRPNNameTable_GetNameA
#endif
 
/* ����MIDIRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_GetNextRPNNameTable 
(MIDIRPNNameTable* pMIDIRPNNameTable);

/* �O��MIDIRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIRPNNameTable* __stdcall MIDIRPNNameTable_GetPrevRPNNameTable 
(MIDIRPNNameTable* pMIDIRPNNameTable);

/* �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrument* __stdcall MIDIRPNNameTable_GetParent
(MIDIRPNNameTable* pMIDIRPNNameTable);

/******************************************************************************/
/*                                                                            */
/*�@MIDINRPNNameTable�N���X�֐��Q                                             */
/*                                                                            */
/******************************************************************************/

/* MIDINRPNNameTable�I�u�W�F�N�g�̐��� */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_Create ();

/* MIDINRPNNameTable�I�u�W�F�N�g�̍폜 */
void __stdcall MIDINRPNNameTable_Delete (MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDINRPNNameTable�I�u�W�F�N�g�̃^�C�g����ݒ� */
long __stdcall MIDINRPNNameTable_SetTitleA 
(MIDINRPNNameTable* pMIDINRPNNameTable, const char* pszTitle);
long __stdcall MIDINRPNNameTable_SetTitleW 
(MIDINRPNNameTable* pMIDINRPNNameTable, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDINRPNNameTable_SetTitle MIDINRPNNameTable_SetTitleW
#else
#define MIDINRPNNameTable_SetTitle MIDINRPNNameTable_SetTitleA
#endif

/* MIDIRPNNameTable�I�u�W�F�N�g�̊e���O��ݒ� */
long __stdcall MIDINRPNNameTable_SetNameA 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, const char* pszName);
long __stdcall MIDINRPNNameTable_SetNameW 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, const wchar_t* pszName);
#ifdef UNICODE
#define MIDINRPNNameTable_SetName MIDINRPNNameTable_SetNameW
#else
#define MIDINRPNNameTable_SetName MIDINRPNNameTable_SetNameA
#endif

/* MIDINRPNNameTable�I�u�W�F�N�g�̃^�C�g�����擾 */
long __stdcall MIDINRPNNameTable_GetTitleA 
(MIDINRPNNameTable* pMIDINRPNNameTable, char* pszTitle, long lLen);
long __stdcall MIDINRPNNameTable_GetTitleW 
(MIDINRPNNameTable* pMIDINRPNNameTable, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDINRPNNameTable_GetTitle MIDINRPNNameTable_GetTitleW
#else
#define MIDINRPNNameTable_GetTitle MIDINRPNNameTable_GetTitleA
#endif

/* MIDIRPNNameTable�I�u�W�F�N�g�̊e���O���擾 */
long __stdcall MIDINRPNNameTable_GetNameA 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, char* pszName, long lLen);
long __stdcall MIDINRPNNameTable_GetNameW 
(MIDINRPNNameTable* pMIDINRPNNameTable, long lNumber, wchar_t* pszName, long lLen);
#ifdef UNICODE
#define MIDINRPNNameTable_GetName MIDINRPNNameTable_GetNameW
#else
#define MIDINRPNNameTable_GetName MIDINRPNNameTable_GetNameA
#endif
 
/* ����MIDINRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_GetNextNRPNNameTable 
(MIDINRPNNameTable* pMIDINRPNNameTable);

/* �O��MIDINRPNNameTable�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDINRPNNameTable* __stdcall MIDINRPNNameTable_GetPrevNRPNNameTable 
(MIDINRPNNameTable* pMIDINRPNNameTable);

/* �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrument* __stdcall MIDINRPNNameTable_GetParent
(MIDINRPNNameTable* pMIDINRPNNameTable);


/******************************************************************************/
/*                                                                            */
/*�@MIDIInstrumentDefinition�N���X�֐��Q                                      */
/*                                                                            */
/******************************************************************************/

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̐��� */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_Create ();

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̍폜 */
void __stdcall MIDIInstrumentDefinition_Delete (MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃^�C�g����ݒ� */
long __stdcall MIDIInstrumentDefinition_SetTitleA 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, const char* pszTitle);
long __stdcall MIDIInstrumentDefinition_SetTitleW 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrumentDefinition_SetTitle MIDIInstrumentDefinition_SetTitleW
#else
#define MIDIInstrumentDefinition_SetTitle MIDIInstrumentDefinition_SetTitleA
#endif

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃R���g���[���[���e�[�u����ݒ� */
long __stdcall MIDIInstrumentDefinition_SetControllerNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIInstrumentDefinition�I�u�W�F�N�g��RPN���e�[�u����ݒ� */
long __stdcall MIDIInstrumentDefinition_SetRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIInstrumentDefinition�I�u�W�F�N�g��NRPN���e�[�u����ݒ� */
long __stdcall MIDIInstrumentDefinition_SetNRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃p�b�`���e�[�u����ݒ� */
long __stdcall MIDIInstrumentDefinition_SetPatchNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃m�[�g���e�[�u����ݒ� */
long __stdcall MIDIInstrumentDefinition_SetNoteNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, long lProgram, MIDINoteNameTable* pMIDINoteNameTable);

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃^�C�g�����擾 */
long __stdcall MIDIInstrumentDefinition_GetTitleA 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, char* pszTitle, long lLen);
long __stdcall MIDIInstrumentDefinition_GetTitleW 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, wchar_t* pszTitle, long lLen);
#ifdef UNICODE
#define MIDIInstrumentDefinition_GetTitle MIDIInstrumentDefinition_GetTitleW
#else
#define MIDIInstrumentDefinition_GetTitle MIDIInstrumentDefinition_GetTitleA
#endif

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃R���g���[���[���e�[�u�����擾 */
MIDIControllerNameTable* __stdcall MIDIInstrumentDefinition_GetControllerNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinition�I�u�W�F�N�g��RPN���e�[�u�����擾 */
MIDIRPNNameTable* __stdcall MIDIInstrumentDefinition_GetRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinition�I�u�W�F�N�g��NRPN���e�[�u�����擾 */
MIDINRPNNameTable* __stdcall MIDIInstrumentDefinition_GetNRPNNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃p�b�`���e�[�u�����擾 */
MIDIPatchNameTable* __stdcall MIDIInstrumentDefinition_GetPatchNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank);

/* MIDIInstrumentDefinition�I�u�W�F�N�g�̃m�[�g���e�[�u�����擾 */
MIDINoteNameTable* __stdcall MIDIInstrumentDefinition_GetNoteNameTable 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition, long lBank, long lProgram);
 
/* ����MIDIInstrumentDefinition�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_GetNextInstrumentDefinition 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* �O��MIDIInstrumentDefinition�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrumentDefinition_GetPrevInstrumentDefinition 
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* �e��MIDIInstrument�ւ̃|�C���^���擾(�Ȃ����NULL) */
MIDIInstrument* __stdcall MIDIInstrumentDefinition_GetParent
(MIDIInstrumentDefinition* pMIDIInstrumentDefinition);


/*******************************************************************************/
/*                                                                             */
/*�@MIDIInstrument�N���X�֐��Q                                                 */
/*                                                                             */
/*******************************************************************************/

/* MIDIInstrument�I�u�W�F�N�g�̐��� */
MIDIInstrument* __stdcall MIDIInstrument_Create ();

/* MIDIInstrument�I�u�W�F�N�g�̍폜 */
void __stdcall MIDIInstrument_Delete (MIDIInstrument* pMIDIInstrument);

/* MIDIInstrument��MIDIPatchNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���) */
long __stdcall MIDIInstrument_InsertPatchNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIPatchNameTable* pMIDIPatchNameTable, MIDIPatchNameTable* pTargetPatchNameTable);

/* MIDIInstrument��MIDIPatchNameTable��ǉ�(�Ō�ɑ}��) */
long __stdcall MIDIInstrument_AddPatchNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIInstrument����MIDIPatchNameTable������(MIDIPatchNameTable���͍̂폜���Ȃ�) */
long __stdcall MIDIInstrument_RemovePatchNameTable
(MIDIInstrument* pMIDIInstrument, MIDIPatchNameTable* pMIDIPatchNameTable);

/* MIDIInstrument��MIDINoteNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���) */
long __stdcall MIDIInstrument_InsertNoteNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDINoteNameTable* pMIDINoteNameTable, MIDINoteNameTable* pTargetNoteNameTable);

/* MIDIInstrument��MIDINoteNameTable��ǉ�(�Ō�ɑ}��) */
long __stdcall MIDIInstrument_AddNoteNameTable 
(MIDIInstrument* pMIDIInstrument, MIDINoteNameTable* pMIDINoteNameTable);

/* MIDIInstrument����MIDINoteNameTable������(MIDINoteNameTable���͍̂폜���Ȃ�) */
long __stdcall MIDIInstrument_RemoveNoteNameTable
(MIDIInstrument* pMIDIInstrument, MIDINoteNameTable* pMIDINoteNameTable);

/* MIDIInstrument��MIDIControllerNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���) */
long __stdcall MIDIInstrument_InsertControllerNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIControllerNameTable* pMIDIControllerNameTable, MIDIControllerNameTable* pTargetControllerNameTable);

/* MIDIInstrument��MIDIControllerNameTable��ǉ�(�Ō�ɑ}��) */
long __stdcall MIDIInstrument_AddControllerNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIInstrument����MIDIControllerNameTable������(MIDIControllerNameTable���͍̂폜���Ȃ�) */
long __stdcall MIDIInstrument_RemoveControllerNameTable
(MIDIInstrument* pMIDIInstrument, MIDIControllerNameTable* pMIDIControllerNameTable);

/* MIDIInstrument��MIDIRPNNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���) */
long __stdcall MIDIInstrument_InsertRPNNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIRPNNameTable* pMIDIRPNNameTable, MIDIRPNNameTable* pTargetRPNNameTable);

/* MIDIInstrument��MIDIRPNNameTable��ǉ�(�Ō�ɑ}��) */
long __stdcall MIDIInstrument_AddRPNNameTable 
(MIDIInstrument* pMIDIInstrument, MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIInstrument����MIDIRPNNameTable������(MIDIRPNNameTable���͍̂폜���Ȃ�) */
long __stdcall MIDIInstrument_RemoveRPNNameTable
(MIDIInstrument* pMIDIInstrument, MIDIRPNNameTable* pMIDIRPNNameTable);

/* MIDIInstrument��MIDINRPNNameTable��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���) */
long __stdcall MIDIInstrument_InsertNRPNNameTableAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDINRPNNameTable* pMIDINRPNNameTable, MIDINRPNNameTable* pTargetNRPNNameTable);

/* MIDIInstrument��MIDINRPNNameTable��ǉ�(�Ō�ɑ}��) */
long __stdcall MIDIInstrument_AddNRPNNameTable 
(MIDIInstrument* pMIDIInstrument, MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDIInstrument����MIDINRPNNameTable������(MIDINRPNNameTable���͍̂폜���Ȃ�) */
long __stdcall MIDIInstrument_RemoveNRPNNameTable
(MIDIInstrument* pMIDIInstrument, MIDINRPNNameTable* pMIDINRPNNameTable);

/* MIDIInstrument��MIDIInstrumentDefinition��}��(�}���ʒu�͎w��^�[�Q�b�g�̒���) */
long __stdcall MIDIInstrument_InsertInstrumentDefinitionAfter
(MIDIInstrument* pMIDIInstrument, 
 MIDIInstrumentDefinition* pMIDIInstrumentDefinition, MIDIInstrumentDefinition* pTargetInstrumentDefinition);

/* MIDIInstrument��MIDIInstrumentDefinition��ǉ�(�Ō�ɑ}��) */
long __stdcall MIDIInstrument_AddInstrumentDefinition 
(MIDIInstrument* pMIDIInstrument, MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* MIDIInstrument����MIDIInstrumentDefinition������(MIDIInstrumentDefinition���͍̂폜���Ȃ�) */
long __stdcall MIDIInstrument_RemoveInstrumentDefinition
(MIDIInstrument* pMIDIInstrument, MIDIInstrumentDefinition* pMIDIInstrumentDefinition);

/* �ŏ��̃p�b�`���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetFirstPatchNameTable
(MIDIInstrument* pMIDIInstrument);

/* �Ō�̃p�b�`���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetLastPatchNameTable
(MIDIInstrument* pMIDIInstrument);

/* �ŏ��̃m�[�g���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetFirstNoteNameTable
(MIDIInstrument* pMIDIInstrument);

/* �Ō�̃m�[�g���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetLastNoteNameTable
(MIDIInstrument* pMIDIInstrument);

/* �ŏ��̃R���g���[�����e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetFirstControllerNameTable
(MIDIInstrument* pMIDIInstrument);

/* �Ō�̃R���g���[�����e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetLastControllerNameTable
(MIDIInstrument* pMIDIInstrument);

/* �ŏ���RPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetFirstRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* �Ō��RPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetLastRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* �ŏ���NRPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetFirstNRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* �Ō��NRPN���e�[�u���ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetLastNRPNNameTable
(MIDIInstrument* pMIDIInstrument);

/* �ŏ��̃C���X�g�D�������g��`�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetFirstInstrumentDefinition
(MIDIInstrument* pMIDIInstrument);

/* �Ō�̃C���X�g�D�������g��`�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetLastInstrumentDefinition
(MIDIInstrument* pMIDIInstrument);

/* �^�C�g������MIDIPatchNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIPatchNameTable* __stdcall MIDIInstrument_GetPatchNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIPatchNameTable* __stdcall MIDIInstrument_GetPatchNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetPatchNameTableFromTitle MIDIInstrument_GetPatchNameTableFromTitleW
#else
#define MIDIInstrument_GetPatchNameTableFromTitle MIDIInstrument_GetPatchNameTableFromTitleA
#endif

/* �^�C�g������MIDINoteNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDINoteNameTable* __stdcall MIDIInstrument_GetNoteNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDINoteNameTable* __stdcall MIDIInstrument_GetNoteNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetNoteNameTableFromTitle MIDIInstrument_GetNoteNameTableFromTitleW
#else
#define MIDIInstrument_GetNoteNameTableFromTitle MIDIInstrument_GetNoteNameTableFromTitleA
#endif

/* �^�C�g������MIDIControllerNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIControllerNameTable* __stdcall MIDIInstrument_GetControllerNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIControllerNameTable* __stdcall MIDIInstrument_GetControllerNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetControllerNameTableFromTitle MIDIInstrument_GetControllerNameTableFromTitleW
#else
#define MIDIInstrument_GetControllerNameTableFromTitle MIDIInstrument_GetControllerNameTableFromTitleA
#endif

/* �^�C�g������MIDIRPNNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIRPNNameTable* __stdcall MIDIInstrument_GetRPNNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIRPNNameTable* __stdcall MIDIInstrument_GetRPNNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetRPNNameTableFromTitle MIDIInstrument_GetRPNNameTableFromTitleW
#else
#define MIDIInstrument_GetRPNNameTableFromTitle MIDIInstrument_GetRPNNameTableFromTitleA
#endif

/* �^�C�g������MIDINRPNNameTable�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDINRPNNameTable* __stdcall MIDIInstrument_GetNRPNNameTableFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDINRPNNameTable* __stdcall MIDIInstrument_GetNRPNNameTableFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetNRPNNameTableFromTitle MIDIInstrument_GetNRPNNameTableFromTitleW
#else
#define MIDIInstrument_GetNRPNNameTableFromTitle MIDIInstrument_GetNRPNNameTableFromTitleA
#endif

/* �^�C�g������MIDIInstrumentDefinition�ւ̃|�C���^��Ԃ�(�Ȃ����NULL) */
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetInstrumentDefinitionFromTitleA 
(MIDIInstrument* pMIDIInstrument, const char* pszTitle);
MIDIInstrumentDefinition* __stdcall MIDIInstrument_GetInstrumentDefinitionFromTitleW 
(MIDIInstrument* pMIDIInstrument, const wchar_t* pszTitle);
#ifdef UNICODE
#define MIDIInstrument_GetInstrumentDefinitionFromTitle MIDIInstrument_GetInstrumentDefinitionFromTitleW
#else
#define MIDIInstrument_GetInstrumentDefinitionFromTitle MIDIInstrument_GetInstrumentDefinitionFromTitleA
#endif

/* Cakewalk�̃C���X�g�D�������g��`�t�@�C��(*.ins)����MIDI�C���X�g�D�������g��ǂݍ��� */
MIDIInstrument* __stdcall MIDIInstrument_LoadA (const char* pszFileName);
MIDIInstrument* __stdcall MIDIInstrument_LoadW (const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIInstrument_Load MIDIInstrument_LoadW
#else
#define MIDIInstrument_Load MIDIInstrument_LoadA
#endif

/* Cakewalk�̃C���X�g�D�������g��`�t�@�C��(*.ins)����MIDI�C���X�g�D�������g��ǉ��ǂݍ��� */
long __stdcall MIDIInstrument_LoadAdditionalA (MIDIInstrument* pMIDIInstrument, const char* pszFileName);
long __stdcall MIDIInstrument_LoadAdditionalW (MIDIInstrument* pMIDIInstrument, const wchar_t* pszFileName);
#ifdef UNICODE
#define MIDIInstrument_LoadAdditional MIDIInstrument_LoadAdditionalW
#else
#define MIDIInstrument_LoadAdditional MIDIInstrument_LoadAdditionalA
#endif

/* forEachControllerNameTable�}�N�� */
#define forEachControllerNameTable(pMIDIInstrument,pMIDIControllerNameTable) \
	for(pMIDIControllerNameTable=pMIDIInstrument->m_pFirstControllerNameTable;\
		pMIDIControllerNameTable;\
		pMIDIControllerNameTable=pMIDIControllerNameTable->m_pNextControllerNameTable)

/* forEachRPNNameTable�}�N�� */
#define forEachRPNNameTable(pMIDIInstrument,pMIDIRPNNameTable) \
	for(pMIDIRPNNameTable=pMIDIInstrument->m_pFirstRPNNameTable;\
		pMIDIRPNNameTable;\
		pMIDIRPNNameTable=pMIDIRPNNameTable->m_pNextRPNNameTable)

/* forEachNRPNNameTable�}�N�� */
#define forEachNRPNNameTable(pMIDIInstrument,pMIDINRPNNameTable) \
	for(pMIDINRPNNameTable=pMIDIInstrument->m_pFirstNRPNNameTable;\
		pMIDINRPNNameTable;\
		pMIDINRPNNameTable=pMIDINRPNNameTable->m_pNextNRPNNameTable)

/* forEachPatchNameTable�}�N�� */
#define forEachPatchNameTable(pMIDIInstrument,pMIDIPatchNameTable) \
	for(pMIDIPatchNameTable=pMIDIInstrument->m_pFirstPatchNameTable;\
		pMIDIPatchNameTable;\
		pMIDIPatchNameTable=pMIDIPatchNameTable->m_pNextPatchNameTable)

/* forEachNoteNameTable�}�N�� */
#define forEachNoteNameTable(pMIDIInstrument,pMIDINoteNameTable) \
	for(pMIDINoteNameTable=pMIDIInstrument->m_pFirstNoteNameTable;\
		pMIDINoteNameTable;\
		pMIDINoteNameTable=pMIDINoteNameTable->m_pNextNoteNameTable)

/* forEachInstrumentDefinition�}�N�� */
#define forEachInstrumentDefinition(pMIDIInstrument,pMIDIInstrumentDefinition) \
	for(pMIDIInstrumentDefinition=pMIDIInstrument->m_pFirstInstrumentDefinition;\
		pMIDIInstrumentDefinition;\
		pMIDIInstrumentDefinition=pMIDIInstrumentDefinition->m_pNextInstrumentDefinition)


#ifdef __cplusplus
}
#endif


#endif

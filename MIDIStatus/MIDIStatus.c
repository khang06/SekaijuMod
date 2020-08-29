/******************************************************************************/
/*                                                                            */
/*　MIDIStatus.c - MIDIStatusソースファイル                (C)2002-2014 くず  */
/*                                                                            */
/******************************************************************************/

/* このモジュールは普通のＣ言語で書かれている。 */
/* このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づき配布される。 */
/* プロジェクトホームページ："http://openmidiproject.sourceforge.jp/index.html" */

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MIDIStatus.h"

/* 汎用マクロ(最小、最大、挟み込み) */
#ifndef MIN
#define MIN(A,B) ((A)>(B)?(B):(A))
#endif
#ifndef MAX
#define MAX(A,B) ((A)>(B)?(A):(B))
#endif
#ifndef CLIP
#define CLIP(A,B,C) ((A)>(B)?(A):((B)>(C)?(C):(B)))
#endif

static unsigned char g_byGMReset[] = 
	{0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7};
static unsigned char g_byGMOff[] = 
	{0xF0, 0x7E, 0x7F, 0x09, 0x02, 0xF7};
static unsigned char g_byGM2Reset[] = 
	{0xF0, 0x7E, 0x7F, 0x09, 0x03, 0xF7};
static unsigned char g_byGSReset[] = 
	{0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41, 0xF7};
static unsigned char g_by88Reset[] =
	{0xF0, 0x41, 0x10, 0x42, 0x12, 0x00, 0x00, 0x7F, 0x00, 0x01, 0xF7};
static unsigned char g_byXGReset[] = 
	{0xF0, 0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00, 0xF7};

static long g_lGM2MasterReverbDefault[6][2] = {
	{   0,   44}, /* Small Room */
	{   1,   50}, /* Medium Room */
	{   2,   56}, /* Large Room */
	{   3,   64}, /* Medium Hall */
	{   4,   64}, /* Large Hall (デフォルト) */
	{   8,   50}  /* Plate */
};

static long g_lGM2MasterChorusDefault[6][5] = {
	{   0,    0,    3,    5,    0}, /* Chorus1 */
	{   1,    5,    9,   19,    0}, /* Chorus2 */
	{   2,    8,    3,   19,    0}, /* Chorus3 (デフォルト) */
	{   3,   16,    9,   16,    0}, /* Chorus4 */
	{   4,   64,    2,   24,    0}, /* FB Chorus */
	{   5,  112,    1,    5,    0}  /* Flanger */
};

static long g_lGSMasterReverbDefault[8][8] = {
	{   0,    0,    3,   64,   80,    0,    0,    0}, /* Room1 */
	{   1,    1,    4,   64,   56,    0,    0,    0}, /* Room2 */
	{   2,    2,    0,   64,   64,    0,    0,    0}, /* Room3 */
	{   3,    3,    4,   64,   72,    0,    0,    0}, /* Hall1 */
	{   4,    4,    0,   64,   64,    0,    0,    0}, /* Hall2 (デフォルト) */
	{   5,    5,    0,   64,   88,    0,    0,    0}, /* Plate */
	{   6,    6,    0,   64,   32,   40,    0,    0}, /* Delay */
	{   7,    7,    0,   64,   64,   32,    0,    0}  /* PanDelay */
};

static long g_lGSMasterChorusDefault[8][9] = {
	{   0,    0,   64,    0,  112,    3,    5,    0,    0}, /* Chorus1 */
	{   1,    0,   64,    5,   80,    9,   19,    0,    0}, /* Chorus2 */
	{   2,    0,   64,    8,   80,    3,   19,    0,    0}, /* Chorus3 (デフォルト) */
	{   3,    0,   64,   16,   64,    9,   16,    0,    0}, /* Chorus4 */
	{   4,    0,   64,   64,  127,    2,   24,    0,    0}, /* Feedback Chorus */
	{   5,    0,   64,  112,  127,    1,    5,    0,    0},	/* Flanger */
	{   6,    0,   64,    0,  127,    0,  127,    0,    0}, /* Short Delay */
	{   7,    0,   64,   80,  127,    0,  127,    0,    0}  /* Short Delay FB */
};

static long g_lGSMasterDelayDefault[10][11] = {
	{   0,    0,   97,    1,    1,  127,    0,    0,   64,   80,    0}, /* Delay1 (デフォルト) */
	{   1,    0,  106,    1,    1,  127,    0,    0,   64,	 80,	0}, /* Delay2 */
	{   2,    0,  115,    1,    1,  127,    0,    0,   64,   72,    0}, /* Delay3 */
	{   3,    0,   83,    1,    1,  127,    0,    0,   64,   72,    0}, /* Delay4 */
	{   4,    0,  105,   12,   24,    0,  125,   60,   64,   74,    0}, /* Pan Delay 1 */
	{   5,    0,  109,   12,   24,    0,  125,   60,   64,   71,    0}, /* Pan Delay 2 */
	{   6,    0,  115,   12,   24,    0,  120,   64,   64,   73,    0}, /* Pan Delay 3 */
    {   7,    0,   93,   12,   24,    0,  120,   64,   64,   72,    0}, /* Pan Delay 4 */
    {   8,    0,  109,   12,   24,    0,  114,   60,   64,   61,   36}, /* Delay to Reverb */
	{   9,    0,  110,   21,   32,   97,  127,   67,   64,   40,    0}  /* Pan Repert */
};

static long g_lGSMasterEqualizerDefault[1][5] = {
	{   0,    0,   64,    0,   64}, /* Flat (Default) */
};


static long g_lXGMasterReverbDefault[19][17] = {
	/*Type,   P1,   P2,   P3,   P4,   P5,   P6,   P7,   P8,   P9,  P10,  P11,  P12,  P13,  P14,  P15,  P16 */ 
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}, /* NO EFFECT */
	{  128,   18,   10,    8,   13,   49,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* HALL1 (Default) */
	{  129,   25,   10,   28,    6,   46,    0,    0,    0,    0,   40,   13,    3,   74,    7,   64,    0}, /* HALL2 */
	{  134,   18,   10,    8,   13,   49,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* HALL M */
	{  135,   18,   10,   28,    6,   46,    0,    0,    0,    0,   40,   13,    3,   74,    7,   64,    0}, /* HALL L */
	{  256,    5,   10,   16,    4,   49,    0,    0,    0,    0,   40,    5,    3,   64,    8,   64,    0}, /* ROOM1 */
	{  257,   12,   10,    5,    4,   38,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* ROOM2 */
	{  258,    9,   10,   47,    5,   36,    0,    0,    0,    0,   40,    0,    4,   60,    8,   64,    0}, /* ROOM3 */
	{  261,   11,   10,    5,    4,   38,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* ROOM S */
	{  262,   13,   10,   16,    4,   49,    0,    0,    0,    0,   40,    5,    3,   64,    8,   64,    0}, /* ROOM M */
	{  264,   15,   10,   47,    5,   36,    0,    0,    0,    0,   40,    0,    4,   60,    8,   64,    0}, /* ROOM L */
	{  384,   19,   10,   16,    7,   54,    0,    0,    0,    0,   40,    0,    3,   64,    6,   64,    0}, /* STAGE1 */
	{  385,   11,   10,   16,    7,   51,    0,    0,    0,    0,   40,    2,    2,   64,    6,   64,    0}, /* STAGE2 */
	{  512,   25,   10,    6,    8,   49,    0,    0,    0,    0,   40,    2,    3,   64,    5,   64,    0}, /* PLATE */
	{  519,   13,   10,    6,    8,   49,    0,    0,    0,    0,   40,    2,    3,   64,    5,   64,    0}, /* GMPLATE */
	{ 2048,    9,    5,   11,    0,   46,   30,   50,   70,    7,   40,   34,    4,   64,    7,   64,    0}, /* WHITE ROOM */
	{ 2176,   48,    6,   19,    0,   44,   33,   52,   70,   16,   40,   20,    4,   64,    7,   64,    0}, /* TUNNEL */
	{ 2304,   59,    6,   63,    0,   45,   34,   62,   91,   13,   40,   25,    4,   64,    4,   64,    0}, /* CANYON */
	{ 2432,    3,    6,    3,    0,   34,   26,   29,   59,   15,   40,   32,    4,   64,    8,   64,    0}, /* BASEMENT */
};

static long g_lXGMasterChorusDefault[21][17] = {
	/* Type,   P1,   P2,   P3,   P4,   P5,   P6,   P7,   P8,   P9,  P10,  P11,  P12,  P13,  P14,  P15,  P16 */ 
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}, /* NO EFFECT */
	{ 8320,    6,   54,   77,  106,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* CHORUS 1 (Default) */
	{ 8321,    8,   63,   64,   30,    0,   28,   62,   42,   58,   64,   46,   64,   10,    0,    0,    0}, /* CHORUS 2 */ 
	{ 8322,    4,   44,   64,  110,    0,   28,   64,   46,   66,   64,   46,   64,   10,    0,    0,    0}, /* CHORUS 3 */
	{ 8323,    9,   10,   64,  109,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 1 */ 
	{ 8324,   26,   34,   67,  105,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 2 */
	{ 8325,    9,   34,   69,  105,    0,   28,   64,   46,   66,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 3 */ 
	{ 8326,   26,   29,   75,  102,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 4 */
	{ 8327,    6,   43,  107,  111,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* FB CHORUS */
	{ 8328,    9,   32,   69,  104,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    1,    0}, /* CHORUS 4 */
	{ 8448,   12,   32,   64,    0,    0,   28,   64,   46,   64,  127,   40,   68,   10,    0,    0,    0}, /* CELESTE 1 */
	{ 8449,   28,   18,   90,    2,    0,   28,   62,   42,   60,   84,   40,   68,   10,    0,    0,    0}, /* CELESTE 2 */
	{ 8450,    4,   63,   44,    2,    0,   28,   64,   46,   68,  127,   40,   68,   10,    0,    0,    0}, /* CELESTE 3 */
	{ 8456,    8,   29,   64,    0,    0,   28,   64,   51,   66,  127,   40,   68,   10,    0,    1,    0}, /* CELESTE 4 */
	{ 8576,   14,   14,  104,    2,    0,   28,   64,   46,   64,   96,   40,   64,   10,    4,    0,    0}, /* FLANGER 1 */
	{ 8577,   32,   17,   26,    2,    0,   28,   64,   46,   60,   96,   40,   64,   10,    4,    0,    0}, /* FLANGER 2 */
	{ 8583,    3,   21,  120,    1,    0,   28,   64,   46,   64,   96,   40,   64,   10,    4,    0,    0}, /* GM FLANGER */
	{ 8584,    4,  109,  109,    2,    0,   28,   64,   46,   64,  127,   40,   64,   10,    4,    0,    0}, /* FLANGER 3 */ 
	{ 8704,   12,   25,   16,    0,    0,   28,   64,   46,   64,  127,   46,   64,   10,    0,    0,    0}, /* SYMPHONIC */
	{ 9216,    8,  111,   74,  104,    0,   28,   64,   46,   64,   64,    6,    1,    0,    0,    0,    0}, /* PHASER 1 */
	{11136,   54,    0,    0,    0,    0,    0,    0,    0,    0,   64,   28,   64,   46,   64,    0,    0}, /* ENSEMBLE DETUNE */
};

static long g_lXGMasterVariationDefault[116][17] = {
	/*Type,   P1,   P2,   P3,   P4,   P5,   P6,   P7,   P8,   P9,  P10,  P11,  P12,  P13,  P14,  P15,  P16 */ 
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}, /* NO EFFECT */
	{  128,   18,   10,    8,   13,   49,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* HALL1 */
	{  129,   25,   10,   28,    6,   46,    0,    0,    0,    0,   40,   13,    3,   74,    7,   64,    0}, /* HALL2 */
	{  134,   18,   10,    8,   13,   49,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* HALL M */
	{  135,   18,   10,   28,    6,   46,    0,    0,    0,    0,   40,   13,    3,   74,    7,   64,    0}, /* HALL L */
	{  256,    5,   10,   16,    4,   49,    0,    0,    0,    0,   40,    5,    3,   64,    8,   64,    0}, /* ROOM1 */
	{  257,   12,   10,    5,    4,   38,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* ROOM2 */
	{  258,    9,   10,   47,    5,   36,    0,    0,    0,    0,   40,    0,    4,   60,    8,   64,    0}, /* ROOM3 */
	{  261,   11,   10,    5,    4,   38,    0,    0,    0,    0,   40,    0,    4,   50,    8,   64,    0}, /* ROOM S */
	{  262,   13,   10,   16,    4,   49,    0,    0,    0,    0,   40,    5,    3,   64,    8,   64,    0}, /* ROOM M */
	{  263,   15,   10,   47,    5,   36,    0,    0,    0,    0,   40,    0,    4,   60,    8,   64,    0}, /* ROOM L */
	{  384,   19,   10,   16,    7,   54,    0,    0,    0,    0,   40,    0,    3,   64,    6,   64,    0}, /* STAGE1 */
	{  385,   11,   10,   16,    7,   51,    0,    0,    0,    0,   40,    2,    2,   64,    6,   64,    0}, /* STAGE2 */
	{  512,   25,   10,    6,    8,   49,    0,    0,    0,    0,   40,    2,    3,   64,    5,   64,    0}, /* PLATE */
	{  519,   13,   10,    6,    8,   49,    0,    0,    0,    0,   40,    2,    3,   64,    5,   64,    0}, /* GMPLATE */
	{  640, 3333, 1667, 5000, 5000,   74,  100,   10,    0,    0,   32,    0,   60,   28,   64,   46,   64}, /* DELAY L,C,R (Default) */
	{  768, 2500, 3750, 3752, 3750,   87,   10,    0,    0,    0,   32,    0,   60,   28,   64,   46,   64}, /* DELAY L,R */
	{  896, 1700,   80, 1780,   80,   10, 1700, 1780,    0,    0,   40,    0,   60,   28,   64,   46,   64}, /* ECOH */
	{ 1024, 1700, 1750,  111,    1,   10,    0,    0,    0,    0,   32,    0,   60,   28,   64,   46,   64}, /* CROSS DELAY */
	{ 1152,    0,   19,    5,   16,   64,    0,   46,    0,    0,   32,    5,    0,   10,    0,    0,    0}, /* EARLY REF1 */
	{ 1153,    2,    7,   10,   16,   64,    3,   46,    0,    0,   32,    5,    2,   10,    0,    0,    0}, /* EARLY REF2 */
	{ 1280,    0,   15,    6,    2,   64,    0,   44,    0,    0,   32,    4,    3,   10,    0,    0,    0}, /* GATE REVERB */
	{ 1408,    1,   19,    8,    3,   64,    0,   47,    0,    0,   32,    6,    3,   10,    0,    0,    0}, /* REVERSE GATE */
	{ 2048,    9,    5,   11,    0,   46,   30,   50,   70,    7,   40,   34,    4,   64,    7,   64,    0}, /* WHITE ROOM */
	{ 2176,   48,    6,   19,    0,   44,   33,   52,   70,   16,   40,   20,    4,   64,    7,   64,    0}, /* TUNNEL */
	{ 2304,   59,    6,   63,    0,   45,   34,   62,   91,   13,   40,   25,    4,   64,    4,   64,    0}, /* CANYON */
	{ 2432,    3,    6,    3,    0,   34,   26,   29,   59,   15,   40,   32,    4,   64,    8,   64,    0}, /* BASEMENT */
	{ 2560,   63,   97,    0,   48,    0,    0,    0,    0,    0,   64,    0,    0,    0,    0,    0,    0}, /* KARAOKE1 */
	{ 2561,   55,  105,    0,   50,    0,    0,    0,    0,    0,   64,    0,    0,    0,    0,    0,    0}, /* KARAOKE2 */
	{ 2562,   43,  110,   14,   53,    0,    0,    0,    0,    0,   64,    0,    0,    0,    0,    0,    0}, /* KARAOKE3 */
	{ 2688,   10,   80,   10,   78,   64,    0,    0,    0,    0,   39,    0,    0,   28,   64,   46,   64}, /* TEMPO DELAY */
	{ 2696,   11,   92,   10,   78,   64,    0,    0,    0,    0,   40,    0,    0,   28,   64,   46,   64}, /* TEMPO ECHO */
	{ 2816,    8,    8,  102,    1,   10,   64,    0,    0,    0,   34,    0,    0,   28,   64,   46,   64}, /* TEMPO CROSS */
	{ 8320,    6,   54,   77,  106,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* CHORUS 1 */
	{ 8321,    8,   63,   64,   30,    0,   28,   62,   42,   58,   64,   46,   64,   10,    0,    0,    0}, /* CHORUS 2 */ 
	{ 8322,    4,   44,   64,  110,    0,   28,   64,   46,   66,   64,   46,   64,   10,    0,    0,    0}, /* CHORUS 3 */
	{ 8323,    9,   10,   64,  109,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 1 */ 
	{ 8324,   26,   34,   67,  105,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 2 */
	{ 8325,    9,   34,   69,  105,    0,   28,   64,   46,   66,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 3 */ 
	{ 8326,   26,   29,   75,  102,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* GM CHORUS 4 */
	{ 8327,    6,   43,  107,  111,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    0,    0}, /* FB CHORUS */
	{ 8328,    9,   32,   69,  104,    0,   28,   64,   46,   64,   64,   46,   64,   10,    0,    1,    0}, /* CHORUS 4 */
	{ 8448,   12,   32,   64,    0,    0,   28,   64,   46,   64,  127,   40,   68,   10,    0,    0,    0}, /* CELESTE 1 */
	{ 8449,   28,   18,   90,    2,    0,   28,   62,   42,   60,   84,   40,   68,   10,    0,    0,    0}, /* CELESTE 2 */
	{ 8450,    4,   63,   44,    2,    0,   28,   64,   46,   68,  127,   40,   68,   10,    0,    0,    0}, /* CELESTE 3 */
	{ 8456,    8,   29,   64,    0,    0,   28,   64,   51,   66,  127,   40,   68,   10,    0,    1,    0}, /* CELESTE 4 */
	{ 8576,   14,   14,  104,    2,    0,   28,   64,   46,   64,   96,   40,   64,   10,    4,    0,    0}, /* FLANGER 1 */
	{ 8577,   32,   17,   26,    2,    0,   28,   64,   46,   60,   96,   40,   64,   10,    4,    0,    0}, /* FLANGER 2 */
	{ 8583,    3,   21,  120,    1,    0,   28,   64,   46,   64,   96,   40,   64,   10,    4,    0,    0}, /* GM FLANGER */
	{ 8584,    4,  109,  109,    2,    0,   28,   64,   46,   64,  127,   40,   64,   10,    4,    0,    0}, /* FLANGER 3 */ 
	{ 8704,   12,   25,   16,    0,    0,   28,   64,   46,   64,  127,   46,   64,   10,    0,    0,    0}, /* SYMPHONIC */
	{ 8832,   81,   35,    0,    0,    0,   24,   60,   45,   54,  127,   33,   52,   30,    0,    0,    0}, /* ROTARY SPEAKER */ 
	{ 8833,    6,   92,    0,    0,    0,   26,   68,   56,   52,  127,    0,    0,    0,    5,   49,   55}, /* DISTORTION+ROTARY SPEAKER */
	{ 8834,    7,   90,    0,    0,    0,   24,   66,   56,   52,  127,    0,    0,    0,    4,   47,   45}, /* OVER, DRIVE+ROTARY SPEAKER */
	{ 8835,    5,   90,    3,    0,    0,   25,   68,   56,   52,  127,    0,    0,    0,    4,   48,   45}, /* AMP SIMULATOR+ROTARY SPEAKER */
	{ 8960,   83,   56,    0,    0,    0,   28,   64,   46,   64,  127,   40,   64,   10,   64,    0,    0}, /* TREMOLO */
	{ 9088,   76,   80,   32,    5,    0,   28,   64,   46,   64,  127,   40,   64,   10,    0,    0,    0}, /* AUTOPAN */ 
	{ 9216,    8,  111,   74,  104,    0,   28,   64,   46,   64,   64,    6,    1,    0,    0,    0,    0}, /* PHASER 1 */
	{ 9224,    8,  111,   74,  108,    0,   28,   64,   46,   64,   64,    5,    0,    4,    0,    0,    0}, /* PHASER 2 */
	{ 9344,   40,   20,   72,   53,   48,    0,   43,   74,   10,  127,  120,    0,    0,    0,    0,    0}, /* DISTORTION */
	{ 9345,   40,   20,   72,   53,   48,    0,   43,   74,   10,  127,  120,    6,    2,  100,    4,    0}, /* COMPRESSOR+DISTORTION */
	{ 9352,   18,   27,   71,   48,   84,    0,   32,   66,   10,  127,  105,    0,    0,    0,    0,    0}, /* STEREO DISTORTION */
	{ 9472,   29,   24,   68,   45,   55,    0,   41,   72,   10,  127,  104,    0,    0,    0,    0,    0}, /* OVER DRIVE */
	{ 9480,   10,   24,   69,   46,  105,    0,   41,   66,   10,  127,  104,    0,    0,    0,    0,    0}, /* STEREO OVER DRIVE */
	{ 9600,   39,    1,   48,   55,    0,    0,    0,    0,    0,  127,  112,    0,    0,    0,    0,    0}, /* AMP SIMULATOR */
	{ 9608,   16,    2,   46,  119,    0,    0,    0,    0,    0,  127,  106,    0,    0,    0,    0,    0}, /* STEREO AMP SIMULATOR */
	{ 9728,   70,   34,   60,   10,   70,   28,   46,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* 3-BAND EQ */
	{ 9856,   28,   70,   46,   70,    0,    0,    0,    0,    0,  127,   34,   64,   10,    0,    0,    0}, /* 2-BAND EQ */
	{ 9984,   70,   56,   39,   25,    0,   28,   66,   46,   64,  127,    0,    0,    0,    0,    0,    0}, /* AUTO WAH */
	{ 9985,   40,   73,   26,   29,    0,   28,   66,   46,   64,  127,   30,   72,   74,   53,   48,    0}, /* AUTO WAH+DISTORTION */
	{ 9986,   48,   64,   32,   23,    0,   28,   66,   46,   64,  127,   29,   68,   72,   45,   55,    0}, /* AUTO WAH+OVER DRIVE */
	{10240,   64,    0,   74,   54,   64,    0,    0,    0,    0,   64,    1,  127,  127,  127,    0,    0}, /* PITCH CHANGE */
	{10241,   65,   50,   67,   61,   87,    0,    0,    0,    0,   32,    1,  127,  127,  127,    0,    0}, /* PITCH CHANGE 2 */
	{10368,   44,   30,   48,    0,    0,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* HARMONIC ENHANCER */
	{10496,   36,    0,   30,    0,    0,   28,   66,   46,   64,  127,    0,    0,    0,    0,    0,    0}, /* TOUCH WAH */
	{10497,   36,    0,   30,    0,    0,   28,   66,   46,   64,  127,   30,    0,    0,    0,    0,    0}, /* TOUCH WAH+DISTORTION */
	{10498,   45,   18,   28,    0,    0,   28,   66,   46,   64,  127,   29,   68,   72,   45,   55,   64}, /* TOUCH WAH+OVER DRIVE */
	{10504,   68,   18,   60,    0,    0,   28,   66,   46,   64,  127,    0,   72,   74,   53,   57,   64}, /* TOUCH WAH 2 */
	{10624,    6,    2,  100,    4,   96,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* COMPRESSOR */
	{10752,    0,   11,   82,   50,    0,    0,    0,    0,    0,  127,    3,    0,    0,    0,    0,    0}, /* NOISE GATE */
	{10880,    0,    0,    0,    0,    0,    0,    0,    0,    0,   64,    8,   25,    0,    0,    0,    0}, /* VOICE CANCEL */
	{11008,   16,   26,   35,   70,    0,   24,   60,   45,   54,  127,   31,   45,    0,    0,    0,    0}, /* 2WAY ROTARY SPEAKER */
	{11009,    6,   28,   30,   64,    0,   24,   66,   56,   59,  127,   36,   60,    0,    3,   48,   60}, /* DISTORTION+2WAY ROTARY SP */
	{11010,    5,   28,   30,   62,    0,   20,   67,   56,   60,  127,   33,   60,    0,    4,   46,   50}, /* OVER DRIVE+2WAY ROTARY SPEAKER */
	{11011,    8,   27,   29,   64,    0,   17,   66,   58,   52,  127,   33,   60,    3,    3,   48,   52}, /* AMP SIMULATOR+2WAY ROTARY SPEAKER */
	{11136,   54,    0,    0,    0,    0,    0,    0,    0,    0,   64,   28,   64,   46,   64,    0,    0}, /* ENSEMBLE DETUNE */
	{11264,  114,    0,    0,    0,    0,   28,   64,   46,   64,   64,    0,    0,    0,    0,    0,    0}, /* AMBIENCE */
	{11392,    0,    0,    0,   64,    0,    1,    1,   84,   44,   64,   39,   47,    0,    0,    0,    0}, /* VOCODER HARMONY */
	{11520,    0,    0,    0,   64,    0,    1,    1,   84,   44,   64,   39,   47,    0,    0,    0,    0}, /* CHORDAL HARMONY */
	{11648,    0,    0,    0,   64,    0,    0,    0,    0,    0,   64,   39,   47,    0,    0,    0,    0}, /* DETUNE HARMONY */
	{11776,    0,    0,    0,   64,    0,    1,    1,   84,   44,   64,   39,   47,    0,    0,    0,    0}, /* CHROMATIC HARMONY */
	{11904,    0,   15,   31,   63,    0,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* TALKING MODULATOR */
	{12032,    2,   60,    6,   54,    5,   10,    1,    1,    0,  127,    0,    0,    0,    0,    1,    0}, /* LO-FI */
	{12160, 2500, 3000, 3750,   74,   70,   40,   48,   72,   74,  127,    0,    0,    0,    0,    0,    0}, /* DISTORTION+DELAY */
	{12161, 1900, 1400, 2500,   78,   60,   29,   55,   68,   72,  127,    0,    0,    0,    0,    0,    0}, /* OVER DRIVE+DELAY */
	{12288, 3000,   72,   66,   40,   48,   72,   74,    0,    0,  127,    6,    2,  100,    4,    0,    0}, /* COMPRESSOR+DISTORTION+DELAY */
	{12289, 3000,   72,   66,   29,   55,   68,   72,    0,    0,  127,    6,    2,  100,    4,    0,    0}, /* COMPRESSOR+OVER DRIVE+DELAY */
	{12416, 1600,   84,   64,   30,   48,   69,   72,    0,    0,  127,   40,    0,   30,   64,    0,    0}, /* WAH+DISTORTION+DELAY */
	{12417, 1600,   84,   64,   24,   55,   65,   70,    0,    0,  127,   40,    0,   30,   64,    0,    0}, /* WAH+OVER DRIVE+DELAY */
	{12544,   22,    3,    2,    6,   88,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* V DISTORTION HARD */
	{12545,   22,    3,    2,    5,   82, 2500, 5000, 5000,   85,  127,   46,    0,    0,    0,    0,    0}, /* V DISTORTION HARD+DELAY */
	{12546,   13,    3,    2,    6,   98,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* V DISTORTION SOFT */
	{12547,   14,    3,    2,    6,   92, 2500, 5000, 5000,   76,  127,   44,    0,    0,    0,    0,    0}, /* V DISTORTION SOFT+DELAY */
	{12672,   15,   18,   89,   91,   54,   22,   20,   22,   52,  127,   14,   72,   34,   61,   60,    0}, /* DUAL ROTOR SPEAKER 1 */
	{12673,   14,   18,   91,   95,   54,   22,   22,   29,   64,  127,   34,   64,   34,   64,   60,    0}, /* DUAL ROTOR SPEAKER 2 */
	{12800,   11,   86,   62,    8,   88,   70,   68,   64,   64,  127,    0,    0,    0,    0,    0,    0}, /* DISTORTION+TEMPO DELAY */
	{12801,   11,   78,   64,   10,  110,   68,   70,   70,   64,  127,    0,    0,    0,    0,    0,    0}, /* OVER DRIVE+TEMPO DELAY */
	{12928,   11,   76,   62,   10,   90,   72,   74,   70,   64,  127,    4,    4,  101,    3,    0,    0}, /* COMPRESSOR+DISTORTION+TEMPO DELAY */
	{12929,   11,   74,   62,    9,   94,   74,   74,   70,   64,  127,    8,    9,  103,    3,    0,    0}, /* COMPRESSOR+OVER DRIVE+TEMPO DELAY */
	{13057,   11,   86,   52,   10,  120,   68,   64,   70,   64,  127,   50,    0,   30,   64,    0,    0}, /* WAH+DISTORTION+TEMPO DELAY */
	{13058,   11,   84,   58,   16,  127,   68,   65,   70,   64,  127,   64,    0,   30,   64,    0,    0}, /* WAH+OVER DRIVE+TEMPO DELAY */
	{13184,   22,    3,    2,    5,   82,   11,   90,   72,   64,  127,   64,    0,    0,    0,    0,    0}, /* V DISTORTION HARD+TEMPO DELAY */
	{13185,   14,    3,    2,    6,   90,   11,   92,   77,   64,  127,   66,    0,    0,    0,    0,    0}, /* V DISTORTION SOFT+TEMPO DELAY */
	{16257,   64,   64,   40,    0,    0,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* 3D MANUAL */
	{16258,   64,   64,   40,    2,   10,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}, /* 3D AUTO */
	{16259,   89,    0,    0,    0,    0,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0}  /* WIDE STEREO */
};

static long g_lXGMasterEqualizerDefault[5][21] = {
	{0,   64,   12,    7,    0,   64,   28,    7,    0,   64,   34,    7,    0,   64,   46,    7,    0,   64,   52,    7,    0}, /* Flat */
	{1,   58,    8,    7,    0,   66,   16,    3,    0,   68,   33,    3,    0,   60,   44,    5,    0,   58,   50,    7,    0}, /* Jazz */
	{2,   68,   16,    7,    0,   60,   24,   20,    0,   67,   34,    7,    0,   60,   40,   20,    0,   70,   48,    7,    0}, /* Pops */
	{3,   71,   16,    7,    0,   68,   20,    7,    0,   60,   36,    5,    0,   68,   41,   10,    0,   66,   50,    7,    0}, /* Rock */
	{4,   67,   12,    7,    0,   68,   24,    7,    0,   64,   34,    5,    0,   66,   50,    7,    0,   61,   52,    7,    0}  /* Concert */
};

/* 汎用関数群(外部隠蔽) */

/* バイト配列の合計を計算する */
long Sum (unsigned char* pData, long lLen) {
	long lRet = 0;
	long i;
	assert (pData);
	for (i = 0; i < lLen; i++) {
		lRet += pData[i];
	}
	return lRet;
}

/******************************************************************************/
/*                                                                            */
/* MIDIPart関数群                                                             */
/*                                                                            */
/******************************************************************************/

/* MIDIPartオブジェクトを削除する。 */
int __stdcall MIDIPart_Delete (MIDIPart* pMIDIPart) {
	free (pMIDIPart);
	return 1;
}

/* MIDIPartオブジェクトを生成する。 */
MIDIPart* __stdcall MIDIPart_Create (MIDIStatus* pParent) {
	MIDIPart* pMIDIPart;
	pMIDIPart = malloc (sizeof (MIDIPart));
	if (pMIDIPart == NULL) {
		return NULL;
	}
	memset (pMIDIPart, 0, sizeof (MIDIPart));
	pMIDIPart->m_pParent = pParent;
	return pMIDIPart;
}


/* MIDIPart_Get系関数 */

/* 現在のパートモードを取得 */
long __stdcall MIDIPart_GetPartMode (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lPartMode; 
}

/* 現在のオムニON/OFF・モノ/ポリモードを取得 */
long __stdcall MIDIPart_GetOmniMonoPolyMode (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lOmniMonoPolyMode; 
}

/* 現在のチャンネルファインチューニングの値を取得 */
long __stdcall MIDIPart_GetChannelFineTuning (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lChannelFineTuning;
}

/* 現在のチャンネルコースチューニングの値を取得 */
long __stdcall MIDIPart_GetChannelCoarseTuning (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lChannelCoarseTuning;
}

/* 現在のピッチベンドセンシティビティの値を取得 */
long __stdcall MIDIPart_GetPitchBendSensitivity (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lPitchBendSensitivity;
}

/* 現在のモジュレーションデプスレンジの値を取得 */
long __stdcall MIDIPart_GetModulationDepthRange (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lModulationDepthRange;
}

/* 現在のレシーブチャンネルの値を取得 */
long __stdcall MIDIPart_GetReceiveChannel (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lReceiveChannel;
}

/* 現在のベロシティセンスデプスの値を取得 */
long __stdcall MIDIPart_GetVelocitySenseDepth (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lVelocitySenseDepth;
}

/* 現在のベロシティセンスオフセットの値を取得 */
long __stdcall MIDIPart_GetVelocitySenseOffset (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lVelocitySenseOffset;
}

/* 現在のキーボードレンジローの値を取得 */
long __stdcall MIDIPart_GetKeyboardRangeLow (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lKeyboardRangeLow;
}

/* 現在のキーボードレンジハイの値を取得 */
long __stdcall MIDIPart_GetKeyboardRangeHigh (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lKeyboardRangeHigh;
}

/* 現在の鍵盤の押され具合を取得する。 */
/* 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ) */
long __stdcall MIDIPart_GetNote (MIDIPart* pMIDIPart, long lKey) {
	assert (pMIDIPart);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIPart->m_cNote[lKey];
}

/* 現在の鍵盤の押され具合をまとめて取得する。 */
long __stdcall MIDIPart_GetNoteEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen) {
	assert (pMIDIPart);
	assert (pBuf);
	memcpy (pBuf, pMIDIPart->m_cNote, MIN (lLen, 128));
	return 1;
}

/* 現在の鍵盤の押され具合(ホールド持続含む)を取得する。 */
/* 各音階毎、0=押されていない、1～127=押されている(押したときのベロシティ) */
long __stdcall MIDIPart_GetNoteKeep (MIDIPart* pMIDIPart, long lKey) {
	assert (pMIDIPart);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIPart->m_cNoteKeep[lKey];
}

/* 現在の鍵盤の押され具合(ホールド持続含む)をまとめて取得する。 */
long __stdcall MIDIPart_GetNoteKeepEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen) {
	assert (pMIDIPart);
	assert (pBuf);
	memcpy (pBuf, pMIDIPart->m_cNoteKeep, MIN (lLen, 128));
	return 1;
}

/* 現在のキーアフタータッチの値を取得 */
long __stdcall MIDIPart_GetKeyAfterTouch (MIDIPart* pMIDIPart, long lKey) {
	assert (pMIDIPart);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIPart->m_cKeyAfterTouch[lKey];
}

/* 現在のキーアフタータッチの値をまとめて取得 */
long __stdcall MIDIPart_GetKeyAfterTouchEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen) {
	assert (pMIDIPart);
	assert (pBuf);
	memcpy (pBuf, pMIDIPart->m_cKeyAfterTouch, MIN (lLen, 128));
	return 1;
}

/* 現在のコントロールチェンジの値を取得 */
long __stdcall MIDIPart_GetControlChange (MIDIPart* pMIDIPart, long lNum) {
	assert (pMIDIPart);
	assert (0 <= lNum && lNum <= 127);
	return pMIDIPart->m_cControlChange[lNum];
}

/* 現在のコントロールチェンジの値をまとめて取得 */
long __stdcall MIDIPart_GetControlChangeEx (MIDIPart* pMIDIPart, unsigned char* pBuf, long lLen) {
	assert (pMIDIPart);
	assert (pBuf);
	memcpy (pBuf, pMIDIPart->m_cControlChange, MIN (lLen, 128));
	return 1;
}

/* 現在のRPNMSBの値を取得 */
long __stdcall MIDIPart_GetRPNMSB (MIDIPart* pMIDIPart, long lCC101, long lCC100) {
	unsigned char cCC101, cCC100;
	assert (pMIDIPart);
	assert (0 <= lCC101 && lCC100 <= 127);
	assert (0 <= lCC101 && lCC100 <= 127);
	cCC101 = (unsigned char)lCC101;
	cCC100 = (unsigned char)lCC100;
#ifdef MIDISTATUS_RECORDRPN
	return pMIDIPart->m_cRPNMSB[cCC101][cCC100];
#else
	/* ピッチベンドレンジ(RPN#0) */
	if (cCC101 == 0 && cCC100 == 0) {
		return pMIDIPart->m_lPitchBendSensitivity;
	}
	/* チャンネルファインチューンMSB(RPN#1) */
	else if (cCC101 == 0 && cCC100 == 1) {
		return (pMIDIPart->m_lChannelFineTuning) >> 7;
	}
	/* チャンネルコースチューン(RPN#2) */
	else if (cCC101 == 0 && cCC100 == 2) {
		return (pMIDIPart->m_lChannelCoarseTuning);
	}
	/* モジュレーションデプスレンジMSB(RPN#5) */
	else if (cCC101 == 0 && cCC100 == 5) {
		return (pMIDIPart->m_lModulationDepthRange) >> 7;
	}
	return 0;
#endif
}

/* 現在のRPNLSBの値を取得 */
long __stdcall MIDIPart_GetRPNLSB (MIDIPart* pMIDIPart, long lCC101, long lCC100) {
	unsigned char cCC101, cCC100;
	assert (pMIDIPart);
	assert (0 <= lCC101 && lCC100 <= 127);
	assert (0 <= lCC101 && lCC100 <= 127);
	cCC101 = (unsigned char)lCC101;
	cCC100 = (unsigned char)lCC100;
#ifdef MIDISTATUS_RECORDRPN
	return pMIDIPart->m_cRPNLSB[cCC101][cCC100];
#else
	/* チャンネルファインチューンMSB(RPN#1) */
	if (cCC101 == 0 && cCC100 == 1) {
		return (pMIDIPart->m_lChannelFineTuning) & 0x007F;
	}
	/* モジュレーションデプスレンジMSB(RPN#5) */
	else if (cCC101 == 0 && cCC100 == 5) {
		return (pMIDIPart->m_lModulationDepthRange) & 0x007F;
	}
	return 0;
#endif
}


/* 現在のNRPNMSBの値を取得 */
long __stdcall MIDIPart_GetNRPNMSB (MIDIPart* pMIDIPart, long lCC99, long lCC98) {
	unsigned char cCC99, cCC98;
	assert (pMIDIPart);
	assert (0 <= lCC99 && lCC99 <= 127);
	assert (0 <= lCC98 && lCC98 <= 127);
	cCC99 = (unsigned char)lCC99;
	cCC98 = (unsigned char)lCC98;
#ifdef MIDISTATUS_RECORDNRPN
	return pMIDIPart->m_cNRPNMSB[cCC99][cCC98];
#else
	/* ビブラートレイト(NRPN#(1*128+8)) */
	if (cCC99 == 1 && cCC98 == 8) {
		return pMIDIPart->m_cControlChange[76];
	}
	/* ビブラートデプス(NRPN#(1*128+9)) */
	else if (cCC99 == 1 && cCC98 == 9) {
		return pMIDIPart->m_cControlChange[77];
	}
	/* ビブラートディレイ(NRPN#(1*128+10)) */
	else if (cCC99 == 1 && cCC98 == 10) {
		return pMIDIPart->m_cControlChange[78];
	}
	/* カットオフフリーケンシー(NRPN#(1*128+32)) */
	else if (cCC99 == 1 && cCC98 == 32) {
		return pMIDIPart->m_cControlChange[74];
	}
	/* レゾナンス(NRPN#(1*128+33)) */
	else if (cCC99 == 1 && cCC98 == 33) {
		return pMIDIPart->m_cControlChange[71];
	}
	/* アタックタイム(NRPN#(1*128+99)) */
	else if (cCC99 == 1 && cCC98 == 99) {
		return pMIDIPart->m_cControlChange[73];
	}
	/* ディケイタイム(NRPN#(1*128+100)) */
	else if (cCC99 == 1 && cCC98 == 100) {
		return pMIDIPart->m_cControlChange[75];
	}
	/* リリースタイム(NRPN#(1*128+102)) */
	else if (cCC99 == 1 && cCC98 == 102) {
		return pMIDIPart->m_cControlChange[72];
	}
	/* 各ノート毎のドラムカットオフフリケンシー(NRPN#(20*128台)) */
	else if (cCC99 ==20) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumCutoffFrequency[cCC98];
		}
	}
	/* 各ノート毎のドラムレゾナンス(NRPN#(21*128台)) */
	else if (cCC99 ==21) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumResonance[cCC98];
		}
	}
	/* 各ノート毎のドラムアタックタイム(NRPN#(22*128台)) */
	else if (cCC99 ==22) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumAttackTime[cCC98];
		}
	}
	/* 各ノート毎のドラムディケイタイム1(NRPN#(23*128台)) */
	else if (cCC99 ==23) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumDecay1Time[cCC98];
		}
	}
	/* 各ノート毎のドラムピッチコース(NRPN#(24*128台)) */
	else if (cCC99 ==24) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumPitchCoarse[cCC98];
		}
	}
	/* 各ノート毎のドラムピッチファイン(NRPN#(25*128台)) */
	else if (cCC99 ==25) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumPitchFine[cCC98];
		}
	}
	/* 各ノート毎のドラムレベル(NRPN#(26*128台)) */
	else if (cCC99 == 26) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumVolume[cCC98];
		}
	}
	/* 各ノート毎のドラムパン(NRPN#(28*128台)) */
	else if (cCC99 == 28) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup =MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumPan[cCC98];
		}
	}
	/* 各ノート毎のドラムリバーブ(NRPN#(29*128台)) */
	else if (cCC99 == 29) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumReverb[cCC98];
		}
	}
	/* 各ノート毎のドラムコーラス(NRPN#(30*128台)) */
	else if (cCC99 == 30) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumChorus[cCC98];
		}
	}
	/* 各ノート毎のドラムディレイ(NRPN#(31*128台)) */
	else if (cCC99 == 31) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			return pMIDIDrumSetup->m_cDrumDelay[cCC98];
		}
	}
	return 0;
#endif
}

/* 現在のNRPNLSBの値を取得 */
long __stdcall MIDIPart_GetNRPNLSB (MIDIPart* pMIDIPart, long lCC99, long lCC98) {
	unsigned char cCC99, cCC98;
	assert (pMIDIPart);
	assert (0 <= lCC99 && lCC99 <= 127);
	assert (0 <= lCC98 && lCC98 <= 127);
	cCC99 = (unsigned char)lCC99;
	cCC98 = (unsigned char)lCC98;
#ifdef MIDISTATUS_RECORDNRPN
	return pMIDIPart->m_cRPNLSB[cCC99][cCC98];
#else
	return 0;
#endif
}

/* 現在のプログラムチェンジの値を取得 */
long __stdcall MIDIPart_GetProgramChange (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lProgramChange;
}

/* 現在のチャンネルアフタータッチの値を取得 */
long __stdcall MIDIPart_GetChannelAfterTouch (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lChannelAfterTouch;
}

/* 現在のピッチベンドの値を取得 */
long __stdcall MIDIPart_GetPitchBend (MIDIPart* pMIDIPart) {
	assert (pMIDIPart);
	return pMIDIPart->m_lPitchBend;
}

/* MIDIPart_Get系関数(ユニークなもの) */

/* 現在押されている鍵盤の数を返す。 */
long __stdcall MIDIPart_GetNumNote (MIDIPart* pMIDIPart) {
	long i;
	long lSum = 0;
	unsigned char* p = &(pMIDIPart->m_cNote[0]);
	assert (pMIDIPart);
	for (i = 0; i <= 127; i++) {
		if (*p++) {
			lSum++;
		}
	}
	return lSum;
}

/* 現在押されている鍵盤の数(ホールド持続含む)を返す。 */
long __stdcall MIDIPart_GetNumNoteKeep (MIDIPart* pMIDIPart) {
	long i;
	long lSum = 0;
	unsigned char* p = &(pMIDIPart->m_cNoteKeep[0]);
	assert (pMIDIPart);
	for (i = 0; i <= 127; i++) {
		if (*p++) {
			lSum++;
		}
	}
	return lSum;
}

/* 現在押されている鍵盤で最も低い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetHighestNote (MIDIPart* pMIDIPart) {
	long i;
	unsigned char* p = &(pMIDIPart->m_cNote[127]);
	assert (pMIDIPart);
	for (i = 127; i >= 0; i--) {
		if (*p--) {
			return i;
		}
	}
	return -1;
}

/* 現在押されている鍵盤(ホールド持続含む)で最も低い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetHighestNoteKeep (MIDIPart* pMIDIPart) {
	long i;
	unsigned char* p = &(pMIDIPart->m_cNoteKeep[127]);
	assert (pMIDIPart);
	for (i = 127; i >= 0; i--) {
		if (*p--) {
			return i;
		}
	}
	return -1;
}

/* 現在押されている鍵盤で最も高い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetLowestNote (MIDIPart* pMIDIPart) {
	long i;
	unsigned char* p = &(pMIDIPart->m_cNote[0]);
	assert (pMIDIPart);
	for (i = 0; i <= 127; i++) {
		if (*p++) {
			return i;
		}
	}
	return -1;
}

/* 現在押されている鍵盤(ホールド持続含む)で最も高い音階を返す(ない場合-1)。 */
long __stdcall MIDIPart_GetLowestNoteKeep (MIDIPart* pMIDIPart) {
	long i;
	unsigned char* p = &(pMIDIPart->m_cNoteKeep[0]);
	assert (pMIDIPart);
	for (i = 0; i <= 127; i++) {
		if (*p++) {
			return i;
		}
	}
	return -1;
}

/* MIDIPart_Set系関数 */

/* 現在のパートモードを設定 */
long __stdcall MIDIPart_SetPartMode (MIDIPart* pMIDIPart, long lPartMode) {
	assert (pMIDIPart);
	assert (0 <= lPartMode && lPartMode <= 4);
	pMIDIPart->m_lPartMode = lPartMode;
	return 1;
}

/* 現在のオムニON/OFF、モノ/ポリモードを設定 */
long __stdcall MIDIPart_SetOmniMonoPolyMode (MIDIPart* pMIDIPart, long lOmniMonoPolyMode) {
	assert (pMIDIPart);
	assert (1 <= lOmniMonoPolyMode && lOmniMonoPolyMode <= 4);
	pMIDIPart->m_lOmniMonoPolyMode = lOmniMonoPolyMode;
	return 1;
}

/* 現在のチャンネルファインチューニングの値を設定 */
long __stdcall MIDIPart_SetChannelFineTuning (MIDIPart* pMIDIPart, long lChannelFineTuning) {
	assert (pMIDIPart);
	assert (0 <= lChannelFineTuning && lChannelFineTuning <= 16383);
	pMIDIPart->m_lChannelFineTuning = lChannelFineTuning;
	return 1;
}

/* 現在のチャンネルコースチューニングの値を設定 */
long __stdcall MIDIPart_SetChannelCoarseTuning (MIDIPart* pMIDIPart, long lChannelCoarseTuning) {
	assert (pMIDIPart);
	assert (0 <= lChannelCoarseTuning && lChannelCoarseTuning <= 127);
	pMIDIPart->m_lChannelCoarseTuning = lChannelCoarseTuning;
	return 1;
}

/* 現在のピッチベンドセンシティビティの値を設定 */
long __stdcall MIDIPart_SetPitchBendSensitivity (MIDIPart* pMIDIPart, long lPitchBendSensitivity) {
	assert (pMIDIPart);
	assert (0 <= lPitchBendSensitivity && lPitchBendSensitivity <= 127);
	pMIDIPart->m_lPitchBendSensitivity = lPitchBendSensitivity;
	return 1;
}

/* 現在のモジュレーションデプスレンジの値を設定 */
long __stdcall MIDIPart_SetModulationDepthRange (MIDIPart* pMIDIPart, long lModulationDepthRange) {
	assert (pMIDIPart);
	assert (0 <= lModulationDepthRange && lModulationDepthRange <= 16383);
	pMIDIPart->m_lModulationDepthRange = lModulationDepthRange;
	return 1;
}

/* 現在のレシーブチャンネルの値を設定 */
long __stdcall MIDIPart_SetReseiveChannel (MIDIPart* pMIDIPart, long lReceiveChannel) {
	assert (pMIDIPart);
	assert (0 <= lReceiveChannel && lReceiveChannel <= 16);
	pMIDIPart->m_lReceiveChannel = lReceiveChannel;
	return 1;
}

/* 現在のベロシティセンスデプスの値を設定 */
long __stdcall MIDIPart_SetVelocitySenseDepth (MIDIPart* pMIDIPart, long lVelocitySenseDepth) {
	assert (pMIDIPart);
	assert (0 <= lVelocitySenseDepth && lVelocitySenseDepth <= 127);
	pMIDIPart->m_lVelocitySenseDepth = lVelocitySenseDepth;
	return 1;
}

/* 現在のベロシティセンスオフセットの値を設定 */
long __stdcall MIDIPart_SetVelocitySenseOffset (MIDIPart* pMIDIPart, long lVelocitySenseOffset) {
	assert (pMIDIPart);
	assert (0 <= lVelocitySenseOffset && lVelocitySenseOffset <= 127);
	pMIDIPart->m_lVelocitySenseOffset = lVelocitySenseOffset;
	return 1;
}

/* 現在のキーボードレンジローの値を設定 */
long __stdcall MIDIPart_SetKeyboardRangeLow (MIDIPart* pMIDIPart, long lKeyboardRangeLow) {
	assert (pMIDIPart);
	assert (0 <= lKeyboardRangeLow && lKeyboardRangeLow <= 127);
	pMIDIPart->m_lKeyboardRangeLow = lKeyboardRangeLow;
	return 1;
}

/* 現在のキーボードレンジハイの値を設定 */
long __stdcall MIDIPart_SetKeyboardRangeHigh (MIDIPart* pMIDIPart, long lKeyboardRangeHigh) {
	assert (pMIDIPart);
	assert (0 <= lKeyboardRangeHigh && lKeyboardRangeHigh <= 127);
	pMIDIPart->m_lKeyboardRangeHigh = lKeyboardRangeHigh;
	return 1;
}

/* 現在の鍵盤の押され具合を設定 */
long __stdcall MIDIPart_SetNote (MIDIPart* pMIDIPart, long lKey, long lVel) {
	assert (pMIDIPart);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVel && lVel <= 127);
	pMIDIPart->m_cNote[lKey] = (unsigned char)lVel;
	/* ノートオフの場合 */
	if (lVel == 0) {
		/* ペダルが押されていない場合 */
		if (pMIDIPart->m_cControlChange[64] <= 63) {
			pMIDIPart->m_cNoteKeep[lKey] = 0;
		}
		/* ペダルが押されている場合は現在のm_cNoteKeep[lKey]の値を保持 */
	}
	/* ノートオンの場合 */
	else {
		pMIDIPart->m_cNoteKeep[lKey] = (unsigned char)lVel;
	}
	return 1;
}

/* 現在の鍵盤の押され具合を設定(隠し関数、ノートオフイベント用)20091226追加 */
long __stdcall MIDIPart_SetNoteOff (MIDIPart* pMIDIPart, long lKey, long lVel) {
	assert (pMIDIPart);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVel && lVel <= 127);
	pMIDIPart->m_cNote[lKey] = 0;
	/* ペダルが押されていない場合 */
	if (pMIDIPart->m_cControlChange[64] <= 63) {
		pMIDIPart->m_cNoteKeep[lKey] = 0;
	}
	/* ペダルが押されている場合は現在のm_cNoteKeep[lKey]の値を保持 */
	return 1;
}

/* 現在のキーアフタータッチの値を設定 */
long __stdcall MIDIPart_SetKeyAfterTouch (MIDIPart* pMIDIPart, long lKey, long lVal) {
	assert (pMIDIPart);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIPart->m_cKeyAfterTouch[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在のコントロールチェンジの値を設定 */
long __stdcall MIDIPart_SetControlChange (MIDIPart* pMIDIPart, long lNum, long lVal) {
	unsigned char cCC98, cCC99, cCC100, cCC101, cNum, cVal;
	MIDIStatus* pMIDIStatus;
	assert (pMIDIPart);
	assert (0 <= lNum && lNum <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIPart->m_cControlChange[lNum] = (unsigned char)lVal;
	/* CC#別の特殊処理 */
	cCC98 = pMIDIPart->m_cControlChange[98];
	cCC99 = pMIDIPart->m_cControlChange[99];
	cCC100 = pMIDIPart->m_cControlChange[100];
	cCC101 = pMIDIPart->m_cControlChange[101];
	cNum = (unsigned char)lNum;
	cVal = (unsigned char)lVal;
	pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
	switch (lNum) {
	case 64: /* ホールド1解放処理(CC#64) */
	case 69: /* ホールド2解放処理(CC#69) */
		if (lVal <= 63) {
			int j;
			for (j = 0; j < 128; j++) {
				if (pMIDIPart->m_cNote[j] == 0) {
					pMIDIPart->m_cNoteKeep[j] = 0;
				}
			}
		}
		break;
	case 0: /* バンクセレクトMSB(CC#0) */
/*		if (pMIDIStatus->m_lModuleMode == MIDISTATUS_MODEGM2) {
			if (lVal == 120) {
				if (pMIDIPart == MIDIStatus_GetMIDIPart (pMIDIStatus, 9)) {
					pMIDIPart->m_lPartMode = 1;	
				}
				else {
					pMIDIPart->m_lPartMode = 2;
				}
			}
			else {
				pMIDIPart->m_lPartMode = 0;
			}
		}
		else if (pMIDIStatus->m_lModuleMode == MIDISTATUS_MODEXG) {
			if (lVal == 127) {
				if (pMIDIPart == MIDIStatus_GetMIDIPart (pMIDIStatus, 9)) {
					pMIDIPart->m_lPartMode = 1;	
				}
				else {
					pMIDIPart->m_lPartMode = 2;
				}
			}
			else {
				pMIDIPart->m_lPartMode = 0;
			}
		}*/
		break;
	case 6: /* データエントリーMSB(CC#6) */
		if (cCC99 != 127 && cCC98 != 127) {
			MIDIPart_SetNRPNMSB (pMIDIPart, cCC99, cCC98, cVal);
		}
		else if (cCC101 != 127 && cCC100 != 127) {
			MIDIPart_SetRPNMSB (pMIDIPart, cCC101, cCC100, cVal);
		}
		break;
	case 38: /* データエントリーLSB(CC#38) */
		if (cCC99 != 127 && cCC98 != 127) {
			MIDIPart_SetNRPNLSB (pMIDIPart, cCC99, cCC98, cVal);
		}
		else if (cCC101 != 127 && cCC100 != 127) {
			MIDIPart_SetRPNLSB (pMIDIPart, cCC101, cCC100, cVal);
		}
		break;
	case 120: /* オールサウンドオフ(CC#120) */
		memset (pMIDIPart->m_cNote, 0, 128);
		memset (pMIDIPart->m_cNoteKeep, 0, 128);
		break;
	case 121: /* リセットオールコントローラー(CC#121) */
		pMIDIPart->m_cControlChange[1] = 0;
		pMIDIPart->m_cControlChange[11] = 127;
		pMIDIPart->m_cControlChange[64] = 0;
		pMIDIPart->m_cControlChange[65] = 0;
		pMIDIPart->m_cControlChange[66] = 0;
		pMIDIPart->m_cControlChange[67] = 0;
		pMIDIPart->m_cControlChange[98] = 127;
		pMIDIPart->m_cControlChange[99] = 127;
		pMIDIPart->m_cControlChange[100] = 127;
		pMIDIPart->m_cControlChange[101] = 127;
		pMIDIPart->m_lPitchBend = 8192;
		pMIDIPart->m_lChannelAfterTouch = 0;
		memset (pMIDIPart->m_cKeyAfterTouch, 0, 128);
		break;
	case 123: /* オールノートオフ(CC#123) */
		memset (pMIDIPart->m_cNote, 0, 128);
		memset (pMIDIPart->m_cNoteKeep, 0, 128);
		break;
	case 124: /* オムニモードオフ */
		pMIDIPart->m_lOmniMonoPolyMode = 1;
		memset (pMIDIPart->m_cNote, 0, 128);
		memset (pMIDIPart->m_cNoteKeep, 0, 128);
		break;
	case 125: /* オムニモードオン */
		pMIDIPart->m_lOmniMonoPolyMode = 2;
		memset (pMIDIPart->m_cNote, 0, 128);
		memset (pMIDIPart->m_cNoteKeep, 0, 128);
		break;
	case 126: /* モノモードオン */
		pMIDIPart->m_lOmniMonoPolyMode = 3;
		memset (pMIDIPart->m_cNote, 0, 128);
		memset (pMIDIPart->m_cNoteKeep, 0, 128);
		break;
	case 127: /* ポリモードオン */
		pMIDIPart->m_lOmniMonoPolyMode = 4;
		memset (pMIDIPart->m_cNote, 0, 128);
		memset (pMIDIPart->m_cNoteKeep, 0, 128);
		break;
	}
	return 1;
}

/* 現在のRPNMSBの値を設定 */
long __stdcall MIDIPart_SetRPNMSB (MIDIPart* pMIDIPart, long lCC101, long lCC100, long lVal) {
	unsigned char cCC101, cCC100, cVal;
	assert (pMIDIPart);
	assert (0 <= lCC101 && lCC101 <= 127);
	assert (0 <= lCC100 && lCC100 <= 127);
	assert (0 <= lVal && lVal <= 127);
	cCC101 = (unsigned char)lCC101;
	cCC100 = (unsigned char)lCC100;
	cVal = (unsigned char)lVal;
#ifdef MIDISTATUS_RECORDRPN
	if (cCC101 != 127 && cCC100 != 127) {
		pMIDIPart->m_cRPNMSB[cCC101][cCC100] = cVal;
	}
#endif
	/* ピッチベンドレンジ(RPN#0) */
	if (cCC101 == 0 && cCC100 == 0) {
		pMIDIPart->m_lPitchBendSensitivity = cVal;
	}
	/* チャンネルファインチューンMSB(RPN#1) */
	else if (cCC101 == 0 && cCC100 == 1) {
		unsigned char cLSB = (pMIDIPart->m_lChannelFineTuning) & 0x007F;
		pMIDIPart->m_lChannelFineTuning = cVal * 128 + cLSB;
	}
	/* チャンネルコースチューン(RPN#2) */
	else if (cCC101 == 0 && cCC100 == 2) {
		pMIDIPart->m_lChannelCoarseTuning = cVal;
	}
	/* モジュレーションデプスレンジMSB(RPN#5) */
	else if (cCC101 == 0 && cCC100 == 5) {
		unsigned char cLSB = (pMIDIPart->m_lModulationDepthRange) & 0x007F;
		pMIDIPart->m_lModulationDepthRange = cVal * 128 + cLSB;
	}
	return 1;
}

/* 現在のRPNLSBの値を設定 */
long __stdcall MIDIPart_SetRPNLSB (MIDIPart* pMIDIPart, long lCC101, long lCC100, long lVal) {
	unsigned char cCC101, cCC100, cVal;
	assert (pMIDIPart);
	assert (0 <= lCC101 && lCC101 <= 127);
	assert (0 <= lCC100 && lCC100 <= 127);
	assert (0 <= lVal && lVal <= 127);
	cCC101 = (unsigned char)lCC101;
	cCC100 = (unsigned char)lCC100;
	cVal = (unsigned char)lVal;
#ifdef MIDISTATUS_RECORDRPN
	if (cCC101 != 127 && cCC100 != 127) {
		pMIDIPart->m_cRPNLSB[lCC101][lCC100] = cVal;
	}
#endif
	/* チャンネルファインチューンLSB(RPN#1) */
	if (cCC101 == 0 && cCC100 == 1) {
		unsigned char cMSB = ((pMIDIPart->m_lChannelFineTuning) >> 7) & 0x7F;
		pMIDIPart->m_lChannelFineTuning = cMSB * 128 + cVal;
	}
	/* モジュレーションデプスレンジLSB(RPN#5) */
	else if (cCC101 == 0 && cCC100 == 5) {
		unsigned char cMSB = ((pMIDIPart->m_lModulationDepthRange) >> 7) & 0x7F;
		pMIDIPart->m_lModulationDepthRange = cMSB * 128 + cVal;
	}
	return 1;
}

/* 現在のNRPNMSBの値を設定 */
long __stdcall MIDIPart_SetNRPNMSB (MIDIPart* pMIDIPart, long lCC99, long lCC98, long lVal) {
	unsigned char cCC99, cCC98, cVal;
	assert (pMIDIPart);
	assert (0 <= lCC99 && lCC99 <= 127);
	assert (0 <= lCC98 && lCC98 <= 127);
	assert (0 <= lVal && lVal <= 127);
	cCC99 = (unsigned char)lCC99;
	cCC98 = (unsigned char)lCC98;
	cVal = (unsigned char)lVal;
#ifdef MIDISTATUS_RECORDNRPN
	if (cCC99 != 127 && cCC98 != 127) {
		pMIDIPart->m_cNRPNMSB[cCC99][cCC98] = cVal;
	}
#endif
	/* ビブラートレイト(NRPN#(1*128+8)) */
	if (cCC99 == 1 && cCC98 == 8) {
		pMIDIPart->m_cControlChange[76]= cVal;
	}
	/* ビブラートデプス(NRPN#(1*128+9)) */
	else if (cCC99 == 1 && cCC98 == 9) {
		pMIDIPart->m_cControlChange[77]= cVal;
	}
	/* ビブラートディレイ(NRPN#(1*128+10)) */
	else if (cCC99 == 1 && cCC98 == 10) {
		pMIDIPart->m_cControlChange[78]= cVal;
	}
	/* カットオフフリーケンシー(NRPN#(1*128+32)) */
	else if (cCC99 == 1 && cCC98 == 32) {
		pMIDIPart->m_cControlChange[74]= cVal;
	}
	/* レゾナンス(NRPN#(1*128+33)) */
	else if (cCC99 == 1 && cCC98 == 33) {
		pMIDIPart->m_cControlChange[71]= cVal;
	}
	/* アタックタイム(NRPN#(1*128+99)) */
	else if (cCC99 == 1 && cCC98 == 99) {
		pMIDIPart->m_cControlChange[73]= cVal;
	}
	/* ディケイタイム(NRPN#(1*128+100)) */
	else if (cCC99 == 1 && cCC98 == 100) {
		pMIDIPart->m_cControlChange[75]= cVal;
	}
	/* リリースタイム(NRPN#(1*128+102)) */
	else if (cCC99 == 1 && cCC98 == 102) {
		pMIDIPart->m_cControlChange[72]= cVal;
	}
	/* 各ノート毎のドラムカットオフフリーケンシー(NRPN#(20*128台)) */
	else if (cCC99 == 20) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumCutoffFrequency[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムレゾナンス(NRPN#(21*128台)) */
	else if (cCC99 == 21) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumResonance[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムアタックタイム(NRPN#(22*128台)) */
	else if (cCC99 == 22) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumAttackTime[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムディケイタイム1及び2(NRPN#(23*128台)) */
	else if (cCC99 == 23) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumDecay1Time[cCC98] = cVal;
			pMIDIDrumSetup->m_cDrumDecay2Time[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムピッチコース(NRPN#(24*128台)) */
	else if (cCC99 == 24) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumPitchCoarse[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムピッチファイン(NRPN#(25*128台)) */
	else if (cCC99 == 25) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumPitchFine[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムレベル(NRPN#(26*128台)) */
	else if (cCC99 == 26) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumVolume[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムパン(NRPN#(28*128台)) */
	else if (cCC99 == 28) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumPan[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムリバーブ(NRPN#(29*128台)) */
	else if (cCC99 == 29) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumReverb[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムコーラス(NRPN#(30*128台)) */
	else if (cCC99 == 30) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumChorus[cCC98] = cVal;
		}
	}
	/* 各ノート毎のドラムディレイ(NRPN#(31*128台))(XGではディレイはヴァリエーションと読み替える) */
	else if (cCC99 == 31) {
		MIDIStatus* pMIDIStatus = (MIDIStatus*)(pMIDIPart->m_pParent);
		if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
			MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
			pMIDIDrumSetup->m_cDrumDelay[cCC98] = cVal;
		}
	}
	return 1;
}

/* 現在のNRPNLSBの値を設定 */
long __stdcall MIDIPart_SetNRPNLSB (MIDIPart* pMIDIPart, long lCC99, long lCC98, long lVal) {
	unsigned char cCC99, cCC98, cVal;
	assert (pMIDIPart);
	assert (0 <= lCC99 && lCC99 <= 127);
	assert (0 <= lCC98 && lCC98 <= 127);
	assert (0 <= lVal && lVal <= 127);
	cCC99 = (unsigned char)lCC99;
	cCC98 = (unsigned char)lCC98;
	cVal = (unsigned char)lVal;
#ifdef MIDISTATUS_RECORDRPN
	if (cCC99 != 127 && cCC98 != 127) {
		pMIDIPart->m_cRPNLSB[lCC99][lCC98] = cVal;
	}
#endif
	return 1;
}



/* 現在のプログラムチェンジの値を設定 */
long __stdcall MIDIPart_SetProgramChange (MIDIPart* pMIDIPart, long lNum) {
	MIDIStatus* pMIDIStatus = NULL;
	assert (pMIDIPart);
	assert (0 <= lNum && lNum <= 127);
	pMIDIPart->m_lProgramChange = lNum;
	pMIDIStatus = (MIDIStatus*)pMIDIPart->m_pParent;
	/* GM2の場合は、Bank Select MSB (CC#0) の値によってパートモードが切り替わる。 */
	if (pMIDIStatus->m_lModuleMode == MIDISTATUS_MODEGM2) {
		if (pMIDIPart->m_cControlChange[0] == 120) {
			if (pMIDIPart == MIDIStatus_GetMIDIPart (pMIDIStatus, 9)) {
				pMIDIPart->m_lPartMode = 1;	
			}
			else {
				pMIDIPart->m_lPartMode = 2;
			}
		}
		else {
			pMIDIPart->m_lPartMode = 0;
		}
	}
	else if (pMIDIStatus->m_lModuleMode == MIDISTATUS_MODEXG) {
		if (pMIDIPart->m_cControlChange[0] == 127) {
			if (pMIDIPart == MIDIStatus_GetMIDIPart (pMIDIStatus, 9)) {
				pMIDIPart->m_lPartMode = 1;	
			}
			else {
				pMIDIPart->m_lPartMode = 2;
			}
		}
		else {
			pMIDIPart->m_lPartMode = 0;
		}
	}
	/* ドラムモードの場合、すべてのドラム固有設定をリセットする。 */
	if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
		MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
		memset (pMIDIDrumSetup->m_cDrumCutoffFrequency, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumResonance, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumAttackTime, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumDecay1Time, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumDecay2Time, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumPitchCoarse, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumPitchFine, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumVolume, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumPan, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumReverb, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumChorus, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumDelay, 64, 128);
#ifdef MIDISTATUS_RECORDNRPN
		memset (&(pMIDIPart->m_cNRPNMSB[20][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[21][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[22][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[23][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[24][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[25][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[26][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[28][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[29][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[30][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[31][0]), 64, 128);
#endif
	}
	return 1;
}

/* 現在のチャンネルアフタータッチの値を設定 */
long __stdcall MIDIPart_SetChannelAfterTouch (MIDIPart* pMIDIPart, long lVal) {
	assert (pMIDIPart);
	assert (0 <= lVal && lVal <= 127);
	pMIDIPart->m_lChannelAfterTouch = lVal;
	return 1;
}

/* 現在のピッチベンドの値を設定 */
long __stdcall MIDIPart_SetPitchBend (MIDIPart* pMIDIPart, long lVal) {
	assert (pMIDIPart);
	assert (0 <= lVal && lVal <= 16383);
	pMIDIPart->m_lPitchBend = lVal;
	return 1;
}



/* MIDIPartの書き込み(外部隠蔽、この関数はMIDIStatus_Writeから呼び出される) */
long __stdcall MIDIPart_Write (MIDIPart* pMIDIPart, FILE* pFile, long lVersion) {
	assert (pMIDIPart);
	assert (pFile);
	assert (0 <= lVersion && lVersion <= 9999);
	if (fwrite (&(pMIDIPart->m_lPartMode), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lOmniMonoPolyMode), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lChannelFineTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lChannelCoarseTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lPitchBendSensitivity), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lModulationDepthRange), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lReceiveChannel), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lVelocitySenseDepth), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lVelocitySenseOffset), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lKeyboardRangeLow), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lKeyboardRangeHigh), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lAssignableControler1Num), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lAssignableControler2Num), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lReserved1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lReserved2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_lScaleOctaveTuning, sizeof(long), 12, pFile) != 12) {
		return 0;
	}
	/* コントローラーディスティネーションセッティング */
	if (fwrite (pMIDIPart->m_cModulationDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cPitchBendDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cChannelAfterTouchDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cKeyAfterTouchDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cAssignableControler1Dest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cAssignableControler2Dest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	/* チャンネルボイス領域 */
	if (fwrite (pMIDIPart->m_cNote, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cNoteKeep, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cKeyAfterTouch, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIPart->m_cControlChange, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lProgramChange), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lChannelAfterTouch), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lPitchBend), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	/* ユーザー用拡張領域 */
	if (fwrite (&(pMIDIPart->m_lUser1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lUser2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lUser3), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIPart->m_lUserFlag), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	return 1;
}

/* MIDIPartの読み込み(外部隠蔽、この関数はMIDIStatus_Readから呼び出される) */
long __stdcall MIDIPart_Read (MIDIPart* pMIDIPart, FILE* pFile, long lVersion) {
	assert (pMIDIPart);
	assert (pFile);
	assert (0 <= lVersion && lVersion <= 9999);
	if (fread (&(pMIDIPart->m_lPartMode), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lOmniMonoPolyMode), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lChannelFineTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lChannelCoarseTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lPitchBendSensitivity), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lModulationDepthRange), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lReceiveChannel), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lVelocitySenseDepth), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lVelocitySenseOffset), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lKeyboardRangeLow), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lKeyboardRangeHigh), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lAssignableControler1Num), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lAssignableControler2Num), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lReserved1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lReserved2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (pMIDIPart->m_lScaleOctaveTuning, sizeof(long), 12, pFile) != 12) {
		return 0;
	}
	/* コントローラーディスティネーションセッティング */
	if (fread (pMIDIPart->m_cModulationDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fread (pMIDIPart->m_cPitchBendDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fread (pMIDIPart->m_cChannelAfterTouchDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fread (pMIDIPart->m_cKeyAfterTouchDest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fread (pMIDIPart->m_cAssignableControler1Dest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	if (fread (pMIDIPart->m_cAssignableControler2Dest, sizeof(unsigned char), 16, pFile) != 16) {
		return 0;
	}
	/* チャンネルボイス領域 */
	if (fread (pMIDIPart->m_cNote, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIPart->m_cNoteKeep, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIPart->m_cKeyAfterTouch, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIPart->m_cControlChange, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lProgramChange), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lChannelAfterTouch), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lPitchBend), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	/* ユーザー用拡張領域 */
	if (fread (&(pMIDIPart->m_lUser1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lUser2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lUser3), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIPart->m_lUserFlag), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	return 1;
}




/******************************************************************************/
/*                                                                            */
/* MIDIDrumSetup関数群                                                        */
/*                                                                            */
/******************************************************************************/

/* MIDIDrumSetupオブジェクトを削除する。 */
int __stdcall MIDIDrumSetup_Delete (MIDIDrumSetup* pMIDIDrumSetup) {
	free (pMIDIDrumSetup);
	return 1;
}

/* MIDIDrumSetupオブジェクトを生成する。 */
MIDIDrumSetup* __stdcall MIDIDrumSetup_Create (MIDIStatus* pParent) {
	MIDIDrumSetup* pMIDIDrumSetup;
	pMIDIDrumSetup = malloc (sizeof (MIDIDrumSetup));
	if (pMIDIDrumSetup == NULL) {
		return NULL;
	}
	memset (pMIDIDrumSetup, 0, sizeof (MIDIDrumSetup));
	pMIDIDrumSetup->m_pParent = pParent;
	return pMIDIDrumSetup;
}

/* MIDIDrumSetup_Get系関数 */

/* 現在の楽器ごとのカットオフフリーケンシーの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumCutoffFrequency (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumCutoffFrequency[lKey];
}

/* 現在の楽器ごとのカットオフフリーケンシーの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumCutoffFrequencyEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumCutoffFrequency, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのレゾナンスの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumResonance (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumResonance[lKey];
}

/* 現在の楽器ごとのレゾナンスの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumResonanceEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumResonance, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのアタックタイムの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumAttackTime (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumAttackTime[lKey];
}

/* 現在の楽器ごとのアタックタイムの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumAttackTimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumAttackTime, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのディケイタイム1の値を取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay1Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumDecay1Time[lKey];
}

/* 現在の楽器ごとのディケイタイム1の値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay1TimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumDecay1Time, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのディケイタイム2の値を取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay2Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumDecay2Time[lKey];
}

/* 現在の楽器ごとのディケイタイム2の値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumDecay2TimeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumDecay2Time, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのピッチコースの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchCoarse (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumPitchCoarse[lKey];
}

/* 現在の楽器ごとのピッチコースの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchCoarseEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumPitchCoarse, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのピッチファインの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchFine (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumPitchFine[lKey];
}

/* 現在の楽器ごとのピッチファインの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumPitchFineEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumPitchFine, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのボリュームの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumVolume (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumVolume[lKey];
}

/* 現在の楽器ごとのボリュームの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumVolumeEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumVolume, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのパンの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumPan (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumPan[lKey];
}

/* 現在の楽器ごとのパンの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumPanEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumPan, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのリバーブの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumReverb (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumReverb[lKey];
}

/* 現在の楽器ごとのリバーブの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumReverbEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumReverb, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのコーラスの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumChorus (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumChorus[lKey];
}

/* 現在の楽器ごとのコーラスの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumChorusEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumChorus, MIN (lLen, 128));
	return 1;
}

/* 現在の楽器ごとのディレイの値を取得 */
long __stdcall MIDIDrumSetup_GetDrumDelay (MIDIDrumSetup* pMIDIDrumSetup, long lKey) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	return pMIDIDrumSetup->m_cDrumDelay[lKey];
}

/* 現在の楽器ごとのディレイの値をまとめて取得 */
long __stdcall MIDIDrumSetup_GetDrumDelayEx (MIDIDrumSetup* pMIDIDrumSetup, unsigned char* pBuf, long lLen) {
	assert (pMIDIDrumSetup);
	assert (pBuf);
	memcpy (pBuf, pMIDIDrumSetup->m_cDrumDelay, MIN (lLen, 128));
	return 1;
}

/* MIDIDrumSetup_Set系関数 */

/* 現在の楽器ごとのカットオフフリーケンシーの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumCutoffFrequency (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumCutoffFrequency[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのレゾナンスの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumResonance (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumResonance[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのアタックタイムの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumAttackTime (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumAttackTime[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのディケイタイム1の値を設定 */
long __stdcall MIDIDrumSetup_SetDrumDecay1Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumDecay1Time[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのディケイタイム2の値を設定 */
long __stdcall MIDIDrumSetup_SetDrumDecay2Time (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumDecay2Time[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのピッチコースの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumPitchCoarse (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumPitchCoarse[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのピッチファインの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumPitchFine (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumPitchFine[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのボリュームの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumVolume (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumVolume[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのパンの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumPan (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumPan[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのリバーブの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumReverb (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumReverb[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのコーラスの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumChorus (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumChorus[lKey] = (unsigned char)lVal;
	return 1;
}

/* 現在の楽器ごとのディレイの値を設定 */
long __stdcall MIDIDrumSetup_SetDrumDelay (MIDIDrumSetup* pMIDIDrumSetup, long lKey, long lVal) {
	assert (pMIDIDrumSetup);
	assert (0 <= lKey && lKey <= 127);
	assert (0 <= lVal && lVal <= 127);
	pMIDIDrumSetup->m_cDrumDelay[lKey] = (unsigned char)lVal;
	return 1;
}



/* MIDIDrumSetupの書き込み(外部隠蔽、この関数はMIDIStatus_Writeから呼び出される) */
long __stdcall MIDIDrumSetup_Write (MIDIDrumSetup* pMIDIDrumSetup, FILE* pFile, long lVersion) {
	assert (pMIDIDrumSetup);
	assert (pFile);
	assert (0 <= lVersion && lVersion <= 9999);
	if (fwrite (pMIDIDrumSetup->m_cDrumCutoffFrequency, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumResonance, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumAttackTime, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumDecay1Time, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumDecay2Time, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumPitchFine, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumPitchCoarse, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumVolume, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumPan, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumReverb, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumChorus, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fwrite (pMIDIDrumSetup->m_cDrumDelay, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	/* ユーザー用拡張領域 */
	if (fwrite (&(pMIDIDrumSetup->m_lUser1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIDrumSetup->m_lUser2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIDrumSetup->m_lUser3), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIDrumSetup->m_lUserFlag), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	return 1;
}

/* MIDIDrumSetupの読み込み(外部隠蔽、この関数はMIDIStatus_Readから呼び出される) */
long __stdcall MIDIDrumSetup_Read (MIDIDrumSetup* pMIDIDrumSetup, FILE* pFile, long lVersion) {
	assert (pMIDIDrumSetup);
	assert (pFile);
	assert (0 <= lVersion && lVersion <= 9999);
	if (fread (pMIDIDrumSetup->m_cDrumCutoffFrequency, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumResonance, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumAttackTime, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumDecay1Time, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumDecay2Time, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumPitchFine, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumPitchCoarse, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumVolume, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumPan, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumReverb, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumChorus, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	if (fread (pMIDIDrumSetup->m_cDrumDelay, sizeof(unsigned char), 128, pFile) != 128) {
		return 0;
	}
	/* ユーザー用拡張領域 */
	if (fread (&(pMIDIDrumSetup->m_lUser1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIDrumSetup->m_lUser2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIDrumSetup->m_lUser3), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIDrumSetup->m_lUserFlag), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	return 1;
}



/******************************************************************************/
/*                                                                            */
/* MIDIStatus関数群                                                           */
/*                                                                            */
/******************************************************************************/

/* MIDIStatusオブジェクトを削除する。 */
void __stdcall MIDIStatus_Delete (MIDIStatus* pMIDIStatus) {
	int i;
	if (pMIDIStatus == NULL) {
		return;
	}
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) { 
		free (pMIDIStatus->m_pMIDIPart[i]);
	}
	for (i = 0; i < pMIDIStatus->m_lNumMIDIDrumSetup; i++) {
		free (pMIDIStatus->m_pMIDIDrumSetup[i]);
	}
	free (pMIDIStatus);
}

/* MIDIStatusオブジェクトを生成する。 */
MIDIStatus* __stdcall MIDIStatus_Create (long lModuleMode, long lNumMIDIPart, long lNumMIDIDrumSetup) {
	MIDIStatus* pMIDIStatus = NULL;
	int i;
	if (lNumMIDIPart < 0 || lNumMIDIPart > MIDISTATUS_MAXMIDIPART) {
		return NULL;
	}
	if (lNumMIDIDrumSetup < 0 || lNumMIDIDrumSetup > MIDISTATUS_MAXMIDIDRUMSETUP) {
		return NULL;
	}
	pMIDIStatus = calloc (1, sizeof (MIDIStatus));
	if (pMIDIStatus == NULL) {
		return NULL;
	}
	memset (pMIDIStatus, 0, sizeof (MIDIStatus));
	pMIDIStatus->m_lModuleMode = lModuleMode;
	pMIDIStatus->m_lNumMIDIPart = lNumMIDIPart;
	pMIDIStatus->m_lNumMIDIDrumSetup = lNumMIDIDrumSetup;
	/* MIDIPartオブジェクトの生成 */
	for (i = 0; i < lNumMIDIPart; i++) {
		pMIDIStatus->m_pMIDIPart[i] = MIDIPart_Create (pMIDIStatus);
		if (pMIDIStatus->m_pMIDIPart[i] == NULL) {
			break;
		}
	}
	if (i < lNumMIDIPart) {
		MIDIStatus_Delete (pMIDIStatus);
		return NULL;
	}
	/* MIDIDrumSetupオブジェクトの生成 */
	for (i = 0; i < lNumMIDIDrumSetup; i++) {
		pMIDIStatus->m_pMIDIDrumSetup[i] = MIDIDrumSetup_Create (pMIDIStatus);
		if (pMIDIStatus->m_pMIDIDrumSetup[i] == NULL) {
			break;
		}
	}
	if (i < lNumMIDIDrumSetup) {
		MIDIStatus_Delete (pMIDIStatus);
		return NULL;
	}
	/* パラメータの初期化 */
	MIDIStatus_SetModuleMode (pMIDIStatus, lModuleMode);
	return pMIDIStatus;
}

/* MIDIStatus_Get系関数 */

/* 現在のモード(NATIVE/GM/GM2/GS/88/XG)を取得 */
long __stdcall MIDIStatus_GetModuleMode (MIDIStatus* pMIDIStatus) {
	return pMIDIStatus->m_lModuleMode;
}

/* 現在のマスターファインチューニングの値を取得 */
long __stdcall MIDIStatus_GetMasterFineTuning (MIDIStatus* pMIDIStatus) {
	assert (pMIDIStatus);
	return pMIDIStatus->m_lMasterFineTuning;
}

/* 現在のマスターコースチューニングの値を取得 */
long __stdcall MIDIStatus_GetMasterCoarseTuning (MIDIStatus* pMIDIStatus) {
	assert (pMIDIStatus);
	return pMIDIStatus->m_lMasterCoarseTuning;
}

/* 現在のマスターボリュームの値を取得 */
long __stdcall MIDIStatus_GetMasterVolume (MIDIStatus* pMIDIStatus) {
	assert (pMIDIStatus);
	return pMIDIStatus->m_lMasterVolume;
}

/* 現在のマスターパンの値を取得 */
long __stdcall MIDIStatus_GetMasterPan (MIDIStatus* pMIDIStatus) {
	assert (pMIDIStatus);
	return pMIDIStatus->m_lMasterPan;
}

/* 現在のマスターリバーブの値を1つ取得 */
long __stdcall MIDIStatus_GetMasterReverb (MIDIStatus* pMIDIStatus, long lNum) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 24);
	return pMIDIStatus->m_lMasterReverb[lNum];
}

/* 現在のマスターリバーブの値をまとめて取得 */
long __stdcall MIDIStatus_GetMasterReverbEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen) {
	assert (pMIDIStatus);
	assert (pBuf);
	memcpy (pBuf, pMIDIStatus->m_lMasterReverb, sizeof(long) * MIN (lLen, 24));
	return 1;
}

/* 現在のマスターコーラスの値を1つ取得 */
long __stdcall MIDIStatus_GetMasterChorus (MIDIStatus* pMIDIStatus, long lNum) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 24);
	return pMIDIStatus->m_lMasterChorus[lNum];
}

/* 現在のマスターコーラスの値をまとめて取得 */
long __stdcall MIDIStatus_GetMasterChorusEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen) {
	assert (pMIDIStatus);
	assert (pBuf);
	memcpy (pBuf, pMIDIStatus->m_lMasterChorus, sizeof(long) * MIN (lLen, 24));
	return 1;
}

/* 現在のマスターディレイの値を1つ取得(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIStatus_GetMasterDelay (MIDIStatus* pMIDIStatus, long lNum) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 24);
	return pMIDIStatus->m_lMasterDelay[lNum];
}

/* 現在のマスターディレイの値をまとめて取得(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIStatus_GetMasterDelayEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen) {
	assert (pMIDIStatus);
	assert (pBuf);
	memcpy (pBuf, pMIDIStatus->m_lMasterDelay, sizeof(long) * MIN (lLen,  24));
	return 1;
}

/* 現在のマスターイコライザの値を1つ取得 */
long __stdcall MIDIStatus_GetMasterEqualizer (MIDIStatus* pMIDIStatus, long lNum) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 24);
	return pMIDIStatus->m_lMasterEqualizer[lNum];
}

/* 現在のマスターイコライザの値をまとめて取得 */
long __stdcall MIDIStatus_GetMasterEqualizerEx (MIDIStatus* pMIDIStatus, long* pBuf, long lLen) {
	assert (pMIDIStatus);
	assert (pBuf);
	memcpy (pBuf, pMIDIStatus->m_lMasterEqualizer, sizeof(long) * MIN (lLen, 24));
	return 1;
}

/* このMIDIStatusオブジェクトに含まれるMIDIPartの数を取得 */
long __stdcall MIDIStatus_GetNumMIDIPart (MIDIStatus* pMIDIStatus) {
	assert (pMIDIStatus);
	assert (0 <= pMIDIStatus->m_lNumMIDIPart && pMIDIStatus->m_lNumMIDIPart <= MIDISTATUS_MAXMIDIPART);
	return pMIDIStatus->m_lNumMIDIPart;
}

/* このMIDIStatusオブジェクトに含まれるMIDIDrumSetupの数を取得 */
long __stdcall MIDIStatus_GetNumMIDIDrumSetup (MIDIStatus* pMIDIStatus) {
	assert (pMIDIStatus);
	assert (0 <= pMIDIStatus->m_lNumMIDIDrumSetup && pMIDIStatus->m_lNumMIDIDrumSetup <= MIDISTATUS_MAXMIDIDRUMSETUP);
	return pMIDIStatus->m_lNumMIDIDrumSetup;
}

/* 各MIDIPartへのポインタを取得 */
MIDIPart* __stdcall MIDIStatus_GetMIDIPart (MIDIStatus* pMIDIStatus, long lIndex) {
	assert (pMIDIStatus);
	assert (0 <= pMIDIStatus->m_lNumMIDIPart && pMIDIStatus->m_lNumMIDIPart <= MIDISTATUS_MAXMIDIPART);
	assert (0 <= lIndex && lIndex < pMIDIStatus->m_lNumMIDIPart);
	if (0 <= lIndex && lIndex < pMIDIStatus->m_lNumMIDIPart) {
		return pMIDIStatus->m_pMIDIPart[lIndex];
	}
	return NULL;
}

/* 各MIDIDrumSetupへのポインタを取得 */
MIDIDrumSetup* __stdcall MIDIStatus_GetMIDIDrumSetup (MIDIStatus* pMIDIStatus, long lIndex) {
	assert (pMIDIStatus);
	assert (0 <= lIndex && lIndex < pMIDIStatus->m_lNumMIDIDrumSetup);
	assert (0 <= pMIDIStatus->m_lNumMIDIDrumSetup && pMIDIStatus->m_lNumMIDIDrumSetup <= MIDISTATUS_MAXMIDIDRUMSETUP);
	if (0 <= lIndex && lIndex < pMIDIStatus->m_lNumMIDIDrumSetup) {
		return pMIDIStatus->m_pMIDIDrumSetup[lIndex];
	}
	return NULL;
}

/* MIDIStatus_Set系関数 */

/* モジュールモード(NONE/GM/GM2/GS/88/XG)を設定する。 */
/* モジュールモードの設定に従いすべてのパラメータをデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetModuleMode (MIDIStatus* pMIDIStatus, long lModuleMode) {
	assert (pMIDIStatus);
	switch (lModuleMode) {
	case MIDISTATUS_MODENATIVE:
		MIDIStatus_PutReset (pMIDIStatus);
		break;
	case MIDISTATUS_MODEGM:
		MIDIStatus_PutGMReset (pMIDIStatus);
		break;
	case MIDISTATUS_MODEGM2:
		MIDIStatus_PutGM2Reset (pMIDIStatus);
		break;
	case MIDISTATUS_MODEGS:
		MIDIStatus_PutGSReset (pMIDIStatus);
		break;
	case MIDISTATUS_MODE88:
		MIDIStatus_Put88Reset (pMIDIStatus);
		break;
	case MIDISTATUS_MODEXG:
		MIDIStatus_PutXGReset (pMIDIStatus);
		break;
	default:
		MIDIStatus_PutReset (pMIDIStatus);
		break;
	}
	return 1;
}

/* 現在のマスターファインチューニングの値を設定 */
long __stdcall MIDIStatus_SetMasterFineTuning (MIDIStatus* pMIDIStatus, long lMasterFineTuning) {
	assert (pMIDIStatus);
	assert (0 <= lMasterFineTuning && lMasterFineTuning <= 16383);
	pMIDIStatus->m_lMasterFineTuning = lMasterFineTuning;
	return 1;
}

/* 現在のマスターコースチューニングの値を設定 */
long __stdcall MIDIStatus_SetMasterCoarseTuning (MIDIStatus* pMIDIStatus, long lMasterCoarseTuning) {
	assert (pMIDIStatus);
	assert (0 <= lMasterCoarseTuning && lMasterCoarseTuning <= 127);
	pMIDIStatus->m_lMasterCoarseTuning = lMasterCoarseTuning;
	return 1;
}

/* 現在のマスターボリュームの値を設定 */
long __stdcall MIDIStatus_SetMasterVolume (MIDIStatus* pMIDIStatus, long lMasterVolume) {
	assert (pMIDIStatus);
	assert (0 <= lMasterVolume && lMasterVolume <= 127);
	pMIDIStatus->m_lMasterVolume = lMasterVolume;
	return 1;
}

/* 現在のマスターパンの値を設定 */
long __stdcall MIDIStatus_SetMasterPan (MIDIStatus* pMIDIStatus, long lMasterPan) {
	assert (pMIDIStatus);
	assert (0 <= lMasterPan && lMasterPan <= 127);
	pMIDIStatus->m_lMasterPan = lMasterPan;
	return 1;
}

/* 現在のマスターリバーブの値を1つ設定 */
long __stdcall MIDIStatus_SetMasterReverb (MIDIStatus* pMIDIStatus, long lNum, long lVal) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 32);
	assert (0 <= lVal && lVal <= 16383);
	if (lNum == 0) {
		MIDIStatus_SetMasterReverbType (pMIDIStatus, lVal);
	}
	else {
		pMIDIStatus->m_lMasterReverb[lNum] = lVal;
	}
	return 1;
}

/* マスターリバーブのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetMasterReverbType (MIDIStatus* pMIDIStatus, long lMasterReverbType) {
	int i;
	assert (pMIDIStatus);
	switch (pMIDIStatus->m_lModuleMode) {
	case MIDISTATUS_MODEGM2:
		for (i = 0; i < 6; i++) {
			if (lMasterReverbType == g_lGM2MasterReverbDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterReverb, &g_lGM2MasterReverbDefault[i][0], sizeof(long) * 2);
				return 1;
			}
		}
		break;
	case MIDISTATUS_MODEGS:
	case MIDISTATUS_MODE88:
		for (i = 0; i < 8; i++) {
			if (lMasterReverbType == g_lGSMasterReverbDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterReverb, &g_lGSMasterReverbDefault[i][0], sizeof(long) * 8);
				return 1;
			}
		}
		break;
	case MIDISTATUS_MODEXG:
		for (i = 0; i < 19; i++) {
			if (lMasterReverbType == g_lXGMasterReverbDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterReverb, &g_lXGMasterReverbDefault[i][0], sizeof(long) * 17);
				return 1;
			}
		}
		memcpy (pMIDIStatus->m_lMasterReverb, &g_lXGMasterReverbDefault[0][0], sizeof(long) * 17);
		break;
	default:
		pMIDIStatus->m_lMasterReverb[0] = lMasterReverbType;
		return 1;
	}
	return 0;
}

/* 現在のマスターコーラスの値を1つ設定 */
long __stdcall MIDIStatus_SetMasterChorus (MIDIStatus* pMIDIStatus, long lNum, long lVal) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 32);
	assert (0 <= lVal && lVal <= 16383);
	if (lNum == 0) {
		MIDIStatus_SetMasterChorusType (pMIDIStatus, lVal);
	}
	else {
		pMIDIStatus->m_lMasterChorus[lNum] = lVal;
	}
	return 1;
}

/* マスターコーラスのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetMasterChorusType (MIDIStatus* pMIDIStatus, long lMasterChorusType) {
	int i;
	assert (pMIDIStatus);
	switch (pMIDIStatus->m_lModuleMode) {
	case MIDISTATUS_MODEGM2:
		for (i = 0; i < 6; i++) {
			if (lMasterChorusType == g_lGM2MasterChorusDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterChorus, &g_lGM2MasterChorusDefault[i][0], sizeof(long) * 5);
				return 1;
			}
		}
		break;
	case MIDISTATUS_MODEGS:
	case MIDISTATUS_MODE88:
		for (i = 0; i < 8; i++) {
			if (lMasterChorusType == g_lGSMasterChorusDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterChorus, &g_lGSMasterChorusDefault[i][0], sizeof(long) * 9);
				return 1;
			}
		}
		break;
	case MIDISTATUS_MODEXG:
		for (i = 0; i < 21; i++) {
			if (lMasterChorusType == g_lXGMasterChorusDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterChorus, &g_lXGMasterChorusDefault[i][0], sizeof(long) * 17);
				return 1;
			}
		}
		memcpy (pMIDIStatus->m_lMasterChorus, &g_lXGMasterChorusDefault[0][0], sizeof(long) * 17);
		break;
	default:
		pMIDIStatus->m_lMasterChorus[0] = lMasterChorusType;
		return 1;
	}
	return 0;
}

/* 現在のマスターディレイの値を1つ設定(XGではディレイはヴァリエーションと読み替える) */
long __stdcall MIDIStatus_SetMasterDelay (MIDIStatus* pMIDIStatus, long lNum, long lVal) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 32);
	assert (0 <= lVal && lVal <= 16383);
	if (lNum == 0) {
		MIDIStatus_SetMasterDelayType (pMIDIStatus, lVal);
	}
	else {
		pMIDIStatus->m_lMasterDelay[lNum] = lVal;
	}
	return 1;
}

/* マスターディレイのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
/* (XGではマスターヴァリエーションとなる) */
long __stdcall MIDIStatus_SetMasterDelayType (MIDIStatus* pMIDIStatus, long lMasterDelayType) {
	int i;
	assert (pMIDIStatus);
	switch (pMIDIStatus->m_lModuleMode) {
	case MIDISTATUS_MODEGS:
	case MIDISTATUS_MODE88:
		for (i = 0; i < 10; i++) {
			if (lMasterDelayType == g_lGSMasterDelayDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterDelay, &g_lGSMasterDelayDefault[i][0], sizeof(long) * 11);
				return 1;
			}
		}
		break;
	case MIDISTATUS_MODEXG:
		for (i = 0; i < 116; i++) {
			if (lMasterDelayType == g_lXGMasterVariationDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterDelay, &g_lXGMasterVariationDefault[i][0], sizeof(long) * 17);
				return 1;
			}
		}
		memcpy (pMIDIStatus->m_lMasterDelay, &g_lXGMasterVariationDefault[0][0], sizeof(long) * 17);
		break;
	default:
		pMIDIStatus->m_lMasterDelay[0] = lMasterDelayType;
		return 1;
	}
	return 0;
}

/* 現在のマスターイコライザの値を1つ設定 */
long __stdcall MIDIStatus_SetMasterEqualizer (MIDIStatus* pMIDIStatus, long lNum, long lVal) {
	assert (pMIDIStatus);
	assert (0 <= lNum && lNum < 32);
	assert (0 <= lVal && lVal <= 16383);
	if (lNum == 0) {
		MIDIStatus_SetMasterEqualizerType (pMIDIStatus, lVal);
	}
	else {
		pMIDIStatus->m_lMasterEqualizer[lNum] = lVal;
	}
	return 1;
}

/* マスターイコライザのタイプを設定し、それに伴い各パラメータもデフォルト値に設定する。 */
long __stdcall MIDIStatus_SetMasterEqualizerType (MIDIStatus* pMIDIStatus, long lMasterEqualizerType) {
	int i;
	assert (pMIDIStatus);
	switch (pMIDIStatus->m_lModuleMode) {
	case MIDISTATUS_MODEGS:
	case MIDISTATUS_MODE88:
		for (i = 0; i < 1; i++) {
			if (lMasterEqualizerType == g_lGSMasterEqualizerDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterEqualizer, &g_lGSMasterEqualizerDefault[i][0], sizeof(long) * 5);
				return 1;
			}
		}
		break;
	case MIDISTATUS_MODEXG:
		for (i = 0; i < 5; i++) {
			if (lMasterEqualizerType == g_lXGMasterEqualizerDefault[i][0]) {
				memcpy (pMIDIStatus->m_lMasterEqualizer, &g_lXGMasterEqualizerDefault[i][0], sizeof(long) * 21);
				return 1;
			}
		}
		break;
	default:
		pMIDIStatus->m_lMasterEqualizer[0] = lMasterEqualizerType;
		return 1;
	}
	return 0;
}


/* リセット */
long __stdcall MIDIStatus_PutReset (MIDIStatus* pMIDIStatus) {
	int i, j;
	pMIDIStatus->m_lModuleMode = MIDISTATUS_MODENATIVE;
	pMIDIStatus->m_lMasterFineTuning = 8192;
	pMIDIStatus->m_lMasterCoarseTuning = 64;
	pMIDIStatus->m_lMasterBalance = 0;
	pMIDIStatus->m_lMasterVolume = 127;
	pMIDIStatus->m_lMasterPan = 64;
	memset (pMIDIStatus->m_lMasterReverb, 0, sizeof(long) * 32);
	memset (pMIDIStatus->m_lMasterChorus, 0, sizeof(long) * 32);
	memset (pMIDIStatus->m_lMasterDelay, 0, sizeof(long) * 32);
	memset (pMIDIStatus->m_lMasterEqualizer, 0, sizeof(long) * 32);
	memset (pMIDIStatus->m_lMasterInsertion, 0, sizeof(long) * 32);
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
		MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
		pMIDIPart->m_lPartMode = ((i % 16) == 9) ? 1 : 0;
		pMIDIPart->m_lOmniMonoPolyMode = 3;
		pMIDIPart->m_lReceiveChannel = i;
		pMIDIPart->m_lChannelFineTuning = 8192;
		pMIDIPart->m_lChannelCoarseTuning = 64;
		pMIDIPart->m_lPitchBendSensitivity = 2;
		pMIDIPart->m_lVelocitySenseDepth = 64;
		pMIDIPart->m_lVelocitySenseOffset = 64;
		pMIDIPart->m_lKeyboardRangeLow = 0;
		pMIDIPart->m_lKeyboardRangeHigh = 127;
		for (j = 0; j < 12; j++) {
			pMIDIPart->m_lScaleOctaveTuning[j] = 64;
		}
		memset (pMIDIPart->m_cNote, 0, 128);
		memset (pMIDIPart->m_cNoteKeep, 0, 128);
		memset (pMIDIPart->m_cKeyAfterTouch, 0, 128);
		memset (pMIDIPart->m_cControlChange, 0, 128);
		pMIDIPart->m_cControlChange[7] = 100;
		pMIDIPart->m_cControlChange[10] = 64;
		pMIDIPart->m_cControlChange[11] = 127;
		pMIDIPart->m_cControlChange[71] = 64;
		pMIDIPart->m_cControlChange[72] = 64;
		pMIDIPart->m_cControlChange[73] = 64;
		pMIDIPart->m_cControlChange[74] = 64;
		pMIDIPart->m_cControlChange[75] = 64;
		pMIDIPart->m_cControlChange[76] = 64;
		pMIDIPart->m_cControlChange[77] = 64;
		pMIDIPart->m_cControlChange[78] = 64;
		pMIDIPart->m_cControlChange[91] = 40;
		pMIDIPart->m_cControlChange[98] = 127;
		pMIDIPart->m_cControlChange[99] = 127;
		pMIDIPart->m_cControlChange[100] = 127;
		pMIDIPart->m_cControlChange[101] = 127;
		pMIDIPart->m_lProgramChange = 0;
		pMIDIPart->m_lChannelAfterTouch = 0;
		pMIDIPart->m_lPitchBend = 8192;
#ifdef MIDISTATUS_RECORDRPN
		memset (&(pMIDIPart->m_cRPNMSB[0][0]), 0, 16384);
		memset (&(pMIDIPart->m_cRPNLSB[0][0]), 0, 16384);
		pMIDIPart->m_cRPNMSB[0][0] = 2;
		pMIDIPart->m_cRPNMSB[0][1] = 0x40;
		pMIDIPart->m_cRPNMSB[0][2] = 0x40;
		pMIDIPart->m_cRPNMSB[0][5] = 0; /* 不明 */
#endif
#ifdef MIDISTATUS_RECORDNRPN
		memset (&(pMIDIPart->m_cNRPNMSB[0][0]), 0, 16384);
		memset (&(pMIDIPart->m_cNRPNLSB[0][0]), 0, 16384);
		memset (&(pMIDIPart->m_cNRPNMSB[20][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[21][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[22][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[23][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[24][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[25][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[26][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[28][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[29][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[30][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[31][0]), 64, 128);
#endif
	}
	for (i = 0; i < pMIDIStatus->m_lNumMIDIDrumSetup; i++) {
		MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, i);
		memset (pMIDIDrumSetup->m_cDrumCutoffFrequency, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumResonance, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumAttackTime, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumDecay1Time, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumDecay2Time, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumPitchCoarse, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumPitchFine, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumVolume, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumPan, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumReverb, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumChorus, 64, 128);
		memset (pMIDIDrumSetup->m_cDrumDelay, 64, 128);
	}
	return 6;
}

/* GMリセット */
long __stdcall MIDIStatus_PutGMReset (MIDIStatus* pMIDIStatus) {
	MIDIStatus_PutReset (pMIDIStatus);
	pMIDIStatus->m_lModuleMode = MIDISTATUS_MODEGM;
	return 6;
}

/* GM2リセット */
long __stdcall MIDIStatus_PutGM2Reset (MIDIStatus* pMIDIStatus) {
	int i;
	MIDIPart* pMIDIPart;
	MIDIStatus_PutReset (pMIDIStatus);
	pMIDIStatus->m_lModuleMode = MIDISTATUS_MODEGM2;
	MIDIStatus_SetMasterReverbType (pMIDIStatus, 4); /* Large Hall */
	MIDIStatus_SetMasterChorusType (pMIDIStatus, 2); /* Chorus 3 */
	for (i = 0; i < 16; i++) {
		pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
		pMIDIPart->m_cControlChange[0] = (i == 9 ? 120 : 121);
	}
	return 6;
}

/* GSリセット */
long __stdcall MIDIStatus_PutGSReset (MIDIStatus* pMIDIStatus) {
#ifdef MIDISTATUS_RECORDNRPN
	int i;
#endif
	MIDIStatus_PutReset (pMIDIStatus);
	pMIDIStatus->m_lModuleMode = MIDISTATUS_MODEGS;
	pMIDIStatus->m_lMasterFineTuning = 1024;
	MIDIStatus_SetMasterReverbType (pMIDIStatus, 4); /* Hall2 */
	MIDIStatus_SetMasterChorusType (pMIDIStatus, 2); /* Chorus3 */
	MIDIStatus_SetMasterDelayType (pMIDIStatus, 0); /* Delay1 */
	MIDIStatus_SetMasterEqualizerType (pMIDIStatus, 0); /* Flat */
#ifdef MIDISTAUTS_RECORDNRPN
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
		MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
		pMIDIPart->m_cNRPNMSB[1][8] = 64;
		pMIDIPart->m_cNRPNMSB[1][9] = 64;
		pMIDIPart->m_cNRPNMSB[1][10] = 64;
		pMIDIPart->m_cNRPNMSB[1][32] = 64;
		pMIDIPart->m_cNRPNMSB[1][33] = 64;
		pMIDIPart->m_cNRPNMSB[1][99] = 64;
		pMIDIPart->m_cNRPNMSB[1][100] = 64;
		pMIDIPart->m_cNRPNMSB[1][102] = 64;
		memset (&(pMIDIPart->m_cNRPNMSB[20][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[21][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[22][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[23][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[24][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[25][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[26][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[28][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[29][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[30][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[31][0]), 64, 128);
	}
#endif
	return 11;
}

/* 88リセット */
long __stdcall MIDIStatus_Put88Reset (MIDIStatus* pMIDIStatus) {
#ifdef MIDISTATUS_RECORDNRPN
	int i;
#endif
	MIDIStatus_PutReset (pMIDIStatus);
	pMIDIStatus->m_lModuleMode = MIDISTATUS_MODE88;
	pMIDIStatus->m_lMasterFineTuning = 1024;
	MIDIStatus_SetMasterReverbType (pMIDIStatus, 4); /* Hall2 */
	MIDIStatus_SetMasterChorusType (pMIDIStatus, 2); /* Chorus3 */
	MIDIStatus_SetMasterDelayType (pMIDIStatus, 0); /* Delay1 */
	MIDIStatus_SetMasterEqualizerType (pMIDIStatus, 0); /* Flat */
#ifdef MIDISTAUTS_RECORDNRPN
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
		MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
		pMIDIPart->m_cNRPNMSB[1][8] = 64;
		pMIDIPart->m_cNRPNMSB[1][9] = 64;
		pMIDIPart->m_cNRPNMSB[1][10] = 64;
		pMIDIPart->m_cNRPNMSB[1][32] = 64;
		pMIDIPart->m_cNRPNMSB[1][33] = 64;
		pMIDIPart->m_cNRPNMSB[1][99] = 64;
		pMIDIPart->m_cNRPNMSB[1][100] = 64;
		pMIDIPart->m_cNRPNMSB[1][102] = 64;
		memset (&(pMIDIPart->m_cNRPNMSB[20][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[21][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[22][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[23][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[24][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[25][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[26][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[28][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[29][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[30][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[31][0]), 64, 128);
	}
#endif
	return 11;
}

/* XGリセット */
long __stdcall MIDIStatus_PutXGReset (MIDIStatus* pMIDIStatus) {
#ifdef MIDISTATUS_RECORDNRPN
	int i;
#endif
	MIDIPart* pMIDIPart = NULL;
	MIDIStatus_PutReset (pMIDIStatus);
	pMIDIStatus->m_lModuleMode = MIDISTATUS_MODEXG;
	pMIDIStatus->m_lMasterFineTuning = 1024;
	MIDIStatus_SetMasterReverbType (pMIDIStatus, 128); /* HALL1 */
	MIDIStatus_SetMasterChorusType (pMIDIStatus, 8320); /* CHORUS1 */
	MIDIStatus_SetMasterDelayType (pMIDIStatus, 640); /* Delay L,C,R */
	MIDIStatus_SetMasterEqualizerType (pMIDIStatus, 0); /* Flat */
	pMIDIStatus->m_lMasterReverb[17] = 64; /* リバーブリターン */
	pMIDIStatus->m_lMasterReverb[18] = 64; /* リバーブパン */
	pMIDIStatus->m_lMasterChorus[17] = 64; /* コーラスリターン */
	pMIDIStatus->m_lMasterChorus[18] = 64; /* コーラスパン */
	pMIDIStatus->m_lMasterDelay[17] = 64; /* バリエーションリターン */
	pMIDIStatus->m_lMasterDelay[18] = 64; /* バリエーションパン */
	pMIDIStatus->m_lMasterDelay[22] = 127; /* バリエーションパート */
#ifdef MIDISTAUTS_RECORDNRPN
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
		MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
		pMIDIPart->m_cNRPNMSB[1][8] = 64;
		pMIDIPart->m_cNRPNMSB[1][9] = 64;
		pMIDIPart->m_cNRPNMSB[1][10] = 64;
		pMIDIPart->m_cNRPNMSB[1][32] = 64;
		pMIDIPart->m_cNRPNMSB[1][33] = 64;
		pMIDIPart->m_cNRPNMSB[1][99] = 64;
		pMIDIPart->m_cNRPNMSB[1][100] = 64;
		pMIDIPart->m_cNRPNMSB[1][102] = 64;
		memset (&(pMIDIPart->m_cNRPNMSB[20][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[21][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[22][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[23][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[24][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[25][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[26][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[28][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[29][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[30][0]), 64, 128);
		memset (&(pMIDIPart->m_cNRPNMSB[31][0]), 64, 128);
	}
#endif
	pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, 9);
	pMIDIPart->m_cControlChange[0] = 127;
	return 9;
}


/* MIDIメッセージを与えることによりパラメータを変更する。 */
long __stdcall MIDIStatus_PutMIDIMessage (MIDIStatus* pMIDIStatus, unsigned char* pMIDIMessage, long lLen) {
	long i, j;
	unsigned char cStatus;
	long lOffset = 0;
	assert (pMIDIStatus);
	assert (pMIDIMessage);
	assert (1 <= lLen && lLen <= 256);
	/* ランニングステータスが使われている場合の措置 */
	if (0x00 <= *pMIDIMessage && *pMIDIMessage <= 0x7F) {
		lOffset = -1;
		cStatus = (unsigned char)(pMIDIStatus->m_lRunningStatus);
	}
	/* ランニングステータスが使われていない場合、1バイト目を次回以降のランニングステータスとする。 */
	else if (0x80 <= *pMIDIMessage && *pMIDIMessage <= 0xEF) {
		pMIDIStatus->m_lRunningStatus = *pMIDIMessage;
		cStatus = *pMIDIMessage;
	}
	/* その他の場合(システムエクスクルーシブ・システムリアルタイムなど) */
	else {
		cStatus = *pMIDIMessage;
	}

	/* ノートオフ */
	if (0x80 <= cStatus && cStatus <= 0x8F && lLen >= 3 + lOffset) {
		unsigned char cCh = cStatus & 0x0F;
		unsigned char cKey = *(pMIDIMessage + 1 + lOffset);
		unsigned char cVel = *(pMIDIMessage + 2 + lOffset);
		if (0 <= cKey && cKey <= 127 &&	0 <= cVel && cVel <= 127) {
			for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
				MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
				if (pMIDIPart->m_lReceiveChannel == cCh) {
					MIDIPart_SetNoteOff (pMIDIPart, cKey, cVel); /* 20091226修正 */
				}
			}
			return 3 + lOffset;
		}
	}

	/* ノートオン */
	else if (0x90 <= cStatus && cStatus <= 0x9F && lLen >= 3 + lOffset) {
		unsigned char cCh = cStatus & 0x0F;
		unsigned char cKey = *(pMIDIMessage + 1 + lOffset);
		unsigned char cVel = *(pMIDIMessage + 2 + lOffset);
		if (0 <= cKey && cKey <= 127 &&	0 <= cVel && cVel <= 127) {
			for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
				MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
				if (pMIDIPart->m_lReceiveChannel == cCh) {
					MIDIPart_SetNote (pMIDIPart, cKey, cVel);
				}
			}
			return 3 + lOffset;
		}
	}

	/* キーアフタータッチ */
	else if (0xA0 <= cStatus && cStatus <= 0xAF && lLen >= 3 + lOffset) {
		unsigned char cCh = cStatus & 0x0F;
		unsigned char cKey = *(pMIDIMessage + 1 + lOffset);
		unsigned char cVal = *(pMIDIMessage + 2 + lOffset);
		if (0 <= cKey && cKey <= 127 &&	0 <= cVal && cVal <= 127) {
			for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
				MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
				if (pMIDIPart->m_lReceiveChannel == cCh) {
					MIDIPart_SetKeyAfterTouch (pMIDIPart, cKey, cVal);
				}
			}
			return 3 + lOffset;
		}
	}

	/* コントロールチェンジ */
	else if (0xB0 <= cStatus && cStatus <= 0xBF && lLen >= 3 + lOffset) {
		unsigned char cCh = cStatus & 0x0F;
		unsigned char cNum = *(pMIDIMessage + 1 + lOffset);
		unsigned char cVal = *(pMIDIMessage + 2 + lOffset);
		if (0 <= cNum && cNum <= 127 && 0 <= cVal && cVal <= 127) {
			for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
				MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
				if (pMIDIPart->m_lReceiveChannel == cCh) {
					MIDIPart_SetControlChange (pMIDIPart, cNum, cVal);
				}
			}
			return 3 + lOffset;
		}
	}

	/* プログラムチェンジ */
	else if (0xC0 <= cStatus && cStatus <= 0xCF && lLen >= 2 + lOffset) {
		unsigned char cCh = cStatus & 0x0F;
		unsigned char cNum = *(pMIDIMessage + 1 + lOffset);
		if (0 <= cNum && cNum <= 127) {
			for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
				MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
				if (pMIDIPart->m_lReceiveChannel == cCh) {
					MIDIPart_SetProgramChange (pMIDIPart, cNum);
				}
			}
			return 2 + lOffset;
		}
	}

	/* チャンネルアフタータッチ */
	else if (0xD0 <= cStatus && cStatus <= 0xDF && lLen >= 2 + lOffset) {
		unsigned char cCh = cStatus & 0x0F;
		unsigned char cVal = *(pMIDIMessage + 1 + lOffset);
		if (0 <= cVal && cVal <= 127) {
			for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
				MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
				if (pMIDIPart->m_lReceiveChannel == cCh) {
					MIDIPart_SetChannelAfterTouch (pMIDIPart, cVal);
				}
			}
			return 2 + lOffset;
		}
	}

	/* ピッチベンド */
	else if (0xE0 <= cStatus && cStatus <= 0xEF && lLen >= 3 + lOffset) {
		unsigned char cCh = cStatus & 0x0F;
		unsigned char cLSB = *(pMIDIMessage + 1 + lOffset);
		unsigned char cMSB = *(pMIDIMessage + 2 + lOffset);
		if (0 <= cLSB && cLSB <= 127 && 0 <= cMSB && cMSB <= 127) {
			for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
				MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
				if (pMIDIPart->m_lReceiveChannel == cCh) {
					MIDIPart_SetPitchBend (pMIDIPart, cMSB * 128 + cLSB);
				}
			}
			return 3 + lOffset;
		}
	}

	/* システムエクスクルーシヴメッセージ */
	else if (cStatus == 0xF0 && lLen >= 2) {
		/* ユニバーサルノンリアルタイムシステムエクスクルーシブメッセージ {0xF0, 0x7E, 0xid, ... , 0xF7} */
		if (*(pMIDIMessage + 1) == 0x7E && lLen >= 4) {
			unsigned char uID = *(pMIDIMessage + 2);
			/* システムモードセット {0xF0, 0x7E, 0xid, 0x7F, ... , 0xF7} */
			if (*(pMIDIMessage + 3) == 0x09 && lLen >= 6) {
				switch (*(pMIDIMessage + 4)) {
				case 0x01: /* GMリセット {0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7} */
					if (*(pMIDIMessage + 5) == 0xF7) {
						return MIDIStatus_PutGMReset (pMIDIStatus);
					}
					break;
				case 0x02: /* GMオフ {0xF0, 0x7E, 0x7F, 0x09, 0x02, 0xF7} */
					if (*(pMIDIMessage + 5) == 0xF7) {
						return MIDIStatus_PutReset (pMIDIStatus);
					}
					break;
				case 0x03: /* GM2リセット {0xF0, 0x7E, 0x7F, 0x09, 0x03, 0xF7}*/
					if (*(pMIDIMessage + 5) == 0xF7) {
						return MIDIStatus_PutGM2Reset (pMIDIStatus);
					}
					break;
				}
			}
			/* スケールオクターブチューニング(資料が無いため確認不能) */
			else if (*(pMIDIMessage + 3) == 0x08 && lLen >= 5) {
				if (*(pMIDIMessage + 4) == 0x08 && lLen >= 21) {				
					unsigned char cCh[16];
					for (j = 5; j <= 19; j++) {
						if (*(pMIDIMessage + j) > 0x7F) {
							return 0;
						}
					}
					if (*(pMIDIMessage + 20) != 0xF7) {
						return 0;
					}
					for (j = 0; j <= 6; j++) {
						cCh[j] = (*(pMIDIMessage + 7) >> j) & 0x01;
					}
					for (j = 7; j <= 13; j++) {
						cCh[j] = (*(pMIDIMessage + 6) >> (j - 7)) & 0x01;
					}
					for (j = 14; j <= 15; j++) {
						cCh[j] = (*(pMIDIMessage + 5) >> (j - 14)) & 0x01;
					}
					for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
						MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
						if (0 <= pMIDIPart->m_lReceiveChannel && pMIDIPart->m_lReceiveChannel < 16) {
							if (cCh[pMIDIPart->m_lReceiveChannel] == 0x01) {
								memcpy (pMIDIPart->m_lScaleOctaveTuning, (pMIDIMessage + 8), 12);
							}
						}
					}
					return 21;
				}
			}
		}

		/* ユニバーサルリアルタイムシステムエクスクルーシブメッセージ */
		else if (*(pMIDIMessage + 1) == 0x7F && lLen >= 4) {
			unsigned char uID = *(pMIDIMessage + 2);
			/* デバイスコントロール */
			if (*(pMIDIMessage + 3) == 0x04 && lLen >= 5) {
				switch (*(pMIDIMessage + 4)) {
				case 0x01: /* マスターボリューム {0xF0, 0x7F, 0xid, 0x04, 0x01, 0xll, 0xmm, 0xF7} */
					if (lLen >= 8) {
						if (0x00 <= *(pMIDIMessage + 5) && *(pMIDIMessage + 5) <= 0x7F &&
							0x00 <= *(pMIDIMessage + 6) && *(pMIDIMessage + 6) <= 0x7F &&
							*(pMIDIMessage + 7) == 0xF7) {
							pMIDIStatus->m_lMasterVolume = *(pMIDIMessage + 6);
							return 8;
						}
					}
					break;
				case 0x02: /* マスターバランス {0xF0, 0x7F, 0xid, 0x04, 0x02, 0xll, 0xmm, 0xF7} */
					if (lLen >= 8) {
						if (0x00 <= *(pMIDIMessage + 5) && *(pMIDIMessage + 5) <= 0x7F &&
							0x00 <= *(pMIDIMessage + 6) && *(pMIDIMessage + 6) <= 0x7F &&
							*(pMIDIMessage + 7) == 0xF7) {
							pMIDIStatus->m_lMasterPan = *(pMIDIMessage + 6);
							return 8;
						}
					}
				case 0x03: /* マスターファインチューンニング {0xF0, 0x7F, 0xid, 0x04, 0x03, 0xll, 0xmm, 0xF7} */
					if (lLen >= 8) {
						if (0x00 <= *(pMIDIMessage + 5) && *(pMIDIMessage + 5) <= 0x7F &&
							0x00 <= *(pMIDIMessage + 6) && *(pMIDIMessage + 6) <= 0x7F &&
							*(pMIDIMessage + 7) == 0xF7) {
							pMIDIStatus->m_lMasterFineTuning = ((long)(*(pMIDIMessage + 6)) << 7) + *(pMIDIMessage + 5);
							return 8;
						}
					}
					break;
				case 0x04: /* マスターコースチューニング {0xF0, 0x7F, 0xid, 0x04, 0x04, 0xll, 0xmm, 0xF7}  */
					if (lLen >= 8) {
						if (0x00 <= *(pMIDIMessage + 5) && *(pMIDIMessage + 5) <= 0x7F &&
							0x00 <= *(pMIDIMessage + 6) && *(pMIDIMessage + 6) <= 0x7F) {
							pMIDIStatus->m_lMasterCoarseTuning = *(pMIDIMessage + 6);
							return 8;
						}
					}
					break;
				case 0x05: /* グローバルパラメーターコントロール {0xF0, 0x7F, 0xid, 0x04, 0x05, ... } */
					if (lLen >= 13) {
						/* マスターリバーブ */
						/* {0xF0, 0x7F, 0xid, 0x04, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0xpp, 0xvv, 0xF7} */
						/* (0xppと0xvvのペアは繰り返されることがある) */
						if (*(pMIDIMessage + 5) == 0x01 && 
							*(pMIDIMessage + 6) == 0x01 && 
							*(pMIDIMessage + 7) == 0x01 &&
							*(pMIDIMessage + 8) == 0x01 && 
							*(pMIDIMessage + 9) == 0x01) {
							unsigned char* p = (pMIDIMessage + 10);
 							while (*p <= 0x7F && *(p + 1) <= 0x7F) {
								switch (*p) {
								case 0x00: /* リバーブタイプ(0xpp == 0x00) */
									MIDIStatus_SetMasterReverbType (pMIDIStatus, (long)(*(p + 1)));
									break;
								case 0x01: /* リバーブタイム(0xpp == 0x01) */
									switch (pMIDIStatus->m_lModuleMode) {
									case MIDISTATUS_MODEGM2:
										pMIDIStatus->m_lMasterReverb[1] = *(p + 1);
										break;
									case MIDISTATUS_MODEGS:
									case MIDISTATUS_MODE88:
										pMIDIStatus->m_lMasterReverb[4] = *(p + 1);
										break;
									case MIDISTATUS_MODEXG:
										pMIDIStatus->m_lMasterReverb[1] = *(p + 1);
										break;
									}
									break;
								}
								p += 2;
								if (p - pMIDIMessage + 2 >= lLen) {
									break;
								}
							}
							return p - pMIDIMessage + 1;
						}
						/* マスターコーラス */
						/* {0xF0, 0x7F, 0xid, 0x04, 0x05, 0x01, 0x01, 0x01, 0x01, 0x02, 0xpp, 0xvv, 0xF7} */
						/* (0xppと0xvvのペアは繰り返されることがある) */
						else if (*(pMIDIMessage + 5) == 0x01 && 
							*(pMIDIMessage + 6) == 0x01 && 
							*(pMIDIMessage + 7) == 0x01 &&
							*(pMIDIMessage + 8) == 0x01 && 
							*(pMIDIMessage + 9) == 0x02) {
							unsigned char* p = (pMIDIMessage + 10);
 							while (*p <= 0x7F  && *(p + 1) <= 0x7F) {
								switch (*p) {
								case 0x00: /* コーラスタイプ(0xpp == 0x00) */
									MIDIStatus_SetMasterChorusType (pMIDIStatus, (long)(*(p + 1)));
									break;
								case 0x01: /* モジュレーションレート(0xpp == 0x01) */
									switch (pMIDIStatus->m_lModuleMode) {
									case MIDISTATUS_MODEGM2:
										pMIDIStatus->m_lMasterChorus[1] = *(p + 1);
										break;
									case MIDISTATUS_MODEGS:
									case MIDISTATUS_MODE88:
										pMIDIStatus->m_lMasterChorus[5] = *(p + 1);
										break;
									case MIDISTATUS_MODEXG:
										pMIDIStatus->m_lMasterChorus[1] = *(p + 1);
										break;
									}
									break;
								case 0x02: /* モジュレーションデプス(0xpp == 0x02) */
									switch (pMIDIStatus->m_lModuleMode) {
									case MIDISTATUS_MODEGM2:
										pMIDIStatus->m_lMasterChorus[2] = *(p + 1);
										break;
									case MIDISTATUS_MODEGS:
									case MIDISTATUS_MODE88:
										pMIDIStatus->m_lMasterChorus[6] = *(p + 1);
										break;
									case MIDISTATUS_MODEXG:
										pMIDIStatus->m_lMasterChorus[2] = *(p + 1);
										break;
									}
									break;
								case 0x03: /* フィードバック(0xpp == 0x03) */
									switch (pMIDIStatus->m_lModuleMode) {
									case MIDISTATUS_MODEGM2:
										pMIDIStatus->m_lMasterChorus[3] = *(p + 1);
										break;
									case MIDISTATUS_MODEGS:
									case MIDISTATUS_MODE88:
										pMIDIStatus->m_lMasterChorus[3] = *(p + 1);
										break;
									case MIDISTATUS_MODEXG:
										pMIDIStatus->m_lMasterChorus[3] = *(p + 1);
										break;
									}
									break;
								case 0x04: /* センドトゥリバーブ(0xpp == 0x04) */
									switch (pMIDIStatus->m_lModuleMode) {
									case MIDISTATUS_MODEGM2:
										pMIDIStatus->m_lMasterChorus[4] = *(p + 1);
										break;
									case MIDISTATUS_MODEGS:
									case MIDISTATUS_MODE88:
										pMIDIStatus->m_lMasterChorus[7] = *(p + 1);
										break;
									case MIDISTATUS_MODEXG:
										pMIDIStatus->m_lMasterChorus[19] = *(p + 1);
										break;
									}
									break;
								}
								p += 2;
								if (p - pMIDIMessage + 2 >= lLen) {
									break;
								}
							}
							return p - pMIDIMessage + 1;
						}
						break;
					}
				}
			}
			/* スケールオクターブチューニング(資料が無いため確認不能) */
			else if (*(pMIDIMessage + 3) == 0x08 && lLen >= 5) {
				if (*(pMIDIMessage + 4) == 0x08 && lLen >= 21) {				
					unsigned char cCh[16];
					for (j = 5; j <= 19; j++) {
						if (*(pMIDIMessage + j) > 0x7F) {
							return 0;
						}
					}
					if (*(pMIDIMessage + 20) != 0xF7) {
						return 0;
					}
					for (j = 0; j <= 6; j++) {
						cCh[j] = (*(pMIDIMessage + 7) >> j) & 0x01;
					}
					for (j = 7; j <= 13; j++) {
						cCh[j] = (*(pMIDIMessage + 6) >> (j - 7)) & 0x01;
					}
					for (j = 14; j <= 15; j++) {
						cCh[j] = (*(pMIDIMessage + 5) >> (j - 14)) & 0x01;
					}
					for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
						MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
						if (0 <= pMIDIPart->m_lReceiveChannel && pMIDIPart->m_lReceiveChannel < 16) {
							if (cCh[pMIDIPart->m_lReceiveChannel] == 0x01) {
								memcpy (pMIDIPart->m_lScaleOctaveTuning, (pMIDIMessage + 8), 12);
							}
						}
					}
					return 21;
				}
			}
			/* キーベースドインストゥルメントコントロール */
			else if (*(pMIDIMessage + 3) == 0x0A && lLen >= 5) {
				if (*(pMIDIMessage + 4) == 0x01 && lLen >= 10) {
					unsigned char cKey = *(pMIDIMessage + 6);
					i = *(pMIDIMessage + 5);
					/* ドラムの鍵盤ごとのボリューム・パンなどの設定 */
					/* {0xF0, 0x7F, 0xid, 0x0A, 0x01, 0xch, 0xke, 0xcc, 0xvv, 0xF7} */
					/* (0xccと0xvvのペアは繰り返されることがある) */
					if (0 <= i && i < pMIDIStatus->m_lNumMIDIPart && 0 <= cKey && cKey <= 127) {
						MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
						if (1 <= pMIDIPart->m_lPartMode && pMIDIPart->m_lPartMode <= pMIDIStatus->m_lNumMIDIDrumSetup) {
							MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, pMIDIPart->m_lPartMode - 1);
							unsigned char* p = (pMIDIMessage + 7);
							while (*p <= 0x7F && *(p + 1) <= 0x7F) {
								switch (*p) {
								case 7: /* Volume */
									pMIDIDrumSetup->m_cDrumVolume[cKey] = *(p + 1);
									break;
								case 10: /* Pan */
									pMIDIDrumSetup->m_cDrumPan[cKey] = *(p + 1);
									break;
								case 91: /* Reverb */
									pMIDIDrumSetup->m_cDrumReverb[cKey] = *(p + 1);
									break;
								case 93: /* Chorus */
									pMIDIDrumSetup->m_cDrumChorus[cKey] = *(p + 1);
									break;
								case 94: /* Delay */
									pMIDIDrumSetup->m_cDrumDelay[cKey] = *(p + 1);
									break;
								}
								p += 2;
								if (p - pMIDIMessage + 2 >= lLen) {
									break;
								}
							}
							return p - pMIDIMessage + 1;
						}
					}
				}
			}
		}
	
		/* Rolandシステムエクスクルーシブメッセージ {0xF0, 0x41, ... , 0xF7} */
		else if (*(pMIDIMessage + 1) == 0x41 && lLen >= 4) {
			/* (ここでは0xidの値は問わないものとする) */
			/* GSシステムエクスクルーシブメッセージ {0xF0, 0x41, 0xid, 0x42, ... , 0xF7} */
			if (*(pMIDIMessage + 3) == 0x42 && lLen >= 5) {
				/* コマンドID=DT1 {0xF0, 0x41, 0xid, 0x42, 0x12, ... , 0xF7} */
				if (*(pMIDIMessage + 4) == 0x12 && lLen >= 11) {
					MIDIPart* pMIDIPart = NULL;
					switch (*(pMIDIMessage + 5)) { /* アドレスバイト上位 */
					case 0x00: /* {0xF0, 0x41, 0xid, 0x42, 0x12, 0x00, ... , 0xF7} */
						switch (*(pMIDIMessage + 6)) { /* アドレスバイト中位 */
						case 0x00: /* {0xF0, 0x41, 0xid, 0x42, 0x12, 0x00, 0x00 ... , 0xF7} */
							switch (*(pMIDIMessage + 7)) { /* アドレスバイト下位 */
							case 0x7F: /* 88Reset {0xF0, 0x41, 0xid, 0x42, 0x12, 0x00, 0x00, 0x7F, 0x00, 0x01, 0xF7} */
								if (*(pMIDIMessage + 8) == 0x00 &&
									*(pMIDIMessage + 9) == 0x01 &&
									*(pMIDIMessage + 10) == 0xF7) {
									MIDIStatus_Put88Reset (pMIDIStatus);
									return 11;
								}
								break;
							}
							break;
						}
						break;
					case 0x40: /* {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, ... , 0xF7} */
						switch (*(pMIDIMessage + 6)) { /* アドレスバイト中位 */
						case 0x00: /* PATCH COMMON PARAMETER  {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x00, ... , 0xF7} */
							switch (*(pMIDIMessage + 7)) { /* アドレスバイト下位 */
							case 0x00: /* MASTER TUNE */
								/* {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x00, 0x00, 0x0v, 0x0w, 0x0x, 0x0y, 0xcs, 0xF7} */
								if (lLen >= 14) {
									if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x0F &&
										0x00 <= *(pMIDIMessage + 9) && *(pMIDIMessage + 9) <= 0x0F &&
										0x00 <= *(pMIDIMessage + 10) && *(pMIDIMessage + 10) <= 0x0F &&
										0x00 <= *(pMIDIMessage + 11) && *(pMIDIMessage + 11) <= 0x0F &&
										*(pMIDIMessage + 12) == 128 - (Sum (pMIDIMessage + 5, 7) % 128) &&
										*(pMIDIMessage + 13) == 0xF7) {
										long lTemp = 
											(*(pMIDIMessage + 8) << 12) |
											(*(pMIDIMessage + 9) << 8) | 
											(*(pMIDIMessage + 10) << 4) | 
											*(pMIDIMessage + 11);
										if (0x0018 <= lTemp && lTemp <= 0x07E8) { 
											pMIDIStatus->m_lMasterFineTuning = lTemp;
											return 14;
										}
									}
								}
								break;
							case 0x04: /* MASTER VOLUME {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x00, 0x04, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterVolume = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x05: /* MASTER KEY SHIFT {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x00, 0x05, 0xvv, 0xcs, 0xF7 } */
								if (0x28 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x58 &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterCoarseTuning = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x06: /* MASTER PAN {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x00, 0x06, 0xvv, 0xcs, 0xF7} */
								if (0x01 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterPan = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x7F: /* GSReset {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41, 0xF7} */
								if (*(pMIDIMessage + 8) == 0x00 &&
									*(pMIDIMessage + 9) == 0x41 &&
									*(pMIDIMessage + 10) == 0xF7) {
									MIDIStatus_PutGSReset (pMIDIStatus);
									return 11;
								}
								break;
							}
							break;
						case 0x01: /* REVERB, CHORUS, DELAY {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, ... , 0xF7} */
							switch (*(pMIDIMessage + 7)) { /* アドレスバイト下位 */
							case 0x30: /* REVERB MACRO {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x30, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x07 &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									MIDIStatus_SetMasterReverbType (pMIDIStatus, *(pMIDIMessage + 8));
									return 11;
								}
								break;
							case 0x31: /* REVERB CHARACTER {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x31, 0xvv, 0xcs, 0xF7} */
							case 0x32: /* REVERB PRE-LPF {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x32, 0xvv, 0xcs, 0xF7} */
							case 0x33: /* REVERB LEVEL {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x33, 0xvv, 0xcs, 0xF7} */
							case 0x34: /* REVERB TIME {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x34, 0xvv, 0xcs, 0xF7} */
							case 0x35: /* REVERB DELAY FEEDBACK {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x35, 0xvv, 0xcs, 0xF7} */
							case 0x37: /* REVERB PRE DELAY TIME [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x37, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterReverb[*(pMIDIMessage + 7) - 0x30] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x38: /* CHORUS MACRO {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x38, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x07 &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									MIDIStatus_SetMasterChorusType (pMIDIStatus, *(pMIDIMessage + 8));
									return 11;
								}
								break;
							case 0x39: /* CHORUS PRE-LPF {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x39, 0xvv, 0xcs, 0xF7} */
							case 0x3A: /* CHORUS LEVEL {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x3A, 0xvv, 0xcs, 0xF7} */
							case 0x3B: /* CHORUS FEEDBACK {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x3B, 0xvv, 0xcs, 0xF7} */
							case 0x3C: /* CHORUS DELAY {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x3C, 0xvv, 0xcs, 0xF7} */
							case 0x3D: /* CHORUS RATE {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x3D, 0xvv, 0xcs, 0xF7} */
							case 0x3E: /* CHORUS DEPTH {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x3E, 0xvv, 0xcs, 0xF7} */
							case 0x3F: /* CHORUS SENDLEVEL TO REVERB {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x3F, 0xvv, 0xcs, 0xF7} */
							case 0x40: /* CHORUS SENDLEVEL TO DELAY [88] {0xF0, 0x41, 0xid, 0x42, 0x01, 0x40, 0x01, 0x40, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterChorus[*(pMIDIMessage + 7) - 0x38] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x50: /* DELAY MACRO [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x50, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x09 &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									MIDIStatus_SetMasterDelayType (pMIDIStatus, *(pMIDIMessage + 8));
									return 11;
								}
								break;
							case 0x51: /* DELAY PRE-LPF [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x51, 0xvv, 0xcs, 0xF7} */
							case 0x52: /* DELAY TIME CENTER [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x52, 0xvv, 0xcs, 0xF7} */
							case 0x53: /* DELAY TIME RATIO LEFT [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x53, 0xvv, 0xcs, 0xF7} */
							case 0x54: /* DELAY TIME RATIO RIGHT [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x54, 0xvv, 0xcs, 0xF7} */
							case 0x55: /* DELAY LEVEL CENTER [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x55, 0xvv, 0xcs, 0xF7} */
							case 0x56: /* DELAY LEVEL LEFT [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x56, 0xvv, 0xcs, 0xF7} */
							case 0x57: /* DELAY LEVEL RIGHT {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x57, 0xvv, 0xcs, 0xF7} */
							case 0x58: /* DELAY LEVEL [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x58, 0xvv, 0xcs, 0xF7} */
							case 0x59: /* DELAY FEEDBACK [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x59, 0xvv, 0xcs, 0xF7} */
							case 0x5A: /* DELAY SENDLEVEL TO REVERB [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x01, 0x5A, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterDelay[*(pMIDIMessage + 7) - 0x50] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							}
							break;
						case 0x02: /* EQUALIZER {0xF7, 0x41, 0xid, 0x42, 0x12, 0x02, ... , 0xF7} */
							switch (*(pMIDIMessage + 7)) { /* アドレスバイト下位 */
							case 0x00: /* EQ LOW FREQ [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x02, 0x00, 0xvv, 0xcs, 0xF7} */
							case 0x02: /* EQ HIGH FREQ [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x02, 0x02, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x01 &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterEqualizer[*(pMIDIMessage + 7) + 1] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x01: /* EQ LOW GAIN [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x02, 0x01, 0xvv, 0xcs, 0xF7} */
							case 0x03: /* EQ HIGH GAIN [88] {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x02, 0x03, 0xvv, 0xcs, 0xF7} */
								if (0x34 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x4C &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIStatus->m_lMasterEqualizer[*(pMIDIMessage + 7) + 1] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							}
							break;
						case 0x10: /* PART10 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x10, ... , 0xF7} */
						case 0x11: /* PART01 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x11, ... , 0xF7} */
						case 0x12: /* PART02 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x12, ... , 0xF7} */
						case 0x13: /* PART03 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x13, ... , 0xF7} */
						case 0x14: /* PART04 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x14, ... , 0xF7} */
						case 0x15: /* PART05 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x15, ... , 0xF7} */
						case 0x16: /* PART06 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x16, ... , 0xF7} */
						case 0x17: /* PART07 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x17, ... , 0xF7} */
						case 0x18: /* PART08 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x18, ... , 0xF7} */
						case 0x19: /* PART09 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x19, ... , 0xF7} */
						case 0x1A: /* PART11 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1A, ... , 0xF7} */
						case 0x1B: /* PART12 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1B, ... , 0xF7} */
						case 0x1C: /* PART13 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1C, ... , 0xF7} */
						case 0x1D: /* PART14 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1D, ... , 0xF7} */
						case 0x1E: /* PART15 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1E, ... , 0xF7} */
						case 0x1F: /* PART16 {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1F, ... , 0xF7} */
							if (*(pMIDIMessage + 6) == 0x10) {
								i = 9;
							}
							else if (0x11 <= *(pMIDIMessage + 6) && *(pMIDIMessage + 6) <= 0x19) {
								i = *(pMIDIMessage + 6) - 0x11;
							}
							else {
								i = *(pMIDIMessage + 6) - 0x10;
							}
							if (0 < i && i < pMIDIStatus->m_lNumMIDIPart) {
								pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
							}
							else {
								break;
							}
							switch (*(pMIDIMessage + 7)) { /* アドレスバイト下位 */
							case 0x02: /* Rx.CHANNEL {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x02, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x10 &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_lReceiveChannel = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x15: /* USE FOR RHYTHM PART {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x15, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x0F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_lPartMode = *(pMIDIMessage + 8);
									return 11;
								}
								break;		
							case 0x16: /* PITCH KEY SHIFT {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x16, 0xvv, 0xcs, 0xF7} */
								if (0x28 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x58 &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_lChannelCoarseTuning = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x19: /* PART LEVEL (=CC#7) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x19, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[7] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x1A: /* VELOCITY SENSE DEPTH {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x1A, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_lVelocitySenseDepth = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x1B: /* VELOCITY SENSE OFFSET {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x1B, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_lVelocitySenseOffset = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x1C: /* PART PANPOT (CC=#10) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x1C, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[10] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x1D: /* KEYBOARD RANGE LOW {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x1D, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_lKeyboardRangeLow = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x1E: /* KEYBOARD RANGE HIGH {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x1E, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_lKeyboardRangeHigh = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x21: /* CHORUS SEND LEVEL (=CC#93) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x21, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[93] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x22: /* REVERB SEND LEVEL (=CC#91) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x22, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[91] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x2A: /* PITCH FINE TUNE {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x2A, 0xvv, 0xww, 0xcs, 0xF7} */
								if (lLen >= 12) {
									if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
										0x00 <= *(pMIDIMessage + 9) && *(pMIDIMessage + 9) <= 0x7F &&
										*(pMIDIMessage + 10) == 128 - (Sum (pMIDIMessage + 5, 5) % 128) &&
										*(pMIDIMessage + 11) == 0xF7) {
										long lTemp = (*(pMIDIMessage + 8) << 7) | *(pMIDIMessage + 9);
										pMIDIPart->m_lChannelFineTuning = lTemp;
#ifdef MIDISTATUS_RECORDRPN
										pMIDIPart->m_cRPNMSB[0][1] = *(pMIDIMessage + 8);
										pMIDIPart->m_cRPNLSB[0][1] = *(pMIDIMessage + 9);
#endif
										return 12;
									}
								}
								break;
							case 0x2C: /* DELAY SEND LEVEL (=CC#94) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x2C, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[94] = *(pMIDIMessage + 8);
									return 11;
								}
								break;
							case 0x30: /* Vibrato Rate (=NRPN#8=CC#76) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x30, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[76] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][8] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x31: /* Vibrato Depth (=NRPN#9=CC#77) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x31, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[77] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][9] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x32: /* Cutoff Freq (=NRPN#32=CC#74) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x32, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[74] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][32] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x33: /* Resonance (=NRPN#33=CC#71) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x33, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[71] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][33] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x34: /* Attack Time (=NRPN#99=CC#73) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x34, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[73] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][99] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x35: /* Decay Time (=NRPN#100=CC#75) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x35, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[75] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][100] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x36: /* Release Time (=NRPN#102=CC#72) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x36, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[72] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][102] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x37: /* Vibrato Delay (=NRPN#10=CC#78) {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x37, 0xvv, 0xcs, 0xF7} */
								if (0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
									*(pMIDIMessage + 9) == 128 - (Sum (pMIDIMessage + 5, 4) % 128) &&
									*(pMIDIMessage + 10) == 0xF7) {
									pMIDIPart->m_cControlChange[78] = *(pMIDIMessage + 8);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][10] = *(pMIDIMessage + 8);
#endif
									return 11;
								}
								break;
							case 0x40: /* Scale Tuning {0xF0, 0x41, 0xid, 0x42, 0x12, 0x40, 0x1X, 0x40, ...(12byte)... , 0xcs, 0xF7} */
								if (lLen >= 22) {
									for (j = 8; j <= 19; j++) {
										if (*(pMIDIMessage + j) > 0x7F) {
											return 0;
										}
									}
									if (*(pMIDIMessage + 20) == 128 - (Sum (pMIDIMessage + 5, 15) % 128) &&
										*(pMIDIMessage + 21) == 0xF7) {
										memcpy (pMIDIPart->m_lScaleOctaveTuning,
											(pMIDIMessage + 8), 12);
										return 22;
									}
								}
								break;
							}
							break;
						}
						break;
					}
				}
			}
		}

		/* YAMAHAシステムエクスクルーシブメッセージ */
		else if (*(pMIDIMessage + 1) == 0x43 && lLen >= 3) {
			/* コマンドID=パラメーターチェンジ */
			if ((*(pMIDIMessage + 2) & 0xF0) == 0x10 && lLen >= 4) {
				/* 非XGシステムエクスクルーシブメッセージ */
				if (*(pMIDIMessage + 3) == 0x27) {
					/* MIDIマスターチューニング */
				
				}
				/* XGシステムエクスクルーシブメッセージ */
				else if (*(pMIDIMessage + 3) == 0x4C && lLen >= 7) {
					MIDIPart* pMIDIPart = NULL;
					switch (*(pMIDIMessage + 4)) { /* アドレスバイト上位 */
					case 0x00: /* XG SYSTEM {0xF0, 0x43, 0x1d, 0x4C, 0x00, ... , 0xF7} */
						switch (*(pMIDIMessage + 5)) { /* アドレスバイト中位 */
						case 0x00: /* {0xF0, 0x43, 0x1d, 0x4C, 0x00, 0x00, ... , 0xF7} */
							switch (*(pMIDIMessage + 6)) { /* アドレスバイト下位 */
							case 0x00: /* MASTER TUNE {0xF0, 0x43, 0x1d, 0x4C, 0x00, 0x00, 0x00, 0xvv, 0xF7} */
								if (lLen >= 11) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x0F &&
										0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x0F &&
										0x00 <= *(pMIDIMessage + 9) && *(pMIDIMessage + 9) <= 0x0F &&
										0x00 <= *(pMIDIMessage + 10) && *(pMIDIMessage + 10) <= 0x0F &&
										*(pMIDIMessage + 11) == 0xF7) {
										long lTemp = 
											(*(pMIDIMessage + 7) << 12) |
											(*(pMIDIMessage + 8) << 8) | 
											(*(pMIDIMessage + 9) << 4) | 
											*(pMIDIMessage + 10);
										if (0x0000 <= lTemp && lTemp <= 0x07FF) { 
											pMIDIStatus->m_lMasterFineTuning = lTemp;
										}
										return 11;
									}
								}
								break;
							case 0x04: /* MASTER VOLUME {0xF0, 0x43, 0x1d, 0x4C, 0x00, 0x00, 0x04, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterVolume = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x06: /* TRANSPOSE {0xF0, 0x43, 0x1d, 0x4C, 0x00, 0x00, 0x06, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x28 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x58 &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterCoarseTuning = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x7E: /* XG RESET {0xF0, 0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00, 0xF7} */
								if (lLen >= 9) {
									if (memcmp (pMIDIMessage, g_byXGReset, 9) == 0) {
										MIDIStatus_PutXGReset (pMIDIStatus);
										return 9;
									}
								}
								break;
							}
							break;
						}
						break;
					case 0x02: /* XG EFFECT1 */
						switch (*(pMIDIMessage + 5)) { /* アドレスバイト中位 */
						case 0x01: /* REVERB, CHORUS, VARIATION {0xF7, 0x43, 0x1d, 0x4C, 0x02, 0x01, ... , 0xF7} */
							switch (*(pMIDIMessage + 6)) { /* アドレスバイト下位 */
							case 0x00: /* REVERB TYPE {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x00, 0xmm, 0xll, 0xF7} */
								if (lLen >= 10) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
										*(pMIDIMessage + 9) == 0xF7) {
										long lTemp = (*(pMIDIMessage + 7) << 7) | *(pMIDIMessage + 8);
										MIDIStatus_SetMasterReverbType (pMIDIStatus, lTemp);
										return 10;
									}
								}
								break;
							case 0x02: /* REVERB PRARMETER 1 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x02, 0xvv, 0xF7} */
							case 0x03: /* REVERB PRARMETER 2 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x03, 0xvv, 0xF7} */
							case 0x04: /* REVERB PRARMETER 3 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x04, 0xvv, 0xF7} */
							case 0x05: /* REVERB PRARMETER 4 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x05, 0xvv, 0xF7} */
							case 0x06: /* REVERB PRARMETER 5 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x06, 0xvv, 0xF7} */
							case 0x07: /* REVERB PRARMETER 6 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x07, 0xvv, 0xF7} */
							case 0x08: /* REVERB PRARMETER 7 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x08, 0xvv, 0xF7} */
							case 0x09: /* REVERB PRARMETER 8 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x09, 0xvv, 0xF7} */
							case 0x0A: /* REVERB PRARMETER 9 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x0A, 0xvv, 0xF7} */
							case 0x0B: /* REVERB PRARMETER 10 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x0B, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterReverb[*(pMIDIMessage + 6) - 1] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x0C: /* REVERB RETURN {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x0C, 0xvv, 0xF7} */
							case 0x0D: /* REVERB PAN {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x0D, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterReverb[*(pMIDIMessage + 6) + 5] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x10: /* REVERB PRARMETER 11 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x10, 0xvv, 0xF7} */
							case 0x11: /* REVERB PRARMETER 12 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x11, 0xvv, 0xF7} */
							case 0x12: /* REVERB PRARMETER 13 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x12, 0xvv, 0xF7} */
							case 0x13: /* REVERB PRARMETER 14 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x13, 0xvv, 0xF7} */
							case 0x14: /* REVERB PRARMETER 15 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x14, 0xvv, 0xF7} */
							case 0x15: /* REVERB PRARMETER 16 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x15, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterReverb[*(pMIDIMessage + 6) - 5] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;

							case 0x20: /* CHORUS TYPE {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x20, 0xmm, 0xll, 0xF7} */
								if (lLen >= 10) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
										*(pMIDIMessage + 9) == 0xF7) {
										long lTemp = (*(pMIDIMessage + 7) << 7) | *(pMIDIMessage + 8);
										MIDIStatus_SetMasterChorusType (pMIDIStatus, lTemp);
										return 10;
									}
								}
								break;
							case 0x22: /* CHORUS PRARMETER 1 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x22, 0xvv, 0xF7} */
							case 0x23: /* CHORUS PRARMETER 2 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x23, 0xvv, 0xF7} */
							case 0x24: /* CHORUS PRARMETER 3 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x24, 0xvv, 0xF7} */
							case 0x25: /* CHORUS PRARMETER 4 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x25, 0xvv, 0xF7} */
							case 0x26: /* CHORUS PRARMETER 5 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x26, 0xvv, 0xF7} */
							case 0x27: /* CHORUS PRARMETER 6 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x27, 0xvv, 0xF7} */
							case 0x28: /* CHORUS PRARMETER 7 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x28, 0xvv, 0xF7} */
							case 0x29: /* CHORUS PRARMETER 8 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x29, 0xvv, 0xF7} */
							case 0x2A: /* CHORUS PRARMETER 9 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x2A, 0xvv, 0xF7} */
							case 0x2B: /* CHORUS PRARMETER 10 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x2B, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterChorus[*(pMIDIMessage + 6) - 33] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x2C: /* CHORUS RETURN {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x2C, 0xvv, 0xF7} */
							case 0x2D: /* CHORUS PAN {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x2D, 0xvv, 0xF7} */
							case 0x2E: /* SEND CHORUS TO REVERB {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x2E, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterChorus[*(pMIDIMessage + 6) - 27] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x30: /* CHORUS PRARMETER 11 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x30, 0xvv, 0xF7} */
							case 0x31: /* CHORUS PRARMETER 12 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x31, 0xvv, 0xF7} */
							case 0x32: /* CHORUS PRARMETER 13 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x32, 0xvv, 0xF7} */
							case 0x33: /* CHORUS PRARMETER 14 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x33, 0xvv, 0xF7} */
							case 0x34: /* CHORUS PRARMETER 15 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x34, 0xvv, 0xF7} */
							case 0x35: /* CHORUS PRARMETER 16 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x35, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterChorus[*(pMIDIMessage + 6) - 37] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;

							case 0x40: /* VARIATION TYPE {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x40, 0xmm, 0xll, 0xF7} */
								if (lLen >= 10) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
										*(pMIDIMessage + 9) == 0xF7) {
										long lTemp = (*(pMIDIMessage + 7) << 7) | *(pMIDIMessage + 8);
										MIDIStatus_SetMasterDelayType (pMIDIStatus, lTemp);
										return 10;
									}
								}
								break;
							case 0x42: /* VARIATION PRARMETER 1 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x42, 0xmm, 0xll, 0xF7} */
							case 0x44: /* VARIATION PRARMETER 2 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x44, 0xmm, 0xll, 0xF7} */
							case 0x46: /* VARIATION PRARMETER 3 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x46, 0xmm, 0xll, 0xF7} */
							case 0x48: /* VARIATION PRARMETER 4 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x48, 0xmm, 0xll, 0xF7} */
							case 0x4A: /* VARIATION PRARMETER 5 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x4A, 0xmm, 0xll, 0xF7} */
							case 0x4C: /* VARIATION PRARMETER 6 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x4C, 0xmm, 0xll, 0xF7} */
							case 0x4E: /* VARIATION PRARMETER 7 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x4E, 0xmm, 0xll, 0xF7} */
							case 0x50: /* VARIATION PRARMETER 8 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x50, 0xmm, 0xll, 0xF7} */
							case 0x52: /* VARIATION PRARMETER 9 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x52, 0xmm, 0xll, 0xF7} */
							case 0x54: /* VARIATION PRARMETER 10 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x54, 0xmm, 0xll, 0xF7} */
								if (lLen >= 10) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										0x00 <= *(pMIDIMessage + 8) && *(pMIDIMessage + 8) <= 0x7F &&
										*(pMIDIMessage + 9) == 0xF7) {
										long lTemp = (*(pMIDIMessage + 7) << 7) | *(pMIDIMessage + 8);
										pMIDIStatus->m_lMasterDelay[*(pMIDIMessage + 6) / 2 - 32] = lTemp;
										return 10;
									}
								}
								break;
							case 0x56: /* VARIATION RETURN {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x56, 0xvv, 0xF7} */
							case 0x57: /* VARIATION PAN {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x57, 0xvv, 0xF7} */
							case 0x58: /* SEND VARIATION TO REVERB {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x58, 0xvv, 0xF7} */
							case 0x59: /* SEND VARIATION TO CHORUS {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x59, 0xvv, 0xF7} */
							case 0x61: /* VARIATION PART {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x5B, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterDelay[*(pMIDIMessage + 6) - 69] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x60: /* VARIATION CONNECTION {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x5A, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x01 &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterDelay[*(pMIDIMessage + 6) - 69] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x70: /* VARIATION PRARMETER 11 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x70, 0xvv, 0xF7} */
							case 0x71: /* VARIATION PRARMETER 12 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x71, 0xvv, 0xF7} */
							case 0x72: /* VARIATION PRARMETER 13 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x72, 0xvv, 0xF7} */
							case 0x73: /* VARIATION PRARMETER 14 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x73, 0xvv, 0xF7} */
							case 0x74: /* VARIATION PRARMETER 15 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x74, 0xvv, 0xF7} */
							case 0x75: /* VARIATION PRARMETER 16 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x01, 0x75, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterDelay[*(pMIDIMessage + 6) - 101] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							}
							break;

						case 0x40: /* XG EQUALIZER {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, ... , 0xF7} */
							switch (*(pMIDIMessage + 6)) { /* アドレスバイト下位 */
							case 0x00: /* EQUALIZER TYPE {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x00, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x04 &&
										*(pMIDIMessage + 8) == 0xF7) {
										MIDIStatus_SetMasterEqualizerType (pMIDIStatus, *(pMIDIMessage + 7));
										return 9;
									}
								}
								break;
							case 0x01: /* EQ gain1 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x01, 0xvv, 0xF7} */
							case 0x05: /* EQ gain2 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x05, 0xvv, 0xF7} */
							case 0x09: /* EQ gain3 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x09, 0xvv, 0xF7} */
							case 0x0D: /* EQ gain4 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x0D, 0xvv, 0xF7} */
							case 0x11: /* EQ gain5 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x11, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x34 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x4C &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterEqualizer[*(pMIDIMessage + 6) - 1] = *(pMIDIMessage + 7);
										return 9;	
									}
								}
								break;
							case 0x02: /* EQ frequency1 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x02, 0xvv, 0xF7} */
							case 0x06: /* EQ frequency2 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x06, 0xvv, 0xF7} */
							case 0x0A: /* EQ frequency3 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x0A, 0xvv, 0xF7} */
							case 0x0E: /* EQ frequency4 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x0E, 0xvv, 0xF7} */
							case 0x12: /* EQ frequency5 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x12, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x04 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x3A &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterEqualizer[*(pMIDIMessage + 6) - 1] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x03: /* EQ Q1 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x03, 0xvv, 0xF7} */
							case 0x07: /* EQ Q2 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x07, 0xvv, 0xF7} */
							case 0x0B: /* EQ Q3 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x0B, 0xvv, 0xF7} */
							case 0x0F: /* EQ Q4 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x0F, 0xvv, 0xF7} */
							case 0x13: /* EQ Q5 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x13, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x01 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x78 &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterEqualizer[*(pMIDIMessage + 6) - 1] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							case 0x04: /* EQ shape1 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x04, 0xvv, 0xF7} */
							case 0x08: /* EQ shape2 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x08, 0xvv, 0xF7} */
							case 0x0C: /* EQ shape3 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x0C, 0xvv, 0xF7} */
							case 0x10: /* EQ shape4 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x10, 0xvv, 0xF7} */
							case 0x14: /* EQ shape5 {0xF0, 0x43, 0x1d, 0x4C, 0x02, 0x40, 0x14, 0xvv, 0xF7} */
								if (lLen >= 9) {
									if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x01 &&
										*(pMIDIMessage + 8) == 0xF7) {
										pMIDIStatus->m_lMasterEqualizer[*(pMIDIMessage + 6) - 1] = *(pMIDIMessage + 7);
										return 9;
									}
								}
								break;
							}
							break;
						}
						break;
					case 0x08: /* XG MULTI PART {0xF0, 0x43, 0x1d, 0x4C, 0x08, ... , 0xF7} */
						i = *(pMIDIMessage + 5) & 0x0F; /* アドレスバイト中位 */
						if (0 <= i && i < pMIDIStatus->m_lNumMIDIPart) {
							pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
						}
						else {
							break;
						}
						switch (*(pMIDIMessage + 6)) { /* アドレスバイト下位 */
						case 0x01: /* BANK SELECT MSB (=CC#0) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x01, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[0] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x02: /* BANK SELECT LSB (=CC#32) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x02, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[32] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x03: /* PROGRAM NUMBER (=PC) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x03, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lProgramChange = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x04: /* Rcv CHANNEL {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x04, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x10 &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lReceiveChannel = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x07: /* PART MODE {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x07, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x10 &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lPartMode = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x08: /* NOTE SHIFT {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x08, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x28 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x58 &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lChannelCoarseTuning = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x0B: /* VOLUME (CC#7) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x0B, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[7] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x0C: /* VELOCITY SENSE DEPTH {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x0C, 0xvv, 0xF0} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lVelocitySenseDepth = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x0D: /* VELOCITY SENSE OFFSET {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x0D, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lVelocitySenseOffset = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x0E: /* PAN (=CC#10) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x0E, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[10] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x0F: /* NOTE LIMIT LOW {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x0F, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lKeyboardRangeLow = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x10: /* NOTE LIMIT HIGH {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x10, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lKeyboardRangeHigh = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x12: /* CHORUS SEND (=CC#93) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x12, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[93] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x13: /* REVERB SEND (=CC#91) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x13, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[91] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x14: /* VARIATION SEND (=CC#94) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x14, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[94] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						case 0x15: /* VIBRATO RATE (=NRPN#8=CC#76) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x15, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[76] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][8] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x16: /* VIBRATO DEPTH (=NRPN#9=CC#77) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x16, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[77] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][9] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x17: /* VIBRATO DELAY (=NRPN#10=CC#78) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x17, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[78] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][10] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x18: /* FILTER CUTOFF FREQ (=NRPN#32=CC#74) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x18, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[74] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][32] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x19: /* FILTER RESONANCE (=NRPN#33=CC#71) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x19, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[71] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][33] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x1A: /* EG ATTACK TIME (=NRPN#99=CC#73) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x1A, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[73] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][99] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x1B: /* EG DECAY TIME (=NRPN#100=CC#75) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x1B, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[75] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][100] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x1C: /* EG RELEASE TIME (=NRPN#102=CC#72) {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x1C, 0xvv, 0xF7} */
							if (lLen >= 9) {
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_cControlChange[72] = *(pMIDIMessage + 7);
#ifdef MIDISTATUS_RECORDNRPN
									pMIDIPart->m_cNRPNMSB[1][102] = *(pMIDIMessage + 7);
#endif
									return 9;
								}
							}
							break;
						case 0x41: /* SCALE TUNING C  {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x41, 0xvv, 0xF7} */
						case 0x42: /* SCALE TUNING C# {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x42, 0xvv, 0xF7} */
						case 0x43: /* SCALE TUNING D  {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x43, 0xvv, 0xF7} */
						case 0x44: /* SCALE TUNING D# {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x44, 0xvv, 0xF7} */
						case 0x45: /* SCALE TUNING E  {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x45, 0xvv, 0xF7} */
						case 0x46: /* SCALE TUNING F  {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x46, 0xvv, 0xF7} */
						case 0x47: /* SCALE TUNING F# {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x47, 0xvv, 0xF7} */
						case 0x48: /* SCALE TUNING G  {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x48, 0xvv, 0xF7} */
						case 0x49: /* SCALE TUNING G# {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x49, 0xvv, 0xF7} */
						case 0x4A: /* SCALE TUNING A  {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x4A, 0xvv, 0xF7} */
						case 0x4B: /* SCALE TUNING A# {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x4B, 0xvv, 0xF7} */
						case 0x4C: /* SCALE TUNING B  {0xF0, 0x43, 0x1d, 0x4C, 0x08, 0xnn, 0x4C, 0xvv, 0xF7} */
							if (lLen >= 9) {
								unsigned char cKey = *(pMIDIMessage + 6) - 0x41;
								if (0x00 <= *(pMIDIMessage + 7) && *(pMIDIMessage + 7) <= 0x7F &&
									*(pMIDIMessage + 8) == 0xF7) {
									pMIDIPart->m_lScaleOctaveTuning[cKey] = *(pMIDIMessage + 7);
									return 9;
								}
							}
							break;
						}
						break;
					}
				}
			}
		}
	}
	return 0;
}

/* MIDIStatusの書き込み(外部隠蔽、この関数はMIDIStatus_Saveから呼び出される) */
long __stdcall MIDIStatus_Write (MIDIStatus* pMIDIStatus, FILE* pFile, long lVersion) {
	int i;
	assert (pMIDIStatus);
	assert (pFile);
	assert (0 <= lVersion && lVersion <= 9999);
	if (fwrite (&(pMIDIStatus->m_lModuleMode), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterFineTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterCoarseTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterBalance), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterVolume), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterPan), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterReverb[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterChorus[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterDelay[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterEqualizer[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lMasterInsertion[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lNumMIDIPart), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lNumMIDIDrumSetup), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	/* 各MIDIPartの書き込み */
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
		MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
		if (MIDIPart_Write (pMIDIPart, pFile, lVersion) == 0) {
			return 0;
		}
	}
	/* 各MIDIDrumSetupの書き込み */
	for (i = 0; i < pMIDIStatus->m_lNumMIDIDrumSetup; i++) {
		MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, i);
		if (MIDIDrumSetup_Write (pMIDIDrumSetup, pFile, lVersion) == 0) {
			return 0;
		}
	}
	/* ユーザー用拡張領域 */
	if (fwrite (&(pMIDIStatus->m_lUser1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lUser2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lUser3), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fwrite (&(pMIDIStatus->m_lUserFlag), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	return 1;
}

/* MIDIStatusの保存(ANSI) */
long __stdcall MIDIStatus_SaveA (MIDIStatus* pMIDIStatus, const char* pszFileName) {
	long lVersion = 1;
	char szText[256];
	FILE* pFile = NULL;
	/* ファイルを開く(書き込み用バイナリ) */
	pFile = fopen (pszFileName, "wb");
	if (pFile == NULL) {
		return 0;
	}
	/* ファイル種類とヴァージョン情報の書き込み */
	memset (szText, 0, sizeof(szText));
	sprintf (szText, "MIDIStatusVer%04d", lVersion);
	if (fwrite (szText, sizeof(char), 17, pFile) != 17) {
		fclose (pFile);
		return 0;
	}
	/* MIDIStatus内のデータ書き込み */
	if (MIDIStatus_Write (pMIDIStatus, pFile, lVersion) == 0) {
		fclose (pFile);
		return 0;
	}
	/* ファイルを閉じる */
	fclose (pFile);
	return 1;
}

/* MIDIStatusの保存(UNICODE) */
long __stdcall MIDIStatus_SaveW (MIDIStatus* pMIDIStatus, const wchar_t* pszFileName) {
	long lVersion = 1;
	char szText[256];
	FILE* pFile = NULL;
	/* ファイルを開く(書き込み用バイナリ) */
	pFile = _wfopen (pszFileName, L"wb");
	if (pFile == NULL) {
		return 0;
	}
	/* ファイル種類とヴァージョン情報の書き込み */
	memset (szText, 0, sizeof(szText));
	sprintf (szText, "MIDIStatusVer%04d", lVersion);
	if (fwrite (szText, sizeof(char), 17, pFile) != 17) {
		fclose (pFile);
		return 0;
	}
	/* MIDIStatus内のデータ書き込み */
	if (MIDIStatus_Write (pMIDIStatus, pFile, lVersion) == 0) {
		fclose (pFile);
		return 0;
	}
	/* ファイルを閉じる */
	fclose (pFile);
	return 1;
}

/* MIDIStatusの読み込み(外部隠蔽、この関数はMIDIStatus_Loadから呼び出される) */
long __stdcall MIDIStatus_Read (MIDIStatus* pMIDIStatus, FILE* pFile, long lVersion) {
	int i;
	assert (pMIDIStatus);
	assert (pFile);
	assert (0 <= lVersion && lVersion <= 9999);
	if (fread (&(pMIDIStatus->m_lModuleMode), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterFineTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterCoarseTuning), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterBalance), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterVolume), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterPan), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterReverb[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterChorus[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterDelay[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterEqualizer[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lMasterInsertion[0]), sizeof(long), 32, pFile) != 32) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lNumMIDIPart), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lNumMIDIDrumSetup), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	/* MIDIPartを必要個数生成 */
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
		pMIDIStatus->m_pMIDIPart[i] = MIDIPart_Create (pMIDIStatus);
		if (pMIDIStatus->m_pMIDIPart[i] == NULL) {
			return 0;
		}
	}
	/* MIDIDrumSetupを必要個数生成 */
	for (i = 0; i < pMIDIStatus->m_lNumMIDIDrumSetup; i++) {
		pMIDIStatus->m_pMIDIDrumSetup[i] = MIDIDrumSetup_Create (pMIDIStatus);
		if (pMIDIStatus->m_pMIDIDrumSetup[i] == NULL) {
			return 0;
		}
	}
	/* 各MIDIPartの読み込み */
	for (i = 0; i < pMIDIStatus->m_lNumMIDIPart; i++) {
		MIDIPart* pMIDIPart = MIDIStatus_GetMIDIPart (pMIDIStatus, i);
		if (MIDIPart_Read (pMIDIPart, pFile, lVersion) == 0) {
			return 0;
		}
	}
	/* 各MIDIDrumSetupの読み込み */
	for (i = 0; i < pMIDIStatus->m_lNumMIDIDrumSetup; i++) {
		MIDIDrumSetup* pMIDIDrumSetup = MIDIStatus_GetMIDIDrumSetup (pMIDIStatus, i);
		if (MIDIDrumSetup_Read (pMIDIDrumSetup, pFile, lVersion) == 0) {
			return 0;
		}
	}
	/* ユーザー用拡張領域 */
	if (fread (&(pMIDIStatus->m_lUser1), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lUser2), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lUser3), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	if (fread (&(pMIDIStatus->m_lUserFlag), sizeof(long), 1, pFile) != 1) {
		return 0;
	}
	return 1;
}

/* MIDIStatusのロード(ANSI) */
MIDIStatus* __stdcall MIDIStatus_LoadA (const char* pszFileName) {
	long lVersion = 0;
	char szText[256];
	FILE* pFile = NULL;
	MIDIStatus* pMIDIStatus = NULL;
	/* MIDIStatusオブジェクトの作成 */	
	pMIDIStatus = MIDIStatus_Create (MIDISTATUS_MODENATIVE, 0, 0);
	if (pMIDIStatus == NULL) {
		return NULL;
	}
	/* ファイルを開く(読み込み用バイナリ) */
	pFile = fopen (pszFileName, "rb");
	if (pFile == NULL) {
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	/* ファイル種類とヴァージョン情報の読み込み */
	memset (szText, 0, sizeof(szText));
	if (fread (szText, sizeof(char), 17, pFile) != 17) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	if (memcmp (szText, "MIDIStatusVer", 13) != 0) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	lVersion = atol (&szText[13]);
	if (lVersion < 0 || lVersion >= 10000) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	/* MIDIStatus内のデータ読み込み */
	if (MIDIStatus_Read (pMIDIStatus, pFile, lVersion) == 0) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	/* ファイルを閉じる */
	fclose (pFile);
	return pMIDIStatus;
}
	

/* MIDIStatusのロード(UNICODE) */
MIDIStatus* __stdcall MIDIStatus_LoadW (const wchar_t* pszFileName) {
	long lVersion = 0;
	char szText[256];
	FILE* pFile = NULL;
	MIDIStatus* pMIDIStatus = NULL;
	/* MIDIStatusオブジェクトの作成 */	
	pMIDIStatus = MIDIStatus_Create (MIDISTATUS_MODENATIVE, 0, 0);
	if (pMIDIStatus == NULL) {
		return NULL;
	}
	/* ファイルを開く(読み込み用バイナリ) */
	pFile = _wfopen (pszFileName, L"rb");
	if (pFile == NULL) {
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	/* ファイル種類とヴァージョン情報の読み込み */
	memset (szText, 0, sizeof(szText));
	if (fread (szText, sizeof(char), 17, pFile) != 17) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	if (memcmp (szText, "MIDIStatusVer", 13) != 0) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	lVersion = atol (&szText[13]);
	if (lVersion < 0 || lVersion >= 10000) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	/* MIDIStatus内のデータ読み込み */
	if (MIDIStatus_Read (pMIDIStatus, pFile, lVersion) == 0) {
		fclose (pFile);
		MIDIStatus_Delete (pMIDIStatus);
		pMIDIStatus = NULL;
		return NULL;
	}
	/* ファイルを閉じる */
	fclose (pFile);
	return pMIDIStatus;
}
	

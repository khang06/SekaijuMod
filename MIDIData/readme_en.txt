/******************************************************************************/
/*                                                                            */
/*  MIDIDataLibrary readme.txt                             (C)2002-2020 kuzu  */
/*                                                                            */
/******************************************************************************/

Thank you for downloading MIDIDataLibrary. This file includes basic information 
and basic notice. Please read this file first. For detail, please see 
"MIDIData_en.html" in the "docs" foleder.

### Features ###

MIDIDataLibrary is free, open source library for creating and editing MIDI data. 
It enables you to create MIDI data, load / save Sekaiju sequence file (*.skj), 
load Early Cakewalk sequence file (*.wrk), load / save Cherry sequence file (*.chy), 
load / save standard MIDI file (*.mid), load / save MIDICSV file (*.csv), 
and create, delete, modify MIDI tracks, and create, delete, modify MIDI events 
including meta event and system exclusive event. This library provides essential 
functions to edit each element of the MIDI data.
 
For time base, both TPQN base (resolution from 1 to 32767 [ticks / quarter note])
 and SMPTE base (24, 25, 29.97, 30 [frames / sec]) are available. For SMF format, 
format0, format1, and format2 are available. The time base and SMF format can be 
converted at any time.

This library has functions to convert time code to [millisec] or convert [millisec]
 to time code by analyzing tempo events. This library also has functions to convert
time code to [measure:beat:tick] or convert [measure:beat:tick] to time code by 
analyzing time signature events. Also, this library has functions to get or set 
title, copyright, comment, start time, end time and so on.</P>

### Contents ###

MIDIData
|-readme.txt        Read me first (Japanese)
|-readme_en.txt     Read me first (English)
|-license.txt       license
|-MIDIData.c        C source file
|-MIDIData.h        C/C++ header file
|-MIDIData.def      C/C++ module definition file
|-MIDIData.mak      C/C++ make file
|-MIDIData.sln      Visual C++ 2008 Service Pack 1 Solution File
|-MIDIData.vcproj   Visual C++ 2008 Service Pack 1 Project work space
|-MIDIData.bas      Visual Basic 4.0/5.0/6.0 import module
|-Debug
| |-MIDIDatad.lib   Object library module (for debug)
| !-MIDIDatad.dll   Dynamic link library (for debug)
|-Release
| |-MIDIData.lib    Object library module (for debug)
| !-MIDIData.dll    Dynamic link library (for release)
!-docs
  |-MIDIData.html    Official guide book (Japanese)
  |-MIDIData_en.html Official guide book (English) 
  |-MIDIDR01.gif    
  |-MIDIDR02.gif    
  |-MIDIDR03.gif    
  !-MIDIDR04.gif    


### How to use ###

 * Extract all files.
 * Put MIDIData.h in the include folder of VisualC++.
 * Put MIDIData.lib in the lib folder of VisualC++.
 * Put MIDIData.dll in the c:\windows\System32\ folder (case 32bit OS), or
   in the c:\windows\SysWOW64\ folder (case 64bit OS).
 * In your source code, write #include "MIDIData.h".
 * In your project, add MIDIData.lib in the build - project's property - 
   consititution and property - linker - input- additional depend file.
 * When you distribute your application, 
   attach MIDIData.dll in the same folder as exe file.

### License ###

(1) This software is released under the terms of GNU LGPL (Lesser General Public 
  License).

(2) This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

### Contact ###

Mail to (temporary) :  ee65051@yahoo.co.jp
Project Web Site :  https://openmidiproject.osdn.jp/index_en.html


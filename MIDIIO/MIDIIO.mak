# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=MIDIIO - Win32 Release
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの MIDIIO - Win32 Release を設定します。
!ENDIF 

!IF "$(CFG)" != "MIDIIO - Win32 Release" && "$(CFG)" != "MIDIIO - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛ 'CFG' を定義することによって
!MESSAGE NMAKE 実行時にﾋﾞﾙﾄﾞ ﾓｰﾄﾞを指定できます。例えば:
!MESSAGE 
!MESSAGE NMAKE /f "MIDIIO.mak" CFG="MIDIIO - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "MIDIIO - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "MIDIIO - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "MIDIIO - Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "MIDIIO - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\MIDIIO.dll"

CLEAN : 
	-@erase ".\Release\MIDIIO.dll"
	-@erase ".\Release\MIDIIO.obj"
	-@erase ".\Release\MIDIIO.lib"
	-@erase ".\Release\MIDIIO.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Gz /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /Gz /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/MIDIIO.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MIDIIO.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/MIDIIO.pdb"\
 /machine:I386 /def:".\MIDIIO.def" /out:"$(OUTDIR)/MIDIIO.dll"\
 /implib:"$(OUTDIR)/MIDIIO.lib" 
DEF_FILE= \
	".\MIDIIO.def"
LINK32_OBJS= \
	"$(INTDIR)/MIDIIO.obj"

"$(OUTDIR)\MIDIIO.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MIDIIO - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\MIDIIOd.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\MIDIIOd.dll"
	-@erase ".\Debug\MIDIIO.obj"
	-@erase ".\Debug\MIDIIOd.ilk"
	-@erase ".\Debug\MIDIIOd.lib"
	-@erase ".\Debug\MIDIIOd.exp"
	-@erase ".\Debug\MIDIIOd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /Gz /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/MIDIIO.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MIDIIO.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/MIDIIOd.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/MIDIIOd.pdb" /debug\
 /machine:I386 /def:".\MIDIIO.def" /out:"$(OUTDIR)/MIDIIOd.dll"\
 /implib:"$(OUTDIR)/MIDIIOd.lib" 
DEF_FILE= \
	".\MIDIIO.def"
LINK32_OBJS= \
	"$(INTDIR)/MIDIIO.obj"

"$(OUTDIR)\MIDIIOd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "MIDIIO - Win32 Release"
# Name "MIDIIO - Win32 Debug"

!IF  "$(CFG)" == "MIDIIO - Win32 Release"

!ELSEIF  "$(CFG)" == "MIDIIO - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\MIDIIO.h

!IF  "$(CFG)" == "MIDIIO - Win32 Release"

!ELSEIF  "$(CFG)" == "MIDIIO - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MIDIIO.c

!IF  "$(CFG)" == "MIDIIO - Win32 Release"

DEP_CPP_MIDII=\
	".\MIDIIO.h"\
	

"$(INTDIR)\MIDIIO.obj" : $(SOURCE) $(DEP_CPP_MIDII) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MIDIIO - Win32 Debug"

DEP_CPP_MIDII=\
	".\MIDIIO.h"\
	
NODEP_CPP_MIDII=\
	".\}"\
	

"$(INTDIR)\MIDIIO.obj" : $(SOURCE) $(DEP_CPP_MIDII) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MIDIIO.def

!IF  "$(CFG)" == "MIDIIO - Win32 Release"

!ELSEIF  "$(CFG)" == "MIDIIO - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################

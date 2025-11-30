# Microsoft Developer Studio Project File - Name="dbprebuild" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dbprebuild - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbprebuild.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbprebuild.mak" CFG="dbprebuild - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbprebuild - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dbprebuild - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "dbprebuild - Win32 Browse" (based on "Win32 (x86) Static Library")
!MESSAGE "dbprebuild - Win32 Minimal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "dbprebuild - Win32 True Time" (based on "Win32 (x86) Static Library")
!MESSAGE "dbprebuild - Win32 Bounds Checker" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbprebuild - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\dbprebuild"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\dbprebuild"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /WX /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /Fr /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\dbprebuild.lib"

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Browse"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinDebug"
# PROP BASE Intermediate_Dir "MinDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /WX /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /Fr /YX /FD /c
# ADD CPP /nologo /W3 /WX /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /Fr /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /WX /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /Fr /YX /FD /c
# ADD CPP /nologo /W3 /WX /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BUILD_WITH_VBA" /Fr /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dbprebuild - Win32 Release"
# Name "dbprebuild - Win32 Debug"
# Name "dbprebuild - Win32 Browse"
# Name "dbprebuild - Win32 Minimal Debug"
# Name "dbprebuild - Win32 True Time"
# Name "dbprebuild - Win32 Bounds Checker"
# Begin Source File

SOURCE="..\lib\db\res\ic-gdt.shx"

!IF  "$(CFG)" == "dbprebuild - Win32 Release"

# Begin Custom Build - Copying binary resources
InputPath="..\lib\db\res\ic-gdt.shx"

"Stage\binary.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\lib\db\res\*.fmp Stage 
	copy ..\lib\db\res\*.fnt Stage 
	copy ..\lib\db\res\*.lin Stage 
	copy ..\lib\db\res\*.unt Stage 
	echo copy exec.time > Stage\binary.trg 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Debug"

# Begin Custom Build - Copying binary resources
InputPath="..\lib\db\res\ic-gdt.shx"

"Debug\binary.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\lib\db\res\*.fmp Debug 
	copy ..\lib\db\res\*.fnt Debug 
	copy ..\lib\db\res\*.lin Debug 
	copy ..\lib\db\res\*.unt Debug 
	echo copy exec.time > Debug\binary.trg 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Browse"

# Begin Custom Build - Copying binary resources
InputPath="..\lib\db\res\ic-gdt.shx"

"Debug\binary.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\lib\db\res\*.fmp Debug 
	copy ..\lib\db\res\*.fnt Debug 
	copy ..\lib\db\res\*.lin Debug 
	copy ..\lib\db\res\*.unt Debug 
	echo copy exec.time > Debug\binary.trg 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Minimal Debug"

# Begin Custom Build - Copying binary resources
InputPath="..\lib\db\res\ic-gdt.shx"

"Debug\binary.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\lib\db\res\*.fmp MinDebug 
	copy ..\lib\db\res\*.fnt MinDebug 
	copy ..\lib\db\res\*.lin MinDebug 
	copy ..\lib\db\res\*.unt MinDebug 
	echo copy exec.time > Debug\binary.trg 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 True Time"

# Begin Custom Build - Copying binary resources
InputPath="..\lib\db\res\ic-gdt.shx"

"True_Time\binary.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\lib\db\res\*.fmp True_Time 
	copy ..\lib\db\res\*.fnt True_Time 
	copy ..\lib\db\res\*.lin True_Time 
	copy ..\lib\db\res\*.unt True_Time 
	echo copy exec.time > True_Time\binary.trg 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Bounds Checker"

# Begin Custom Build - Copying binary resources
InputPath="..\lib\db\res\ic-gdt.shx"

"Bounds_Checker\binary.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\lib\db\res\*.fmp Bounds_Checker 
	copy ..\lib\db\res\*.fnt Bounds_Checker 
	copy ..\lib\db\res\*.lin Bounds_Checker 
	copy ..\lib\db\res\*.unt Bounds_Checker 
	echo copy exec.time > Bounds_Checker\binary.trg 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\Icad.idl

!IF  "$(CFG)" == "dbprebuild - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Browse"

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICAD\icad.odl

!IF  "$(CFG)" == "dbprebuild - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Debug"

# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Browse"

# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Minimal Debug"

# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "dbprebuild - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# End Target
# End Project

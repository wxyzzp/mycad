# Microsoft Developer Studio Project File - Name="auto" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=auto - Win32 Minimal Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "auto.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "auto.mak" CFG="auto - Win32 Minimal Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "auto - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "auto - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "auto - Win32 Browse" (based on "Win32 (x86) Static Library")
!MESSAGE "auto - Win32 Minimal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "auto - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "auto___W"
# PROP BASE Intermediate_Dir "auto___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\auto"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "SDS_NEWDB" /YX"Icad.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug\auto.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "auto - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "auto___0"
# PROP BASE Intermediate_Dir "auto___0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\auto"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "SDS_NEWDB" /YX"Icad.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "auto - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "auto___W"
# PROP BASE Intermediate_Dir "auto___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\auto"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "SDS_NEWDB" /FR /YX"Icad.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "auto - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "auto___W"
# PROP BASE Intermediate_Dir "auto___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\auto"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /D "SDS_NEWDB" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /YX"Icad.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug\auto.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "auto - Win32 Release"
# Name "auto - Win32 Debug"
# Name "auto - Win32 Browse"
# Name "auto - Win32 Minimal Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\lib\auto\IcadAuto.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoApp.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoBase.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoDocs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\icadautoent1.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoEnt2.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\icadautoents.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoLib.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoTable1.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoTable2.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\icadautotables.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoWnds.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\icadplotcfg.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\PlotProperties.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\auto\PlotSupport.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\lib\auto\IcadAuto.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoApp.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoBase.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoDoc.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoDocs.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoEnts.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoLib.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoTables.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoWnd.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\IcadAutoWnds.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\icadplotcfg.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\PlotProperties.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Icad\icad.odl

!IF  "$(CFG)" == "auto - Win32 Release"

# Begin Custom Build - Automation Interface Definitions
InputPath=..\ICAD\icad.odl

BuildCmds= \
	midl /Oicf /I "c:\vba5sdk\include" /D NDEBUG /h "..\icad\IIcad.h"  /iid\
    "..\lib\auto\icad_i.c" /tlb "Release\Icad.tlb" "..\icad\icad.odl"

"..\icad\iicad.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\icad_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Release\icad.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Debug"

# Begin Custom Build - Automation Interface Definitions
InputPath=..\ICAD\icad.odl

BuildCmds= \
	midl /Oicf /I "c:\vba5sdk\include" /D _DEBUG /h "..\icad\IIcad.h"  /iid\
    "..\lib\auto\icad_i.c" /tlb "Debug\Icad.tlb" "..\icad\icad.odl"

"..\icad\iicad.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\icad_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Debug\icad.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Browse"

# Begin Custom Build - Automation Interface Definitions
InputPath=..\ICAD\icad.odl

BuildCmds= \
	midl /Oicf /I "c:\vba5sdk\include" /D _DEBUG /h "..\icad\IIcad.h"  /iid\
    "..\lib\auto\icad_i.c" /tlb "Debug\Icad.tlb" "..\icad\icad.odl"

"..\icad\iicad.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\icad_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Debug\icad.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Minimal Debug"

# Begin Custom Build - Automation Interface Definitions
InputPath=..\ICAD\icad.odl

BuildCmds= \
	midl /Oicf /I "c:\vba5sdk\include" /D NDEBUG /h "..\icad\IIcad.h"  /iid\
    "..\lib\auto\icad_i.c" /tlb "MinDebug\Icad.tlb" "..\icad\icad.odl"

"..\icad\iicad.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\icad_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MinDebug\icad.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\auto\PlotSupport.idl

!IF  "$(CFG)" == "auto - Win32 Release"

# Begin Custom Build - Plot Interface Definitions
InputPath=..\lib\AUTO\PlotSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\PlotSupport.h" /iid "..\lib\auto\PlotSupport_i.c"\
   /tlb "Release\PlotSupport.tlb" "..\lib\auto\PlotSupport.idl"

"..\lib\auto\PlotSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\PlotSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Release\PlotSupport.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Debug"

# Begin Custom Build - Plot Interface Definitions
InputPath=..\lib\AUTO\PlotSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\PlotSupport.h" /iid "..\lib\auto\PlotSupport_i.c"\
   /tlb "Debug\PlotSupport.tlb" "..\lib\auto\PlotSupport.idl"

"..\lib\auto\PlotSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\PlotSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Debug\PlotSupport.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Browse"

# Begin Custom Build - Plot Interface Definitions
InputPath=..\lib\AUTO\PlotSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\PlotSupport.h" /iid "..\lib\auto\PlotSupport_i.c"\
   /tlb "Debug\PlotSupport.tlb" "..\lib\auto\PlotSupport.idl"

"..\lib\auto\PlotSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\PlotSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Debug\PlotSupport.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Minimal Debug"

# Begin Custom Build - Plot Interface Definitions
InputPath=..\lib\AUTO\PlotSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\PlotSupport.h" /iid "..\lib\auto\PlotSupport_i.c"\
   /tlb "MinDebug\PlotSupport.tlb" "..\lib\auto\PlotSupport.idl"

"..\lib\auto\PlotSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\PlotSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MinDebug\PlotSupport.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\auto\PlotSupport.rc
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\RegSupport.idl

!IF  "$(CFG)" == "auto - Win32 Release"

# Begin Custom Build
InputPath=..\lib\AUTO\RegSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\RegSupport.h" /iid "..\lib\auto\RegSupport_i.c"\
   "..\lib\auto\RegSupport.idl"

"..\lib\auto\RegSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\RegSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Debug"

# Begin Custom Build
InputPath=..\lib\AUTO\RegSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\RegSupport.h" /iid "..\lib\auto\RegSupport_i.c"\
   "..\lib\auto\RegSupport.idl"

"..\lib\auto\RegSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\RegSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Browse"

# Begin Custom Build
InputPath=..\lib\AUTO\RegSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\RegSupport.h" /iid "..\lib\auto\RegSupport_i.c"\
  /tlb "Debug\PlotSupport.tlb"  "..\lib\auto\RegSupport.idl"

"..\lib\auto\RegSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\RegSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "auto - Win32 Minimal Debug"

# Begin Custom Build
InputPath=..\lib\AUTO\RegSupport.idl

BuildCmds= \
	midl /Oicf /h "..\lib\auto\RegSupport.h" /iid "..\lib\auto\RegSupport_i.c"\
   "..\lib\auto\RegSupport.idl"

"..\lib\auto\RegSupport.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\lib\auto\RegSupport_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project

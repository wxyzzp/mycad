# Microsoft Developer Studio Project File - Name="grlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=grlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "grlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "grlib.mak" CFG="grlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "grlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "grlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "grlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRSTATIC" /D "DBSTATIC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Release\grlib.lib"

!ELSEIF  "$(CFG)" == "grlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\grlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRSTATIC" /D "DBSTATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Debug\grlib.lib"

!ENDIF 

# Begin Target

# Name "grlib - Win32 Release"
# Name "grlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Not Precompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\GR\dispobjs.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\Sds\sds_findfile.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_resbuf.cpp
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\GR\arcgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordSetup.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordUCS.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordWidePC.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\debuginfo.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\entsel.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_DisplaySurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_Hatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_splines.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_view.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\leadergen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\linetypegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\mlinegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\mtextgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\pc2dispobj.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\pointgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\rayxlinegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\selectedent.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\selectorlink.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\splinegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\textgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\tolerancegen.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\lib\Gr\Gr.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_DisplaySurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_view.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\splines.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\splineutils.h
# End Source File
# End Group
# End Target
# End Project

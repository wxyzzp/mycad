# Microsoft Developer Studio Project File - Name="Its" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Its - Win32 ReleaseTest
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Its.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Its.mak" CFG="Its - Win32 ReleaseTest"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Its - Win32 DebugTest" (based on "Win32 (x86) Application")
!MESSAGE "Its - Win32 ReleaseTest" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Its - Win32 DebugTest"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugTest"
# PROP BASE Intermediate_Dir "DebugTest"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Its"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\TestSystem" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFX_NO_DEBUG_CRT" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icadlib.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /pdbtype:sept /libpath:".\Debug" /defaultlib:"acis.lib" /defaultlib:"testunitd.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Its - Win32 ReleaseTest"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Its___Win32_ReleaseTest"
# PROP BASE Intermediate_Dir "Its___Win32_ReleaseTest"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Its"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\TestSystem" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFX_NO_DEBUG_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\TestSystem" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFX_NO_DEBUG_CRT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /libpath:"$(A3DT)\lib\NT_DLL" /libpath:".\Release" /opt:noref
# SUBTRACT LINK32 /pdb:none /debug

!ENDIF 

# Begin Target

# Name "Its - Win32 DebugTest"
# Name "Its - Win32 ReleaseTest"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\TestSystem\Its\ItsApp.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsApp.rc
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsLinkLibraries.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsMainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsResult.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsResultView.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsTestRunner.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsTestTree.cpp
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\UiProgressBar.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\TestSystem\Its\ItsApp.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsDoc.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsMainFrm.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsResult.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsResultView.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsTestRunner.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\ItsTestTree.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\Resource.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\UiProgressBar.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\TestSystem\Its\res\Its.rc2
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\res\ItsApp.ico
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\res\ItsDoc.ico
# End Source File
# Begin Source File

SOURCE=..\TestSystem\Its\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\TestSystem\Its\ReadMe.txt
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="DCL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DCL - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DCL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DCL.mak" CFG="DCL - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DCL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Minimal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Release Ita_It" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Release Jpn_Jp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Debug Jpn_Jp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Release Chn_Sp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Debug Chn_Sp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Release Chn_Td" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Debug Chn_Td" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 True Time" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DCL - Win32 Bounds Checker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DCL - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prj\make\Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-Us" /i "..\..\..\prj\icad" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\DCL.def" /out:"..\..\..\prj\make\Stage/DCL.dll" /OPT:NOREF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-Us" /i "..\..\..\prj\icad" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "DCL - Win32 Browse"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DCL___Win32_Browse"
# PROP BASE Intermediate_Dir "DCL___Win32_Browse"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prj\make\Debug"
# PROP Intermediate_Dir "..\..\..\prj\make\Debug\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "DCL - Win32 Minimal Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DCL___Win32_Minimal_Debug"
# PROP BASE Intermediate_Dir "DCL___Win32_Minimal_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prj\make\MinDebug"
# PROP Intermediate_Dir "..\..\..\prj\make\MinDebug\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\\" /I "..\PCCTS" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "_WINDLL" /D "_AFXDLL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MINDEBUG" /FD /c
# SUBTRACT CPP /WX /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-us" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /OPT:NOREF
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /OPT:NOREF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Ita_It"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DCL___Win32_Release_Ita_it"
# PROP BASE Intermediate_Dir "DCL___Win32_Release_Ita_it"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prj\make\Release"
# PROP Intermediate_Dir "..\..\..\prj\make\Release\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Ita-it" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\Ita-it" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\prj\make\Stage/DCL.dll" /OPT:NOREF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Jpn_Jp"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DCL___Win32_Release_Jpn_Jp"
# PROP BASE Intermediate_Dir "DCL___Win32_Release_Jpn_Jp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prj\make\Release"
# PROP Intermediate_Dir "..\..\..\prj\make\Release\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Jpn-Jp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x411 /i "..\..\..\..\Jpn-jp" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /def:".\DCL.def" /out:"..\..\..\prj\make\Stage/DCL.dll" /OPT:NOREF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Jpn_Jp"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DCL___Win32_Debug_Jpn_Jp"
# PROP BASE Intermediate_Dir "DCL___Win32_Debug_Jpn_Jp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prj\make\Debug"
# PROP Intermediate_Dir "..\..\..\prj\make\Debug\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Jpn-Jp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x411 /i "..\..\..\..\Jpn-jp" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Chn_Sp"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DCL___Win32_Release_Chn_Sp"
# PROP BASE Intermediate_Dir "DCL___Win32_Release_Chn_Sp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prj\make\Release"
# PROP Intermediate_Dir "..\..\..\prj\make\Release\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Chn-Sp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x804 /i "..\..\..\..\Chn-Sp" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /def:".\DCL.def" /out:"..\..\..\prj\make\Stage/DCL.dll" /OPT:NOREF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Chn_Sp"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DCL___Win32_Debug_Chn_Sp"
# PROP BASE Intermediate_Dir "DCL___Win32_Debug_Chn_Sp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prj\make\Debug"
# PROP Intermediate_Dir "..\..\..\prj\make\Debug\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Chn-Sp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x804 /i "..\..\..\..\Chn-Sp" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Chn_Td"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DCL___Win32_Release_Chn_Td"
# PROP BASE Intermediate_Dir "DCL___Win32_Release_Chn_Td"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\prj\make\Release"
# PROP Intermediate_Dir "..\..\..\prj\make\Release\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Chn-Td" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x404 /i "..\..\..\..\Chn-Td" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /def:".\DCL.def" /out:"..\..\..\prj\make\Stage/DCL.dll" /OPT:NOREF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Chn_Td"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DCL___Win32_Debug_Chn_Td"
# PROP BASE Intermediate_Dir "DCL___Win32_Debug_Chn_Td"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prj\make\Debug"
# PROP Intermediate_Dir "..\..\..\prj\make\Debug\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Chn-Td" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x404 /i "..\..\..\..\Chn-Td" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "DCL - Win32 True Time"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prj\make\True_Time"
# PROP Intermediate_Dir "..\..\..\prj\make\True_Time\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "DCL - Win32 Bounds Checker"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker\DCL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\prj\make\Bounds_Checker"
# PROP Intermediate_Dir "..\..\..\prj\make\Bounds_Checker\DCL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I ".\\" /I "..\PCCTS" /I "..\include" /I "..\..\..\prj\api\sds" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "DCL - Win32 Release"
# Name "DCL - Win32 Debug"
# Name "DCL - Win32 Browse"
# Name "DCL - Win32 Minimal Debug"
# Name "DCL - Win32 Release Ita_It"
# Name "DCL - Win32 Release Jpn_Jp"
# Name "DCL - Win32 Debug Jpn_Jp"
# Name "DCL - Win32 Release Chn_Sp"
# Name "DCL - Win32 Debug Chn_Sp"
# Name "DCL - Win32 Release Chn_Td"
# Name "DCL - Win32 Debug Chn_Td"
# Name "DCL - Win32 True Time"
# Name "DCL - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DCL.cpp
# End Source File
# Begin Source File

SOURCE=.\DCL.def

!IF  "$(CFG)" == "DCL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug"

!ELSEIF  "$(CFG)" == "DCL - Win32 Browse"

!ELSEIF  "$(CFG)" == "DCL - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Jpn_Jp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Chn_Sp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Chn_Td"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "DCL - Win32 True Time"

!ELSEIF  "$(CFG)" == "DCL - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DCL.rc
# End Source File
# Begin Source File

SOURCE=.\Global.cpp

!IF  "$(CFG)" == "DCL - Win32 Release"

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug"

# ADD CPP /Ob2
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "DCL - Win32 Browse"

# ADD BASE CPP /Ob2
# SUBTRACT BASE CPP /O<none>
# ADD CPP /Ob2
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "DCL - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Jpn_Jp"

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Jpn_Jp"

# ADD CPP /Ob2
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Chn_Sp"

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Chn_Sp"

# ADD CPP /Ob2
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "DCL - Win32 Release Chn_Td"

!ELSEIF  "$(CFG)" == "DCL - Win32 Debug Chn_Td"

# ADD CPP /Ob2
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "DCL - Win32 True Time"

# ADD BASE CPP /Ob2
# SUBTRACT BASE CPP /O<none>
# ADD CPP /Ob2
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "DCL - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DCL.h
# End Source File
# Begin Source File

SOURCE=.\dclprivate.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DCL.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Error.ico
# End Source File
# End Group
# Begin Group "PCCTS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\PCCTS\AParser.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\AParser.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\AToken.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\ATokenBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\ATokenBuffer.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\ATokenStream.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\ATokPtr.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\config.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\DLexerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\DLexerBase.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\DLGLexer.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\DLGLexer.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\GrammarWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\PCCTS\Grammer.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\Grammer.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\tokens.h
# End Source File
# End Group
# Begin Group "DCL Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\AttributeInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AuditLevel.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Button.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ControlID.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\DefaultAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\EditBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ErrorMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ExceptionManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\FileInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ImageButton.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\InactiveCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ListBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ListOfAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Main.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\NonDefinedNode.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\OpenDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Parser.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\PopUpList.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ScreenObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Slider.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Spacer.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Text.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Tile.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TileAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TileAttributes.h
# End Source File
# Begin Source File

SOURCE=..\Source\Tree.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TreeDebugger.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TreeNode.cpp
# End Source File
# End Group
# Begin Group "DCL Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Include\Button.h
# End Source File
# Begin Source File

SOURCE=..\Include\ControlID.h
# End Source File
# Begin Source File

SOURCE=..\Include\DefaultAttributes.h
# End Source File
# Begin Source File

SOURCE=..\Include\Dialog.h
# End Source File
# Begin Source File

SOURCE=..\Include\EditBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\ErrorMessage.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExceptionManager.h
# End Source File
# Begin Source File

SOURCE=..\Include\FileInfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\InactiveCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\ListBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\ListOfAttributes.h
# End Source File
# Begin Source File

SOURCE=..\Include\Main.h
# End Source File
# Begin Source File

SOURCE=..\Include\NonDefinedNode.h
# End Source File
# Begin Source File

SOURCE=..\Include\OpenDlg.h
# End Source File
# Begin Source File

SOURCE=..\Include\Parser.h
# End Source File
# Begin Source File

SOURCE=..\Include\PopUpList.h
# End Source File
# Begin Source File

SOURCE=..\Include\ScreenObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\Slider.h
# End Source File
# Begin Source File

SOURCE=..\Include\Spacer.h
# End Source File
# Begin Source File

SOURCE=..\Include\Text.h
# End Source File
# Begin Source File

SOURCE=..\Include\Tile.h
# End Source File
# Begin Source File

SOURCE=..\Include\TileAttributes.h
# End Source File
# Begin Source File

SOURCE=..\Include\Tree.h
# End Source File
# Begin Source File

SOURCE=..\Include\TreeDebugger.h
# End Source File
# Begin Source File

SOURCE=..\Include\TreeNode.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Include\base.dcl
# End Source File
# Begin Source File

SOURCE=..\include\primitives.dcl
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

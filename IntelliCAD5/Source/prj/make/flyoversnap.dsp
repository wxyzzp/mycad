# Microsoft Developer Studio Project File - Name="flyoversnap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=flyoversnap - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "flyoversnap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "flyoversnap.mak" CFG="flyoversnap - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "flyoversnap - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "flyoversnap - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "flyoversnap - Win32 True Time" (based on "Win32 (x86) Static Library")
!MESSAGE "flyoversnap - Win32 Bounds Checker" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "flyoversnap - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\flyoversnap"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\..\api\sds" /I ".\icad" /I ".\..\lib\sds" /I ".\..\lib\flyoversnap" /I ".\..\lib\auto" /I ".\..\lib\IcadUtils" /I ".\..\lib\db" /I ".\..\lib\gr" /I ".\..\lib\lisp" /I ".\..\icad" /I ".\..\lib\icadlib" /I ".\..\lib\drw" /I ".\..\lib\cmds" /I ".\..\..\ThirdParty\geo" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "flyoversnap - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\flyoversnap"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\..\api\sds" /I ".\..\lib\sds" /I ".\..\lib\flyoversnap" /I ".\..\lib\auto" /I ".\..\lib\IcadUtils" /I ".\..\lib\db" /I ".\..\lib\gr" /I ".\..\lib\lisp" /I ".\..\icad" /I ".\..\lib\icadlib" /I ".\..\lib\drw" /I ".\..\lib\cmds" /I ".\..\..\ThirdParty\geo" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\flyoversnap.lib"

!ELSEIF  "$(CFG)" == "flyoversnap - Win32 True Time"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\flyoversnap"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\..\api\sds" /I ".\..\lib\sds" /I ".\..\lib\flyoversnap" /I ".\..\lib\auto" /I ".\..\lib\IcadUtils" /I ".\..\lib\db" /I ".\..\lib\gr" /I ".\..\lib\lisp" /I ".\..\icad" /I ".\..\lib\icadlib" /I ".\..\lib\drw" /I ".\..\lib\cmds" /I ".\..\..\ThirdParty\geo" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\..\api\sds" /I ".\..\lib\sds" /I ".\..\lib\flyoversnap" /I ".\..\lib\auto" /I ".\..\lib\IcadUtils" /I ".\..\lib\db" /I ".\..\lib\gr" /I ".\..\lib\lisp" /I ".\..\icad" /I ".\..\lib\icadlib" /I ".\..\lib\drw" /I ".\..\lib\cmds" /I ".\..\..\ThirdParty\geo" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "flyoversnap - Win32 Bounds Checker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\flyoversnap"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\..\api\sds" /I ".\..\lib\sds" /I ".\..\lib\flyoversnap" /I ".\..\lib\auto" /I ".\..\lib\IcadUtils" /I ".\..\lib\db" /I ".\..\lib\gr" /I ".\..\lib\lisp" /I ".\..\icad" /I ".\..\lib\icadlib" /I ".\..\lib\drw" /I ".\..\lib\cmds" /I ".\..\..\ThirdParty\geo" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\..\api\sds" /I ".\..\lib\sds" /I ".\..\lib\flyoversnap" /I ".\..\lib\auto" /I ".\..\lib\IcadUtils" /I ".\..\lib\db" /I ".\..\lib\gr" /I ".\..\lib\lisp" /I ".\..\icad" /I ".\..\lib\icadlib" /I ".\..\lib\drw" /I ".\..\lib\cmds" /I ".\..\..\ThirdParty\geo" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "flyoversnap - Win32 Release"
# Name "flyoversnap - Win32 Debug"
# Name "flyoversnap - Win32 True Time"
# Name "flyoversnap - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\flyoversnap\hiparray.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\OsnapHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\osnapLocker.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\osnapManagerImp.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\OsnapMarkerStd.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\OsnapModeDefTan.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\osnapModeStd.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\lib\flyoversnap\hiparray.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\OsnapHelpers.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\osnapLocker.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\osnapManagerIcad.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\osnapManagerImp.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\OsnapMarkerStd.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\OsnapModeDefTan.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\osnapModeStd.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\OsnapObjects.h
# End Source File
# Begin Source File

SOURCE=..\lib\flyoversnap\StdAfx.h
# End Source File
# End Group
# End Target
# End Project

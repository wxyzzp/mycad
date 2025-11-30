# Microsoft Developer Studio Project File - Name="IcadUtils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IcadUtils - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IcadUtils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IcadUtils.mak" CFG="IcadUtils - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IcadUtils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "IcadUtils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "IcadUtils - Win32 Minimal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "IcadUtils - Win32 True Time" (based on "Win32 (x86) Static Library")
!MESSAGE "IcadUtils - Win32 Bounds Checker" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IcadUtils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\IcadUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX"IcadUtils.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "IcadUtils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\IcadUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX"IcadUtils.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\IcadUtils.lib"

!ELSEIF  "$(CFG)" == "IcadUtils - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "IcadUtils___Win32_Minimal_Debug"
# PROP BASE Intermediate_Dir "IcadUtils___Win32_Minimal_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "mindebug\IcadUtils"
# PROP Intermediate_Dir "MinDebug\IcadUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /ZI /Od /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX"IcadUtils.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /Zi /O2 /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX"IcadUtils.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "IcadUtils - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\IcadUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX"IcadUtils.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX"IcadUtils.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "IcadUtils - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\IcadUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX"IcadUtils.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX"IcadUtils.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "IcadUtils - Win32 Release"
# Name "IcadUtils - Win32 Debug"
# Name "IcadUtils - Win32 Minimal Debug"
# Name "IcadUtils - Win32 True Time"
# Name "IcadUtils - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\IcadUtils\TransformUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\lib\IcadUtils\TransformUtils.h
# End Source File
# End Group
# End Target
# End Project

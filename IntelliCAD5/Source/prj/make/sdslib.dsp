# Microsoft Developer Studio Project File - Name="sds LIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sds LIB - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sdslib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sdslib.mak" CFG="sds LIB - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sds LIB - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sds LIB - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "sds LIB - Win32 True Time" (based on "Win32 (x86) Static Library")
!MESSAGE "sds LIB - Win32 Bounds Checker" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sds LIB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\sds LIB\Release"
# PROP BASE Intermediate_Dir ".\sds LIB\Release"
# PROP BASE Target_Dir ".\sds LIB"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\sdslib"
# PROP Target_Dir ".\sds LIB"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I ".." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FD /c
# SUBTRACT CPP /WX /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\api\sds\sds.lib"

!ELSEIF  "$(CFG)" == "sds LIB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\sds LIB\Debug"
# PROP BASE Intermediate_Dir ".\sds LIB\Debug"
# PROP BASE Target_Dir ".\sds LIB"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\sdslib"
# PROP Target_Dir ".\sds LIB"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Og /I ".." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /Fr /FD /c
# SUBTRACT CPP /Ox /Ot /Oi /Os /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\api\sds\sds.lib"

!ELSEIF  "$(CFG)" == "sds LIB - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sds LIB\True Time"
# PROP BASE Intermediate_Dir "sds LIB\True Time"
# PROP BASE Target_Dir "sds LIB"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "sds LIB\True Time"
# PROP Intermediate_Dir "sds LIB\True Time"
# PROP Target_Dir "sds LIB"
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Og /I ".." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /Fr /FD /c
# SUBTRACT BASE CPP /Ox /Ot /Oi /Os /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Og /I ".." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /Fr /FD /c
# SUBTRACT CPP /Ox /Ot /Oi /Os /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\api\sds\sds.lib"
# ADD LIB32 /nologo /out:"..\api\sds\sds.lib"

!ELSEIF  "$(CFG)" == "sds LIB - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sds LIB\Bounds Checker"
# PROP BASE Intermediate_Dir "sds LIB\Bounds Checker"
# PROP BASE Target_Dir "sds LIB"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "sds LIB\Bounds Checker"
# PROP Intermediate_Dir "sds LIB\Bounds Checker"
# PROP Target_Dir "sds LIB"
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Og /I ".." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /Fr /FD /c
# SUBTRACT BASE CPP /Ox /Ot /Oi /Os /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Og /I ".." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /Fr /FD /c
# SUBTRACT CPP /Ox /Ot /Oi /Os /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\api\sds\sds.lib"
# ADD LIB32 /nologo /out:"..\api\sds\sds.lib"

!ENDIF 

# Begin Target

# Name "sds LIB - Win32 Release"
# Name "sds LIB - Win32 Debug"
# Name "sds LIB - Win32 True Time"
# Name "sds LIB - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\api\sds\entpoint.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\api\sds\sds.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=Stage\icad.lib
# End Source File
# End Target
# End Project

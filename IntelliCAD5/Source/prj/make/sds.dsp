# Microsoft Developer Studio Project File - Name="sds" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sds - Win32 Browse
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sds.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sds.mak" CFG="sds - Win32 Browse"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sds - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sds - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "sds - Win32 Browse" (based on "Win32 (x86) Static Library")
!MESSAGE "sds - Win32 Minimal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sds - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\sds\Release"
# PROP BASE Intermediate_Dir ".\sds\Release"
# PROP BASE Target_Dir ".\sds"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\sds"
# PROP Target_Dir ".\sds"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"icad.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\icadsds.lib"

!ELSEIF  "$(CFG)" == "sds - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\sds\Debug"
# PROP BASE Intermediate_Dir ".\sds\Debug"
# PROP BASE Target_Dir ".\sds"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\sds"
# PROP Target_Dir ".\sds"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /GX /ZI /Od /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_SDSSTANDALONE" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"icad.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\Debug/sds.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\icadsds.lib"

!ELSEIF  "$(CFG)" == "sds - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sds\Browse"
# PROP BASE Intermediate_Dir "sds\Browse"
# PROP BASE Target_Dir "sds"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\sds"
# PROP Target_Dir "sds"
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\icadlib" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_SDSSTANDALONE" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /YX"icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /ZI /Od /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_SDSSTANDALONE" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /YX"icad.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\Debug/sds.bsc"
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\icadsds.lib"

!ELSEIF  "$(CFG)" == "sds - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "sds\Minimal Debug"
# PROP BASE Intermediate_Dir "sds\Minimal Debug"
# PROP BASE Target_Dir "sds"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\sds"
# PROP Target_Dir "sds"
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"icad.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /Zi /O2 /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"icad.h" /FD /c
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

# Name "sds - Win32 Release"
# Name "sds - Win32 Debug"
# Name "sds - Win32 Browse"
# Name "sds - Win32 Minimal Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\Lib\Sds\Sds5.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Sds\sds_alert.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Sds\sds_findfile.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Sds\sds_printf.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\API\SDS\sds.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

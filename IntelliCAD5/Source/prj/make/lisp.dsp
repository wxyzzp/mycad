# Microsoft Developer Studio Project File - Name="lisp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lisp - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lisp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lisp.mak" CFG="lisp - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lisp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lisp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "lisp - Win32 Browse" (based on "Win32 (x86) Static Library")
!MESSAGE "lisp - Win32 Minimal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "lisp - Win32 True Time" (based on "Win32 (x86) Static Library")
!MESSAGE "lisp - Win32 Bounds Checker" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lisp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\lisp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"lisp.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "lisp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\lisp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\flyoversnap" /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"lisp.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\lisp.lib"

!ELSEIF  "$(CFG)" == "lisp - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lisp___W"
# PROP BASE Intermediate_Dir "lisp___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\lisp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\icadlib" /I "..\lib\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\geo" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /YX"lisp.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR /YX"lisp.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "lisp - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lisp___W"
# PROP BASE Intermediate_Dir "lisp___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\lisp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\geo" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"lisp.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /Zi /O2 /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "NDEBUG" /D "MINDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"lisp.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "lisp - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\lisp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\flyoversnap" /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"lisp.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\flyoversnap" /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"lisp.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "lisp - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\lisp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\flyoversnap" /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"lisp.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\flyoversnap" /I "..\api\sds" /I "..\..\ThirdParty\geo" /I "..\..\ThirdParty\DCL_Components\include" /I "..\icad" /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\lib\db" /I "..\lib\sds" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"lisp.h" /FD /c
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

# Name "lisp - Win32 Release"
# Name "lisp - Win32 Debug"
# Name "lisp - Win32 Browse"
# Name "lisp - Win32 Minimal Debug"
# Name "lisp - Win32 True Time"
# Name "lisp - Win32 Bounds Checker"
# Begin Source File

SOURCE=..\lib\LISP\app.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\bindtable.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\bindtable.h
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\commandatom.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\commandatom.h
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\compare.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\consoleio.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\convert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\datahome.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\dcl.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\dict.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\ent.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\freers.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\funcs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\geom.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\getters.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\gr.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\interpreter.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\LISP.H
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\lispvars.h
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\list.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\logical.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\loopncond.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\math.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\mem.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\predefs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\ss.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\string.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\sym.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\system.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\table.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\LISP\xdata.cpp
# End Source File
# End Target
# End Project

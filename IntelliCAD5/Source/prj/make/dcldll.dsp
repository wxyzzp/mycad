# Microsoft Developer Studio Project File - Name="dcl DLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dcl DLL - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dcldll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dcldll.mak" CFG="dcl DLL - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dcl DLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dcl DLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dcl DLL - Win32 Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dcl DLL - Win32 Minimal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dcl DLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\dcl DLL\Release"
# PROP BASE Intermediate_Dir ".\dcl DLL\Release"
# PROP BASE Target_Dir ".\dcl DLL"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\dcldll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\dcl DLL"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /WX /GX /O1 /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shdw32m.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ctl3d32.lib /nologo /base:"0x51B80000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt" /out:".\Stage\dcl.dll" /libpath:"..\..\ThirdParty\smartheap" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "dcl DLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\dcl DLL\dcl_DLL_"
# PROP BASE Intermediate_Dir ".\dcl DLL\dcl_DLL_"
# PROP BASE Target_Dir ".\dcl DLL"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\dcldll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\dcl DLL"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ctl3d32.lib /nologo /base:"0x04000000" /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\dcl.dll" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "dcl DLL - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcl DLL\Browse"
# PROP BASE Intermediate_Dir "dcl DLL\Browse"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir "dcl DLL"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\dcldll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "dcl DLL"
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /WX /GX /O2 /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\sds" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ctl3d32.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Stage\dcl.dll"
# ADD LINK32 delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ctl3d32.lib /nologo /base:"0x04000000" /subsystem:windows /dll /machine:I386 /out:".\Debug\dcl.dll" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "dcl DLL - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcl DLL\Minimal Debug"
# PROP BASE Intermediate_Dir "dcl DLL\Minimal Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir "dcl DLL"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\dcldll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "dcl DLL"
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /WX /GX /Zi /O2 /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ctl3d32.lib /nologo /base:"0x51B80000" /subsystem:windows /dll /machine:I386 /out:".\Stage\dcl.dll"
# ADD LINK32 shdw32m.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ctl3d32.lib /nologo /base:"0x51B80000" /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"libcmt" /out:"MinDebug\dcl.dll" /delayload:db.dll

!ENDIF 

# Begin Target

# Name "dcl DLL - Win32 Release"
# Name "dcl DLL - Win32 Debug"
# Name "dcl DLL - Win32 Browse"
# Name "dcl DLL - Win32 Minimal Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\lib\Dcl\Dcl.def
# End Source File
# Begin Source File

SOURCE=..\lib\Dcl\Dcl1.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Dcl\Dcl2.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Dcl\Dcl3.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Dcl\Dcl4.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Dcl\Dcl5.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Dcl\Dcl6.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DCL\dcldll.rc
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_alert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_findfile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

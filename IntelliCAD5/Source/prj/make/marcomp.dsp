# Microsoft Developer Studio Project File - Name="Marcomp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Marcomp - Win32 Browse
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Marcomp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Marcomp.mak" CFG="Marcomp - Win32 Browse"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Marcomp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Marcomp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Marcomp - Win32 Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Marcomp - Win32 Minimal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Marcomp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Marcomp_"
# PROP BASE Intermediate_Dir "Marcomp_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Marcomp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /O1 /I "..\..\ThirdParty\marcomp" /D "NDEBUG" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x51f80000" /subsystem:windows /dll /pdb:".\Release\DwgEx.pdb" /machine:I386 /out:".\Stage\DwgEx.dll"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying drwex,SmartHeap DLL
PostBuild_Cmds=copy ..\..\ThirdParty\Smartheap\sh30w32.dll Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Marcomp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Marcomp0"
# PROP BASE Intermediate_Dir "Marcomp0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Marcomp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /Zi /Od /I "..\..\ThirdParty\marcomp" /D "_DEBUG" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x06000000" /subsystem:windows /dll /pdb:".\Debug\DwgEx.pdb" /debug /machine:I386 /out:".\Debug\DwgEx.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying drwex,SmartHeap DLL
PostBuild_Cmds=copy ..\..\ThirdParty\Smartheap\sh30w32.dll Debug 	mkdir   Debug\Fonts	copy            Stage\Fonts Debug\Fonts	copy Stage\icad.dwg Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Marcomp - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Marcomp_"
# PROP BASE Intermediate_Dir "Marcomp_"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\marcomp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /D "_DEBUG" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "NODISGUISE" /D "ODT_WIN32ONLY" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "..\..\ThirdParty\marcomp" /D "_DEBUG" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:".\Debug\DwgEx.pdb" /debug /machine:I386 /out:".\Debug\DwgEx.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x06000000" /subsystem:windows /dll /pdb:".\Debug\DwgEx.pdb" /debug /machine:I386 /out:".\Debug\DwgEx.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Marcomp - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Marcomp_"
# PROP BASE Intermediate_Dir "Marcomp_"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\Marcomp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /WX /GX /O2 /D "NDEBUG" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "NODISGUISE" /D "ODT_WIN32ONLY" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /Zi /O2 /I "..\..\ThirdParty\marcomp" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "SDS_NEWDB" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MINDEBUG" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x51f80000" /subsystem:windows /dll /pdb:".\Release\DwgEx.pdb" /machine:I386 /out:".\Stage\DwgEx.dll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x51f80000" /subsystem:windows /dll /profile /debug /machine:I386 /out:"MinDebug\DwgEx.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying drwex,SmartHeap DLL
PostBuild_Cmds=copy ..\..\ThirdParty\Smartheap\sh30w32.dll Stage
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Marcomp - Win32 Release"
# Name "Marcomp - Win32 Debug"
# Name "Marcomp - Win32 Browse"
# Name "Marcomp - Win32 Minimal Debug"
# Begin Source File

SOURCE=..\lib\marcomp\ad2.def
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\AD2.H
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\marcomp\ad2proti.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\marcomp\ad2strus.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADAUDIT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADBLOB.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADBTREE.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADCALLSR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDATA.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDEFAUL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDWGR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDXFDAT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDXFR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADEDDTBL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADERROR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADFONTS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADFREEEM.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADINCLUD.H
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADLL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADNEW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRAPP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRAPPDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLHDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLKDF.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLKDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLKH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLRDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRCLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRCLSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDIC.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDICDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDIMDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDIMST.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDWOBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRENTDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRENTFN.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRFNSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRFRD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRFRD2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRH2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRHDRDF.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRHDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLAY.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLAYDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLTP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLTPDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRMLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRMLSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADROBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADROBJDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRRAS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRSHP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRSHPDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRSTEND.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRTBLDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRTBLLD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRUCS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRUCSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRUTILS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVIEDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVIEW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPEDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPODX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPORT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRXD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADSNIFF.C
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\adstream.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\adstream.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADTEXT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADTJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVAPP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVBLKH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVCLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVDIC.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVDIMST.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVFILE.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVLAY.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVLD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVLTP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVMLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVOBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVSHP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVUCS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVVIEW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVVPORT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVXD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW13ENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW13TBL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW2DX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW3.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW4.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW5.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWDWOBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWENDDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWENTDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFILE1.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFILE2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFNSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFWR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFWR2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWHAND.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWHDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWINIT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWXD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWZOMB.C
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\dwgex.rc
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\filestream.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\filestream.h
# End Source File
# End Target
# End Project

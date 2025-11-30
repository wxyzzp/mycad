# Microsoft Developer Studio Project File - Name="Authenticate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Authenticate - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Authenticate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Authenticate.mak" CFG="Authenticate - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Authenticate - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Minimal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release Deu_De" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release Fra_Fr" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release Esl_Es" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release Ita_It" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release Jpn_Jp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Debug Jpn_Jp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release Chn_Sp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Debug Chn_Sp" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Release Chn_Td" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Debug Chn_Td" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 True Time" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Authenticate - Win32 Bounds Checker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Authenticate - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /i ".\..\icad" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"\OutputDir\icad5\Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=\OutputDir\icad5\Debug
TargetPath=\OutputDir\icad5\Debug\IcadAuth.dll
InputPath=\OutputDir\icad5\Debug\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenti"
# PROP BASE Intermediate_Dir "Authenti"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i ".\..\..\..\Eng-Us" /i ".\..\icad" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /map /debug /machine:I386 /out:"Stage/IcadAuth.dll" /libpath:"Release" /MAPINFO:LINES /opt:ref,icf
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Stage\IcadAuth.dll
InputPath=.\Stage\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Browse"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Authent2"
# PROP BASE Intermediate_Dir "Authent2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /FR /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\IcadAuth.dll
InputPath=.\Debug\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Minimal Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Authenti"
# PROP BASE Intermediate_Dir "Authenti"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /FR /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /O2 /I "..\lib\icadlib" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MINDEBUG" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /debug /machine:I386 /out:"MinDebug/IcadAuth.dll" /pdbtype:sept /libpath:"MinDebug"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\MinDebug
TargetPath=.\MinDebug\IcadAuth.dll
InputPath=.\MinDebug\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Deu_De"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenticate___Win32_Release_Deu_De"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Release_Deu_De"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\Deu-De"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate\Deu-De"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Deu-de" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/Authenticate.bsc"
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib authenticate.res /nologo /base:"0x51680000" /subsystem:windows /dll /pdb:none /debug /machine:I386 /out:"Release/IcadAuth.dll" /libpath:"Release"
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Release/Deu-De/IcadAuth.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release\Deu-De
TargetPath=.\Release\Deu-De\IcadAuth.dll
InputPath=.\Release\Deu-De\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Fra_Fr"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenticate___Win32_Release_Fra_Fr"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Release_Fra_Fr"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\Fra-Fr"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate\Fra-Fr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Fra-fr" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/Authenticate.bsc"
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib authenticate.res /nologo /base:"0x51680000" /subsystem:windows /dll /pdb:none /debug /machine:I386 /out:"Release/IcadAuth.dll" /libpath:"Release"
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Release/Fra-Fr/IcadAuth.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release\Fra-Fr
TargetPath=.\Release\Fra-Fr\IcadAuth.dll
InputPath=.\Release\Fra-Fr\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Esl_Es"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenticate___Win32_Release_Esl_Es"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Release_Esl_Es"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\Esl-Es"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate\Esl-Es"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Esl-es" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/Authenticate.bsc"
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib authenticate.res /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Release/Fra-Fr/IcadAuth.dll" /libpath:"Release"
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 icadlib.lib version.lib authenticate.res /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Release/Esl-Es/IcadAuth.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release\Esl-Es
TargetPath=.\Release\Esl-Es\IcadAuth.dll
InputPath=.\Release\Esl-Es\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=echo Compiling Authenticate resources	rc /fo authenticate.res /i ../icad/res/ ./../../../Esl-Es/authenticate.rc
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Ita_It"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenticate___Win32_Release_Ita_It"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Release_Ita_It"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Ita-it" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Stage/IcadAuth.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Stage\IcadAuth.dll
InputPath=.\Stage\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Jpn_Jp"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenticate___Win32_Release_Jpn_Jp"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Release_Jpn_Jp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x411 /i "..\..\..\Jpn-jp" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/Authenticate.bsc"
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Release/IcadAuth.dll" /libpath:"Release"
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Stage/IcadAuth.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Stage\IcadAuth.dll
InputPath=.\Stage\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Jpn_Jp"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Authenticate___Win32_Debug_Jpn_Jp"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Debug_Jpn_Jp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x411 /i "..\..\..\Jpn-jp" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\IcadAuth.dll
InputPath=.\Debug\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Sp"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenticate___Win32_Release_Chn_Sp"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Release_Chn_Sp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i "..\..\..\Chn-Sp" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/Authenticate.bsc"
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Release/IcadAuth.dll" /libpath:"Release"
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Stage/IcadAuth.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Stage\IcadAuth.dll
InputPath=.\Stage\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Sp"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Authenticate___Win32_Debug_Chn_Sp"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Debug_Chn_Sp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i "..\..\..\Chn-Sp" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\IcadAuth.dll
InputPath=.\Debug\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Td"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Authenticate___Win32_Release_Chn_Td"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Release_Chn_Td"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /D "NDEBUG" /D "_AFXDLL" /D "AUTH_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x404 /i "..\..\..\Chn-Td" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/Authenticate.bsc"
# ADD BSC32 /nologo /o"Debug/Authenticate.bsc"
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Release/IcadAuth.dll" /libpath:"Release"
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x51680000" /subsystem:windows /dll /machine:I386 /out:"Stage/IcadAuth.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Stage\IcadAuth.dll
InputPath=.\Stage\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Td"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Authenticate___Win32_Debug_Chn_Td"
# PROP BASE Intermediate_Dir "Authenticate___Win32_Debug_Chn_Td"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x404 /i "..\..\..\Chn-Td" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Debug/IcadAuth.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\IcadAuth.dll
InputPath=.\Debug\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 True Time"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"True_Time/IcadAuth.dll" /pdbtype:sept /libpath:"True_Time"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"True_Time/IcadAuth.dll" /pdbtype:sept /libpath:"True_Time"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\True_Time
TargetPath=.\True_Time\IcadAuth.dll
InputPath=.\True_Time\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Bounds Checker"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\Authenticate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Bounds_Checker/IcadAuth.dll" /pdbtype:sept /libpath:"Bounds_Checker"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 icadlib.lib version.lib /nologo /base:"0x01000000" /subsystem:windows /dll /debug /machine:I386 /out:"Bounds_Checker/IcadAuth.dll" /pdbtype:sept /libpath:"Bounds_Checker"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Bounds_Checker
TargetPath=.\Bounds_Checker\IcadAuth.dll
InputPath=.\Bounds_Checker\IcadAuth.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Authenticate - Win32 Debug"
# Name "Authenticate - Win32 Release"
# Name "Authenticate - Win32 Browse"
# Name "Authenticate - Win32 Minimal Debug"
# Name "Authenticate - Win32 Release Deu_De"
# Name "Authenticate - Win32 Release Fra_Fr"
# Name "Authenticate - Win32 Release Esl_Es"
# Name "Authenticate - Win32 Release Ita_It"
# Name "Authenticate - Win32 Release Jpn_Jp"
# Name "Authenticate - Win32 Debug Jpn_Jp"
# Name "Authenticate - Win32 Release Chn_Sp"
# Name "Authenticate - Win32 Debug Chn_Sp"
# Name "Authenticate - Win32 Release Chn_Td"
# Name "Authenticate - Win32 Debug Chn_Td"
# Name "Authenticate - Win32 True Time"
# Name "Authenticate - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Authenticate\Authenticate.cpp

!IF  "$(CFG)" == "Authenticate - Win32 Debug"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Browse"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Minimal Debug"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Deu_De"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Fra_Fr"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Esl_Es"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Jpn_Jp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Sp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Td"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 True Time"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Authenticate\Authenticate.def
# End Source File
# Begin Source File

SOURCE=..\Authenticate\Authenticate.rc
# End Source File
# Begin Source File

SOURCE=..\authenticate\Authenticator.cpp
# End Source File
# Begin Source File

SOURCE=..\Authenticate\memleaks.cpp

!IF  "$(CFG)" == "Authenticate - Win32 Debug"

# ADD CPP /D "_MBCS"
# SUBTRACT CPP /I "..\lib\icadlib" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /YX

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Browse"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Deu_De"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Fra_Fr"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Esl_Es"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Jpn_Jp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Sp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Td"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 True Time"

# ADD BASE CPP /D "_MBCS"
# SUBTRACT BASE CPP /I "..\lib\icadlib" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /YX
# ADD CPP /D "_MBCS"
# SUBTRACT CPP /I "..\lib\icadlib" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /YX

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Bounds Checker"

# ADD BASE CPP /D "_MBCS"
# SUBTRACT BASE CPP /I "..\lib\icadlib" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /YX
# ADD CPP /D "_MBCS"
# SUBTRACT CPP /I "..\lib\icadlib" /D "_USRDLL" /D "AUTH_DLL" /D "_WINDLL" /D "_AFXDLL" /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Authenticate\StdAfx.cpp

!IF  "$(CFG)" == "Authenticate - Win32 Debug"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Browse"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Minimal Debug"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Deu_De"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Fra_Fr"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Esl_Es"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Jpn_Jp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Sp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Release Chn_Td"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 True Time"

!ELSEIF  "$(CFG)" == "Authenticate - Win32 Bounds Checker"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Authenticate\Authenticator.h
# End Source File
# Begin Source File

SOURCE=..\Authenticate\authrc.h
# End Source File
# Begin Source File

SOURCE=..\authenticate\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\res\icadbeta_splash.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IcadSplash.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\IcadSplashLite.bmp
# End Source File
# End Group
# End Target
# End Project

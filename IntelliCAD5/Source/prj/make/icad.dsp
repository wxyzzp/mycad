# Microsoft Developer Studio Project File - Name="icad" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=icad - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "icad.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "icad.mak" CFG="icad - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "icad - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Browse" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Minimal Debug" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Release Deu_De" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Release Fra_Fr" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Release Esl_Es" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Release Ita_It" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Release Jpn_Jp" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Debug Jpn_Jp" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Release Chn_Sp" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Debug Chn_Sp" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Release Chn_Td" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Debug Chn_Td" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 True Time" (based on "Win32 (x86) Application")
!MESSAGE "icad - Win32 Bounds Checker" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "icad - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /out:"Stage/icad.exe" /delayload:db.dll /delayload:gr.dll /MAPINFO:LINES /opt:ref,icf
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Eng-us\icad.tip Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR /YX"Icad.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"Debug\Icad.res" /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Eng-us\icad.tip Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Browse"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "icad___W"
# PROP BASE Intermediate_Dir "icad___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\sds" /I "..\icad" /I "..\lib\gr" /I "..\lib\db" /I "..\lib\drw" /I "c:\vba5sdk\include" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "DEBUG" /D "_DEBUG" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /Fr /YX"Icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "....\..\ThirdParty\geo" /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "_DEBUG" /D "INTELLICAD" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"Debug\Icad.res" /d "_DEBUG"
# ADD RSC /l 0x409 /fo"Debug\Icad.res" /i ".\..\..\..\Eng-Us" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Ctl3d32.lib Opengl32.lib Glu32.lib GlAux.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /profile /map
# ADD LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "icad - Win32 Minimal Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___1"
# PROP BASE Intermediate_Dir "icad___1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug\icad"
# PROP Intermediate_Dir "MinDebug\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\api\sds" /I "..\icad" /I "..\lib\gr" /I "..\lib\db" /I "..\lib\drw" /I "c:\vba5sdk\include" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "NDEBUG" /D "MINDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 shdw32m.lib Ctl3d32.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /out:"Stage\icad.exe"
# SUBTRACT BASE LINK32 /profile /map /debug
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libcmt" /out:"MinDebug\icad.exe" /delayload:db.dll /delayload:gr.dll

!ELSEIF  "$(CFG)" == "icad - Win32 Release Deu_De"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___Win32_Release_Deu_De"
# PROP BASE Intermediate_Dir "icad___Win32_Release_Deu_De"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\deu-de"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad\deu-de"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Deu-De" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /i ".\..\..\..\Deu-De" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/icad.bsc"
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib icad.res /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Release\cmds.lib Release\dcl.lib Release\gr.lib Release\db.lib Release\icadutils.lib Release\icadlib.lib Release\DCL.lib Release\lisp.lib Release\flyoversnap.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "icad - Win32 Release Fra_Fr"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___Win32_Release_Fra_Fr"
# PROP BASE Intermediate_Dir "icad___Win32_Release_Fra_Fr"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\Fra-Fr"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad\Fra-Fr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Fra-Fr" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i ".\..\..\..\Fra-Fr" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/icad.bsc"
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib icad.res /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Release\cmds.lib Release\dcl.lib Release\gr.lib Release\db.lib Release\icadutils.lib Release\icadlib.lib Release\DCL.lib Release\lisp.lib Release\flyoversnap.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "icad - Win32 Release Esl_Es"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___Win32_Release_Esl_Es"
# PROP BASE Intermediate_Dir "icad___Win32_Release_Esl_Es"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\Esl-Es"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad\Esl-Es"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "./../../../Fra-Fr" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Esl-Es" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc0a /i ".\..\..\..\Esl-Es" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/icad.bsc"
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 icad.res delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib Release\geo.lib Release\dcl.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 icad.res delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Release\cmds.lib Release\dcl.lib Release\gr.lib Release\db.lib Release\icadutils.lib Release\icadlib.lib Release\DCL.lib Release\lisp.lib Release\flyoversnap.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=echo Compiling Icad resoures	rc /fo icad.res /i ../icad/res/ ./../../../Esl-Es/icad.rc
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Release Ita_It"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___Win32_Release_Ita_It"
# PROP BASE Intermediate_Dir "icad___Win32_Release_Ita_It"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Ita-It" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Ita-It" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x411 /i ".\..\..\..\Ita-It" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/icad.bsc"
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Release\cmds.lib Release\dcl.lib Release\gr.lib Release\db.lib Release\IcadAuth.lib Release\icadutils.lib Release\icadlib.lib Release\DCL.lib Release\lisp.lib Release\flyoversnap.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /out:"Stage/icad.exe" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Ita-it\icad.tip Stage	copy Release\icad.lib Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Release Jpn_Jp"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___Win32_Release_Jpn_Jp"
# PROP BASE Intermediate_Dir "icad___Win32_Release_Jpn_Jp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Jpn-Jp" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Jpn-Jp" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x411 /i ".\..\..\..\Jpn-Jp" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/icad.bsc"
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Release\cmds.lib Release\dcl.lib Release\gr.lib Release\db.lib Release\IcadAuth.lib Release\icadutils.lib Release\icadlib.lib Release\DCL.lib Release\lisp.lib Release\flyoversnap.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /out:"Stage/icad.exe" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Jpn-jp\icad.tip Stage	copy Release\icad.lib Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Jpn_Jp"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "icad___Win32_Debug_Jpn_Jp"
# PROP BASE Intermediate_Dir "icad___Win32_Debug_Jpn_Jp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "./../../../Eng-us" /I "..\lib\flyoversnap" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /YX"Icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "./../../../Jpn-Jp" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /YX"Icad.h" /FD /c
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"Debug\Icad.res" /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x411 /fo"Debug\Icad.res" /i ".\..\..\..\Jpn-Jp" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Debug\cmds.lib Debug\dcl.lib Debug\gr.lib Debug\db.lib Debug\IcadAuth.lib Debug\icadutils.lib Debug\icadlib.lib Debug\DCL.lib Debug\lisp.lib Debug\flyoversnap.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to
PostBuild_Cmds=copy ..\..\..\Jpn-jp\icad.tip Debug	copy Release\icad.lib Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Sp"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___Win32_Release_Chn_Sp"
# PROP BASE Intermediate_Dir "icad___Win32_Release_Chn_Sp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Chn-Sp" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Chn-Sp" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i ".\..\..\..\Chn-Sp" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/icad.bsc"
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Release\cmds.lib Release\dcl.lib Release\gr.lib Release\db.lib Release\IcadAuth.lib Release\icadutils.lib Release\icadlib.lib Release\DCL.lib Release\lisp.lib Release\flyoversnap.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /out:"Stage/icad.exe" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Chn-Sp\icad.tip Stage	copy Release\icad.lib Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Sp"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "icad___Win32_Debug_Chn_Sp"
# PROP BASE Intermediate_Dir "icad___Win32_Debug_Chn_Sp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "./../../../Eng-us" /I "..\lib\flyoversnap" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /YX"Icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "./../../../Chn-Sp" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /YX"Icad.h" /FD /c
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"Debug\Icad.res" /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /fo"Debug\Icad.res" /i ".\..\..\..\Chn-Sp" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Debug\cmds.lib Debug\dcl.lib Debug\gr.lib Debug\db.lib Debug\IcadAuth.lib Debug\icadutils.lib Debug\icadlib.lib Debug\DCL.lib Debug\lisp.lib Debug\flyoversnap.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to
PostBuild_Cmds=copy ..\..\..\Chn-Sp\icad.tip Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Td"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icad___Win32_Release_Chn_Td"
# PROP BASE Intermediate_Dir "icad___Win32_Release_Chn_Td"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Chn-Td" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "./../../../Chn-Td" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"Icad.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x404 /i ".\..\..\..\Chn-Td" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/icad.bsc"
# ADD BSC32 /nologo /o"Debug/icad.bsc"
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Release\cmds.lib Release\dcl.lib Release\gr.lib Release\db.lib Release\IcadAuth.lib Release\icadutils.lib Release\icadlib.lib Release\DCL.lib Release\lisp.lib Release\flyoversnap.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc" /out:"Stage/icad.exe" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Chn-Td\icad.tip Stage	copy Release\icad.lib Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Td"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "icad___Win32_Debug_Chn_Td"
# PROP BASE Intermediate_Dir "icad___Win32_Debug_Chn_Td"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "./../../../Eng-us" /I "..\lib\flyoversnap" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /YX"Icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "./../../../Chn-Td" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /YX"Icad.h" /FD /c
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /fo"Debug\Icad.res" /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x404 /fo"Debug\Icad.res" /i ".\..\..\..\Chn-Td" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib Debug\cmds.lib Debug\dcl.lib Debug\gr.lib Debug\db.lib Debug\IcadAuth.lib Debug\icadutils.lib Debug\icadlib.lib Debug\DCL.lib Debug\lisp.lib Debug\flyoversnap.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to
PostBuild_Cmds=copy ..\..\..\Chn-Td\icad.tip Debug	copy Release\icad.lib Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 True Time"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR /YX"Icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR /YX"Icad.h" /FD /c
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"True_Time\Icad.res" /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Eng-us\icad.tip True_Time	copy True_Time\icad.lib Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "icad - Win32 Bounds Checker"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\icad"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR /YX"Icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "./../../../Eng-us" /I "..\lib\auto" /I "..\authenticate" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\icad" /I "..\lib\gr" /I "..\lib\drw" /I "..\..\ThirdParty\Wintab\include" /I "..\..\ThirdParty\Geo" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\IcadUtils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR /YX"Icad.h" /FD /c
# ADD BASE MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "c:\vba5sdk\include" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"Bounds_Checker\Icad.res" /i ".\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 GlAux.lib delayimp.lib Ctl3d32.lib Version.lib Opengl32.lib Glu32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy files to Stage
PostBuild_Cmds=copy ..\..\..\Eng-us\icad.tip Bounds_Checker	copy Bounds_Checker\icad.lib Stage
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "icad - Win32 Release"
# Name "icad - Win32 Debug"
# Name "icad - Win32 Browse"
# Name "icad - Win32 Minimal Debug"
# Name "icad - Win32 Release Deu_De"
# Name "icad - Win32 Release Fra_Fr"
# Name "icad - Win32 Release Esl_Es"
# Name "icad - Win32 Release Ita_It"
# Name "icad - Win32 Release Jpn_Jp"
# Name "icad - Win32 Debug Jpn_Jp"
# Name "icad - Win32 Release Chn_Sp"
# Name "icad - Win32 Debug Chn_Sp"
# Name "icad - Win32 Release Chn_Td"
# Name "icad - Win32 Debug Chn_Td"
# Name "icad - Win32 True Time"
# Name "icad - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "NotPrecompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ICAD\EntityRenderer.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\Sds5.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_alert.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_findfile.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_geom.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_resbuf.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\DB\sds_wcmatch.cpp
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Source File

SOURCE=..\icad\AppAuthenticate.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\BHatchBoundaryTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\BhatchDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\BHatchPatPropTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\BHatchPatternTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\BinChange.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\Change.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\ColorComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Icad\COMAddin.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\COMAddInManager.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\COMAddinManagerDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\commandqueue.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\ContextIOToolBar.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\CWinTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DDOpenGL.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DDPlaneBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogGroup.cpp
# ADD CPP /IE:\IntelliCAD\IntelliCAD\Eng-us
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogGroupHLight.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogGroupOrder.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogHyperlink.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogPassword.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DictionaryChange.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DimBitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DimStyleComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DlgSpoolingSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DocAutoSds.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DrawDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DrawSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DropActions.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\DwfInfoDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\entityfilter.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\ExplorerPropsTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\ExternalEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\framebuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\GroupChange.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\HatchChange.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icad.rc
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAboutDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAccelEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAlias.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadApi.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadApp.cpp

!IF  "$(CFG)" == "icad - Win32 Release"

# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Debug"

!ELSEIF  "$(CFG)" == "icad - Win32 Browse"

!ELSEIF  "$(CFG)" == "icad - Win32 Minimal Debug"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "icad - Win32 Release Deu_De"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /Z<none>
# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Release Fra_Fr"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /Z<none>
# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Release Esl_Es"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /Z<none>
# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Release Ita_It"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /Z<none>
# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Release Jpn_Jp"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /Z<none>
# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Sp"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /Z<none>
# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Td"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /Z<none>
# ADD CPP /Od
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "icad - Win32 True Time"

!ELSEIF  "$(CFG)" == "icad - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAppLoad.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAttDefDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAtteDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAttExtDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadBitmaps.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadBoundaryDia.cpp

!IF  "$(CFG)" == "icad - Win32 Release"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug"

!ELSEIF  "$(CFG)" == "icad - Win32 Browse"

# ADD CPP /I "..\..\ThirdParty\geo"
# SUBTRACT CPP /I "....\..\ThirdParty\geo"

!ELSEIF  "$(CFG)" == "icad - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Deu_De"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Fra_Fr"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Esl_Es"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Jpn_Jp"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Sp"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Td"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "icad - Win32 True Time"

!ELSEIF  "$(CFG)" == "icad - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadChildWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCmdBar.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCntrItem.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadColorDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCommandLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadConfigDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCursor.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusButton.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusMenuAdvDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusMenuTree.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusShortcuts.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCustom.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusToolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusToolbar1.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusToolBarAdvDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icaddialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadDoc.cpp

!IF  "$(CFG)" == "icad - Win32 Release"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug"

!ELSEIF  "$(CFG)" == "icad - Win32 Browse"

!ELSEIF  "$(CFG)" == "icad - Win32 Minimal Debug"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "icad - Win32 Release Deu_De"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Fra_Fr"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Esl_Es"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Jpn_Jp"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Sp"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Td"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "icad - Win32 True Time"

!ELSEIF  "$(CFG)" == "icad - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadDockBar.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadDockContext.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadDragging.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icaddraw.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icaddrawapi.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icaddrawbitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadEditXdata.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadEntityRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadEntPropMain.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadEntPropTabs.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadexplorer.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadFlyOut.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadFontComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadFontLangCombo.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadfontstylecombo.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadform.cpp

!IF  "$(CFG)" == "icad - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Release Deu_De"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Release Fra_Fr"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Release Esl_Es"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Release Ita_It"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Release Jpn_Jp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Jpn_Jp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Sp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Sp"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Td"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Td"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadGlobal.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadgridiconsnap.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadGrips.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadgroupdia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadGuids.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadInsertDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadInsName.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadIpFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadLWCombo.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadLWComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadLWListBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMail.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMain.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMenuMain.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMleditDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMTextDia.cpp

!IF  "$(CFG)" == "icad - Win32 Release"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug"

!ELSEIF  "$(CFG)" == "icad - Win32 Browse"

!ELSEIF  "$(CFG)" == "icad - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Deu_De"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Fra_Fr"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Esl_Es"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Ita_It"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Jpn_Jp"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Jpn_Jp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Sp"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Sp"

!ELSEIF  "$(CFG)" == "icad - Win32 Release Chn_Td"

!ELSEIF  "$(CFG)" == "icad - Win32 Debug Chn_Td"

!ELSEIF  "$(CFG)" == "icad - Win32 True Time"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "icad - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadNewWiz.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadOleLinksDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadPMSpaceDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadPromptMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadPromptWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadraster.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadRectTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadRenameApp.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadRunForm.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadSelectDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadSetvarsDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadSpell.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadSplash.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadSrvrItem.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadStdde.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadStylePreview.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTablet.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTBDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTextScrn.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTipDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadToleranceDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadToolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadToolBarMain.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadToolTip.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadUcspDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadVbaWorkspace.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadview.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadViewCtl.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadVpointDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadWndAction.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadXLink.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadXrefDia.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadZoomView.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IdBufferChange.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\InputEditBoxDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\inputeventfilter.cpp
# End Source File
# Begin Source File

SOURCE=..\Icad\inputeventqueue.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IntCmds.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayerComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayoutInsert.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayoutRename.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayoutTabs.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\LicensedComponents.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\LTypeComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\LWeightComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\menubuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextPropertiesTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextRichEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextStyleBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextTextTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionPathListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsCrosshairsTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsDisplayTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsFuncts.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsGeneralTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsPathsTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsSnapDlgPic.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsSnappingTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsSnappingTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\paths.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\penmaplistctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\Preferences.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintAdvancedTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Icad\PrinterDrawDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintPenMapTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintScaleViewTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\realtime.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\resbufs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_apis.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_command.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_conversion.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_db.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_drawing.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_entryexit.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_get.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_image.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_invoke.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_misc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_osnap.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_printf.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_selectset.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_vars.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_xform.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_xref.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsapplication.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsthread.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsthreadjobsink.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\sizecbar.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\TextStyleDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\TextStyleTableInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\threadcontroller.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\threadcore.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\IcadUtils\TransformUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\UCSIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\ValueValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\vmldrawdevice.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\ICAD\BHatchBoundaryTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\BhatchDialog.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\BHatchPatPropTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\BHatchPatternTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\binchange.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\ColorComboBox.h
# End Source File
# Begin Source File

SOURCE=..\Icad\COMAddin.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\COMAddInManager.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\COMAddinManagerDlg.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\commandqueue.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\configure.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\ContextIOToolBar.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\CSdsName.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DDOpenGL.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DDPlaneBuffer.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogGroup.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\dialoggrouphlight.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\dialoggrouporder.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogHyperlink.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DialogPassword.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DictionaryChange.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DimStyleComboBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DlgSpoolingSettings.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DrawDevice.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DrawSurface.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DropActions.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\DwfInfoDialog.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\EntityRenderer.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\ExplorerPropsTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\ExplorerSubDlg.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\framebuffer.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\GroupChange.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\HatchChange.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\icad.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadAccelEdit.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadApi.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadApp.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\IcadATL.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadChildWnd.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCmdBar.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCntrItem.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCursor.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusButton.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusKeyboard.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusMenu.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusShortcuts.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCustom.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadCusToolbar.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\icaddialog.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadDoc.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadEntityRenderer.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadEntPropTabs.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadExplorer.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadFontComboBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadfontstylecombo.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadform.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\IcadGlobals.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadGPS.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\icadgridiconsnap.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadHelp.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadHelpers.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadIpFrame.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Icadlib\Icadlib.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadLWComboBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadLWListBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMain.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadMenu.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadNewWiz.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadPromptMenu.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadPromptWnd.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadSplash.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadSrvrItem.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadStatusBar.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadStylePreview.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTBDlg.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTextScrn.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTipDlg.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadToolBar.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadToolBarMain.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadToolTip.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadTrialDlg.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadUtil.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadVbaAddin.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadVbaCommProj.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadVbaMacro.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadVbaWorkspace.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadView.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadXlink.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IdBufferChange.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\InputEditBoxDlg.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\inputevent.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\inputeventfilter.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\inputeventqueue.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\IntCmds.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayerComboBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayoutInsert.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayoutRename.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\LayoutTabs.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\Sheriff\Licdefs.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\LicensedComponents.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\Sheriff\Licscode.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\Sheriff\Lictype.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\LTypeComboBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\LWeightComboBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\MapChange.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextPropertiesTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextPropSheet.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextRichEdit.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextStyleBox.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\MTextTextTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionPathListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsCrosshairsTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsDisplayTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsFuncts.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsGeneralTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsPathsTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsPropSheet.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\OptionsSnapDlgPic.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\paths.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\penmaplistctrl.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\Preferences.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintAdvancedTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintDialog.h
# End Source File
# Begin Source File

SOURCE=..\Icad\PrinterDrawDevice.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintPenMapTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\PrintScaleViewTab.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\rasterconstants.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\realtime.h
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_engine.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsapplication.h
# End Source File
# Begin Source File

SOURCE=..\sdsapplication.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsthread.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsthreadexception.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsthreadhandler.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsthreadjob.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\sdsthreadjobsink.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\SyncObject.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\TextStyleDlg.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\TextStyleTableInfo.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\UCSIcon.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\ValueValidator.h
# End Source File
# Begin Source File

SOURCE=..\ICAD\vmldrawdevice.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\RES\ABOUTPIC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\all.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWL_RTPAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWL_RTROT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWL_RTROTX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWL_RTROTY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWL_RTROTZ.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWL_RTZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWS_RTPAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWS_RTROT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWS_RTROTX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWS_RTROTY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWS_RTROTZ.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\BWS_RTZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\center.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\co_blue.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\co_cyan.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\co_green.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\col_vari.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\col_varies.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\cross.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\cross.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\cross95.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\crossbox1.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\crossbox2.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\crossbox3.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\crossing.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\crossing.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\CUBIC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\cur00001.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\cur00002.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\cursor1.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor10.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor11.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor3.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor4.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor5.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor6.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor7.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor8.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\cursor9.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\custflyouth.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\custflyouth.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\custflyouthr.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\custflyouthr.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\custflyoutv.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\custflyoutv.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\custflyoutvr.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\custflyoutvr.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\custtoolbar.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\custtoolbar.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\db_icad_.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_closed_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_closed_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_closedfilled_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_closedfilled_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_custom_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_custom_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_dot_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_dot_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_none_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_none_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_oblique_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_oblique_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_open_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_open_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_origin_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_origin_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_rightangle_first.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarrow_rightangle_second.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_archtick_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_archtick_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_boxblank_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_boxblank_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_boxfilled_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_boxfilled_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_closed_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_closed_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_closedblank_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_closedblank_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_closedfilled_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_closedfilled_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_custom_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_custom_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_datumblank_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_datumblank_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_datumfilled_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_datumfilled_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_dot_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_dot_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_dotblank_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_dotblank_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_dotsmall_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_dotsmall_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_integral_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_integral_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_none_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_none_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_oblique_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_oblique_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_open30_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_open30_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_open90_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_open90_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_open_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_open_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_origin_1st.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\dimarw_origin_2nd.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_color0.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_color256.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\EC_donut.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode0.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode100.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode32.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode33.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode34.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode35.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode36.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode64.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode65.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode66.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode67.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode68.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode96.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode97.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode98.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmode99.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmodecircle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmodesc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pdmodesquare.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\EC_PLIN2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_pline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ec_skpol.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\EC_SKPOLY2.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\em_cham1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\em_cham2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\em_cham3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\em_cham4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\em_mirrr.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\em_mirrt.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\empty.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\empty.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\endpoint.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\eraser.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\eraser.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_blocks.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_blocks.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_blocks1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\EXP_CHECK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_del_.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_dim.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_dims.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\exp_dimsty.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\exp_dimsty1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_form.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_helpmark.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_layer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_layer.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_layer1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_layer_tbar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_LAYER_TOOLS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_ltype.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_ltype.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_ltype1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_new.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_proj.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_styles.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_styles.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_styles1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_tackdown.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_tackup.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_tbar16.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\EXP_TBAR24.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_tbb2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_tbbw16.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_ucs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_ucs.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_ucs1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_view.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_view.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_view1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_x.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exp_xlink.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\expblocktb.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\expbwtbar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\expbwtbarlg.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Expfdiagr1.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\Expfdiagr1.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\Expfdiagr2.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\Expfdiagr2.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\Expfhorr.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\Expfhorr.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\Expfmove.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\Expfmove.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\ExpfSelect.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\ExpfSelect.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\Expfverr.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\Expfverr.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\ExpTbar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\exptbarlg.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\font_shp.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\font_shx.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\font_ttf.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\FORMTBAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\frm_widt.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\FRMTBARM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\frmtlcorner.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\grabhand.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\grabhand.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\hatchignore.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\hatchnest.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\hatchouter.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\helpbtn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\HelpIcon.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\HelpIcon.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\ICAD\Icad.ico
# End Source File
# Begin Source File

SOURCE=..\ICAD\ICAD.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\icad_att.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\icad_cop.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\icad_han.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\icad_han.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\ICAD_IDB_LG_PRINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\icad\IcadDoc.ico
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadDoc.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\IcadSplash.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ico00001.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\ico00002.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\..\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_BWS_ICAD_ARC_CENTER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_ARC_ASCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_ARC_ASCL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_ATTDEF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_attext.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_backgrounds.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bhatch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bool_int.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bool_sub.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bool_unn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bpoly.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ABOUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ACSA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ACSE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_acsl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_asac.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_asae.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ASCA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ASCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ASCL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_asde.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ASEA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_asec.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ASED.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ARC_ASER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_asra.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_asre.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_arc_tangent.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_AREA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ATTD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ATTDISP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ATTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_attext.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ATTXT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_backgrounds.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_BASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_bhatch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_block.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_bool_int.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_bool_sub.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_bool_unn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_bpoly.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_BREAK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CFG.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CHPP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CIRC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_circle_arc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CIRCLE_C2P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CIRCLE_C3P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CIRCLE_CCD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CIRCLE_CCR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CIRCLE_CTTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_clear.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_COLOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_color_byblock.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_color_bylayer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CONAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_COPY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_COPY2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_copyedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_custom1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_custom2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_custom3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_custom4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_custom5.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_custom6.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_CUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DDATT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ddedit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DDEMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DDLMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DEL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_deledata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMAL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_dimap.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMB.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMRO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMRS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMSY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_dimtol.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIMVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DONUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_dview.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DXFIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_DXFOT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_edit_edata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_elarc_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_elarc_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_elarc_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_elarc_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ELEV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ELLIP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ellip_arcax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ellip_arcrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ellip_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ellip_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ellip_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ellip_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ERASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_EXIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_expblks.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bwl_expdim.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_EXPL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bwl_explore.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_expucs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_expviews.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_FILES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_FILL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_flatten.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_fullrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_generic.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_GPSLIVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_GPSPROP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_hatch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_HELP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_id.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_image.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_insert.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_join.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_layer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_lighting.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_LIMTS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_LINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_LIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ltype.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_lyrbyent.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_materials.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_MCR_PLAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_MCR_RECORD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_MCR_STOP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_MINS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_mledit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_mline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_mlstyle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_MOVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_moveedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_mtxt.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_mview.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwL_new.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_NGON.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ngon_edge.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ngon_side.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ngon_vert.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OFFST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ORTHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_os_int2d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_os_int3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_os_intvs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNAP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\iDB_ICAD_BWL_OSNIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNMI.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNNE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNNO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNOF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNPE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSNQU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_OSTAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_paint.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_PARAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_PASTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_PEDIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_PLAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_PLINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_pmspace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_point.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_PRINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bwl_qrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bwl_qrenderset.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_ray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_reassoc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_RECT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_REDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_redraw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_redrawall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_REGEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_regenall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_renamevar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_render.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_REVS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_RING.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ROTAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_rotate3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_SAVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_setrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_SETVR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_shaderen.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_SKETC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_solid.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_spell.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_spline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_STAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_style.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_text.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_thick.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_TIME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_TOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_trace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_TRIM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_U.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_UCS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_UNDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_vba.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_view.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_VPOIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_vports.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_VSLD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_vwctrl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_wblk.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_WCLOS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_WOPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_WSCAD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_WTILE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_xline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_xref.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_zoomin.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bwl_zoomout.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOMP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWL_ZOOMW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ABOUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC_ACSA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC_ACSE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_arc_acsl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_arc_asac.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_arc_asae.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC_ASCA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_arc_asde.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC_ASEA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_arc_asec.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC_ASED.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC_ASER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_arc_asra.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_arc_asre.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ARC_TANGENT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_AREA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ATTD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ATTDISP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ATTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_attext.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ATTXT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_backgrounds.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_BASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_bhatch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_block.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_bool_int.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_bool_sub.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_bool_unn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_bpoly.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_BREAK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CFG.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CHPP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CIRC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_circle_arc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CIRCLE_C2P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CIRCLE_C3P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CIRCLE_CCD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CIRCLE_CCR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CIRCLE_CTTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_clear.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_COLOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_color_byblock.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_color_bylayer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CONAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_COPY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_COPY2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_copyedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_custom1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_custom2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_custom3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_custom4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_custom5.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_custom6.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_CUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DDATT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ddedit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DDEMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DDLMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DEL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_deledata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMAL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_dimap.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMB.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMRO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMRS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMSY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_dimtol.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIMVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DONUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_dview.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DXFIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_DXFOT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_edit_edata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_elarc_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_elarc_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_elarc_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_elarc_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ELEV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ELLIP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ellip_arcax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ellip_arcrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ellip_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ellip_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ellip_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ellip_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ERASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_EXIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_expblks.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bws_expdim.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_EXPL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bws_explore.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_expucs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_expviews.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_FILES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_FILL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_flatten.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_fullrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_generic.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_GPSLIVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_GPSPROP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_hatch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_HELP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_id.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_image.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_insert.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_join.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_layer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_lighting.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_LIMTS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_LINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_LIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ltype.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_lyrbyent.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_materials.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_MCR_PLAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_MCR_RECORD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_MCR_STOP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_MINS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_mledit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_mline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_mlstyle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_MOVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_moveedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_mtxt.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_mview.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_NEW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_NGON.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ngon_edge.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ngon_side.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ngon_vert.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OFFST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ORTHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_os_int2d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_os_int3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_os_intvs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNAP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNMI.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNNE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNNO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNOF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNPE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSNQU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_OSTAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_paint.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_PARAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_PASTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_PEDIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_PLAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_PLINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_pmspace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_point.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bws_qrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_bws_qrenderset.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_ray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_reassoc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_RECT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_REDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_redraw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_redrawall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_REGEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_regenall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_renamevar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_render.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_REVS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_RING.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ROTAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_rotate3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_SAVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_setrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_SETVR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_shaderen.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_SKETC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_solid.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_spell.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_spline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_STAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_style.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_text.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_thick.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_TIME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_TOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_trace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_TRIM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_U.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_UCS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_UNDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_vba.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_view.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_VPOIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_vports.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_VSLD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_vwctrl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_wblk.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_WCLOS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_WOPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_WSCAD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_WTILE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_xline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_xref.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_zoomin.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_bws_zoomout.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOMP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_BWS_ZOOMW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_circle_arc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_clear.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_color_byblock.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_color_bylayer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_custom1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_custom2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_custom3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_custom4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_custom5.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_custom6.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_ddedit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_dimap.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_dimtol.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_edit_edata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_expblks.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_expdim.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_expucs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_expviews.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_flatten.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_fullrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_generic.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_join.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_arc_acsl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_arc_asac.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_arc_asae.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_arc_asde.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_arc_asec.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_arc_asra.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_arc_asre.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_attext.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_backgrounds.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_bhatch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_bool_int.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_bool_sub.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_bool_unn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_bpoly.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_circle_arc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_clear.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_color_byblock.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_color_bylayer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_LG_COPY2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_custom1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_custom2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_custom3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_custom4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_custom5.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_custom6.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_LG_CUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_ddedit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_dimap.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_dimtol.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_edit_edata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_expblks.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_lg_expdim.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_expucs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_expviews.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_flatten.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_fullrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_generic.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_join.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_lighting.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_lyrbyent.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_materials.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_mesh.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_mledit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_mline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_mlstyle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_mtxt.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_mview.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_ngon_edge.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_ngon_side_.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_ngon_vert.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_LG_OSTAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_paint.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_LG_PASTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_pmspace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_lg_qrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_lg_qrenderset.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_redraw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_redrawall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_regen.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_regenall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_render.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_rotate3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_LG_SAVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_setrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_shaderen.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_spell.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_spline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_thick.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_vba.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_vports.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_vwctrl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lg_xref.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lighting.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_lyrbyent.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_materials.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_mesh.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_mledit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_mline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_mlstyle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_mtxt.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_mview.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_ngon_edge.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_ngon_side.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_ngon_vert.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IDB_ICAD_OSTAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_paint.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_pmspace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_qrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\idb_icad_qrenderset.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_redraw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_redrawall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_regen.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_regenall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_render.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_rotate3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_setrender.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_shaderen.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_spell.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_spline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_thick.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_vba.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_vports.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_vwctrl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_xdata_all.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idb_icad_xref.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\idc_custtoolbaradd.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDC_CUSTTOOLBARADD.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\idc_custtoolbardel.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDC_CUSTTOOLBARDEL.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\idc_icad_os_int2d.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\idc_icad_os_int3d.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\idc_icad_os_intvs.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_dcl_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_DCL_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_dxf_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_DXF_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_ica_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_ICA_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_ick_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_ICK_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_icm_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_ICM_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_lin_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_LIN_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_lsp_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_LSP_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_lwi_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_LWI_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_pat_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_PAT_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_scr_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_SCR_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_sld_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_SLD_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_unt_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_UNT_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\idi_vbi_file.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\IDI_VBI_FILE.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\iIDB_ICAD_BWS_PRINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\intersect.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\IToolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lestest16.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lestest23.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lestest24.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Lg_.DIME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_2darray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_3darray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_3DFAC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_3DMES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_3dmirror.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_3dpoly.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ABOUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_APPER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ACSA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ACSE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ASCA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ASCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ASCL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ASEA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ASED.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_ASER_.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ARC_TANGENT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_AREA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ATTD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ATTDISP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ATTDP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ATTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ATTXT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_BASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_BLOCK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_blue.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_BOX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_BREAK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CFG.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CHAMF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CHNG.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CHPP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CIRC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CIRCLE_C2P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CIRCLE_C3P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CIRCLE_CCD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CIRCLE_CCR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CIRCLE_CTTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CLOSE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_COLOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CONAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CONE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_COPY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_copyedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_cyan.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_CYL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DDATT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DDEMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DDLMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DDUCS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DEL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_deledata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMAL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMB.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMRO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMRS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMSY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIMVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DISH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DIVID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DOME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DONUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DRMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DVIEW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DXFIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_DXFOT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_EDGES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_elarc_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_elarc_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_elarc_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_elarc_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ELEV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ELLIP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_ellip_arcax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_ellip_arcrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_ellip_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_ellip_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_ellip_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_ellip_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ERASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_EXIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_EXPL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\lg_explore.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_EXT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_FILES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_FILL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_FILLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_GPSLIVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_GPSMODE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_GPSPAUSE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_GPSPLAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_GPSPROP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_GPSSTOP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_green.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_GRID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_HATCH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_HELP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_HIDE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_image.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_INSER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_LAYER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_LENGT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_LIMTS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_LINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_LIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_LTYPE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_magenta.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MCRPLY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MCRREC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MCRSTP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MEAS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MENU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MINS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MIRR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MOVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_moveedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_MSLD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_NEW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_NGON.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OFFST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OOPS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ORTHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_os_int2d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_os_int3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_os_intvs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNAP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNMI.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNNE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNNO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNOF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNPE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_OSNQU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PARAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PEDIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PFACE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PLAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PLINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_POINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PRINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PSETP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_PYR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_ray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_RDRAW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_reassoc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_RECT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_red.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_REDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_REGEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_renamevar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_REVS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_RGENA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_RING.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ROTAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\LG_RTPAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\LG_RTROT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\LG_RTROTX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\LG_RTROTY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\LG_RTROTZ.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\LG_RTZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_RULE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SCALE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SCR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SETVR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SHADE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SKETC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SNAP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SOLID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_SPHER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_STAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_STRET.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_STYLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_TABSF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_TEST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_TEXT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_TIME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_TOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_TOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_trace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_TRIM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_U.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_UCS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_UCSI.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_UNDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_UNITS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_VIEW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_VPOIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_VPORT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_VSLD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_WBLK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_WCLOS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_WED.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_white.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_WOPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_WSCAD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_WTILE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_xline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_yellow.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_zoomin.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\lg_zoomout.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOMP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LG_ZOOMW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\midpoint.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_ADDVERTEX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_BLENDEDCROSS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_BLENDEDTEE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_CLOSEDCROSS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_CLOSEDTEE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_CORNERJOINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_CUTALL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_CUTSINGLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_DELETEVERTEX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_OPENCROSS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_OPENTEE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ML_WELDALL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\near.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\New1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\node.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\NOFRAME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\olemove.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\olemove.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\openhand.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\openhand.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\OUTER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\outside.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\outside.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pencil.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pencil.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\perpen.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\perspective.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PERSPECTIVE.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick1.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK1.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick10.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK10.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick10A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick10A.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick1A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick1A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick2.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK2.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick2A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick2A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick3.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK3.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick3A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick3A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick4.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK4.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick4A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick4A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick5.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK5.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick5A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick5A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick6.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK6.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick6A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick6A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick7.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK7.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick7A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick7A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick8.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK8.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick8A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick8A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pick9.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\PICK9.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pick9A.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pick9A.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\pickbox.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pickbox.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\pickboxA.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\pickboxA.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\popbutton.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\props_lg_bw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\props_lg_clr.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\props_sm_bw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\props_sm_clr.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\QUAD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\quad.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\rt_cyl_x.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\rt_cyl_x.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\rt_cyl_y.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\rt_cyl_y.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\rt_cyl_z.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\rt_cyl_z.cur
# End Source File
# Begin Source File

SOURCE=..\..\res\rt_sphere.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\rt_sphere.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_2darray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_3darray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_3DFAC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_3DMES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_3dmirror.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_3dpoly.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ABOUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_APPER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_ACSA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_ACSE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_acsl.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_asac.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_asae.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_ASCA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_ASCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_ASCL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_asde.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_ASEA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_asec.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_ASED.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_ASER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_asra.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_asre.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ARC_CENTER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_arc_tangent.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_AREA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ATTD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ATTDISP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ATTDP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ATTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ATTXT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_BASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_BLOCK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_blue.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_BOX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_BREAK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CFG.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CHAMF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CHNG.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CHPP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CIRC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CIRCLE_C2P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CIRCLE_C3P.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CIRCLE_CCD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CIRCLE_CCR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CIRCLE_CTTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_COLOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CONAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CONE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_COPY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Sm_copy2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_copyedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_cyan.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_CYL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DDATT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DDEMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DDLMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DEL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_deledata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMAL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMB.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMRO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMRS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMSY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMTR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIMVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DISH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DIVID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DOME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DONUT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DRMD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DVIEW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DXFIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_DXFOT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_EDGES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_elarc_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_elarc_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_elarc_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_elarc_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ELEV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ELLIP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_ellip_arcax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_ellip_arcrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_ellip_axax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_ellip_axrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_ellip_cenax.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_ellip_cenrot.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ERASE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_EXIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_EXPL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\sm_explore.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_EXT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_FILES.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_FILL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_FILLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_GPSLIVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_GPSMODE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_GPSPAUSE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_GPSPLAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_GPSPROP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_GPSSTOP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_green.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_GRID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_HATCH.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_HELP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_HIDE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_image.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_INSER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_LAYER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_LENGT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_LIMTS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_LINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_LIST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_LTYPE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_magenta.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MCRPLY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MCRREC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MCRSTP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MEAS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MENU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MINS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MIRR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MOVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_moveedata.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_MSLD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_NEW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_NGON.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OFFST.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ORTHO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_os_int2d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_os_int3d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_os_intvs.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNAP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNCE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNMI.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNNE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNNO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNOF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNPE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_OSNQU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PARAY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PASTE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PEDIT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PFACE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PLAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PLINE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_POINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PRINT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_PYR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_ray.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_reassoc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_RECT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_red.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_REDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_REGEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_renamevar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_REVS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_RING.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ROTAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\SM_RTPAN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\SM_RTROT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\SM_RTROTX.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\SM_RTROTY.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\SM_RTROTZ.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\SM_RTZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_RULE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SAVE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SCALE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SCR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SETVR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SHADE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SKETC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SNAP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SOLID.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_SPHER.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_STAT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_STRET.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_STYLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_TABSF.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_tex.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_TIME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_TOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_TOR.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_trace.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_TRIM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_U.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_UCS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_UCSI.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_UNDO.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_UNITS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_VIEW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_VPOIN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_VPORT.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_VSLD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_WBLK.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_WCLOS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_WED.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_white.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_WOPEN.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_WSCAD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_WTILE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_xline.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_yellow.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOLE.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOM.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOMA.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOMC.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_zoomin.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOML.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sm_zoomout.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOMP.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOMS.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOMV.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SM_ZOOMW.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_center.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_center.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_endpoint.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_endpoint.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_insertion.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_insertion.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_int2d.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\SNAP_INT2D.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_int3d.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\SNAP_INT3D.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_intersect.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_intersect.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_intvs.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\SNAP_INTVS.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_midpoint.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_midpoint.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_multi.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_multi.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_near.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_near.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_node.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_node.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_perp.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_perp.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_quad.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_quad.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\snap_tangent.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\snap_tangent.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\STANDARD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Test.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\text_bol.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\text_bol.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\text_bol.ico
# End Source File
# Begin Source File

SOURCE=..\..\res\text_ita.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\text_ita.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\text_ita.ico
# End Source File
# Begin Source File

SOURCE=..\..\res\text_und.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\text_und.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\text_und.ico
# End Source File
# Begin Source File

SOURCE=..\..\res\text_undo.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\text_undo.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_a.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_b.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_blank.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_c.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_d.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_e.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_f.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_g.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_h.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_i.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_j.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_k.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_l.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_m.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_n.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_p.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_r.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_s.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_t.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tol_u.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ucsp_back.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ucsp_bottom.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ucsp_front.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ucsp_left.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ucsp_right.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ucsp_top.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\USEFRAME.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_pan_down.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_pan_left.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_pan_right.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_pan_up.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_x_neg.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_x_pos.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_y_neg.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_y_pos.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_z_neg.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vctl_z_pos.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\visindicator.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\visio_web_lg.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\visio_web_lg_bw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\visio_web_sm.bmp
# End Source File
# Begin Source File

SOURCE=..\..\res\visio_web_sm_bw.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_back.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_botto.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_front.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_left.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct5.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct6.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct7.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_oct8.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_right.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\vp_top.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\window.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\window.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\wiz_new1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\wiz_new2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\wiz_new3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\wiz_new4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\wiz_new5.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\wizard.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_3dpoint.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_binary.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_control.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_distance.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_handle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_integer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_layer.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_long.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_real.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_scale.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_string.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_worlddir.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_worlddis.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\xdata_worldpos.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Xlink.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Xlink1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Xlink2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\zoom.cur
# End Source File
# Begin Source File

SOURCE=..\..\..\..\res\zoom.cur
# End Source File
# End Group
# Begin Source File

SOURCE=..\ICAD\IcadApi.def
# End Source File
# End Target
# End Project
# Section icad : {340A0CF0-23F4-11CF-870B-444553540000}
# 	0:8:Splash.h:C:\dev\PRJ\icad\Splash.h
# 	0:10:Splash.cpp:C:\dev\PRJ\icad\Splash.cpp
# 	1:10:IDB_SPLASH:107
# 	2:10:ResHdrName:resource.h
# 	2:11:ProjHdrName:stdafx.h
# 	2:10:WrapperDef:_SPLASH_SCRN_
# 	2:12:SplClassName:CSplashWnd
# 	2:21:SplashScreenInsertKey:4.0
# 	2:10:HeaderName:Splash.h
# 	2:10:ImplemName:Splash.cpp
# 	2:7:BmpID16:IDB_SPLASH
# End Section
# Section icad : {340A0CF4-23F4-11CF-870B-444553540000}
# 	2:9:CAboutDlg:1
# End Section

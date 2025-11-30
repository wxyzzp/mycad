# Microsoft Developer Studio Project File - Name="cmds" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cmds - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cmds.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cmds.mak" CFG="cmds - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cmds - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cmds - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "cmds - Win32 Browse" (based on "Win32 (x86) Static Library")
!MESSAGE "cmds - Win32 Minimal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "cmds - Win32 True Time" (based on "Win32 (x86) Static Library")
!MESSAGE "cmds - Win32 Bounds Checker" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cmds - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\cmds"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "NDEBUG" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"cmds.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\cmds"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"cmds.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\cmds.lib"

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cmds___W"
# PROP BASE Intermediate_Dir "cmds___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\cmds"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\icadlib" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\lib\sds" /I "..\icad" /I "..\lib\gr" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\geo" /D "SDS_SDSSTANDALONE" /D "DEBUG" /D "_DEBUG" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /YX"icad.h" /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "_DEBUG" /D "INTELLICAD" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR /YX"cmds.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cmds___W"
# PROP BASE Intermediate_Dir "cmds___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\cmds"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\api\sds" /I "..\icad" /I "..\lib\gr" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\geo" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"icad.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "NDEBUG" /D "MINDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"cmds.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\cmds"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"cmds.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"cmds.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\cmds"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"cmds.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\auto" /I "..\lib\cmds" /I "..\lib\dcl" /I "..\lib\lisp" /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "c:\vba5sdk\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\Icadutils" /I "..\lib\flyoversnap" /I "..\..\ThirdParty\HtmlHelp\include" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "INTELLICAD" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"cmds.h" /FD /c
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

# Name "cmds - Win32 Release"
# Name "cmds - Win32 Debug"
# Name "cmds - Win32 Browse"
# Name "cmds - Win32 Minimal Debug"
# Name "cmds - Win32 True Time"
# Name "cmds - Win32 Bounds Checker"
# Begin Group "Source"

# PROP Default_Filter "*.c;*.cpp"
# Begin Group "NotPrecompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\CMDS\undo\ParentUndoUnit.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\undo\UndoMgr.cpp
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\CMDS\3dface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\3dsolids.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\acisInOut.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\arc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\array.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\bhatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BHatchBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\Bhatchinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\block.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BoundaryBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BPolyBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\Candidate.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CandidateList.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\changeprop.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\circle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CloneHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmd_vplayer.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdCommands.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdCurveConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdExtrude.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdFilletChamfer.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdMassprop.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdOut.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdQueryTools.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdQueryTools.tpl
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdRegion.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdRevolve.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Cmds\Cmds3.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Cmds\Cmds4.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Cmds\Cmds8.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Cmds\Cmds9.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmds_misc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsAlign.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDim.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDimBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDimVars.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDraworder.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdSection.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsError.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsHatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsInet.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsLayout.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsLicence.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsNotUsed.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdSolidedit.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdSolidedit.inl
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdSolidedit.tpl
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsViewport.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsWBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Convcolr.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\coordinate.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\copy_and_move.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\ddcmd.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\ddedit.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimAng.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimAngA3P.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimContinue.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaCenterMark.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaDia.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaFunc.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaMain.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaRad.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaRad0.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaRad3.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaRad4.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimDiaRad5.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\dimensions.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimLinear.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimLinearFunc.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimMakeDim.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimOrdinate.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimStyle.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\DimTEdit.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\divide_and_measure.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\drag.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\dwgcodepage.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\dxfInOut.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\ellipse.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\entityData.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\explode.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\ExplodeLeader.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\flatten.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\GeoConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\hatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\hideAndShade.cpp
# End Source File
# Begin Source File

SOURCE=..\ICAD\IcadUndoRedo.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\insert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\layer.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\Leader.cpp

!IF  "$(CFG)" == "cmds - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "cmds - Win32 Debug"

# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Browse"

!ELSEIF  "$(CFG)" == "cmds - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "cmds - Win32 True Time"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ELSEIF  "$(CFG)" == "cmds - Win32 Bounds Checker"

# ADD BASE CPP /ZI /Od
# ADD CPP /ZI /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\line.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\line_or_circle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\listcoll.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Listdxf.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\mail.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\menu.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\mpSpace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\mview.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\offset.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\openClose.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\panZoom.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\paste.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\pedit2.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\polyline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\purge.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\read.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\realtimecommands.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\regen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\rename.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\render.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\save.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\scale_and_rotate.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\scripts.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\sdsTextbox.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\shape.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\sketch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\slide.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\solid_and_trace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\sortlist.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\sound.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\stretch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\surfaces.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\tablet.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\TanCircs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\text.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\textstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\todo.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\ucs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Cmds\undo.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\undo\UndoSaver.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\UpdateLeader.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\utilities.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\viewports.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\vslide.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\web.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\xclip.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\lib\CMDS\bhatch.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BHatchBuilder.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BHatchFactory.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BhatchInfo.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BoundaryBuilder.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\BPolyBuilder.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\Candidate.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CandidateList.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CloneHelper.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdCommand.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdCurveConstructor.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdOut.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdQueryTools.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmds.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsAlign.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDim.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDimBlock.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDimVars.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsDraworder.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsGroup.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsHatch.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsInet.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsLayout.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsLicence.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsNotUsed.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\CmdSolidedit.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsViewport.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\cmdsWBlock.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\dimensions.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\GeoConvert.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\HideAndShade.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\IcadGlobals.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Icadlib\Icadlib.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\IcadUndo.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\line_or_circle.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\listcoll.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\undo\ParentUndoUnit.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\TanCircs.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\undo\Undo.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\undo\UndoMgr.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\undo\UndoSaver.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\undo\UndoTLB.h
# End Source File
# Begin Source File

SOURCE=..\lib\CMDS\UpdateLeader.h
# End Source File
# End Group
# End Target
# End Project

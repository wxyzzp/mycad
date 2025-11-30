# Microsoft Developer Studio Project File - Name="AutoDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AutoDLL - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AutoDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoDLL.mak" CFG="AutoDLL - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AutoDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AutoDLL - Win32 Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AutoDLL - Win32 Minimal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AutoDLL - Win32 True Time" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AutoDLL - Win32 Bounds Checker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AutoDLL_"
# PROP BASE Intermediate_Dir "AutoDLL_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\AutoDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "NDEBUG" /D "NEWVBA6" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /out "..\lib\auto" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i ".\..\..\..\Eng-Us" /i "./../ICAD" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 delayimp.lib mfcs42.lib msvcrt.lib /nologo /base:"0x51780000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"Stage/IcadAuto.dll" /delayload:db.dll /delayload:gr.dll /MAPINFO:LINES /opt:ref,icf
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AutoDLL0"
# PROP BASE Intermediate_Dir "AutoDLL0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\AutoDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /D "_DEBUG" /out "..\lib\auto" /win32
# SUBTRACT MTL /nologo /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i ".\..\..\..\Eng-Us" /i "./../ICAD" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 delayimp.lib mfcs42d.lib msvcrtd.lib /nologo /base:"0x02000000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"\OutputDir\icad5\Debug/IcadAuto.dll" /pdbtype:sept /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AutoDLL1"
# PROP BASE Intermediate_Dir "AutoDLL1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\AutoDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_DEBUG" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 delayimp.lib mfcs42.lib msvcrt.lib /nologo /base:"0x02000000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"Debug/IcadAuto.dll" /pdbtype:sept /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AutoDLL2"
# PROP BASE Intermediate_Dir "AutoDLL2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\AutoDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "NDEBUG" /D "MINDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /out "..\lib\auto" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 delayimp.lib mfcs42d.lib msvcrtd.lib /nologo /base:"0x51780000" /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"MinDebug/IcadAuto.dll" /delayload:db.dll /delayload:gr.dll

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\AutoDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /D "_DEBUG" /out "..\lib\auto" /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "_DEBUG" /out "..\lib\auto" /win32
# SUBTRACT MTL /nologo /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib mfcs42d.lib msvcrtd.lib /nologo /base:"0x02000000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"Debug/IcadAuto.dll" /pdbtype:sept /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib mfcs42d.lib msvcrtd.lib /nologo /base:"0x02000000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"True_Time/IcadAuto.dll" /pdbtype:sept /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\AutoDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\dcl" /I "..\lib\gr" /I "..\lib\lisp" /I "..\lib\icadlib" /I "..\api\sds" /I "c:\vba5sdk\include" /I "..\lib\sds" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "_WINDLL" /D "_USRDLL" /D "AUTODLL" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /D "_DEBUG" /out "..\lib\auto" /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "_DEBUG" /out "..\lib\auto" /win32
# SUBTRACT MTL /nologo /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib mfcs42d.lib msvcrtd.lib /nologo /base:"0x02000000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"Bounds_Checker/IcadAuto.dll" /pdbtype:sept /delayload:db.dll /delayload:gr.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib mfcs42d.lib msvcrtd.lib /nologo /base:"0x02000000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"Bounds_Checker/IcadAuto.dll" /pdbtype:sept /delayload:db.dll /delayload:gr.dll
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AutoDLL - Win32 Release"
# Name "AutoDLL - Win32 Debug"
# Name "AutoDLL - Win32 Browse"
# Name "AutoDLL - Win32 Minimal Debug"
# Name "AutoDLL - Win32 True Time"
# Name "AutoDLL - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "NotPrecompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\SDS\sds_alert.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_resbuf.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\AUTO\Auto3DFace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\Auto3DPoly.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoApp.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAppPrefs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoArc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAttribute.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAttributeDef.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoBlockInsert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoBlocks.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoCircle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoDbItem.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDim3PointAngular.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimAligned.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimAngular.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimDiametric.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimension.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimOrdinate.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimRadial.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimRotated.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimStyle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimStyles.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDLL.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDocPrefs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDocuments.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoEllipse.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoEntities.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoEntity.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoHatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoImage.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoInfLine.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLayer.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLayers.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLeader.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLibrary.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLine.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLinetype.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLinetypes.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLWPolyline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoMSpace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoMText.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPointEnt.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoPoints.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPolyfaceMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPolygonMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPolyline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoPSpace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPViewport.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoRay.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoRegApps.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSelSet.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSelSets.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoShape.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSolid.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSpline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoText.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTextStyle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTextStyles.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTolerance.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoUCS.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoUCSs.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoUtility.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoVector.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoView.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoViewport.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoViewports.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoViews.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoWindows.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoXRef.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\icadplotcfg.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\PlotProperties.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\lib\AUTO\Auto3DFace.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\Auto3DPoly.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoApp.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAppPrefs.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoArc.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAttribute.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAttributeDef.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoAttributes.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoBaseDefs.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoBlock.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoBlockInsert.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoBlocks.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoCircle.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoDbItem.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDim3PointAngular.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimAligned.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimAngular.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimDiametric.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimension.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimOrdinate.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimRadial.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimRotated.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimStyle.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDimStyles.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDLL.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoDoc.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDocPrefs.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoDocuments.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoEllipse.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoEntities.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoEntity.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoHatch.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoHelpers.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoImage.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoInfLine.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLayer.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLayers.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLeader.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLibrary.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoLine.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLinetype.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLinetypes.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoLWPolyline.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoMatrix.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoMSpace.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoMText.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoPoint.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPointEnt.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoPoints.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPolyfaceMesh.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPolygonMesh.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPolyline.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoPSpace.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoPViewport.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoRay.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\autorc.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoRegApp.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoRegApps.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSelSet.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSelSets.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoShape.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSolid.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoSpline.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoTableItem.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoText.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTextStyle.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTextStyles.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTolerance.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoTrace.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoUCS.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoUCSs.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoUtility.h
# End Source File
# Begin Source File

SOURCE=..\lib\Auto\AutoVector.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoView.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoViewport.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoViewports.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoViews.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoWindow.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoWindows.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\AutoXRef.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\icadplotcfg.h
# End Source File
# Begin Source File

SOURCE=..\lib\auto\PlotProperties.h
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\StdAfx.h
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\AUTO\DLLLoadInfo.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\Icad.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# ADD MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c" /Oicf
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# ADD MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c"
# SUBTRACT MTL /nologo /mktyplib203 /Oicf

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# ADD MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c" /Oicf
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# ADD MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c" /Oicf
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# ADD BASE MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c"
# SUBTRACT BASE MTL /nologo /mktyplib203 /Oicf
# ADD MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c"
# SUBTRACT MTL /nologo /mktyplib203 /Oicf

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# ADD BASE MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c"
# SUBTRACT BASE MTL /nologo /mktyplib203 /Oicf
# ADD MTL /tlb "Icad.tlb" /h "..\..\api\sds\IcadTLB.h" /iid "icad_i.c"
# SUBTRACT MTL /nologo /mktyplib203 /Oicf

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadCoClasses.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadDimensions.idh
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadDimensions.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadDocument.idh
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadDocument.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadEntity.idh
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadEntity.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadEnums.idh
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadEnums.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadGlobal.idh
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadGlobal.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadLibrary.idh
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadLibrary.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\IcadTables.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\PlotSupport.idh
# End Source File
# Begin Source File

SOURCE=..\lib\AUTO\PlotSupport.idl

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\auto\AutoDLL.def
# End Source File
# Begin Source File

SOURCE=..\lib\auto\AutoDLL.rc

!IF  "$(CFG)" == "AutoDLL - Win32 Release"

# ADD BASE RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "\icaddev2\prj\lib\auto" /i "\icad\prj\lib\auto" /i "\dev\prj\lib\auto"
# SUBTRACT BASE RSC /i ".\..\..\..\Eng-Us" /i "./../ICAD"
# ADD RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "Release"
# SUBTRACT RSC /i ".\..\..\..\Eng-Us" /i "./../ICAD"

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Debug"

# ADD BASE RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "\icaddev2\prj\lib\auto" /i "\icad\prj\lib\auto" /i "\dev\prj\lib\auto"
# SUBTRACT BASE RSC /i ".\..\..\..\Eng-Us"
# ADD RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "Debug"
# SUBTRACT RSC /i ".\..\..\..\Eng-Us"

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Browse"

# ADD BASE RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "\icaddev2\prj\lib\auto" /i "\icad\prj\lib\auto" /i "\dev\prj\lib\auto"
# ADD RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "Debug"

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Minimal Debug"

# ADD BASE RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "\icaddev2\prj\lib\auto" /i "\icad\prj\lib\auto" /i "\dev\prj\lib\auto"
# ADD RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "MinDebug"

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 True Time"

# ADD BASE RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "Debug"
# ADD RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "Debug"

!ELSEIF  "$(CFG)" == "AutoDLL - Win32 Bounds Checker"

# ADD BASE RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "Debug"
# ADD RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\auto" /i "\Zwcad5.0\Source\prj\lib\auto" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\auto" /i "\DG\ICAD_ITC\Source\prj\lib\auto" /i "\IcadITC\IntelliCAD\Source\prj\lib\auto" /i "\R40_Dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD_ITC\Source\prj\lib\auto" /i "\ICAD\IntelliCAD\Source\prj\lib\auto" /i "\dev\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\Source\prj\lib\auto" /i "\IntelliCAD\IntelliCAD\Source\prj\lib\auto" /i "\ItcDev\prj\lib\auto" /i "\IcadDev\prj\lib\auto" /i "\IntelliCAD\prj\lib\auto" /i "Debug"

!ENDIF 

# End Source File
# End Target
# End Project

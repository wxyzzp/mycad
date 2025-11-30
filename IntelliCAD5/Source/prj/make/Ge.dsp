# Microsoft Developer Studio Project File - Name="Ge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Ge - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Ge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ge.mak" CFG="Ge - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Ge - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Ge - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Ge - Win32 True Time" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Ge - Win32 Bounds Checker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Ge - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Ge"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\Ge\ents2d" /I "..\lib\Ge\ents3d" /I "..\lib\Ge\misc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GEDLL" /YX"GePch.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /debug /machine:I386 /out:"Stage/Ge.dll" /opt:ref,icf
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Ge - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\Ge"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /I "..\lib\Ge\ents2d" /I "..\lib\Ge\ents3d" /I "..\lib\Ge\misc" /D "_DEBUG" /D "GEDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fr /YX"GePch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "Ge - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\Ge"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /I "..\lib\Ge\ents2d" /I "..\lib\Ge\ents3d" /I "..\lib\Ge\misc" /D "_DEBUG" /D "GEDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fr /YX"GePch.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /I "..\lib\Ge\ents2d" /I "..\lib\Ge\ents3d" /I "..\lib\Ge\misc" /D "_DEBUG" /D "GEDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fr /YX"GePch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "Ge - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\Ge"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /I "..\lib\Ge\ents2d" /I "..\lib\Ge\ents3d" /I "..\lib\Ge\misc" /D "_DEBUG" /D "GEDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fr /YX"GePch.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /I "..\lib\Ge\ents2d" /I "..\lib\Ge\ents3d" /I "..\lib\Ge\misc" /D "_DEBUG" /D "GEDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fr /YX"GePch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Ge - Win32 Release"
# Name "Ge - Win32 Debug"
# Name "Ge - Win32 True Time"
# Name "Ge - Win32 Bounds Checker"
# Begin Group "ents2d"

# PROP Default_Filter "h;hpp;hxx;hm;inl;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeBoundBlock2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeBoundBlock2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCircArc2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCircArc2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeClipBoundary2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeClipBoundary2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCompositeCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCompositeCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCubicSplineCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCubicSplineCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCurveCurveInt2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeCurveCurveInt2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeEllipArc2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeEllipArc2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeEntity2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeEntity2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeExternalCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeExternalCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeLine2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeLine2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeLinearCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeLinearCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeLineSegment2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeLineSegment2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeNurbCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeNurbCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeOffsetCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeOffsetCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePointEntity2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePointEntity2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePointOnCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePointOnCurve2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePolyline2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePolyline2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePosition2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GePosition2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeRay2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeRay2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeSplineCurve2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents2d\GeSplineCurve2d.h
# End Source File
# End Group
# Begin Group "ents3d"

# PROP Default_Filter "h;hpp;hxx;hm;inl;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeAugPolyline3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeAugPolyline3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeBoundBlock3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeBoundBlock3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeBoundedPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeBoundedPlane.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCircArc3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCircArc3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCompositeCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCompositeCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCone.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCone.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCubicSplineCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCubicSplineCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCurveCurveInt3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCurveCurveInt3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCurveSurfInt.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCurveSurfInt.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeCylinder.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeEllipArc3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeEllipArc3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeEntity3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeEntity3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeExternalBoundedSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeExternalBoundedSurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeExternalCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeExternalCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeExternalSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeExternalSurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeLine3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeLine3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeLinearCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeLinearCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeLineSegment3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeLineSegment3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeNurbCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeNurbCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeNurbSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeNurbSurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeOffsetCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeOffsetCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeOffsetSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeOffsetSurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePlanarSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePlanarSurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePlane.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePlane.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePointEntity3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePointEntity3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePointOnCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePointOnCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePointOnSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePointOnSurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePolyline3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePolyline3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePosition3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GePosition3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeRay3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeRay3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSphere.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSplineCurve3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSplineCurve3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSplineCurveBase.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSplineCurveBase.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSurfaceSurfaceInt.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeSurfaceSurfaceInt.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeTorus.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\ents3d\GeTorus.h
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter "h;hpp;hxx;hm;inl;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\Ge\misc\GeArrays.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeContext.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeContext.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeCurveBoundary.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeCurveBoundary.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeInterval.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeInterval.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeKnotVector.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeKnotVector.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeLibVersion.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeLibVersion.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeMatrix2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeMatrix2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeMatrix3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeMatrix3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GePch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GePch.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GePoint2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GePoint2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GePoint3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GePoint3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeScale2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeScale2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeScale3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeScale3d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeTolerance.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeTolerance.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeVector2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeVector2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeVector3d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Ge\misc\GeVector3d.h
# End Source File
# End Group
# End Target
# End Project

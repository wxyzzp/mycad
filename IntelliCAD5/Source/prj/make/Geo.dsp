# Microsoft Developer Studio Project File - Name="geo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=geo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Geo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Geo.mak" CFG="geo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "geo - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geo - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "geo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\geo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GEO_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "GEO_EXPORTS" /D "GEODLL" /D "INTELLICAD" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib /nologo /dll /debug /machine:I386 /out:".\Stage\geo.dll" /opt:ref,icf
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "geo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\geo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GEO_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "_MBCS" /D "GEO_EXPORTS" /D "GEODLL" /D "INTELLICAD" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "geo - Win32 Release"
# Name "geo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\ThirdParty\geo\2CurveSolver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\array.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\array2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\Boolean.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\compcrvs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\crvofset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\crvpair.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\ctrlpts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\curve.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\elliparc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\extents.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\Geo.rc
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\knots.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\line.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\LinSolver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\lsqtrans.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\nurbs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\path.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\plane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\point.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\PolyLine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\polynomial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\projective.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\ptarray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\rational.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\Region.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\roots.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\sqarray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\surface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\theuser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\trnsform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\vector.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\ThirdParty\geo\2CurveSolver.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\array.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\array2d.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\boolean.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\compcrvs.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\constnts.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\crvofset.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\crvpair.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\ctrlpts.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\curve.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\elliparc.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\extents.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\geomptrs.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\knots.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\line.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\LinSolver.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\lsqtrans.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\macros.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\Matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\nurbs.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\path.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\plane.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\point.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\PolyLine.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\polynomial.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\projective.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\ptarray.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\rational.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\region.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\roots.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\sqarray.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\surface.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\theuser.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\trnsform.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\geo\vector.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

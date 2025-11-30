# Microsoft Developer Studio Project File - Name="ACIS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ACIS - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ACIS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ACIS.mak" CFG="ACIS - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ACIS - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACIS - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACIS - Win32 DebugTest" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACIS - Win32 ReleaseTest" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACIS - Win32 True Time" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACIS - Win32 Bounds Checker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\ACIS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\include" /D "SPA_NO_AUTO_LINK" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib db.lib icadlib.lib SpaABlend.lib SpaACIS.lib SpaALops.lib SpaAPart.lib SpaASurf.lib SpaAVis.lib SpaAWarp.lib SpaBase.lib /nologo /dll /debug /machine:I386 /out:".\Stage\ACIS.dll" /libpath:"$(A3DT)\lib\NT_DLL" /libpath:".\Release" /opt:ref,icf
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy *.DLL Stage...
PostBuild_Cmds=copy $(A3DT)\bin\NT_DLL\*.DLL Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\ACIS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\include" /D "SPA_NO_AUTO_LINK" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Fr /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib db.lib icadlib.lib SpaACISd.lib SpaBased.lib SpaABlendd.lib SpaALopsd.lib SpaAVisd.lib SpaAWarpd.lib SpaAPartd.lib SpaASurfd.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(A3DT)\lib\NT_DLLD" /libpath:".\Debug"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ACIS___Win32_DebugTest"
# PROP BASE Intermediate_Dir "ACIS___Win32_DebugTest"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Acis"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT BASE CPP /WX /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\TestSystem" /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /D "_TEST" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib db.lib icadlib.lib /nologo /dll /debug /machine:I386 /out:".\Debug\ACIS.dll" /pdbtype:sept /libpath:"$(A3DT)\lib\NT_DLLD" /libpath:".\Debug"
# ADD LINK32 kernel32.lib db.lib icadlib.lib testunitd.lib SpaABlendd.lib SpaACISd.lib SpaALopsd.lib SpaAPartd.lib SpaASurfd.lib SpaAVisd.lib SpaAWarpd.lib SpaBased.lib /nologo /dll /debug /machine:I386 /out:".\Debug\ACIS.dll" /pdbtype:sept /libpath:"$(A3DT)\lib\NT_DLLD" /libpath:".\Debug"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ACIS___Win32_ReleaseTest"
# PROP BASE Intermediate_Dir "ACIS___Win32_ReleaseTest"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\Acis"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\TestSystem" /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /D "_TEST" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib db.lib icadlib.lib /nologo /dll /debug /machine:I386 /out:".\Stage\ACIS.dll" /libpath:"$(A3DT)\lib\NT_DLL" /libpath:".\Release"
# ADD LINK32 kernel32.lib db.lib icadlib.lib SpaABlend.lib SpaACIS.lib SpaALops.lib SpaAPart.lib SpaASurf.lib SpaAVis.lib SpaAWarp.lib SpaBase.lib /nologo /dll /machine:I386 /out:".\Stage\ACIS.dll" /libpath:"$(A3DT)\lib\NT_DLL" /libpath:".\Release"
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy *.DLL Stage...
PostBuild_Cmds=copy $(A3DT)\bin\NT_DLL\*.DLL Stage
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\ACIS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT BASE CPP /WX /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib db.lib icadlib.lib /nologo /dll /debug /machine:I386 /out:".\True_Time\ACIS.dll" /pdbtype:sept /libpath:"$(A3DT)\lib\NT_DLLD" /libpath:".\True_Time"
# ADD LINK32 kernel32.lib db.lib icadlib.lib SpaABlendd.lib SpaACISd.lib SpaALopsd.lib SpaAPartd.lib SpaASurfd.lib SpaAVisd.lib SpaAWarpd.lib SpaBased.lib /nologo /dll /debug /machine:I386 /out:".\True_Time\ACIS.dll" /pdbtype:sept /libpath:"$(A3DT)\lib\NT_DLLD" /libpath:".\True_Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\ACIS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT BASE CPP /WX /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\DB" /I "..\lib\ICADLIB" /I "..\..\ThirdParty\GEO" /I "..\api\sds" /I "$(A3DT)\base" /I "$(A3DT)\law" /I "$(A3DT)\kern" /I "$(A3DT)\cstr" /I "$(A3DT)\intr" /I "$(A3DT)\eulr" /I "$(A3DT)\bool" /I "$(A3DT)\covr" /I "$(A3DT)\ofst" /I "$(A3DT)\swp" /I "$(A3DT)\fct" /I "$(A3DT)\blnd" /I "$(A3DT)\oper" /I "$(A3DT)\rbase" /I "$(A3DT)\gl" /I "$(A3DT)\gi" /I "$(A3DT)\ct" /I "$(A3DT)\skin" /I "$(A3DT)\lopt" /I "$(A3DT)\rem" /I "$(A3DT)\rbi" /I "$(A3DT)\lop" /I "$(A3DT)\shl" /I "$(A3DT)\mmgr" /I "$(A3DT)\ga" /I "$(A3DT)\ihl" /I "$(A3DT)\warp" /I "$(A3DT)\part" /I "$(A3DT)\sbool" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACIS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib db.lib icadlib.lib /nologo /dll /debug /machine:I386 /out:".\Bounds_Checker\ACIS.dll" /pdbtype:sept /libpath:"$(A3DT)\lib\NT_DLLD" /libpath:".\Bounds_Checker"
# ADD LINK32 kernel32.lib db.lib icadlib.lib SpaABlendd.lib SpaACISd.lib SpaALopsd.lib SpaAPartd.lib SpaASurfd.lib SpaAVisd.lib SpaAWarpd.lib SpaBased.lib /nologo /dll /debug /machine:I386 /out:".\Bounds_Checker\ACIS.dll" /pdbtype:sept /libpath:"$(A3DT)\lib\NT_DLLD" /libpath:".\Bounds_Checker"

!ENDIF 

# Begin Target

# Name "ACIS - Win32 Release"
# Name "ACIS - Win32 Debug"
# Name "ACIS - Win32 DebugTest"
# Name "ACIS - Win32 ReleaseTest"
# Name "ACIS - Win32 True Time"
# Name "ACIS - Win32 Bounds Checker"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\ACIS\ACIS.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Acis.rc

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD BASE RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\ACIS" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\ACIS" /i "\Dev\R50\IntelliCAD\Source\prj\lib\ACIS"
# ADD RSC /l 0x409 /i "\Project\IntelliCAD5\Source\prj\lib\ACIS" /i "\Check In Checkout\12-Aug-04\Remote Get\IntelliCAD\Source\prj\lib\ACIS" /i "\Dev\R50\IntelliCAD\Source\prj\lib\ACIS"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsAttrib.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsAttribMarker.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsCache.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsConstructor.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsDisplayObject.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsExtractor.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsMesh.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsMeshManager.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsModeler.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsObject.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsSatStream.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsSnaping.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsTools.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsViewer.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\StdAfx.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# ADD CPP /D "ACIS_R12" /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\lib\ACIS\ACIS.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\acisHeaders.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsAttrib.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsAttribMarker.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsAutoPtr.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsCache.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsConstructor.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsDisplayObject.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsDisplayObject.inl
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsExtractor.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsFunctional.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsMesh.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsMeshManager.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsModeler.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsObject.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsSatStream.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsSnaping.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\AsTools.h
# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\StdAfx.h
# End Source File
# End Group
# Begin Group "Tests"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsBooleanOperations.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /D "SPA_NO_AUTO_LINK"

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsCrasher.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsCrasher.h

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsCynematic.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsDisplay.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsExploding.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsFunctional.h

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsPicking.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsPrimitiveConstruction.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsQuery.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsSaveLoadSatFile.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsSectioning.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsSolidedit.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsSurfaceConstruction.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsTools.cpp

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\ACIS\Tests\TsTools.h

!IF  "$(CFG)" == "ACIS - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 DebugTest"

!ELSEIF  "$(CFG)" == "ACIS - Win32 ReleaseTest"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ACIS - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project

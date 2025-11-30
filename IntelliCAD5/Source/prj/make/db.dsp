# Microsoft Developer Studio Project File - Name="db" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=db - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "db.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "db.mak" CFG="db - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "db - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "db - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "db - Win32 Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "db - Win32 Minimal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "db - Win32 True Time" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "db - Win32 Bounds Checker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "db - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "db___Win"
# PROP BASE Intermediate_Dir "db___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\db"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\sds" /I "..\lib\dcl" /I "..\..\Licensed\DWGdirect\ExServices" /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gi" /I "..\..\Licensed\DWGdirect\include\gs" /I "..\..\Licensed\DWGdirect\include\ModelerGeometry" /I "..\..\Licensed\DWGdirect\Exports\SvgExport\include" /I "..\..\Licensed\DWGdirect\Exports\DwfExport\include" /I "..\..\Licensed\DWGdirect\Imports\DwfImport\include" /D "NDEBUG" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "USE_DWGdirect" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /out "..\lib\db" /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /i ".\..\icad" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 DD_VC6MD_AcisBuilder.lib DD_VC6MD_AcisRenderer.lib DD_VC6MD_Alloc.lib DD_VC6MD_Br.lib DD_VC6MD_Db.lib DD_VC6MD_DwfExport.lib DD_VC6MD_DwfImport.lib DD_VC6MD_DwfToolkit.lib DD_VC6MD_FT.lib DD_VC6MD_Ge.lib DD_VC6MD_Gi.lib DD_VC6MD_Gs.lib DD_VC6MD_Jpeg.lib DD_VC6MD_ModelerGeometry.lib DD_VC6MD_Root.lib DD_VC6MD_SpatialIndex.lib DD_VC6MD_SvgExport.lib DD_VC6MD_Zip.lib DD_VC6MD_ZLib.lib DD_VC6MD_Ole.lib Gs_WinGDI.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x51980000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"msvcrtd" /out:".\Stage\db.dll" /libpath:"..\..\Licensed\DWGdirect\lib\VC6MD" /MAPINFO:LINES /opt:ref,icf
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\Release
TargetPath=.\Stage\db.dll
InputPath=.\Stage\db.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "db - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "db___Wi0"
# PROP BASE Intermediate_Dir "db___Wi0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\db"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\Licensed\DWGdirect\ExServices" /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gi" /I "..\..\Licensed\DWGdirect\include\gs" /I "..\..\Licensed\DWGdirect\include\ModelerGeometry" /I "..\..\Licensed\DWGdirect\Exports\SvgExport\include" /I "..\..\Licensed\DWGdirect\Exports\DwfExport\include" /I "..\..\Licensed\DWGdirect\Imports\DwfImport\include" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /out "..\lib\db" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /i ".\..\icad" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 DD_VC6MD_AcisBuilder.lib DD_VC6MD_AcisRenderer.lib DD_VC6MD_Alloc.lib DD_VC6MD_Br.lib DD_VC6MD_Db.lib DD_VC6MD_DwfExport.lib DD_VC6MD_DwfImport.lib DD_VC6MD_DwfToolkit.lib DD_VC6MD_FT.lib DD_VC6MD_Ge.lib DD_VC6MD_Gi.lib DD_VC6MD_Gs.lib DD_VC6MD_Jpeg.lib DD_VC6MD_ModelerGeometry.lib DD_VC6MD_Root.lib DD_VC6MD_SpatialIndex.lib DD_VC6MD_SvgExport.lib DD_VC6MD_Zip.lib DD_VC6MD_ZLib.lib DD_VC6MD_Ole.lib Gs_WinGDI.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03000000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /pdbtype:sept /libpath:"..\..\Licensed\DWGdirect\lib\VC6MD"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "db - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "db___Win"
# PROP BASE Intermediate_Dir "db___Win"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\db"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /I "..\lib\sds" /I "..\lib\db" /I "..\..\ThirdParty\marcomp" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "INC_OLE1" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\lib\dcl" /I "..\..\Licensed\DWGdirect\ExServices" /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gi" /I "..\..\Licensed\DWGdirect\include\gs" /I "..\..\Licensed\DWGdirect\include\ModelerGeometry" /I "..\..\Licensed\DWGdirect\Exports\SvgExport\include" /I "..\..\Licensed\DWGdirect\Exports\DwfExport\include" /I "..\..\Licensed\DWGdirect\Imports\DwfImport\include" /D "DEBUG" /D "_DEBUG" /D "USE_DWGdirect" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /FR /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /out "..\lib\db" /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\db.dll" /pdbtype:sept
# ADD LINK32 DD_VC6MD_AcisBuilder.lib DD_VC6MD_AcisRenderer.lib DD_VC6MD_Alloc.lib DD_VC6MD_Br.lib DD_VC6MD_Db.lib DD_VC6MD_DwfExport.lib DD_VC6MD_DwfImport.lib DD_VC6MD_DwfToolkit.lib DD_VC6MD_FT.lib DD_VC6MD_Ge.lib DD_VC6MD_Gi.lib DD_VC6MD_Gs.lib DD_VC6MD_Jpeg.lib DD_VC6MD_ModelerGeometry.lib DD_VC6MD_Root.lib DD_VC6MD_SpatialIndex.lib DD_VC6MD_SvgExport.lib DD_VC6MD_Zip.lib DD_VC6MD_ZLib.lib DD_VC6MD_Ole.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03000000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /out:".\Debug\db.dll" /pdbtype:sept /libpath:"..\..\Licensed\DWGdirect\lib\VC6MD"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\db.dll
InputPath=.\Debug\db.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "db - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "db___Win"
# PROP BASE Intermediate_Dir "db___Win"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\db"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\marcomp" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "INC_OLE1" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\lib\sds" /I "..\lib\dcl" /I "..\..\Licensed\DWGdirect\ExServices" /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gi" /I "..\..\Licensed\DWGdirect\include\gs" /I "..\..\Licensed\DWGdirect\include\ModelerGeometry" /I "..\..\Licensed\DWGdirect\Exports\SvgExport\include" /I "..\..\Licensed\DWGdirect\Exports\DwfExport\include" /I "..\..\Licensed\DWGdirect\Imports\DwfImport\include" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "NDEBUG" /D "MINDEBUG" /D "USE_DWGdirect" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /out "..\lib\db" /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/db.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x51980000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt" /out:".\Stage\db.dll"
# ADD LINK32 DD_VC6MD_AcisBuilder.lib DD_VC6MD_AcisRenderer.lib DD_VC6MD_Alloc.lib DD_VC6MD_Br.lib DD_VC6MD_Db.lib DD_VC6MD_DwfExport.lib DD_VC6MD_DwfImport.lib DD_VC6MD_DwfToolkit.lib DD_VC6MD_FT.lib DD_VC6MD_Ge.lib DD_VC6MD_Gi.lib DD_VC6MD_Gs.lib DD_VC6MD_Jpeg.lib DD_VC6MD_ModelerGeometry.lib DD_VC6MD_Root.lib DD_VC6MD_SpatialIndex.lib DD_VC6MD_SvgExport.lib DD_VC6MD_Zip.lib DD_VC6MD_ZLib.lib DD_VC6MD_Ole.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x51980000" /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"msvcrt" /libpath:"..\..\Licensed\DWGdirect\lib\VC6MD"
# Begin Custom Build - Performing registration
OutDir=.\MinDebug
TargetPath=.\MinDebug\db.dll
InputPath=.\MinDebug\db.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "db - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\db"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\Licensed\DWGdirect\ExServices" /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gi" /I "..\..\Licensed\DWGdirect\include\gs" /I "..\..\Licensed\DWGdirect\include\ModelerGeometry" /I "..\..\Licensed\DWGdirect\Exports\SvgExport\include" /I "..\..\Licensed\DWGdirect\Exports\DwfExport\include" /I "..\..\Licensed\DWGdirect\Imports\DwfImport\include" /D "DEBUG" /D "_DEBUG" /D "USE_DWGdirect" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /out "..\lib\db" /win32
# ADD MTL /nologo /D "_DEBUG" /out "..\lib\db" /win32
# ADD BASE RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03000000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /out:".\Debug\db.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 DD_VC6MD_AcisBuilder.lib DD_VC6MD_AcisRenderer.lib DD_VC6MD_Alloc.lib DD_VC6MD_Br.lib DD_VC6MD_Db.lib DD_VC6MD_DwfExport.lib DD_VC6MD_DwfImport.lib DD_VC6MD_DwfToolkit.lib DD_VC6MD_FT.lib DD_VC6MD_Ge.lib DD_VC6MD_Gi.lib DD_VC6MD_Gs.lib DD_VC6MD_Jpeg.lib DD_VC6MD_ModelerGeometry.lib DD_VC6MD_Root.lib DD_VC6MD_SpatialIndex.lib DD_VC6MD_SvgExport.lib DD_VC6MD_Zip.lib DD_VC6MD_ZLib.lib DD_VC6MD_Ole.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03000000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /out:".\True_Time\db.dll" /pdbtype:sept /libpath:"..\..\Licensed\DWGdirect\lib\VC6MD"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\True_Time
TargetPath=.\True_Time\db.dll
InputPath=.\True_Time\db.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "db - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\db"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "DEBUG" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\Licensed\DWGdirect\ExServices" /I "..\..\Licensed\ODTBIN" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gi" /I "..\..\Licensed\DWGdirect\include\gs" /I "..\..\Licensed\DWGdirect\include\ModelerGeometry" /I "..\..\Licensed\DWGdirect\Exports\SvgExport\include" /I "..\..\Licensed\DWGdirect\Exports\DwfExport\include" /I "..\..\Licensed\DWGdirect\Imports\DwfImport\include" /D "DEBUG" /D "_DEBUG" /D "USE_DWGdirect" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "DBDLL" /D "INTELLICAD" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /out "..\lib\db" /win32
# ADD MTL /nologo /D "_DEBUG" /out "..\lib\db" /win32
# ADD BASE RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Eng-us" /i "..\lib\db\res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03000000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /out:".\Bounds_Checker\db.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 DD_VC6MD_AcisBuilder.lib DD_VC6MD_AcisRenderer.lib DD_VC6MD_Alloc.lib DD_VC6MD_Br.lib DD_VC6MD_Db.lib DD_VC6MD_DwfExport.lib DD_VC6MD_DwfImport.lib DD_VC6MD_DwfToolkit.lib DD_VC6MD_FT.lib DD_VC6MD_Ge.lib DD_VC6MD_Gi.lib DD_VC6MD_Gs.lib DD_VC6MD_Jpeg.lib DD_VC6MD_ModelerGeometry.lib DD_VC6MD_Root.lib DD_VC6MD_SpatialIndex.lib DD_VC6MD_SvgExport.lib DD_VC6MD_Zip.lib DD_VC6MD_ZLib.lib DD_VC6MD_Ole.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x03000000" /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /out:".\Bounds_Checker\db.dll" /pdbtype:sept /libpath:"..\..\Licensed\DWGdirect\lib\VC6MD"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\Bounds_Checker
TargetPath=.\Bounds_Checker\db.dll
InputPath=.\Bounds_Checker\db.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "db - Win32 Release"
# Name "db - Win32 Debug"
# Name "db - Win32 Browse"
# Name "db - Win32 Minimal Debug"
# Name "db - Win32 True Time"
# Name "db - Win32 Bounds Checker"
# Begin Group "Source"

# PROP Default_Filter ".c;.cpp"
# Begin Group "NotPrecompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Lib\Db\Db1.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbdllloadinfo.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbmodule.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\DB\filestream.cpp
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Group "DWGdirect"

# PROP Default_Filter ""
# Begin Group "Dwg"

# PROP Default_Filter ""
# Begin Group "Dwg Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectConverter.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectFiler.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectProtocolExtension.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectReader.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\GiWorldDrawVectors.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\Utilities.cpp
# End Source File
# End Group
# Begin Group "Dwg Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\DRW\CollectionClasses.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectConverter.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectFiler.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectProtocolExtension.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectReader.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectReaderCrypt.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectWriter.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\DWGdirectWriterCrypt.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\GiWorldDrawVectors.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\Utilities.h
# End Source File
# End Group
# End Group
# Begin Group "Svg"

# PROP Default_Filter ""
# Begin Group "Svg Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\Exports\SvgExport\Source\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\Exports\SvgExport\Source\SvgExport.cpp
# End Source File
# End Group
# Begin Group "Svg Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\Exports\SvgExport\Source\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\Exports\SvgExport\Include\SvgExport.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\Exports\SvgExport\Source\SvgShxEmbedding.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\Exports\SvgExport\Source\SvgWrappers.h
# End Source File
# End Group
# End Group
# Begin Group "ExServices"

# PROP Default_Filter ""
# Begin Group "Services Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExGiRasterImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExHostAppServices.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExSystemServices.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExUndoController.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\OdFileBuf.cpp
# End Source File
# End Group
# Begin Group "Services Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExGiEnhancedMetafile.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExGiRasterImage.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExHostAppServices.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExOdStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExSystemServices.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExUndoController.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\OdFileBuf.h
# End Source File
# End Group
# End Group
# End Group
# Begin Source File

SOURCE=..\lib\DB\3dface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\acad_proxy_entity.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\acad_proxy_object.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\appidtabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\arc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\arcalignedtext.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\attdef.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\attrib.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\block.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\blocktabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\circle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\class.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Db\Db_util.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbAcisEntity.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbAcisEntityData.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbGroup.cpp

!IF  "$(CFG)" == "db - Win32 Release"

!ELSEIF  "$(CFG)" == "db - Win32 Debug"

# SUBTRACT CPP /WX

!ELSEIF  "$(CFG)" == "db - Win32 Browse"

!ELSEIF  "$(CFG)" == "db - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "db - Win32 True Time"

!ELSEIF  "$(CFG)" == "db - Win32 Bounds Checker"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbLayout.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbMText.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbPlotSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbSortentsTable.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbToHatchLink.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dictionary.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dictionaryvar.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dictionarywdflt.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dimassoc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dimension.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dimstyletabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\drawing.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\drwfns.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Db\dwgcodepage.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\dwgload.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\dwgsave.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\edata.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ellipse.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\endblk.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\entity.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\failedfonttable.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\fixup1005.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\fixuptable.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\GeoData.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\GeoData.inl
# End Source File
# Begin Source File

SOURCE=..\lib\DB\getendpt.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handitem.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handletable.cpp

!IF  "$(CFG)" == "db - Win32 Release"

!ELSEIF  "$(CFG)" == "db - Win32 Debug"

# SUBTRACT CPP /WX

!ELSEIF  "$(CFG)" == "db - Win32 Browse"

!ELSEIF  "$(CFG)" == "db - Win32 Minimal Debug"

!ELSEIF  "$(CFG)" == "db - Win32 True Time"

# SUBTRACT CPP /WX

!ELSEIF  "$(CFG)" == "db - Win32 Bounds Checker"

# SUBTRACT CPP /WX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\DB\hatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\hireflink.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\idbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\image.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\imagedef.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\imagedefreactor.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\insert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\layer_index.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\layertabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\leader.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\line.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ltypetabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\lwpolyline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\mline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\mlinestyle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\Modeler.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\object_ptr.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\objhandle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\oldvars.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ole2frame.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\placeholder.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\point.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\polyline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\rastervariables.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ray.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\reactor.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ResourceString.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\rtext.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_alert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_findfile.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_resbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\sds_wcmatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\seqend.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\setfont.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\shape.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\solid.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\spatial_filter.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\spatial_index.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\spline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\styletabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\table.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\tablerecord.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\text.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\textstyleutils.cpp

!IF  "$(CFG)" == "db - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\DB\tolerance.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\trace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ucstabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\vbaproject.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\vertex.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\viewport.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\viewtabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\vporttabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\vxtabrec.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\wipeout.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\wipeoutvariables.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\xline.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\xrecord.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\lib\DB\appid.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\BinaryResource.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\blocktabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\charbuflink.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\class.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\cvisioraster.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Db\Db.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbAcisEntity.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbAcisEntityData.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbdefines.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbdllloadinfo.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbExport.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbGroup.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbLayout.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbMText.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbPlotSettings.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbprotos.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbrc.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbSortentsTable.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\DbToHatchlink.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dimstyletabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\drawing.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Drw\Drw.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\drwprotos.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Db\dwgcodepage.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\entities.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\entity.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\entitywithextrusion.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\extvars.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\failedfonttable.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\filestream.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\fixup1005.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\fixuptable.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\fontlink.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\GeoData.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handitem.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handletable.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\hireflink.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\irasdef.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\layertabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ltypetabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\Modeler.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ModelerStub.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\objects.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\objhandle.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\odtver.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\oldvars.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\reactor.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\styletabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\sysvarlink.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\table.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\tablerecord.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\textstyleutils.h

!IF  "$(CFG)" == "db - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\DB\ucstabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\viewport.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\viewtabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\vporttabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\vxtabrec.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Lib\Db\Db.def
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbdll.rc
# End Source File
# Begin Source File

SOURCE=..\lib\DB\res\dbdll.rc2

!IF  "$(CFG)" == "db - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Browse"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Minimal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 True Time"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "db - Win32 Bounds Checker"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\DB\DBDLLLoadInfo.rgs
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbmodule.idl
# ADD MTL /I "..\lib\icadlib" /tlb "dbmodule.tlb" /h "dbmodule.h"
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\Odtbin\ADV6ND.LIB
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project

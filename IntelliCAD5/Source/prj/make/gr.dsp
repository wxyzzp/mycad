# Microsoft Developer Studio Project File - Name="gr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gr - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gr.mak" CFG="gr - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gr - Win32 Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gr - Win32 Minimal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gr - Win32 True Time" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gr - Win32 Bounds Checker" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gr___Win"
# PROP BASE Intermediate_Dir "gr___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\gr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /I "..\lib\icadlib" /D "NDEBUG" /out "..\lib\gr" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i ".\..\icad" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x51d80000" /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libcmt" /out:".\Stage\gr.dll" /libpath:"..\api\sds" /delayload:db.dll /MAPINFO:LINES /opt:ref,icf
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\Release
TargetPath=.\Stage\gr.dll
InputPath=.\Stage\gr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "gr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gr___Wi0"
# PROP BASE Intermediate_Dir "gr___Wi0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\gr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /out "..\lib\gr" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i ".\..\icad" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05000000" /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\api\sds" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=\OutputDir\icad5\Debug
TargetPath=\OutputDir\icad5\Debug\gr.dll
InputPath=\OutputDir\icad5\Debug\gr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "gr - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gr___Win"
# PROP BASE Intermediate_Dir "gr___Win"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\gr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\lib\icadlib" /I "..\lib\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\geo" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /FR /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /I "..\lib\icadlib" /D "_DEBUG" /out "..\lib\gr" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\gr.dll" /pdbtype:sept
# ADD LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05000000" /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\gr.dll" /pdbtype:sept /libpath:"..\api\sds" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\gr.dll
InputPath=.\Debug\gr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "gr - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gr___Win"
# PROP BASE Intermediate_Dir "gr___Win"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\gr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\api\sds" /I "..\icad" /I "..\lib\db" /I "..\lib\drw" /I "..\lib\gr" /I "..\lib\geo" /D "NDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_AFXDLL" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "NDEBUG" /D "MINDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /out "..\lib\gr" /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x51d80000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt" /out:".\Stage\gr.dll"
# ADD LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x51d80000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmt" /libpath:"..\api\sds" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\MinDebug
TargetPath=.\MinDebug\gr.dll
InputPath=.\MinDebug\gr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "gr - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\gr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /out "..\lib\gr" /win32
# ADD MTL /nologo /D "_DEBUG" /out "..\lib\gr" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05000000" /subsystem:windows /dll /debug /machine:I386 /out:".\True_Time\gr.dll" /pdbtype:sept /libpath:"..\api\sds" /delayload:db.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05000000" /subsystem:windows /dll /debug /machine:I386 /out:".\True_Time\gr.dll" /pdbtype:sept /libpath:"..\api\sds" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\True_Time
TargetPath=.\True_Time\gr.dll
InputPath=.\True_Time\gr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "gr - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\gr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "..\..\ThirdParty\geo" /I "..\icad" /I "..\lib\drw" /I "..\lib\gr" /I "..\..\ThirdParty\Wintab\include" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\sds" /I "..\lib\cmds" /I "..\lib\lisp" /D "_DEBUG" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "_MBCS" /D "SDS_NEWDB" /D "GR_USEEXTERNALSPLINE" /D "INTELLICAD" /D "GRDLL" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /out "..\lib\gr" /win32
# ADD MTL /nologo /D "_DEBUG" /out "..\lib\gr" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05000000" /subsystem:windows /dll /debug /machine:I386 /out:".\Bounds_Checker\gr.dll" /pdbtype:sept /libpath:"..\api\sds" /delayload:db.dll
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 odbc32.lib odbccp32.lib delayimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x05000000" /subsystem:windows /dll /debug /machine:I386 /out:".\Bounds_Checker\gr.dll" /pdbtype:sept /libpath:"..\api\sds" /delayload:db.dll
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\Bounds_Checker
TargetPath=.\Bounds_Checker\gr.dll
InputPath=.\Bounds_Checker\gr.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "gr - Win32 Release"
# Name "gr - Win32 Debug"
# Name "gr - Win32 Browse"
# Name "gr - Win32 Minimal Debug"
# Name "gr - Win32 True Time"
# Name "gr - Win32 Bounds Checker"
# Begin Group "Source"

# PROP Default_Filter "*.c;*.cpp"
# Begin Group "NotPrecompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\GR\grdllloadinfo.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\GR\grmodule.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\GR\grvars.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_findfile.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\lib\SDS\sds_resbuf.cpp
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\GR\2d.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\arcgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\clipper.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordSetup.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordUCS.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\coordWidePC.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\d3.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\dispobjs.cpp

!IF  "$(CFG)" == "gr - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "gr - Win32 Debug"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "gr - Win32 Browse"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "gr - Win32 Minimal Debug"

# ADD CPP /O2 /YX

!ELSEIF  "$(CFG)" == "gr - Win32 True Time"

# ADD BASE CPP /YX
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "gr - Win32 Bounds Checker"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoBase.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoDef.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoFont.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoSelect.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoText.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\entsel.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\explodemtext.cpp
# End Source File
# Begin Source File

SOURCE=..\Lib\Gr\glyph.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_DisplaySurface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_Hatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_parsetext.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_splines.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_view.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_wordwrap.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\leadergen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\linetypegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\mlinegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\mtextgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\pc2dispobj.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\pointgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\polycache.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\rayxlinegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\selectedent.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\selectorlink.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\splinegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\textgen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\tolerancegen.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\GR\TrueTypeUtils.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\lib\GR\2d.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\clipper.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\d3.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DBCSfontname.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoBase.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoDef.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoFont.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoSelect.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\DoText.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Gr\glyph.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Gr\Gr.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_DisplaySurface.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_parsetext.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_view.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\gr_wordwrap.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\grdllloadinfo.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\linetypegen.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\matrix.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\mtextgen.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\pc2dispobj.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\polycache.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\splines.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\splineutils.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\textgen.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\tolerancegen.h
# End Source File
# Begin Source File

SOURCE=..\lib\GR\TrueTypeUtils.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\GR\grdll.rc
# End Source File
# Begin Source File

SOURCE=..\lib\GR\grdllloadinfo.rgs
# End Source File
# Begin Source File

SOURCE=..\lib\GR\grmodule.def
# End Source File
# Begin Source File

SOURCE=..\lib\GR\grmodule.idl

!IF  "$(CFG)" == "gr - Win32 Release"

# ADD MTL /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "gr - Win32 Debug"

# PROP Intermediate_Dir "Debug\gr"
# ADD MTL /I "..\lib\icadlib" /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "gr - Win32 Browse"

# ADD MTL /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "gr - Win32 Minimal Debug"

# ADD MTL /I "..\lib\icadlib" /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "gr - Win32 True Time"

# PROP BASE Intermediate_Dir "Debug\gr"
# PROP Intermediate_Dir "Debug\gr"
# ADD BASE MTL /I "..\lib\icadlib" /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /I "..\lib\icadlib" /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "gr - Win32 Bounds Checker"

# PROP BASE Intermediate_Dir "Debug\gr"
# PROP Intermediate_Dir "Debug\gr"
# ADD BASE MTL /I "..\lib\icadlib" /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /I "..\lib\icadlib" /tlb "grmodule.tlb" /h "grmodule.h"
# SUBTRACT MTL /mktyplib203

!ENDIF 

# End Source File
# End Target
# End Project

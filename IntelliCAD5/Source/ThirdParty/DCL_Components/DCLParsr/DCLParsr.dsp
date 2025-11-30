# Microsoft Developer Studio Project File - Name="DCLParsr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DCLParsr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DCLParsr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DCLParsr.mak" CFG="DCLParsr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DCLParsr - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DCLParsr - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DCLParsr - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /Ob2 /I ".\\" /I "..\PCCTS" /I "..\source" /I "..\include" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-Us" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../../../prj/make/release/dcl.lib /nologo /subsystem:windows /machine:I386 /OPT:NOREF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DCLParsr - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I ".\\" /I "..\PCCTS" /I "..\source" /I "..\include" /I "..\..\..\..\Eng-Us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\..\Eng-Us" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../../prj/make/debug/dcl.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DCLParsr - Win32 Release"
# Name "DCLParsr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Source\AttributeInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AuditLevel.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Button.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ControlID.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\DefaultAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\EditBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ErrorMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ExceptionManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\FileInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ImageButton.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\InactiveCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ListBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ListOfAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Main.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\NonDefinedNode.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\OpenDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Parser.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\PopUpList.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ScreenObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Slider.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Spacer.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Source\Text.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Tile.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TileAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Tree.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TreeDebugger.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TreeNode.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DCLParsr.ico
# End Source File
# Begin Source File

SOURCE=.\res\DCLParsr.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Error.ico
# End Source File
# Begin Source File

SOURCE=.\res\res\Error.ico
# End Source File
# End Group
# Begin Group "Pccts "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\PCCTS\AParser.cpp
# End Source File
# Begin Source File

SOURCE=..\PCCTS\AParser.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\AToken.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\ATokenBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\PCCTS\ATokenBuffer.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\ATokenStream.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\ATokPtr.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\config.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\DLexerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\PCCTS\DLexerBase.h
# End Source File
# Begin Source File

SOURCE=.\DLGLexer.cpp
# End Source File
# Begin Source File

SOURCE=.\DLGLexer.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\GrammarWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Grammer.cpp
# End Source File
# Begin Source File

SOURCE=.\Grammer.h
# End Source File
# Begin Source File

SOURCE=.\GrammerForDCL.g

!IF  "$(CFG)" == "DCLParsr - Win32 Release"

# Begin Custom Build
InputPath=.\GrammerForDCL.g

BuildCmds= \
	antlr.exe -CC -gl  -k 2 GrammerForDCL.g \
	dlg.exe -CC   parser.dlg \
	copy Grammer*.*    ..\PCCTS\Grammer*.* \
	copy DLGLexer.*  ..\PCCTS\DLGLexer.* \
	copy tokens.h  ..\PCCTS\tokens.h \
	

"GrammerForDCL.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Grammer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Grammer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"DLGLexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"DLGLexer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tokens.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "DCLParsr - Win32 Debug"

# Begin Custom Build
InputPath=.\GrammerForDCL.g

BuildCmds= \
	antlr.exe -CC -gl  -k 2 GrammerForDCL.g \
	dlg.exe -CC   parser.dlg \
	copy Grammer*.*    ..\PCCTS\Grammer*.* \
	copy DLGLexer.*  ..\PCCTS\DLGLexer.* \
	copy tokens.h  ..\PCCTS\tokens.h \
	

"GrammerForDCL.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Grammer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Grammer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"DLGLexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"DLGLexer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"tokens.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tokens.h
# End Source File
# End Group
# Begin Group "TestFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TestDCLFiles\123.dcl
# End Source File
# Begin Source File

SOURCE=.\TestDCLFiles\456.dcl
# End Source File
# Begin Source File

SOURCE=.\AttirbuteList.txt
# End Source File
# Begin Source File

SOURCE=.\TestDCLFiles\base.dcl
# End Source File
# Begin Source File

SOURCE=.\TestDCLFiles\ddmodify.dcl
# End Source File
# Begin Source File

SOURCE=.\Debug\Error.log
# End Source File
# Begin Source File

SOURCE=.\Release\Error.log
# End Source File
# Begin Source File

SOURCE=.\TestDCLFiles\Test.out
# End Source File
# Begin Source File

SOURCE=.\TestDCLFiles\testfile1.dcl
# End Source File
# Begin Source File

SOURCE=.\TestDCLFiles\visDCLTest.dcl
# End Source File
# Begin Source File

SOURCE=.\TestDCLFiles\xyz.dcl
# End Source File
# End Group
# Begin Group "DCLParsr Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DCLParsr.cpp
# End Source File
# Begin Source File

SOURCE=.\DCLParsr.rc
# End Source File
# Begin Source File

SOURCE=.\DCLParsrDlg.cpp
# End Source File
# End Group
# Begin Group "DCLParsr Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DCLParsr.h
# End Source File
# Begin Source File

SOURCE=.\DCLParsrDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Include\AttributeInfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\AuditLevel.h
# End Source File
# Begin Source File

SOURCE=..\Include\Button.h
# End Source File
# Begin Source File

SOURCE=..\Include\Constants.h
# End Source File
# Begin Source File

SOURCE=..\Include\ControlID.h
# End Source File
# Begin Source File

SOURCE=..\Include\DefaultAttributes.h
# End Source File
# Begin Source File

SOURCE=..\Include\Dialog.h
# End Source File
# Begin Source File

SOURCE=..\Include\EditBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\ErrorMessage.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExceptionManager.h
# End Source File
# Begin Source File

SOURCE=..\Include\FileInfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\ImageButton.h
# End Source File
# Begin Source File

SOURCE=..\Include\InactiveCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\ListBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\ListOfAttributes.h
# End Source File
# Begin Source File

SOURCE=..\Include\Main.h
# End Source File
# Begin Source File

SOURCE=..\Include\NonDefinedNode.h
# End Source File
# Begin Source File

SOURCE=..\Include\OpenDlg.h
# End Source File
# Begin Source File

SOURCE=..\Include\Parser.h
# End Source File
# Begin Source File

SOURCE=..\Include\PopUpList.h
# End Source File
# Begin Source File

SOURCE=..\Include\ScreenObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\sds.h
# End Source File
# Begin Source File

SOURCE=..\Include\Slider.h
# End Source File
# Begin Source File

SOURCE=..\Include\Spacer.h
# End Source File
# Begin Source File

SOURCE=..\Include\Text.h
# End Source File
# Begin Source File

SOURCE=..\Include\Tile.h
# End Source File
# Begin Source File

SOURCE=..\Include\TileAttributes.h
# End Source File
# Begin Source File

SOURCE=..\Include\Tree.h
# End Source File
# Begin Source File

SOURCE=..\Include\TreeDebugger.h
# End Source File
# Begin Source File

SOURCE=..\Include\TreeNode.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Include\base.dcl
# End Source File
# Begin Source File

SOURCE=..\include\primitives.dcl
# End Source File
# Begin Source File

SOURCE=.\print.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="DCLTest_UI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DCLTest_UI - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DCLTest_UI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DCLTest_UI.mak" CFG="DCLTest_UI - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DCLTest_UI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DCLTest_UI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DCLTest_UI - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /Od /Ob2 /I ".\\" /I "..\include" /I "..\source" /I "..\PCCTS" /I "..\DCLTest_UI" /I "../../../../eng-us" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../../../prj/make/release/dcl.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DCLTest_UI - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /Ob2 /I ".\\" /I "..\include" /I "..\source" /I "..\PCCTS" /I "..\DCLTest_UI" /I "../../../../eng-us" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_TESTEXE" /FR /FD /GZ /c
# SUBTRACT CPP /WX /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../../prj/make/debug/dcl.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DCLTest_UI - Win32 Release"
# Name "DCLTest_UI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CtrlListAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\DCL.cpp
# End Source File
# Begin Source File

SOURCE=.\DCLDialogList.cpp
# End Source File
# Begin Source File

SOURCE=.\DCLDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DCLView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CtrlListAttributes.h
# End Source File
# Begin Source File

SOURCE=.\DCL.h
# End Source File
# Begin Source File

SOURCE=.\DCLDialogList.h
# End Source File
# Begin Source File

SOURCE=.\DCLDoc.h
# End Source File
# Begin Source File

SOURCE=.\DCLView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DCL.ico
# End Source File
# Begin Source File

SOURCE=.\DCL.rc
# End Source File
# Begin Source File

SOURCE=.\res\DCLDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Error.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "PCCTS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Pccts\AParser.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\AParser.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\AToken.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\ATokenBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\ATokenBuffer.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\ATokenStream.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\ATokPtr.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\config.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\DLexerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\DLexerBase.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\DLGLexer.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\DLGLexer.h
# End Source File
# Begin Source File

SOURCE=..\PCCTS\GrammarWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\Grammer.cpp
# End Source File
# Begin Source File

SOURCE=..\Pccts\Grammer.h
# End Source File
# Begin Source File

SOURCE=..\Pccts\tokens.h
# End Source File
# End Group
# Begin Group "Src"

# PROP Default_Filter ""
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

SOURCE=..\Source\Text.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Tile.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TileAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TileAttributes.h
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
# Begin Group "SrcHeader"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\Include\Button.h
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

SOURCE=..\Include\Primitives.dcl
# End Source File
# End Target
# End Project

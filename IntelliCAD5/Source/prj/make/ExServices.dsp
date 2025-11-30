# Microsoft Developer Studio Project File - Name="ExServices" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ExServices - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ExServices.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ExServices.mak" CFG="ExServices - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ExServices - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ExServices - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ExServices - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\ExServices"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /O1 /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gs" /I "..\..\Licensed\DWGdirect\include\gi" /D "NDEBUG" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "NON_VISIO_APP" /D "INTELLICAD" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\ExService.lib"

!ELSEIF  "$(CFG)" == "ExServices - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ExServices___Win32_Debug"
# PROP BASE Intermediate_Dir "ExServices___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\ExServices"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /Gf /Gy /I "..\..\Licensed\DWGdirect\include" /I "..\..\Licensed\DWGdirect\include\ge" /I "..\..\Licensed\DWGdirect\include\br" /I "..\..\Licensed\DWGdirect\include\gi" /I "..\..\Licensed\DWGdirect\include\gs" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "NON_VISIO_APP" /D "INTELLICAD" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\ExService.lib"

!ENDIF 

# Begin Target

# Name "ExServices - Win32 Release"
# Name "ExServices - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExGiEnhancedMetafile.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExGiRasterImage.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExHostAppServices.h
# End Source File
# Begin Source File

SOURCE=..\..\Licensed\DWGdirect\ExServices\ExOdStorage.h
# PROP Exclude_From_Build 1
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
# End Target
# End Project

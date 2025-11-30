# Microsoft Developer Studio Project File - Name="marcomplib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=marcomplib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "marcomplib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "marcomplib.mak" CFG="marcomplib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "marcomplib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "marcomplib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "marcomplib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\ThirdParty\marcomp" /D "NDEBUG" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "NODISGUISE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "marcomplib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\marcomplib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\ThirdParty\marcomp" /D "_DEBUG" /D "AD_VM_WINNT" /D "microsoftc" /D "WINNT" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "NODISGUISE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "marcomplib - Win32 Release"
# Name "marcomplib - Win32 Debug"
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\AD2.H
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADAUDIT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADBLOB.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADBTREE.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADCALLSR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDATA.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDEFAUL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDWGR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDXFDAT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADDXFR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADEDDTBL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADERROR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADFONTS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADFREEEM.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADINCLUD.H
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADLL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADNEW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRAPP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRAPPDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLHDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLKDF.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLKDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLKH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRBLRDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRCLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRCLSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDIC.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDICDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDIMDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDIMST.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRDWOBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRENTDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRENTFN.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRFNSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRFRD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRFRD2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRH2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRHDRDF.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRHDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLAY.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLAYDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLTP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRLTPDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRMLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRMLSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADROBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADROBJDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRRAS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRSHP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRSHPDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRSTEND.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRTBLDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRTBLLD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRUCS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRUCSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRUTILS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVIEDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVIEW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPEDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPODX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRVPORT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADRXD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADSNIFF.C
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\adstream.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\adstream.h
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADTEXT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADTJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVAPP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVBLKH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVCLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVDIC.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVDIMST.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVFILE.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVLAY.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVLD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVLTP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVMLS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVOBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVSHP.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVUCS.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVVIEW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVVPORT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADVXD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW13ENT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW13TBL.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW2DX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW3.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW4.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADW5.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWDWOBJ.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWENDDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWENTDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFILE1.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFILE2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFNSDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFWR.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWFWR2.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWH.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWHAND.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWHDX.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWINIT.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWXD.C
# End Source File
# Begin Source File

SOURCE=..\..\ThirdParty\MARCOMP\ADWZOMB.C
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\filestream.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\MARCOMP\filestream.h
# End Source File
# End Target
# End Project

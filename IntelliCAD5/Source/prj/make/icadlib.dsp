# Microsoft Developer Studio Project File - Name="icadlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=icadlib - Win32 Bounds Checker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "icadlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "icadlib.mak" CFG="icadlib - Win32 Bounds Checker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "icadlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "icadlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "icadlib - Win32 Browse" (based on "Win32 (x86) Static Library")
!MESSAGE "icadlib - Win32 Minimal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "icadlib - Win32 True Time" (based on "Win32 (x86) Static Library")
!MESSAGE "icadlib - Win32 Bounds Checker" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "icadlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release\icadlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "NDEBUG" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /YX"icadlib.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "icadlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\OutputDir\icad5\Debug"
# PROP Intermediate_Dir "\OutputDir\icad5\Debug\icadlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "DEBUG" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /FR /YX"icadlib.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\icadlib.lib"

!ELSEIF  "$(CFG)" == "icadlib - Win32 Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "icadlib_"
# PROP BASE Intermediate_Dir "icadlib_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\icadlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\icadlib" /I "..\lib\sds" /D "DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_AFXDLL" /D "_MBCS" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /FR /YX"icadlib.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "DEBUG" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /FR /YX"icadlib.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "icadlib - Win32 Minimal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "icadlib_"
# PROP BASE Intermediate_Dir "icadlib_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinDebug"
# PROP Intermediate_Dir "MinDebug\icadlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "..\lib\icadlib" /I "..\api\sds" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /YX"icadlib.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /Zi /O2 /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "SDS_ICADMFC" /D "SDS_OURDWGDB" /D "NDEBUG" /D "MINDEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /YX"icadlib.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "icadlib - Win32 True Time"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "True_Time"
# PROP BASE Intermediate_Dir "True_Time"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "True_Time"
# PROP Intermediate_Dir "True_Time\icadlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "DEBUG" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /FR /YX"icadlib.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "DEBUG" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /FR /YX"icadlib.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "icadlib - Win32 Bounds Checker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Bounds_Checker"
# PROP BASE Intermediate_Dir "Bounds_Checker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Bounds_Checker"
# PROP Intermediate_Dir "Bounds_Checker\icadlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "DEBUG" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /FR /YX"icadlib.h" /FD /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /Gi /GX /Zi /Od /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\lib\auto" /I "..\..\thirdparty\geo" /D "DEBUG" /D "SDS_SDSSTANDALONE" /D "_DEBUG" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "SDS_NEWDB" /D "WIN32" /D "_WINDOWS" /FR /YX"icadlib.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "icadlib - Win32 Release"
# Name "icadlib - Win32 Debug"
# Name "icadlib - Win32 Browse"
# Name "icadlib - Win32 Minimal Debug"
# Name "icadlib - Win32 True Time"
# Name "icadlib - Win32 Bounds Checker"
# Begin Group "Sources"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\lib\ICADLIB\2pta2arc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\3dxseg.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\3pt2arc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Adscode.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\allocator.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Angpar.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Arbaxis.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Arcextns.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Arcxarc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Assoc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Atan2.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Bulg2arc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\changeslash.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Checkfp.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\crossprod.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\delayload.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Divmeas.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Dotstr.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Ellpts.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Encoding.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\EncodingPP.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Endquote.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\EntConversion.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\entxss.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Escconv.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\filexten.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Gcd.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\gvector.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Handmath.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\hiddenwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Inorout.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Isnumstr.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Isvalidc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Isvalidn.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Linexarc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Linxlin.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Llarea.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\lockableobject.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Lpad.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Makebak.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Mkprimes.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Nfgets.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Normang.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Ortho.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\polar.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Primefac.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Ptlndist.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Ptsgdist.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Resbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Resval.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\rotaxis.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Rpad.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\segdist.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\segxseg.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\setext.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\singlethreadsection.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\Icadlib\SmartPointer.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\ss2dexts.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Sscpy.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Ssxss.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Stristr.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Strncpy.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\transf.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Trim.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\TTFInfoGetter.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Usemat.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\vecutils.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\win32misc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\xformutils.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\xmldoc.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\XmlNode.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\XmlStream.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\lib\ICADLIB\allocator.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\delayload.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Encoding.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\EncodingPP.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\EntConversion.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\greedyheap.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\gvector.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\gvector.inl
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\hiddenwindow.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\ic_dynarray.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Icaddef.h
# End Source File
# Begin Source File

SOURCE=..\Lib\Icadlib\Icadlib.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\lockableobject.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\matrix.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\matrix.inl
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\ntype.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\Resbuf.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\safecounter.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\SelectionSet.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\singlethreadsection.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\transf.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\TTFInfoGetter.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\values.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\win32misc.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\xml.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\xmldoc.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\XmlNode.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\XmlStream.h
# End Source File
# Begin Source File

SOURCE=..\lib\ICADLIB\xstd.h
# End Source File
# End Group
# End Target
# End Project

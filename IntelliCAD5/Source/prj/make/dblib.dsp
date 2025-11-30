# Microsoft Developer Studio Project File - Name="dblib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dblib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dblib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dblib.mak" CFG="dblib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dblib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dblib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dblib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dblib___Win32_Release"
# PROP BASE Intermediate_Dir "dblib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "\OutputDir\Icad5\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\lib\sds" /I "..\lib\dcl" /I "..\..\ThirdParty\marcomp" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "NDEBUG" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "INTELLICAD" /D "DBSTATIC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dblib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dblib___Win32_Debug"
# PROP BASE Intermediate_Dir "dblib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\dblib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\lib\sds" /I "..\lib\dcl" /I "..\..\ThirdParty\marcomp" /I "..\lib\icadlib" /I "..\api\sds" /I "..\lib\db" /I "..\..\ThirdParty\geo" /I "..\lib\cmds" /D "DEBUG" /D "_DEBUG" /D "_AFXDLL" /D "IC_NOSTRINGSTUFF" /D "SDS_UDSOURDCL" /D "SDS_USEOURADS" /D "SDS_CADAPI" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "SDS_NEWDB" /D "INTELLICAD" /D "LIB" /D "DBSTATIC" /YX /FD /GZ /c
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

# Name "dblib - Win32 Release"
# Name "dblib - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "NotPrecompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lib\DB\db1.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\DB\3dface.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\3dsolid.cpp
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

SOURCE=..\lib\DB\body.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\circle.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\class.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\db_util.cpp
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

SOURCE=..\lib\DB\getendpt.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\group.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handitem.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handletable.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\hatch.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\header.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\header_ctor_dtor.cpp
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

SOURCE=..\lib\DB\layout.cpp
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

SOURCE=..\lib\DB\mtext.cpp
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

SOURCE=..\lib\DB\plotsettings.cpp
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

SOURCE=..\lib\DB\region.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\resourcestring.cpp
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

SOURCE=..\lib\DB\sortentstable.cpp
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

SOURCE=..\lib\DB\sysvar.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\sysvar_ctor_dtor.cpp
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

SOURCE=..\lib\DB\tolerance.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\trace.cpp
# End Source File
# Begin Source File

SOURCE=..\lib\DB\ucstabrec.cpp
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

# PROP Default_Filter ""
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

SOURCE=..\lib\DB\dbdefines.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbdllloadinfo.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbprotos.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\dbrc.h
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

SOURCE=..\lib\DB\fixup1005.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\fixuptable.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\fontlink.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handitem.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\handletable.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\header.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\header_search.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\header_search_defs.h
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

SOURCE=..\lib\DB\objects.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\objhandle.h
# End Source File
# Begin Source File

SOURCE=..\lib\DRW\odtver.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\reactor.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\styletabrec.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\sysvar.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\sysvar_search.h
# End Source File
# Begin Source File

SOURCE=..\lib\DB\sysvar_search_defs.h
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
# End Target
# End Project

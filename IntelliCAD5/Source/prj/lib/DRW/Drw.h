//********************************************************************
//*      Copyright (C) 1994, 1995, 1996, 1997, 1998 by Visio Corporation *
//********************************************************************

#pragma once
#ifndef _DRW_H
#define DRW_H

#if defined(DRW_DEFVARS)
    #define DRW_EXTERN
#else
    #define DRW_EXTERN extern
#endif

// Forward declarations of anonymous types
class db_charbuflink;
class db_drawing;
class db_handitem;

class DB_CLASS drw_readwrite
{
public:
	drw_readwrite(bool bCheckOnLoad = true);
	virtual ~drw_readwrite();

public:
	// Gerneral reader/writer calls other versions
	void drw_setconfigbuf(db_charbuflink *CurConfig) {
		m_CurConfig=CurConfig;
	}

//	int drw_getfileversion(char *pn,int *filetype,int *version);
	// TODO Someday when we have our own readers/writers we need to check
	//      the version and type and call the correct reader or writer.
	db_drawing *drw_openfile(db_drawing *argdp, char *pn, int *rcp,bool convertplines,bool useprog=TRUE, short recovering=0) {
		if(useprog) db_progressstart();
		db_drawing *dp=drw_DWGdirect_Open(argdp,pn,rcp,convertplines, recovering);
		if(useprog) db_progresspercent(100);
		if(useprog) db_progressstop();
		return(dp);
	}
	int drw_savefile(db_drawing *dp,long arghipct,db_handitem **arghipp,char *savename,char filetype,int version,short dxfnegz,short dxfdecprec,short dxfwritezeroes,char r12dxfvbls,int argmakebak,bool useprog=TRUE, bool usePasswd=false, bool savePreview=true) {
		if(useprog) db_progressstart();
		int fi1=drw_DWGdirect_Save(dp,arghipct,arghipp,savename,filetype,version,dxfnegz,dxfdecprec,dxfwritezeroes,r12dxfvbls,argmakebak,m_CurConfig, usePasswd, savePreview);
		if(useprog) db_progresspercent(100);
		if(useprog) db_progressstop();
		return(fi1);
	}
	db_drawing* drw_auditfile(db_drawing *dp,bool fixErrors, short writeLogFile, bool useprog=TRUE) {
		if(useprog) db_progressstart();
		db_drawing *pDWG = drw_DWGdirect_Audit(dp,fixErrors, writeLogFile);
		if(useprog) db_progresspercent(100);
		if(useprog) db_progressstop();
		return(pDWG);
	}
	bool drw_getdwgbitmap(const char *pFielName, void* &pBuffer, unsigned long &bytes, bool &isBmp);

private:
	db_charbuflink *m_CurConfig;
	bool m_bCheckOnLoad;

	// DWGdirect readers/writers
	db_drawing *drw_DWGdirect_Open(db_drawing *argdp, char *pn, int *rcp, bool convertplines, short bIsRecovering);
	int drw_DWGdirect_Save(db_drawing *dp,long arghipct,db_handitem **arghipp,char *savename,char filetype,int version,short dxfnegz,short dxfdecprec,short dxfwritezeroes,char r12dxfvbls,int argmakebak,db_charbuflink *CurConfig, bool bUsePasswd, bool bSavePreview);
	db_drawing *drw_DWGdirect_Audit(db_drawing *dp,bool fixErrors, short writeLogFile);

};

#endif // _DRW_H



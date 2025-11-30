// ***************************************************
// SDS_ENGINE.H
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
// Declares real Engine calls
//
//

#ifndef _SDS_ENGINE_H
#define _SDS_ENGINE_H

// ***************************************************************
// ***************************************************************
// ***************************************************************
// ***************************************************************
// This function is called on every entry point.  It can be used for
// state validation, or simply as an easy place to trace calls into the
// api
//
//
//	Note: The original function call has been replaced by a macro which instantiates
//	a state class deriving from the AFX_MANAGE_STATE2. This assures all entry via
//	sds participates in the AFX MODULE state convention.



class SDS_MANAGE_STATE	: AFX_MAINTAIN_STATE2
	{
public:
	SDS_MANAGE_STATE() : AFX_MAINTAIN_STATE2( AfxGetAppModuleState())
		{
#ifndef NDEBUG
		::InterlockedIncrement( &ziEntryCount );
#endif
		}

private:
	static long ziEntryCount;
	};


#define	SDSAPI_Entry()	SDS_MANAGE_STATE	_sds_state



int				   sdsengine_agetcfg(const char *szSymbol, char  *szVariable);   // Not documented
int				   sdsengine_agetenv(const char *szSymbol, char  *szVariable);   // Not documented
double             sdsengine_angle(const sds_point ptStart, const sds_point ptEnd);
int                sdsengine_angtof(const char *szAngle, int nUnitType, double *pdAngle);
int				   sdsengine_angtof_absolute(const char *szAngle, int nUnitType, double *pdAngle);
int                sdsengine_angtos(double dAngle, int nUnitType, int nPrecision, char *szAngle);
int                sdsengine_angtos_end(double dAngle, int nUnitType, int nPrecision, char *szAngle);
int                sdsengine_angtos_convert(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle);
int                sdsengine_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle, int nDimazin, int nDimdesp ); // EBATECH #78443 DIMAZIN 2003/3/31 DIMDSEP
//int                sdsengine_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle);
int				   sdsengine_angtos_absolute(double dAngle, int nUnitType, int nPrecision, char *szAngle);
int				   sdsengine_arxload(const char *szARXProgram);
struct sds_resbuf *sdsengine_arxloaded(void);
int				   sdsengine_arxunload(const char *szARXProgram);
int                sdsengine_asetcfg(const char *szSymbol, const char *szValue);   // Not documented
int				   sdsengine_asetenv(const char *szSymbol, const char *szValue);   // Not documented

int					sdsengine_bmpout(const char* pFile, int width = 0, int height = 0); /*DG - 24.9.2002*/

int                sdsengine_callinmainthread(int (*fnDragEnts)(void *),void *pUserData);

int                sdsengine_cmd(const struct sds_resbuf *prbCmdList);
int                sdsengine_cvunit(double dOldNum, const char *szOldUnit, const char *szNewUnit, double *pdNewNum);

int                sdsengine_defun(const char *szFuncName, int nFuncCode);	
int				   sdsengine_dictadd(const sds_name nmDict, const char *szAddThis, const sds_name nmNonGraph);  // ??
int				   sdsengine_dictdel(const sds_name nmDict, const char *szDelThis);
struct sds_resbuf *sdsengine_dictnext(const sds_name nmDict, int swFirst);
int				   sdsengine_dictrename(const sds_name nmDict, const char *szOldName, const char *szNewName);
struct sds_resbuf *sdsengine_dictsearch(const sds_name nmDict, const char *szFindThis, int swFirst);
double			   sdsengine_distance(const sds_point ptFrom, const sds_point ptTo);
struct sds_dobjll *sdsengine_dispobjs(const sds_name nmEntity,int nDispMode);
int                sdsengine_distof(const char *szDistance, int nUnitType, double *pdDistance);
int                sdsengine_draggen(const sds_name nmSelSet, const char *szDragMsg, int nCursor, int (*fnDragEnts)(sds_point ptCursorLoc, sds_matrix mxTransform), sds_point ptDestPoint);	//TODO

int                sdsengine_entdel(const sds_name nmEntity);
struct sds_resbuf *sdsengine_entget(const sds_name nmEntity);
struct sds_resbuf *sdsengine_entgetx(const sds_name nmEntity, const struct sds_resbuf *prbAppList);
int                sdsengine_entlast(sds_name nmLastEnt);
int                sdsengine_entmake(const struct sds_resbuf *prbEntList);
int				   sdsengine_entmakex(const struct sds_resbuf *prbEntList, sds_name nmNewEnt);
int                sdsengine_entmod(const struct sds_resbuf *prbEntList);
int	               sdsengine_entnext_withxref(const sds_name nmKnownEnt, sds_name nmNextEnt);
int                sdsengine_entsel(const char *szSelectMsg, sds_name nmEntity, sds_point ptSelected, bool bDirectCall = true );
int                sdsengine_entupd(const sds_name nmEntity);

void               sdsengine_fail(const char *szFailMsg);
void			   sdsengine_freedispobjs(struct sds_dobjll *pDispObjs);

int                sdsengine_getangle(const sds_point ptStart, const char *szAngleMsg, double *pdRadians);
struct sds_resbuf *sdsengine_getargs(void);
int				   sdsengine_getcfg(const char *szSymbol, char  *szVariable, int nLength);  
int                sdsengine_getcname(const char *szOtherLang, char **pszEnglish);
int                sdsengine_getcorner( const sds_point ptStart, const char *szCornerMsg, sds_point ptOpposite, bool bApplySnapping = false, bool bDirectCall = true );
int                sdsengine_getdist(const sds_point ptStart, const char *szDistMsg, double *pdDistance);
struct sds_resbuf *sdsengine_getdoclist(void);
int                sdsengine_getfiled(const char *szTitle, const char *szDefaultPath, const char *szExtension, int bsOptions, struct sds_resbuf *prbFileName);
int                sdsengine_getfuncode(void);
int                sdsengine_getinput(char *szEntry);
int                sdsengine_getint(const char *szIntMsg, int *pnInteger);
int                sdsengine_getkword(const char *szKWordMsg, char *szKWord);
int                sdsengine_getorient(const sds_point ptStart, const char *szOrientMsg, double *pdRadians);
int                sdsengine_getpoint( const sds_point ptReference, const char *szPointMsg, sds_point ptPoint, bool bDirectCall = true );
int                sdsengine_getreal(const char *szRealMsg, double *pdReal);
int                sdsengine_getstring( int swSpaces, const char *szStringMsg, char *szString, bool bDirectCall = true );
int                sdsengine_getsym(const char *szSymbol, struct sds_resbuf **pprbSymbolInfo);
int                sdsengine_gettbpos(const char *pToolBarName, sds_point ptTbPos);
int                sdsengine_getvar(const char *szSysVar, struct sds_resbuf *prbVarInfo);
int                sdsengine_graphscr(void);
int                sdsengine_grclear(void);
int		    	   sdsengine_grarc(const sds_point ptCenter, double dRadius, double dStartAngle, double dEndAngle, int nColor, int swHighlight);
int		    	   sdsengine_grfill(const sds_point *pptPoints, int nNumPoints, int nColor, int swHighlight);
int                sdsengine_grdraw(const sds_point ptFrom, const sds_point ptTo, int nColor, int swHighlight);
int                sdsengine_grread(int bsAllowed, int *pnInputType, struct sds_resbuf *prbInputValue);
int                sdsengine_grtext(int nWhere, const char *szTextMsg, int swHighlight);
int                sdsengine_grvecs(const struct sds_resbuf *prbVectList, sds_matrix mxDispTrans);

int                sdsengine_handent(db_objhandle EntHandle, sds_name nmEntity);

int                sdsengine_help(char *szHelpFile, char *szContextID, int nMapNumber);

int                sdsengine_initget(int bsAllowed, const char *szKeyWordList);
int                sdsengine_inters(const sds_point ptFrom1, const sds_point ptTo1, const sds_point ptFrom2, const sds_point ptTo2, int swFinite, sds_point ptIntersection);
int                sdsengine_invoke(const struct sds_resbuf *prbArguments,    struct sds_resbuf **pprbReturn);


struct sds_resbuf *sdsengine_loaded(void);

int                sdsengine_menucmd(const char *szPartToDisplay);
int                sdsengine_menugroup(char *pMemuGroupName);

int	               sdsengine_namedobjdict(sds_name nmDict);
int                sdsengine_nentsel(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, sds_point ptECStoWCS[4], struct sds_resbuf **pprbNestBlkList);
int                sdsengine_nentselp(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, int swUserPick, sds_matrix mxECStoWCS, struct sds_resbuf **pprbNestBlkList);
struct sds_resbuf *sdsengine_newrb(int nTypeOrDXF);

int                sdsengine_osnap(const sds_point ptAperCtr, const char *szSnapModes, sds_point ptPoint);

void               sdsengine_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint);
int                sdsengine_progresspercent(int iPercentDone);	
int				   sdsengine_progressstart(void);	
int                sdsengine_progressstop(void);	
int                sdsengine_prompt(const char *szPromptMsg);
int                sdsengine_putsym(const char *szSymbol, struct sds_resbuf *prbSymbolInfo);

int				   sdsengine_readaliasfile(char *szAliasFile);
int                sdsengine_redraw(const sds_name nmEntity, int nHowToDraw);
int                sdsengine_regapp(const char *szApplication);
int                sdsengine_regappx(const char *szApplication, int swSaveAsR12); // I have no idea is 2nd param is right.
int                sdsengine_regfunc(int (*nFuncName)(void), int nFuncCode);	
int                sdsengine_relrb(struct sds_resbuf *prbReleaseThis);
int                sdsengine_retint(int nReturnInt);
int                sdsengine_retlist(const struct sds_resbuf *prbReturnList);
int                sdsengine_retname(const sds_name nmReturnName, int nReturnType);
int                sdsengine_retnil(void);
int                sdsengine_retpoint(const sds_point ptReturn3D);
int                sdsengine_retreal(double dReturnReal);
int                sdsengine_retstr(const char *szReturnString);
int                sdsengine_rett(void);
int                sdsengine_retval(const struct sds_resbuf *prbReturnValue);
int                sdsengine_retvoid(void);
int				   sdsengine_rp2pix(double dNumberX,double dNumberY,int *pPixelX,int *pPixelY);
int                sdsengine_rtos(double dNumber, int nUnitType, int nPrecision, char *szNumber, int iDimType);

int                sdsengine_sendmessage(char *szCommandMsg);
int                sdsengine_setcallbackfunc(int (*cbfnptr)(int flag,void *arg1,void *arg2,void *arg3));
int                sdsengine_setfunhelp(char *szFunctionName, char *szHelpFile, char *szContextID, int nMapNumber);
int                sdsengine_setvar(const char *szSysVar, const struct sds_resbuf *prbVarInfo, bool bCheckReadOnly = false );
int                sdsengine_setview(const struct sds_resbuf *prbViews, int nWhichVPort); 
int                sdsengine_snvalid(const char *szTableName, int swAllowPipe);
int                sdsengine_ssadd(const sds_name nmEntToAdd, const sds_name nmSelSet, sds_name nmNewSet);
int                sdsengine_ssdel(const sds_name nmEntToDel, const sds_name nmSelSet);
int                sdsengine_ssfree(sds_name nmSetToFree);
int                sdsengine_ssget(const char *szSelMethod, const void *pFirstPoint, const void *pSecondPoint, const struct sds_resbuf *prbFilter, sds_name nmNewSet);	
int                sdsengine_ssgetfirst(struct sds_resbuf **pprbHaveGrips, struct sds_resbuf **pprbAreSelected);
int                sdsengine_sslength(const sds_name nmSelSet, long *plNumberOfEnts);
int                sdsengine_ssmemb(const sds_name nmEntity, const sds_name nmSelSet);
int                sdsengine_ssname(const sds_name nmSelSet, long lSetIndex, sds_name nmEntity);
int                sdsengine_ssnamex(struct sds_resbuf **pprbEntName, const sds_name nmSelSet, const long iIndex);
int                sdsengine_sssetfirst(const sds_name nmGiveGrips, const sds_name nmSelectThese);
int                sdsengine_swapscreen(void);

int                sdsengine_tablet (const struct sds_resbuf *prbGetOrSet, struct sds_resbuf **pprbCalibration);
struct sds_resbuf *sdsengine_tblnext(const char *szTable, int swFirst);
int	               sdsengine_tblobjname(const char *szTable, const char *szEntInTable, sds_name nmEntName);
struct sds_resbuf *sdsengine_tblsearch(const char *szTable, const char *szFindThis, int swNextItem);
int                sdsengine_textbox(const struct sds_resbuf *prbTextEnt, sds_point ptCorner, sds_point ptOpposite);	
int                sdsengine_textpage(void);
int                sdsengine_textscr(void);
int                sdsengine_tolower(int nASCIIValue);
int                sdsengine_toupper(int nASCIIValue);
int                sdsengine_trans(const sds_point ptVectOrPtFrom, const struct sds_resbuf *prbCoordFrom, const struct sds_resbuf *prbCoordTo, int swVectOrDisp, sds_point ptVectOrPtTo);

int				   sdsengine_ucs2rp(sds_point ptSour3D,sds_point ptDest3D);
int                sdsengine_update(int nWhichVPort,const sds_point ptCorner1,const sds_point ptCorner2);
int                sdsengine_undef(const char *szFuncName, int nFuncCode);
int                sdsengine_usrbrk(void);

int                sdsengine_vports(struct sds_resbuf **prbViewSpecs);	

int                sdsengine_wcmatch(const char *szCompareThis, const char *szToThis);

int                sdsengine_xdroom(const sds_name nmEntity, long *plMemAvail);
int                sdsengine_xdsize(const struct sds_resbuf *prbEntData, long *plMemUsed);
int                sdsengine_xformss(const sds_name nmSetName, sds_matrix mxTransform);
int                sdsengine_xload(const char *szApplication);
int                sdsengine_xstrcase(char *szString);   // Not documented
char              *sdsengine_xstrsave(char *szSource, char **pszDest);   // Not documented
int                sdsengine_xunload(const char *szApplication); 



	HDC *sdsengine_getviewhdc(void);
	int sdsengine_getrgbvalue(int nColor);
	HPALETTE sdsengine_getpalette(void);
	HWND sdsengine_getviewhwnd(void); 
	HWND sdsengine_getmainhwnd(void); 
	int sdsengine_getpreviewbmp(const char *szFileName, void** pHBITMAP, unsigned long* pAllocatedBytes, short* bIsBitmap);


#endif // _SDS_ENGINE_H


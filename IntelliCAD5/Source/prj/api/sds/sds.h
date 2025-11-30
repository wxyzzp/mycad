/*	C:\DEV\LIB\SDS\SDS.H - SDS Api defines and structures.
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *  This header file ships with the product. Third party developers
 *  need this with the sds.lib to create external sds applications.
 *
 *
 */


#pragma once
#ifndef _SDS_H
#define _SDS_H

// Includes
#ifndef _INC_STDLIB
	#include <stdlib.h>
#endif

#if defined(__cplusplus)
#ifndef	EXTERN_C
	#define EXTERN_C	extern "C"
#endif
	extern "C"
	{
#endif

#ifndef	EXTERN_C
	#define EXTERN_C
#endif

/* The following defines are for compatibility only. */
#define adsart(a)  short code=(a)
#define _(a)       a


typedef double				sds_real;
typedef sds_real			sds_point[3];
typedef long				sds_name[2];
typedef sds_real			sds_matrix[4][4];
typedef sds_real			*sds_pointp;
typedef long				*sds_namep;
typedef struct _sds_hdlg	*sds_hdlg;
typedef struct _sds_htile	*sds_htile;

typedef struct {
    sds_hdlg  dialog;
    sds_htile tile;
    char*     value;
    void*     client_data;
    int       reason;
    long      x, y;
} sds_callback_packet;

struct sds_binary {
    long clen;
    char *buf;
};

union sds_u_val {
   sds_real rreal;
   sds_real rpoint[3];
   short rint;
   char *rstring;
   long rlname[2];
   long rlong;
   struct sds_binary rbinary;
};

struct sds_resbuf {
    struct sds_resbuf *rbnext;
    short restype;
    union sds_u_val resval;
};
// EBATECH(CNBR) Expand struct member and size. 2003/5/5
struct sds_dobjll {
    char         type;
    short        color;
#ifdef BUILD_WITH_NEW_SDS_DOBJLL
    short        lweight;
    int          npts;      // short to int
#else
    short         npts;
#endif
    sds_real    *chain;
    struct sds_dobjll *next;
};
struct sds_blockTree {
struct	sds_blockTree *p_next,*p_contents;
//	sds_name ename;
	char *name;		// block name
	char *path;		// NULL for block, actual file name for xref(not name of xref)
	sds_name entity_name	// entity's SDS internal name
			,tblrec_name;	// The record's SDS internal name in block table
	int type;
	void *bitmap,*wmf;	// bitmap & WMF image buffer
	int bmpsz,wmfsz;	// bitmap & WMF buffer size
	void *p_hook;		/* The hook for adding expanded data to instances of this structure.
						   Isn't used by SDS functions. You can use it at own discretion.
						   Is initialized as NULL.
						   (!) sds_relBlockTree() will not delete p_hook.
						*/
};

#define SDS_X             0
#define SDS_Y             1
#define SDS_Z             2
#define SDS_T             3

#define SDS_TRUE          1
#define SDS_FALSE         0

#define SDS_PAUSE         "\\"

#define SDS_RSG_NONULL    0x01
#define SDS_RSG_NOZERO    0x02
#define SDS_RSG_NONEG     0x04
#define SDS_RSG_NOLIM     0x08
#define SDS_RSG_GETZ      0x10
#define SDS_RSG_DASH      0x20
#define SDS_RSG_2D        0x40
#define SDS_RSG_OTHER     0x80
#define SDS_RSG_NOPMENU   0x100
#define SDS_RSG_NOCANCEL  0x200
#define SDS_RSG_NOVIEWCHG 0x400
#define SDS_RSG_NODOCCHG  0x800

#define SDS_RQSAVE        14
#define SDS_RQEND         15
#define SDS_RQQUIT        16
#define SDS_RQCFG         22
#define SDS_RQXLOAD       100
#define SDS_RQXUNLD       101
#define SDS_RQSUBR        102
#define SDS_RQHUP         105
#define SDS_RQXHELP       118

#define SDS_RSRSLT        1
#define SDS_RSERR         3
#define SDS_RTERROR       -5001
#define SDS_RTCAN         -5002
#define SDS_RTREJ         -5003
#define SDS_RTFAIL        -5004
#define SDS_RTKWORD       -5005
#define SDS_RTNONE        5000
#define SDS_RTREAL        5001
#define SDS_RTPOINT       5002
#define SDS_RTSHORT       5003
#define SDS_RTANG         5004
#define SDS_RTSTR         5005
#define SDS_RTENAME       5006
#define SDS_RTPICKS       5007
#define SDS_RTORINT       5008
#define SDS_RT3DPOINT     5009
#define SDS_RTLONG        5010
#define SDS_RTVOID        5014
#define SDS_RTLB          5016
#define SDS_RTLE          5017
#define SDS_RTDOTE        5018
#define SDS_RTNIL         5019
#define SDS_RTDXF0        5020
#define SDS_RTT           5021
#define SDS_RTNORM        5100
#define SDS_RTBINARY      5022

#define COND_OP_CODE		-4

#define SDS_RTDRAGPT      5500

#define SDS_CBR_SELECT       1
#define SDS_CBR_LOST_FOCUS   2
#define SDS_CBR_DRAG         3
#define SDS_CBR_DOUBLE_CLICK 4

#define SDS_DLGALLDONE   -1
#define SDS_DLGCANCEL     0
#define SDS_DLGOK         1
#define SDS_DLGSTATUS     2

#define SDS_BGLCOLOR      -2
#define SDS_DBGLCOLOR     -15
#define SDS_DFGLCOLOR     -16
#define SDS_LINELCOLOR    -18

#define SDS_LIST_CHANGE   1
#define SDS_LIST_APPEND   2
#define SDS_LIST_NEW      3

/*================================
	 SDS xref actions 0 - 16
 ================================*/
#define SDS_XREF_FIND		0
#define SDS_XREF_ATTACH     1
#define SDS_XREF_INSERT     2
#define SDS_XREF_EDIT_PATH	3
#define SDS_XREF_RELOAD		4
#define SDS_XREF_UNLOAD		5
#define SDS_XREF_DETACH		6
#define SDS_XREF_RENAME		7
/*================================
	 SDS xref action flags
 ================================*/
#define SDS_XREF_OVERLAY	0x10
#define SDS_XREF_CROP_PATH	0x20
#define SDS_XREF_BIND		0x40
#define SDS_XREF_USERDEF	0x80

/*=======================================================================
	sds_xref(action,param)
	actions:
	-----------------
	SDS_XREF_FIND		Inquiry to the user for xref search.
						On input :
							Can be combined with "SDS_XREF_OVERLAY" flag
							(by default will be request
								for the attachment file)
							Can be combined with "SDS_XREF_USERDEF" flag.
							In a this case if 'param->restype' is "RTSTR"
							then dialog title will present as
							'param->resval.rstring' string.
						On exit:
						In param->resval.rstring will be returned path.
	---------------------------------------------------------------------
	SDS_XREF_ATTACH
					On input:
						Can be combined with the flags "SDS_XREF_OVERLAY"
						(In this case the file will be attached as overlay)
						and "SDS_XREF_CROP_PATH" (the xref will be kept
						with file name only, without full path)

					Remark:
						In this action "param" contains those parameters
							(for example rotate angle, original point etc.),
						 which will not be requested from the user.
							The DXF-codes may be next:
						 10 - insertion point,	41,42,43 - x,y,z scales
						 50 - rotation,			1 - path,	2 - name
	---------------------------------------------------------------------
	SDS_XREF_INSERT		In this actions "param" contains xref names list
	SDS_XREF_RELOAD					(DXF code 2 or SDS code RTSTR)
	SDS_XREF_UNLOAD
	SDS_XREF_DETACH
					Remark:
						SDS_XREF_INSERT can be combined
						with "SDS_XREF_BIND"
	---------------------------------------------------------------------
	SDS_XREF_EDIT_PATH
						On input:
							Can be combined with "SDS_XREF_CROP_PATH" flag
							(will be kept the file name only,
								without fully path)
						Remark:
							In this action "param" contains
							xref name pattern	(DXF code 2)
							and new path		(DXF code 1)
	---------------------------------------------------------------------
	SDS_XREF_RENAME
						On input:
							For this action "param" contains
								xref name 			(DXF code 2)
								and xref new name	(DXF code RTSTR)
========================================================================*/


//********************************
// SDS callback flags
// The latest number is 49 SDS_CBCHAR
//********************************

// Command Start/End.
#define SDS_CBCMDBEGIN        0  // arg1="command name"	 cast as (char *)
							     // Callback return  RTERROR will cause command to not be called.
#define SDS_CBCMDEND          1	 // arg1="command name"	 cast as (char *)
						         // Return code does not matter.

// Mouse movements and Mouse buttons.
#define SDS_CBMOUSEMOVE       2	 // arg1=X pixel  arg2=Y pixel
#define SDS_CBLBUTTONDN       3	 // arg1=X pixel  arg2=Y pixel
#define SDS_CBLBUTTONUP       4	 // arg1=X pixel  arg2=Y pixel
#define SDS_CBLBUTTONDBLCLK   5	 // arg1=X pixel  arg2=Y pixel
#define SDS_CBRBUTTONDN       6	 // arg1=X pixel  arg2=Y pixel
#define SDS_CBRBUTTONUP       7	 // arg1=X pixel  arg2=Y pixel
							     // Callback return  RTERROR will cause point to be ignored.
								 // Cast arg1 and arg2 as (int*)

// Transforming a Selection Set.
#define SDS_CBXFORMSS         8	 // arg1=Selection set (ok to modify) cast as (sds_name)
								 // arg2=Transformation Matrix (ok to modify) cast as (sds_matrix)
								 // Callback return RTERROR will stop the transformation.

// Undo/Redo.
#define SDS_CBENTUNDO         9	 // arg1=Entname (ok to modify)	cast as (sds_name), arg2 below defines on undo type
								 // Callback return RTERROR will stop UNDO for this ent.
#define SDS_CBENTREDO        10	 // arg1=Entname (ok to modify)	 cast as (sds_name), arg2 below defines on undo type
								 // Callback return RTERROR will stop REDO for this ent.
// Undo/Redo: called just after successful undoing/redoing.
#define SDS_CBENTUNDONE      44	 // arg1 = ename, cast as (sds_namep), arg2 below defines on undo type, arg3 is NULL
#define SDS_CBENTREDONE      45	 // arg1 = ename, cast as (sds_namep), arg2 below defines on redo type, arg3 is NULL

// Undo/Redo constants - Passed back as arg2 parameter so the user can tell what action is being undone.
#define SDS_ADD_NOTICE		1
#define SDS_MODIFY_NOTICE	2
#define SDS_DELETE_NOTICE	3

// Palette Change.
#define SDS_CBPALETTECHG     11	 // arg1=HPALETTE cast as (HPALETTE)

// Open/Close/New/Save on documents.
#define SDS_CBOPENDOC        12	 // arg1=file name, cast as (char *)
#define SDS_CBNEWDOC         13	 // (all args NULL)
#define SDS_CBCLOSEDOC       14	 // arg1=file name, cast as (char *)
#define SDS_CBSAVEDOC        15	 // arg1= new file name, cast as (char *)
								 // arg2= old file name, cast as (char *)

// !!! See new callback SDS_CBDOCCHG below for better MDI control
// Doc/View change (when the user activates a new MDI window).
#define SDS_CBVIEWDOCCHG     16	 // arg1=file name, cast as (char *)
								 // arg2=current window HWND, cast as (HWND)

// Changes to documents.
#define SDS_CBENTDEL         17	 // arg1=Entname to be deleted (or undeleted), cast as (sds_name)
								 // Callback return RTERROR will stop sds_entdel() for this ent.
#define SDS_CBENTMAKE        18	 // arg1=Entname of new entity, cast as (sds_name)
#define SDS_CBENTMOD         19	 // arg1=Entname of modified entity, cast as (sds_name)

// sysvar change
#define SDS_CBVARCHANGE      43  // arg1 = name of sysvar, cast as (char*); arg2 & arg3 are NULL

// Grip edits.
#define SDS_CBGRIPEDITBEG    20	 // arg1=Entname being edited, cast as (sds_name)
								 // arg2=Point being edited, cast as (sds_point)
								 // Callback return RTERROR will stop grip edit.
#define SDS_CBGRIPEDITEND    21	 // arg1=Entname of entity that was edited, cast as (sds_name)
								 // arg2=Point being edited, cast as (sds_point)

#define SDS_CBVIEWCHANGE     22	 // arg1=type of paint
                                 // 1=redraw, 2=shade, 3=hide, 4=view change (zoom, vpoint, etc.) 5=regen
				                 //
// Mouse movements and Mouse buttons returning current UCS cordinates
// Callback return  RTERROR will cause point to be ignored.
#define SDS_CBMOUSEMOVEUCS      23  // arg1=X coord  arg2=Y coord arg3=Z coord
#define SDS_CBLBUTTONDNUCS      24	 // arg1=X coord  arg2=Y coord arg3=Z coord
#define SDS_CBLBUTTONUPUCS      25	 // arg1=X coord  arg2=Y coord arg3=Z coord
#define SDS_CBLBUTTONDBLCLKUCS  26	 // arg1=X coord  arg2=Y coord arg3=Z coord
#define SDS_CBRBUTTONDNUCS      27	 // arg1=X coord  arg2=Y coord arg3=Z coord
#define SDS_CBRBUTTONUPUCS      28	 // arg1=X coord  arg2=Y coord arg3=Z coord
								 // Cast arg1 and arg2 as (int*)

// Callbacks associated with painting and dragging
#define SDS_CBBEGINPAINT		 29  // no arguments
#define SDS_CBENDPAINT			 30  // no arguments
#define SDS_CBENDMOUSEMOVE		 31  // arg1=X coord  arg2=Y coord arg3 = is Dragging flag
								 // Cast arg1 and arg2 as (int*) cast arg3 as bool

// Doc change (when the user activates a new MDI window).
#define SDS_CBDOCCHG			32	 // arg1=new file name, cast as (char *)
							 		 // arg2=new window, cast as HWND
									 // arg3=old window, cast as HWND

// Callbacks associated with cloning
#define SDS_CBBEGINCLONE		33	// arg1=selection set of the entities to be processed by sds-apps

#define SDS_CBENDCLONE			34	// arg1=selection set of the entities the cloning was performed from
						// arg2=selection set of created objects or entity name of created block
						// arg3=mode(0 - arg2=selection set, 1 - agr2=block entity name)

// Mouse movements and Mouse buttons.
#define SDS_CBMBUTTONDN			35	// arg1=X pixel  arg2=Y pixel // SystemMetrix(Maeda)
#define SDS_CBMBUTTONUP			36	// arg1=X pixel  arg2=Y pixel // SystemMetrix(Maeda)
#define SDS_CBMBUTTONDNUCS		37	// arg1=X coord  arg2=Y coord arg3=Z coord // SystemMetrix(Maeda)
#define SDS_CBMBUTTONUPUCS		38	// arg1=X coord  arg2=Y coord arg3=Z coord // SystemMetrix(Maeda)
#define SDS_CBMBUTTONDBLCLK		39	// arg1=X pixel  arg2=Y pixel // SystemMetrix(Maeda)
#define SDS_CBMBUTTONDBLCLKUCS	40	// arg1=X coord  arg2=Y coord arg3=Z coord // SystemMetrix(Maeda)
#define SDS_CBRBUTTONDBLCLK		41	// arg1=X pixel  arg2=Y pixel // SystemMetrix(Maeda)
#define SDS_CBRBUTTONDBLCLKUCS	42	// arg1=X coord  arg2=Y coord arg3=Z coord // SystemMetrix(Maeda)

								 // Cast arg1 and arg2 and arg3 as (double)

#define SDS_CBKEYDOWN			47	// arg1=virtual key code arg2=status flags     // SystemMetrix(Maeda)
#define SDS_CBKEYUP				48	// arg1=virtual key code arg2=status flags     // SystemMetrix(Maeda)
#define SDS_CBCHAR				49	// arg1=virtual key code arg2=status flags     // SystemMetrix(Maeda)


// 'current selection set changed' callback notification
#define SDS_CBSSCHANGE			46	// arg1 is a changing type (add, del, free: see defines below), cast as int;
									// arg2 is an entity if it's added or deleted, cast as sds_namep;
									//arg3 is NULL
// The following callback gives third-party apps access to the display buffer for the purposes
// of adding their own image data as a background
#define SDS_CBFRAMEBUFFER		98	// arg2 is the struct HDCInfo that contains the frame buffer info

#define SDS_CB_SSMOD	0	// ss is changed somehow, e.g. several ents were added to it and/or other ones were deleted from;
							// note, neither SDS_CB_SSADD nor SDS_CB_SSDEL are sent if SDS_CB_SSMOD is sent
#define SDS_CB_SSADD	1
#define SDS_CB_SSDEL	2
#define SDS_CB_SSFREE	3								// Cast arg1 and arg2 as (UINT)                // SystemMetrix(Maeda)

//*****************************
// end SDS callback flags
//*****************************

// ** SDS Display Object flags **

#define SDS_DISP_OBJ_PTS_3D				1	// not set -- points are 2D
#define SDS_DISP_OBJ_PTS_CLOSED			2	// not set -- lines are open
#define SDS_DISP_OBJ_PTS_FILLABLE		4	// not set -- entity is not fillable
#define SDS_DISP_OBJ_PTS_GDI_TRUETYPE	8	// not set -- points are entity other than TrueType text/mtext/etc. GDI stuff
#define SDS_DISP_OBJ_PTS_GDI_FILLPATH	16	// this is a path of a SOLID hatch boundary or a TTF text contour GDI stuff
#define SDS_DISP_OBJ_HIGHLIGHTED		32	// display object should be highlighted
#define SDS_DISP_OBJ_SPECIAL			64	// indicates special display object, data interprets dependening on type of source entity
#define SDS_DISP_OBJ_ENDPATH			128	// this bit is used for indication that GDI path should be end'ed

// ** End DispObj falags **

#define SDS_MODE_ENABLE   0
#define SDS_MODE_DISABLE  1
#define SDS_MODE_SETFOCUS 2
#define SDS_MODE_SETSEL   3
#define SDS_MODE_FLIP     4

#define SDS_MAX_TILE_STR   40
#define SDS_TILE_STR_LIMIT 255

#define sds_name_set(nmFrom, nmTo)				(*(nmTo)=*(nmFrom),(nmTo)[1]=(nmFrom)[1])
#define sds_point_set(ptFrom, ptTo)				(*(ptTo)=*(ptFrom),(ptTo)[1]=(ptFrom)[1],(ptTo)[2]=(ptFrom)[2])
#define sds_name_clear(nmAnyName)				 nmAnyName[0]=nmAnyName[1]=0
#define sds_name_nil(nmAnyName)					(nmAnyName[0]==0 && nmAnyName[1]==0)
#define sds_name_equal(nmNameOne, nmNameTwo)	(nmNameOne[0]==nmNameTwo[0] && nmNameOne[1]==nmNameTwo[1])

// The following naming convention is being used here for variables in the prototypes:     (Mostly from "Programming Windows, by Charles Petzold)
//
//_Abbrev____Type_______________Example:__________________________________________________________________________________________________________
//
//	   sw -- Switch (int)		int swExitMode		Note:  Use for any int that will only be zero or non-zero (not for Booleans, they would be "b")
//	   bs -- Bitset (int)		int bsAllowed		Note:  Use for any int variable containing a bitset
//		n -- Int or Short		int nRType
//		d -- Double				double dAngle
//		l -- Long				long lSetIndex
//	   pt -- sds_point			const sds_point ptFrom
//	   sz -- Pointer to a null-terminated string	const char *szAlertMsg
//		p -- Pointer			const void *pFirstPoint   (also, combines with any other, such as: double *pdRadians)
//	   rb -- sds_resbuf			const struct sds_resbuf *prbCmdList
//	   nm -- sds_name			const sds_name nmNextEnt
//	   mx -- sds_matrix			sds_matrix mxECStoWCS
//   size -- size_t				size_t sizeBytesEach
//________________________________________________________________________________________________________________________________________________

/* SDS protos for functions. */


#if defined(_WINDOWS_)
	HDC *sds_getviewhdc(void);
	int sds_getrgbvalue(int nColor);
	HPALETTE sds_getpalette(void);
	HWND sds_getviewhwnd(void);
	HWND sds_getmainhwnd(void);
	int sds_drawLinePattern(HDC h_dc, RECT rect, struct sds_resbuf *lineParam);

#endif

int SDS_main(int nARGC, char *nARGV[]);

int				   sds_getpreviewbmp(const char *szFileName, void** pHBITMAP, unsigned long* pAllocatedBytes, short* bIsBitmap);
void			   sds_abort(const char *szAbortMsg);
void		       sds_abortintellicad(void);
int				   sds_agetcfg(const char *szSymbol, char  *szVariable);   // Not documented
int				   sds_agetenv(const char *szSymbol, char  *szVariable);   // Not documented
int				   sds_alert(const char *szAlertMsg);
double             sds_angle(const sds_point ptStart, const sds_point ptEnd);
int                sds_angtof(const char *szAngle, int nUnitType, double *pdAngle);
int				   sds_angtof_absolute(const char *szAngle, int nUnitType, double *pdAngle);
int                sds_angtos(double dAngle, int nUnitType, int nPrecision, char *szAngle);
int                sds_angtos_end(double dAngle, int nUnitType, int nPrecision, char *szAngle);
int                sds_angtos_convert(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle);
// EBATECH(CNBR) -[ Bugzilla#78443-2 DIMADEC,DIMAZIN Support, 2003/3/31 DIMDSEP
int                sds_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle, int nDimazin, int nDimdsep );
//int                sds_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle);
int				   sds_angtos_absolute(double dAngle, int nUnitType, int nPrecision, char *szAngle);
int				   sds_arxload(const char *szARXProgram);
struct sds_resbuf *sds_arxloaded(void);
int				   sds_arxunload(const char *szARXProgram);
int                sds_asetcfg(const char *szSymbol, const char *szValue);   // Not documented
int				   sds_asetenv(const char *szSymbol, const char *szValue);   // Not documented

int					sds_bmpout(const char* pFile, int width, int height);

struct sds_resbuf *sds_buildlist(int nRType, ...);
struct sds_blockTree *sds_buildBlockTree(int type_mask);
int                sds_callinmainthread(int (*fnDragEnts)(void *),void *pUserData);
void              *sds_calloc(size_t sizeHowMany, size_t sizeBytesEach);

int                sds_cmd(const struct sds_resbuf *prbCmdList);
int                sds_command(int nRType, ...);
int                sds_cvunit(double dOldNum, const char *szOldUnit, const char *szNewUnit, double *pdNewNum);

int                sds_defun(const char *szFuncName, int nFuncCode);
int				   sds_dictadd(const sds_name nmDict, const char *szAddThis, const sds_name nmNonGraph);  // ??
int				   sds_dictdel(const sds_name nmDict, const char *szDelThis);
struct sds_resbuf *sds_dictnext(const sds_name nmDict, int swFirst);
int				   sds_dictrename(const sds_name nmDict, const char *szOldName, const char *szNewName);
struct sds_resbuf *sds_dictsearch(const sds_name nmDict, const char *szFindThis, int swFirst);
double			   sds_distance(const sds_point ptFrom, const sds_point ptTo);
struct sds_dobjll *sds_dispobjs(const sds_name nmEntity,int nDispMode);
int                sds_distof(const char *szDistance, int nUnitType, double *pdDistance);
int                sds_draggen(const sds_name nmSelSet, const char *szDragMsg, int nCursor, int (*fnDragEnts)(sds_point ptCursorLoc, sds_matrix mxTransform), sds_point ptDestPoint);	//TODO

int                sds_entdel(const sds_name nmEntity);
struct sds_resbuf *sds_entget(const sds_name nmEntity);
struct sds_resbuf *sds_entgetx(const sds_name nmEntity, const struct sds_resbuf *prbAppList);
int                sds_entlast(sds_name nmLastEnt);
int                sds_entmake(const struct sds_resbuf *prbEntList);
int				   sds_entmakex(const struct sds_resbuf *prbEntList, sds_name nmNewEnt);
int                sds_entmod(const struct sds_resbuf *prbEntList);
int                sds_entnext(const sds_name nmKnownEnt, sds_name nmNextEnt);
int                sds_entsel(const char *szSelectMsg, sds_name nmEntity, sds_point ptSelected);
int                sds_entupd(const sds_name nmEntity);
void               sds_exit(int swAbnormalExit);

void               sds_fail(const char *szFailMsg);
int                sds_findfile(const char *szLookFor, char *szPathFound);
void               sds_free(void *pMemLoc);
void			   sds_freedispobjs(struct sds_dobjll *pDispObjs);

int                sds_getangle(const sds_point ptStart, const char *szAngleMsg, double *pdRadians);
char              *sds_getappname(void);
struct sds_resbuf *sds_getargs(void);
int				   sds_getcfg(const char *szSymbol, char  *szVariable, int nLength);
int                sds_getcname(const char *szOtherLang, char **pszEnglish);
int                sds_getcorner(const sds_point ptStart, const char *szCornerMsg, sds_point ptOpposite);
int                sds_getdist(const sds_point ptStart, const char *szDistMsg, double *pdDistance);
struct sds_resbuf *sds_getdoclist(void);
int                sds_getfiled(const char *szTitle, const char *szDefaultPath, const char *szExtension, int bsOptions, struct sds_resbuf *prbFileName);
int                sds_getfuncode(void);
int                sds_getinput(char *szEntry);
int                sds_getint(const char *szIntMsg, int *pnInteger);
int                sds_getkword(const char *szKWordMsg, char *szKWord);
int                sds_getorient(const sds_point ptStart, const char *szOrientMsg, double *pdRadians);
int                sds_getpoint(const sds_point ptReference, const char *szPointMsg, sds_point ptPoint);
int                sds_getreal(const char *szRealMsg, double *pdReal);
int                sds_getstring(int swSpaces, const char *szStringMsg, char *szString);
int                sds_getsym(const char *szSymbol, struct sds_resbuf **pprbSymbolInfo);
int                sds_gettbpos(const char *pToolBarName, sds_point ptTbPos);
int                sds_getvar(const char *szSysVar, struct sds_resbuf *prbVarInfo);
int                sds_graphscr(void);
int                sds_grclear(void);
int		    	   sds_grarc(const sds_point ptCenter, double dRadius, double dStartAngle, double dEndAngle, int nColor, int swHighlight);
int		    	   sds_grfill(const sds_point *pptPoints, int nNumPoints, int nColor, int swHighlight);
int                sds_grdraw(const sds_point ptFrom, const sds_point ptTo, int nColor, int swHighlight);
int                sds_grread(int bsAllowed, int *pnInputType, struct sds_resbuf *prbInputValue);
int                sds_grtext(int nWhere, const char *szTextMsg, int swHighlight);
int                sds_grvecs(const struct sds_resbuf *prbVectList, sds_matrix mxDispTrans);

int                sds_handent(const char *szEntHandle, sds_name nmEntity);
int                sds_help(char *szHelpFile, char *szContextID, int nMapNumber);

int                sds_init(int nARGC, char *nARGV[]);
int                sds_initget(int bsAllowed, const char *szKeyWordList);
int                sds_inters(const sds_point ptFrom1, const sds_point ptTo1, const sds_point ptFrom2, const sds_point ptTo2, int swFinite, sds_point ptIntersection);
int                sds_invoke(const struct sds_resbuf *prbArguments,    struct sds_resbuf **pprbReturn);
int                sds_isalnum(int nASCIIValue);
int                sds_isalpha(int nASCIIValue);
int                sds_iscntrl(int nASCIIValue);
int                sds_isdigit(int nASCIIValue);
int                sds_isgraph(int nASCIIValue);
int                sds_islower(int nASCIIValue);
int                sds_isprint(int nASCIIValue);
int                sds_ispunct(int nASCIIValue);
int                sds_isspace(int nASCIIValue);
int                sds_isupper(int nASCIIValue);
int                sds_isxdigit(int nASCIIValue);

int                sds_link(int nRSMsg);
struct sds_resbuf *sds_loaded(void);

void    	      *sds_malloc(size_t sizeBytes);
int                sds_menucmd(const char *szPartToDisplay);
int                sds_menugroup(char *pMemuGroupName);
int                sds_msize(void *pvBuffer);

int	               sds_namedobjdict(sds_name nmDict);
int                sds_nentsel(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, sds_point ptECStoWCS[4], struct sds_resbuf **pprbNestBlkList);
int                sds_nentselp(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, int swUserPick, sds_matrix mxECStoWCS, struct sds_resbuf **pprbNestBlkList);
struct sds_resbuf *sds_newrb(int nTypeOrDXF);

int                sds_osnap(const sds_point ptAperCtr, const char *szSnapModes, sds_point ptPoint);

void               sds_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint);
int                sds_printf(const char *szPrintThis, ...);
int                sds_progresspercent(int iPercentDone);
int				   sds_progressstart(void);
int                sds_progressstop(void);
int                sds_prompt(const char *szPromptMsg);
int                sds_putsym(const char *szSymbol, struct sds_resbuf *prbSymbolInfo);

int				   sds_readaliasfile(char *szAliasFile);
void              *sds_realloc(void *pOldMemLoc, size_t sizeBytes);	// TODO - Should return char*?
int                sds_redraw(const sds_name nmEntity, int nHowToDraw);
int                sds_regapp(const char *szApplication);
int                sds_regappx(const char *szApplication, int swSaveAsR12); // I have no idea is 2nd param is right.
int                sds_regfunc(int (*nFuncName)(void), int nFuncCode);
int                sds_relrb(struct sds_resbuf *prbReleaseThis);
void			   sds_relBlockTree(struct sds_blockTree *pTree);
int                sds_retint(int nReturnInt);
int                sds_retlist(const struct sds_resbuf *prbReturnList);
int                sds_retname(const sds_name nmReturnName, int nReturnType);
int                sds_retnil(void);
int                sds_retpoint(const sds_point ptReturn3D);
int                sds_retreal(double dReturnReal);
int                sds_retstr(const char *szReturnString);
int                sds_rett(void);
int                sds_retval(const struct sds_resbuf *prbReturnValue);
int                sds_retvoid(void);
int				   sds_rp2pix(double dNumberX,double dNumberY,int *pPixelX,int *pPixelY);
int                sds_rtos(double dNumber, int nUnitType, int nPrecision, char *szNumber);

int                sds_sendmessage(char *szCommandMsg);
int                sds_setcallbackfunc(int (*cbfnptr)(int flag,void *arg1,void *arg2,void *arg3));
int                sds_setfunhelp(char *szFunctionName, char *szHelpFile, char *szContextID, int nMapNumber);
int                sds_setvar(const char *szSysVar, const struct sds_resbuf *prbVarInfo);
int                sds_setview(const struct sds_resbuf *prbViews, int nWhichVPort);
int                sds_snvalid(const char *szTableName, int swAllowPipe);
int                sds_ssadd(const sds_name nmEntToAdd, const sds_name nmSelSet, sds_name nmNewSet);
int                sds_ssdel(const sds_name nmEntToDel, const sds_name nmSelSet);
int                sds_ssfree(sds_name nmSetToFree);
int                sds_ssget(const char *szSelMethod, const void *pFirstPoint, const void *pSecondPoint, const struct sds_resbuf *prbFilter, sds_name nmNewSet);
int                sds_ssgetfirst(struct sds_resbuf **pprbHaveGrips, struct sds_resbuf **pprbAreSelected);
int                sds_sslength(const sds_name nmSelSet, long *plNumberOfEnts);
int                sds_ssmemb(const sds_name nmEntity, const sds_name nmSelSet);
int                sds_ssname(const sds_name nmSelSet, long lSetIndex, sds_name nmEntity);
int                sds_ssnamex(struct sds_resbuf **pprbEntName, const sds_name nmSelSet, const long iIndex);
int                sds_sssetfirst(const sds_name nmGiveGrips, const sds_name nmSelectThese);
int                sds_swapscreen(void);

int                sds_tablet (const struct sds_resbuf *prbGetOrSet, struct sds_resbuf **pprbCalibration);
struct sds_resbuf *sds_tblnext(const char *szTable, int swFirst);
int	               sds_tblobjname(const char *szTable, const char *szEntInTable, sds_name nmEntName);
struct sds_resbuf *sds_tblsearch(const char *szTable, const char *szFindThis, int swNextItem);
int                sds_textbox(const struct sds_resbuf *prbTextEnt, sds_point ptCorner, sds_point ptOpposite);
int                sds_textpage(void);
int                sds_textscr(void);
int                sds_tolower(int nASCIIValue);
int                sds_toupper(int nASCIIValue);
int                sds_trans(const sds_point ptVectOrPtFrom, const struct sds_resbuf *prbCoordFrom, const struct sds_resbuf *prbCoordTo, int swVectOrDisp, sds_point ptVectOrPtTo);

int				   sds_ucs2rp(sds_point ptSour3D,sds_point ptDest3D);
int                sds_update(int nWhichVPort,const sds_point ptCorner1,const sds_point ptCorner2);
int                sds_undef(const char *szFuncName, int nFuncCode);
int                sds_usrbrk(void);

int                sds_vports(struct sds_resbuf **prbViewSpecs);

int                sds_wcmatch(const char *szCompareThis, const char *szToThis);

int                sds_xdroom(const sds_name nmEntity, long *plMemAvail);
int                sds_xdsize(const struct sds_resbuf *prbEntData, long *plMemUsed);
int                sds_xformss(const sds_name nmSetName, sds_matrix mxTransform);
int                sds_xload(const char *szApplication);
int				   sds_xref(char action,struct sds_resbuf *param);
int                sds_xstrcase(char *szString);   // Not documented
char              *sds_xstrsave(char *szSource, char **pszDest);   // Not documented
int                sds_xunload(const char *szApplication);


//	These are used as arguments to sds_help.
//	They match the Windows WinHelp definitons in WINUSER.H and are provided for convenience only.
#define HELP_CONTENTS     0x0003L  /* display first topic */
#define HELP_HELPONHELP   0x0004L  /* Display help on using help */
#define HELP_PARTIALKEY   0x0105L  /* Display Search dialog */


#if !defined(SDS_DONTUSEOURADS)

    #define SDS_USEOURDCL 1

    #define main           SDS_main
    #define ads_main	   SDS_main
    #define acrxEntryPoint SDS_main

    #define  OL_GOOD         0
    #define  OL_ESNVALID     1
    #define  OL_ENAMEVALID   2
    #define  OL_ESSMAX       3
    #define  OL_ESSVALID     4
    #define  OL_EBDEVALID    5
    #define  OL_EXDEVALID    6
    #define  OL_ENTSELPICK   7
    #define  OL_EEOEF        8
    #define  OL_EEOBD        9
    #define  OL_EEMPDB      10
    #define  OL_EDELVPORT   11
    #define  OL_EACQPLINE   12
    #define  OL_EHANDLE     13
    #define  OL_ENOHAND     14
    #define  OL_ETRANS      15
    #define  OL_EXSPACE     16
    #define  OL_EDELENT     17
    #define  OL_ETBLNAME    18
    #define  OL_ETBLARG     19
    #define  OL_ERDONLY     20
    #define  OL_ENONZERO    21
    #define  OL_ERANGE      22
    #define  OL_ERGBUSY     23
    #define  OL_EMMTYPE     24
    #define  OL_EMMLAY      25
    #define  OL_EMMLT       26
    #define  OL_EMMCOLOR    27
    #define  OL_EMMSTYLE    28
    #define  OL_EMMSHAPE    29
    #define  OL_EMMFTYPE    30
    #define  OL_EMODDEL     31
    #define  OL_EMODSEQ     32
    #define  OL_EMODHAND    33
    #define  OL_EMODVPVIS   34
    #define  OL_EMMLL       35
    #define  OL_EMKTYPE     36
    #define  OL_EMKPLINE    37
    #define  OL_EMKCMPLX    38
    #define  OL_EMKBNAME    39
    #define  OL_EMKBFLAG    40
    #define  OL_EMKDBNAME   41
    #define  OL_EMKNORM     42
    #define  OL_EMKNOBNM    43
    #define  OL_EMKNOBFLG   44
    #define  OL_EMKANON     45
    #define  OL_EMKBLOCK    46
    #define  OL_EMKMANDF    47
    #define  OL_EMMXDTYPE   48
    #define  OL_EMMXDNEST   49
    #define  OL_EMMXDAPPID  50
    #define  OL_EMMXDSIZE   51
    #define  OL_ENTSELNULL  52
    #define  OL_EMXDAPPID   53
    #define  OL_EMMVPORT    54
    #define  OL_INVEXT      55
    #define  OL_EFLTR1      56
    #define  OL_EFLTR2      57
    #define  OL_EFLTR3      58
    #define  OL_EFLTR4      59
    #define  OL_EFLTR5      60
    #define  OL_EFLTR6      61
    #define  OL_EFLTR7      62
    #define  OL_EFLTR8      63
    #define  OL_EFLTR9      64
    #define  OL_EFLTR10     65
    #define  OL_EFLTR11     66
    #define  OL_EFLTR12     67
    #define  OL_TABNOT      68
    #define  OL_TABNOCAL    69
    #define  OL_TABERR      70
    #define  OL_ENEWRB      71
    #define  OL_ENULLPTR    72
    #define  OL_EOPEN       73
    #define  OL_ELOADED     74
    #define  OL_EMAXAPP     75
    #define  OL_EEXEC       76
    #define  OL_EVERSION    77
    #define  OL_EDENIED     78
    #define  OL_EREFUSE     79
    #define  OL_ENOTLOADED  80
    #define  OL_ENOMEM      81
    #define  OL_EXFMVALID   82
    #define  OL_ESYMNAM     83
    #define  OL_ESYMVAL     84
    #define  OL_NONDIALOG   85

    /* Our additions (ACAD uses 0-100 currently): */

    #define  OL_EROPEN     501  /* Bad pathname or can't find/open. */
    #define  OL_EWOPEN     502  /* Can't create DWG file. */
    #define  OL_ESEEKREAD  503  /* fseek or fread error. */
    #define  OL_EDWGVER    504  /* Not a DWG, or not a version we handle. */
    #define  OL_EWALKING   505  /* Error walking the entities. */
    #define  OL_ERDWGENT   506  /* Error reading a DWG entity. */
    #define  OL_EBIGENT    507  /* Skipped ent that was too big for buffer. */
    #define  OL_EDWGWRITE  508  /* fwrite error. */
    #define  OL_ERNOTDWG   509  /* Appears not to be an AutoCAD drawing. */
      /* These two are for entmakes where ACAD prints a message instead */
      /* of setting ERRNO: */
    #define  OL_EMKNESTBLK 510  /* "Cannot nest block definitions." */
    #define  OL_EMKINVCOMP 511  /* "Invalid complex object." */


	#define ads_pointp sds_pointp
	#define ads_namep sds_namep

    #define ads_real sds_real
    #define ads_point sds_point
    #define ads_name sds_name
    #define ads_matrix sds_matrix
    #define ads_binary sds_binary
    #define ads_u_val  sds_u_val
    #define resbuf sds_resbuf

    #undef X
    #undef Y
    #undef Z
    #undef T

    #define X SDS_X
    #define Y SDS_Y
    #define Z SDS_Z
    #define T SDS_T

	#undef TRUE
	#undef FALSE

	#define TRUE      SDS_TRUE
	#define FALSE     SDS_FALSE

    #define EOS       '\0'

    #if !defined(SDS_NOSETRCODES)

        #define RQSAVE    SDS_RQSAVE
        #define RQEND     SDS_RQEND
        #define RQQUIT    SDS_RQQUIT
        #define RQCFG     SDS_RQCFG
        #define RQXLOAD   SDS_RQXLOAD
        #define RQXUNLD   SDS_RQXUNLD
        #define RQHUP     SDS_RQHUP
        #define RQXHELP   SDS_RQXHELP
        #define RQSUBR    SDS_RQSUBR
        #define RSRSLT    SDS_RSRSLT
        #define RSERR     SDS_RSERR

     	#define RTREJ     SDS_RTREJ
    	#define RTFAIL    SDS_RTFAIL

    	#define PAUSE      SDS_PAUSE

    	#define RSG_NONULL SDS_RSG_NONULL
    	#define RSG_NOZERO SDS_RSG_NOZERO
    	#define RSG_NONEG  SDS_RSG_NONEG
    	#define RSG_NOLIM  SDS_RSG_NOLIM
    	#define RSG_GETZ   SDS_RSG_GETZ
    	#define RSG_DASH   SDS_RSG_DASH
    	#define RSG_2D     SDS_RSG_2D
    	#define RSG_OTHER  SDS_RSG_OTHER

    	#define RTNONE    SDS_RTNONE
    	#define RTREAL    SDS_RTREAL
    	#define RTPOINT   SDS_RTPOINT
    	#define RTSHORT   SDS_RTSHORT
    	#define RTANG     SDS_RTANG
    	#define RTSTR     SDS_RTSTR
    	#define RTENAME   SDS_RTENAME
    	#define RTPICKS   SDS_RTPICKS
    	#define RTORINT   SDS_RTORINT
    	#define RT3DPOINT SDS_RT3DPOINT
    	#define RTLONG    SDS_RTLONG
    	#define RTVOID    SDS_RTVOID
    	#define RTLB      SDS_RTLB
    	#define RTLE      SDS_RTLE
    	#define RTDOTE    SDS_RTDOTE
    	#define RTNIL     SDS_RTNIL
    	#define RTDXF0    SDS_RTDXF0
    	#define RTT       SDS_RTT
    	#define RTNORM    SDS_RTNORM
    	#define RTERROR   SDS_RTERROR
    	#define RTCAN     SDS_RTCAN
    	#define RTKWORD   SDS_RTKWORD
        #define RTBINARY  SDS_RTBINARY

    #endif /* !defined(SDS_NOSETRCODES) */

    #define ads_name_set      sds_name_set
    #define ads_point_set     sds_point_set
    #define ads_name_clear    sds_name_clear
    #define ads_name_nil      sds_name_nil
    #define ads_name_equal    sds_name_equal
    #define ads_init          sds_init
    #define ads_fail          sds_fail
    #define ads_abort         sds_abort
    #define ads_exit          sds_exit
    #define ads_regfunc       sds_regfunc
    #define ads_link          sds_link
    #define ads_newrb         sds_newrb
    #define ads_relrb         sds_relrb
    #define ads_usrbrk        sds_usrbrk
    #define ads_defun         sds_defun
    #define ads_undef         sds_undef
    #define ads_getfuncode    sds_getfuncode
    #define ads_getargs       sds_getargs
    #define ads_retlist       sds_retlist
    #define ads_retval        sds_retval
    #define ads_retpoint      sds_retpoint
    #define ads_retstr        sds_retstr
    #define ads_retname       sds_retname
    #define ads_retint        sds_retint
    #define ads_retreal       sds_retreal
    #define ads_rett          sds_rett
    #define ads_retnil        sds_retnil
    #define ads_retvoid       sds_retvoid
    #define ads_malloc	      sds_malloc
    #define ads_free	      sds_free
    #define ads_realloc	      sds_realloc
    #define ads_calloc	      sds_calloc
	#define ads_entdel        sds_entdel
    #define ads_entgetx       sds_entgetx
    #define ads_entget        sds_entget
    #define ads_entlast       sds_entlast
    #define ads_entnext       sds_entnext
    #define ads_entupd        sds_entupd
    #define ads_entmod        sds_entmod
    #define ads_entmake       sds_entmake
    #define ads_entmakex      sds_entmakex
    #define ads_entsel        sds_entsel
    #define ads_nentsel       sds_nentsel
    #define ads_nentselp      sds_nentselp
    #define ads_ssget         sds_ssget
    #define ads_ssfree        sds_ssfree
    #define ads_sslength      sds_sslength
    #define ads_ssadd         sds_ssadd
    #define ads_ssdel         sds_ssdel
    #define ads_ssmemb        sds_ssmemb
    #define ads_ssname        sds_ssname
    #define ads_xdroom        sds_xdroom
    #define ads_xdsize        sds_xdsize
    #define ads_tblnext       sds_tblnext
    #define ads_tblsearch     sds_tblsearch
    #define ads_handent       sds_handent
    #define ads_trans         sds_trans
    #define ads_angtos        sds_angtos
    #define ads_cvunit        sds_cvunit
    #define ads_wcmatch       sds_wcmatch
    #define ads_rtos          sds_rtos
    #define ads_angtof        sds_angtof
    #define ads_distof        sds_distof
    #define ads_setvar        sds_setvar
    #define ads_initget       sds_initget
    #define ads_getsym        sds_getsym
    #define ads_putsym        sds_putsym
    #define ads_loaded        sds_loaded
    #define ads_xload         sds_xload
    #define ads_xunload       sds_xunload
    #define ads_arxloaded     sds_arxloaded
    #define ads_arxxload      sds_arxxload
    #define ads_arxxunload    sds_arxxunload
    #define ads_command       sds_command
    #define ads_buildlist     sds_buildlist
    #define ads_printf        sds_printf
    #define ads_cmd           sds_cmd
    #define ads_invoke        sds_invoke
    #define ads_inters        sds_inters
    #define ads_getvar        sds_getvar
    #define ads_findfile      sds_findfile
    #define ads_getstring     sds_getstring
    #define ads_menucmd       sds_menucmd
    #define ads_prompt        sds_prompt
    #define ads_alert         sds_alert
    #define ads_regapp        sds_regapp
    #define ads_getappname    sds_getappname
    #define ads_getangle      sds_getangle
    #define ads_getcorner     sds_getcorner
    #define ads_getdist       sds_getdist
    #define ads_getorient     sds_getorient
    #define ads_getpoint      sds_getpoint
    #define ads_getint        sds_getint
    #define ads_getkword      sds_getkword
    #define ads_getreal       sds_getreal
    #define ads_getinput      sds_getinput
    #define ads_vports        sds_vports
    #define ads_textscr       sds_textscr
    #define ads_graphscr      sds_graphscr
    #define ads_textpage      sds_textpage
    #define ads_grclear       sds_grclear
    #define ads_redraw        sds_redraw
    #define ads_osnap         sds_osnap
    #define ads_grread        sds_grread
    #define ads_grtext        sds_grtext
    #define ads_grdraw        sds_grdraw
    #define ads_grvecs        sds_grvecs
    #define ads_xformss       sds_xformss
    #define ads_draggen       sds_draggen
    #define ads_angle         sds_angle
    #define ads_distance      sds_distance
    #define ads_polar         sds_polar
    #define ads_getfiled      sds_getfiled
    #define ads_textbox       sds_textbox
    #define ads_tablet        sds_tablet
    #define ads_isalpha       sds_isalpha
    #define ads_isupper       sds_isupper
    #define ads_islower       sds_islower
    #define ads_isdigit       sds_isdigit
    #define ads_isxdigit      sds_isxdigit
    #define ads_isspace       sds_isspace
    #define ads_ispunct       sds_ispunct
    #define ads_isalnum       sds_isalnum
    #define ads_isprint       sds_isprint
    #define ads_isgraph       sds_isgraph
    #define ads_iscntrl       sds_iscntrl
    #define ads_toupper       sds_toupper
    #define ads_tolower       sds_tolower
	#define ads_agetenv		  sds_agetenv
	#define ads_asetenv		  sds_asetenv
	#define ads_agetcfg		  sds_agetcfg
	#define ads_asetcfg		  sds_asetcfg
	#define ads_namedobjdict  sds_namedobjdict
	#define ads_dictsearch	  sds_dictsearch
	#define ads_dictnext	  sds_dictnext
	#define ads_tblobjname	  sds_tblobjname
	#define ads_getcname	  sds_getcname
	#define ads_help		  sds_help
	#define ads_regappx		  sds_regappx
	#define ads_setfunhelp	  sds_setfunhelp
	#define ads_setview		  sds_setview
	#define ads_snvalid		  sds_snvalid
	#define ads_ssgetfirst	  sds_ssgetfirst
	#define ads_sssetfirst	  sds_sssetfirst
	#define ads_ssnamex		  sds_ssnamex
	#define ads__msize		  sds__msize
	#define ads_xstrsave	  sds_xstrsave
	#define ads_strdup	      sds_xstrsave
	#define ads_xstrcase	  sds_xstrcase
	#define adsw_acadMainWnd  sds_getmainhwnd
	#define ads_update        sds_update

	#define SDS_DLLEXPORT __declspec(dllexport)

	#ifdef _WINDOWS_
	#define	ENTRYARG	HWND
	#else
	#define	ENTRYARG
	#endif
    void SDS_DLLEXPORT SDS_EntryPoint( ENTRYARG);

#endif  /* SDS_DONTUSEOURADS */

// The following naming convention is being used here for variables in the prototypes:     (Mostly from "Programming Windows, by Charles Petzold)
//
//_Abbrev____Type_______________Example:__________________________________________________________________________________________________________
//
//		n -- Int or Short		int nRType
//		d -- Double				double dAngle
//		l -- Long				long lSetIndex
//		h -- Handle				sds_hdlg hDialog
//		p -- Pointer			const void *pFirstPoint   (also, combines with any other, such as: double *pdRadians)
//________________________________________________________________________________________________________________________________________________

/* Below is all of our DCL stuff. */
int dlg_new_dialog(char *szDialogName,int nDialogFileID,void *pfnDef_Callback,sds_hdlg *hDialog);
int dlg_load_dialog(char *szFileToLoad,int *nDialogFileID);
int dlg_unload_dialog(int nDialogFileID);
int dlg_start_dialog(sds_hdlg hDialog, int *nMessage);
int dlg_done_dialog(sds_hdlg hDialog,long nMessage);
int dlg_new_positioned_dialog(char *szDialogName, int nDialogFileID, void *pfnDef_Callback, int nXPos, int nYPos, sds_hdlg *hDialog);
int dlg_done_positioned_dialog(sds_hdlg hDialog, int nMessage, int *nXPos, int *nYPos);
int dlg_term_dialog(void);

int dlg_start_list(sds_hdlg hDialog,char *szControlKey, int nChangeAddNew,int nWhichItem);
int dlg_add_list(char *szAddToList);
int dlg_get_list_string(sds_hdlg hDialog, char *szControlKey, char *szListString, int nStringLength, int nWhichItem);
int dlg_end_list(void);

int dlg_get_attr(sds_hdlg hDialog, char *szControlKey, char *szAttribName, char *szAttribInfo, int lInfoMaxLength);
int dlg_get_attr_string(sds_htile hControl,char *szAttribName, char *szAttribInfo, long lInfoMaxLength);

int dlg_get_tile(sds_hdlg hDialog,char *szControlKey,char *szControlInfo,long lInfoMaxLength);
int dlg_action_tile(sds_hdlg hDialog,char *szControlKey,void *pfnControlFunc);
int dlg_client_data_tile(sds_hdlg hDialog, char *szControlKey, void *pApplicationData);
int dlg_dimensions_tile(sds_hdlg hDialog, char *szControlKey, short *nWidthLess1, short *nHeightLess1);
int dlg_mode_tile(sds_hdlg hDialog, char *szControlKey, int nTileState);
int dlg_set_tile(sds_hdlg hDialog, char *szControlKey, char *szControlInfo);

int dlg_start_image(sds_hdlg hDialog, char *szControlKey);
int dlg_fill_image(int nULCornerX, int nULCornerY, int nWidthLess1, int nHeightLess1, int nColor);
int dlg_slide_image(int nULCornerX, int nULCornerY, int nWidthLess1, int nHeightLess1, char *szSnapshotName);
int dlg_vector_image(int nXVectStart, int nYVectStart, int nXVectEnd, int nYVectEnd, int nColor);
int dlg_end_image(void);
#ifdef __4M_RENDER__
//Elias Add:1->
int dlg_create_preview_image(
							  sds_hdlg hDialog,		 // i : Handle to the dialog.
							  char *szControlKey,	 // i : Key specifies the tile.
							  const void *pPreviewData	 // [in]  The preview definition structure
							 );

//Elias Add:1<-
#endif //__4M_RENDER__


#if defined(SDS_USEOURDCL)

    #undef  CALLB
    #define CALLB

    #define ads_hdlg sds_hdlg
    #define ads_htile sds_htile
    #define ads_callback_packet sds_callback_packet

	#ifdef __cplusplus
		typedef void (*CLIENTFUNC) (sds_callback_packet *cpkt);
	#else
		#define CLIENTFUNC void *
	#endif

    #define SDS_NULLCB ((CLIENTFUNC)0)

    #define NULLCB           SDS_NULLCB

    #define CBR_SELECT       SDS_CBR_SELECT
    #define CBR_LOST_FOCUS   SDS_CBR_LOST_FOCUS
    #define CBR_DRAG         SDS_CBR_DRAG
    #define CBR_DOUBLE_CLICK SDS_CBR_DOUBLE_CLICK

    #define DLGCANCEL     SDS_DLGCANCEL
    #define DLGOK         SDS_DLGOK
    #define DLGALLDONE    SDS_DLGALLDONE

    #define DLGSTATUS     SDS_DLGSTATUS

    #define BGLCOLOR      SDS_BGLCOLOR
    #define DBGLCOLOR     SDS_DBGLCOLOR
    #define DFGLCOLOR     SDS_DFGLCOLOR
    #define LINELCOLOR     SDS_LINELCOLOR

    #define LIST_CHANGE   SDS_LIST_CHANGE
    #define LIST_APPEND   SDS_LIST_APPEND
    #define LIST_NEW      SDS_LIST_NEW

    #define MODE_ENABLE   SDS_MODE_ENABLE
    #define MODE_DISABLE  SDS_MODE_DISABLE
    #define MODE_SETFOCUS SDS_MODE_SETFOCUS
    #define MODE_SETSEL   SDS_MODE_SETSEL
    #define MODE_FLIP     SDS_MODE_FLIP

    #define MAX_TILE_STR   SDS_MAX_TILE_STR
    #define TILE_STR_LIMIT SDS_TILE_STR_LIMIT


    #define ads_load_dialog            dlg_load_dialog
    #define ads_unload_dialog          dlg_unload_dialog
    #define ads_new_dialog             dlg_new_dialog
    #define ads_new_positioned_dialog  dlg_new_positioned_dialog
    #define ads_start_dialog           dlg_start_dialog
    #define ads_term_dialog            dlg_term_dialog
    #define ads_action_tile            dlg_action_tile
    #define ads_done_dialog            dlg_done_dialog
    #define ads_done_positioned_dialog dlg_done_positioned_dialog
    #define ads_set_tile               dlg_set_tile
    #define ads_client_data_tile       dlg_client_data_tile
    #define ads_get_tile               dlg_get_tile
    #define ads_get_attr               dlg_get_attr
    #define ads_get_attr_string        dlg_get_attr_string
    #define ads_start_list             dlg_start_list
    #define ads_add_list               dlg_add_list
    #define ads_end_list               dlg_end_list
    #define ads_mode_tile              dlg_mode_tile
    #define ads_dimensions_tile        dlg_dimensions_tile
    #define ads_start_image            dlg_start_image
    #define ads_vector_image           dlg_vector_image
    #define ads_fill_image             dlg_fill_image
    #define ads_slide_image            dlg_slide_image
    #define ads_end_image              dlg_end_image
#ifdef __4M_RENDER__
//Elias Add:1->
	#define ads_create_preview_image  dlg_create_preview_image
//Elias Add:1<-
#endif //__4M_RENDER__
    #define ads_get_list_string        dlg_get_list_string

#endif /* SDS_USEOURDCL */

#if defined(__cplusplus)
	}
#endif

#endif // _SDS_H

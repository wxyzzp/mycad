/* F:\BLD\PRJ\LIB\CMDS\CMDS.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*********************************************************************

#pragma once
#ifndef _CMDS_H
#define _CMDS_H

#include "Icad.h"			/*DNT*/
#include "IcadTextScrn.h"	/*DNT*/
#include "insert.h"		/*DNT*/

extern BOOL cmd_bOpenReadOnly;
extern BOOL cmd_bSaveWithPassWord;
extern bool cmd_InsideRedrawAll;
extern bool cmd_InsideRegen;
extern bool cmd_HatchedOK;
extern bool cmd_DontCalcViewSize;
extern bool cmd_InsideDDInsert;
extern bool cmd_InsideDxfInCommand;

extern bool cmd_GaveACISMsg;
extern bool cmd_GaveProxyMsg;
extern bool cmd_GaveA2KEntMsg;

extern int	SDS_AtNodeDrag;
extern bool SDS_WasFromPipeLine;

extern sds_name	SDS_EditPropsSS;
extern sds_name SDS_AttEditName;

class CIcadDoc;
class CIcadView;
class db_drawing;
class db_handitem;
class db_viewport;

#if defined(__cplusplus)
	extern "C"
	{
#endif

#define CMD_FUZZ 1.0e-11	//should be used instead of IC_ZRO
//Modified Cybage SBD 30/04/2001 DD/MM/YYYY
//Reason : Mail from Ben Thump(mcthum@siacad.com) Dated 17/04/2001 DD/MM/YYYY.Re: join not working
#define CMD_FUZZ2 1.0e-7

#define CMD_XREF_TBLDIVIDER '|'/*DNT*/
#define CMD_XREF_TBLDIVIDERSTR "|"/*DNT*/
#define CMD_XREF_TBLBINDER	'$'/*DNT*/
#define CMD_XREF_TBLBINDERSTR "$"/*DNT*/

#define CMD_DDEDIT_TEXTMODE   0
#define CMD_DDEDIT_ATTDEFMODE 1

#define CMD_ENT_NATIVEWCS	147.4747

#define ATTX_BL 	 1
#define ATTX_QUOTE	 2
#define ATTX_DELIM	 3
#define ATTX_OTHER	 4

#define CMD_SCALE_DRAG	   0
#define CMD_ROTATE_DRAG    1
#define CMD_TRANSLATE_DRAG 2
#define CMD_MIRROR_DRAG    3

#define CMD_NULLASSOCVAL  -33  //*** This is an arbitrary value that should not be a valid DXF group code number.
#define dwg_filelink	db_drawing

bool cmd_cmdecho_printf( int ce );

#define CMD_CMDECHO_PRINTF(CE)	cmd_cmdecho_printf( CE )

#if defined(debug)									// Pass this the line number of an error (often "rc").
	#define CMD_INTERNAL_MSG(ErrorLine) sds_printf("\nInternal Error: Line %d, File %s",ErrorLine,__FILE__)
#else
#define CMD_INTERNAL_MSG(ErrorLine) do {} while (false)  // This does nothing, it's just a placeholder function.
#endif


//	dangerous globals declared in multiple files
extern BOOL	cmd_InsideRotateCommand,cmd_InsideScaleCommand,cmd_InsideMoveCopyCommand;
extern BOOL	cmd_InsideSds_draggen;
extern BOOL cmd_InsideViewPorts,cmd_InsideCreateNewViewports,cmd_InsideCreatePaperspace;
extern BOOL cmd_InsideOpenCommand,cmd_InsideNewCommand;
extern BOOL cmd_InsideMirrorCommand;
extern BOOL	cmd_CalcChildRect;


// Used by cmd_rtos_dim() to tell which type of dimension we are suppressing
// the zeros on.
// EBATECH(CNBR) Bugzilla#78443 Change assigned value.
#define DIMTYPEMAIN   -1
#define DIMTYPEALT	  -2
#define DIMTYPEALTTOL -3
#define DIMTYPETOL	  -4
//#define DIMTYPEMAIN   0
//#define DIMTYPEALT	  1
//#define DIMTYPEALTTOL 2
//#define DIMTYPETOL	  3
// EBATECH(CNBR) ]-

//*** Structures

struct cmd_elistll { //*** This structure is intended for creating a linked list of entity lists for a selection set.
	sds_name ename; 		 //*** The name of this entity.
	struct resbuf* entlst;	 //*** This is a pointer to the entity list returned from sds_entget().
	struct resbuf* grpptr;	 //*** This is a pointer to a specific group in the entlst.
	short modflag;			 //*** This flag used by PEDIT for inserting/removing vertex entities
	struct cmd_elistll* next;
	struct cmd_elistll* prev;
};
//this struct used for wblock and insertfunc to track old & new anon block names
struct cmd_anon_blklst {	//used for tracking anon block names when inserting one
	char type;				//	file into another
	int oldnum;				//type0=dim, type1=hatch, type2=user
	int newnum;
	struct cmd_anon_blklst *next;
};

//** Structure for template link list
struct cmd_xtemplate {
	char   *szFieldName; //name of the bl's and tags
	short  sFieldType;	 //type of data stored in fieldname
	char   cTypeChar;	 //type of char N,C or could be the delim or quote
	short  sCharWidth;	 //width field
	short  sDecWidth;	 //number of dicimals
	struct cmd_xtemplate *next;
};

struct cmd_drag_globalpacket {
	sds_point pt1;		//*** Base point for dragging, used in cmd_drag() and draggen() callback.
	sds_point lastpt;
	short reference;	//*** Reference flag.
	sds_real refval;	//*** Reference value.
	short rband;		//*** Turn rubber-banding on/off.  1-on 0-off.
};

//the following 2 structures are intended to hold array information for the array command to use when
//working non-interactively (i.e. through automation)
struct rect_array_info
	{
	int		nColumns;
	int		nRows;
	int		nLevels;
	double	ColDist;
	double	RowDist;
	double	LevDist;
	};

struct polar_array_info
	{
	int			nItems;
	double		FillAngle;
	double		AngleBetweenItems;
	sds_point	BasePoint;
	sds_point	PointOnAxis;
	bool		RotateItems;
	};

//*** Protos
bool  IsInternalArrow(char* arrowHeadBlkName);
short ProcessInsertBlockName(char *inblkname,char *outblkname,char *outblkfile);
short FindTheBlockFile(char *inblkfile, char *outblkfile);
short LoadTheBlock(char *blkname, char *blkfile);
bool PrintMode(void);
short GetExtrusionAndRotation(sds_real *extru, sds_real *rotang, sds_real *rotbase);
short MakeDragParlink(char *blkname,sds_real *extru, sds_real rotang, db_insert **ins);
short GetInsertParameters(db_insert *elp,bool minsert,short ask4onescl,bool draginspt,bool givetblsloadedmsg,
						  sds_real rotbase,sds_real *curelev,sds_real rotang,sds_point retinspt,bool *multiIns, FLAGS_BYTE flags =0xFF);
short DoTheInsert(char *blkname, bool explodeflag, db_insert *ins, bool fromdisk, sds_point retinspt,sds_real curelev,bool multiIns, bool redefineregen);
// EBATECH(CNBR) -[ 2002/4/24 Use both SHAPE and INSERT
short DoDrag(sds_name ename, double *pt,char *promptstr,double *pt1, short dragmode);
short GetOneScale(db_handitem *elp, sds_real *scl, bool givetblsloadedmsg);
short GetRotation(sds_name ename,db_handitem *elp, sds_point pt1, sds_real rotbase, bool givetblsloadedmsg, sds_real rotang);
// EBATECH(CNBR) ]-
short TableNameLengthsLegalForXref(db_drawing *xrefdp,char *blkname);

short cmd_renderfull(void);
short cmd_renderwf(void);
short cmd_renderhl(void);
short cmd_editgb(void);
short cmd_roptions(void);
int   cmd_InitRender(int initmode);
void  cmd_prn_dwg_times(int mode);
// Ebatech(CNBR) -[ 2003/3/31 Add iDimdsep
int   cmd_rtos_dim(double dNumber,int nUnitType,int nPrecision,char *szNumber, int iDimType, int iDimdsep = 0);
int   cmd_rtos_main(double dNumber,int nUnitType,int nPrecision,char *szNumber,int mode, int iDimType, int iDimdsep);
// Ebatech(CNBR) Bugzilla#78443 Use Macro
int   cmd_rtos_area(double dNumber,int nUnitType,int nPrecision,char *szNumber, int iDimType=DIMTYPEMAIN);
// EBATECH(CNBR) ]-
void  cmd_PlaySound(int mode);
void  cmd_remove_rblink(struct resbuf **pRbbll,int rbtype);
void  cmd_ucs_freell(CIcadDoc *pDoc);
int   cmd_pedit_drawx(sds_point drawpt,int mode, sds_point extru);
short cmd_pt_add(sds_point pt1,sds_point pt2,sds_point pt3);
short cmd_pt_sub(sds_point pt1,sds_point pt2,sds_point pt3);
short cmd_rpt_mul(sds_point pt1,sds_real r,sds_point pt2);
short cmd_rpt_div(sds_point pt1,sds_real r,sds_point pt2);
bool  cmd_access(char *fname);
short cmd_make_block(char *blkname);
void  cmd_cleanup(void);
short cmd_dofuncs(void);
void  cmd_dt(void);
void  cmd_init(void);
short cmd_loadfuncs(void);
long  cmd_ucsworld(void);
void  cmd_test(void);
short cmd_ucsdef (int fromexplorer);
void RemovePersistentReactors(struct resbuf *rb); // 2003-4-30 Bugzilla#78510 Ebatech(cnbr) share with explodig minsert.
int   cmd_ReadAsBlock(char *dwgname, char *newname, int xrefmode,db_drawing *xrefdep);
short AddInsertEntity(char *blkname,db_insert *ins, sds_real curelev,bool blockhasattribs,bool multiIns,bool redefineregen, sds_real RotAng = 0.0);	/*D.G.*/// RotAng only for minserts with attribs
short cmd_doxref(short attachoroverlay, char *blkname, char *filename, short recurlevel, bool bInteractive=TRUE, double *inspt=NULL, double *scales=NULL, double *rot= NULL);
long  cmd_ucslister(void);
long  cmd_ucsprev(void);
long  cmd_ucsrestore(char* name);
short cmd_ltload(char* linetype, char* fname, short mode);
void  cmd_unit_vector(sds_point pt1,sds_point pt2,sds_point pt3);
void  cmd_polar(sds_point startpt,sds_point vec,sds_real length,sds_point endpt);
int	  cmd_mledit_prompts(int nMLEditType);				// This gets called by the Edit MultiLine dialog box
long  cmd_layer_list(void);								// Being called from cmds2 and cmds3
int   cmd_zoom_extents(int ck_limits);
short cmd_templinklist(const char *fileloc,char *cQuote,char *cDelim,struct cmd_xtemplate **pXtmp,int *reterror);
void  cmd_prt2dpt (sds_point tmppt, LPCTSTR secstr);
void  cmd_prtpt (sds_point tmppt, LPCTSTR secstr);	  //function for printing points (list, ucs list)
void  cmd_prtstr (LPCTSTR fststr, LPCTSTR secstr); //function for printing strings
void  cmd_prttsty (LPCTSTR fststr, LPCTSTR secstr); //function for printing strings (dimtxsty)
void  cmd_prtint (int tmpint, LPCTSTR secstr);	 //function for printing int's
void  cmd_prtlong(long tmpint, LPCTSTR secstr);   //function for printing longs
void  cmd_prtscale (sds_real tmpreal, LPCTSTR secstr); //function for printing scales
void  cmd_prtreal (sds_real tmpreal, LPCTSTR secstr); //function for printing reals
void  cmd_prtarea (sds_real tmpreal, LPCTSTR secstr); //function for printing areas
void  cmd_prtangle (sds_real tmpreal, LPCTSTR secstr);//function for printing angles
short cmd_prtxstr2buf(char *tmpstr,char cQuote,char cDelim,char *outfilter,char *outputline,struct cmd_xtemplate *pXtmp,int *overflowflag);
short cmd_prtxreal2buf(sds_real tmpreal,char cDelim,char *outfilter,char *outputline,struct cmd_xtemplate *pXtmp);
int   cmd_entlister_scrollchk(int *scrollcur, int scrollmax);
short cmd_explode_objects(int inheritmode, bool bInteractive=true, sds_name ssetin=NULL, sds_name ssetout=NULL);
long  cmd_explode_makesubents(sds_name argename, sds_point inspt, sds_point blkinspt, sds_real xscl, sds_real yscl, sds_real zscl,
				sds_real rotang, sds_point extru, int psflag, struct resbuf *rbinherit, resbuf **subEntities);
int cmd_fillet_or_chamfer_pline(sds_name pline,int mode,sds_real dist1,sds_real dist2,sds_point pickpt1,sds_point pickpt2);
int cmd_fillet_or_chamfer_lwpline(sds_name pline,int mode,sds_real dist1,sds_real dist2,sds_point pickpt1,sds_point pickpt2);
//	cmd_fillet_strseg2strseg needed by cmd_fillet_or_chamfer_pline
int cmd_fillet_strseg2strseg(sds_point p1,sds_point p2,sds_point p3,sds_real filletrad,sds_real *segdist,sds_real *newbulg);
int cmd_hatch_blkmidpt(sds_name ename, sds_point result);
void cmd_elev_correct(sds_real zbase, sds_point pchg, int mode, sds_point extru2use);
short cmd_array_func(int mode3d,sds_name rotateset,sds_point ucsbase,sds_point ucsvect,sds_real rotang, bool bInteractive=true, struct rect_array_info *rinfo=NULL, struct polar_array_info *pinfo=NULL, sds_name setofnewents=NULL);
short cmd_view_plan(int getmode);

//*** all these protos are for pline curve fitting and are needed in node dragging
void cmd_pedit_spline_interp3(long *m, sds_point *vertices, long n, sds_point *q,int *span_size);
void cmd_pedit_spline_interp2(long *m, sds_point *vertices, long n, sds_point *ctrl_point,int *span_size);
void cmd_pedit_interp2_tan(sds_point *tangent, long n, sds_point *ctrl_point);
void cmd_pedit_spline_bfit_coeff(double **b, double t, long n, int k, long *c, long i1);
int cmd_pedit_spline_Bfit(long *no_of_vertices, sds_point *curve,long n,
							sds_point *q,int *span_size,int k);
int cmd_decurve_pline(sds_name pent, int mode);
int cmd_pedit_fitcurve(struct resbuf *elist,int mode,int fit_type, int splinesegs,sds_name newent);
//***

BOOL  cmd_PasteItem(void *memptr,db_drawing *To_dbDrawing);

short cmd_3d(void);
short cmd_3darray(void);
short cmd_about(void);
short cmd_aperture(void);
short cmd_arc(void);
short cmd_area(void);
short cmd_array(void);
short cmd_attdef(void);
short cmd_attdisp(void);
short cmd_attedit(void);
short cmd_attext(void);
short cmd_audit(void);
short cmd_base(void);
short cmd_bhatch(void);
/* Albert.Isaev.ITC[31.7.2002/19:22:57] */
short cmd_bhatch_cmdline(void);
short cmd_blipmode(void);
short cmd_block(void);
short cmd_box_mesh();
short cmd_box();
short cmd_bpoly(void);
short cmd_dashBPoly(void);
short cmd_break(void);
short cmd_chamfer(void);
short cmd_change(void);
short cmd_chprop(void);
short cmd_circle(void);
short cmd_cmdbar(void);
short cmd_color(void);
short cmd_compile(void);
short cmd_customize(void);
short cmd_config(void);
short cmd_cone_mesh(void);
short cmd_cone();
short cmd_copy(void);
short cmd_customize(void);
short cmd_cylinder_mesh();
short cmd_cylinder();
short cmd_dblist(void);
short cmd_ddatte(void);
short cmd_ddblocks(void);
short cmd_ddstyle(void);
short cmd_ddapp(void);
short cmd_ddltype(void);
short cmd_ddattext(void);
short cmd_ddattdef(void);
short cmd_ddcolor(void);
short cmd_ddchprop(void);
short cmd_ddedit(void);
short cmd_ddemodes(void);
short cmd_ddfilter(void);
short cmd_ddgrips(void);
short cmd_ddim(void);
short cmd_expdimsty(void);
short cmd_ddinsert(void);
short cmd_ddnew(void);
short cmd_ddlmodes(void);
short cmd_ddosnap(void);
short cmd_ddptype(void);
short cmd_ddmodify(void);
short cmd_ddrmodes(void);
short cmd_ddrename(void);
short cmd_ddselect(void);
short cmd_ddsetvar(void);
short cmd_ddunits(void);
short cmd_dducs(void);
short cmd_dducsp(void);
short cmd_ddview(void);
short cmd_ddvpoint(void);
short cmd_delay(void);
short cmd_defxlink(void);
short cmd_dimangular(BOOL mode,db_drawing *flp);
short cmd_dimtedit(void);
short cmd_dish_mesh();
short cmd_dish();
short cmd_dist(void);
short cmd_divide(void);
short cmd_dome_mesh();
short cmd_dome();
short cmd_donut(void);
short cmd_doughnut(void);
short cmd_dragmode(void);
short cmd_dtext(void);
short cmd_dview(void);
short cmd_dxbin(void);
short cmd_dxfin(void);
short cmd_dxfout(void);
short cmd_edge(void);
short cmd_edgesurf(void);
short cmd_edgesurf_mesh(void);
short cmd_elev(void);
short cmd_ellipse(void);
short cmd_end(void);
short cmd_erase(void);
short cmd_explode(void);
short cmd_extend(void);
short cmd_exit(void);
short cmd_files(void);
short cmd_fill(void);
short cmd_fillet(void);
short cmd_flipscr(void);
short cmd_flatten(void);
short cmd_filmroll(void);
short cmd_graphscr(void);
short cmd_grid(void);
short cmd_gps(void);
short cmd_handles(void);
short cmd_hatch(void);
short cmd_help(void);
short cmd_hide(void);
short cmd_id(void);
short cmd_igesin(void);
short cmd_igesout(void);
short cmd_insert(void);
short cmd_insobject(void);
short cmd_isoplane(void);
short cmd_iswindowopen(void);
short cmd_join(void);
short cmd_layer(void);
short cmd_lengthen(void);
short cmd_limits(void);
short cmd_line(void);
short cmd_ltypelister(void);	// EBATECH(CNBR) 2002/9/14 Share -LAYER and -LINETYPE
short cmd_lweightlister(int flag);	// EBATECH(CNBR) 2003/3/24 Share -LAYER and CHANGE
short cmd_linetype(void);
short cmd_list(void);
short cmd_load(void);
short cmd_loadshape(char* filename); // EBATECH(CNBR) 2002/4/24 Support SHAPE
short cmd_mail(void);
short cmd_measure(void);
short cmd_menu(void);
short cmd_mesh(void);
short cmd_minsert(void);
short cmd_mirror(void);
short cmd_mirror3d(void);
short cmd_move(void);
short cmd_mslide(void);
short cmd_mspace(void);
short cmd_multiple(void);
short cmd_mview(void);
short cmd_mtext(void);
short cmd_cui_mtext(void);	// EBATECH(CNBR)
short cmd_new(void);
short cmd_offset(void);
short cmd_oldmtext(CString text, CString style, double width, int rotation, int justify);
short cmd_oops(void);
short cmd_open(void);
short cmd_openimage(void);
short cmd_ortho(void);
short cmd_osnap(void);
short cmd_pan(void);
short cmd_pedit(void);
short cmd_pface(void);
short cmd_plan(void);
short cmd_pline(void);
short cmd_plot(void);
short cmd_point(void);
short cmd_polygon(void);
short cmd_psdrag(void);
short cmd_psfill(void);
short cmd_psin(void);
short cmd_psout(void);
short cmd_pspace(void);
short cmd_purge(void);
short cmd_pyramid_mesh();
short cmd_pyramid();
short cmd_qsave(void);
short cmd_qtext(void);
short cmd_quit(void);
short cmd_ray(void);
short cmd_recover(void);
short cmd_recmacro(void);
short cmd_rectang(void);
short cmd_redefine(void);
short cmd_redo(void);
short cmd_redraw(void);
short cmd_redrawall(void);
short cmd_regen(void);
short cmd_regenall(void);

short cmd_regenall_lweight();

short cmd_regenauto(void);
short cmd_reinit(void);
short cmd_readaliases(void);
short cmd_readaccelerators(void);
short cmd_rename(void);
short cmd_resume(void);
short cmd_revsurf(void);
short cmd_revsurf_mesh(void);
short cmd_rotate(void);
short cmd_rscript(void);
short cmd_rulesurf(void);
short cmd_rulesurf_mesh(void);
short cmd_runform(void);
short cmd_save(void);
short cmd_saveas(void);
short cmd_scale(void);
short cmd_script(void);
short cmd_select(void);
short cmd_selgrips(void);
short cmd_setvar(void);
short cmd_shade(void);
short cmd_shape(void);
short cmd_sketch(void);
short cmd_snap(void);
short cmd_solid(void);
short cmd_sphere_mesh();
short cmd_sphere();
short cmd_scrlbar(void);
short cmd_statbar(void);
short cmd_status(void);
short cmd_stopmacro(void);
short cmd_stretch(void);
short cmd_style(void);
short cmd_dashStyle(void);
short cmd_tablet(void);
short cmd_tabsurf(void);
short cmd_tabsurf_mesh(void);
short cmd_text(void);
short cmd_textscr(void);
short cmd_toolbar(void);
short cmd_torus_mesh();
short cmd_torus();
short cmd_time(void);
short cmd_tipofday(void);
short cmd_trace(void);
short cmd_treestat(void);
short cmd_trim(void);
short cmd_u(void);
short cmd_ucs(void);
short cmd_ucsicon(void);
short cmd_undefine(void);
short cmd_undo(void);
short cmd_units(void);
short cmd_vba(void);
short cmd_vbaload(void);
short cmd_vbaunload(void);
short cmd_vbarun(void);
short cmd_dashvbarun(void);
short cmd_addinrun(void);
short cmd_vbasecurity(void);
short cmd_view(void);
short cmd_viewctl(void);
short cmd_viewports(void);
short cmd_viewres(void);
short cmd_vplayer(void);
short cmd_vpoint(void);
short cmd_vslide(void);
short cmd_wedge_mesh();
short cmd_wedge();
short cmd_xbind(void);
short cmd_xline(void);
short cmd_xref(void);

/*-------------------------------------------------------------------------*//**
Uncouple xref.
NOTE: when binding or inserting an xref, this f'n performs table modifications

@author ?
@param	xrefPattern		 =>
@param	bindmode		 => IC_UNCOUPLE_DETACH:	detaches all xrefs matching pattern and removes all table references to xrefs.
							IC_UNCOUPLE_BIND:	binds all refs matching pattern, altering table references
							IC_UNCOUPLE_INSERT:	inserts all refx matching pattern, converting them to standard INSERTs.
@param	displayMessages	 => display messages
@return if successful, returns # of xrefs detached/bound/inserted, if return val<0, there's an error
*//*--------------------------------------------------------------------------*/
int		cmd_xref_uncouple
(
const char	*xrefPattern,
int		bindmode,
bool	displayMessages
);


short cmd_lweight(void);

short cmd_zoom(void);
short cmd_3dface(void);
short cmd_3dmesh(void);
short cmd_3dpoly(void);
short cmd_rotate3d(void);
void catTwoResourceStrings(char *FirstStr, char *SecondStr, char *RetStr);
int getCurrColorIndex();

short cmd_query(void);
short cmd_extopen(void);

short cmd_colordlg(void);
short cmd_helpdlg(void);
short cmd_sortlist(void);

short cmd_setlayer(void);
short cmd_matchprop(void);
short cmd_close(void);
short cmd_saveall(void);
short cmd_print(void);
short cmd_psetup(void);
short cmd_ppreview(void);
short cmd_wcascade(void);
short cmd_whtile(void);
short cmd_wiarange(void);
short cmd_wvtile(void);
short cmd_wopen(void);
short cmd_wclose(void);
short cmd_wcloseall(void);
short cmd_wsplit(void);
short cmd_windows(void);
short cmd_exit(void);
short cmd_tbconfig(void);

short cmd_xplode(void);
short cmd_subtract(void);
short cmd_union(void);
short cmd_tolerance(void);
short cmd_syswindows(void);
short cmd_splinedit(void);
short cmd_spline(void);
short cmd_spell(void);
short cmd_saveasr12(void);
short cmd_render(void);
short cmd_mline(void);
short cmd_mlstyle(void);
short cmd_mledit(void);
short cmd_appload(void);
short cmd_arx(void);
short cmd_menuload(void);
short cmd_menuunload(void);
short cmd_logfileoff(void);
short cmd_logfileon(void);
short cmd_intersect(void);
//short cmd_import(void);
short cmd_export(void);

short cmd_filter(void);
short cmd_extrude(void);
short cmd_copyhist(void);
short cmd_cal(void);
short cmd_bmpout(void);
short cmd_replay(void);
short cmd_rendscr(void);
short cmd_region(void);
short cmd_pastespec(void);
short cmd_pcxin(void);
short cmd_olelinks(void);
short cmd_xrefclip(void);
short cmd_acisin(void);
short cmd_acisout(void);
short cmd_attredef(void);
//short cmd_boundary(void);
short cmd_copylink(void);
short cmd_gifin(void);
short cmd_hatchedit(void);
short cmd_interfere(void);
short cmd_light(void);
short cmd_massprop(void);
short cmd_matlib(void);
short cmd_mvsetup(void);
short cmd_revolve(void);
short cmd_rmat(void);
short cmd_rpref(void);
short cmd_saveimg(void);
short cmd_scene(void);
short cmd_section(void);
short cmd_showmat(void);
short cmd_slice(void);
short cmd_soldraw(void);
short cmd_solidedit(void);
short cmd_solprof(void);
short cmd_solview(void);
short cmd_stlout(void);
short cmd_tiffin(void);
short cmd_vlconv(void);
short cmd_wmfin(void);
short cmd_wmfopts(void);
short cmd_wmfout(void);
short cmd_editxdata(void);
short cmd_xclip();

short cmd_none(void);
short cmd_nearest(void);
short cmd_endpoint(void);
short cmd_midpoint(void);
short cmd_center(void);
short cmd_intersection(void);
short cmd_3dintersection(void);
short cmd_2dintersection(void);
short cmd_planviewint(void);
short cmd_visible(void);
short cmd_perpendicular(void);
short cmd_quadrant(void);
short cmd_insertion(void);
short cmd_node(void);
short cmd_tangent(void);
short cmd_quick(void);
short cmd_tolerance(void);
short cmd_coordinate(void);
//short cmd_background(void);
short cmd_browser(void);
short cmd_convert(void);
short cmd_dwfout(void);
short cmd_vmlout(void);
short cmd_fog(void);
short cmd_imageadjust(void);
short cmd_imageattach(void);
short cmd_imageclip(void);
short cmd_imageframe(void);
short cmd_imagequality(void);
short cmd_lsedit(void);
short cmd_lslib(void);
short cmd_lsnew(void);
short cmd_setuv(void);
short cmd_transparency(void);
short cmd_pmspace(void);
short cmd_rtpan(void);
short cmd_rtzoom(void);
short cmd_rtrotx(void);
short cmd_rtroty(void);
short cmd_rtrotz(void);
short cmd_rtrot(void);

short cmd_copyclip(void);
short cmd_pasteclip(void);
short cmd_cutclip(void);
short cmd_dash_group(void);
short cmd_loadlispmnl(const char *fname);
short cmd_qprint(void);
//Modified Cybage SBD 06/04/2001 DD/MM/YYYY
//Reason : Mail from Surya Sarda(suryas@cadopia.com) Dated 03/04/2001 DD/MM/YYYY.
short cmd_onweb(void);
//<alm>
short cmd_3dconvert();
short cmd_commands();
//</alm>

short cmd_CleanUpDimVarStrings(void);		// Added to handle cleaning up after each dim function.			LSE
void  cmd_DeleteDimStructAndStrings(void);	// Added so SDS_EXIT can delete dimension struct from cmds12	LSE

//Old hatch structs and functions which are reused by bhatch

//Structures to use for hatch command.	Note: hpdef will point to a link in the
//	hplinedef.	The hplinedef is linked in a loop (end->next=beg), so free it
//	very carefully!
struct cmd_hplinedef{
	sds_real drawlength;		//length of stroke (shut up, Beavis!)
	short pendown;				//1= pen down draw, 0=pen up skip, -1= continuous line
								//		(drawlength ignored for -1)
	struct cmd_hplinedef *next;	//to enable llist
};
struct cmd_hpdefll {
	cmd_hplinedef *linedef;		//pointer to 1st line in pattern
	sds_real startpt[2];		//pattern start point
	sds_real xoffset;			//x offset for every row
	sds_real yoffset;			//distance between successive rows
	sds_real angle;				//angle for lines, stored in RADIANS
	sds_real linelen;			//sum of lengths of all pen strokes
	struct cmd_hpdefll *next;		//to enable llist
};

long	cmd_hatch_list(void);			/* Albert.Isaev.ITC[31.7.2002/15:36:02] For cmd_bhatch_cmdline */
int		cmd_hatch_valid(char *hpname,char *patfile);	/* Albert.Isaev.ITC[31.7.2002/15:36:42] For cmd_bhatch_cmdline */
void	cmd_hatch_freehpdefll(struct cmd_hpdefll **def2free);
int		cmd_hatch_loadpat(const char *hpfile, const char *hpname,struct cmd_hpdefll **hplcur);

// Note the space can be calculated in the load as it was in the original hatch command by setting the
// space parameter.  This can be handled outside the routine in the same way scale is handled by leaving
// space to a default 1;
bool cmd_loadUserDefPattern(cmd_hpdefll **phplcur, int hpdouble, double hpspace = 1);

//bpoly/bhatch functions
class HanditemCollection;
class CBPolyBuilder;
short cmd_getEntitySet(HanditemCollection &collection);
short cmd_getAllEntities(HanditemCollection &collection);
short cmd_boundaryPointSelection(sds_point pt, CBoundaryBuilder &builder, HanditemCollection &boundaryEnts);
void cmd_drawHICollection(HanditemCollection *selectedEnts, int howToDraw);
short cmd_boundaryAccept(CBoundaryBuilder &polyBuilder);
short cmd_boundaryCancel(CBoundaryBuilder &polyBuilder);
void cmd_redrawEntity (db_handitem *hip, int howToDraw);

//assoc functions
void cmd_updateReactors (db_handitem *entity);
void cmd_updateSSReactors(const sds_name selSet);  //checks all the reactors in a selection set

// Some operations require certain entities to break the associations if their associated entities
// are not part of the selection set.  This gives an entity a chance to do this if it should.
void cmd_checkReactors (const sds_name selSet);

sds_real cmd_dist2d(sds_point pt1, sds_point pt2);
short cmd_SaveFunc(short saveasmode, CIcadDoc* pDoc);
long  cmd_alloc_elistll(struct cmd_elistll **beg, struct cmd_elistll **cur, sds_name ename, short assocval);
void  cmd_free_elistll(struct cmd_elistll **beg, struct cmd_elistll **cur);
//int   cmd_pt_equal(sds_point p1, sds_point p2);
//int   cmd_direction_equal(sds_point p1,sds_point p2, sds_point p3, sds_point p4);
//int   cmd_real_equal(sds_real r1, sds_real r2);
//Modified Cybage SBD 30/04/2001 DD/MM/YYYY
//Reason : Mail from Ben Thump(mcthum@siacad.com) Dated 17/04/2001 DD/MM/YYYY.Re: join not working
//int   cmd_real_equal2(sds_real r1, sds_real r2);
//int   cmd_angle_equal(sds_real r1, sds_real r2);
int   cmd_make_pline_end(sds_name ename, int closeflag, sds_point closept);
int   cmd_make_pline_start(sds_name startent, sds_name endent);
int   cmd_get_startend(sds_name ent, sds_point startpt, sds_point endpt);//plines only!
long  cmd_pl_vtx_no(sds_name pent, sds_point ppucs, int elevadj, int *exactvert);
long  cmd_lwpl_vtx_no(sds_name pent, sds_point ppucs, int elevadj, int *exactvert);
int   cmd_ang_betw(sds_real angchk, sds_real startang, sds_real endang);
int   cmd_pline_entnext(sds_name ent, sds_name result, int closed);
int	  cmd_lwpline_entnext(int thisvert,db_lwpolyline *lwpl);
int   cmd_pline_dist(sds_name plent, sds_point p1, sds_point p2, sds_real *dist);
short cmd_PolylineFunc(short ThreeDmode);
short cmd_NewFunc(void);
int   cmd_lengthen_pline_end(sds_name pline,sds_real dist);
short cmd_erase_and_oops(sds_name argss, short oopsmode);
int   cmd_offset_linepts(sds_point p1,sds_point p2, sds_real offset, sds_point p3, sds_point p4);
int   cmd_offset_arcpts(sds_point p1,sds_point p2,sds_real bulge,sds_real offset,sds_point p3,sds_point p4);
long  cmd_ss2elistll(struct cmd_elistll **beg, struct cmd_elistll **cur, sds_name ss1, short assocval);
void  cmd_free_elistll(struct cmd_elistll **beg, struct cmd_elistll **cur);
short cmd_CreateLeader(const struct resbuf *elist);
int   cmd_pline_next2lastvtx(sds_name mainent,sds_name next2lastvtx);
long  cmd_block_list(bool mode);
int   cmd_drag(short mode, sds_name ss1, LPCTSTR prompt, char *initkwl,sds_point respt, char *inputstr);
short cmd_WriteSldofMf(char *fname);
short cmd_color_fromuser(struct resbuf *rb, int intmode);
void cmd_MakeVpActive(CIcadView *pView,db_handitem *vphip,db_drawing *dp);
bool cmd_SetActiveMView( CIcadView *pView, int vpnum, db_drawing *dp );

short cmd_isinpspace(void);

// Function that returns the number of Mview ports.
int cmd_GetNumberOfMViewPorts(void);


short cmd_istransparent(void);
struct resbuf *cmd_listsorter(struct resbuf *ab);
int cmd_CreateNewViewports(db_drawing *flp);
int cmd_setActiveDimStyle (struct resbuf *elist);

/*-------------------------------------------------------------------------*//**
This function sets the active dimstyle to the current value of the dimstyle
system variable.

@author Martin Waardenburg
*//*--------------------------------------------------------------------------*/
int cmd_dimStyleSet();

// Protos from surfaces.cpp
short cmd_tabsurf_waves(sds_name ent,sds_point vpt,bool closeflag,bool ecs1flag,bool reverseflag);
short cmd_tabsurf_nonspline(sds_name ent,sds_point vpt,bool ecs1flag,bool reverseflag);
short cmd_reverse_link_list(struct resbuf **linklist);
short cmd_rev_it_up(sds_name ename1,sds_name ename2,sds_point pt2,sds_real sa,sds_real ea,short flag1,short flag2);
short cmd_divmeas_pline_pts(sds_name ent,short curvesegs,long *vcount,struct resbuf **result);
short cmd_coons(sds_name ename1,sds_name ename2,sds_name ename3,sds_name ename4,short ecsflag1,short ecsflag2,short ecsflag3,short ecsflag4);
int   cmd_revolve_pl_ptarray(sds_point cc,sds_point extru_axis, sds_point *ptarray,int vcnt, sds_real totalangle, int ribs, struct resbuf *vtx2make);
short cmd_amess(sds_point pt1,sds_point pt2,sds_point pt3,sds_point pt4,int msize,int nsize);
// Protos from 3dsolids.cpp
short cmd_box_create(sds_point pt1,sds_point pt2, sds_real high);
short cmd_wedge_create(sds_point pt1,sds_point pt2, sds_real high,double ang);
short cmd_cone_create(sds_point pt0,sds_real bdia,sds_real tdia,sds_point pthigh,int nseg);
short cmd_torus_create(sds_point pt0,sds_real tordia,sds_real tubedia,int torseg,int tubseg);
short cmd_sphere_create(sds_point pt0,sds_real rad,int longseg,int latseg, int domemode);
short cmd_pyramid_create(int mode, sds_point bpt1,sds_point bpt2,sds_point bpt3,sds_point bpt4,sds_point tpt1,sds_point tpt2,sds_point tpt3,sds_point tpt4);
short cmd_wedge3d(void);
short cmd_cone_create_elp(sds_point cent,sds_point v1,sds_point v2,sds_real ratio,sds_real pthigh,int iseg);
short cmd_cyl_create(sds_point pt0,sds_real dia,sds_point pthigh,int nseg);
short cmd_cyl_create_elp(sds_point cent,sds_point v1,sds_point v2,sds_real ratio,sds_point pthigh,int nseg);
int cmd_revolve_pl_ptarray(sds_point cc,sds_point extru_axis, sds_point *ptarray,int vcnt, sds_real totalangle, int ribs, struct resbuf *vtx2make);

// Lightweight polyline resbuf handling
void LwplrbSetNumVerts(resbuf *rb);
void LwplrbAddVert(db_lwpolyline *lwpl, resbuf **lwpladdrb, int vertno);
void LwplrbAddVertFromData(resbuf **lwpladdrb, sds_point pt, sds_real startwidth, sds_real endwidth, sds_real bulge);
void LwplrbDeleteVertexData(resbuf *lwpl);



/*||||||||||||||||||||||||||||||||||||
||			EED Functions			||
|||||||||||||||||||||||||||||||||||*/
short cmd_copyeed(sds_name ename, sds_name ss, char *appname);
short cmd_moveeed(sds_name ename, sds_name ss, char *appname);
short cmd_deleed(sds_name ss, char *appname);
short cmd_renamevarf(sds_name ss, char *fs1, char *fs2);
short cmd_renameed(sds_name ss, char *oldapp, char *newapp, short mode);
short cmd_copyedata (void);
short cmd_renameapp (void);
short cmd_moveedata (void);
short cmd_deledata (void);

long  cmd_SaveAs(db_drawing *pCurDwg, char* fname, int nStrLen,int exportmode);
int   cmd_makedimension(db_handitem *elp,db_drawing *flp,short dragmode);
int   cmd_makewindowport(sds_point pt1,sds_point pt2,db_handitem *TabEnt);

short dotheextract(sds_name ssname,struct cmd_xtemplate *pXbeg,char *outputline,FILE *outputfile,char cQuote,char cDelim,char *outfilter);
int internalGetpoint(const sds_point ptReference, const char *szPointMsg, sds_point ptPoint, bool internalCall = TRUE);

short purge_drawing (db_drawing *dp, char *fs3);
short cmd_layer_check(char *layerlst);

int RemoveHatchFromSS(sds_name ss);

int IC_DrawSlideFile( char *szSlideFileName, HDC hDC, RECT *pRect );

// Dimension helper functions
BOOL checkBlock(char *blockName,db_drawing *flp);

int   cmd_InitQRender(int initmode);
short cmd_qrender(void);
short cmd_qroptions(void);
short	cmd_dwgcodepage(void); // EBATECH(CNBR) 2001-06-16 : Force DWGCODEPAGE System Variable
void	cmd_ErrorPrompt(int promptID, short mode, void* arg1 = NULL, void* arg2 = NULL, void* arg3 = NULL);
//Bugzilla::78503; 28-04-03
short cmd_ieHelp(void);
#if defined(__cplusplus)
	}
#endif


#endif // _CMDS_H

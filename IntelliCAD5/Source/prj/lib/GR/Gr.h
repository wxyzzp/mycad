/* G:\ICADDEV\PRJ\LIB\GR\GR.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
#ifndef __grH__
#define __grH__

#if defined( GRDLL )
	#define	GR_CLASS __declspec( dllexport)
	#define	GR_DECLSPEC __declspec( dllexport)
#else
	#if defined( GRSTATIC )
		#define	GR_CLASS
		#define	GR_DECLSPEC
	#else
		#define	GR_CLASS __declspec( dllimport)
		#define	GR_DECLSPEC __declspec( dllimport)
	#endif
#endif

#ifndef	GR_API
	#define	GR_API extern "C" GR_CLASS
#endif

#define _USE_DISP_OBJ_PTS_3D_
#undef _DOCLASSES_

#define GR_DIST(P0,P1) sqrt(((P1)[0]-(P0)[0])*((P1)[0]-(P0)[0])+((P1)[1]-(P0)[1])*((P1)[1]-(P0)[1])+((P1)[2]-(P0)[2])*((P1)[2]-(P0)[2]))
#define GR_PTCPY(P0,P1) (P0)[0]=(P1)[0],(P0)[1]=(P1)[1],(P0)[2]=(P1)[2]
#define GR_LEN(V) sqrt((V)[0]*(V)[0]+(V)[1]*(V)[1]+(V)[2]*(V)[2])
#define GR_UNITIZE(V,L) (V)[0]/=L,(V)[1]/=L,(V)[2]/=L

#include <afxwin.h>
#include "db.h"

#include "icaddef.h"
#include "sds.h"
#include "gr_view.h"
#include "DoDef.h"

#include <math.h>
#include <string.h>

#if defined(__cplusplus)
	extern "C"
	{
#endif

//8/98 I don't believe this is used anymore so commented for now
//#if defined(GR_DEFVARS)
//	  #define GR_EXTERN
//#else
//	  #define GR_EXTERN extern
//#endif
//#if defined(GR_EXPORTS)
//	#define	GR_EXPORT __declspec( dllexport)
//#else
//	#define	GR_EXPORT __declspec( dllimport)
//#endif
					  /***** PROTOTYPES *****/
short gr_arc2pc(
	short	 mode,
	sds_real r0,
	sds_real r1,
	sds_real r2,
	sds_real r3,
	sds_real r4,
	sds_real elev,
	short	 cat,
	sds_real swid,
	sds_real ewid,
	short	 ltsclwblk,
	sds_real pixsz,
	int 	 curvdispqual,
	struct gr_view *viewp);
short gr_point2doll(
	sds_point *entaxis,
	struct gr_displayobject **begpp,
	struct gr_displayobject **endpp,
	struct gr_view			 *viewp);
GR_API int gr_textbox(
	struct resbuf		*ent,
	sds_point			 p1,
	sds_point			 p2,
	db_drawing			*dp,
	struct gr_view		*viewp,
	CDC					*dc);
short gr_getucs(
	sds_point	  org,
	sds_point	  xdir,
	sds_point	  ydir,
	sds_point	  zdir,
	struct gr_viewvars *viewvarsp,
	db_drawing	 *dp);
GR_API short gr_fixtextjust(
	db_handitem 	*enthip,
	db_drawing		*dp,
	struct gr_view	*viewp,
	CDC				*dc,
	const int		 mtextVert);			// if mtext, need to know if shape specifies vertical--72
GR_API short gr_initview(
	db_drawing			 *dp,
	db_charbuflink		 *configbuf,
	struct gr_view	   **viewpp,
	struct gr_viewvars	*viewvarsp);
void gr_ncs2ucs(
	sds_point		 sour,
	sds_point		 dest,
	short			 disp,
	struct gr_view *viewp);
void gr_ucs2ncs(
	sds_point		 sour,
	sds_point		 dest,
	short			 disp,
	struct gr_view *viewp);
short gr_setupixfp(struct gr_view *viewp);
GR_API short gr_ucs2rp(
	sds_point sour,
	sds_point dest,
	struct gr_view *viewp);
// EBATECH(CNBR) -[ Explode MTEXT
GR_API int ExplodeMtext(
  sds_name			ename,
  std::vector<long>&  ssetnew,
  gr_view*			pView,
  db_drawing*		pDrawing,
  CDC*				pDC
);
// ]- EBATECH(CNBR)

GR_API short gr_rp2ucs(
	sds_point		 sour,
	sds_point		 dest,
	struct gr_view *viewp);
GR_API void gr_rp2pix(
	struct gr_view *p_viewp,
	sds_real		 p_rx,
	sds_real		 p_ry,
	int 			*p_ixp,
	int 			*p_iyp);
GR_API void gr_pix2rp(
	struct gr_view *p_viewp,
	int 			 p_ix,
	int 			 p_iy,
	sds_real		*p_rxp,
	sds_real		*p_ryp);
GR_API int gr_vect_rp2pix(
	sds_real		*p_rp0,
	sds_real		*p_rp1,
	int 			*p_ip,
	struct gr_view *p_viewp);
GR_API short gr_twist4ucs(db_drawing *dp, sds_real *dontset);
GR_API void  gr_freeviewll(struct gr_view *vll);
GR_API void  gr_clean(void);

GR_API void gr_freeselector(struct gr_selectorlink *sl);
GR_API struct gr_selectorlink *gr_initselector(
	char		   type,
	char		   mode,
	struct resbuf *def);
short gr_segxbox(
	sds_real  *p0,
	sds_real  *p1,
	sds_point *box);
GR_API short gr_selected(
	db_handitem *enthip,
	struct gr_selectorlink *sl);
GR_API int gr_nentselphelper(
  /* Given (all required): */
	int 			  mode,
	db_handitem 	 *outerhip,
	gr_selectorlink  *sl,
	gr_view 		 *viewp,
	db_drawing		 *dp,
	CDC				 *dc,
  /* Results (NULL okay for unwanted ones): */
	db_handitem 	**innerhipp,
	sds_matrix		  xformres,
	resbuf			**refstkrespp);

#if defined(GR_DEFVARS)

	char *gr_str_0			="0";
	char *gr_str_acad		="ACAD";
	char *gr_str_byblock	="BYBLOCK";
	char *gr_str_bylayer	="BYLAYER";
	char *gr_str_continuous ="CONTINUOUS";
	char *gr_str_defpoints	="DEFPOINTS";
	char *gr_str_invalid	="INVALID";
	char *gr_str_msblockname="*MODEL_SPACE"; // 2002/10/30 Ebatech "*MSPACE_BLOCK";
	char *gr_str_psblockname="*PAPER_SPACE"; // 2002/10/30 Ebatech "*PSPACE_BLOCK";
	char *gr_str_quotequote ="";
	char *gr_str_standard	="STANDARD";
	char *gr_str_staractive ="*ACTIVE";
	char *gr_str_txt		="TXT";

	/* These two are the same as their db_ counterparts (which gr can't use): */
#ifdef DWGCNVT
	char *gr_icfontnm="DWG",*gr_icfontfn="DWG.FNT";
#else
	char *gr_icfontnm="ICAD",*gr_icfontfn="ICAD.FNT";
#endif


	char *gr_tolfontnm="GDT";  /* The TOLERANCE symbols font. */

#else

	extern char *gr_str_0;
	extern char *gr_str_acad;
	extern char *gr_str_byblock;
	extern char *gr_str_bylayer;
	extern char *gr_str_continuous;
	extern char *gr_str_defpoints;
	extern char *gr_str_invalid;
	extern char *gr_str_msblockname;
	extern char *gr_str_psblockname;
	extern char *gr_str_quotequote;
	extern char *gr_str_standard;
	extern char *gr_str_staractive;
	extern char *gr_str_txt;

	extern char *gr_icfontnm,*gr_icfontfn;

	extern char *gr_tolfontnm;

#endif

#if defined(__cplusplus)
	}
#endif

__declspec( dllexport) int
gr_getdispobjs(
	db_drawing				*p_topdp,
	db_blocktabrec			*xrefblkptr,
	db_handitem				**p_hipp,
	struct gr_displayobject	**p_begdopp,
	struct gr_displayobject	**p_enddopp,
	sds_point				*p_ucsextp,
	db_drawing				*p_dp,
	struct gr_view			*p_viewp,
	short					p_mode,
	CDC						*dc = NULL,
	db_viewport				*pInViewport = NULL,
	bool					bMeshAcisEntities = false);		/*D.G.*/// 'true' to get mesh surface disp. objs for ACIS entities.

//protos from gr1
GR_API short gr_rayxlinegenpts(
	sds_point	  p0,
	sds_point	  vect,
	short		  type,
	sds_point	  res0,
	sds_point	  res1,
	gr_view 	*viewp,
	db_drawing	 *dp);

int gr_leader2doll(
	struct gr_displayobject	**begdopp,
	struct gr_displayobject	**enddopp,
	short					projmode,
	struct gr_view			*viewp,
	db_drawing				*dp,
	CDC						*dc = NULL);
int gr_mline2doll(
	sds_point		  *entaxis,
	gr_displayobject **begdopp,
	gr_displayobject **enddopp,
	gr_view 		  *viewp,
	db_drawing* pDrawing,
	CDC* pDC);

int gr_gg3spline(
	int 	  *m,
	sds_point *vertices,
	int 	   n,
	sds_point *q,
	int 	  *span_size);

__declspec( dllexport) short ApplyLayerStatesToXref(db_drawing *topdp,db_drawing *xrefdp,db_blocktabrec *xrefblkptr);
__declspec( dllexport)short GetTopLevelLayerHip(db_drawing *topdp,db_blocktabrec *xrefblkptr,db_handitem **laycheckhip);

__declspec( dllexport) bool TrueTypeTextForGDI(db_handitem *text, gr_view *view,
	bool *haveTrueTypeFont = NULL);

#endif



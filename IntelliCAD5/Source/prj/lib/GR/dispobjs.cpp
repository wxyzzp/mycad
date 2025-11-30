/* D:\ICADDEV\PRJ\LIB\GR\DISPOBJS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * gr_getdispobjs
 *
 *
 */
#include "..\..\ICAD\Configure.h"

#include "gr.h"/*DNT*/
#include "drw.h"/*DNT*/
#include "tchar.h"/*DNT*/
#include "splines.h"/*DNT*/
#include "gr_DisplaySurface.h"/*DNT*/
#include "gr_view.h"/*DNT*/
#include "viewport.h"/*DNT*/
#include "styletabrec.h"/*DNT*/
//#include "truetypedispobjs.h"/*DNT*/
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include <float.h>		// for DBL_MAX

#include "pc2dispobj.h"
#include "DoBase.h"

#include "linetypegen.h"
#include "textgen.h"
#include "tolerancegen.h"
#include <stack>
#include "objects.h"
#include "mtextgen.h"
//#include "gr_wordwrap.h"	EBATECH(CNBR) move to mtextgen.cpp
#include "Modeler.h"
#include "DbAcisEntity.h"
//<alm>
#include "gvector.h"
using icl::point;
using icl::gvector;
#include "transf.h"
using icl::transf;
using icl::translation;
#include "CmdQueryTools.h"//ucs2wcs
//</alm>
#include "DbMText.h"
#include "..\..\ICAD\res\icadrc2.h"

#undef _BIG_TRANSFORMATION_COMPUTATION_

static void xformpt(
	sds_point	from,
	sds_point	to,
	sds_matrix	mat);

/*
**							  DEFINITIONS
**
**	NCS
**
**		An entity's Native Coordinate System -- WCS or ECS -- depending
**		on the entity type.
**
**	RP
**
**		Real Projection plane.	Similar to the DCS, except that Z is not
**		currently being stored.  Also, gr_ucs2rp() and gr_rp2ucs()
**		work correctly in perspective mode; sds_trans() involving
**		the DCS does not.
**
**
**							GENERAL STRATEGY
**
**	For each entity, gr_getdispobjs() (or one of its extension
**	functions) defines a llist of points in viewp->pc.	If it's an
**	entity that supports linetype, that llist is given to gr_doltype()
**	to break it up into pieces, and each piece is given to
**	gr_gensolid() to create the actual display objects llist (in RP
**	coordinates).  If the entity does not support linetype, the pc llist
**	is sent directly to gr_gensolid().	The conversion from the NCS
**	system to the UCS can occur in one of two places.  If it's done
**	before gr_doltype() is called, the INSERT-scaling does NOT scale
**	linetype. If it's done after the call to gr_doltype(),
**	INSERT-scaling DOES scale linetype. I originally did it the latter
**	way, then discovered that ACAD does it the former way.	Therefore,
**	we created system variable LTSCLWBLK to decide which way to do it,
**	and kept the "wrong" method as an option.
**
**	gr_getdispobjs() does a trick to transform an entity from its NCS
**	into the UCS for the display objects.  It sets up the point and unit
**	vectors defining the NCS origin and axes and then transforms them
**	from the inner-most block all the way up to the WCS, and then to
**	the UCS.  Those "distorted" axis vectors and translated origin are
**	then used to convert NCS points to the UCS (see gr_ncs2ucs()).
**	This transformed system is described by viewp->xfp[].  See
**	gr_getdispobjs() for a more detailed explanation of this trick.
**
**
**								 NOTES
**
**	IC_ZRO versus 0.0 when working with vectors:
**
**		My latest strategy is as follows.  If you're unitizing a vector,
**		check the length for 0.0.  If you're crossing two UNIT vectors to get
**		a third, check the length of the resulting vector for <IC_ZRO to
**		detect parallel (or anti-parallel) source vectors.	When dotting
**		two vectors to check for perpendicularity, check the result for
**		fabs(result)<IC_ZRO. (I don't think there ARE any of this latter
**		kind yet in this code.)
*/

static char *gr_thisfile=__FILE__;
char *gr_errform="\nError at line %ld in %s."/*DNT*/;

short ApplyLayerStatesToXref(db_drawing *topdp,db_drawing *xrefdp,db_blocktabrec *xrefblkptr)
{
// scan this drawing and apply any xref layer states to the xref
// also apply the state of layer 0 to the xref's layer 0, although this may not be necessary, and
// could even be wrong.
// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight

	ASSERT(topdp!=NULL);
	if (topdp==NULL) return RTERROR;

	ASSERT(xrefdp!=NULL);
	if (xrefdp==NULL) return RTERROR;

	db_handitem *hip,*hip2;
	db_layertabrec *layhip,*xreflayhip;
	char *vbarptr;
	char *xreflayname,templayname[512];
	int	lweight;

    db_handitem* firstTabRecMain = NULL, *lastTabRecMain = NULL;
	topdp->get_tabrecllends(DB_LAYERTABREC, &firstTabRecMain, &lastTabRecMain);
    
	db_handitem* firstTabRecXref = NULL, *lastTabRecXref = NULL;
	xrefdp->get_tabrecllends(DB_LAYERTABREC, &firstTabRecXref, &lastTabRecXref);

	for(hip = firstTabRecMain; hip != NULL; hip = hip->next) 
	{	// search all layers
		if(hip->ret_deleted())
			continue;
		layhip=(db_layertabrec *)hip;
		strcpy(templayname,layhip->retp_name());	// see if the layer is from an xref
		vbarptr=strchr(templayname,'|');
		if (vbarptr!=NULL) {						// if so, get the layer name and block name
			*vbarptr=0;
			if (strisame(templayname,xrefblkptr->retp_name())) { // this is the xref block -- templayname is really
													// just the blockname, now that the end is cut off
				xreflayname=&vbarptr[1];			// layer name is 2nd part
				for (hip2 = firstTabRecXref; hip2 != NULL; hip2 = hip2->next) 
				{	// search all layers in xreef
					if(hip2->ret_deleted())
						continue;
					xreflayhip=(db_layertabrec *)hip2;
					if (strisame(xreflayname,xreflayhip->retp_name())) {		// this is the layer
						xreflayhip->set_62(layhip->ret_color());			// set xref layer color (& therefore on/off)
						xreflayhip->set_70(layhip->ret_flags());			// set xref layer flags (for frozen)
						xreflayhip->set_6(layhip->ret_ltype());				// set xref layer ltype
						layhip->get_370(&lweight);							// set xref layer lweight
						xreflayhip->set_370(lweight);
						break;	// out of the hip2 loop
					}
				}
			}
		}
	}

	// now set layer 0 in the xref
	for(hip = firstTabRecMain; hip != NULL; hip = hip->next) 
	{	// find layer 0 in top
		layhip=(db_layertabrec *)hip;
		if (strsame(layhip->retp_name(),"0"/*DNT*/)) {
			for (hip2 = firstTabRecXref; hip2 != NULL; hip2 = hip2->next) 
			{		// search all layers in xreef
				if(hip2->ret_deleted())
					continue;
				xreflayhip=(db_layertabrec *)hip2;
				if (strsame(xreflayhip->retp_name(),"0"/*DNT*/)) {	// this is the layer
					xreflayhip->set_62(layhip->ret_color());		// set xref layer color (& therefore on/off)
					xreflayhip->set_70(layhip->ret_flags());		// set xref layer flags (for frozen)
					xreflayhip->set_6(layhip->ret_ltype());			// set xref layer ltype
					layhip->get_370(&lweight);						// set xref layer lweight
					xreflayhip->set_370(lweight);
					break;	// out of the hip2 loop
				}
			}
			break;	// out of the hip loop
		}
	}
	return RTNORM;
}


// given the top level drawing and an xref block name, see if the indicated layer is
// listed (in xrefname|layername form), and if so return a pointer to it
short GetTopLevelLayerHip(db_drawing *topdp,db_blocktabrec *xrefblkptr,db_handitem **laycheckhip)
{
	char toplevellayname[512];
	db_handitem *foundlayerhip;

	ASSERT(topdp!=NULL);
	ASSERT(xrefblkptr!=NULL);

	sprintf(toplevellayname,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),((db_layertabrec *)(*laycheckhip))->retp_name());
	if ((foundlayerhip=topdp->findtabrec(DB_LAYERTAB,toplevellayname,0))!=NULL) {
		*laycheckhip=foundlayerhip;
	}
	return(RTNORM);
}


short gr_genClippingBoundary(db_entity* pSourceEntity, db_drawing* pDrawing, gr_view* pGrView, gr_displayobject** ppBegDO, gr_displayobject** ppEndDO);
bool gr_acisGetDispObjs(db_entity* pEntity, db_drawing* pDrw, gr_view* pGrView, bool bMeshAcisEntities, gr_displayobject** ppBegDO, gr_displayobject** ppEndDO);


/*****************************************************
// PUBLIC FUNCTION
//
// gr_getdispobjs
//
//
**	CALL SUCCESSFULLY BEFORE CALLING THIS FUNCTION.
**	The p_viewp members must be set from the system variables
**	that define the view and display.
**
**	Reads the entity definition in *p_hipp and allocates a llist of
**	struct gr_displayobject links for the type of projection specified
**	by p_mode and p_viewp->mode.  *p_begdopp is pointed to the first
**	link; *p_enddopp is pointed to the last link (for ease in chaining
**	together the results of several calls).
**
**	*p_hipp is updated for the caller for the next main entity; walking
**	complex entities occurs in here.
**
**	If p_ucsextp!=NULL, it must point to an array of at least 2
**	sds_points.  The UCS extent of the entity under consideration
**	will be set for the caller (p_ucsext[0] will have the minimums;
**	p_ucsext[1] will have the maximums).
**
**	p_dp is the usual pointer to the "big class" for the drawing under
**	consideration.	We need the tables.
**
**	The projection mode is specified by p_mode and p_viewp->mode. p_mode
**	is 0 (or 2) when you want UCS (3D) objects and 1 for planar (2D)
**	projections.  (p_viewp->mode comes from VIEWMODE; bit 0 (1) flags
**	perspective mode).	p_mode and p_viewp->mode are used to set
**	p_viewp->projmode for ease of use thereafter.
**
**	  p_mode==0 (p_viewp->projmode==0):
**
**		Objects in the UCS (3D) (not really a projection). The llist of
**		objects will define line chains and faces in UCS coordinates.
**
**	  p_mode==1:
**
**		(p_viewp->mode&1)==0 (orthogonal, p_viewp->projmode==1):
**
**			Orthogonal projection (2D, non-perspective). The llist of
**			objects will define line chains and faces in 2D -- the
**			result of projecting the 3D objects onto a 2D plane using
**			rays perpendicular to the plane.
**
**		(p_viewp->mode&1)!=0 (perspective, p_viewp->projmode==2):
**
**			Central projection (2D, perspective). The llist of objects
**			will define line chains and faces in 2D -- the result of
**			projecting the 3D objects onto a 2D plane using rays through
**			a "camera" point.
**
**	  p_mode==2:
**
**		This is a trick to generate 3D (UCS) display objects using the
**		2D logic (efficient, non-overlapping display objects instead
**		of faces).	This was needed for dragging 3D entities, like a
**		sphere, so that the XOR painting wouldn't make the whole
**		entity invisible.  The trick is implemented by avoiding the
**		UCS to RP transformation in gr_ucs2pr(). When p_mode is 2, we
**		set p_viewp->noucs2rp to 1 and then set p_mode to 1
**		(normal projection mode).  Then, in gr_ucs2rp(),
**		if p_viewp->noucs2rp is non-0, we simply copy the source to the
**		destination instead of doing the projection. The noucs2rp flag
**		is finally zeroed again in the "out" of gr_getdispobjs(), when the
**		PRIMARY call exits.
**
**	THIS FUNCTION IS RECURSIVE.
**
**	When given an INSERT, this function undergoes recursion to process
**	the entities in the block definition.  p_viewp->idll
**	points to a llist of the INSERTs involved as recursion occurs. Each
**	recursion call in this function will add a new link to the TOP of it
**	just before the call and then remove that link after the call.	The
**	result is that when this function processes a non-INSERT entity, it
**	will be able to walk the imbedded INSERTs from the innermost to the
**	outermost and use the insertion pt, scaling, rotating, and extrusion
**	data to make the successive transformations.  Entities are broken
**	into display objects at the innermost level, and those points are
**	transformed upward through all levels until UCS coordinates or real
**	projection plane coordinates are reached.
**
**	All transformations are performed during the innermost recursion
**	call (where *p_hipp is NOT an INSERT).	Each recursion level adds its
**	contribution to the end of the display objects llist (*p_enddopp)
**	and keeps *p_enddopp updated.
**
**		WARNING: Entities other than INSERT can use recursion, too,
**		like LEADER, which creates a SOLID or INSERT for its arrow.
**		When tricking gr_getdispobjs() into generating another
**		entity's display objects by feeding it a temporary entity
**		of that type -- DO IT LAST.  Example: In gr_leader2doll(),
**		I created the line work, then the arrow (via a recursion
**		call to gr_getdispobjs()), then the negative-DIMGAP box
**		around the associated MTEXT.  The box came out filled.
**		Why?  Because, after the recursion call, the view struct
**		still had .ment pointing at the temporary SOLID (arrow).
**		(The problem is bigger than just .ment.  The .xfp[] array
**		would be wrong, too, since SOLID is ECS and LEADER is WCS,
**		and probably other view struct members would also be wrong.)
**		Therefore, when creating an entity by building other entities
**		inside of it, DO ANY RECURSION CALLS TO gr_getdispobjs() LAST.
**
**	Returns:
**		 0 : OK  (Note that *p_begdopp and *p_enddopp will be NULL
**			   if the entity is not displayable, as sometimes
**			   occurs in perspective mode.	That's not an error.
**			   The caller should check these two pointers carefully
**			   after the call.)
**		-1 : Calling error (something NULL or not set up correctly)
**		-2 : Runaway recursion (or INSERTs nested more than 100 deep).
**		-3 : No memory
*/

/*F*/
__declspec( dllexport) int
gr_getdispobjs(
	db_drawing				*p_topdp,
	db_blocktabrec			*xrefblkptr,  // name of block in which xref currently being processed, if any, lives
	db_handitem				**p_hipp,
	struct gr_displayobject	**p_begdopp,
	struct gr_displayobject	**p_enddopp,
	sds_point				*p_ucsextp,
	db_drawing				*p_dp,
	gr_view					*p_viewp,
	short					p_mode,
	CDC						*dc,
	db_viewport				*pInViewport,
	bool					bMeshAcisEntities /*= false*/)	/*D.G.*/// 'true' to get mesh surface disp. objs for ACIS entities.
{
	struct gr_displayobject *doll,*lastdo;
	db_blocktabrec *lastxrefblkptr;

	// These variables were static in an effort to reduce stack usage, but
	// when static we ran into thread contention issues.  I very carefully checked each
	// use of the variables and determined it was okay to make them automatic.	apa needed to
	// be initialized and I initialized a few others just to be sure.  No performance hit
	// was found.
	//
	short its3d,wits3d,savltsclwblk,extruok;
	int	fi1,fi2,fi3;
	int menttype,fint1,fint2;
	sds_real ar1,ar2,ar3,ar4;
	const static sds_real cap=0.015625;  /* 1/64; square polar cap. */
	sds_point entaxis[3];
	sds_point *waxis = NULL;
	sds_point *apa = NULL;
	sds_point ap1,ap2;
	struct gr_displayobject *tbegdo = NULL, *tenddo = NULL;
	db_handitem *lay0hip,*genlayhip,*bllayhip,*thip1;
	db_insdatalink *tidp1 = NULL;
	gr_stringlink *tslp1 = NULL;
	bool islwpolyline=false;
	RT	rt;
	gr_displayobject* pEntityListBegin = NULL;
	gr_displayobject* pEntityListEnd = NULL;

	CMatrix4 insertTransformation;
	insertTransformation.makeIdentity();

	// sanity checks
	ASSERT( p_hipp!=NULL );
	ASSERT( *p_hipp !=NULL );
	ASSERT( p_begdopp!=NULL );
	ASSERT(	p_enddopp!=NULL );
	ASSERT( p_dp!=NULL );
	ASSERT( p_viewp != NULL );
	if (p_viewp==NULL)
		goto out;  /* Should never */
	if (p_hipp==NULL ||
		*p_hipp==NULL ||
		p_begdopp==NULL ||
		p_enddopp==NULL ||
		p_dp==NULL)
		{
		p_viewp->gdo_rc=-1;
		p_viewp->errln=__LINE__;
		goto out;
		}

	/* Each recursion builds its own llist of display objects: */
	doll=lastdo=NULL;

	if (p_viewp->recurlevel < 1)
		{
		p_viewp->gdo_rc = p_viewp->recurlevel = 0;
		p_viewp->errln = 0L;
		p_viewp->freeInsertList();
#ifndef _BIG_TRANSFORMATION_COMPUTATION_
		p_viewp->m_transformation.makeIdentity();
		p_viewp->absxsclp = 1.0;
		p_viewp->absysclp = 1.0;
		p_viewp->absrot = 0.0;
#endif
		if (p_ucsextp == NULL)
			{
			p_viewp->ucsextmode = -1;
			p_viewp->m_ClipUcsExtMode = -1;   // Bug#78331 fix, SWH, 8/27/2003
			}
		else
			{
			   p_viewp->ucsextmode = 0;
			   memset(p_viewp->ucsext,0,sizeof(p_viewp->ucsext));

               // Bug #78331 fix, SWH, 8/27/2003
			   p_viewp->m_ClipUcsExtMode = 0;
			   memset(p_viewp->m_ClipUcsExt,0,sizeof(p_viewp->m_ClipUcsExt));
			}
		p_viewp->freeXrefPathList();
		} // end of if (p_viewp->recurlevel < 1)

	if (++p_viewp->recurlevel > 100)
		{
		// Apparently "they" don't want us to go this deep
		//
		ASSERT( false );
		p_viewp->gdo_rc=-2;
		p_viewp->errln=__LINE__;
		goto out;
		}


	// ****************************
	// HACK HACK HACK -- to fix bug 55528
	// MTEXT entities are drawn wrong if p_mode is 2.  So reset it, easiest way
	// to make this work without breaking anything else
	//
	if ( (p_mode == 2) &&
		 (((*p_hipp)->ret_type() == DB_MTEXT) ||
		  ((*p_hipp)->ret_type() == DB_DIMENSION) ) )
		{
		p_mode = 1;
		}

	// see comments in function header for meaning of p_mode == 2
	//
	if (p_mode==2)
		{
		p_mode=1;
		p_viewp->noucs2rp=1;
		}

	p_viewp->ment = *p_hipp;
	menttype = p_viewp->ment->ret_type();

	if (menttype==DB_LWPOLYLINE)	// to make things easier (I hope), consider all polylines the same
		{							// and just flag for lw
		menttype=DB_POLYLINE;
		islwpolyline=true;
		}

	/* If deleted or invisible, or it's PSPACE and we're preocessing */
	/* an xref insert (in either space), we're outta here: */
	if (p_viewp->ment->ret_deleted()   ||
		p_viewp->ment->ret_invisible() ||
		   (p_viewp->ment->ret_pspace() &&
			p_viewp->xrefpnll!=NULL))
		goto out;

	/* Set p_viewp->projmode: 0=UCS; 1=orthogonal; 2=central */
	// see notes in function header above about projmode and p_mode
	// connection
	//
	// Here's the original line before I broke it out
	//	  p_viewp->projmode = (!p_mode) ? 0 : ((p_viewp->mode&1) ? 2 : 1);
	//
	if ( p_mode == 0 || p_mode == 3)
		{
		p_viewp->projmode = 0;
		}
	else
		{
		if ( p_viewp->mode & 1 )
			{
			p_viewp->projmode = 2;
			}
		else
			{
			p_viewp->projmode = 1;
			}
		}


	/* If it's anything but an ATTRIB, clear the p_viewp->previns... stuff: */
	if(menttype != DB_ATTRIB && menttype != DB_ATTDEF)	/*D.G.*/// ATTDEF added.
	{
		if(p_viewp->previnsdata != NULL)
		{
			delete p_viewp->previnsdata;
			p_viewp->previnsdata = NULL;
		}
		p_viewp->previnsfrozen = 0;
	}

	/* Find layer 0 for the current p_dp: */
	// note gr_str_0 = "0"
	//
	lay0hip = p_dp->findtabrec(DB_LAYERTAB,gr_str_0,1);

	/*
	**	Deal with recursion for INSERT and DIMENSION.
	*/
	if (menttype==DB_INSERT ||
		menttype==DB_DIMENSION)
		{
		char *btxrefpn;
		short nRows = 1;
		short nCols = 1;
		short iRow, iCol;
		short xref;
		int btflags,added2xrefpnll,added2idll;
		db_handitem *glhip;
		sds_real rowsp,colsp;
		sds_point trueipt;
		db_blocktabrec *bthip;
		db_handitem *bdhip,*begbdhip;
		db_drawing *btxrefdp,*wdp;

		CMatrix4	fullInsertTransformation, transformation;

#ifndef _BIG_TRANSFORMATION_COMPUTATION_
		bool	bRemoveLastFilter = false;

		resbuf	rb;
		bool	bXclipframe;
		if(db_getvar("XCLIPFRAME"/*DNT*/, 0 , &rb, p_dp, NULL, NULL) != RTNORM)
		{
			p_viewp->errln = __LINE__;
			goto out;
		}
		bXclipframe = !!rb.resval.rint;

#endif

		/* Init tbegdo and tenddo to NULL; we're going to free the */
		/* llist at the end of this block if it wasn't captured: */
		tbegdo=tenddo=NULL;

		btflags=added2xrefpnll=added2idll=0;
		btxrefpn=NULL; btxrefdp=NULL;



		/* Find the layer on which this INSERT will be GENERATED: */
		thip1=p_viewp->ment->ret_layerhip();

		if ( (thip1==lay0hip) &&
			(p_viewp->idll != NULL ))
			{
			glhip = p_viewp->idll->glhip;
			}
		else
			{
			glhip = thip1;
			}

		if (glhip==NULL)
			{
			//this is a corrupt DWG - this should never happen
			ASSERT(0);
			glhip = lay0hip;
			}

		// *************************
		// VPLAYER STUFF
		//
		if ( pInViewport != NULL )
			{
			db_handitem *laycheckhip=glhip;

			if (xrefblkptr!=NULL)
				{
					GetTopLevelLayerHip(p_topdp,xrefblkptr,&laycheckhip);
				}

			if ( pInViewport->IsLayerVpLayerFrozen( (db_layertabrec *)laycheckhip ) )
				{
				p_viewp->previnsfrozen=1;
				goto out;
				}
			}
		/* If the INSERT is to be generated on a FROZEN layer */
		/* (not just off), don't gen: */
		//
		glhip->get_70(&fint1);
		if (fint1 & IC_LAYER_FROZEN)
			{
			p_viewp->previnsfrozen=1;
			goto out;
			}

		bdhip=begbdhip=NULL;

		/* Make a new TOP link for the INSERT data llist (p_viewp->idll): */
		if ((tidp1= new struct db_insdatalink )==NULL)
			{
			ASSERT( false );
			p_viewp->gdo_rc=-3;
			p_viewp->errln=__LINE__;
			goto out;
			}
		memset(tidp1,0,sizeof(struct db_insdatalink));

		tidp1->next=p_viewp->idll;
		p_viewp->idll=tidp1;
		added2idll=1;


		/* DON'T GOTO OUT UNTIL WE REMOVE IT! */


		/* Fill in the INSERT data (start with defaults): */
		p_viewp->idll->hip=p_viewp->ment;
		p_viewp->idll->axis[0][0]=p_viewp->idll->axis[1][1]=p_viewp->idll->axis[2][2]=1.0;
		p_viewp->idll->scl[0]=p_viewp->idll->scl[1]=p_viewp->idll->scl[2]=1.0;
		p_viewp->idll->layhip=p_viewp->idll->glhip=lay0hip;
		p_viewp->idll->color=DB_BLCOLOR;
		p_viewp->idll->lweight=DB_DFWEIGHT;
		p_viewp->idll->lthip=p_dp->findtabrec(DB_LTYPETAB,gr_str_bylayer,1);
		p_viewp->idll->cs=1.0;
		rowsp=colsp=0.0;
		xref=0;

		p_viewp->ment->get_210(p_viewp->idll->axis[2]);
		p_viewp->ment->get_2((db_handitem **)(&bthip));
		if (menttype==DB_INSERT)
			{
			p_viewp->ment->get_10(trueipt);
			p_viewp->ment->get_41(&p_viewp->idll->scl[0]);
			p_viewp->ment->get_42(&p_viewp->idll->scl[1]);
			p_viewp->ment->get_43(&p_viewp->idll->scl[2]);
			p_viewp->ment->get_50(&ar1);
			p_viewp->idll->cs=cos(ar1); p_viewp->idll->sn=sin(ar1);
			p_viewp->ment->get_71(&fint1);
			nRows=(short)fint1;
			if(nRows < 1)
				nRows = 1;
			p_viewp->ment->get_70(&fint1);
			nCols=(short)fint1;
			if(nCols < 1)
				nCols = 1;
			p_viewp->ment->get_45(&rowsp);
			p_viewp->ment->get_44(&colsp);

			if (icadRealEqual(p_viewp->idll->scl[0],0.0))
				p_viewp->idll->scl[0] = 1.0;
			if (icadRealEqual(p_viewp->idll->scl[1],0.0))
				p_viewp->idll->scl[1] = 1.0;
			if (icadRealEqual(p_viewp->idll->scl[2],0.0))
				p_viewp->idll->scl[2] = 1.0;
			}
		else
			{  /* DIM */
			p_viewp->ment->get_12(trueipt);
			p_viewp->ment->get_54(&ar1);
			p_viewp->idll->cs=cos(ar1);
			p_viewp->idll->sn=sin(ar1);
			}  /* End else DIMENSION */

// set the extrusion axes
		if (ic_arbaxis(p_viewp->idll->axis[2],
						p_viewp->idll->axis[0],
						p_viewp->idll->axis[1],
						p_viewp->idll->axis[2])==-1) {
			p_viewp->idll->axis[2][2]=1.0;
		}

		/* Set layer/generation-layer/color/ltype: */
		p_viewp->idll->layhip=p_viewp->ment->ret_layerhip();
		p_viewp->idll->glhip=glhip;
		p_viewp->ment->get_62(&fint1);
		p_viewp->idll->color=(short)fint1;
		p_viewp->ment->get_370(&fint1);
		p_viewp->idll->lweight=(short)fint1;
		p_viewp->idll->lthip=p_viewp->ment->ret_ltypehip();

		/* Set blkbase and begbdhip (ptr to 1st def ent), */
		/* and open the xref'd file (if this is an xref): */
		if (bthip!=NULL)
			{

			/* If it's an xref and the file has not been read in, */
			/* see if we can read it: */

			bthip->get_70(&btflags);
			bthip->get_3(&btxrefpn);
			btxrefdp=(db_drawing *)bthip->ret_xrefdp();

			/* If this is an OVERLAY xref and we're already processing */
			/* an outer xref (of either kind), bail: */
			if ((btflags & IC_BLOCK_XREF) && (btflags & IC_BLOCK_XREF_OVERLAID) &&
				p_viewp->xrefpnll!=NULL)
				{
				goto insertdone;
				}

			if ((btflags & IC_BLOCK_XREF) &&
				btxrefdp==NULL &&
				btxrefpn!=NULL &&
				*btxrefpn &&
				!bthip->ret_looked4xref())
				{
					LoadXref(p_topdp,(db_blocktabrec *)bthip,btxrefpn,p_viewp->recurlevel);
				}


			btxrefdp=(db_drawing *)bthip->ret_xrefdp();  // re-get this

			if (btxrefdp!=NULL)
				{
				char *xrefheadbuf;
				struct db_sysvarlink *svar;

				btflags |= IC_XREF_RESOLVED;  /* Resolved xref */
				bthip->set_70(btflags);

				lastxrefblkptr=xrefblkptr; // save this to restore after recursive call
				xrefblkptr=(db_blocktabrec *)bthip;

// apply layer states from top level drawing to this file
				ApplyLayerStatesToXref(p_topdp,btxrefdp,xrefblkptr);

				xrefheadbuf=btxrefdp->ret_headerbuf();

				if (xrefheadbuf!=NULL &&
					(svar=db_Getsysvar())!=NULL)
					{
					/* Looks like the block name table record for an xref */
					/* does NOT have the referenced drawing's INSBASE */
					/* in its "base" member, so get it from the drawing: */

					fi1=sizeof(sds_real);
					memcpy(p_viewp->idll->blkbase  ,xrefheadbuf+svar[DB_QINSBASE].bc,fi1);
					memcpy(p_viewp->idll->blkbase+1,xrefheadbuf+svar[DB_QINSBASE].bc+fi1,fi1);
					memcpy(p_viewp->idll->blkbase+2,xrefheadbuf+svar[DB_QINSBASE].bc+fi1+fi1,fi1);

					/* Set the "beginning block def entity pointer" to the */
					/* entll in the xref'd drawing: */
					begbdhip=btxrefdp->entnext_noxref(NULL);  /* entll */
					}

				}
			else
				{

				btflags &= ~IC_XREF_RESOLVED;  /* NOT a resolved xref */
				bthip->set_70(btflags);

				bthip->get_10(p_viewp->idll->blkbase);
				bthip->get_firstentinblock(&begbdhip);	/* entll */

				} // end of not-xref process

			}  // end of xref process

		/*
		**	We're about to do the recursion calls.	If we're doing
		**	an xref, check p_viewp->xrefpnll to see if we already
		**	xref'd this drawing in an earlier recursion.  If so, bail.
		**	If not, push a new link onto p_viewp->xrefpnll for this drawing:
		*/
		if (btxrefdp!=NULL &&
			btflags & IC_BLOCK_XREF)
			{  /* An xref (attached or overlaid) */
			char *pn;

			if ((pn=btxrefdp->ret_pn())!=NULL &&
				*pn)
				{
				/* See if we're already working on an xref of this drawing: */
				for (tslp1=p_viewp->xrefpnll; tslp1!=NULL && !strisame(tslp1->str,pn); tslp1=tslp1->next);

				// In other words, this keeps us from recursively nesting xrefs
				//
				if (tslp1!=NULL)
					{  /* Already doing it in earlier recursion. */
					goto insertdone;
					}

				/* It's a new one.	Push a link onto p_viewp->xrefpnll: */				
				if ((tslp1= new gr_stringlink )==NULL)
					{
					ASSERT( false );
					p_viewp->gdo_rc=-3;
					p_viewp->errln=__LINE__;
					goto insertdone;
					}

				if ((tslp1->str= new char [(strlen(pn)+1)])==NULL)
					{
					ASSERT( false );
					IC_FREE(tslp1);
					p_viewp->gdo_rc=-3;
					p_viewp->errln=__LINE__;
					goto insertdone;
					}
				strcpy(tslp1->str,pn);
				tslp1->next=p_viewp->xrefpnll; p_viewp->xrefpnll=tslp1;
				added2xrefpnll=1;
				} // end of if ((pn=btxrefdp....
			} // end of if (btxrefdp!=NULL

		/* Do each row and col of an MINSERT (defaults to 1 row 1 col): */
		p_viewp->idll->ipt[2]=trueipt[2];
		for(iRow = 0; iRow < nRows; ++iRow)
			{
			for (iCol = 0; iCol < nCols; ++iCol)
				{
				p_viewp->idll->ipt[0] = trueipt[0] + iCol * colsp * p_viewp->idll->cs-
												 iRow * rowsp * p_viewp->idll->sn;
				p_viewp->idll->ipt[1] = trueipt[1] + iRow * rowsp * p_viewp->idll->cs+
												 iCol * colsp * p_viewp->idll->sn;

#ifndef _BIG_TRANSFORMATION_COMPUTATION_

				transformation.makeIdentity();
				transformation(0,0) = p_viewp->idll->cs * p_viewp->idll->scl[0];
				transformation(1,0) = p_viewp->idll->sn * p_viewp->idll->scl[0];
				transformation(2,0) = 0.0;
				transformation(0,1) = -p_viewp->idll->sn * p_viewp->idll->scl[1];
				transformation(1,1) = p_viewp->idll->cs * p_viewp->idll->scl[1];
				transformation(2,1) = 0.0;
				transformation(0,2) = 0.0;
				transformation(1,2) = 0.0;
				transformation(2,2) = p_viewp->idll->scl[2];
				transformation(0,3) = p_viewp->idll->ipt[0] -
					transformation(0,0) * p_viewp->idll->blkbase[0] -
					transformation(0,1) * p_viewp->idll->blkbase[1];
				transformation(1,3) = p_viewp->idll->ipt[1] -
					transformation(1,0) * p_viewp->idll->blkbase[0] -
					transformation(1,1) * p_viewp->idll->blkbase[1];
				transformation(2,3) = p_viewp->idll->ipt[2] - p_viewp->idll->scl[2] * p_viewp->idll->blkbase[2];

				if(iCol || iRow)
					p_viewp->m_transformation = p_viewp->m_transformation * fullInsertTransformation.invert();

				fullInsertTransformation.makeIdentity();
				fullInsertTransformation(0,0) = p_viewp->idll->axis[0][0];
				fullInsertTransformation(1,0) = p_viewp->idll->axis[0][1];
				fullInsertTransformation(2,0) = p_viewp->idll->axis[0][2];
				fullInsertTransformation(0,1) = p_viewp->idll->axis[1][0];
				fullInsertTransformation(1,1) = p_viewp->idll->axis[1][1];
				fullInsertTransformation(2,1) = p_viewp->idll->axis[1][2];
				fullInsertTransformation(0,2) = p_viewp->idll->axis[2][0];
				fullInsertTransformation(1,2) = p_viewp->idll->axis[2][1];
				fullInsertTransformation(2,2) = p_viewp->idll->axis[2][2];

				fullInsertTransformation = fullInsertTransformation * transformation;
				p_viewp->m_transformation = p_viewp->m_transformation * fullInsertTransformation;
				transformation = p_viewp->m_transformation;

				p_viewp->absxsclp *= fabs(p_viewp->idll->scl[0]);
				p_viewp->absysclp *= fabs(p_viewp->idll->scl[1]);
				p_viewp->absrot += atan2(p_viewp->idll->sn, p_viewp->idll->cs);
				sds_real xScaleFactor = p_viewp->absxsclp;
				sds_real yScaleFactor = p_viewp->absysclp;
				sds_real rotationAngle = p_viewp->absrot;

				if(menttype == DB_INSERT && !iRow && !iCol)
					bRemoveLastFilter = p_viewp->m_Clipper.AddFilter(((db_insert*)p_viewp->ment)->GetFilter(p_dp), &transformation);

#endif
				/* Walk the block def and call this function recursively: */
				bdhip=begbdhip;  /* The recursion call will increment bdhip. */

				if ( btxrefdp != NULL )
					{
					wdp = btxrefdp;
					}
				else
					{
					wdp = p_dp;
					}

				while (bdhip!=NULL &&
					(xref || bdhip->ret_type()!=DB_ENDBLK) &&
					!gr_getdispobjs(p_topdp,
									xrefblkptr,
									&bdhip,
									&tbegdo,
									&tenddo,
									NULL,
									wdp,
									p_viewp,
									p_mode,
									dc,
									pInViewport,
									bMeshAcisEntities))
					{

					/* If we just processed an INSERT that found nentselp's */
					/* innerhip, leave: */
					if (p_viewp->nesp_innerhip!=NULL)
						{
						goto insertdone;
						}

#ifndef _BIG_TRANSFORMATION_COMPUTATION_
					p_viewp->m_transformation = transformation;
					p_viewp->absxsclp = xScaleFactor;
					p_viewp->absysclp = yScaleFactor;
					p_viewp->absrot = rotationAngle;
#endif
					if (tbegdo==NULL ||
						tenddo==NULL)
						{
						continue;
						}

					/* Link on the display objects from the recursion call: */
					if (doll==NULL)
						{
						doll=tbegdo;
						}
					else
						{
						lastdo->next=tbegdo;
						}
					lastdo=tenddo;
					tbegdo=tenddo=NULL;  /* Important */
					} //. end of while( bdhip!=NULL ...
				} // iCol
			} // iRow

	insertdone:

#ifndef _BIG_TRANSFORMATION_COMPUTATION_
		if(menttype == DB_INSERT && bRemoveLastFilter)
		{
			p_viewp->m_Clipper.RemoveLastFilter();
			bRemoveLastFilter = false;

			if(bXclipframe)
			{
				if(p_viewp->gdo_rc = gr_genClippingBoundary((db_entity*)*p_hipp, p_dp, p_viewp, &tbegdo, &tenddo))
					p_viewp->errln = __LINE__;
				else
					if(tbegdo && tenddo)
					{
						if(!doll)
							doll = tbegdo;
						else
							lastdo->next = tbegdo;
						lastdo = tenddo;
						tbegdo = tenddo = NULL;
					}
			}
		}
#endif

		if ((btflags & IC_BLOCK_XREF) &&
			btxrefdp!=NULL) {
			xrefblkptr=lastxrefblkptr;	 // restore this
		}

		if (tbegdo!=NULL)  /* (Can happen during an nentselp call.) */
			{
			gr_freedisplayobjectll(tbegdo);
			tbegdo=tenddo=NULL;
			}

		if (added2idll)
			{
			/*
			**	Remove the top p_viewp->idll link and save it in p_viewp->previnsdata
			**	for the benefit of future ATTRIBs (but free the old one, first):
			*/
			if (p_viewp->previnsdata!=NULL)
				{
				delete p_viewp->previnsdata;
				}
			p_viewp->previnsdata=p_viewp->idll;
			p_viewp->idll=p_viewp->idll->next;
			added2idll=0;
			}

		/* The recursion calls are done; remove top link of p_viewp->xrefpnll */
		/* (if we added to it): */
		if (added2xrefpnll &&
			p_viewp->xrefpnll!=NULL)
			{
			tslp1=p_viewp->xrefpnll->next;
			if (p_viewp->xrefpnll->str!=NULL)
				{
				delete [] p_viewp->xrefpnll->str;
				}
			delete p_viewp->xrefpnll;
			p_viewp->xrefpnll=tslp1;
			added2xrefpnll=0;
			}

		/* NOW WE CAN GOTO OUT AGAIN. */

		goto out;

		}  /* End if INSERT or DIMENSION */

	// ***********************************
	// ***********************************
	// ***********************************
	/* NOT AN INSERT IF WE GET TO HERE. */
	// ***********************************
	// ***********************************
	// ***********************************

	if (menttype==DB_POLYLINE)
		{
		if (islwpolyline) its3d=0;
		else
			{
			p_viewp->ment->get_70(&fint1);
			its3d=((fint1 & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH | IC_PLINE_3DPLINE))!=0);
		}
		}
	else
		{
		its3d=(menttype==DB_LINE   ||
			   menttype==DB_3DFACE ||
			   menttype==DB_RAY    ||
			   menttype==DB_SPLINE ||
			   menttype==DB_XLINE  ||
			   menttype==DB_LEADER ||
			   menttype==DB_IMAGE ||
			   // DP: transformations of this entities should be added in corresponding functions
				menttype==DB_MTEXT ||
				menttype==DB_TOLERANCE);
		}
	/* Note that POINT is left out; we're tricking the code into */
	/* treating POINT like a planar entity.  See gr_point2doll(). */
	/* (True of some of the R13+ "WCS planar" entities, too.) */

	/*
	**	In this block of code, do not "goto out".
	**	Use "goto laycolorlt_out" instead.
	**
	**	Thisblock contains the code for ATTRIBs that temporarily
	**	makes it look like part of the previous INSERT, for the purposes
	**	of determining generation layer, color, and linetype.
	*/
	{
		int leave,attribtrick;
		db_layertabrec *pEntLayer = NULL;

		leave=attribtrick=0;

		if (menttype == DB_ATTRIB)
		{
			/* ATTRIB associated with an INSERT whose gen layer is frozen? */
			/* DP: It's not right.
			if (p_viewp->previnsfrozen)
				{
				leave=1;
				goto laycolorlt_out;
				}
			*/
			if (p_viewp->previnsdata != NULL)
			{
				/* Push the prev INSERT's data back onto p_viewp->idll so that the */
				/* ATTRIB temporarily looks like part of the block def: */

				p_viewp->previnsdata->next = p_viewp->idll;
				p_viewp->idll = p_viewp->previnsdata;
				p_viewp->previnsdata = NULL;
				attribtrick = 1;
			}
			else
			{
				// ATTRIB associated with an INSERT whose gen layer is frozen?
				leave=1;
				goto laycolorlt_out;
			}
		}

		pEntLayer = (db_layertabrec *)p_viewp->ment->ret_layerhip();

		genlayhip = pEntLayer;
		/* Determine layer of generation: */
		for (tidp1=p_viewp->idll; genlayhip==lay0hip &&
			tidp1!=NULL; genlayhip=tidp1->layhip,tidp1=tidp1->next);

		if (genlayhip==NULL)
			{
			genlayhip=lay0hip;
			}

		/* Check for reasons NOT to gen based on the generation layer: */
		if (genlayhip!=NULL)
			{


			// *************************
			// VPLAYER STUFF
			//
			if (pInViewport != NULL)
				{
				db_handitem *laycheckhip=genlayhip;

				if (xrefblkptr!=NULL)
					{
					GetTopLevelLayerHip(p_topdp,xrefblkptr,&laycheckhip);
					}

				if ( pInViewport->IsLayerVpLayerFrozen( (db_layertabrec *)laycheckhip ) )
					{
					leave=1;
					goto laycolorlt_out;
					}
				}


			/* Off or frozen? */
			genlayhip->get_62(&fint1);
			genlayhip->get_70(&fint2);
			if (fint1<0 || // negative is off
				(fint2 & IC_LAYER_FROZEN))
				{
				leave=1;
				goto laycolorlt_out;
				}

			/* We're printing and the layer of generation is DEFPOINTS? */
/*DG - 11.6.2002*/// dxf290 (plottability flag) of layers are supported since dxf 2000.
			if(p_viewp->isprinting && ((db_layertabrec*)genlayhip)->ret_plottable() == 0 )
				{
				leave=1;
				goto laycolorlt_out;
				}
#if 0
			char *fcp1 = NULL;
			genlayhip->get_2(&fcp1);
			if ( fcp1!=NULL && strisame( fcp1, gr_str_defpoints))
				if (p_viewp->isprinting)
					{
					leave=1;
					goto laycolorlt_out;
					}
#endif
			}
		/*
		**	Determine final color, and, if we need them for this entity,
		**	determine the resolved BYBLOCK and BYLAYER colors.	This is done
		**	in a loop below, where "pass" indicates which color is being
		**	determined:
		**
		**	  0 : BYBLOCK;	1 : BYLAYER;  2 : The entity's color
		*/
		{
			short val[3];
			int pass;

			val[0]=val[1]=val[2]=7;  /* Safe default */

			for (pass=(menttype==DB_MTEXT || menttype==DB_TOLERANCE ||
				menttype==DB_LEADER || menttype==DB_MLINE ||
				menttype==DB_POLYLINE) ? 0 : 2; pass<3; pass++)
				{

				/* (POLYLINE for the sake of PFACEs. */

				bllayhip=genlayhip;  /* The BYLAYER layer */
				if (pass>1)
					{
					p_viewp->ment->get_62(&fint1);
					val[pass]=(short)fint1;
					}
				else
					{
					val[pass]=(short)((pass==0) ? DB_BBCOLOR : DB_BLCOLOR);
					}
				for (tidp1=p_viewp->idll; tidp1!=NULL && val[pass]==DB_BBCOLOR; tidp1=tidp1->next)
					{
					val[pass]=tidp1->color;
					bllayhip=tidp1->glhip;
					}

				if (val[pass]==DB_BLCOLOR &&
					bllayhip!=NULL)
					{
					bllayhip->get_62(&fint1);
					val[pass]=(short)abs(fint1);  /* Layer may be off */
												  /* (negative color)! */
					}
				if (val[pass]==DB_BLCOLOR ||
					val[pass]==DB_BBCOLOR ||
					val[pass]<1)
					{
					val[pass]=7;
					}
				} // end of for(pass=

			p_viewp->bbcolor=val[0];
			p_viewp->blcolor=val[1];
			p_viewp->color	=val[2];
		}


		/*
		**	Determine final linetype, and, if we need them for this entity,
		**	determine the resolved BYBLOCK and BYLAYER linetypes.  This is done
		**	in a loop below, where "pass" indicates which linetype is being
		**	determined.  NULL is used for CONTINUOUS.
		**
		**	  0 : BYBLOCK;	1 : BYLAYER;  2 : The entity's linetype
		*/
		if ( menttype==DB_LINE				  ||
			 menttype==DB_CIRCLE			  ||
			 menttype==DB_ARC				  ||
			(menttype==DB_POLYLINE && !its3d) ||
			 menttype==DB_RAY				  ||
			 menttype==DB_XLINE 			  ||
			 menttype==DB_MLINE 			  ||
			 menttype==DB_SPLINE			  ||
			 menttype==DB_ELLIPSE			  ||
			 menttype==DB_TOLERANCE 		  ||
			 menttype==DB_IMAGE 			  ||
			 menttype==DB_LEADER			  ||
			 menttype==DB_HATCH)	/*D.Gavrilov*/// I've added this for processing
			{						// "User-defined"-style hatches.

			int pass;
			db_handitem *val[3];

			for (pass=(menttype==DB_MTEXT || menttype==DB_TOLERANCE ||
				menttype==DB_LEADER || menttype==DB_MLINE) ? 0 : 2; pass<3; pass++)
				{
				bllayhip=genlayhip;  /* The BYLAYER layer */

				if(pass==0)
					{
					val[pass]=p_dp->findtabrec(DB_LTYPETAB,gr_str_byblock,1);
					}
				else if (pass==1)
					{
					val[pass]=p_dp->findtabrec(DB_LTYPETAB,gr_str_bylayer,1);
					}
				else
					{
					val[pass]=p_viewp->ment->ret_ltypehip();
					}

				for (tidp1=p_viewp->idll; tidp1!=NULL && val[pass]!=NULL &&
					val[pass]->ret_byblock(); tidp1=tidp1->next)
					{
					val[pass]=tidp1->lthip;
					bllayhip=tidp1->glhip;
					}
				if (val[pass]!=NULL &&
					val[pass]->ret_bylayer() &&
					bllayhip!=NULL)
					{
					val[pass]=bllayhip->ret_ltypehip();
					}
				if (val[pass]!=NULL &&
					val[pass]->ret_continuous())
					{
					val[pass]=NULL;
					}
				} // end of for (pass=...

			p_viewp->bblthip=val[0];
			p_viewp->bllthip=val[1];
			p_viewp->lthip	=val[2];

			}
		else
			{
			p_viewp->bblthip=p_viewp->bllthip=p_viewp->lthip=NULL;
			}

		// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
		/*
		**	Determine final lineweight, and, if we need them for this entity,
		**	determine the resolved BYBLOCK and BYLAYER lineweight.
		**	p_viewp->lweight will be 0-211 and DB_DFWEIGHT(-3).
		**
		*/
		{
			short val[3];
			int pass;

			val[0]=val[1]=val[2]=DB_DFWEIGHT;  /* Safe default */

			for (pass=(menttype==DB_TOLERANCE || menttype==DB_LEADER ) ? 0 : 2; pass<3; pass++)
			{

				if(pass==0)
					val[0] = DB_BBWEIGHT;
				else if(pass==1)
					val[1] = DB_BLWEIGHT;
				else{
					p_viewp->ment->get_370(&fint1);
					val[2]=(short)fint1;
				}

				bllayhip=genlayhip;  /* The BYLAYER layer */
				for (tidp1=p_viewp->idll; tidp1!=NULL && val[pass]==DB_BBWEIGHT; tidp1=tidp1->next)
					{
					val[pass]=tidp1->lweight;
					bllayhip=tidp1->glhip;
					}

				if (val[pass]==DB_BLWEIGHT &&
					bllayhip!=NULL)
					{
					bllayhip->get_370(&fint1);
					val[pass]=(short)fint1;
					}
				if( ! db_is_validweight( val[pass], DB_DFWMASK ))
					{
					val[pass] = DB_DFWEIGHT;
					}
			} // end of for(pass=

			p_viewp->bblweight=val[0];
			p_viewp->bllweight=val[1];
			p_viewp->lweight  =val[2];
		}
		// EBATECH(CNBR) ]-

	laycolorlt_out:

		if(attribtrick)
		{
			/* Take it back off again: */
			p_viewp->previnsdata=p_viewp->idll;
			p_viewp->idll=p_viewp->idll->next;
		}
		if(leave)
			goto out;
	}

	/*
	**	Set the transformation vectors for NCS to UCS.
	**
	**	Here's an idea that saves us from having to transform all object
	**	points individually: we determine the NCS origin, unit axes, and
	**	thickness vector according to the UCS.	We start with 5 points
	**	in the (non-INSERT) entity's NCS (these are p_viewp->members):
	**		.xfp[0] : Origin (0,0,0)
	**		.xfp[1] : A pt 1 unit along the X-axis from the origin (1,0,0)
	**		.xfp[2] : A pt 1 unit along the Y-axis from the origin (0,1,0)
	**		.xfp[3] : A pt 1 unit along the Z-axis from the origin (0,0,1)
	**		.xfp[4] : A pt 1 unit in the thickness direction from
	**					the origin (same as xfp[3] except for
	**					LINE.  See gr_point2doll(). ).
	**
	**	We transform these points all the way up to the UCS.
	**	Subtracting [0] from [1]-[4] gives us 4 vectors that still
	**	REPRESENT unit vectors in the NCS -- even though they may no
	**	longer BE unit vectors.  Now the X, Y, and Z coordinates of a
	**	point in the NCS can be applied as scale factors for the axis
	**	vectors .xfp[1], .xfp[2], and .xfp[3]. The scaled vectors can
	**	then be added to the origin pt (.xfp[0]) to give the transformed
	**	point.	(See gr_ncs2ucs().)  Multiplying .xfp[4] by the
	**	original thickness gives the transformed thickness offset vector
	**	in the UCS.
	**
	**	Note that for all p_viewp->projmodes, .xfp will be set up to
	**	transform points to the UCS.
	*/

	/*
	**	Get the current ent's extrusion system.
	**	If this function is treating it as 3D, set entaxis parallel
	**	to the WCS.  Otherwise, use the extrusion vector.
	**	(This will catch entities that are WCS in the database
	**	but planar in nature and that are therefore treated as ECS
	**	by this function.)
	*/
	memset(entaxis, 0, 9 * sizeof(double));
	if (its3d)
		{
		extruok=0;	/* Force default */
		}
	else
		{
		p_viewp->ment->get_210(entaxis[2]);
		extruok=1;
		}

	if (extruok)
		{
		if (icadRealEqual((ar1=GR_LEN(entaxis[2])),0.0))
			{
			extruok=0;
			}
		else
			{
			if (ar1!=1.0)
				{
				GR_UNITIZE(entaxis[2],ar1);
				}

			if (icadRealEqual(entaxis[2][2],1.0))
				{
				extruok=0;
				}
			else
				{
				if (ic_arbaxis(entaxis[2],entaxis[0],entaxis[1],entaxis[2])==-1)
					extruok=0; // shouldn't happen
				}
			}
		} // end of if (extruok)

	if (!extruok)
		{  /* DON'T MAKE THIS AN "else" TO THE ONE ABOVE! */
		ic_identity(entaxis);
		}

	p_viewp->m_thickness[0] = 0.0;
	p_viewp->m_thickness[1] = 0.0;
	p_viewp->m_thickness[2] = 1.0;
	/* LINE is special: its extrusion dir is only for thickness: */
	if (menttype==DB_LINE)
		p_viewp->ment->get_210(p_viewp->m_thickness);
	/* POINT left out because we're tricking the code into treating */
	/* it like a planar entity.  See gr_point2doll(). */

	/* Transform the p_viewp->xfp pts all the way out to the WCS, */
	/* and get the cumulative x-scale product (for circle/arc resolution): */
	insertTransformation.makeIdentity();
	insertTransformation(0,0) = entaxis[0][0];
	insertTransformation(1,0) = entaxis[0][1];
	insertTransformation(2,0) = entaxis[0][2];
	insertTransformation(0,1) = entaxis[1][0];
	insertTransformation(1,1) = entaxis[1][1];
	insertTransformation(2,1) = entaxis[1][2];
	insertTransformation(0,2) = entaxis[2][0];
	insertTransformation(1,2) = entaxis[2][1];
	insertTransformation(2,2) = entaxis[2][2];
	wits3d = its3d;
#ifdef _BIG_TRANSFORMATION_COMPUTATION_
	p_viewp->m_transformation.makeIdentity();
	tidp1 = p_viewp->idll;
	p_viewp->absxsclp = p_viewp->absysclp = 1.0;
	p_viewp->absrot = 0;
	for (;;)
	{
		/* ECS to WCS (or BCS): */
		if (!wits3d /*&& (insertTransformation(1,2) != 1.0)*/)
		{
			/* Transform p_viewp->xfp to WCS (BCS) points: */
			p_viewp->m_transformation = insertTransformation * p_viewp->m_transformation;
		} // end of if (!wits3d ...
		if (tidp1 == NULL)
			break;

		/*
		**	Transform from BCS to next INSERT's ECS.  We scale and
		**	rotate about the block base, then translate by the vector
		**	from the block base to the insertion point.  This REMOVES
		**	an occurrence of the block base from the equations below.
		**	I didn't forget to include it; it gets canceled.
		*/
 		insertTransformation.makeIdentity();
		insertTransformation(0,0) = tidp1->cs * tidp1->scl[0];
		insertTransformation(1,0) = tidp1->sn * tidp1->scl[0];
		insertTransformation(2,0) = 0.0;
		insertTransformation(0,1) = -tidp1->sn * tidp1->scl[1];
		insertTransformation(1,1) = tidp1->cs * tidp1->scl[1];
		insertTransformation(2,1) = 0.0;
		insertTransformation(0,2) = 0.0;
		insertTransformation(1,2) = 0.0;
		insertTransformation(2,2) = tidp1->scl[2];
		insertTransformation(0,3) = tidp1->ipt[0] -
			insertTransformation(0,0) * tidp1->blkbase[0] -
			insertTransformation(0,1) * tidp1->blkbase[1];
		insertTransformation(1,3) = tidp1->ipt[1] -
			insertTransformation(1,0) * tidp1->blkbase[0] -
			insertTransformation(1,1) * tidp1->blkbase[1];
		insertTransformation(2,3) = tidp1->ipt[2] - tidp1->scl[2] * tidp1->blkbase[2];

		p_viewp->absxsclp *= fabs(tidp1->scl[0]);
		p_viewp->absysclp *= fabs(tidp1->scl[1]);
		// TO-DO: figure out the absolute rotation;
		p_viewp->absrot += atan2(tidp1->sn, tidp1->cs);

		p_viewp->m_transformation = insertTransformation * p_viewp->m_transformation;
 		insertTransformation.makeIdentity();
		insertTransformation(0,0) = tidp1->axis[0][0];
		insertTransformation(1,0) = tidp1->axis[0][1];
		insertTransformation(2,0) = tidp1->axis[0][2];
		insertTransformation(0,1) = tidp1->axis[1][0];
		insertTransformation(1,1) = tidp1->axis[1][1];
		insertTransformation(2,1) = tidp1->axis[1][2];
		insertTransformation(0,2) = tidp1->axis[2][0];
		insertTransformation(1,2) = tidp1->axis[2][1];
		insertTransformation(2,2) = tidp1->axis[2][2];
		wits3d = 0;
		tidp1 = tidp1->next;
	} // end of for(;;)
#else
	if (!wits3d)
		p_viewp->m_transformation = p_viewp->m_transformation * insertTransformation;
#endif

	if (p_viewp->absxsclp < IC_ZRO)
		p_viewp->absxsclp = IC_ZRO;

	if (p_viewp->absysclp < IC_ZRO)
		p_viewp->absysclp = IC_ZRO;

	/* Now the p_viewp->xfp pts are in the WCS; transform to UCS: */
	insertTransformation.makeIdentity();
	insertTransformation(0,0) = p_viewp->ucsaxis[0][0];
	insertTransformation(1,0) = p_viewp->ucsaxis[1][0];
	insertTransformation(2,0) = p_viewp->ucsaxis[2][0];
	insertTransformation(0,1) = p_viewp->ucsaxis[0][1];
	insertTransformation(1,1) = p_viewp->ucsaxis[1][1];
	insertTransformation(2,1) = p_viewp->ucsaxis[2][1];
	insertTransformation(0,2) = p_viewp->ucsaxis[0][2];
	insertTransformation(1,2) = p_viewp->ucsaxis[1][2];
	insertTransformation(2,2) = p_viewp->ucsaxis[2][2];
	insertTransformation.multiplyRight(ap1, p_viewp->ucsorg);
	insertTransformation(0,3) = -ap1[0];
	insertTransformation(1,3) = -ap1[1];
	insertTransformation(2,3) = -ap1[2];
	p_viewp->m_transformation = insertTransformation * p_viewp->m_transformation;

	GR_PTCPY(ap1, p_viewp->m_thickness);
	p_viewp->m_transformation.multiplyRight(p_viewp->m_thickness, ap1);
	p_viewp->ment->get_39(&ar1);  /* Thickness */
	p_viewp->m_thickness[0] -= p_viewp->m_transformation(0,3);
	p_viewp->m_thickness[1] -= p_viewp->m_transformation(1,3);
	p_viewp->m_thickness[2] -= p_viewp->m_transformation(2,3);
	p_viewp->m_thickness[0] *= ar1;
	p_viewp->m_thickness[1] *= ar1;
	p_viewp->m_thickness[2] *= ar1;
	/* Find IC_ZRO times the length of the longest xfp vector (so we can */
	/* judge thickness in a relative manner instead of absolute): */
	ap1[0] = p_viewp->m_transformation(0,0);
	ap1[1] = p_viewp->m_transformation(0,1);
	ap1[2] = p_viewp->m_transformation(0,2);
	ar1 = GR_LEN(ap1);
	ap1[0] = p_viewp->m_transformation(1,0);
	ap1[1] = p_viewp->m_transformation(1,1);
	ap1[2] = p_viewp->m_transformation(1,2);
	ar2 = GR_LEN(ap1);
	if(ar1 < ar2)
		ar1 = ar2;
	ap1[0] = p_viewp->m_transformation(2,0);
	ap1[1] = p_viewp->m_transformation(2,1);
	ap1[2] = p_viewp->m_transformation(2,2);
	if(ar1 < ar2)
		ar1=ar2;
	ar1 *= IC_ZRO;

	/* See if we need to do thickness: */
	p_viewp->dothick=(GR_LEN(p_viewp->m_thickness)>ar1);
	/*
	**	If there's a thickness and we're projecting on RP and
	**	the projections of the thickness vector onto the RP X-
	**	and Y-axes are 0, then we don't need to bother with thickness:
	*/
	if (p_viewp->dothick && p_viewp->projmode &&
		 fabs(p_viewp->m_thickness[0]*p_viewp->rpaxis[0][0]+
			  p_viewp->m_thickness[1]*p_viewp->rpaxis[0][1]+
			  p_viewp->m_thickness[2]*p_viewp->rpaxis[0][2])<ar1 &&
		 fabs(p_viewp->m_thickness[0]*p_viewp->rpaxis[1][0]+
			  p_viewp->m_thickness[1]*p_viewp->rpaxis[1][1]+
			  p_viewp->m_thickness[2]*p_viewp->rpaxis[1][2])<ar1)
		p_viewp->dothick=0;
	p_viewp->dowid = 0;  /* Default; 2D POLYLINE may override. */

	/* Set p_viewp->ixfp[] to safe defaults (just in case): */
	p_viewp->m_transformationInverse.makeIdentity();

	/* Set p_viewp->ncs_ucs to take advantage of short cuts. */
	/* Remember that the xfp[] axes aren't necessarily UNIT vectors: */
	p_viewp->ncs_ucs = IC_TRANS_NO_SHORTCUTS;
#ifdef _BIG_TRANSFORMATION_COMPUTATION_
	ar1 = IC_ZRO * p_viewp->viewsize;
	if (fabs(1.0 - p_viewp->m_transformation(0,0)) < ar1 &&
		fabs(p_viewp->m_transformation(1,0)) < ar1 &&
		fabs(p_viewp->m_transformation(2,0)) < ar1 &&
		fabs(p_viewp->m_transformation(0,1)) < ar1 &&
		fabs(1.0 - p_viewp->m_transformation(1,1)) < ar1 &&
		fabs(p_viewp->m_transformation(2,1)) < ar1 &&
		fabs(p_viewp->m_transformation(0,2)) < ar1 &&
		fabs(p_viewp->m_transformation(1,2)) < ar1 &&
		fabs(1.0 - p_viewp->m_transformation(2,2)) < ar1)
	{
		if (fabs(p_viewp->m_transformation(0,3)) < ar1 &&
			fabs(p_viewp->m_transformation(1,3)) < ar1 &&
			fabs(p_viewp->m_transformation(2,3)) < ar1)
			p_viewp->ncs_ucs = IC_TRANS_NO_SCALE_ROTATE_TRANSLATE;
		else
			p_viewp->ncs_ucs = IC_TRANS_NO_SCALE_OR_ROTATE;
	}
#endif
	// ===================================================================
	// *********************** VERY BIG SWITCH ***************************
	// ===================================================================

	/*
	**	Do each entity (main ents only, minus INSERT, plus ATTRIB).
	*/
	switch (menttype)
		{  /* Main ents only (excluding INSERT). */

		case DB_LINE:

			if (p_viewp->allocatePointsChain(2))
			{
				ASSERT(false);
				p_viewp->gdo_rc = -3;
				p_viewp->errln = __LINE__;
				break;
			}
			p_viewp->ment->get_10(ap1);
			p_viewp->ment->get_11(ap2);
			if (p_viewp->ltsclwblk)
			{
				GR_PTCPY(p_viewp->pc->pt	  ,ap1);
				GR_PTCPY(p_viewp->pc->next->pt,ap2);
			}
			else
			{
				gr_ncs2ucs(ap1,p_viewp->pc->pt		,0,p_viewp);
				gr_ncs2ucs(ap2,p_viewp->pc->next->pt,0,p_viewp);
			}
			p_viewp->pc->swid = p_viewp->pc->ewid = 0.0;
			p_viewp->pc->startang = p_viewp->pc->endang = IC_ANGLE_DONTCARE;
			p_viewp->pc->next->swid = p_viewp->pc->next->ewid = 0.0;
			p_viewp->pc->next->startang = p_viewp->pc->next->endang = IC_ANGLE_DONTCARE;
			p_viewp->pc->d2go = GR_DIST(p_viewp->pc->pt,p_viewp->pc->next->pt);
			p_viewp->pc->next->d2go = -1.0;
			p_viewp->lupc = p_viewp->pc->next;
			if ((p_viewp->gdo_rc=gr_doltype( (db_entity*)p_viewp->ment, &tbegdo,&tenddo,p_viewp,p_dp,dc))!=0)
			{
				ASSERT(false);
				p_viewp->errln = __LINE__;
			}
			break; // end of DB_LINE

		case DB_POINT:
			if ((p_viewp->gdo_rc=gr_point2doll(entaxis,&tbegdo,&tenddo,p_viewp))!=0)
			{
				ASSERT( false );
				p_viewp->errln=__LINE__;
			}
			break; // end of DB_POINT

		case DB_CIRCLE:
		case DB_ARC:
			{
				int mode;
				p_viewp->ment->get_10(ap1);
				p_viewp->ment->get_40(&ar1);
				if(menttype == DB_CIRCLE)
				{
					ar2 = 0.0;
					ar3 = 0.0;
					mode = 2;
				}
				else
				{
					p_viewp->ment->get_50(&ar2);
					p_viewp->ment->get_51(&ar3);
					mode = 0;
				}
				if ((p_viewp->gdo_rc=gr_arc2pc(mode,ap1[0],ap1[1],ar1,
					ar2,ar3,ap1[2],0,0.0,0.0,
					p_viewp->ltsclwblk,
					fabs( p_viewp->GetPixelHeight() )/p_viewp->absxsclp,
					p_viewp->curvdispqual,p_viewp))!=0)
				{
					ASSERT( false );
					p_viewp->errln=__LINE__;
				}
				else
				{
					if ((p_viewp->gdo_rc=gr_doltype( (db_entity*)p_viewp->ment, &tbegdo,&tenddo,p_viewp,p_dp,dc))!=0)
					{
						ASSERT( false );
						p_viewp->errln=__LINE__;
					}
				}
				break; // end of DB_ARC, DB_CIRCLE
			}

		case DB_SHAPE:
			{
				/* Trick gr_textgen() into doing SHAPEs. */
				db_handitem *pTextStyle;
				gr_textgenparslink textInfo;
				memset(&textInfo, 0, sizeof(gr_textgenparslink));
				db_shape *pShape = (db_shape*)p_viewp->ment;
				char shape[2];
				int n;
				/* Use shape # to make a 1-char string: */
				pShape->get_2(&n);
				shape[0] = n;
				shape[1] = 0;
				textInfo.val = shape;
				/* Get the other parameters: */
				pShape->get_10(textInfo.llpt);
				pShape->get_40(&textInfo.ht);
				pShape->get_41(&textInfo.wfact);
				pShape->get_50(&textInfo.rot);
				pShape->get_51(&textInfo.obl);
				textInfo.absxsclp =p_viewp->absxsclp;
				/* Font.  (Let NULL slide; should make a question mark.): */
				pTextStyle = pShape->ret_tship();
				if(pTextStyle)
					textInfo.font = pTextStyle->ret_font();

				if((p_viewp->gdo_rc = gr_textgen(pShape, &textInfo,
					0, p_dp, p_viewp, dc, &tbegdo, &tenddo, NULL, NULL)) != 0)
					p_viewp->errln = __LINE__;
				break; // end of DB_SHAPE
			}

		case DB_TEXT:
		case DB_ATTRIB:
		case DB_ATTDEF:
			{
				int flags;
				p_viewp->ment->get_70(&flags);
				// DP: cases when we don't display ATTDEFs
				// if it's a const attdef in a block, get the value
				// if not const, don't display

				//Added Cybage AW 27/08/2001 [
				//Reason: Fix for Bug No: 77830 from BugZilla
				/*
				if(menttype == DB_ATTDEF && (p_viewp->recurlevel > 1) &&
					((p_viewp->attmode == 0) || !(flags & IC_ATTDEF_CONST)))
					break;
				*/
				if(menttype == DB_ATTDEF && (p_viewp->recurlevel > 1) &&
					((p_viewp->attmode == 0) || !(flags & IC_ATTDEF_CONST)
					|| (flags & IC_ATTRIB_INVIS)))
					break;
				//Added Cybage AW 27/08/2001 ]

				// DP: cases when we don't display ATTRIBs
				/* Skip ATTRIB if ATTMODE is 0, or if it's 1 and the ATTRIB is invisible. */
				if(menttype == DB_ATTRIB &&
					((p_viewp->attmode == 0) || ((p_viewp->attmode == 1) && (flags & IC_ATTRIB_INVIS))))
					break;

				CTextFragmentStyle textInfo;
				if(menttype == DB_ATTDEF)
				{
					if(p_viewp->recurlevel > 1)
					{
						p_viewp->ment->get_1(&textInfo.val);  // default value (THE value for constant)

						/*D.G.*/// We should fix text justification of the attdef if it has IC_ATTDEF_CONST flag.
						// (gr_fixtextjust always uses get_2 for attdefs if they are constant or not)
						//EBATECH(CNBR) -[ remove CString
						char*		tagChar;
						//CString	tagString;
						char*		tagString;
						p_viewp->ment->get_2(&tagChar);
						//tagString = tagChar;
//						tagString = strdup(tagChar);
						tagString = NULL;
						db_astrncpy(&tagString, tagChar, -1);
						if(!tagString)
						{
							ASSERT(0);
							p_viewp->errln = __LINE__;
							break;
						}
						p_viewp->ment->set_2(textInfo.val);
						gr_fixtextjust(p_viewp->ment, p_dp, p_viewp, dc, 0);
						//p_viewp->ment->set_2((char*)(LPCTSTR)tagString);
						p_viewp->ment->set_2(tagString);
						IC_FREE(tagString);
						//EBATECH(CNBR) ]-
					}
					else
					{
						p_viewp->ment->get_2(&textInfo.val);  /* Tag */
					}
				}
				else
				{
					p_viewp->ment->get_1(&textInfo.val);
				}
				if(!*textInfo.val)
					break;

				p_viewp->ment->get_10(textInfo.m_insertionPoint);
				p_viewp->ment->get_40(&textInfo.m_height);
				p_viewp->ment->get_41(&textInfo.m_widthFactor);
				p_viewp->ment->get_50(&textInfo.m_rotation);
				p_viewp->ment->get_51(&textInfo.m_obliqueAngle);
				p_viewp->ment->get_71(&textInfo.gen);

				db_handitem *pTextStyle;
				p_viewp->ment->get_7(&pTextStyle);
				if(pTextStyle)
				{
					int styleFlags = 0;
					pTextStyle->get_70(&styleFlags);
					textInfo.vert = ((styleFlags & 0x04) != 0);
					textInfo.m_pFont	= pTextStyle->ret_font();
					textInfo.bigfont = pTextStyle->ret_bigfont();
					if(icadRealEqual(textInfo.m_height,0.0))
						pTextStyle->get_40(&textInfo.m_height);
					if(icadRealEqual(textInfo.m_widthFactor,0.0))
						pTextStyle->get_41(&textInfo.m_widthFactor);
				}
				// EBATECH(CNBR) -[ prevent error
				if( textInfo.m_pFont == 0 && textInfo.bigfont == 0 ){
					ASSERT(0);
					break;
				}
				// EBATECH(CNBR) ]-
				textInfo.absxsclp = p_viewp->absxsclp;

				// Added by Denis Petrov.
				// This code generates gr_displayobject's for ATTRIBs in common case
				// when INSERT have more than one columns and rows.
				gr_displayobject* pEntityListBegin = NULL;
				gr_displayobject* pEntityListEnd = NULL;
				int nrows = 1;
				int ncols = 1;
				sds_real rowsp = 0.0;
				sds_real colsp = 0.0;
				sds_real angleSin = 0.0;
				sds_real angleCos = 0.0;
				/*DG - 11.3.2002*/// Check p_viewp->previnsdata->hip too.
				// E.g. we nullify it in osnap_helper_preprocessSS.
				if(menttype == DB_ATTRIB && p_viewp->previnsdata && p_viewp->previnsdata->hip &&
					p_viewp->previnsdata->hip->ret_type() == DB_INSERT)
				{
					p_viewp->previnsdata->hip->get_71(&nrows);
					p_viewp->previnsdata->hip->get_70(&ncols);
					p_viewp->previnsdata->hip->get_45(&rowsp);
					p_viewp->previnsdata->hip->get_44(&colsp);
					angleSin = p_viewp->previnsdata->sn;
					angleCos = p_viewp->previnsdata->cs;
				}
				int i, j;
				sds_real initialInsertPoint[2];
				initialInsertPoint[0] = textInfo.m_insertionPoint[0];
				initialInsertPoint[1] = textInfo.m_insertionPoint[1];
				for (i = 0; i < nrows; ++i)
					for (j = 0; j < ncols; ++j)
					{
						textInfo.m_insertionPoint[0] = initialInsertPoint[0] + j * colsp * angleCos -
							i * rowsp * angleSin;
						textInfo.m_insertionPoint[1] = initialInsertPoint[1] + i * rowsp * angleCos +
							j * colsp * angleSin;

						if ((p_viewp->gdo_rc = gr_textgen((db_entity *)p_viewp->ment,
							&textInfo,
							0,
							p_dp,
							p_viewp,
							dc,
							&pEntityListBegin,
							&pEntityListEnd,
							NULL,
							NULL))!=0)
						{
							p_viewp->errln=__LINE__;
							i = nrows;
							break;
						}
						/* Link on the display objects from the recursion call: */
						if(pEntityListBegin && pEntityListEnd)
						{
							if (tbegdo == NULL)
								tbegdo = pEntityListBegin;
							else
								tenddo->next = pEntityListBegin;
							tenddo = pEntityListEnd;
						}
					}
				// End of addition.
				break; // end of DB_TEXT, DB_ATTRIB, DB_ATTDEF
			}

		case DB_TRACE:
		case DB_SOLID:
		case DB_3DFACE:
		  {
			int nPoints;
			int flags = 0;
			short capped = 1;
			sds_point points[4];

			p_viewp->ment->get_10(points[0]);
			p_viewp->ment->get_11(points[1]);
			if(menttype == DB_3DFACE)
			{
				p_viewp->ment->get_12(points[2]);
				p_viewp->ment->get_13(points[3]);
				p_viewp->ment->get_70(&flags);
				capped = 0;
			}
			else
			{	/*D.G.*/// Note, the order of vertices for solids and traces.
				p_viewp->ment->get_12(points[3]);
				p_viewp->ment->get_13(points[2]);
			}

			if(icadPointEqual(points[2],points[3]))
				nPoints = 4;
			else
				nPoints = 5;
			if (p_viewp->allocatePointsChain(nPoints))
			{
				p_viewp->gdo_rc = -3;
				p_viewp->errln = __LINE__;
				break;
			}
			/* Don't bother with d2go since we're not calling gr_doltype(). */
			/* Trick gr_gensolid() into calling gr_ncs2ucs() (since */
			/* linetype is always solid for these entities). */
			/* DON'T LEAVE UNTIL THIS IS REPAIRED! */
			savltsclwblk = p_viewp->ltsclwblk;
			p_viewp->ltsclwblk = 1;
			if (menttype == DB_3DFACE && /*p_viewp->projmode &&*/ !p_viewp->splframe && flags)
			{
				/* Projected, splframe is 0, and at least 1 edge invisible. */
				/*
				**	° For now, do each side as a separate display object
				**	so that we can handle edge-visibility easily.
				**	The alternative is to figure out how many chains
				**	and how many segments in each.
				*/
				p_viewp->lupc=p_viewp->pc->next;
				p_viewp->upc=2;
				p_viewp->lupc->swid=p_viewp->lupc->ewid=
				  p_viewp->pc->swid=  p_viewp->pc->ewid=0.0;
				p_viewp->lupc->startang=p_viewp->lupc->endang=
				  p_viewp->pc->startang=  p_viewp->pc->endang=IC_ANGLE_DONTCARE;
				for (fi1=0,fi2=1,fi3=nPoints-1; fi2<=fi3; fi1=fi2,fi2++)
				{
					if (flags&(1<<fi1)) continue;  /* Invisible edge. */

					GR_PTCPY(p_viewp->pc->pt  ,points[fi1]);
					GR_PTCPY(p_viewp->lupc->pt,points[(fi2<fi3) ? fi2 : 0]);
#ifdef _DOCLASSES_
					CDoBase::get().setEntity((db_entity*)*p_hipp);
					CDoBase::get().setViewData(p_viewp);
					CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, NULL, 0);
					CDoBase::get().setFlags(0, 0);
					CDoBase::get().setDotDir(NULL);
					if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(pEntityListBegin, pEntityListEnd)) != 0)
#else
					if ((p_viewp->gdo_rc=gr_gensolid( (db_entity*)*p_hipp, p_viewp->pc,p_viewp->lupc,NULL,
						0,0,0,NULL,&pEntityListBegin,&pEntityListEnd,p_viewp))!=0)
#endif
					{
						p_viewp->errln=__LINE__;
						break;
					}
					else
					{
						if (pEntityListBegin && pEntityListEnd)
						{
							if(tbegdo == NULL)
								tbegdo = pEntityListBegin;
							else
								tenddo->next = pEntityListBegin;
							tenddo = pEntityListEnd;
						}
					}
				}
			}
			else
			{
				p_viewp->lupc = p_viewp->pc;
				for (fi1=0,fi2=nPoints-1; fi1<fi2; fi1++,
					p_viewp->lupc=p_viewp->lupc->next)
				{

					GR_PTCPY(p_viewp->lupc->pt,points[fi1]);
					p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
					p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;
				}
				GR_PTCPY(p_viewp->lupc->pt,points[0]);
				p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
				p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;

#ifdef _DOCLASSES_
				CDoBase::get().setEntity((db_entity*)*p_hipp);
				CDoBase::get().setViewData(p_viewp);
				CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, p_viewp->pc->next, nPoints - 2);
				CDoBase::get().setFlags(1, capped);
				CDoBase::get().setDotDir(NULL);
				if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(tbegdo, tenddo)) != 0)
#else
				if ((p_viewp->gdo_rc=gr_gensolid( (db_entity*)*p_hipp, p_viewp->pc,p_viewp->lupc,p_viewp->pc->next,
					nPoints-2,1,capped,NULL,&tbegdo,&tenddo,p_viewp))!=0)
#endif
					p_viewp->errln = __LINE__;
			}
			p_viewp->ltsclwblk=savltsclwblk; /* RESTORE CORRECT VAL. */
			break;
		  }

		case DB_VIEWPORT:
		  {
			int id;
			sds_real width,height;
			sds_point center;
			struct sds_resbuf rb;
			int flags = 0;

			p_viewp->ment->get_90(&flags);
			p_viewp->ment->get_10(center);
			p_viewp->ment->get_40(&width);
			p_viewp->ment->get_41(&height);
			p_viewp->ment->get_69(&id);

			if (db_getvar(NULL,DB_QTILEMODE,&rb,p_dp,NULL,NULL)!=RTNORM)
			{
				p_viewp->errln=__LINE__;
				break;
			}

			// If tilemode==1 of its the PS viewport don't draw.
			if(rb.resval.rint || id==1 || (flags&0x20000))	/*D.G.*/// (flags&0x20000) added.
				break;

			sds_point wpt[4];
			sds_real wd=width/2.0;
			sds_real ht=height/2.0;

			wpt[0][0]=center[0]-wd;
			wpt[0][1]=center[1]-ht;
			wpt[1][0]=center[0]+wd;
			wpt[1][1]=center[1]-ht;
			wpt[2][0]=center[0]+wd;
			wpt[2][1]=center[1]+ht;
			wpt[3][0]=center[0]-wd;
			wpt[3][1]=center[1]+ht;
			wpt[0][2]=wpt[1][2]=wpt[2][2]=wpt[3][2]=0.0;

			if(p_viewp->allocatePointsChain(4))
			{
				p_viewp->gdo_rc = -3;
				p_viewp->errln = __LINE__;
				break;
			}

			/* Trick gr_gensolid() into calling gr_ncs2ucs() (since */
			/* linetype is always solid for these entities). */
			/* DON'T LEAVE UNTIL THIS IS REPAIRED! */
			savltsclwblk=p_viewp->ltsclwblk;
			p_viewp->ltsclwblk=1;

			p_viewp->lupc=p_viewp->pc;
			for (fi1=0,fi2=p_viewp->upc-1; fi1<4; fi1++,
				p_viewp->lupc=p_viewp->lupc->next)
			{

				GR_PTCPY(p_viewp->lupc->pt,wpt[fi1]);
				p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
				p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;
			}
			/* Don't bother with d2go since we're not calling gr_doltype(). */

#ifdef _DOCLASSES_
			CDoBase::get().setEntity((db_entity*)*p_hipp);
			CDoBase::get().setViewData(p_viewp);
			CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, p_viewp->pc->next, p_viewp->upc - 1);
			CDoBase::get().setFlags(1, 1);
			CDoBase::get().setDotDir(NULL);
			if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(tbegdo, tenddo)) != 0)
#else
			if((p_viewp->gdo_rc=gr_gensolid( (db_entity*)*p_hipp, p_viewp->pc,p_viewp->lupc,p_viewp->pc->next,
				p_viewp->upc-1,1,1,NULL,&tbegdo,&tenddo,p_viewp)) != 0)
				p_viewp->errln=__LINE__;
#endif
			p_viewp->ltsclwblk=savltsclwblk; /* RESTORE CORRECT VAL. */
			break;
		  }

		case DB_POLYLINE:
		  {
			/*
			**	Note that for several types of POLYLINEs below (but not
			**	for ALL types), it's not convenient to step *p_hipp as we go.
			**	There's code in "out" to step *p_elp past VERTEXs and
			**	SEQENDs -- whether or not we do it here.
			*/
			// lwpolylines come in here also

			short pflags,closed,finishing,eachseg,nfv,sf,splined,doframe;
			long vidx;
			int flags, vflags,pmcnt,pncnt,vvidx[4],glcolor,npasses;
			sds_real vbulge,vswid,vewid;
			sds_point ecsendpt,pcendpt,prevpcendpt,vpt;
			db_handitem *savhip,*savlthip,*lastsplvert,*nextvert,*thip1;
			struct gr_pclink *savlupc,*tpclp1;
			sds_real pcendptstartang, pcendptendang, prevpcendptstartang,prevpcendptendang;

			sf=p_viewp->splframe;  /* For simplicity */


			p_viewp->ment->get_70(&flags);
			pflags = shortCast( flags);
			p_viewp->ment->get_71(&pmcnt);
			p_viewp->ment->get_72(&pncnt);

			if (!islwpolyline && (pflags & IC_PLINE_POLYFACEMESH))
			{  /* Polyface mesh */
				int bidx,thisidx,gen,done,pidx,pass;
				short savcolor;
				sds_point *apa;
				db_handitem *glhip;

				apa=NULL;

				/* DON'T goto out IN HERE; USE pfbreak. */

				/* Like ACAD, we'll ignore the vertex and face counts */
				/* in the POLYLINE entity and see for ourself. */

				/* Trick gr_gensolid() into calling gr_ncs2ucs() (since */
				/* linetype is always solid for pfaces): */
				savltsclwblk=p_viewp->ltsclwblk;
				p_viewp->ltsclwblk=1;

				/* Save p_viewp->color so we can restore it when we're done */
				/* (since we're going to force it for each face): */
				savcolor=p_viewp->color;

				pidx=0;  /* Safety */
				for (pass=0; pass<2; pass++)
				{
					/* 0 : Count the VERTEXs that have pts. */
					/* 1 : Alloc apa[] and capture the pts. */
					if (pass)
					{
						if (pidx<1) goto pfbreak;
						if ((apa= new sds_point [pidx])==NULL)
							{ p_viewp->gdo_rc=-3; p_viewp->errln=__LINE__; goto pfbreak; }
						memset(apa,0,sizeof(sds_point)*pidx);
					}
					for (pidx=0,thip1=(*p_hipp)->next; thip1!=NULL &&
						thip1->ret_type()==DB_VERTEX; thip1=thip1->next,pidx++)
					{
						thip1->get_70(&vflags);
						if (!(vflags & IC_VERTEX_3DMESHVERT)) break; /* 1st face def */
						if (!pass) continue;
						thip1->get_10(apa[pidx]);
					}
				}

				/* Note that thip1 now points to the 1st face-def VERTEX. */

				/* Max chain length we'll need is 5: */
				if (p_viewp->allocatePointsChain(5))
				{
					p_viewp->gdo_rc=-3;
					p_viewp->errln=__LINE__;
					goto pfbreak;
				}

				/* Set all the widths to 0.0, all the angles to DONTCARE (don't worry about d2go): */
				for (fi1=0,tpclp1=p_viewp->pc; fi1<p_viewp->upc; fi1++,
					tpclp1=tpclp1->next)
				{
					tpclp1->swid=tpclp1->ewid=0.0;
					tpclp1->startang=tpclp1->endang=IC_ANGLE_DONTCARE;
				}

				/* Do each face (each TRUE face -- 1 per data VERTEX) : */
				for (; thip1!=NULL && thip1->ret_type()==DB_VERTEX; thip1=thip1->next)
				{

					/* Get the VERTEX indices: */
					thip1->get_71(vvidx);
					thip1->get_72(vvidx+1);
					thip1->get_73(vvidx+2);
					thip1->get_74(vvidx+3);

					/* Count the # of verts in this face: */
					for (nfv=0; nfv<4 && vvidx[nfv]; nfv++);
					if (nfv<1) continue;

					/*
					**	Get the layer.	I THINK we need to do this by
					**	determining the "generation layer" in a manner
					**	similar to that for genlayhip earlier in this
					**	function.  This nested block stuff always
					**	confuses the hell out of me.  A PFACE with
					**	per-face layers is bizarre enough that I'm
					**	not going to sweat it right now.  This works
					**	for simple cases anyway:
					*/
					for (glhip=thip1->ret_layerhip(),tidp1=p_viewp->idll; glhip==
						lay0hip && tidp1!=NULL; glhip=tidp1->layhip,
						tidp1=tidp1->next);
					if (glhip==NULL) glhip=lay0hip;

					// VPLAYER ?????
					//
					/* If this face is on an OFF or FROZEN layer, skip it: */
					fint1=fint2=0;
					if (glhip!=NULL)
					{
						glhip->get_62(&glcolor);
						glhip->get_70(&fint1);
					}
					if (glcolor<0 || (fint1&1)) continue;

					/* Get this face's color and set p_viewp->color to it */
					/* for the duration of the generation of this face: */
					thip1->get_62(&fint1);
					if		(fint1==DB_BBCOLOR) fint1=p_viewp->bbcolor;
					else if (fint1==DB_BLCOLOR) {
						/* Not p_viewp->blcolor.  That would be color based */
						/* on the POLYLINE's layer.  Use the one from glhip. */

						fint1=glcolor;
					}
					p_viewp->color=(short)fint1;

					//if (p_viewp->projmode || p_mode == 3)
					if(!sf)
					{
						/* Planar; watch out for invisible edges. */
						/* Do 1 line per disp obj; not worth the effort */
						/* to do the 2-4 line cases. */

						bidx=abs(vvidx[0])-1;

						/* Since we're only doing a dot or lines, */
						/* set these now: */
						p_viewp->upc=2; p_viewp->lupc=p_viewp->pc->next;

						/* Consider each line in this face: */
						for (thisidx=done=0; !done && thisidx<4; thisidx++)
						{
						  gen=0;  /* Set to 1 when we have something to */
								  /* generate. */
						  switch (thisidx) {
							case 0:  /* Considering 0 to 1 */
							  if (!vvidx[1]) done=1;
							  if (!sf && vvidx[0]<0) break;
							  GR_PTCPY(p_viewp->pc->pt,apa[bidx]);
							  if (done) {  /* A dot */
								GR_PTCPY(p_viewp->lupc->pt,p_viewp->pc->pt);
							  } else {	/* 0-1 */
								fi1=abs(vvidx[1])-1;
								GR_PTCPY(p_viewp->lupc->pt,apa[fi1]);
							  }
							  gen=1;
							  break;
							case 1:  /* Considering 1 to 2 */
							  if (!vvidx[2]) done=1;
							  /* Special case: don't draw a line right back */
							  /* to 0: */
							  if (done || (!sf && vvidx[1]<0)) break;
							  /* 1-2: */
							  fi1=abs(vvidx[1])-1;
							  GR_PTCPY(p_viewp->pc->pt,apa[fi1]);
							  fi1=abs(vvidx[2])-1;
							  GR_PTCPY(p_viewp->lupc->pt,apa[fi1]);
							  gen=1;
							  break;
							case 2:  /* Considering 2 to 3 */
							  if (!vvidx[3]) done=1;
							  if (!sf && vvidx[2]<0) break;
							  fi1=abs(vvidx[2])-1;
							  GR_PTCPY(p_viewp->pc->pt,apa[fi1]);
							  if (vvidx[3]) {  /* 2-3 */
								fi1=abs(vvidx[3])-1;
								GR_PTCPY(p_viewp->lupc->pt,apa[fi1]);
							  } else {	/* 2-0 and done */
								GR_PTCPY(p_viewp->lupc->pt,apa[bidx]);
							  }
							  gen=1;
							  break;
							case 3:  /* Considering 3 to 0 */
							  done=1;  /* Safety */
							  if (!sf && vvidx[3]<0) break;
							  fi1=abs(vvidx[3])-1;
							  GR_PTCPY(p_viewp->pc->pt,apa[fi1]);
							  GR_PTCPY(p_viewp->lupc->pt,apa[bidx]);
							  gen=1;
							  break;
						  }
						  if (gen)
						  {
	  						// ************
							// Source Entity is the face vertex
							//
#ifdef _DOCLASSES_
							CDoBase::get().setEntity((db_entity*)thip1);
							CDoBase::get().setViewData(p_viewp);
							CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, NULL, 0);
							CDoBase::get().setFlags(0, 0);
							CDoBase::get().setDotDir(NULL);
							if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(pEntityListBegin, pEntityListEnd)) != 0)
#else
							if ((p_viewp->gdo_rc=gr_gensolid( (db_entity*)thip1, p_viewp->pc,p_viewp->lupc,
							  NULL,0,0,0,NULL,&pEntityListBegin,&pEntityListEnd,p_viewp))!=0)
#endif
								{ p_viewp->errln=__LINE__; goto pfbreak; }
							if (pEntityListBegin && pEntityListEnd)
							{
								if(tbegdo == NULL)
									tbegdo = pEntityListBegin;
								else
									tenddo->next = pEntityListBegin;
								tenddo = pEntityListEnd;
							}
						  }
						}
					}
					else
					{  /* UCS (3D).  Make faces; ignore invisibility. */
						/* Init: */
						p_viewp->upc=nfv+1;
						p_viewp->lupc=p_viewp->pc;
						/* Take the pts: */
						for (fi1=0; fi1<=nfv; fi1++)
						{
							fi2=abs(vvidx[(fi1>=nfv) ? 0 : fi1])-1;
							GR_PTCPY(p_viewp->lupc->pt,apa[fi2]);
							if (fi1<nfv) p_viewp->lupc=p_viewp->lupc->next;
						}
						/* Gen: */
						// ************
						// Source Entity is the face vertex
						//

#ifdef _DOCLASSES_
						CDoBase::get().setEntity((db_entity*)thip1);
						CDoBase::get().setViewData(p_viewp);
						CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, (nfv<2) ? NULL : p_viewp->pc->next, p_viewp->upc-2);
						CDoBase::get().setFlags((nfv>1), 0);
						CDoBase::get().setDotDir(NULL);
						if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(pEntityListBegin, pEntityListEnd)) != 0)
#else
						if ((p_viewp->gdo_rc=gr_gensolid((db_entity*)thip1,p_viewp->pc,p_viewp->lupc,
						  (nfv<2) ? NULL : p_viewp->pc->next,
						  p_viewp->upc-2,(nfv>1),0,NULL,&pEntityListBegin,&pEntityListEnd,p_viewp))!=0)
#endif
							{ p_viewp->errln=__LINE__; goto pfbreak; }
						if (pEntityListBegin && pEntityListEnd)
						{
							if(tbegdo == NULL)
								tbegdo = pEntityListBegin;
							else
								tenddo->next = pEntityListBegin;
							tenddo = pEntityListEnd;
						}
					}
				}  /* End for each face-data VERTEX */

			  pfbreak:
				p_viewp->color=savcolor;
				p_viewp->ltsclwblk=savltsclwblk;
				if (apa!=NULL) IC_FREE(apa);

			} else if (!islwpolyline && (pflags & IC_PLINE_3DMESH)) {  /* Polygon mesh */
				short mclosed,nclosed,midx,nidx;
				db_handitem **rpa;

				/* DON'T goto out IN HERE; USE pmbreak. */

				/* Trick gr_gensolid() into calling gr_ncs2ucs() (since */
				/* linetype is always solid for meshes): */
				savltsclwblk=p_viewp->ltsclwblk;
				p_viewp->ltsclwblk=1;

				rpa=NULL;

				if (pmcnt<2 || pncnt<2) goto pmbreak;

				mclosed=((pflags & IC_PLINE_CLOSED)!=0);
				nclosed=((pflags & IC_PLINE_CLOSEDN)!=0);

				/* Alloc the row ptrs (entlink where each row starts) */
				/* and init them (and check for VERTEXs as we go): */
				if ((rpa= new db_handitem *[pmcnt] )==NULL)
						{ p_viewp->gdo_rc=-3; p_viewp->errln=__LINE__; goto pmbreak; }
				memset(rpa,0,sizeof(db_handitem *)*pmcnt);
				if ((rpa[0]=(*p_hipp)->next)==NULL ||
					rpa[0]->ret_type()!=DB_VERTEX) goto pmbreak;
				for (midx=1; midx<pmcnt; midx++)
				{
					rpa[midx]=rpa[midx-1];
					for (nidx=0; nidx<pncnt; nidx++)
						if ((rpa[midx]=rpa[midx]->next)==NULL ||
							rpa[midx]->ret_type()!=DB_VERTEX) goto pmbreak;
				}
				/* Check the last row for VERTEXs: */
				for (thip1=rpa[pmcnt-1],nidx=0; thip1!=NULL &&
					nidx<pncnt; thip1=thip1->next,nidx++)
					if (thip1->ret_type()!=DB_VERTEX) break;
				if (nidx<pncnt) goto pmbreak;

				if (p_viewp->projmode)
				{  /* 2D projection. */
					/* DO EACH ROW AS 1 DISPLAY OBJECT: */

					if (p_viewp->allocatePointsChain(pncnt + nclosed))
					{
						p_viewp->gdo_rc=-3;
						p_viewp->errln=__LINE__;
						goto pmbreak;
					}
					thip1=*p_hipp;	/* The POLYLINE ent */

					for (midx=0; midx<pmcnt; midx++)
					{
						/* Start the next row: */
						p_viewp->lupc=p_viewp->pc;
						p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
						p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;
						if ((thip1=thip1->next)==NULL) goto pmbreak;

						fi1=p_viewp->upc-1;  /* idx of last pc link to fill */
						fi2=pncnt-1;  /* idx of the last col */
						for (nidx=0; nidx<=fi1; nidx++)
						{
							if (nidx>fi2) {
								GR_PTCPY(p_viewp->lupc->pt,p_viewp->pc->pt);
							} else {
								thip1->get_10(p_viewp->lupc->pt);
							}
							if (nidx<fi2 && (thip1=thip1->next)==NULL) goto pmbreak;
							if (nidx<fi1)
							{
								p_viewp->lupc=p_viewp->lupc->next;
								p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
								p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;
							}
						}

						// ******
						// TODO TODO TODO
						// Passing the polyline entity as the SourceEntity.  Don't know if this is right
						// Not sure what VERTEX to pass or where to get it
						//
#ifdef _DOCLASSES_
						CDoBase::get().setEntity((db_entity*)*p_hipp);
						CDoBase::get().setViewData(p_viewp);
						CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, p_viewp->pc->next, p_viewp->upc-2);
						CDoBase::get().setFlags(nclosed, 0);
						CDoBase::get().setDotDir(NULL);
						if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(pEntityListBegin, pEntityListEnd)) != 0)
#else
						if ((p_viewp->gdo_rc=gr_gensolid( (db_entity*)*p_hipp, p_viewp->pc,p_viewp->lupc,
							p_viewp->pc->next,p_viewp->upc-2,
							nclosed,0,NULL,&pEntityListBegin,&pEntityListEnd,p_viewp))!=0)
#endif
								{ p_viewp->errln=__LINE__; goto pmbreak; }

						if (pEntityListBegin && pEntityListEnd)
						{
							if(tbegdo == NULL)
								tbegdo = pEntityListBegin;
							else
								tenddo->next = pEntityListBegin;
							tenddo = pEntityListEnd;
						}
					}

					/* DO EACH COLUMN AS 1 DISPLAY OBJECT: */
					if (p_viewp->allocatePointsChain(pmcnt+mclosed))
					{
						p_viewp->gdo_rc=-3;
						p_viewp->errln=__LINE__;
						goto pmbreak;
					}

					fi3=pncnt-1;  /* Idx of last col */
					for (nidx=0; nidx<=fi3; nidx++)
					{
						/* Start the next col: */
						p_viewp->lupc=p_viewp->pc;
						p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
						p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;

						/* Use the row ptrs: */
						fi1=p_viewp->upc-1;  /* idx of last pc link to fill */
						fi2=pmcnt-1;  /* idx of the last row */
						for (midx=0; midx<=fi1; midx++)
						{
							rpa[(midx>fi2) ? 0 : midx]->
								get_10(p_viewp->lupc->pt);
							if (midx<fi1)
							{
								p_viewp->lupc=p_viewp->lupc->next;
								p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
								p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;
							}
						}

						// ************
						// TODO TODO TODO
						// Passing the polyline as the source entity
						// because we are doing all the vectors from one column together
						//
#ifdef _DOCLASSES_
						CDoBase::get().setEntity((db_entity*)*p_hipp);
						CDoBase::get().setViewData(p_viewp);
						CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, p_viewp->pc->next, p_viewp->upc-2);
						CDoBase::get().setFlags(mclosed, 0);
						CDoBase::get().setDotDir(NULL);
						if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(pEntityListBegin, pEntityListEnd)) != 0)
#else
						if ((p_viewp->gdo_rc=gr_gensolid( (db_entity*)*p_hipp, p_viewp->pc,p_viewp->lupc,
							p_viewp->pc->next,p_viewp->upc-2,
							mclosed,0,NULL,&pEntityListBegin,&pEntityListEnd,p_viewp))!=0)
#endif
								{ p_viewp->errln=__LINE__; goto pmbreak; }

						if (pEntityListBegin && pEntityListEnd)
						{
							if(tbegdo == NULL)
								tbegdo = pEntityListBegin;
							else
								tenddo->next = pEntityListBegin;
							tenddo = pEntityListEnd;
						}

						if (nidx<fi3)
						{  /* Step each row ptr to next col. */
							for (midx=0; midx<pmcnt; midx++)
								if ((rpa[midx]=rpa[midx]->next)==NULL)
									goto pmbreak;
						}
					}

				} else {  /* UCS (3D) */

					/* Make each FACE as 1 closed disp obj. */

					/*
					**			N
					**
					**		0  1  2  3
					**	   0*  *  *  *...
					**
					**	 M 1*  *  *  *
					**
					**	   2*  *  *  *
					**		.
					**		.
					**		.
					**
					**	Do the faces left to right, a row at a time.
					**	(starting at the upper left). THE LEFT-EDGE AND
					**	TOP-EDGE idx's WILL NEVER WRAP -- EVEN FOR CLOSED
					**	CASES.	Only the right-edge and bottom edge idx's
					**	will wrap for closed cases.
					**
					**	The variables below use Top-Bottom-Left-Right
					**	nomenclature.
					*/

					int tidx,bidx,lidx,lasttidx,lastlidx;
					db_handitem *tlhip,*trhip,*blhip,*brhip;

					if (p_viewp->allocatePointsChain(5))
					{
						p_viewp->gdo_rc = -3;
						p_viewp->errln = __LINE__;
						goto pmbreak;
					}

					/* Set all the widths to 0.0 (don't worry about d2go): */
					for (fi1=0,tpclp1=p_viewp->pc; fi1<p_viewp->upc; fi1++,
						tpclp1=tpclp1->next)
					{
						tpclp1->swid=tpclp1->ewid=0.0;
						tpclp1->startang=tpclp1->endang=IC_ANGLE_DONTCARE;
					}

					/* Get the last top and left idx's (for stepping): */
					lasttidx=pmcnt-2+mclosed;
					lastlidx=pncnt-2+nclosed;

					for (tidx=0; tidx<=lasttidx; tidx++)
					{  /* Top idx */
						if ((bidx=tidx+1)>=pmcnt) bidx=0; /* Bottom idx */
						tlhip=rpa[tidx]; trhip=tlhip->next;
						blhip=rpa[bidx]; brhip=blhip->next;
						for (lidx=0; lidx<=lastlidx; lidx++)
						{
							/* Start this face: */
							p_viewp->lupc=p_viewp->pc;

							/* Assign the pts: */
							tlhip->get_10(p_viewp->lupc->pt);
							p_viewp->lupc=p_viewp->lupc->next;
							trhip->get_10(p_viewp->lupc->pt);
							p_viewp->lupc=p_viewp->lupc->next;
							brhip->get_10(p_viewp->lupc->pt);
							p_viewp->lupc=p_viewp->lupc->next;
							blhip->get_10(p_viewp->lupc->pt);
							p_viewp->lupc=p_viewp->lupc->next;
							GR_PTCPY(p_viewp->lupc->pt,p_viewp->pc->pt);

							// ******************************
							// TODO TODO TODO
							// passing the Polyline as the source entity
							// not the vertex  would have to change generation code
							// to be able to get the vertex
							//
#ifdef _DOCLASSES_
							CDoBase::get().setEntity((db_entity*)*p_hipp);
							CDoBase::get().setViewData(p_viewp);
							CDoBase::get().setPointsData(p_viewp->pc, p_viewp->lupc, p_viewp->pc->next, p_viewp->upc-2);
							CDoBase::get().setFlags(1, 0);
							CDoBase::get().setDotDir(NULL);
							if((p_viewp->gdo_rc = CDoBase::get().getDisplayObjects(pEntityListBegin, pEntityListEnd)) != 0)
#else
							if ((p_viewp->gdo_rc=gr_gensolid( (db_entity *)*p_hipp, p_viewp->pc,p_viewp->lupc,
								p_viewp->pc->next,p_viewp->upc-2,
								1,0,NULL,&pEntityListBegin,&pEntityListEnd,p_viewp))!=0)
#endif
									{ p_viewp->errln=__LINE__; goto pmbreak; }

							if (pEntityListBegin && pEntityListEnd)
							{
								if(tbegdo == NULL)
									tbegdo = pEntityListBegin;
								else
									tenddo->next = pEntityListBegin;
								tenddo = pEntityListEnd;
							}

							if (lidx<lastlidx)
							{
								/* Step the hip's to the next face: */
								tlhip=trhip; blhip=brhip;
								if (lidx+2>=pncnt) {
									trhip=rpa[tidx]; brhip=rpa[bidx];
								} else {
									trhip=trhip->next; brhip=brhip->next;
								}
							}
						}
					}
				}

			  pmbreak:

				p_viewp->ltsclwblk=savltsclwblk;
				if (rpa!=NULL) IC_FREE(rpa);
				if (p_viewp->gdo_rc) goto out;

			}
			else 	// 2D and 3D POLYLINE, including lwpolyline
				{
				/*
				**	Strategy: The walking occurs one VERTEX at a time
				**	(not in pairs).
				**
				**	First, we make sure there's at least 1 link
				**	in p_viewp->pc and we point p_viewp->lupc at it.
				**
				**	VERTEX announcing a line seg:
				**
				**	  That point (1st pt of the seg) is captured in the
				**	  current link (p_viewp->lupc) and then p_viewp->lupc
				**	  is stepped to the next link to be filled.  The
				**	  endpoint is not taken (here).
				**
				**	VERTEX announcing an arc seg:
				**
				**	  gr_arc2pc() fills all the necessary links
				**	  beginning with p_viewp->lupc and leaves
				**	  p_viewp->lupc pointing at the last one used.
				**
				**	The last VERTEX:
				**
				**	  ecsendpt and pcendpt are set to the 1st pt
				**	  in case the POLYLINE is closed, then a flag is set
				**	  so that the line-seg and arc-seg cases finish
				**	  things properly.
				**
				**	Note that line VERTEXs do NOT "draw" their segments
				**	but they walk p_viewp->lupc to the next link in
				**	preparation for the end point (and calc .d2go by
				**	peeking at the next VERTEX).  Arc VERTEXs DO "draw"
				**	to the end of the seg -- but leave p_viewp->lupc
				**	pointing at the end of the seg. This means that BOTH
				**	types of segs start at the current p_viewp->lupc and
				**	leave p_viewp->lupc pointing at the end of the
				**	segment, so that the next segment will complete the
				**	one before it.	(Actually, the arc endpt gets
				**	redefined by the next seg, which is okay since it's
				**	the same point). The two slightly different methods
				**	here came about because gr_arc2pc() was created to
				**	draw complete arcs (for ARC and CIRCLE, too) and the
				**	line-seg procedure avoids duplicating the endpoints.
				**
				**	Be VERY careful with the calls to allocatePointsChain() here,
				**	so that p_viewp->upc remains accurate.
				*/

				db_lwpolyline *lwpp=NULL;
				if (islwpolyline)
				{
					lwpp=((db_lwpolyline *)(*p_hipp));
					if(lwpp->ret_90()<2)
					{
						p_viewp->errln=__LINE__;
						break;
					}
					splined=0;
				}
				else
				{
					/* Require at least 2 VERTEXs: */
					if (((*p_hipp)->next == NULL ||
						(*p_hipp)->next->ret_type() != DB_VERTEX ||
						(*p_hipp)->next->next == NULL ||
						(*p_hipp)->next->next->ret_type() != DB_VERTEX))
					{
						p_viewp->errln=__LINE__;
						break;
					}
					splined = ((pflags & IC_PLINE_SPLINEFIT) != 0);
				}

				closed=  (short)(pflags & IC_PLINE_CLOSED);
				eachseg=!(pflags & IC_PLINE_CONTINUELT);
				/*
				**	Now we have to think about splined polylines.
				**	For splined polylines, the VERTEXs are in the
				**	following order:  FS...SF...F.
				**	That is, the first frame VERTEX comes first,
				**	followed by all of the spline-generated VERTEXs,
				**	followed by the rest of the frame VERTEXs.
				**	(Note that the first S duplicates
				**	the first F (at least in coordinates), indicating
				**	that the S's form their own complete description
				**	of the splined polyline.)
				*/

				/* If we have a splined polyline, we need a ptr to the last */
				/* spline-created VERTEX (to tell when we're processing */
				/* the last one, and to step properly): */
				lastsplvert=NULL;
				if (splined)
					{
					for (thip1=(*p_hipp)->next; thip1!=NULL &&
						thip1->ret_type()==DB_VERTEX; thip1=thip1->next)
						{
						thip1->get_70(&vflags);
						if (vflags & IC_VERTEX_SPLINEVERTEX)
							lastsplvert=thip1;
						}
					/* If we didn't find one, or there aren't any more */
					/* VERTEXs after it, something's wrong; just draw it */
					/* as unsplined: */
					if (lastsplvert==NULL || lastsplvert->next==NULL ||
						lastsplvert->next->ret_type()!=DB_VERTEX)
							{
							lastsplvert=NULL;
							splined=0;
						}
					}

				/* Now lastsplvert is non-NULL only if splined is 1. */

				/* How many passes will we do?: */
				npasses=1+(splined && sf);

				savhip=*p_hipp;  /* Save in case we do a 2nd pass for SPLFRAME. */

				for (doframe=0; doframe<npasses; doframe++)
				{
					/*
					**	0 :
					**	  Unsplined : Do the whole thing.
					**		Splined : Do the spline-generated path.
					**
					**	1 : Do the frame (skip the spline VERTEXs).
					*/

					*p_hipp=savhip;  /* Start over for 2nd pass. */

					p_viewp->dowid=0;  /* May override below. */

					/* Start the chain (setting p_viewp->upc to 1): */
					if (p_viewp->allocatePointsChain(1))
					{
						p_viewp->gdo_rc = -3;
						p_viewp->errln = __LINE__;
						break;
					}
					p_viewp->lupc=p_viewp->pc;
					p_viewp->pc->d2go=0.0;

					pcendpt[0] = pcendpt[1] = pcendpt[2] = 0;
					pcendptstartang = 0;
					pcendptendang = 0;
					for (finishing=0,vidx=0L; ; vidx++)
					{
						// Step to the next (or first) VERTEX and find
						// the one after that:
						GR_PTCPY(prevpcendpt,pcendpt);
						prevpcendptstartang=pcendptstartang;
						prevpcendptendang  =pcendptendang;

						if (islwpolyline)
						{
							lwpp->get_10(vpt,vidx);
							lwpp->get_40(&vswid,vidx);
							lwpp->get_41(&vewid,vidx);
							lwpp->get_42(&vbulge,vidx);
							nextvert=NULL;	// just for safety's sake

							/* Set ecsendpt and pcendpt appropriately: */
							if(vidx == lwpp->ret_90() - 1)
							{
								/* Set the endpts to the 1st vert in case closed: */
								if (!doframe && splined)
									p_viewp->ment->next->next->get_10(ecsendpt);
								else
									lwpp->get_10(ecsendpt,0);
								GR_PTCPY(pcendpt,p_viewp->pc->pt);
								pcendptstartang=p_viewp->pc->startang;
								pcendptendang  =p_viewp->pc->endang;
								finishing=1;
							}
							else
							{
								lwpp->get_10(ecsendpt,vidx+1);
								if (p_viewp->ltsclwblk)
									GR_PTCPY(pcendpt,ecsendpt);
								else
									gr_ncs2ucs(ecsendpt,pcendpt,0,p_viewp);
							}
						}
						else		// lwpolyline
						{
							if (vidx)
							{  /* Not the first VERTEX */
								*p_hipp=nextvert;
								if (nextvert!=NULL)
									nextvert=nextvert->next;
							}
							else
							{  /* First VERTEX */
								if (!doframe && splined)
									*p_hipp=(*p_hipp)->next;
								*p_hipp=(*p_hipp)->next;
								nextvert=(doframe) ? lastsplvert->next : (*p_hipp)->next;
							}

							(*p_hipp)->get_10(vpt);
							(*p_hipp)->get_40(&vswid);
							(*p_hipp)->get_41(&vewid);
							(*p_hipp)->get_42(&vbulge);

							if(*p_hipp == lastsplvert || nextvert == NULL || nextvert->ret_type() != DB_VERTEX)
							{
								/* Set ecsendpt and pcendpt appropriately: */
								if (!doframe && splined)
									p_viewp->ment->next->next->get_10(ecsendpt);
								else
									p_viewp->ment->next->get_10(ecsendpt);
								GR_PTCPY(pcendpt,p_viewp->pc->pt);
								pcendptstartang=p_viewp->pc->startang;
								pcendptendang  =p_viewp->pc->endang;
								finishing=1;
							}
							else
							{
								nextvert->get_10(ecsendpt);
								if (p_viewp->ltsclwblk)
									GR_PTCPY(pcendpt,ecsendpt);
								else
									gr_ncs2ucs(ecsendpt,pcendpt,0,p_viewp);
							}

						}

						if (icadRealEqual(vbulge,0.0))  /* Line seg */
						{
							if (vidx)
							{
								GR_PTCPY(p_viewp->lupc->pt,prevpcendpt);
								p_viewp->lupc->startang=prevpcendptstartang;
								p_viewp->lupc->endang=prevpcendptendang;
							}
							else
							{
								if (p_viewp->ltsclwblk)
									GR_PTCPY(p_viewp->lupc->pt,vpt);
								else
									gr_ncs2ucs(vpt,p_viewp->lupc->pt,0,p_viewp);
							}
							p_viewp->lupc->swid=vswid;
							p_viewp->lupc->ewid=vewid;
							p_viewp->lupc->startang=
							p_viewp->lupc->endang=IC_ANGLE_DONTCARE;
							if (!doframe && !(pflags & IC_PLINE_3DPLINE) &&  /* 2D */
								(vswid>0.0 || vewid>0.0))
									p_viewp->dowid=1;

							if (!finishing || closed)
							{
								ar1=GR_DIST(p_viewp->lupc->pt,pcendpt);
								if (eachseg) p_viewp->lupc->d2go=ar1;
								else
								{
									p_viewp->pc->d2go+=ar1;
									if (vidx) p_viewp->lupc->d2go=-1.0;
								}

								/* For a line seg, jump to next pc link to fill: */
								if (p_viewp->allocatePointsChain(p_viewp->upc+1))
								{
									p_viewp->gdo_rc=-3;
									p_viewp->errln=__LINE__;
									doframe = npasses;
									break;
								}
								p_viewp->lupc=p_viewp->lupc->next;
							}

							if (finishing && closed)
							{
								GR_PTCPY(p_viewp->lupc->pt,pcendpt);
								if (vidx) p_viewp->lupc->d2go=-1.0;
// Removed the following line to fix a problem DOT linetype: the last
// dot was showing 0 width.  Not sure why I ever zeroed the widths here.
// Maybe just a precaution, but I don't want to just delete it yet.
//							  p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
								break;
							}

						}
						else  /* Arc seg */
						{
							if (finishing && !closed)  /* Just take the pt */
							{
								if (vidx)
									{
									GR_PTCPY(p_viewp->lupc->pt,prevpcendpt);
									p_viewp->lupc->startang=prevpcendptstartang;
									p_viewp->lupc->endang=prevpcendptendang;
								}
								else
								{
									if (p_viewp->ltsclwblk)
										GR_PTCPY(p_viewp->lupc->pt,vpt);
									else
										gr_ncs2ucs(vpt,p_viewp->lupc->pt,0,p_viewp);
								}
								if (vidx) p_viewp->lupc->d2go=-1.0;
// Removed the following line to fix a problem DOT linetype: the last
// dot was showing 0 width.  Not sure why I ever zeroed the widths here.
// Maybe just a precaution, but I don't want to just delete it yet.
//							  p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
								break;
							}

							savlupc=p_viewp->lupc;
							if ((p_viewp->gdo_rc=gr_arc2pc(1,vpt[0],vpt[1],
								ecsendpt[0],ecsendpt[1],vbulge,vpt[2],
								1,vswid,vewid,
								p_viewp->ltsclwblk,
								fabs(p_viewp->GetPixelHeight())/p_viewp->absxsclp,
								p_viewp->curvdispqual,p_viewp))!=0)
							{
								p_viewp->errln=__LINE__;
								doframe = npasses;
								break;
							}
							if (!doframe && !(pflags & IC_PLINE_3DPLINE) &&  /* 2D */
								(vswid>0.0 || vewid>0.0))
									p_viewp->dowid=1;
							if (!eachseg && vidx)
							{
								p_viewp->pc->d2go+=savlupc->d2go;
								savlupc->d2go=-1.0;
							}

							/* DON'T jump to next pc link to fill for arc seg. */

							/* It's up to gr_arc2pc() to adjust p_viewp->upc */
							/* (via calls to allocatePointsChain()) and p_viewp->lupc */
							/* in this "else" */

						}

						if (finishing) break;

					}  /* End for each VERTEX */

					/* If we've got a 2D POLYLINE with width, set up */
					/* p_viewp->ixfp() for the gr_ucs2ncs() calls in */
					/* gr_gensolid(): */
					if (p_viewp->dowid && gr_setupixfp(p_viewp))
						p_viewp->dowid=0;

					/*
					**	If we're doing the spline frame, save
					**	p_viewp->ltrec and set it to NULL to trick
					**	gr_doltype() into making it continuous.
					**	FIX BEFORE LEAVING!
					*/
					if (doframe)
					{
						savlthip=p_viewp->lthip;
						p_viewp->lthip=NULL;
					}

					p_viewp->gdo_rc=gr_doltype( (db_entity*)p_viewp->ment, &pEntityListBegin,&pEntityListEnd,p_viewp,p_dp,dc);

					if (doframe) p_viewp->lthip=savlthip;

					if (p_viewp->gdo_rc)
					{
						p_viewp->errln=__LINE__;
						break;
					}

					if (pEntityListBegin && pEntityListEnd)
					{
						if(tbegdo == NULL)
							tbegdo = pEntityListBegin;
						else
							tenddo->next = pEntityListBegin;
						tenddo = pEntityListEnd;
					}
				}  /* End for each doframe */
				}  /* End else 2D and 3D POLYLINE */

			break;
		  }

		case DB_RAY:
		case DB_XLINE:
		{
			sds_point ep[2],pt,vect;

			if (p_viewp->allocatePointsChain(2))
			{
				p_viewp->gdo_rc=-3;
				p_viewp->errln=__LINE__;
				break;
			}

			p_viewp->ment->get_10(pt);
			p_viewp->ment->get_11(vect);

			if (gr_rayxlinegenpts(pt,vect,menttype,
				ep[0],ep[1],p_viewp,p_dp))
				break;

			if (p_viewp->ltsclwblk)
			{
				GR_PTCPY(p_viewp->pc->pt	  ,ep[0]);
				GR_PTCPY(p_viewp->pc->next->pt,ep[1]);
			}
			else
			{
				gr_ncs2ucs(ep[0],p_viewp->pc->pt	  ,0,p_viewp);
				gr_ncs2ucs(ep[1],p_viewp->pc->next->pt,0,p_viewp);
			}
			p_viewp->pc->swid=p_viewp->pc->ewid=0.0;
			p_viewp->pc->startang=p_viewp->pc->endang=IC_ANGLE_DONTCARE;
			p_viewp->pc->next->swid=p_viewp->pc->next->ewid=0.0;
			p_viewp->pc->next->startang=p_viewp->pc->next->endang=IC_ANGLE_DONTCARE;
			p_viewp->pc->d2go=GR_DIST(p_viewp->pc->pt,p_viewp->pc->next->pt);
			p_viewp->pc->next->d2go=-1.0;
			p_viewp->lupc=p_viewp->pc->next;

			if ((p_viewp->gdo_rc=gr_doltype( (db_entity*)p_viewp->ment, &tbegdo,&tenddo,p_viewp,p_dp,dc))!=0)
				p_viewp->errln=__LINE__;
			break;
		  }

		case DB_ELLIPSE:
		  {
			/*
			**	The center pt and vector are WCS -- even though an
			**	ellipse is planar by nature and has an extrusion
			**	direction!	The code below xforms the center and the
			**	semi-major axis pt from WCS to the ECS specified by
			**	the extrusion.	The entity is then treated as
			**	an ECS entity.
			*/

			sds_real axisratio,spar,epar,newepar;
			sds_point center,smavect,extru;
			struct resbuf wcs,ecs;
			struct gr_pclink *pp;

			wcs.rbnext=ecs.rbnext=NULL;
			wcs.restype=RTSHORT; wcs.resval.rint=0;
			ecs.restype=RT3DPOINT;

			p_viewp->ment->get_10(center);
			p_viewp->ment->get_11(smavect);
			p_viewp->ment->get_40(&axisratio);
			p_viewp->ment->get_41(&spar);
			p_viewp->ment->get_42(&epar);
			p_viewp->ment->get_210(extru);

			GR_PTCPY(ecs.resval.rpoint,extru);

			/*
			**	In an entmake, ACAD accepts spar (41) as given
			**	and corrects epar (42) based on spar -- which means
			**	their definitions are order-dependent, so they
			**	can't be done by the setters.  So, THIS should be
			**	a good place to do it.	Here are the rules for
			**	setting the 42:
			**
			**	  If epar<=spar, add 2PI to it until epar>spar.
			**
			**	  If epar>spar+2PI set it to spar+2PI (full ellipse).
			**
			**	  (Actually, ACAD refuses to entmake cases where
			**	  spar==epar, but we'll draw a full ellipse
			**	  if that occurs at this point.)
			*/
			newepar=epar;
			if ((ar1=newepar-spar)>IC_TWOPI) newepar=spar+IC_TWOPI;
			else if (ar1<=0.0)
			{
				ar1/=-IC_TWOPI;  /* The number of revs by which newepar */
								 /* precedes spar (like 2.34). */
				modf(ar1,&ar1);  /* Chop off the fractional part. */

				/* Add that many full revs, plus 1 more: */
				newepar+=(ar1+1.0)*IC_TWOPI;
			}
			if (newepar!=epar)
			{  /* If we corrected it, change it in the database. */
				epar=newepar;
				p_viewp->ment->set_42(epar);
			}

			ap2[0]=center[0]+smavect[0];
			ap2[1]=center[1]+smavect[1];
			ap2[2]=center[2]+smavect[2];
			p_dp->trans(center,&wcs,&ecs,0,ap1,NULL);
			p_dp->trans(ap2,&wcs,&ecs,0,ap2,NULL);
			ap2[2]=ap1[2];	/* Precaution */
			ar1=GR_LEN(smavect)*axisratio;
			ar2=spar; ic_normang(&ar2,NULL);
			ar3=epar; ic_normang(&ar3,NULL);
			ar4=fabs(ar3-ar2);

			short is;
			fi2=ic_ellpts(ap1,ap2,ar1,ar2,ar3,1,
				(ar4<IC_ZRO || fabs(IC_TWOPI-ar4)<IC_ZRO),		/* Full ellipse */
				shortCast((p_viewp->isprinting) ? 100 : 50),	/* Segs per quadrant. */	/*D.G.*/// set 50 instead of 12 to fix CR929
				&is,&apa);
			fi1 = is;
			if (fi1<2 || apa==NULL) {

				if (fi2==-3) { p_viewp->errln=__LINE__; p_viewp->gdo_rc=-3; }
				break;
			}

			if (p_viewp->allocatePointsChain(fi1))
			{
				p_viewp->gdo_rc=-3;
				p_viewp->errln=__LINE__;
				break;
			}
			p_viewp->pc->d2go=0.0;
			fi1=0; pp=NULL; p_viewp->lupc=p_viewp->pc;
			for (;;)
			{
				if (p_viewp->ltsclwblk) {
					GR_PTCPY(p_viewp->lupc->pt,apa[fi1]);
				} else {
					gr_ncs2ucs(apa[fi1],p_viewp->lupc->pt,0,p_viewp);
				}

				p_viewp->lupc->swid=p_viewp->lupc->ewid=0.0;
				p_viewp->lupc->startang=p_viewp->lupc->endang=IC_ANGLE_DONTCARE;
				if (pp!=NULL) {
					p_viewp->pc->d2go+=GR_DIST(pp->pt,p_viewp->lupc->pt);
					p_viewp->lupc->d2go=-1.0;
				}

				if (++fi1>=p_viewp->upc)
					break;

				pp=p_viewp->lupc;
				if ((p_viewp->lupc=p_viewp->lupc->next)==NULL)
					goto out;  /* Should never. */
			}

			if ((p_viewp->gdo_rc=gr_doltype( (db_entity*)p_viewp->ment, &tbegdo,&tenddo,p_viewp,p_dp,dc))!=0)
				p_viewp->errln=__LINE__;
			break;
		  }

		case DB_MLINE:
			if ((p_viewp->gdo_rc=rt=gr_mline2doll(entaxis,&tbegdo,&tenddo,p_viewp,p_dp,dc))!=0)
				p_viewp->errln=__LINE__;
			break;

		case DB_MTEXT:
			if(p_viewp->gdo_rc = gr_mtextgen(tbegdo, tenddo, p_viewp, p_dp, dc)!= 0)
				p_viewp->errln = __LINE__;
			// EBATECH(CNBR) -[ These routine move into gr_mtextgen()
			//char*	str_in_base;									//VS
			//char*	str_bak;										//VS
			//char*	str_with_slashes;								//VS
			//
			//((CDbMText*)p_viewp->ment) -> get_1(&str_in_base);		//VS
			//if(str_in_base == NULL)
			//	break;
			//str_bak = new char[strlen(str_in_base) + 1];			//VS
			//strcpy(str_bak, str_in_base);							//VS
			//str_with_slashes = cjk_wordwrap((unsigned char*)str_bak, p_dp);
			//((CDbMText*)p_viewp->ment) -> set_1(str_with_slashes);	//VS
			//
			//if(p_viewp->gdo_rc = gr_mtextgen(tbegdo, tenddo, p_viewp, p_dp, dc)!= 0)
			//	p_viewp->errln = __LINE__;
			//
			//((CDbMText*)p_viewp->ment) -> set_1(str_bak);			//VS
			//delete str_bak; 										//VS
			//delete str_with_slashes;								//VS
			// EBATECH(CNBR) ]-
			break;

		case DB_HATCH:
			{
			gr_DisplaySurface surface(fabs(p_viewp->GetPixelHeight()) / p_viewp->absxsclp, &tbegdo, &tenddo, p_viewp);

			// for bug 1-75595
			sds_point elevpt;
			(db_hatch *) p_viewp->ment->get_10(elevpt);
			surface.m_elev = elevpt[2];

			//don't check result as previous color is better than invalid color
			surface.SetColor(p_viewp->color);
			surface.SetLWeight(p_viewp->lweight);// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight

			//gr_DisplayHatch will fill surface with the individual lines needed to fill the display objects list
			if((p_viewp->gdo_rc = gr_DisplayHatch((db_hatch *) p_viewp->ment, &surface)) != 0)
				p_viewp->errln = __LINE__;

			break;
			}

		case DB_TOLERANCE:
			{
			db_tolerance *pTolerance = (db_tolerance *)*p_hipp;
			if ((p_viewp->gdo_rc=rt=gr_tol2doll(pTolerance,
												&tbegdo,
												&tenddo,
												p_viewp,
												dc,
												p_dp))!=0)
				p_viewp->errln=__LINE__;
			break;
			}

		case DB_SPLINE:

			/* DON'T goto out IN HERE; goto splinebreak. */

			/*
			**	TODO
			**	For now, simply using whichever pts the DWG contained
			**	(10's or 11's).  Haven't figured out how to generate
			**	the set that's missing.
			**
			**	OK.  Here we go.  Let's cloud the issue.
			**	APPARENTLY, closed SPLINEs are always scenario 1
			**	and are EXPLICITLY closed (1st and last 10-pt are
			**	coincident).  However, I'm not SURE about this, and I
			**	went to a lot of trouble to write the closing logic.
			**	SO...I'm going to make it LOOK like closed SPLINEs
			**	are open: like there's no such thing as an implicitly
			**	closed one.  This preserves the implicit closing-logic code
			**	in case we need to go back to it.  The trick is done by
			**	simply setting closed=0 -- no matter what the flags
			**	indicate.
			*/
		  {
			int closed,pidx,eidx,ndest,genret,nsour,lntrick,savflags,fint1;
			sds_real resocoeff,*thispt,ar1;
			sds_point *dest,*sourarray,ap[2];
			struct gr_pclink *prevpcp,*thispcp;

			int sp_scenario,sp_flags,sp_deg,sp_nknots,sp_ncpts,sp_nfpts;
			sds_point *sp_cpt,*sp_fpt,sp_tandir[2];
			sds_real *sp_wt,*sp_knot,sp_ktol,sp_ctol,sp_ftol;

			#define RESOFACT  1.0  /* Master resocoeff factor.	(C in the */
								   /*	equations discussed below.) */
			#define D2VS	 10.0  /* Max extent-Diagonal to ViewSize ratio */
								   /*	allowed before we define a fake PixelHeight */
								   /*	to determine a reasonable resocoeff. */

			/* Set defaults (in case something goes wrong reading the data): */
			sp_flags=sp_nknots=sp_ncpts=sp_nfpts=0;
			sp_deg=3;
			sp_cpt=sp_fpt=NULL;
			/* Set the tandirs to (0,0,0) (undefined): */
			sp_tandir[0][0]=sp_tandir[0][1]=sp_tandir[0][2]=
				sp_tandir[1][0]=sp_tandir[1][1]=sp_tandir[1][2]=0.0;
			sp_wt=sp_knot=NULL;
			sp_ktol=sp_ctol=1.0E-7; sp_ftol=0.0;

			ndest=0; dest=NULL; lntrick=savflags=0;

			/*
			**	Prepare resolution coefficient.
			**
			**	Our arc resolution is set using the equation
			**	n=0.3222*sqrt(c*r), where n is the number of segments
			**	in a full circle whose radius in PIXELS is r and c is
			**	curvdispqual (circle zoom percent).  (See gr_arc2pc()
			**	for details.)  For SPLINE span resolution, we're going
			**	to try something similar: a constant (C) times the
			**	square root of the distance between the two points (in
			**	pixels):
			**
			**		n=C*sqrt(dist_in_pixels);
			**
			**	But dist_in_pixels=dist_in_NCS*p_viewp->absxsclp/p_viewp->GetPixelHeight();
			**
			**	So, we can define a new constant for the current
			**	situation:
			**
			**		resocoeff=C*sqrt(p_viewp->absxsclp/p_viewp->GetPixelHeight());
			**
			**	Then we have
			**
			**		n=resocoeff*sqrt(dist_in_NCS);
			**
			**	(Note that since we're using RESOFACT==1, resocoeff is the
			**	number of pixels per drawing unit, or the inverse of
			**	the size of a pixel in drawing units.)
			**
			**	For now, for SPLINEs, we're not going to complicate
			**	the issue with curvdispqual (circle zoom percent).
			*/
			resocoeff=RESOFACT*sqrt(fabs(p_viewp->absxsclp * p_viewp->GetReciprocalOfPixelHeight()));

			closed=0;  /* Instead of "((sp_flags&'\x01')!=0);".  See note above. */

			/* Get data needed by both scenarios: */
			sp_scenario=p_viewp->ment->ret_scenario();
			p_viewp->ment->get_70(&sp_flags);
			p_viewp->ment->get_71(&sp_deg);

			/* Get the data specific to each scenario and call */
			/* the appropriate spline generator: */
			if (sp_scenario==1)
			{
				p_viewp->ment->get_73(&sp_ncpts);
				if (sp_ncpts<2) { p_viewp->errln=__LINE__; goto splinebreak; }
				p_viewp->ment->get_43(&sp_ctol);
				if ((sp_cpt= new sds_point [sp_ncpts] )==NULL) { p_viewp->gdo_rc=-3; p_viewp->errln=__LINE__; goto splinebreak; }
				memset(sp_cpt,0,sizeof(sds_point)*sp_ncpts);
				if (sp_ncpts>0 && (sp_wt= new sds_real [sp_ncpts])==NULL) { p_viewp->gdo_rc=-3; p_viewp->errln=__LINE__; goto splinebreak; }
				memset(sp_wt,0,sizeof(sds_real)*sp_ncpts);
				p_viewp->ment->get_10(sp_cpt ,sp_ncpts);
				p_viewp->ment->get_41array(sp_wt  ,sp_ncpts);
				for (fint1=0; fint1<sp_ncpts; fint1++)
					if (sp_wt[fint1]<=0.0) sp_wt[fint1]=1.0;

				p_viewp->ment->get_42(&sp_ktol);
				p_viewp->ment->get_72(&sp_nknots);
				if (sp_nknots>0 && (sp_knot= new sds_real [sp_nknots] )==NULL) { p_viewp->gdo_rc=-3; p_viewp->errln=__LINE__; goto splinebreak; }
				memset(sp_knot,0,sizeof(sds_real)*sp_nknots);
				p_viewp->ment->get_40array(sp_knot,sp_nknots);

				{  /* Check extent and keep resocoeff from being too large: */

					/* Find the extent: */
					GR_PTCPY(ap[0],sp_cpt[0]); GR_PTCPY(ap[1],sp_cpt[0]);
					for (fi1=1; fi1<sp_ncpts; fi1++) {
						if		(ap[0][0]>sp_cpt[fi1][0]) ap[0][0]=sp_cpt[fi1][0];
						else if (ap[1][0]<sp_cpt[fi1][0]) ap[1][0]=sp_cpt[fi1][0];
						if		(ap[0][1]>sp_cpt[fi1][1]) ap[0][1]=sp_cpt[fi1][1];
						else if (ap[1][1]<sp_cpt[fi1][1]) ap[1][1]=sp_cpt[fi1][1];
						if		(ap[0][2]>sp_cpt[fi1][2]) ap[0][2]=sp_cpt[fi1][2];
						else if (ap[1][2]<sp_cpt[fi1][2]) ap[1][2]=sp_cpt[fi1][2];
					}
					/* Get the diagonal length of the extent: */
					ap[1][0]-=ap[0][0]; ap[1][1]-=ap[0][1]; ap[1][2]-=ap[0][2];
					ar1=sqrt(ap[1][0]*ap[1][0]+ap[1][1]*ap[1][1]+ap[1][2]*ap[1][2]);

					if (ar1/p_viewp->viewsize>D2VS)
					{
						ar1/=D2VS;	/* Fake viewsize -- smallest we'll allow */
						ar1*=p_viewp->GetPixelHeight()/p_viewp->viewsize;  /* Fake PixelHeight -- smallest we'll allow */
						resocoeff=RESOFACT*sqrt(fabs(p_viewp->absxsclp/
							((icadRealEqual(ar1,0.0)) ? 1.0 : ar1)));  /* Largest resocoeff we'll allow */
					}

				}

				/*
				**	If we have a 3-pt SPLINE that's closed and
				**	periodic, let's lie to splinegen and tell it we
				**	only have 2 pts (and that it's not closed and not
				**	periodic), so it will generate a line (rather than
				**	generate nothing and return an error code).
				**
				**	It's worse than that.  We have to lie to US, too,
				**	about the flags value so that doltype()/gensolid()
				**	will work.	They check the flags.  So -- we temporarily
				**	change the flags in the database.
				**
				**	(THIS CODE IS IN 2 PLACES -- splinegen1 AND 2 --
				**	USING THE CORRECT POINTS FOR EACH CASE.)
				*/
				if (sp_ncpts==3 && sp_flags&2 && icadPointEqual(sp_cpt[2],sp_cpt[0]))
				{

					lntrick=1;
					savflags=sp_flags; sp_flags&=~3;
					p_viewp->ment->set_70(sp_flags);
					sp_ncpts=2;
				}

				genret=gr_splinegen1(
					sp_deg,
					sp_flags,
					sp_nknots,
					sp_knot,
					sp_ktol,
					sp_ncpts,
					sp_cpt,
					sp_wt,
					sp_ctol,
					resocoeff,
					&ndest,
					&dest);
			}
			else
			{
				/* In this scenario, note that the control points */
				/* and knots may be allocated by the spline generator. */

				p_viewp->ment->get_74(&sp_nfpts);
				if (sp_nfpts<2) { p_viewp->errln=__LINE__; goto splinebreak; }
				p_viewp->ment->get_44(&sp_ftol);
				if (sp_nfpts>0 && (sp_fpt= new sds_point [sp_nfpts])==NULL) { p_viewp->gdo_rc=-3; p_viewp->errln=__LINE__; goto splinebreak; }
				memset(sp_fpt,0,sizeof(sds_point)*sp_nfpts);
				p_viewp->ment->get_11(sp_fpt ,sp_nfpts);

				p_viewp->ment->get_12(sp_tandir[0]);
				p_viewp->ment->get_13(sp_tandir[1]);

				{  /* Check extent and keep resocoeff from being too large: */

					/* Find the extent: */
					GR_PTCPY(ap[0],sp_fpt[0]); GR_PTCPY(ap[1],sp_fpt[0]);
					for (fi1=1; fi1<sp_nfpts; fi1++) {
						if		(ap[0][0]>sp_fpt[fi1][0]) ap[0][0]=sp_fpt[fi1][0];
						else if (ap[1][0]<sp_fpt[fi1][0]) ap[1][0]=sp_fpt[fi1][0];
						if		(ap[0][1]>sp_fpt[fi1][1]) ap[0][1]=sp_fpt[fi1][1];
						else if (ap[1][1]<sp_fpt[fi1][1]) ap[1][1]=sp_fpt[fi1][1];
						if		(ap[0][2]>sp_fpt[fi1][2]) ap[0][2]=sp_fpt[fi1][2];
						else if (ap[1][2]<sp_fpt[fi1][2]) ap[1][2]=sp_fpt[fi1][2];
					}
					/* Get the diagonal length of the extent: */
					ap[1][0]-=ap[0][0]; ap[1][1]-=ap[0][1]; ap[1][2]-=ap[0][2];
					ar1=sqrt(ap[1][0]*ap[1][0]+ap[1][1]*ap[1][1]+ap[1][2]*ap[1][2]);

					if (ar1/p_viewp->viewsize>D2VS) {
						ar1/=D2VS;	/* Fake viewsize -- smallest we'll allow */
						ar1*=p_viewp->GetPixelHeight()/p_viewp->viewsize;  /* Fake PixelHeight -- smallest we'll allow */
						resocoeff=RESOFACT*sqrt(fabs(p_viewp->absxsclp/
							((icadRealEqual(ar1,0.0)) ? 1.0 : ar1)));  /* Largest resocoeff we'll allow */
					}

				}

				/*
				**	If we have a 3-pt SPLINE that's closed and
				**	periodic, let's lie to splinegen and tell it we
				**	only have 2 pts (and that it's not closed and not
				**	periodic), so it will generate a line (rather than
				**	generate nothing and return an error code).
				**
				**	It's worse than that.  We have to lie to US, too,
				**	about the flags value so that doltype()/gensolid()
				**	will work.	They check the flags.  So -- we temporarily
				**	change the flags in the database.
				**
				**	(THIS CODE IS IN 2 PLACES -- splinegen1 AND 2 --
				**	USING THE CORRECT POINTS FOR EACH CASE.)
				*/
				if (sp_nfpts==3 && sp_flags&2 && icadPointEqual(sp_fpt[2],sp_fpt[0])) {

					lntrick=1;
					savflags=sp_flags; sp_flags&=~3;
					p_viewp->ment->set_70(sp_flags);
					sp_nfpts=2;
				}

				genret=gr_splinegen2(
					sp_deg,
					sp_nfpts,
					sp_fpt,
					sp_ftol,
					sp_tandir[0],
					sp_tandir[1],
					sp_flags & 2,  /* don't mess with the '& 2'. */
					resocoeff,
					&ndest,
					&dest,
					&sp_ncpts,
					&sp_cpt,
					&sp_nknots,
					&sp_knot);

				// update NURBS definition data of the curve - SWH
				if (!genret) { 
					db_spline *pSpline = (db_spline *)*p_hipp;
					pSpline->setDefinitionData(sp_cpt, sp_ncpts, sp_knot, sp_nknots);
				}
			}
			if (!genret && !p_viewp->gdo_rc && ndest>1) {
				/* Use dest to gen disp objects: */
				if (p_viewp->allocatePointsChain(ndest))
				{
					p_viewp->gdo_rc=-3;
					p_viewp->errln=__LINE__;
					goto splinebreak;
				}
				for (pidx=0,prevpcp=NULL,thispcp=p_viewp->pc; pidx<
					ndest; pidx++,prevpcp=thispcp,thispcp=thispcp->next) {

					if (p_viewp->ltsclwblk) GR_PTCPY(thispcp->pt,dest[pidx]);
					else gr_ncs2ucs(dest[pidx],thispcp->pt,0,p_viewp);
					thispcp->swid=thispcp->ewid=0.0;
					thispcp->startang=thispcp->endang=IC_ANGLE_DONTCARE;
					if (prevpcp==NULL) p_viewp->pc->d2go=0.0;
					else {
						p_viewp->pc->d2go+=GR_DIST(prevpcp->pt,thispcp->pt);
						thispcp->d2go=-1.0;
					}
					p_viewp->lupc=thispcp;
				}

				if ((p_viewp->gdo_rc=gr_doltype( (db_entity*)p_viewp->ment, &pEntityListBegin,&pEntityListEnd,p_viewp,p_dp,dc))!=0)
					{ p_viewp->errln=__LINE__; goto splinebreak; }

				if (pEntityListBegin && pEntityListEnd)
				{
					if(tbegdo == NULL)
						tbegdo = pEntityListBegin;
					else
						tenddo->next = pEntityListBegin;
					tenddo = pEntityListEnd;
				}
			}

			/*
			**	If SPLFRAME is non-zero, tack on the spline frame
			**	disp objects.
			*/
			if (p_viewp->splframe) {

				if (sp_scenario==1) { nsour=sp_ncpts; sourarray=sp_cpt; }
				else				{ nsour=sp_nfpts; sourarray=sp_fpt; }

				if (nsour<2 || sourarray==NULL) goto splinebreak;

				if (p_viewp->allocatePointsChain(nsour))
				{
					p_viewp->gdo_rc=-3;
					p_viewp->errln=__LINE__;
					goto splinebreak;
				}
				p_viewp->pc->d2go=0.0;
				for (prevpcp=NULL,thispcp=p_viewp->pc,pidx=0,
					eidx=nsour-1; pidx<=eidx; prevpcp=thispcp,
					thispcp=thispcp->next,pidx++) {

					thispt=sourarray[(pidx<nsour) ? pidx : 0];

					if (p_viewp->ltsclwblk) GR_PTCPY(thispcp->pt,thispt);
					else gr_ncs2ucs(thispt,thispcp->pt,0,p_viewp);
					thispcp->swid=thispcp->ewid=0.0;
					thispcp->startang=thispcp->endang=IC_ANGLE_DONTCARE;
					if (prevpcp!=NULL) {
						p_viewp->pc->d2go+=GR_DIST(prevpcp->pt,thispcp->pt);
						thispcp->d2go=-1.0;
					}
					p_viewp->lupc=thispcp;
				}

				if ((p_viewp->gdo_rc=gr_doltype( (db_entity*)p_viewp->ment, &pEntityListBegin,&pEntityListEnd,p_viewp,p_dp,dc))!=0)
					{ p_viewp->errln=__LINE__; goto splinebreak; }

				if (pEntityListBegin && pEntityListEnd)
				{
					if(tbegdo == NULL)
						tbegdo = pEntityListBegin;
					else
						tenddo->next = pEntityListBegin;
					tenddo = pEntityListEnd;
				}
			}

		  splinebreak:

			if (lntrick) p_viewp->ment->set_70(savflags);

			if (dest!=NULL) IC_FREE(dest);

			if (sp_cpt !=NULL) IC_FREE(sp_cpt);
			if (sp_fpt !=NULL) IC_FREE(sp_fpt);
			if (sp_wt  !=NULL) IC_FREE(sp_wt);
			if (sp_knot!=NULL) IC_FREE(sp_knot);

			#undef RESOFACT
			#undef D2VS
			break;
		 }

		case DB_LEADER:
		  {
			/*
			**	WARNING:
			**
			**	This one's unusual in that it's disp objs are partially
			**	generated by calling gr_getdispobjs() again with
			**	a "fake" entity (a SOLID or INSERT for the arrow).
			**	That messes up the data that an nentselp call needs;
			**	the data must be for the LEADER -- not the SOLID
			**	or INSERT.	Therefore -- for this entity, when we're doing
			**	an nentselp -- we have to do some nasty tricks:
			*/
			gr_selectorlink *savsl;
			db_handitem *savment;
			sds_point saventaxis[sizeof(entaxis)/sizeof(entaxis[0])];
			CMatrix4 transformation(p_viewp->m_transformation);

			savsl=NULL;
			if (p_viewp->nesp_sl!=NULL) {
				/* Save the things nentselp needs.	(p_viewp->idll should be okay; */
				/* an INSERT will add to it, but it will be restored afterward.) */
				savsl=p_viewp->nesp_sl;
				savment=p_viewp->ment;
				memcpy(saventaxis,entaxis,sizeof(entaxis));

				/* Shut off nentselp mode temporarily: */
				p_viewp->nesp_sl=NULL;
			}

			if ((p_viewp->gdo_rc=rt=gr_leader2doll(&tbegdo,&tenddo,p_mode,p_viewp,p_dp,dc))!=0)
				{ p_viewp->errln=__LINE__; break; }

			if (savsl!=NULL) {	/* Restore everything. */
				p_viewp->nesp_sl=savsl;
				p_viewp->ment=savment;
				memcpy(entaxis,saventaxis,sizeof(entaxis));
				p_viewp->m_transformation = transformation;
			}

			break;
		  }

		case DB_IMAGE:
			/*
			**	Disp objs for this one are for the surrounding rectangle.
			**	(Actually, it should probably be for the clip edges --
			**	but we aren't clipping yet anyway.)
			**
			**	The usual entity color and linetype apply.
			*/
			if(p_viewp->allocatePointsChain(5))
			{
				p_viewp->gdo_rc = -3;
				p_viewp->errln = __LINE__;
				break;
			}
			tbegdo = gr_displayobject::newDisplayObject((db_entity*)p_viewp->ment);
			tbegdo->type = DISP_OBJ_PTS_3D | DISP_OBJ_SPECIAL;
			tbegdo->npts = 4;			
			tbegdo->chain = new sds_real[tbegdo->npts * 3];

			sds_point ipt, offset[2], pix;
			p_viewp->ment->get_10(ipt);
			p_viewp->ment->get_11(offset[0]);  /* For 1 pixel */
			p_viewp->ment->get_12(offset[1]);  /* For 1 pixel */
			p_viewp->ment->get_13(pix);

			/* Adjust offset[0] and offset[1] so that they are for */
			/* the whole image: */
			offset[0][0] *= pix[0];
			offset[0][1] *= pix[0];
			offset[0][2] *= pix[0];
			offset[1][0] *= pix[1];
			offset[1][1] *= pix[1];
			offset[1][2] *= pix[1];
			for (fi1 = 0, p_viewp->lupc = p_viewp->pc; fi1 < 4; fi1++, p_viewp->lupc = p_viewp->lupc->next)
			{
				p_viewp->lupc->swid = p_viewp->lupc->ewid = 0.0;
				p_viewp->lupc->startang = p_viewp->lupc->endang = IC_ANGLE_DONTCARE;

				GR_PTCPY(ap1, ipt);
				if (fi1 == 1 || fi1 == 2)
				{
					ap1[0] += offset[0][0];
					ap1[1] += offset[0][1];
					ap1[2] += offset[0][2];
				}
				if (fi1 == 2 || fi1 == 3)
				{
					ap1[0] += offset[1][0];
					ap1[1] += offset[1][1];
					ap1[2] += offset[1][2];
				}

				if (p_viewp->ltsclwblk)
					GR_PTCPY(p_viewp->lupc->pt, ap1);
				else
					gr_ncs2ucs(ap1,p_viewp->lupc->pt, 0, p_viewp);
				memcpy(&tbegdo->chain[3 * fi1], p_viewp->lupc->pt, 3 * sizeof(sds_real));
			}
			p_viewp->lupc->swid = p_viewp->lupc->ewid = 0.0;
			p_viewp->lupc->startang = p_viewp->lupc->endang = 0.0;
			p_viewp->lupc->d2go = -1.0;
			GR_PTCPY(p_viewp->lupc->pt, p_viewp->pc->pt);

			/* There are only 4; just set the d2go values wihout a loop: */
			p_viewp->pc->d2go = p_viewp->pc->next->next->d2go = GR_DIST(p_viewp->pc->pt, p_viewp->pc->next->pt);
			p_viewp->pc->next->d2go = p_viewp->pc->next->next->next->d2go = GR_DIST(p_viewp->pc->next->pt, p_viewp->pc->next->next->pt);

			if ((p_viewp->gdo_rc = gr_doltype((db_entity*)p_viewp->ment, &tbegdo->next, &tenddo, p_viewp, p_dp, dc)) != 0)
				p_viewp->errln=__LINE__;
			tbegdo->color = tbegdo->next->color;
			tbegdo->lweight = tbegdo->next->lweight;	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
			break;

		case DB_DIMENSION: break;  /* Handled as an INSERT */

#ifdef BUILD_WITH_ACIS_SUPPORT // reqired for non-ACIS builds. Otherwise, non-ACIS builds will not work properly
		case DB_REGION	 :
		case DB_3DSOLID  :
		case DB_BODY	 :
				if(!gr_acisGetDispObjs((db_entity*)*p_hipp, p_dp, p_viewp, bMeshAcisEntities, &tbegdo, &tenddo))
				{
					p_viewp->errln =__LINE__;
					p_viewp->gdo_rc = -3;
				}
#endif // BUILD_WITH_ACIS_SUPPORT
	}
	// ===================================================================
	// ******************* THE END OF VERY BIG SWITCH ********************
	// ===================================================================

	if(p_viewp->gdo_rc)
		goto out;
	if (tbegdo && tenddo)
	{
		if (doll == NULL)
			doll = tbegdo;
		else
			lastdo->next = tbegdo;
		lastdo = tenddo;
	}

	/* If this is an nentselp call, use doll, free it, and take */
	/* the necessary data: */
	if (p_viewp->nesp_sl!=NULL && doll!=NULL) {
		/*
		**	See if we have a winner.
		**
		**	Note the use of p_viewp->nesp_shuttleline to shuttle
		**	the disp objs to gr_selected().  I tried using
		**	p_viewp->ment (saving and restoring any existing disp
		**	objs) but something didn't like that and apparently
		**	blasted some memory.  Never figured it out.
		**	The p_viewp->nesp_shuttleline approach seems to work
		**	with no problems (indicating that it probably WAS a problem
		**	with existing disp objs).  p_viewp->nesp_shuttleline is a
		**	LINE whose only purpose in life is to carry display
		**	objects to gr_selected().  (The only thing that make me
		**	nervous about it is that it's a LINE conveying disp objs
		**	for ANY type of entity.)
		*/
		if (p_viewp->nesp_shuttleline==NULL)  /* Leave it for gr_freeviewll(). */
			p_viewp->nesp_shuttleline=new db_line(p_dp);
		p_viewp->nesp_shuttleline->set_disp(doll);	/* Point at the disp objs. */
		fi1=gr_selected(p_viewp->nesp_shuttleline,p_viewp->nesp_sl);
		p_viewp->nesp_shuttleline->set_disp(NULL);	/* Stop pointing at 'em. */

		/* Free the disp objs; we're not collecting them during an nentselp: */
		if (doll!=NULL) gr_freedisplayobjectll(doll);
		doll=lastdo=NULL;

		if (fi1) {	/* Gotcha */
			p_viewp->nesp_innerhip=p_viewp->ment;
			p_viewp->nesp_entaxis = new sds_point[3];
			if( p_viewp->nesp_entaxis )
				memcpy( p_viewp->nesp_entaxis, entaxis, sizeof(entaxis) );

			/* Build refstkres from p_viewp->idll: */
			if (p_viewp->nesp_refstkres!=NULL) {
				sds_relrb(p_viewp->nesp_refstkres);
				p_viewp->nesp_refstkres=NULL;
			}
			// EBATECH(CNBR) -[ Bugzilla78556 2003/6/25 Loose 2 or more nested insert enames.
			resbuf *trbp1, *trbp2;
			for (tidp1=p_viewp->idll; tidp1!=NULL; tidp1=tidp1->next) {
				if ((trbp2= new resbuf )==NULL)
					{ p_viewp->gdo_rc=-3; p_viewp->errln=__LINE__; goto out; }
				memset(trbp2,0,sizeof(resbuf));
				if( p_viewp->nesp_refstkres == NULL )
					trbp1 = p_viewp->nesp_refstkres = trbp2;
				else
					trbp1 = trbp1->rbnext = trbp2;

				trbp1->restype=RTENAME;
				trbp1->resval.rlname[0]=(long)tidp1->hip;
				trbp1->resval.rlname[1]=(long)p_dp;
				/*
				**	We need a stack (inside out) -- but p_viewp->idll
				**	is already a stack, so just build a normal
				**	llist.	(Shouldn't have many links, so
				**	don' bother maintaining a "last" ptr --
				**	just walk to it:
				*/
				//if (p_viewp->nesp_refstkres==NULL)
				//	 p_viewp->nesp_refstkres		=trbp1;
				//else p_viewp->nesp_refstkres->rbnext=trbp1;
			}
			// EBATECH(CNBR) ]-
		}
	}


out:
	if(p_viewp == NULL || p_viewp->gdo_rc)
	{
		if (doll)
			gr_freedisplayobjectll(doll);
		doll = lastdo = NULL;

		ASSERT(p_viewp->recurlevel >= 2);
	}
	else
	{
		/*
		**	Step one entity, then skip VERTEXs and SEQENDs --
		**	BUT NOT ATTRIBs.  (Some types of POLYLINE generation above
		**	don't update *p_elp, and we have to treat ATTRIBs
		**	like normal entities that just happen to follow INSERTs.)
		*/
		if(p_hipp && *p_hipp)
		{
			*p_hipp = (*p_hipp)->next;
			while(*p_hipp)
			{
				fint1 = (*p_hipp)->ret_type();
				if (fint1 != DB_VERTEX && fint1 != DB_SEQEND) break;
				*p_hipp = (*p_hipp)->next;
			}
		}
	}

	if(p_begdopp && p_enddopp)
	{
		*p_begdopp = doll;
		*p_enddopp = (doll == NULL) ? NULL : lastdo;
		doll = lastdo = NULL;
	}
	if(doll)
		gr_freedisplayobjectll(doll);
	doll = lastdo = NULL;
	if(apa)
	{
		IC_FREE(apa);
		apa=NULL;
	}

	if (--p_viewp->recurlevel<1)
	{
		p_viewp->recurlevel = 0;
		p_viewp->freeXrefPathList();
		if (p_ucsextp && p_viewp) {
            // Copy the m_ClipUcsExt[2] to p_ucsextp[2] if the block insert 
            // is clipped - Bug #78331 fix, SWH, 8/27/2003
			if (menttype == DB_INSERT && p_viewp->m_ClipUcsExtMode == 1) { // the block or Xref has been clipped
				GR_PTCPY(p_ucsextp[0], p_viewp->m_ClipUcsExt[0]);
				GR_PTCPY(p_ucsextp[1], p_viewp->m_ClipUcsExt[1]);
			}
			else {
				GR_PTCPY(p_ucsextp[0], p_viewp->ucsext[0]);
				GR_PTCPY(p_ucsextp[1], p_viewp->ucsext[1]);
			}
		}
		p_viewp->noucs2rp = 0;
	}

	return (p_viewp==NULL) ? -1 : p_viewp->gdo_rc;
}
//}		balance braces for DevStudio tools
// TODO -- change this to use gr_displayobject *, since following comment is ignorant
// of basics of C/C++
//

// Note this function changes the struct gr_view temporarily to get the bounding box in
// 'plan' view. It then restores the struct gr_view to the way it came in. It's main use
// is to correctly set up the horizontal offset between each letter of an mtext string.
// This calculates the offset as if the mtext string was in plan view.
int gr_getboundingBox(
	db_handitem *texthip,
	gr_textgenparslink par,
	sds_point bottomLeft,
	sds_point upperRight,
	db_drawing *p_dp,
	gr_view *p_viewp,
	CDC *dc)
{
	bool firstPass = TRUE;
	sds_real left, bottom, right, top;

	sds_real oldrot = par.rot;
	par.rot = 0.0;

	CMatrix4 transformation(p_viewp->m_transformation);
	p_viewp->m_transformation.makeIdentity();

	gr_displayobject *Head=NULL, *Tail=NULL, *tmp=NULL;
	left = bottom = right = top = 0.0;

	bottomLeft[0] = bottomLeft[1] = bottomLeft[2] = 0.0;
	upperRight[0] = upperRight[1] = upperRight[2] = 0.0;

	if (texthip == NULL)
		return RTERROR;

	sds_point rpaxis0, rpaxis1, rpaxis2;
	GR_PTCPY(rpaxis0,p_viewp->rpaxis[0]);
	GR_PTCPY(rpaxis1,p_viewp->rpaxis[1]);
	GR_PTCPY(rpaxis2,p_viewp->rpaxis[2]);
	p_viewp->rpaxis[0][0] = 1.0; p_viewp->rpaxis[0][1] = 0.0; p_viewp->rpaxis[0][2] = 0.0;
	p_viewp->rpaxis[1][0] = 0.0; p_viewp->rpaxis[1][1] = 1.0; p_viewp->rpaxis[1][2] = 0.0;
	p_viewp->rpaxis[2][0] = 0.0; p_viewp->rpaxis[2][1] = 0.0; p_viewp->rpaxis[2][2] = 1.0;

	if ((p_viewp->gdo_rc=gr_textgen((db_entity*)texthip,
									&par,
									0,
									p_dp,
									p_viewp,
									dc,
									&Head,
									&Tail,
									NULL,
									NULL))!=0) {
		delete Head; Head = NULL;
		p_viewp->errln=__LINE__;
		return RTERROR;
	}
	GR_PTCPY(p_viewp->rpaxis[0],rpaxis0);
	GR_PTCPY(p_viewp->rpaxis[1],rpaxis1);
	GR_PTCPY(p_viewp->rpaxis[2],rpaxis2);

	p_viewp->m_transformation = transformation;

	tmp = Head;
	while (tmp != NULL) {
		if (firstPass) {
			left = right = tmp->chain[0];	// Initialize to first 'x'.
			bottom = top = tmp->chain[1];
			firstPass = FALSE;
		}
		for (int i = 0; i < tmp->npts * 2; i++) {
			if (tmp->chain[i] < left)
				left = tmp->chain[i];
			else if(tmp->chain[i] > right)
				right = tmp->chain[i];
			i++;
			if (tmp->chain[i] < bottom)
				bottom = tmp->chain[i];
			else if(tmp->chain[i] > top)
				top = tmp->chain[i];
		}
		tmp = tmp->next;
	}
	bottomLeft[0] = left;
	bottomLeft[1] = bottom;
	upperRight[0] = right;
	upperRight[1] = top;

	par.rot = oldrot;

	if (Head!=NULL)
		gr_freedisplayobjectll(Head);
	Head=Tail=NULL;

	return RTNORM;
}

static void xformpt(sds_point from, sds_point to, sds_matrix mat) {
/*
**	Applies an ADS-type matrix to a point.
**	'from and 'to' can be the same point in memory.
*/
	sds_point ap1;

	ap1[0]=mat[0][0]*from[0]+mat[0][1]*from[1]+mat[0][2]*from[2]+mat[0][3];
	ap1[1]=mat[1][0]*from[0]+mat[1][1]*from[1]+mat[1][2]*from[2]+mat[1][3];
	ap1[2]=mat[2][0]*from[0]+mat[2][1]*from[1]+mat[2][2]*from[2]+mat[2][3];
	to[0]=ap1[0]; to[1]=ap1[1]; to[2]=ap1[2];
}


short ClipAndCreateDO(db_entity* pSourceEntity, CPolyLine& Points, short Closed, short Fillable, short Dimension, gr_view* pGrView, gr_displayobject** ppBegNewDO, gr_displayobject** ppEndNewDO);

extern gr_view gr_defview;

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	An extension of gr_getdispobjs. Generate display objects list for
 *			the clipping boundary of the given insert.
 * Returns:	gr_getdispobjs return codes.
 ********************************************************************************/
short
gr_genClippingBoundary
(
 db_entity*			pSourceEntity,	// =>
 db_drawing*		pDrawing,		// =>
 gr_view*			pGrView,		// =>
 gr_displayobject**	ppBegDO,		// <=
 gr_displayobject**	ppEndDO			// <=
)
{
	short				rc = 0;
	CPolyLine			PointsArray;
	sds_point*			pPointsArray = NULL;
	int					OriginalNumberOfPoints, NumberOfPoints, i;
	gr_displayobject	*pBegDO = NULL, *pEndDO = NULL;
	db_spatial_filter*	pSpatialFilter = (db_spatial_filter*)((db_insert*)pSourceEntity)->GetFilter(pDrawing);

	sds_point			entAxes[3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
						pTmpMatrix[4],
						point;
	bool				bAxesOk = true;
	double				length;
	sds_matrix			pMatrix;
	CMatrix4			FullTransformation, OrigInsertTransformInv, ActualInsertTransform, BoundaryTransform, entityTransform;

	ASSERT(pSpatialFilter && pSourceEntity && pSourceEntity->ret_type() == DB_INSERT && pDrawing && ppBegDO && ppEndDO);
	if(!pSpatialFilter || !pSourceEntity || pSourceEntity->ret_type() != DB_INSERT || !pDrawing || !ppBegDO || !ppEndDO)
	{
		rc = -1;
		goto exit;
	}

	if(!pGrView || pGrView->viewsize < IC_ZRO)
		pGrView = &gr_defview;

	// get boundary points from the spatial filter object
	NumberOfPoints = OriginalNumberOfPoints = pSpatialFilter->ret_npts();

	if(OriginalNumberOfPoints == 2)
		NumberOfPoints = 4;

	if(!(pPointsArray = new sds_point[NumberOfPoints]))
	{
		rc = -3;
		goto exit;
	}

	pSpatialFilter->get_ptarray(pPointsArray, OriginalNumberOfPoints);

	if(OriginalNumberOfPoints == 2)
	{
		GR_PTCPY(pPointsArray[2], pPointsArray[1]);
		pPointsArray[1][0] = pPointsArray[0][0];
		GR_PTCPY(pPointsArray[3], pPointsArray[0]);
		pPointsArray[3][0] = pPointsArray[2][0];
	}

	// fill the Geo array with points transforming them
	if(PointsArray.MakeRoom(0, NumberOfPoints) != SUCCESS)
	{
		rc = -3;
		goto exit;
	}

	pMatrix[3][0] = pMatrix[3][1] = pMatrix[3][2] = 0.0;
	pMatrix[3][3] = 1.0;

	pSpatialFilter->get_inverseblockmat(pTmpMatrix);
	for(i = 0; i < 4; i++)	// transpose the matrix
	{
		pMatrix[0][i] = pTmpMatrix[i][0];
		pMatrix[1][i] = pTmpMatrix[i][1];
		pMatrix[2][i] = pTmpMatrix[i][2];
	}

	OrigInsertTransformInv = (double*)pMatrix;

	pSpatialFilter->get_clipboundmat(pTmpMatrix);
	for(i = 0; i < 4; i++)	// transpose the matrix
	{
		pMatrix[0][i] = pTmpMatrix[i][0];
		pMatrix[1][i] = pTmpMatrix[i][1];
		pMatrix[2][i] = pTmpMatrix[i][2];
	}

	BoundaryTransform = (double*)pMatrix;

	memset(entAxes, 0, 9 * sizeof(double));
	pSpatialFilter->get_extru(entAxes[2]);

	if((length = GR_LEN(entAxes[2])) < IC_ZRO)
		bAxesOk = false;
	else
	{
		if(fabs(length - 1.0) > IC_ZRO)
			GR_UNITIZE(entAxes[2], length);

		if(fabs(entAxes[2][2] - 1.0) < IC_ZRO)
			bAxesOk = false;
		else
		{
			if(ic_arbaxis(entAxes[2], entAxes[0], entAxes[1], entAxes[2]) == -1)
				bAxesOk = false;
		}
	}

	if(!bAxesOk)
		ic_identity(entAxes);

	entityTransform.makeIdentity();
	entityTransform(0,0) = entAxes[0][0];
	entityTransform(1,0) = entAxes[0][1];
	entityTransform(2,0) = entAxes[0][2];
	entityTransform(0,1) = entAxes[1][0];
	entityTransform(1,1) = entAxes[1][1];
	entityTransform(2,1) = entAxes[1][2];
	entityTransform(0,2) = entAxes[2][0];
	entityTransform(1,2) = entAxes[2][1];
	entityTransform(2,2) = entAxes[2][2];

	FullTransformation = BoundaryTransform *
						 pGrView->m_transformation * OrigInsertTransformInv *
						 BoundaryTransform.invert();

	for(i = 0; i < NumberOfPoints; i++)
	{
		FullTransformation.multiplyRight(point, pPointsArray[i]);
		GR_PTCPY(pPointsArray[i], point);
	}

	for(i = 1; i < NumberOfPoints; i++)
		pPointsArray[i][2] = pPointsArray[0][2];

	for(i = 0; i < NumberOfPoints; i++)
	{
		entityTransform.multiplyRight(point, pPointsArray[i]);
		PointsArray.Set(i, C3Point(point[0], point[1], point[2]));
	}

	// create a display list at last
	if(rc = ClipAndCreateDO(pSourceEntity, PointsArray, 1, 0, 2 + (!pGrView->projmode || pGrView->noucs2rp), pGrView, &pBegDO, &pEndDO))
	{
		gr_freedisplayobjectll(pBegDO);
		pBegDO = pEndDO = NULL;
	}

exit:

	delete [] pPointsArray;

	*ppBegDO = pBegDO;
	*ppEndDO = pEndDO;

	return rc;
}


static
bool gr_genDispObj
(
sds_dobjll* pBegDisp,
db_entity* pEntity,
gr_view* pGrView,
gr_displayobject** ppBegDO,
gr_displayobject** ppEndDO
);

static
bool gr_genDispObjWithClipping
(
sds_dobjll* pBegDisp,
db_entity* pEntity,
gr_view* pGrView,
gr_displayobject** ppBegDO,
gr_displayobject** ppEndDO
);

static
void freeSdsDispObjList(sds_dobjll* pDisp);

bool gr_acisGetDispObjs
(
db_entity* pEntity,
db_drawing*	pDrw,
gr_view* pGrView,
bool bMeshAcisEntities,
gr_displayobject** ppBegDO,
gr_displayobject** ppEndDO
)
{
	ASSERT(pEntity && pEntity->isAcisEntity() && pDrw && pGrView && ppBegDO && ppEndDO);
	if(!pEntity || !pEntity->isAcisEntity() || !pDrw || !pGrView || !ppBegDO || !ppEndDO)
		return false;

	*ppBegDO = *ppEndDO = 0;
	resbuf rb;
	if (db_getvar("ISOLINES"/*DNT*/, 0, &rb, pDrw, NULL, NULL) != RTNORM)
		return false;

	sds_dobjll* pBegDisp = 0;
	CDbAcisEntity* pEnt = static_cast<CDbAcisEntity*>(pEntity);
	if (!CModeler::get()->display(pEnt->getData(),
								  bMeshAcisEntities? -1 : rb.resval.rint,
								  fabs(pGrView->GetPixelHeight()) / pGrView->absxsclp,
								  &pBegDisp))
	{
        ASSERT(0);          // To catch ACIS problems.
		return false;
	}

	// if it's dragging of acis object, just transform exist dipobj
	if (pEnt->draggingCurr() != pEnt->draggingStart())
	{
		point pnt = pEnt->draggingCurr();
		gvector axisZ = pEnt->axisZ();
		gvector view = pEnt->viewDir();
		point base = pEnt->draggingStart();
		point pr = icl::projection(pnt, base, axisZ, &view);

		transf move = translation(pr - base);

		sds_dobjll* pDo = pBegDisp;
		while (pDo)
		{
			for (int i = 0; i < pDo->npts; i++)
			{
				double* pt = pDo->chain + 3*i;
				*reinterpret_cast<point*>(pt) *= move;
			}
			pDo = pDo->next;
		}
	}

	bool bResult;
	int	ltsclwblk_Saved = pGrView->ltsclwblk;
	int color_Saved = pGrView->color;
	int lweight_Saved = pGrView->lweight;
	pGrView->ltsclwblk = 1;
	if (pGrView->m_Clipper.IsNotEmpty())
	{
		bResult = gr_genDispObjWithClipping(pBegDisp, pEntity, pGrView, ppBegDO, ppEndDO);
	}	// clipping
	else
	{
		bResult = gr_genDispObj(pBegDisp, pEntity, pGrView, ppBegDO, ppEndDO);
	}	// no clipping

	freeSdsDispObjList(pBegDisp);

	pGrView->ltsclwblk = ltsclwblk_Saved;
	pGrView->color = color_Saved;
	pGrView->lweight = lweight_Saved;

	return bResult;
}

static
bool gr_genDispObj
(
sds_dobjll* pBegDisp,
db_entity* pEntity,
gr_view* pGrView,
gr_displayobject** ppBegDO,
gr_displayobject** ppEndDO
)
{
	int dimension = 2 + (!pGrView->projmode || pGrView->noucs2rp);

	for (sds_dobjll* pCurr = pBegDisp; pCurr; pCurr = pCurr->next)
	{
		gr_displayobject* pNewDO = gr_displayobject::newDisplayObject(pEntity);
		if(!pNewDO)
			return false;

		pNewDO->next = 0;
		pNewDO->type = pCurr->type;
		if (dimension < 3)
			--(pNewDO->type);
#if 0
		pNewDO->color = (pCurr->color == DB_BLCOLOR) ?
						pGrView->color :
						DB_BLCOLOR;
#else
        //<alm>:it's need for coloring of Acis entities in SOLIDEDIT/FACE(EDGE)/COLOR
        pNewDO->color = (pCurr->color == DB_BLCOLOR)? pGrView->color: pCurr->color;
        //</alm>
#endif
#ifdef BUILD_WITH_NEW_SDS_DOBJLL
		pNewDO->lweight = (pCurr->lweight < 0 )? pGrView->lweight : pCurr->lweight; // 2003-5-5
		//pNewDO->lweight = pGrView->lweight;	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
#else
		pNewDO->lweight = 0;
#endif
		pNewDO->npts = pCurr->npts;

		if (!(pNewDO->chain = new sds_real[pNewDO->npts * dimension ]))
		{
			IC_FREE(pNewDO);
			return false;
		}

		for (int i = pCurr->npts; i--; )
		{
			sds_point pnt;
			if (pGrView->ltsclwblk)
				gr_ncs2ucs(pCurr->chain + i*3, pnt, 0, pGrView);
			else
				GR_PTCPY(pnt, (pCurr->chain + i*3));

			if (pGrView->projmode && gr_ucs2rp(pnt, pnt, pGrView))
			{
				gr_displayobject::freeDisplayObject(pNewDO);
				return false;
			}
			gr_setdochainpt(pNewDO, i, pnt, dimension);
		}

		if (!*ppBegDO)
			*ppBegDO = pNewDO;
		else
			(*ppEndDO)->next = pNewDO;
		*ppEndDO = pNewDO;
	}	// for( thru sds disp objs

	return true;
}

static
bool gr_genDispObjWithClipping
(
sds_dobjll* pBegDisp,
db_entity* pEntity,
gr_view* pGrView,
gr_displayobject** ppBegDO,
gr_displayobject** ppEndDO
)
{
	CPolyLine PointsArray;
	gr_displayobject* pBegDO, *pEndDO;
	for (sds_dobjll* pCurr = pBegDisp; pCurr; pCurr = pCurr->next)
	{
		pBegDO = pEndDO = 0;
		if (PointsArray.MakeRoom(0, pCurr->npts) != SUCCESS)
			return false;

		for (int i = pCurr->npts; i--;)
		{
			sds_point pnt;
			if(pGrView->ltsclwblk)
				gr_ncs2ucs(pCurr->chain + i*3, pnt, 0, pGrView);
			else
				GR_PTCPY(pnt, (pCurr->chain + i*3));
			PointsArray.Set(i, C3Point(pnt[0], pnt[1], pnt[2]));
		}

		if (pCurr->color != DB_BLCOLOR)
			pGrView->color = pCurr->color;

#ifdef BUILD_WITH_NEW_SDS_DOBJLL
		if (pCurr->lweight >= 0)
			pGrView->lweight = pCurr->lweight;
#else
			pGrView->lweight = 0;
#endif

		if (ClipAndCreateDO(pEntity,
			 				PointsArray,
						   (pCurr->type & DISP_OBJ_PTS_CLOSED) ? 1 : 0,
							0,
							2 + (!pGrView->projmode || pGrView->noucs2rp),
							pGrView,
						   &pBegDO, &pEndDO))
		{
			gr_freedisplayobjectll(pBegDO);
			pBegDO = pEndDO = 0;
			return false;
		}

		if (pBegDO && pEndDO)
		{
			if(!*ppBegDO)
				*ppBegDO = pBegDO;
			else
				(*ppEndDO)->next = pBegDO;
			*ppEndDO = pEndDO;
		}

		PointsArray.RemoveAll();
	}	// for( thru sds disp objs

	return true;
}

static
void freeSdsDispObjList(sds_dobjll* pDisp)
{
	while (pDisp)
	{
		if(pDisp->chain)
		{
			delete pDisp->chain;
			pDisp->chain = 0;
		}

		sds_dobjll* pTmp = pDisp->next;
		delete pDisp;
		pDisp = pTmp;
	}
}

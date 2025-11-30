/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * functions that aid view class
 * 
 * History
 * 24-03-2003 EBATECH(CNBR) Bugzilla#78471 Support Lineweight
 */ 

#include "gr.h"
#include "gr_view.h"
#include "pc2dispobj.h"

gr_view::gr_view() 
{
	// nullify zero members
	// Don't use memset!

	m_CenterProjection[0] = m_CenterProjection[1] = m_CenterProjection[2] =
	target[0] = target[1] = target[2] =
	ucsorg[0] = ucsorg[1] = ucsorg[2] =
	m_thickness[0] = m_thickness[1] = m_thickness[2] =
	ucsext[0][0] = ucsext[0][1] = ucsext[0][2] =
	ucsext[1][0] = ucsext[1][1] = ucsext[1][2] =
	camera[0] = camera[1] =
	rpaxis[0][1] = rpaxis[0][2] =
	rpaxis[1][0] = rpaxis[1][2] =
	rpaxis[2][0] = rpaxis[2][1] =
	ucsaxis[0][1] = ucsaxis[0][2] =
	ucsaxis[1][0] = ucsaxis[1][2] =
	ucsaxis[2][0] = ucsaxis[2][1] =
	m_ClipRect.xmin = m_ClipRect.xmax = m_ClipRect.ymin = m_ClipRect.ymax =
	m_VPortClipRect.xmin = m_VPortClipRect.xmax = m_VPortClipRect.ymin = m_VPortClipRect.ymax =
	m_ReciprocalOfPixelHeight = m_ReciprocalOfPixelWidth =
	elevation =
	absrot = 0.0;
    nesp_entaxis = NULL;
	ment = bblthip = bllthip = lthip = nesp_innerhip = nesp_shuttleline = NULL;
	pc = lapc = lupc = NULL;
	nesp_sl = NULL;
	nesp_refstkres = NULL;
	previnsdata = idll = NULL;
	xrefpnll = NULL;
	next = NULL;
    previnsfrozen = mode = noucs2rp = isprinting = splframe = pdmode =
	qtextmode = ltsclwblk = projmode = dothick = dowid = gdo_rc = recurlevel = 0;
	npc = upc = errln = 0L;

	// Now init all the non-0 stuff:

	ucsextmode = -1;
	attmode = fillmode = 1;
	rp_ucs = ncs_ucs = IC_TRANS_NO_SCALE_ROTATE_TRANSLATE;
	bbcolor = blcolor = color = 7;
	bblweight = bllweight = lweight = DB_DFWEIGHT;
	curvdispqual = 100;
	midwin[0] = 160;	// a 320x200
	midwin[1] = 100;	// display
	viewdist = ltscale = absysclp = absxsclp = camera[2] = 1.0;
	viewsize = 10.0;
	pdsize = 0.25;
	m_PixelHeight = - (m_PixelWidth = 0.05);
	rpaxis[0][0] = rpaxis[1][1] = rpaxis[2][2] = 1.0;
	ucsaxis[0][0] = ucsaxis[1][1] = ucsaxis[2][2] = 1.0;
	m_transformation.makeIdentity();
	m_transformationInverse.makeIdentity();
	m_bVPortClip = false;
}

gr_view::gr_view(const gr_view &sour)
{
	// Don't use memcpy!

	GR_PTCPY(m_CenterProjection, sour.m_CenterProjection);
	GR_PTCPY(target, sour.target);
	GR_PTCPY(ucsorg, sour.ucsorg);
	GR_PTCPY(m_thickness, sour.m_thickness);
	GR_PTCPY(ucsext[0], sour.ucsext[0]);
	GR_PTCPY(ucsext[1], sour.ucsext[1]);
	GR_PTCPY(camera, sour.camera);
	GR_PTCPY(rpaxis[0], sour.rpaxis[0]);
	GR_PTCPY(rpaxis[1], sour.rpaxis[1]);
	GR_PTCPY(rpaxis[2], sour.rpaxis[2]);
	GR_PTCPY(ucsaxis[0], sour.ucsaxis[0]);
	GR_PTCPY(ucsaxis[1], sour.ucsaxis[1]);
	GR_PTCPY(ucsaxis[2], sour.ucsaxis[2]);

	m_ClipRect.xmin = sour.m_ClipRect.xmin;
	m_ClipRect.xmax = sour.m_ClipRect.xmax;
	m_ClipRect.ymin = sour.m_ClipRect.ymin;
	m_ClipRect.ymax = sour.m_ClipRect.ymax;
	m_VPortClipRect.xmin = sour.m_VPortClipRect.xmin;
	m_VPortClipRect.xmax = sour.m_VPortClipRect.xmax;
	m_VPortClipRect.ymin = sour.m_VPortClipRect.ymin;
	m_VPortClipRect.ymax = sour.m_VPortClipRect.ymax;
	m_ReciprocalOfPixelHeight = sour.m_ReciprocalOfPixelHeight;
	m_ReciprocalOfPixelWidth = sour.m_ReciprocalOfPixelWidth;
	elevation = sour.elevation;
	absrot = sour.absrot;
	nesp_entaxis = sour.nesp_entaxis;
	ment = sour.ment;
	bblthip = sour.bblthip;
	lthip = sour.lthip;
	nesp_innerhip = sour.nesp_innerhip;
	lapc = sour.lapc;
	lupc = sour.lupc;
	nesp_sl = sour.nesp_sl;
	next = sour.next;
	previnsfrozen = sour.previnsfrozen;
	mode = sour.mode;
	noucs2rp = sour.noucs2rp;
	isprinting = sour.isprinting;
	splframe = sour.splframe;
	pdmode = sour.pdmode;
	qtextmode = sour.qtextmode;
	ltsclwblk = sour.ltsclwblk;
	projmode = sour.projmode;
	dothick = sour.dothick;
	dowid = sour.dowid;
	gdo_rc = sour.gdo_rc;
	recurlevel = sour.recurlevel;
	npc = sour.npc;
	upc = sour.upc;
	errln = sour.errln;
	ucsextmode = sour.ucsextmode;
	attmode = sour.attmode;
	fillmode = sour.fillmode;
	rp_ucs = sour.rp_ucs;
	ncs_ucs = sour.ncs_ucs;
	bbcolor = sour.bbcolor;
	blcolor = sour.blcolor;
	color = sour.color;
	bblweight = sour.bblweight;
	bllweight = sour.bllweight;
	lweight = sour.lweight;
	curvdispqual = sour.curvdispqual;
	midwin[0] = sour.midwin[0];
	midwin[1] = sour.midwin[1];
	viewdist = sour.viewdist;
	ltscale = sour.ltscale;
	absysclp = sour.absysclp;
	absxsclp = sour.absxsclp;
	viewsize = sour.viewsize;
	pdsize = sour.pdsize;
	m_PixelHeight = sour.m_PixelHeight;
	m_PixelWidth = sour.m_PixelWidth;
	m_bVPortClip = sour.m_bVPortClip;
	m_transformation = sour.m_transformation;
	m_transformationInverse = sour.m_transformationInverse;
	
	// don't copy allocated members
	pc = NULL;
	nesp_refstkres = NULL;
	nesp_shuttleline = NULL;
	previnsdata = idll = NULL;
	xrefpnll = NULL;
}

gr_view::~gr_view()
{
	gr_stringlink*	tslp1;
	db_insdatalink*	tidp1;

	freePointsChain();
	sds_relrb(nesp_refstkres);
	delete nesp_shuttleline;
	delete previnsdata;	// Just one link.

	while(idll)
	{
		tidp1 = idll->next;
		delete idll;
		idll = tidp1;
	}

	while(xrefpnll) 
	{
		IC_FREE(xrefpnll->str);
		tslp1 = xrefpnll->next;
		delete xrefpnll;
		xrefpnll = tslp1;
	}
}

short gr_view::allocatePointsChain(long nwanted)
{
    if(nwanted < 1L)
	{
		upc = 0L; 
		return 0; 
	}
    if(pc == NULL) 
	{
        npc=0L;
        if((pc = lapc = new struct gr_pclink ) == NULL)
			return -1;
		memset(pc,0,sizeof(struct gr_pclink));
        npc++;
    }
    while(nwanted > npc) 
	{
        if((lapc->next = new struct gr_pclink ) == NULL)
			return -1;
		memset(lapc->next,0,sizeof(struct gr_pclink));
        lapc = lapc->next;
        npc++;
    }
    upc=nwanted;
    return 0;
}

void gr_view::freePointsChain()
{
	gr_pclink *pPointsChainListItem;
	while(pc != NULL) 
	{ 
		pPointsChainListItem = pc->next; 
		delete pc; 
		pc = pPointsChainListItem; 
	}
	lapc = NULL;
	npc = 0;
	lupc = NULL;
	upc = 0;
}

bool gr_view::isCoplanarToScreen(db_entity_with_extrusion *ent)
{
	// get and normalize the extrusion vector for the ent
	sds_point entVector;
	ent->get_210(entVector);

	double num;
	num = sqrt(entVector[0] * entVector[0] + entVector[1] * entVector[1] + entVector[2] * entVector[2]);
	entVector[0] /= num;
	entVector[1] /= num;
	entVector[2] /= num;

	// get and normalize the view's Z direction
	sds_point viewZDir;
	viewZDir[0] = rpaxis[2][0];
	viewZDir[1] = rpaxis[2][1];
	viewZDir[2] = rpaxis[2][2];

	num = sqrt(viewZDir[0] * viewZDir[0] + viewZDir[1] * viewZDir[1] + viewZDir[2] * viewZDir[2]);
	viewZDir[0] /= num;
	viewZDir[1] /= num;
	viewZDir[2] /= num;

	// calculate the crossproduct of the vectors and see if the result's
	// length is close to 0
	sds_point crossProduct;
	ic_crossproduct(entVector, viewZDir, crossProduct);
	if (ic_veclength(crossProduct) < IC_ZRO)
		return true;
	else
		return false;
}

GR_API short gr_initview(
    db_drawing           *dp,
    db_charbuflink       *configbuf,
    gr_view     **viewpp,
    struct gr_viewvars  *viewvarsp) {
/*
**  Sets the members of *viewpp based on the appropriate system
**  variables -- or the members of *viewvarsp.  Allocates *viewpp if
**  it's NULL.  IT'S UP TO THE CALLER TO FREE *viewpp (with
**  gr_freeviewll()) IF THIS FUNCTION ALLOCATES IT.
**
**  configbuf is needed to get SCREENSIZE.  SCREENSIZE will default
**  to 320,200 if configbuf is passed as NULL.
**
**  If viewvarsp is non-NULL, it must specify the viewing parameters
**  (instead of the system variables).  The variables must be in the
**  same coordinate systems used by the header and tables.  That is,
**  certain variables that are presented to the user in the UCS but that
**  are actually stored in the WCS or DCS must be WCS or DCS in viewvarsp
**  also (TARGET, VIEWDIR, VIEWCTR).
**
**  If viewvarsp is NULL, the system variables are used (via getvar).
**
**  This function should be called before any of the viewing/displaying
**  conversion functions are called (such as gr_getdispobjs(),
**  gr_ucs2rp(), gr_rp2ucs(), gr_rp2pix(), and gr_pix2rp()).
**
**  Beware of juggling the order of events in here; some settings
**  rely on earlier ones.
**
**  Returns:
**
**       0 : OK
**      -1 : dp==NULL or viewpp==NULL
**      -2 : No memory
**      -3 : db_getvar() (or gr_getucs()) failed
**      -4 : VIEWDIR is (0,0,0) (camera and target coincident)
**      -5 : VIEWCTR is behind the camera (perspective)
**      -6 : SCREENSIZE has a zero height and/or width
**      -7 : VIEWSIZE is 0.0
*/
    char *headerbuf;
    short fi1,fi2,rc;
    sds_real ar1,ar2;
    sds_point ap1,ap2;
    struct resbuf rb,wcsrb,ucsrb,dcsrb;
    gr_view *viewp;

    static short dejavu;


    rc=0; memset(&rb,0,sizeof(rb));

    if (!dejavu) {
        db_setgrafunptrs(gr_freedisplayobjectll);
        dejavu=1;
    }

    if (dp==NULL || viewpp==NULL) { rc=-1; goto out; }

    /* Make sure that *viewpp is allocated: */
    if (*viewpp==NULL) 
		*viewpp=new gr_view;

    viewp=*viewpp;  /* For convenience. */

    /* Set the coordinate system resbufs for sds_trans(): */
    wcsrb.rbnext=        ucsrb.rbnext=        dcsrb.rbnext =NULL;
    wcsrb.restype=       ucsrb.restype=       dcsrb.restype=RTSHORT;
    wcsrb.resval.rint=0; ucsrb.resval.rint=1; dcsrb.resval.rint=2;


    /*
    **  The following makes sure that all elements that need initialization
    **  get it.  (Note that there are many elements that I use in
    **  gr_getdispobjs() and its extensions that I set as needed
    **  in those functions; they don't need any work here.  They used to
    **  be globals before we starting using threads.  Then we put them in here.)
    */

                      /* noucs2rp, isprinting */

    viewp->noucs2rp=viewp->isprinting=0;

                         /* ucsorg, ucsaxis */
    if (gr_getucs(viewp->ucsorg,viewp->ucsaxis[0],viewp->ucsaxis[1],
        viewp->ucsaxis[2],viewvarsp,dp)) { rc=-3; goto out; }

                          /* mode, target */

    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QVIEWMODE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->mode=rb.resval.rint;
        if (db_getvar(NULL,DB_QTARGET,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        GR_PTCPY(viewp->target,rb.resval.rpoint);
    } else {

        viewp->mode=viewvarsp->viewmode;

        /*
        **  Trans TARGET from WCS to UCS -- but DON'T USE dp->trans()!
        **  That would use the UCS from the header.  We need to use
        **  the UCS just set in viewp above.
        */
        ap1[0]=(viewvarsp->target[0]-viewp->ucsorg[0]);
        ap1[1]=(viewvarsp->target[1]-viewp->ucsorg[1]);
        ap1[2]=(viewvarsp->target[2]-viewp->ucsorg[2]);
        viewp->target[0]=ap1[0]*viewp->ucsaxis[0][0]+
                         ap1[1]*viewp->ucsaxis[0][1]+
                         ap1[2]*viewp->ucsaxis[0][2];
        viewp->target[1]=ap1[0]*viewp->ucsaxis[1][0]+
                         ap1[1]*viewp->ucsaxis[1][1]+
                         ap1[2]*viewp->ucsaxis[1][2];
        viewp->target[2]=ap1[0]*viewp->ucsaxis[2][0]+
                         ap1[1]*viewp->ucsaxis[2][1]+
                         ap1[2]*viewp->ucsaxis[2][2];
    }


                     /* camera, rpaxis, viewdist */

    /* NOTE: Uses the UCS axes (viewp->ucsaxis) and viewp->target, */
    /* so keep this code AFTER the code that sets them. */

    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QVIEWDIR,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        GR_PTCPY(viewp->rpaxis[2],rb.resval.rpoint);
    } else {
        /*
        **  Trans VIEWDIR from WCS to UCS -- but DON'T USE dp->trans()!
        **  That would use the UCS from the header.  We need to use
        **  the UCS just set in viewp above.  (Remember VIEWDIR is a vector!)
        */
        viewp->rpaxis[2][0]=viewvarsp->viewdir[0]*viewp->ucsaxis[0][0]+
                            viewvarsp->viewdir[1]*viewp->ucsaxis[0][1]+
                            viewvarsp->viewdir[2]*viewp->ucsaxis[0][2];
        viewp->rpaxis[2][1]=viewvarsp->viewdir[0]*viewp->ucsaxis[1][0]+
                            viewvarsp->viewdir[1]*viewp->ucsaxis[1][1]+
                            viewvarsp->viewdir[2]*viewp->ucsaxis[1][2];
        viewp->rpaxis[2][2]=viewvarsp->viewdir[0]*viewp->ucsaxis[2][0]+
                            viewvarsp->viewdir[1]*viewp->ucsaxis[2][1]+
                            viewvarsp->viewdir[2]*viewp->ucsaxis[2][2];
    }
    if (icadRealEqual((viewp->viewdist=GR_LEN(viewp->rpaxis[2])),0.0)) { rc=-4; goto out; }
    /* When VIEWDIR is ALMOST in the UCS XY-plane, let's force it to be */
    /* precisely in it (to avoid huge numbers and rounding errors */
    /* associated with "XY edge-on" projections: */
    if (fabs(viewp->rpaxis[2][2])<fabs(IC_ZRO*viewp->viewdist)) {
        viewp->rpaxis[2][2]=0.0;
        if (icadRealEqual((viewp->viewdist=GR_LEN(viewp->rpaxis[2])),0.0)) { rc=-4; goto out; }
    }
    viewp->camera[0]=viewp->target[0]+viewp->rpaxis[2][0];
    viewp->camera[1]=viewp->target[1]+viewp->rpaxis[2][1];
    viewp->camera[2]=viewp->target[2]+viewp->rpaxis[2][2];
    GR_UNITIZE(viewp->rpaxis[2],viewp->viewdist);

    /*
    **  Cross WorldCS Z-axis with viewp->rpaxis[2] to get the X-axis.
    **  Remember that WCS axes matrix in the UCS is the transpose
    **  of the UCS axes matrix in the WCS:
    */
    viewp->rpaxis[0][0]=viewp->ucsaxis[1][2]*viewp->rpaxis[2][2]-viewp->ucsaxis[2][2]*viewp->rpaxis[2][1];
    viewp->rpaxis[0][1]=viewp->ucsaxis[2][2]*viewp->rpaxis[2][0]-viewp->ucsaxis[0][2]*viewp->rpaxis[2][2];
    viewp->rpaxis[0][2]=viewp->ucsaxis[0][2]*viewp->rpaxis[2][1]-viewp->ucsaxis[1][2]*viewp->rpaxis[2][0];
    if ((ar1=GR_LEN(viewp->rpaxis[0]))<IC_ZRO) {
        /*
        **  Set WCS plan view (or anti-plan view).
        **  REMEMBER TO USE UCS COORDINATES, HOWEVER.
        */

        /* Dot ZWCS with VIEWDIR to see if plan or anti-plan view: */
        fi1=(viewp->ucsaxis[0][2]*viewp->rpaxis[2][0]+
             viewp->ucsaxis[1][2]*viewp->rpaxis[2][1]+
             viewp->ucsaxis[2][2]*viewp->rpaxis[2][2]>0.0);  /* 1 = plan */

        /* The RP Y-axis follows the +YWCS in either case: */
        viewp->rpaxis[1][0]=viewp->ucsaxis[0][1];
        viewp->rpaxis[1][1]=viewp->ucsaxis[1][1];
        viewp->rpaxis[1][2]=viewp->ucsaxis[2][1];

        /*
        **  The RP X- and Z-axes follow the positive WCS ones for plan view
        **  and the negative ones for anti-plan view.
        **  (The Z-axis is set here just to align it EXACTLY.)
        */
        if (fi1) {
            viewp->rpaxis[0][0]=viewp->ucsaxis[0][0];
            viewp->rpaxis[0][1]=viewp->ucsaxis[1][0];
            viewp->rpaxis[0][2]=viewp->ucsaxis[2][0];
            viewp->rpaxis[2][0]=viewp->ucsaxis[0][2];
            viewp->rpaxis[2][1]=viewp->ucsaxis[1][2];
            viewp->rpaxis[2][2]=viewp->ucsaxis[2][2];
        } else {
            viewp->rpaxis[0][0]=-viewp->ucsaxis[0][0];
            viewp->rpaxis[0][1]=-viewp->ucsaxis[1][0];
            viewp->rpaxis[0][2]=-viewp->ucsaxis[2][0];
            viewp->rpaxis[2][0]=-viewp->ucsaxis[0][2];
            viewp->rpaxis[2][1]=-viewp->ucsaxis[1][2];
            viewp->rpaxis[2][2]=-viewp->ucsaxis[2][2];
        }
    } else {
        if (ar1!=1.0) {
            GR_UNITIZE(viewp->rpaxis[0],ar1);
        }

        /* Cross Z-axis with the X-axis to get the Y-axis: */
        viewp->rpaxis[1][0]=viewp->rpaxis[2][1]*viewp->rpaxis[0][2]-viewp->rpaxis[2][2]*viewp->rpaxis[0][1];
        viewp->rpaxis[1][1]=viewp->rpaxis[2][2]*viewp->rpaxis[0][0]-viewp->rpaxis[2][0]*viewp->rpaxis[0][2];
        viewp->rpaxis[1][2]=viewp->rpaxis[2][0]*viewp->rpaxis[0][1]-viewp->rpaxis[2][1]*viewp->rpaxis[0][0];
    }
    /*
    **  Now twist the X- and Y-axes VIEWTWIST CW about the Z-axis.
    **  Note that, in the RP system, the new X-axis is
    **  (cos(t),-sin(t),0) and the new Y-axis is
    **  (sin(t),cos(t),0) (where t is VIEWTWIST):
    */
    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QVIEWTWIST,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
    } else {
        rb.restype=RTREAL; rb.resval.rreal=viewvarsp->viewtwist;
    }
    ar1=cos(rb.resval.rreal); ar2=sin(rb.resval.rreal);
    ap1[0]=ar1*viewp->rpaxis[0][0]-ar2*viewp->rpaxis[1][0];
    ap1[1]=ar1*viewp->rpaxis[0][1]-ar2*viewp->rpaxis[1][1];
    ap1[2]=ar1*viewp->rpaxis[0][2]-ar2*viewp->rpaxis[1][2];
    ap2[0]=ar2*viewp->rpaxis[0][0]+ar1*viewp->rpaxis[1][0];
    ap2[1]=ar2*viewp->rpaxis[0][1]+ar1*viewp->rpaxis[1][1];
    ap2[2]=ar2*viewp->rpaxis[0][2]+ar1*viewp->rpaxis[1][2];
    GR_PTCPY(viewp->rpaxis[0],ap1);
    GR_PTCPY(viewp->rpaxis[1],ap2);

                       /* viewsize, PixelHeight,PixelWidth, midwin */

    /*
    **  Watch out for order-dependency.  Many previously set viewp->
    **  members are used.
    **
    **  Assumes square pixels and that the upper left corner is (0,0).
    */
    if (viewvarsp==NULL) {
        if (configbuf==NULL) {
            rb.restype=RTPOINT;
            rb.resval.rpoint[0]=320.0;
            rb.resval.rpoint[1]=200.0;
        } else {
            if (db_getvar(NULL,DB_QSCREENSIZE,&rb,dp,configbuf,NULL)!=RTNORM)
                { rc=-3; goto out; }
        }
    } else {
        rb.restype=RTPOINT;
        rb.resval.rpoint[0]=viewvarsp->screensize[0];
        rb.resval.rpoint[1]=viewvarsp->screensize[1];
    }
    if ((fi1=(short)rb.resval.rpoint[0])==0 ||
        (fi2=(short)rb.resval.rpoint[1])==0) { rc=-6; goto out; }
    viewp->midwin[0]=fi1/2; viewp->midwin[1]=fi2/2;
    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QVIEWSIZE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
    } else {
        rb.restype=RTREAL; rb.resval.rreal=viewvarsp->viewsize;
    }
    if (icadRealEqual((viewp->viewsize=fabs(rb.resval.rreal)),0.0)) { rc=-7; goto out; }
    if (viewp->mode&1) {  /* Perspective */
        /* Assumes viewdist and midwin[] have been set. */
        if (viewvarsp==NULL) {
            if (db_getvar(NULL,DB_QLENSLENGTH,&rb,dp,configbuf,NULL)!=RTNORM)
                { rc=-3; goto out; }
        } else {
            rb.restype=RTREAL; rb.resval.rreal=viewvarsp->lenslength;
        }
        if (icadRealEqual(rb.resval.rreal,0.0)) rb.resval.rreal=50.0;
        else if (rb.resval.rreal<0.0) rb.resval.rreal=-rb.resval.rreal;
		viewp->SetPixelWidth( fabs(17.5*viewp->viewdist/rb.resval.rreal/viewp->midwin[0]) );
		viewp->SetPixelHeight( -viewp->GetPixelWidth() );


    } else {
		viewp->SetPixelHeight( -viewp->viewsize / fi2 );
		viewp->SetPixelWidth( -viewp->GetPixelHeight() );
    }

                              /* rp_ucs */

    /* (Assumes rpaxis[] and viewsize have been set. */
    viewp->rp_ucs=0; ar1=IC_ZRO*viewp->viewsize;
    if (fabs(1.0-viewp->rpaxis[0][0])<ar1 &&
        fabs(1.0-viewp->rpaxis[1][1])<ar1 &&
        fabs(1.0-viewp->rpaxis[2][2])<ar1) {

        viewp->rp_ucs=1;
        if (fabs(viewp->target[0])<ar1 &&
            fabs(viewp->target[1])<ar1 &&
            fabs(viewp->target[2])<ar1) viewp->rp_ucs=2;
    }

                            /* ltscale */

    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QLTSCALE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
    } else {
        rb.restype=RTREAL; rb.resval.rreal=viewvarsp->ltscale;
    }
    if ((viewp->ltscale=rb.resval.rreal)<=IC_ZRO) viewp->ltscale=1.0;


                   /* attmode, splframe, pdmode */

    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QATTMODE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->attmode=rb.resval.rint;
        if (db_getvar(NULL,DB_QSPLFRAME,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->splframe=rb.resval.rint;
        if (db_getvar(NULL,DB_QPDMODE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->pdmode=rb.resval.rint;
    } else {
        viewp->attmode =viewvarsp->attmode;
        viewp->splframe=viewvarsp->splframe;
        viewp->pdmode  =viewvarsp->pdmode;

    }


                              /* pdsize */
    /*
    **  PDSIZE is a little strange.  A positive value specifies the
    **  distance from the center to the tip of one of the "plus" arms
    **  (HALF the length of a "plus" arm).  A negative value specifies a
    **  percentage of VIEWSIZE -- but refers to the length of a WHOLE
    **  "plus" arm. Therefore, the true size is determined from a
    **  negative PDSIZE by -0.005*PDSIZE*VIEWSIZE.  (PDSIZE 0.0 is the
    **  same as -5.0.)
    **
    **  Note that we're setting viewp->pdsize to the TRUE size --
    **  not the value of the system variable.
    */
    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QPDSIZE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->pdsize=rb.resval.rreal;
    } else {
        viewp->pdsize=viewvarsp->pdsize;
    }
    if (icadRealEqual(viewp->pdsize,0.0)) viewp->pdsize=-5.0;
    if (viewp->pdsize<0.0) viewp->pdsize*=-0.005*viewp->viewsize;


           /* fillmode, elevation, qtextmode, ltsclwblk */

    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QFILLMODE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->fillmode=rb.resval.rint;
        if (db_getvar(NULL,DB_QELEVATION,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->elevation=rb.resval.rreal;
        if (db_getvar(NULL,DB_QQTEXTMODE,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->qtextmode=rb.resval.rint;
        if (db_getvar(NULL,DB_QLTSCLWBLK,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->ltsclwblk=rb.resval.rint;
    } else {
        viewp->fillmode =viewvarsp->fillmode;
        viewp->elevation=viewvarsp->elevation;
        viewp->qtextmode=viewvarsp->qtextmode;
        viewp->ltsclwblk=viewvarsp->ltsclwblk;
    }

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QLWDISPLAY,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->lwdisplay=rb.resval.rint;
        if (db_getvar(NULL,DB_QLWDEFAULT,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->lwdefault=rb.resval.rint;
	} else {
		viewp->lwdisplay=viewvarsp->lwdisplay;
		viewp->lwdefault=viewvarsp->lwdefault;
	}
	// EBATECH(CNBR) ]-

               /* curvdispqual (circle zoom percent) */

    if (viewvarsp==NULL) {
        if (db_getvar(NULL,DB_QZOOMPERCENT,&rb,dp,configbuf,NULL)!=RTNORM)
            { rc=-3; goto out; }
        viewp->curvdispqual=rb.resval.rint;
    } else {
        viewp->curvdispqual=viewvarsp->curvdispqual;
    }

                             /* CenterProjection */

	sds_point pointTemp;
    if (viewp->mode&1) {  /* Perspective */

        /* Project target onto the RP to get CenterProjection: */
        gr_ucs2rp(viewp->target,pointTemp,viewp);

    } else {  /* Orthogonal */

        /* The DCS viewctr IS CenterProjection: */

        if (viewvarsp==NULL) {
            if (dp==NULL || (headerbuf=dp->ret_headerbuf())==NULL ||
                db_qgetvar(DB_QVIEWCTR,headerbuf,pointTemp,DB_3DPOINT,0))
                    { rc=-3; goto out; }
        } else {
            GR_PTCPY(pointTemp,viewvarsp->viewctr);
        }

    }
    pointTemp[2]=viewp->target[2];  /* Needed? */
	viewp->SetCenterProjection( pointTemp );


out:
    return rc;
}


GR_API void gr_freeviewll(gr_view *vll) {
    gr_view *tvp;

    while (vll!=NULL) { tvp=vll->next; delete vll; vll=tvp; }
}



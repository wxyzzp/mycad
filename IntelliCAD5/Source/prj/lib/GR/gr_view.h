/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
**  View/display specification (set by gr_initview()).
**
**  Threading operation made it necessary to add to this
**  struct some stuff that used to be in a struct called gr_convdata
**  which had a global instance called gr_conv.  (See the members at
**  the bottom of this struct definition.)  gr_conv was created for the
**  benefit of gr_getdispobjs() and it's extension functions. The idea
**  was to avoid passing down a bunch of variables and duplicating them
**  on the stack when functions call functions that all need some of the
**  same variables.  Hopefully, there is a speed benefit.  Another
**  reason was the need for an expandable chain of points (the pc member
**  and its associates).  The functions that use this currently are
**  gr_getdispobjs() and functions that are extensions of that
**  function.  Now, we are putting this data into struct gr_view so
**  that it can be passed around with the original view data via
**  pointers.  Each thread has its own gr_view struct to work with, allegedly.
**  
**
**  IF YOU ADD OR REMOVE MEMBERS, BE SURE TO ADD OR REMOVE THE
**  CORRESPONDING INITIALIZERS IN THE DECLARATION OF gr_defview.
**
** History
** 24-03-2003 EBATECH(CNBR) Bugzilla#78471 Support Lineweight
*/

#ifndef __gr_viewH__
#define __gr_viewH__

#include "d3.h"
#include "matrix.h"
#include "clipper.h"

// *******************
// rectangle
// used for clipping and ops
//
struct gr_real_rectangle
	{
	sds_real xmin;
	sds_real xmax;
	sds_real ymin;
	sds_real ymax;
	};


/* For anything that needs a llist of strings */
struct gr_stringlink {
	char *str;
	struct gr_stringlink *next;
};

/*
**	This one is needed in struct gr_view below (for preserving a single
**	link) and in gr_getdispobjs() (for a llist of info about nested
**	INSERTs).  (Until we added a ptr to one of these in gr_view,
**	it was a local struct def in gr_getdispobjs().)
*/
struct db_insdatalink {
	db_handitem *hip;  /* INSERT hip, for the nentselp stuff. */
					   /*	(Could replace some members below -- but I'm */
					   /*	not rocking the boat now! */
	sds_point axis[3]; /* The ECS axes */
	sds_point blkbase;
	sds_point ipt;
	sds_point scl;	/* 0=Xscale; 1=Yscale; 2=Zscale */
	sds_real  cs;	/* cos(rot) */
	sds_real  sn;	/* sin(rot) */

	db_handitem *layhip;
	db_handitem *glhip;  /* The layer this INSERT is GENERATED on */
	short		 color;
	short		 lweight; /* EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight */
	db_handitem *lthip;

	struct db_insdatalink *next;
};

/*
**	For gr_initselector() and gr_selected().  The freer is
**	gr_freeselector().	2D geometry (RP), despite the RT3DPOINT restypes.
*/
struct gr_selectorlink {
	char				type;	/* 'C'ircle, 'F'ence, poly'G'on, 'P'oint,	*/
								/*	 'W'indow								*/
								/*											*/
	char				mode;	/* 'I'nside, 'C'rossing, 'O'utside			*/
								/*	 (See gr_initselector() for the exact  */
								/*	 meanings for each type.)				*/
								/*											*/
	struct ic_pointsll *def;	/* Llist specifying defining the selector	*/
								/*	 (defined like this so that polyGon can */
								/*	 use ic_inorout()): 					*/
								/*	   Circle : Center is def->pt.			*/
								/*				Radius is def->next->pt[0]	*/
								/*		Fence : 2 or more pts				*/
								/*	  polyGon : 3 or more pts				*/
								/*		Point : 1 pt						*/
								/*	   Window : 2 pts (ll/ur)				*/
								/*											*/
	sds_point		   *extent; /* Allocated by gr_initselector().	When   */
								/*	 not NULL, an array of 2 sds_points:	*/
								/*	 lower left (extent[0]) and upper right */
								/*	 (extent[1]) corners of the extents box */
								/*	 for this selector. (Used for initial	*/
								/*	 quick checks.) 						*/
};

struct GR_CLASS gr_view {

	// **************
	// CTORS and DTORS
	//
	public:
	gr_view();
	gr_view(const gr_view &sour);
	~gr_view();


    // *********************************
   // PUBLIC ACCESSORS
   //
	public:
		void SetVPortClipRectangle( sds_point ptmin, sds_point ptmax )
			{
			m_VPortClipRect.xmin = ptmin[0];
			m_VPortClipRect.ymin = ptmin[1];
			m_VPortClipRect.xmax = ptmax[0];
			m_VPortClipRect.ymax = ptmax[1];
			m_bVPortClip = true;

			}
		void ClearVPortClipRectangle( void )
			{
			m_bVPortClip = false;
			}
		const inline gr_real_rectangle &GetClipRectangle( void )
			{
			if ( m_bVPortClip )
				{
				return m_VPortClipRect;
				}
			else
				{
				return m_ClipRect;
				}
			}

		inline void SetCenterProjection( sds_point newvalue )
			{
				GR_PTCPY( m_CenterProjection, newvalue );
				CalcClipRectangle();
			}
		inline sds_real GetCenterProjectionX( void )
			{
				return m_CenterProjection[0];
			}
		inline sds_real GetCenterProjectionY( void )
			{
				return m_CenterProjection[1];
			}
		inline void GetCenterProjection( sds_point returnvalue )
			{
				GR_PTCPY( returnvalue, m_CenterProjection );
			}
		inline sds_real GetPixelHeight( void )
			{
				return m_PixelHeight;
			}
		inline sds_real GetPixelWidth( void )
			{
				return m_PixelWidth;
			}
		inline sds_real GetReciprocalOfPixelHeight( void )
			{
				if (!m_ReciprocalOfPixelHeight)
				{
					if (icadRealEqual(m_PixelHeight,0.0))
						{
						m_ReciprocalOfPixelHeight = 1.0;
						}
					else
						{
						m_ReciprocalOfPixelHeight = 1 / m_PixelHeight;
						}  
				}

				return m_ReciprocalOfPixelHeight;
			}
		inline sds_real GetReciprocalOfPixelWidth( void )
			{
				if (!m_ReciprocalOfPixelWidth)
				{
					if (icadRealEqual(m_PixelWidth,0.0))
						{
						m_ReciprocalOfPixelWidth = 1.0;
						}
					else
						{
						m_ReciprocalOfPixelWidth = 1 / m_PixelWidth;
						}
				}

				return m_ReciprocalOfPixelWidth;
			}

		void SetPixelHeight( sds_real newvalue )
			{
				m_PixelHeight = newvalue;

				// We also calculate the reciprocal to avoid division later
				//
				if (icadRealEqual(m_PixelHeight,0.0))
					{
					m_ReciprocalOfPixelHeight = 1.0;
					}
				else
					{
					m_ReciprocalOfPixelHeight = 1 / m_PixelHeight;
					}  
				CalcClipRectangle();

			}
		void SetPixelWidth( sds_real newvalue )
			{
				m_PixelWidth = newvalue;

				// We also calculate the reciprocal to avoid divisions later
				//
				if (icadRealEqual(m_PixelWidth,0.0))
					{
					m_ReciprocalOfPixelWidth = 1.0;
					}
				else
					{
					m_ReciprocalOfPixelWidth = 1 / m_PixelWidth;
					}
				CalcClipRectangle();
			}
		// Helper methods
		private:
			void CalcClipRectangle( void )
				{
					sds_real rHalfScreen;

					rHalfScreen=::fabs( (this->midwin[0] - 1) * this->GetPixelWidth() );
					m_ClipRect.xmin = this->GetCenterProjectionX() - rHalfScreen;
					m_ClipRect.xmax = this->GetCenterProjectionX() + rHalfScreen;

					rHalfScreen=::fabs( (this->midwin[1] - 1) * this->GetPixelHeight() );
					m_ClipRect.ymin = this->GetCenterProjectionY() - rHalfScreen;
					m_ClipRect.ymax = this->GetCenterProjectionY() + rHalfScreen;
				}
	// *********************
	// PRIVATE MEMBERS
	//
	private:  // Note, these were public!
    sds_point m_CenterProjection;   /* Proj plane pt that maps to center of window */
                          /*   (projection of VIEWCTR). */
	gr_real_rectangle	m_ClipRect;

    sds_real  m_PixelHeight;      /* Height and width of a pixel in proj plane units */
    sds_real  m_PixelWidth;		  /*   (CAN BE NEG).  (In orthogonal, they're from */
                          /*   SCREENSIZE and VIEWSIZE, assuming upper left */
                          /*   is (0,0); in perspective, they're from */
                          /*   LENSLENGTH, viewdist, and SCREENSIZE). */

	// To avoid making divisions later, save 1 / m_PixelHeight and multiply 
	//
	sds_real	m_ReciprocalOfPixelHeight;
	sds_real	m_ReciprocalOfPixelWidth;


	// *****
	// Used for paperspace clipping
	//
	gr_real_rectangle	m_VPortClipRect;
	bool				m_bVPortClip;

	// ********************
	// PUBLIC MEMBERS
	//
	// !!!!!!!!!!!! NOTE PUBLIC !!!!!!!!!
	public:  // Note was struct, so need public (for now)


    short     mode;       /* VIEWMODE (perspective is bit 0 (1)) */

    short     noucs2rp;   /* See p_mode==2 in gr_getdispobjs(). */
    short     isprinting; /* A flag to let gr_getdispobjs() know that  */
                          /*   printing is occurring so that it can    */
                          /*   avoid generating stuff on the DEFPOINTS */
                          /*   layer.                                  */
    sds_point target;     /* TARGET (UCS) */
    sds_point camera;     /* Camera point TARGET+VIEWDIR (UCS) */
    sds_point rpaxis[3];  /* Real projection plane axes according to */
                          /*   the current UCS (target is the origin):   */
                          /*     0 : Unit X-axis */
                          /*     1 : Unit Y-axis */
                          /*     2 : Unit Z-axis (unit VIEWDIR) */
    short     rp_ucs;     /* Used in creating short cuts for RP/UCS        */
                          /*   transformations:                            */
                          /*     0 : No short cuts                         */
                          /*     1 : RP and UCS differ only in translation */
                          /*           (TARGET is not (0,0,0)).            */
                          /*     2 : RP and UCS are identical.             */
    sds_real  viewdist;   /* Length of VIEWDIR */
    sds_real  viewsize;   /* VIEWSIZE */
    int       midwin[2];  /* Pixel coords of the center of the window */
                          /*   (from SCREENSIZE, assuming upper left is (0,0)). */

    sds_real  ltscale;    /* LTSCALE */
    short     attmode;    /* ATTMODE */
    short     splframe;   /* SPLFRAME */
    short     pdmode;     /* PDMODE */
    sds_real  pdsize;     /* PDSIZE (set to appropriate positive TRUE size) */
    short     fillmode;   /* FILLMODE */
    sds_real  elevation;  /* ELEVATION */
    short     qtextmode;  /* QTEXTMODE */
    short     ltsclwblk;  /* Our own: LineType SCaLes With BLocK */
    int       curvdispqual; /* Circle zoom percent */
	short     lwdisplay;  /* LWDISPLAY Heavy lines draws bold. */
	short     lwdefault;  /* LWDEFAULT Actual Weight for DB_DFWEIGHT (Plotting) */
	                      /* LWDEFAULT Boundary weight. (Displaying) */

    sds_point ucsorg;     /* UCSORG */
    sds_point ucsaxis[3]; /* The current UCS axes (according to the WCS): */
                          /*   0 : Unit UCSXDIR */
                          /*   1 : Unit UCSYDIR */
                          /*   2 : Unit Z-axis determined from the other 2 */


    /*
    ** Okay.  Here's the stuff from the former struct gr_convdata
    ** (minus the struct gr_view ptr that gr_convdata used to have).
    ** There are also some new ones I created later.
    ** (See comments above.)
    */

    /* Variables that gr_getdispobjs() is currently dealing with: */

    short            projmode;   /* 0=UCS (3D); 1=orthogonal; 2=central */
    short            dothick;    /* Whether or not to add thickness */
    short            dowid;      /* Whether or not the pc chain has non-0 */
                                 /*   width somewhere along it */

	// thickness vector according to the UCS
	sds_point m_thickness;
	// transformation matrix from NCS to UCS
	CMatrix4 m_transformation;
                                 /*   WARNING: only set up when needed     */
                                 /*   (which is currently just for 2D      */
                                 /*   POLYLINES with width).               */
	// transformation matrix from UCS to NCS
	CMatrix4 m_transformationInverse;

    short            ncs_ucs;    /* Used in creating short cuts for NCS/UCS */
                                 /*   transformations:                      */
                                 /*     0 : No short cuts                   */
                                 /*     1 : NCS and UCS differ only in      */
                                 /*           translation.                  */
                                 /*     2 : NCS and UCS are identical.      */
    sds_real         absxsclp;   /* Absolute value of the cumulative INSERT */
                                 /*   X-scale product for entities in */
                                 /*   (possibly nested) blocks (used in */
                                 /*   determining circle/arc resolution -- */
                                 /*   and in a temporary POLYLINE width */
                                 /*   solution). */
	sds_real		 absysclp;	 // Absolute value of the cumulative INSERT Y scale
	sds_real		 absrot;	 // Absolute value of the cumulative INSERT rotation
    db_handitem     *ment;       /* Handitem for the current MAIN entity */
                                 /*                                        */
                                 /* The following came about because       */
                                 /*   certain new entities have elements   */
                                 /*   that have linetypes and colors that  */
                                 /*   differ from those for the whole      */
                                 /*   entity: TOLERANCE, MTEXT, MLINE,     */
                                 /*   LEADER.                              */
                                 /*                                        */
                                 /*   WARNING: gr_getdispobjs() bothers to */
                                 /*   set these only for the entities that */
                                 /*   need them; for all others, their     */
                                 /*   value is 7.                          */
                                 /*                                        */
    short          bbcolor;      /*   Resolved BYBLOCK color.              */
    short          blcolor;      /*   Resolved BYLAYER color.              */
    db_handitem   *bblthip;      /*   Resolved BYBLOCK linetype.           */
    db_handitem   *bllthip;      /*   Resolved BYLAYER linetype.           */

    short            color;      /* Actual (resolved) color for the display */
                                 /*   objects. */
    db_handitem     *lthip;      /* Actual (resolved) linetype for */
                                 /*   display object generation */
                                 /*   (NULL means CONTINUOUS). */
    short          lweight;      /* lineweight for the display objects. (0-211 and Default) */
    short          bblweight;    /* Resolved BYBLOCK (for MTEXT) */
    short          bllweight;    /* Resolved BYLAYER (for LEADER,MTEXT) */

    /*
    **  The following are for determining the UCS extent of ONE entity
    **  in gr_getdispobjs().  That function sets ucsextmode to the
    **  appropriate value (-1 or 0) and then picks up the result in
    **  ucsext[] before it returns.  The work is done by gr_ncs2ucs()
    **  as points are transformed to the UCS prior to projection onto
    **  the RP.
    */
    short            ucsextmode; /* -1 : Don't bother */
                                 /*  0 : Processing 1st pt */
                                 /* +1 : Processing pts after the 1st */
    sds_point        ucsext[2];

    /*
    **  The following are for determining the UCS extent of ONE clipped 
	**  block insert or xref in gr_getdispobjs().  That function sets 
	**  m_ClipUcsExtMode to the appropriate value (-1 or 0) and then picks 
	**  up the result in m_ClipUcsExt[] before it returns.  
    */
    short            m_ClipUcsExtMode;	/* -1 : Don't bother */
										/*  0 : Processing 1st pt */
										/* +1 : Processing pts after the 1st */
	sds_point        m_ClipUcsExt[2];

    /*
    **  An expandable llist of points (PointChain) set up by
    **  gr_getdispobjs() specifying a chain of points in the current
    **  entity's NCS or the UCS (depending on when gr_ncs2ucs() is
    **  performed, which depends on LTSCLWBLK).  (.xfp has been
    **  transformed properly to deal with the appropriate NCS
    **  in gr_ncs2ucs().)  DEFINES ONE ENTITY AND CLOSES BY HAVING THE
    **  LAST POINT COINCIDENT WITH THE FIRST (FOR THE BENEFIT OF
    **  gr_doltype()) AND gr_gensolid().  gr_doltype() uses this
    **  llist as a template while generating linetype.
    */
    struct gr_pclink *pc;      /* First link */
    struct gr_pclink *lapc;    /* Last link allocated */
    struct gr_pclink *lupc;    /* Last link used.  Used primarily for */
                                /*   building in stages the chain for a */
                                /*   whole POLYLINE.  Should be */
                                /*   maintained in all cases, however, */
                                /*   because gr_doltype() uses it. */
    long               npc;     /* Number of links allocated */
    long               upc;     /* Number of links currently used */

    /*
    **  The following are used for communication between gr_nentselphelper()
    **  and gr_getdispobjs() to get the exotic things that sds_nentselp()
    **  needs.
    */
    /* Given to gr_getdispobjs(): */
    struct gr_selectorlink *nesp_sl;  /* The selector. */

    /* Set by gr_getdispobjs(): */
    db_handitem     *nesp_innerhip;   /* Ptr to the block def entity selected. */
                                      /*   Also used as a flag that we found it */
    sds_point       *nesp_entaxis;    /* The entity system from gr_getdispobjs() */
    resbuf          *nesp_refstkres;  /* Llist of enames of the INSERTs -- */
                                      /*   innermost to outermost. */

    /* Used by gr_getdispobjs() when needed: */
    db_handitem     *nesp_shuttleline;  /* A dummy LINE used for shuttling */
                                        /* disp objs to gr_selected(). */

    /*
    **  This is a pointer to a SINGLE link (not a llist) -- the data
    **  for the most recent INSERT encountered by gr_getdispobjs().
    **  For the most part, ATTRIBs behave like any other entities
    **  (dissociated from the INSERT) -- except concerning color
    **  (and I'm sure linetype, if ATTRIBs had linetype), where the
    **  rules need to be applied just as if the ATTRIB were in
    **  the block def.
    **
    **  This pointer makes it possible to add the INSERT's data
    **  back to the llist of nested INSERT data just long enough
    **  to make the ATTRIB's color determination (but NOT long enough
    **  to affect the transformation stuff).
    */
    struct db_insdatalink *previnsdata;

    /*
    **  This one's similar.  If the INSERT's generation layer is frozen,
    **  no entities in the block def are drawn -- and neither are
    **  the ATTRIBs.
    */
    int previnsfrozen;

    /*
    **  These used to be statics in gr_getdispobjs(). All of them
    **  currently deal with recursion in some manner (although I may I
    **  some in the future that don't).
    */
    short           gdo_rc;				/* The ultimate return code. */
    short           recurlevel;			/* Recursion level counter. */
    long            errln;				/* Error line number (debugging). */
    db_insdatalink *idll;				/* Recursion llist stack of INSERT data for transformations. */
    gr_stringlink  *xrefpnll;			/* Recursion llist stack of xref pathnames. */

	CClipper		m_Clipper;			// current clipper for inserts (from spatial_filter object)

    gr_view *next;

	/***************************************************************************************
	** Author:		Denis Petrov
	** Description: Helper function to allocate temporary data. Makes sure that llist 
	**				pc is allocated for at least nwanted links, sets upc to nwanted.
	** Return: 0 if OK and -1 if no free memory available
	***************************************************************************************/
	short allocatePointsChain(long nwanted);

	/***************************************************************************************
	** Author:		Denis Petrov
	** Description: Helper function to remove temporary data.
	***************************************************************************************/
	void freeInsertList();
	void freeXrefPathList();
	void freePointsChain();

	bool isCoplanarToScreen(db_entity_with_extrusion *ent);
};

inline void gr_view::freeInsertList()
{
	db_insdatalink* pInsert;
	while(idll != NULL)
	{
		pInsert = idll->next;
		delete idll;
		idll = pInsert;
	}
}

inline void gr_view::freeXrefPathList()
{
	gr_stringlink* pXrefPath;
	while(xrefpnll != NULL)
	{
		if(xrefpnll->str)
			IC_FREE(xrefpnll->str);
		pXrefPath = xrefpnll->next;
		delete xrefpnll;
		xrefpnll = pXrefPath;
	}
}

/*
**  This struct was created so that gr_initview could be called
**  with view parameters from some source other than the system
**  variables.  (The alternative was to save the pertinent sysvars,
**  change them temporarily, call gr_initview(), and then set them
**  back to what they were.  Way too much BS.)
*/
struct gr_viewvars {
    sds_point ucsorg;
    sds_point ucsxdir;
    sds_point ucsydir;
    short	  viewmode;
    sds_point target;
    sds_point viewdir;
    sds_real  viewtwist;
    sds_point screensize;
    sds_real  viewsize;
    sds_real  lenslength;
    sds_real  ltscale;
    short	  attmode;
    short	  splframe;
    short	  pdmode;
    sds_real  pdsize;
    short	  fillmode;
    sds_real  elevation;
    short	  qtextmode;
    short	  ltsclwblk;
	short	  lwdisplay;
	short	  lwdefault;
    sds_point viewctr;
    int       curvdispqual;  /* Circle zoom percent */
};
#endif

/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "gr.h"

int gr_entity_nentsel_3d(
  /* Given (all required): */
    db_entity		 *pParentEntity,
    gr_selectorlink  *pSelector,
	gr_view			 *pView,
  /* Results */
    db_handitem     **innerhipp)
	{
	*innerhipp = NULL;
	gr_displayobject *pCurrent = (gr_displayobject *)pParentEntity->ret_disp();
	// Selection mode for nentsel is always WINDOW, CROSSING
	for(; pCurrent && !*innerhipp; pCurrent = pCurrent->next)
	{
		ASSERT(pCurrent->type&'\x01');
		if(!pCurrent->chain || pCurrent->npts < 1)
			continue;

		if(pCurrent->npts < 2)
		{
			ASSERT( pCurrent->npts == 1 );
			CD3 proj;
			gr_ucs2rp(pCurrent->chain, proj, pView);
			if ( (proj[0] >= pSelector->extent[0][0]) &&
				 (proj[0] <= pSelector->extent[1][0]) &&
				 (proj[1] >= pSelector->extent[0][1]) &&
				 (proj[1] <= pSelector->extent[1][1]) )
				{
				*innerhipp = pCurrent->GetSourceEntity();
				ASSERT( *innerhipp != NULL );
				}
		}
		else
		{
			CD3 projPrev, projCur;
			gr_ucs2rp(pCurrent->chain, projPrev, pView);
			for(int iIndex = 3, i = 1; i < pCurrent->npts; ++i, iIndex += 3, projPrev = projCur)
			{
				gr_ucs2rp(pCurrent->chain + iIndex, projCur, pView);
				if(gr_segxbox(projPrev, projCur, pSelector->extent))
				{
					*innerhipp = pCurrent->GetSourceEntity();
					ASSERT( *innerhipp != NULL );
					break;
				}
			}

			// If closed, check the segment back to the origin
			if(pCurrent->type & 2) // closed
			{
				gr_ucs2rp(pCurrent->chain, projCur, pView);
 	            if(gr_segxbox(projPrev, projCur, pSelector->extent))
				{
					*innerhipp = pCurrent->GetSourceEntity();
					ASSERT( *innerhipp != NULL );
				}
			}
		} // if
	}// for


    // Bug 1-47183 fix. nentsel should return the first vertex for db_polyline.
    if (*innerhipp != NULL && (*innerhipp)->ret_type() == DB_POLYLINE)
        *innerhipp = (*innerhipp)->next;
	return 0;
}


int gr_entity_nentsel(
  /* Given (all required): */
    db_entity		 *pParentEntity,
    gr_selectorlink  *pSelector,
	gr_view			 *pView,
	/* Results */
    db_handitem     **innerhipp )
	{

	*innerhipp = NULL;

	gr_displayobject *pCurrent = (gr_displayobject *)pParentEntity->ret_disp();
	const int ptDim = pCurrent ? (pCurrent->type & DISP_OBJ_PTS_3D ? 3 : 2) : 0;

	if(pCurrent && (pCurrent->type & DISP_OBJ_PTS_3D))
		return gr_entity_nentsel_3d(pParentEntity, pSelector, pView, innerhipp);

	// Selection mode for nentsel is always WINDOW, CROSSING
	//
	while( (pCurrent != NULL) &&
		   (*innerhipp == NULL) )
		{
		if (pCurrent->type&'\x01' || 
			pCurrent->chain==NULL ||
			pCurrent->npts < 1) 
			{
			pCurrent = pCurrent->next;
			continue;
			}

		if ( pCurrent->npts < 2 )
			{
			ASSERT( pCurrent->npts == 1 );
			if ( (pCurrent->chain[0] >= pSelector->extent[0][0]) &&
				 (pCurrent->chain[0] <= pSelector->extent[1][0]) &&
				 (pCurrent->chain[1] >= pSelector->extent[0][1]) &&
				 (pCurrent->chain[1] <= pSelector->extent[1][1]) )
				{
				*innerhipp = pCurrent->GetSourceEntity();
				ASSERT( *innerhipp != NULL );
				}
			}
		else
			{
			int iLastIndex = 2 * (pCurrent->npts - 1);
			/*
			Note by Pieter Clarysse, nov 30 1999.
			gr_segxbox () wants 2 2d-points as the first 2 arguments.
			We have in pCurrent->chain an array of iLastIndex+2 sds_reals.
			So, in this for-loop iIndex jumps form 0,2,4,... till iLastIndex-2.
			Inside the loop we use iIndex and iIndex+2, so we use the array
			from 0 till iLastIndex. In gr_sgxbox, the last sds_real accessed is 
			pCurrent->chain[iLastIndex+1]. This used to be 
			pCurrent->chain[iLastIndex+2] in the previous implementation. (iIndex
			running from 0,1,2,...till iLastIndex-1)
			*/
			for( int iIndex = 0; iIndex < iLastIndex; iIndex+=2 )
				{
	            if (gr_segxbox( (pCurrent->chain + iIndex),
							    (pCurrent->chain + iIndex + 2),
								pSelector->extent ))
					{
					*innerhipp = pCurrent->GetSourceEntity();
					ASSERT( *innerhipp != NULL );
					break;
					}
				}

			// If closed, check the segment back to the origin
			//
	        bool bClosed=( (pCurrent->type & 2) != 0 );
			if ( bClosed )
				{
 	            if (gr_segxbox( (pCurrent->chain + iLastIndex - 2),
							    (pCurrent->chain + 0),
								pSelector->extent ))
					{
					*innerhipp = pCurrent->GetSourceEntity();
					ASSERT( *innerhipp != NULL );
					}
				}
			}
		pCurrent = pCurrent->next;
		} // end of while loop

    // Bug 1-47183 fix. nentsel should return the first vertex for db_polyline.
    if (*innerhipp != NULL && (*innerhipp)->ret_type() == DB_POLYLINE)
        *innerhipp = (*innerhipp)->next;

	return 0;
	}


GR_API int gr_nentselphelper(
  /* Given (all required): */
    int               mode,
    db_handitem      *outerhip,
    gr_selectorlink  *sl,
    gr_view          *viewp,
    db_drawing       *dp,
	CDC				 *dc,
  /* Results (NULL okay for unwanted ones): */
    db_handitem     **innerhipp,
    sds_matrix        xformres,
    resbuf          **refstkrespp) {

/*
**  This is a helper-function for sds_nentselp().  This function is
**  expecting to be called from sds_nentselp() after that function
**  has determined the "outer-most" entity (outerhip -- the
**  entity actually inserted into the drawing (which could be an INSERT)),
**  and after it has set up the selector (sl) for the pick-box.
**  The current view and drawing are the usual: viewp and dp.
**
**  The results depend on what type of entity outerhip refers to,
**  and the value of mode.
**
**    *innerhipp:
**
**      outerhip is an INSERT:     The selected block def entity (a POLYLINE,
**                                   if the block def entity is a POLYLINE --
**                                   not a VERTEX yet.  We've got problems
**                                   with that).
**
**      outerhip is a POLYLINE:    The POLYLINE entity -- not a VERTEX yet.
**                                   We've got problems with that.
**
**      outerhip is anything else: Just outerhip itself.
**
**      Nothing selected:          NULL.
**
**    xformres (See coordinate system definitions below):
**
**      mode==0 (ACAD's way):
**
**        BCS to WCS matrix (which means the identity matrix for any
**          outerhip except an INSERT).
**
**      mode==1 (for us):
**
**        trueNCS to UCS matrix.
**
**      If nothing selected, the identity matrix.
**
**    *refstkres:
**
**      Llist of entity names of the INSERTs, from the inside out.
**        (NULL for any outerhip except an INSERT.)
**        CALLER MUST FREE (sds_relrb()) if allocated for the caller (non-NULL).
**
**
**  COORDINATE SYSTEMS
**
**    trueNCS:
**
**      The Native Coordinate System for an entity (WCS or ECS, depending
**      on the entity type).  "Entget" coordinates are trueNCS.
**
**    gdoNCS:
**
**      This is the Native Coordinate System ACCORDING TO gr_getdispobjs().
**      What's the difference?
**      gr_getdispobjs() treats some WCS entities as if they are ECS --
**      planar entities that ACAD saw fit to define in WCS coordinates,
**      but that gr_getdispobjs() treats as ECS to simplify things.
**      Currently, these are ELLIPSE, POINT, MLINE, MTEXT, and TOLERANCE.
**      (You might think that LEADER would fall into this group, too,
**      but it doesn't: gr_getdispobjs() didn't need to use the ECS approach
**      for this one.)
**
**      gr_ncs2ucs(), viewp->xfp, and viewp->nesp_entaxis all deal with
**      the gdoNCS -- because gr_getdispobjs() sets them up.
**      viewp->xfp is the gdoNCS according to the UCS.
**      viewp->nesp_entaxis is the gdoNCS according to the BCS.
**
**      When trueNCS and gdoNCS are different, trueNCS==WCS and
**      gdoNCS==ECS.
**
**    BCS:
**
**      The Block Coordinate System.  (ACAD calls it the Model Coordinate
**      system (MCS)).  For an entity in a block definition, the entity's
**      coordinates are based on its location in the WCS at the time
**      the block was defined.  Thats the BCS.  Another way to look at it:
**
**        For an ECS (planar) entity in a block def, if you transform its
**        points from ECS to WCS, you get its points in the BCS.
**
**        A WCS entity in a block def already HAS its points in the BCS.
**
**
**  HOW TO USE xformres
**
**    mode==0
**
**      This is the BCS to WCS mode that ACAD uses.  To transform
**      a point in a planar entity in a block def, do the following:
**
**        sds_trans() from ECS to WCS using the entity's extrusion
**          vector.  This generates a BCS point.
**
**        Use xformres to go from BCS to WCS.
**
**        sds_trans() from WCS to UCS (if needed).
**
**      A WCS entity like LINE has BCS points already, so skip the
**      first step listed above; just do the last two.
**
**      This xformres is more trouble to use -- BUT -- it's independent of the
**      current UCS and it's the same for all entities at the
**      same nesting level in a given INSERT.
**
**      This one's the best if you're going to do lots of transformations
**      for a number of entities at the same nesting level in a
**      given INSERT.
**
**    mode==1
**
**     This is a mode for us to use internally.  Why go trueNCS->BCS->WCS->UCS
**     when we can set xformres to go trueNCS->UCS in one transformation?
**     This is the way gr_getdispobjs() works; it uses viewp->xfp[]
**     (the "distorted gdoNCS axes") which essentially IS the matrix
**     (except when gdoNCS!=trueNCS).
**
**     Just use xformres to go from trueNCS to UCS.
**
**     This xformres is easier to use -- BUT -- it is UCS-dependent and
**     it varies from entity to entity even within the same
**     nesting-level in a given INSERT.
**
**     This one's the best choice when you do transformations for a
**     single entity inside a block def.
**
**
**  HOW THIS FUNCTION WORKS
**
**     This function uses viewp to communicate with gr_getdispobjs().
**     It sets some members of viewp and calls gr_getdispobjs() with
**     outerhip.  In gr_getdispobjs(), as each block def entity's
**     display objects are obtained, gr_selected() is called to see if
**     that entity is selected. If so, the necessary members in viewp
**     are set and gr_getdispobjs() returns to this function, which
**     retrieves the results from viewp.
**
**     That's the short story.  See gr_getdispobjs() and struct gr_view
**     to figure out the details.
**
**
**  Returns:
**       0 : OK (but check *innerhipp to see if anything was selected).
**      -1 : Calling error.
**      -2 : An error occurred in gr_getdispobjs().
*/
    int outertype,innertype,rc,fi1,fi2,fi3;
    sds_point workcs[4],ap1,ap2;
    resbuf ucsrb,wcsrb;
    db_handitem *thip1;
    gr_displayobject *tdop1,*tdop2;


    rc=0;
    memset(workcs,0,sizeof(workcs));
    workcs[1][0]=workcs[2][1]=workcs[3][2]=1.0;

    wcsrb.rbnext=ucsrb.rbnext=NULL;
    wcsrb.restype=ucsrb.restype=RTSHORT;
    wcsrb.resval.rint=0; ucsrb.resval.rint=1;


    /* Init results (for safety): */
    if (innerhipp!=NULL) *innerhipp=NULL;
    if (refstkrespp!=NULL) *refstkrespp=NULL;
    if (xformres!=NULL) {
        memset(xformres,0,sizeof(sds_matrix));
        xformres[0][0]=xformres[1][1]=xformres[2][2]=xformres[3][3]=1.0;
    }

    /* Check the "given" pass parameters: */
    if (mode<0 || mode>1 || outerhip==NULL || sl==NULL || viewp==NULL ||
        dp==NULL) { rc=-1; goto out; }

    outertype=outerhip->ret_type();

	// ****************************
	// NENTSEL FOR POLYLINES
	// 
	// !!!!!!!!!!!!!!
	//
	// TODO Actually this will work for blocks and inserts too, but it
	// needs to be tested and a few quirks worked out (plus it needs help
	// getting the xform matrix).
	//
	if ( outertype == DB_POLYLINE )
		{
		return gr_entity_nentsel( (db_entity *)outerhip,
									sl, viewp,
									innerhipp );
		}


    /* If it's not an INSERT or a DIMENSION and we're doing ACAD mode, */
    /* set innerhip to outerhip and we're done: */
    if (outertype!=DB_INSERT && outertype!=DB_DIMENSION && !mode) {
        if (innerhipp!=NULL) *innerhipp=outerhip;
        goto out;
    }



    /* Init the viewp->nesp members: */
    viewp->nesp_sl=sl;
    viewp->nesp_innerhip=NULL;
    if( viewp->nesp_entaxis )
	{
		delete [] viewp->nesp_entaxis;
		viewp->nesp_entaxis=NULL;
	}
    if (viewp->nesp_refstkres!=NULL)
        { sds_relrb(viewp->nesp_refstkres); viewp->nesp_refstkres=NULL; }

    /* Call gr_getdispobjs() to get the data: */
    tdop1=tdop2=NULL;
    thip1=outerhip;  /* Don't let gr_getdispobjs() step outerhip itself. */
    if (gr_getdispobjs(dp,NULL,&thip1,&tdop1,&tdop2,NULL,dp,viewp,1,dc))
        { rc=-2; goto out; }

    /* This gr_getdispobjs() call should NOT be returning with any */
    /* display objects, but, for safety... */
    if (tdop1!=NULL) { gr_freedisplayobjectll(tdop1); tdop1=tdop2=NULL; }

    if ( viewp->nesp_innerhip==NULL || 
		 viewp->nesp_entaxis==NULL)
		{

		// If the above method of looking for an internal entity failed, and we
		// are looking at an insert, maybe we should be getting an ATTRIB entity
		//
		if ( outertype == DB_INSERT )
			{

			db_insert *pInsertEntity = (db_insert *)outerhip;

			// Does this insert have attribs?
			//
			if ( pInsertEntity->retAttribsFollow() )
				{

				// If there are ATTRIBS, they follow the insert
				//
				db_entity *pNextEntity = (db_entity *)pInsertEntity->next;
				while( (pNextEntity != NULL) &&
					   (pNextEntity->ret_type() == DB_ATTRIB) )
					{
					if ( !pNextEntity->ret_deleted() )
						{

						// Use this method to find the actual entity 
						//
						rc = gr_entity_nentsel( pNextEntity,
												sl, viewp,
												innerhipp );
						if ( *innerhipp != NULL )
							{
							break;
							}
						}
					pNextEntity = (db_entity *)pNextEntity->next;
					}
				}
			}
		goto out;
		}

    innertype=viewp->nesp_innerhip->ret_type();

    /* At this point, viewp->xfp[] should be set for the inner entity, */
    /* since gr_getdispobjs() bailed once it generated that entity's */
    /* display objects and discovered that they were "selected". */

    /* Pick up the results from viewp: */
    if (innerhipp!=NULL) {
        *innerhipp=viewp->nesp_innerhip;
        viewp->nesp_innerhip=NULL;  /* Precaution. */
    }
    if (refstkrespp!=NULL) {
        *refstkrespp=viewp->nesp_refstkres;
        viewp->nesp_refstkres=NULL;  /* IMPORTANT: we're stealing it. */
    }
    if (xformres!=NULL) {  /* Set xformres. */
        /*
        **  Set up workcs so that it defines the BCS according to
        **  the WCS (mode 0) or the trueNCS according to the UCS
        **  (mode 1).  With workcs (as with viewp->xfp), note the
        **  following definitions:
        **
        **    [0] : Origin pt
        **    [1] : X-axis vector
        **    [2] : Y-axis vector
        **    [3] : Z-axis vector
        */
        if (mode==0) {  /* BCS to WCS */
            /*
            **  This takes some thought.  viewp->xfp is the gdoNCS
            **  according to the UCS.  viewp->nesp_entaxis is the gdoNCS
            **  according to the BCS.  We want to set up workcs so
            **  that it represents the BCS according to the WCS.
            **
            **  We can use viewp->xfp and viewp->entaxis to
            **  set workcs in the following manner:
            **
            **    First, we need the BCS according to the gdoNCS of
            **    the inner entity.  viewp->nesp_entaxis is
            **    the key: it's the gdoNCS according to the BCS.
            **    The transpose is the BCS according to the gdoNCS.
            **
            **    Next, transform the BCS system from gdoNCS to UCS via
            **    gr_ncs2ucs().  (Note that the axes are not necessarily
            **    unit length, nor are they mutually perpendicular
            **    after this.  The vectors represent the BCS unit axes
            **    AS VIEWED BY the UCS.)
            **
            **    Then, transform the BCS system from UCS to WCS via
            **    sds_trans().
            */

            /* We can do this one point or vector at a time: */

          /* BCS origin (which coincides with the gdoNCS origin) */
          /* according to the UCS: */
			ap1[0] = viewp->m_transformation(0,3);
			ap1[1] = viewp->m_transformation(1,3);
			ap1[2] = viewp->m_transformation(2,3);

          /* BCS origin according to the WCS: */
            dp->trans(ap1,&ucsrb,&wcsrb,0,workcs[0],NULL);

          /* BCS X-axis according to the gdoNCS (from transposing entaxis): */
            ap1[0]=viewp->nesp_entaxis[0][0];
            ap1[1]=viewp->nesp_entaxis[1][0];
            ap1[2]=viewp->nesp_entaxis[2][0];
          /* BCS X-axis according to the UCS: */
            gr_ncs2ucs(ap1,ap2,1,viewp);
          /* BCS X-axis according to the WCS: */
            dp->trans(ap2,&ucsrb,&wcsrb,1,workcs[1],NULL);

          /* BCS Y-axis according to the gdoNCS (from transposing entaxis): */
            ap1[0]=viewp->nesp_entaxis[0][1];
            ap1[1]=viewp->nesp_entaxis[1][1];
            ap1[2]=viewp->nesp_entaxis[2][1];
          /* BCS Y-axis according to the UCS: */
            gr_ncs2ucs(ap1,ap2,1,viewp);
          /* BCS Y-axis according to the WCS: */
            dp->trans(ap2,&ucsrb,&wcsrb,1,workcs[2],NULL);

          /* BCS Z-axis according to the gdoNCS (from transposing entaxis): */
            ap1[0]=viewp->nesp_entaxis[0][2];
            ap1[1]=viewp->nesp_entaxis[1][2];
            ap1[2]=viewp->nesp_entaxis[2][2];
          /* BCS Z-axis according to the UCS: */
            gr_ncs2ucs(ap1,ap2,1,viewp);
          /* BCS Z-axis according to the WCS: */
            dp->trans(ap2,&ucsrb,&wcsrb,1,workcs[3],NULL);

        } else if (mode==1) {  /* trueNCS to UCS */
            /*
            **  In most cases, the trueNCS and the gdoNCS are the same,
            **  so we can just use viewp->xfp as the trueNCS
            **  according to the UCS.  In the cases where
            **  the database uses WCS but gr_getdispobjs() uses ECS,
            **  we have to get the trueNCS (WCS) according to the gdoNCS (ECS),
            **  and then transform those axes to the UCS (via gr_ncs2ucs).
            */
            if (innertype==DB_ELLIPSE ||
                innertype==DB_MLINE   ||
                innertype==DB_MTEXT   ||
                innertype==DB_POINT   ||
                innertype==DB_TOLERANCE) {

                /*
                **  viewp->nesp_entaxis is the gdoNCS according to the
                **  WCS, (or the trueNCS in this case).  The origins coincide.
                **  The transpose of the axis vectors will give us the
                **  first step: the trueNCS according to the gdoECS.
                */

              /* trueNCS origin (which coincides with the gdoNCS origin) */
              /* according to the UCS: */
				workcs[0][0] = viewp->m_transformation(0,3);
				workcs[0][1] = viewp->m_transformation(1,3);
				workcs[0][2] = viewp->m_transformation(2,3);

              /* trueNCS X-axis according to the gdoNCS (from transposing entaxis): */
                ap1[0]=viewp->nesp_entaxis[0][0];
                ap1[1]=viewp->nesp_entaxis[1][0];
                ap1[2]=viewp->nesp_entaxis[2][0];
              /* trueNCS X-axis according to the UCS: */
                gr_ncs2ucs(ap1,workcs[1],1,viewp);

              /* trueNCS Y-axis according to the gdoNCS (from transposing entaxis): */
                ap1[0]=viewp->nesp_entaxis[0][1];
                ap1[1]=viewp->nesp_entaxis[1][1];
                ap1[2]=viewp->nesp_entaxis[2][1];
              /* trueNCS Y-axis according to the UCS: */
                gr_ncs2ucs(ap1,workcs[2],1,viewp);

              /* trueNCS Z-axis according to the gdoNCS (from transposing entaxis): */
                ap1[0]=viewp->nesp_entaxis[0][2];
                ap1[1]=viewp->nesp_entaxis[1][2];
                ap1[2]=viewp->nesp_entaxis[2][2];
              /* trueNCS Z-axis according to the UCS: */
                gr_ncs2ucs(ap1,workcs[3],1,viewp);

            } else {  /* trueNCS==gdoNCS */

				workcs[0][0] = viewp->m_transformation(0,3);
				workcs[0][1] = viewp->m_transformation(1,3);
				workcs[0][2] = viewp->m_transformation(2,3);
				workcs[1][0] = viewp->m_transformation(0,1);
				workcs[1][1] = viewp->m_transformation(0,2);
				workcs[1][2] = viewp->m_transformation(0,3);
				workcs[2][0] = viewp->m_transformation(1,1);
				workcs[2][1] = viewp->m_transformation(1,2);
				workcs[2][2] = viewp->m_transformation(1,3);
				workcs[3][0] = viewp->m_transformation(2,1);
				workcs[3][1] = viewp->m_transformation(2,2);
				workcs[3][2] = viewp->m_transformation(2,3);
            }
        }

        /*
        **  If workcs describes system A according to B, we need to use it to
        **  fill xformres such that it will transform points or vectors
        **  from A to B.
        **
        **  workcs is arranged as follows:
        **
        **      Origin 00 01 02
        **
        **      X-axis 10 11 12
        **
        **      Y-axis 20 21 22
        **
        **      Z-axis 30 31 32
        **
        **  Transforming a point or vector from (x,y,z) in A
        **  to (x',y',z') in B involves using the source's coordinates
        **  as coefficents of the vectors and adding like components:
        **
        **       x'   y'   z'
        **
        **      x.10 x.11 x.12          x' = x.10 + y.20 + z.30
        **        +    +    +
        **      y.20 y.21 y.22    or    y' = x.11 + y.21 + z.31
        **        +    +    +
        **      z.30 z.31 z.32          z' = x.12 + y.22 + z.32
        **
        **  Then, for a point (not a vector), we add the origin:
        **
        **      x' = x.10 + y.20 + z.30 + 00
        **
        **      y' = x.11 + y.21 + z.31 + 01
        **
        **      z' = x.12 + y.22 + z.32 + 02
        **
        **  Therefore, the matrix must be set up as follows:
        **
        **                xformres
        **
        **    | x'|   | 10 20 30 00 |   | x |
        **    |   |   |             |   |   |
        **    | y'|   | 11 21 31 01 |   | y |
        **    |   | = |             | . |   |
        **    | z'|   | 12 22 32 02 |   | z |
        **    |   |   |             |   |   |
        **    | 1 |   |  0  0  0  1 |   | 1 |
        **
        **  The following loop uses workcs to form xformres
        **  in this manner; the bottom row was set earlier:
        */
        for (fi1=0; fi1<3; fi1++) {  /* xformres row */
            for (fi2=0; fi2<4; fi2++) {  /* xformres col */
                if ((fi3=fi2+1)>3) fi3=0;  /* workcs row */
                xformres[fi1][fi2]=workcs[fi3][fi1];
            }
        }

    }  /* End if (xformres!=NULL) */


out:
    if (rc) {
        if (innerhipp!=NULL) *innerhipp=NULL;
        if (refstkrespp!=NULL) {
            if (*refstkrespp!=NULL)
                { sds_relrb(*refstkrespp); *refstkrespp=NULL; }
        }
        if (xformres!=NULL) {
            memset(xformres,0,sizeof(xformres));
            xformres[0][0]=xformres[1][1]=xformres[2][2]=xformres[3][3]=1.0;
        }
    }

    /* Clean up viewp stuff: */
    if (viewp!=NULL) {
        viewp->nesp_sl=NULL;
        viewp->nesp_innerhip=NULL;
        if( viewp->nesp_entaxis )
		{
			delete [] viewp->nesp_entaxis;
			viewp->nesp_entaxis=NULL;
		}

        /* Free viewp->nesp_refstkres if it's still set: */
        if (viewp->nesp_refstkres!=NULL)
            { sds_relrb(viewp->nesp_refstkres); viewp->nesp_refstkres=NULL; }

    }

    return rc;
}




#include "gr.h"
#include "DoSelect.h"
#include "DoBase.h"
#include "2d.h"

GR_API int gr_selected2
(
db_handitem* pEntity,
struct gr_selectorlink* pSelector, 
gr_view* pView
)
{
/*
**  Determines as rapidly as possible whether or not the entity
**  described by pEntity is selected by the selector described
**  by pSelector.
**
**  The entlink MUST have display objects.  If the entity's extent
**  is not set for some reason, the local one gets set here (because
**  we need it in here for the logic).
**
**  Call gr_initselector() first to allocate and set up pSelector.
**
**  Returns:
**      +1 : Selected
**       0 : Not selected
*/
    int result = 0;

	if(pEntity==NULL || pSelector==NULL || pSelector->def==NULL || pSelector->extent==NULL)
		return 0;

    int enttype = pEntity->ret_type();

	struct gr_displayobject* pEntityPoints = (gr_displayobject *)pEntity->ret_disp();  /* For simplicity */
    if(pEntityPoints == NULL)
		return 0;

	CD3 minPoint, maxPoint;
	CDoBase::getPlanarBoundingBox(pView, static_cast<db_entity*>(pEntity), minPoint, maxPoint);
	
    /* If the ent extent and selector extent are separate, definitely OUT: */
    if(minPoint[0] > pSelector->extent[1][0] || maxPoint[0] < pSelector->extent[0][0] ||
		minPoint[1] > pSelector->extent[1][1] || maxPoint[1] < pSelector->extent[0][1])
		return pSelector->mode == 'O';

    /*
    **  If the ent extent is completely inside the selector extent,
    **  we don't know anything (except for Window).  However --
    **  for all types except Point -- if the selector extent is
    **  completely inside the ent entent, we can't have Inside:
    */
    if(pSelector->type != 'P' && pSelector->mode == 'I' &&
        pSelector->extent[0][0] > minPoint[0] && pSelector->extent[1][0] < maxPoint[0] &&
        pSelector->extent[0][1] > minPoint[1] && pSelector->extent[1][1] < maxPoint[1]) 
		return 0;

	CD3 inUCS;
	CD3 projected, projectedEnd;
	CMatrix4 toUCS;
	toUCS.makeIdentity();
	bool bToUCS = !toUCS.isEqualWithin(CMatrix4(CMatrix4::identity()));
	CRect2D bbox(minPoint[0], minPoint[1], maxPoint[0], maxPoint[1]);

	int i, n;
	bool bHavePointInside = false;
    struct gr_displayobject* pCurElement;
    struct ic_pointsll* pPointsList = NULL;
    switch(pSelector->type) 
	{
	case 'W':  
		{
			/* Window; just use the extent (not the def). */
			/* If entextent is completely inside pSelector->extent, then the */
			/* whole entity is in: */
			// Added FUZZ to fix 1-46167. DP: ???
			if((minPoint[0] >= pSelector->extent[0][0] && maxPoint[0] <= pSelector->extent[1][0] &&
				minPoint[1] >= pSelector->extent[0][1] && maxPoint[1] <= pSelector->extent[1][1]) ||
				FEQUAL(pSelector->extent[0][0], minPoint[0], FUZZ_GEN))
				return pSelector->mode != 'O';
			
			/* That's the ONLY way the whole entity can be in: */
			if(pSelector->mode == 'I')
				return 0;
			
			/* See if even one pt is in: */
			CRect2D win(pSelector->extent[0][0], pSelector->extent[0][1], pSelector->extent[1][0], pSelector->extent[1][1]);
			for(pCurElement = pEntityPoints; pCurElement != NULL; pCurElement = pCurElement->next) 
			{
				if(pCurElement->npts < 1 || pCurElement->chain == NULL || pCurElement->type & DISP_OBJ_SPECIAL) 
					continue;
				
				if(pCurElement->type & DISP_OBJ_PTS_3D)
				{
					n = 3 * pCurElement->npts;
					for(i = 0; i < n; i += 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[i]));
							gr_ucs2rp(inUCS, projected, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[i]), projected, pView);
						if(win.isIntersected(projected[0], projected[1]))
							return pSelector->mode == 'C';
					}
				}
				else
				{
					n = 2 * pCurElement->npts;
					for(i = 0; i < n; i += 2)
						if(win.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1]))
							return pSelector->mode == 'C';
				}
			}
			
			/* No pts are in -- but we may have a disp obj segment that cuts */
			/* through the window: */
			for(pCurElement = pEntityPoints; pCurElement != NULL; pCurElement = pCurElement->next) 
			{
				/* Each disp obj */
				if(pCurElement->npts < 2 || pCurElement->chain == NULL || pCurElement->type & DISP_OBJ_SPECIAL)
					continue;
				
				if(pCurElement->type & DISP_OBJ_PTS_3D)
				{
					n = 3 * pCurElement->npts;
					if(bToUCS)
					{
						toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
						gr_ucs2rp(inUCS, projected, pView);
					}
					else
						gr_ucs2rp(&(pCurElement->chain[0]), projected, pView);
					for(i = 3; i < n; i += 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[i]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[i]), projectedEnd, pView);
						if(win.isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1]))
							return pSelector->mode == 'C';
						
						projected = projectedEnd;
					}
					
					if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[0]), projectedEnd, pView);
						if(win.isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1]))
							return pSelector->mode == 'C';
					}
				}
				else
				{
					n = 2 * (pCurElement->npts - 1);
					for(i = 0; i < n; i += 2)
						if(win.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[i + 2], pCurElement->chain[i + 3]))
							return pSelector->mode == 'C';
						if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
							if(win.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[0], pCurElement->chain[1]))
								return pSelector->mode == 'C';
				}
			}
			
			/* Still here?  No pts inside and no disp obj seg crosses */
			/* an extent seg: */
			result = (pSelector->mode =='O');
			break;
		}
		
	case 'C': 
		{
			/* Circle */
			CCircle2D circle(pSelector->def->pt[0], pSelector->def->pt[1], pSelector->def->next->pt[0]);
			
			/* If ent extent is completely inside the circle, the ent */
			/* is completely inside.  (If the ent extent corners are completely */
			/* outside the circle, we don't know anything: the circle may be */
			/* inside the ent extent.): */
			if(circle.isIntersected(minPoint[0], minPoint[1]) && circle.isIntersected(maxPoint[0], maxPoint[1]))
				return pSelector->mode != 'O'; 
			
			/* Check the disp obj pts; do the in-or-out thing. */
			/* Note that finding one of each (a pt that's in and a pt */
			/* that's out) guarantees that we exit the moment it is */
			/* discovered: */
			for(pCurElement = pEntityPoints; pCurElement != NULL; pCurElement = pCurElement->next) 
			{
				if(pCurElement->npts < 1 || pCurElement->chain == NULL || pCurElement->type & DISP_OBJ_SPECIAL) 
					continue;
				
				if(pCurElement->type & DISP_OBJ_PTS_3D)
				{
					n = 3 * pCurElement->npts;
					for(i = 0; i < n; i += 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[i]));
							gr_ucs2rp(inUCS, projected, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[i]), projected, pView);
						if(circle.isIntersected(projected[0], projected[1]))
						{
							if(pSelector->mode == 'C') 
								return 1;
							else
								if(pSelector->mode == 'O')
									return 0;
								bHavePointInside = true;
						}
						else
						{
							if(pSelector->mode == 'I')
								return 0; 
						}
					}
				}
				else
				{
					n = 2 * pCurElement->npts;
					for(i = 0; i < n; i += 2)
						if(circle.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1]))
						{
							if(pSelector->mode == 'C') 
								return 1;
							else
								if(pSelector->mode == 'O')
									return 0;
								bHavePointInside = true;
						}
						else
						{
							if(pSelector->mode == 'I')
								return 0; 
						}
				}
			}
			
			/* Note that they are either all in or all out at this point. */
			if(bHavePointInside) 
				return pSelector->mode != 'O';
			
			/* Still here?  The disp obj pts are all out. */
			/* Can't have Inside, then: */
			if(pSelector->mode == 'I')
				return 0;
			
			/*
			**  However, a seg may cut through the circle.  For each seg,
			**  find the pt on the seg's line that is closest to the center
			**  of the circle and see if it is on the seg AND inside the
			**  circle:
			*/
			for(pCurElement = pEntityPoints; pCurElement != NULL; pCurElement = pCurElement->next) 
			{
				/* Each disp obj */
				if(pCurElement->npts < 2 || pCurElement->chain == NULL || pCurElement->type & DISP_OBJ_SPECIAL)
					continue;
				
				if(pCurElement->type & DISP_OBJ_PTS_3D)
				{
					n = 3 * pCurElement->npts;
					if(bToUCS)
					{
						toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
						gr_ucs2rp(inUCS, projected, pView);
					}
					else
						gr_ucs2rp(&(pCurElement->chain[0]), projected, pView);
					for(i = 3; i < n; i += 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[i]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[i]), projectedEnd, pView);
						if(circle.isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1]))
							return pSelector->mode == 'C';
						
						projected = projectedEnd;
					}
					
					if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[0]), projectedEnd, pView);
						if(circle.isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1]))
							return pSelector->mode == 'C';
					}
				}
				else
				{
					n = 2 * (pCurElement->npts - 1);
					for(i = 0; i < n; i += 2)
						if(circle.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[i + 2], pCurElement->chain[i + 3]))
							return pSelector->mode == 'C';
					if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
						if(circle.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[0], pCurElement->chain[1]))
							return pSelector->mode == 'C';
				}
			}
			
			/* Still here?  Completely out: */
			result = (pSelector->mode == 'O');
			break;
		}

	case 'G':
		{
			/* polyGon */
			for(pPointsList = pSelector->def, n = 0; pPointsList != NULL; pPointsList = pPointsList->next, ++n);
			
			sds_real* pPoints = new sds_real[2 * n];
			for(pPointsList = pSelector->def, i = 0; pPointsList != NULL; pPointsList = pPointsList->next)
			{
				pPoints[i] = pPointsList->pt[0];
				++i;
				pPoints[i] = pPointsList->pt[1];
				++i;
			}
			CPolygon2D polygon(n, pPoints);
			
			/* Check the disp obj pts; do the in-or-out thing. */
			/* Note that finding one of each (a pt that's in and a pt */
			/* that's out) guarantees that we exit the moment it is */
			/* discovered: */
			for(pCurElement = pEntityPoints; pCurElement != NULL; pCurElement = pCurElement->next) 
			{
				if(pCurElement->npts < 1 || pCurElement->chain == NULL || pCurElement->type & DISP_OBJ_SPECIAL) 
					continue;
				
				if(pCurElement->type & DISP_OBJ_PTS_3D)
				{
					n = 3 * pCurElement->npts;
					for(i = 0; i < n; i += 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[i]));
							gr_ucs2rp(inUCS, projected, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[i]), projected, pView);
						if(polygon.isIntersected(projected[0], projected[1]))
						{
							if(pSelector->mode == 'C') 
								return 1;
							if(pSelector->mode == 'O')
								return 0;
							bHavePointInside = true;
						}
						else
						{
							if(pSelector->mode == 'I')
								return 0; 
						}
					}
				}
				else
				{
					n = 2 * pCurElement->npts;
					for(i = 0; i < n; i += 2)
						if(polygon.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1]))
						{
							if(pSelector->mode == 'C') 
								return 1;
							if(pSelector->mode == 'O')
								return 0;
							bHavePointInside = true;
						}
						else
						{
							if(pSelector->mode == 'I')
								return 0; 
						}
				}
			}
			
			/* Note that they are either all in or all out at this point: */
			if(bHavePointInside)
			{
				/* If all in, it's still possible for the ent SEGS to cross */
				/* this kind of selector, so we can't conclude the entity is In: */
				if(pSelector->mode == 'C')
					return 1; 
				if(pSelector->mode == 'O') 
					return 0; 
			} 
			else 
			{
				/* If they're all out, can't have Inside: */
				if(pSelector->mode == 'I') 
					return 0; 
			}
			
			/* Now they're either all in and we're looking for Inside, */
			/* or all out and we're looking for Crossing or Outside. */
			/* Either way, we need to know if any disp obj segs actually */
			/* cross a selector seg: */
			
			/* Check each disp obj seg against the sel seg: */
			for(pCurElement = pEntityPoints; pCurElement != NULL; pCurElement = pCurElement->next) 
			{  
				/* Each disp obj */
				if(pCurElement->npts < 2 || pCurElement->chain==NULL || pCurElement->type & DISP_OBJ_SPECIAL)
					continue;
				
				if(pCurElement->type & DISP_OBJ_PTS_3D)
				{
					n = 3 * pCurElement->npts;
					if(bToUCS)
					{
						toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
						gr_ucs2rp(inUCS, projected, pView);
					}
					else
						gr_ucs2rp(&(pCurElement->chain[0]), projected, pView);
					for(i = 3; i < n; i += 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[i]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[i]), projectedEnd, pView);
						if(polygon.isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1]))
							return pSelector->mode == 'C';
						
						projected = projectedEnd;
					}
					
					if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[0]), projectedEnd, pView);
						if(polygon.isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1]))
							return pSelector->mode == 'C';
					}
				}
				else
				{
					n = 2 * (pCurElement->npts - 1);
					for(i = 0; i < n; i += 2)
						if(polygon.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[i + 2], pCurElement->chain[i + 3]))
							return pSelector->mode == 'C';
					if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
						if(polygon.isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[0], pCurElement->chain[1]))
							return pSelector->mode == 'C';
				}
			}
			
			/* Still here?  No actual crossing.  So, with what we know from */
			/* the comment above the "for" loop... */
			result = (bHavePointInside || pSelector->mode=='O');
			break;
		}

	case 'F':  /* Fence */

		for(pPointsList = pSelector->def; pPointsList->next != NULL; pPointsList = pPointsList->next) 
		{  
			/* Each selector seg */
			
			// DP: ???
			/* If the pSelector seg doesn't cross through the ent extent box, */
			/* it can't cross the ent: */
			if(!bbox.isIntersected(pPointsList->pt[0], pPointsList->pt[1], pPointsList->next->pt[0], pPointsList->next->pt[1]))
				continue;
			
			/* Check each disp obj seg: */
			for(pCurElement = pEntityPoints; pCurElement != NULL; pCurElement = pCurElement->next) 
			{  
				/* Each disp obj */
				if(pCurElement->chain == NULL || pCurElement->npts < 2 || pCurElement->type & DISP_OBJ_SPECIAL) 
					continue;
				
				if(pCurElement->type & DISP_OBJ_PTS_3D)
				{
					n = 3 * pCurElement->npts;
					if(bToUCS)
					{
						toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
						gr_ucs2rp(inUCS, projected, pView);
					}
					else
						gr_ucs2rp(&(pCurElement->chain[0]), projected, pView);
					for(i = 3; i < n; i += 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[i]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[i]), projectedEnd, pView);
						if(isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1],
							pPointsList->pt[0], pPointsList->pt[1], pPointsList->next->pt[0], pPointsList->next->pt[1]))
							return pSelector->mode != 'O';
						
						projected = projectedEnd;
					}
					
					if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[0]));
							gr_ucs2rp(inUCS, projectedEnd, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[0]), projectedEnd, pView);
						if(isIntersected(projected[0], projected[1], projectedEnd[0], projectedEnd[1],
							pPointsList->pt[0], pPointsList->pt[1], pPointsList->next->pt[0], pPointsList->next->pt[1]))
							return pSelector->mode != 'O';
					}
				}
				else
				{
					n = 2 * (pCurElement->npts - 1);
					for(i = 0; i < n; i += 2)
						if(isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[i + 2], pCurElement->chain[i + 3],
							pPointsList->pt[0], pPointsList->pt[1], pPointsList->next->pt[0], pPointsList->next->pt[1]))
							return pSelector->mode == 'C';
					if(pCurElement->type & DISP_OBJ_PTS_CLOSED)
						if(isIntersected(pCurElement->chain[i], pCurElement->chain[i + 1], pCurElement->chain[0], pCurElement->chain[1],
							pPointsList->pt[0], pPointsList->pt[1], pPointsList->next->pt[0], pPointsList->next->pt[1]))
							return pSelector->mode != 'O';
				}
			}
		}
		
		/* Still here?  Completely out (not crossing): */
		result = (pSelector->mode == 'O');
		break;
		  
	case 'P':  
		/* Point */
		/*
		**  For now, we're requiring a closed entity that has just
		**  one closed display object.  That means that entities whose
		**  display objects show non-solid linetypes or thicknesses are
		**  always considered Outside.
		*/
		
		int flag = 0;  /* Closed, non-mesh POLYLINE? */
		if(enttype == DB_LWPOLYLINE)
		{
			pEntity->get_70(&flag); 
			flag = ((flag & 0x01) == 1);
		}
		if(enttype==DB_POLYLINE)
		{
			pEntity->get_70(&flag); 
			flag = ((flag & 0x51) == 1);
		}
		if((pEntityPoints->next == NULL || enttype == DB_MTEXT) &&
			(pEntityPoints->type & DISP_OBJ_PTS_CLOSED) && 
			pEntityPoints->npts > 2 && pEntityPoints->chain != NULL &&
			(flag              ||
			enttype==DB_MTEXT  ||
			enttype==DB_CIRCLE ||
			enttype==DB_TRACE  ||
			enttype==DB_SOLID  ||
			enttype==DB_3DFACE ||
			enttype==DB_ELLIPSE)) 
		{  
			/* Closed entity */
			for(pCurElement = pEntityPoints; pCurElement && !result; pCurElement = pCurElement->next)
			{
				if(pCurElement->type & DISP_OBJ_SPECIAL)
					continue;
				/* Allocate ptsll for ic_inorout() ("No memory" shows up as */
				/* "not selected".)  Note that since we know the number of */
				/* links at allocation time, we can do ONE allocation and */
				/* use it as an array here, but link it up for ic_inorout(): */
				if(pPointsList != NULL)
				{ 
					IC_FREE(pPointsList); 
					pPointsList = NULL; 
				}
				if((pPointsList = new struct ic_pointsll [pEntityPoints->npts] ) == NULL)
					return 0;
				memset(pPointsList, 0 ,sizeof(struct ic_pointsll)* pEntityPoints->npts);
				
				int nDim = 2 + (pEntityPoints->type & DISP_OBJ_PTS_3D);
				/* Fill ptsll from the disp obj (and link it): */
				for(i = n = 0; i < pCurElement->npts; ++i, n += nDim) 
				{
					if(nDim == 3)
					{
						if(bToUCS)
						{
							toUCS.multiplyRight(inUCS, &(pCurElement->chain[n]));
							gr_ucs2rp(inUCS, projected, pView);
						}
						else
							gr_ucs2rp(&(pCurElement->chain[n]), projected, pView);
						
						pPointsList[i].pt[0] = projected[0];
						pPointsList[i].pt[1] = projected[1];
					}
					else
					{
						pPointsList[i].pt[0] = pCurElement->chain[n];
						pPointsList[i].pt[1] = pCurElement->chain[n + 1];
					}
					if(i)
						pPointsList[i - 1].next = pPointsList + i;
				}
				
				flag = ic_inorout(pPointsList, minPoint[0], minPoint[1], maxPoint[0], maxPoint[1],
					pSelector->def->pt[0], pSelector->def->pt[1]);
				
				result = (flag == +1) ? (pSelector->mode!='O') : (pSelector->mode=='O');
			}
			if(pPointsList != NULL)
			{ 
				IC_FREE (pPointsList); 
				pPointsList = NULL; 
			}
		} 
		else 
		{  /* Open entity, so OUT. */
			result=(pSelector->mode=='O');
		}
		
		break;
    }  /* End switch (pSelector->type) */
	
    return result;
}

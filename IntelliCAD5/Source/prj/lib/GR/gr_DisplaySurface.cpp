/* C:\DEV\PRJ\LIB\GR\GR_DISPLAYSURFACE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * gr_DisplaySurface is a class which adds elements to a gr_displayobject list. Currently it
 * is only set up to add 2d lines for hatch entities, but this class can easily be extended
 * as more functioanlity is needed.
 *
 * History
 * 24-03-2003 EBATECH(CNBR) Bugzilla#78471 Support Lineweight
 */


#include "gr.h"
#include "db.h"
#include "gr_DisplaySurface.h"
#include "gr_view.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Note: gr_Display surface takes in gr_displayobjects, so that it can add to these lists.  It is not
//responsible for destroying memory in the display object lists.
gr_DisplaySurface::gr_DisplaySurface(const double &resolution, gr_displayobject **begObjectList, gr_displayobject **endObjectList,
									 gr_view *view)
	: m_resolution(resolution), m_color(DB_BLCOLOR), m_lweight(DB_DFWEIGHT), m_begObjectList(begObjectList), 
	m_endObjectList(endObjectList), m_pGrView(view), m_elev(0.0), m_bAlreadyInUcs(false)
	{
    }


gr_DisplaySurface::~gr_DisplaySurface()
	{

	}


//Set color to a value between 0 and 256
//Returns 0 if successful; -1 if value is out of range
//NOTE:EBATECH(CNBR) I think here is actual color between 1 to 255.
int gr_DisplaySurface::SetColor(const short color)
	{
	if ((color >= 0) && (color <= 256))
		{
		m_color = color;
		return 0;
		}

	return -1;
	}

//Set Lineweight to a value between 0-211, and DB_DFWEIGHT
//Returns 0 if successful; -1 if value is out of range
int gr_DisplaySurface::SetLWeight(const short lweight)
	{
		if( db_is_validweight( lweight, DB_DFWMASK ))
		{
		m_lweight = lweight;
		return 0;
		}
	return -1;
	}


//adds a display segment between the 2D points (x1,y1) and  (x2,y2)
//Return Values: 0 = success; -1 = not enough memory
int gr_DisplaySurface::AddSegment(const double x1, const double y1, const double x2, const double y2)
{
	sds_point source[2];

	GetPoint(x1, y1, 0, source[0]);
	GetPoint(x2, y2, 0, source[1]);

	if(m_pGrView->projmode)
		return AddDisplayObj(source, 2, 2, 0);
	else
		return AddDisplayObj(source, 2, 3, DISP_OBJ_PTS_3D);
}

//Add a 2D point
//Returns 0 if successful
int gr_DisplaySurface::AddPoint(const double x, const double y)
{
	sds_point source[2];

	GetPoint(x, y, 0, source[0]);
	GetPoint(x, y, 0, source[1]);

	if(m_pGrView->projmode)
		return AddDisplayObj(source, 2, 2, 0);
	else
		return AddDisplayObj(source, 2, 3, DISP_OBJ_PTS_3D);
}

int
gr_DisplaySurface::AddDisplayObj
(
 sds_point*	source,
 const int	numPoints,
 const int	numDimemsions,
 const int	type,
 db_hatch*	pSourceHatch /*= NULL*/
)
{
	// TODO TODO TODO
	// Pass the source entity to newDisplayObject so it can make selections properly
	//
	gr_displayobject *myObject = gr_displayobject::newDisplayObject((db_entity*)pSourceHatch);

	if (!myObject)
		return -1;

	myObject->type = charCast(type);
	myObject->color = m_color;
	myObject->lweight = m_lweight;

	myObject->npts = numPoints;

	//Chain expressed as XY... since 2D
	myObject->chain = new sds_real[ myObject->npts * numDimemsions ];

    if (!myObject->chain)
		{
		IC_FREE(myObject);
		return -1;
		}

	sds_point dest;
	for (int i = 0; i < myObject->npts; i++)
		{
		//Transform points to proper coordinate system
		if (!Transform (source[i], dest))
			gr_setdochainpt(myObject, i, dest, numDimemsions);
		}

	myObject->next = NULL;

	if (!*m_begObjectList)
		{
		*m_begObjectList = myObject;
		*m_endObjectList = myObject;
		}
	else
		{
		(*m_endObjectList)->next = myObject;
		*m_endObjectList = myObject;
		}

	return 0;
}

void gr_DisplaySurface::GetPoint(const double &x, const double &y, const double &z, sds_point point)
	{
	point[0] = x;
	point[1] = y;
	point[2] = z;
	}


//Returns 0 if successful, non-zero if source is behind the camera (perspective)
int gr_DisplaySurface::Transform(sds_point source, sds_point dest)
	{
	if(!m_bAlreadyInUcs)
		{
	    source[2] += m_elev;	//  for bug 1-75595
		gr_ncs2ucs(source, dest, 0, m_pGrView);
		}
	else
        memcpy(dest, source, 3 * sizeof(double));

        return m_pGrView->projmode && gr_ucs2rp(dest,dest,m_pGrView);
	}


// 5/98 Below is code that essentially copyies one gr_displayobject to another.  I decided not to
// use it here, but I thought I would leave the code commented in case it was useful in another situation.
// returns 0 if successfull, -1 if not enough memory
///*//<-4M Item:131
int gr_DisplaySurface::GetDisplayObjects(gr_displayobject **begObject, gr_displayobject **endObject)
	{
	gr_displayobject *currentObject = *m_begObjectList;

	if (*begObject)
		{
		gr_freedisplayobjectll(*begObject);
		begObject = NULL;
		endObject = NULL;
		}

	while (currentObject)
		{
		gr_displayobject *myObject = gr_displayobject::newDisplayObject( currentObject->GetSourceEntity() );

		if (!myObject)
			return -1;

		myObject->type = currentObject->type;
		myObject->color = currentObject->color;
		myObject->lweight = currentObject->lweight;
		myObject->npts = currentObject->npts;

		//bit 1 of type is 0 if 2D or 1 if 3D
		int chainSize = myObject->npts * ((myObject->type & 1) ? 3 : 2);		
		myObject->chain = new sds_real[chainSize ];

		if (!myObject->chain)
			{
			IC_FREE(myObject);
			return -1;
			}

		for (int i = 0; i < chainSize; i++)
			myObject->chain[i] = currentObject->chain[i];

		if (!*begObject)
			*begObject = myObject;
		else
			(*endObject)->next = myObject;

		*endObject = myObject;
		currentObject = currentObject->next;
		}

		(*endObject)->next = NULL;
		return 0;
	}
//*/ //<-4M Item:131



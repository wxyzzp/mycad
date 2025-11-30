/* C:\DEV\PRJ\LIB\GR\GR_DISPLAYSURFACE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * gr_DisplaySurface is a class which adds elements to a gr_displayobject list. Currently it
 * is only set up to add 2d lines for hatch entities, but this class can easily be extended
 * as more functioanlity is needed.
 *
 * Note that this class currently avoids using the point chain in gr_view->pc.  This seems to
 * only be needed for line types. This class was originally developed for hatch entities where line
 * types do not apply.  May need to add this functionality for other entities
 *
 * History
 * 24-03-2003 EBATECH(CNBR) Bugzilla#78471 Support Lineweight
 */


#if !defined(AFX_GR_DISPLAYSURFACE_H__AC80B9E2_E440_11D1_B351_0060081C2430__INCLUDED_)
#define AFX_GR_DISPLAYSURFACE_H__AC80B9E2_E440_11D1_B351_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "gr.h"

class gr_DisplaySurface
{
public:
	//Note: gr_Display surface takes in gr_displayobjects, so that it can add to these lists.  It is not
	//responsible for destroying memory in the display object lists.
	gr_DisplaySurface(const double &resolution, gr_displayobject **begObjectList, gr_displayobject **endObjectList, gr_view *view);
	virtual ~gr_DisplaySurface();

	//Returns pixel height size
	double Resolution() {return m_resolution;}

	//Set color to a value between 1 to 255
	//Returns 0 if successful; -1 if value is out of range
	int SetColor(const short color);
	short GetColor() {return m_color;}

	//Set Lineweight to a value between 0-211, and DB_DFWEIGHT
	//Returns 0 if successful; -1 if value is out of range
	int SetLWeight(const short lweight);
	short GetLWeight() {return m_lweight;}

	//adds a display segment between the points (x1,y1) and  (x2,y2)
	//Return Values: 0 = success;
	int AddSegment(const double x1, const double y1, const double x2, const double y2);

	//Add a 2D point
	//Returns 0 if successful
	int AddPoint(const double x, const double y);

   int GetDisplayObjects(gr_displayobject **begObject, gr_displayobject **endObject);//<-4M Item:131

	//Note type parameter will be assigned to gr_displayobject->type. See gr_displayobject for details
	//Returns 0 if successful
	/*D.Gavrilov*/// I've made this method public for access from CFillDisplay::Generate.
    int AddDisplayObj(sds_point *source, const int numPoints, const int numDimemsions, const int type, db_hatch* pSourceHatch = NULL);

	double		m_elev;
	gr_view*	m_pGrView;	/*D.G.*/// Made public for access from gr_hatch.cpp.

	bool		m_bAlreadyInUcs;	// true if points in caches have been already transformed to UCS

protected:
	double m_resolution;
	short m_color;
	short m_lweight;	/*EBATECH(CNBR)*/

	gr_displayobject **m_begObjectList;
	gr_displayobject **m_endObjectList;

    void GetPoint(const double &x, const double &y, const double &z, sds_point point);

	//Returns 0 if successful, non-zero if source is behind the camera (perspective)
    int Transform(sds_point source, sds_point dest);

};

// Prototype of hatch display
int gr_DisplayHatch(
	db_hatch* pHatch,				// I: Hatch entity's DB handle
	gr_DisplaySurface* pSurface);	// I/O: The display surface

#endif // !defined(AFX_GR_DISPLAYSURFACE_H__AC80B9E2_E440_11D1_B351_0060081C2430__INCLUDED_)



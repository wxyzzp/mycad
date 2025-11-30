/* C:\ICAD\PRJ\LIB\DB\HATCH.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#pragma warning (disable : 4786)	/*D.G.*/

#include "db.h"/*DNT*/
#include "fixuptable.h"/*DNT*/
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include "Region.h"/*DNT*/
#include "Boolean.h"/*DNT*/
#include "BHatchFactory.h"/*DNT*/
#include "BHatchBuilder.h"/*DNT*/
#include "..\..\icad\res\icadrc2.h"/*DNT*/
#include "SelectionSet.h"/*DNT*/

/********************* db_hatchsplinelink *******************/

void db_hatchsplinelink::operator = (const db_hatchsplinelink &splineedge) 
{
	flags = splineedge.flags;
	deg = splineedge.deg;
	nknots = splineedge.nknots;
	ncpts = splineedge.ncpts;

	if (knot) {
		delete[] knot;
		knot = NULL;
	}

	if (nknots) {
		knot = new sds_real[nknots];
		for (int i = 0; i < nknots; i++)
			knot[i] = splineedge.knot[i];
	}

	if (control) {
		delete [] control;
		control = NULL;
	}

	if (ncpts) {
		control = new db_hatchsplinecontrollink[ncpts];
		for (int i = 0; i < ncpts; i++)
			control[i] = splineedge.control[i];
	}
}

bool db_hatchsplinelink::operator == (const db_hatchsplinelink &splineedge) const 
{
	if (flags != splineedge.flags || deg != splineedge.deg || nknots != splineedge.nknots || 
		ncpts != splineedge.ncpts)
		return false;

	for (int i = 0; i < nknots; i++)
		if (knot[i] != splineedge.knot[i])
			return false;

	for (i = 0; i < ncpts; i++)
		if (control[i] != splineedge.control[i])
			return false;

	return true;
}

/********************* db_hatchedgelink *******************/

db_hatchedgelink::~db_hatchedgelink(void) {
    switch (type) {  /* By type, so the correct destructor is called. */
        case DB_HATCHEDGELINE  : delete u.line  ; break;
        case DB_HATCHEDGEARC   : delete u.arc   ; break;
        case DB_HATCHEDGEELLARC: delete u.ellarc; break;
        case DB_HATCHEDGESPLINE: delete u.spline; break;
        case DB_HATCHEDGEPLINE : delete u.pline ; break;
    }
}

void db_hatchedgelink::operator = (const db_hatchedgelink& edge)
{
	type = edge.type;
	
	switch (type) {  /* By type, so the correct destructor is called. */
		case DB_HATCHEDGELINE  : 
			if (u.line)
				delete u.line;

			u.line = new db_hatchlinelink;

			*(u.line) = *(edge.u.line);
			break;

		case DB_HATCHEDGEARC   : 
			if (u.arc)
				delete u.arc;

			u.arc = new db_hatcharclink;

			*(u.arc) = *(edge.u.arc);
			break;

		case DB_HATCHEDGEELLARC: 
			if (u.ellarc)
				delete u.ellarc;

			u.ellarc = new db_hatchellarclink;

			*(u.ellarc) = *(edge.u.ellarc);
			break;

		case DB_HATCHEDGESPLINE: 
			if (u.spline)
				delete u.spline;

			u.spline = new db_hatchsplinelink;

			*(u.spline) = *(edge.u.spline);
			break;

		case DB_HATCHEDGEPLINE : 
			if (u.pline)
				delete u.pline;

			u.pline = new db_hatchplinelink;

			*(u.pline) = *(edge.u.pline);
			break;
	}
}

bool db_hatchedgelink::operator == (const db_hatchedgelink& edge) const
{
	if (type != edge.type)
		return false;
	
	switch (type) {  /* By type, so the correct destructor is called. */
		case DB_HATCHEDGELINE  : return u.line == edge.u.line;
		case DB_HATCHEDGEARC   : return u.arc == edge.u.arc;
		case DB_HATCHEDGEELLARC: return u.ellarc == edge.u.ellarc;
		case DB_HATCHEDGESPLINE: return u.spline == edge.u.spline;
		case DB_HATCHEDGEPLINE : return u.pline == edge.u.pline;
	}

	return false;
}

/********************* db_hatchboundobjslink *******************/

db_hatchboundobjslink::db_hatchboundobjslink(db_hatchboundobjslink &boundobjlist)
{
	db_hatchboundobjs *current = NULL, *previous;

	boundobjlist.getNextObj(&current);

	if (!current)
	{
		m_first = NULL;
		m_last = NULL;
		return;
	}

	db_hatchboundobjs *newObj = new db_hatchboundobjs(*current);
	m_first = newObj;
	previous = newObj;

	while (boundobjlist.getNextObj(&current))
	{
		newObj = new db_hatchboundobjs(*current);
		previous->setNext(newObj);
		previous = newObj;
	}

	m_last = newObj;

	m_drawing = boundobjlist.m_drawing;
}

db_hatchboundobjslink::~db_hatchboundobjslink()
{
	db_hatchboundobjs *next;

	while (m_first)
	{
		next = m_first->getNext();
		delete m_first;
		m_first = next;
	}

	m_first = NULL;
	m_last = NULL;
}

void db_hatchboundobjslink::CleanAssociations(db_objhandle &hatchHandle, db_handitem* hatch)
{
	db_hatchboundobjs *next = m_first;
	while (next)
	{
		next->CleanAssociations(hatchHandle, hatch);
		next = next->getNext();
	}
}

void db_hatchboundobjslink::CreateAssociations(db_handitem *hatch)
{
	db_hatchboundobjs *next = m_first;
	db_handitem *reactor;

	while (next)
	{
		if(next->getBoundaryObj(&reactor) && reactor)	/*D.G.*/// Checking added.
			//Add assocation from hip back to hatch
			reactor->addReactorAllowDups(hatch->RetHandle(), hatch, ReactorItem::SOFT_POINTER);

		next = next->getNext();
	}
}

// When a hatch entitiy is initially created we want to make sure duplicate entries are not
// added when more than one path uses the same boundary object.
void db_hatchboundobjslink::CreateAssocNoDups(db_handitem *hatch)
{
	db_hatchboundobjs *next = m_first;
	db_handitem *reactor = NULL;//Modified by SMR 2001/5/19

	while (next)
	{
		if(next->getBoundaryObj(&reactor) && reactor)	/*D.G.*/// Checking added.
			//Add assocation from hip back to hatch
			reactor->addReactor(hatch->RetHandle(), hatch, ReactorItem::SOFT_POINTER);

		next = next->getNext();
	}
}

//copies input string so method user is responsible for handling memory destruction of input value
bool db_hatchboundobjslink::addBoundaryObj(char *newObj)
{
	if (m_drawing && newObj && *newObj)
    { 
		db_handitem *hip = m_drawing->handent(newObj);

		if (hip) 
			return addBoundaryObj(hip);
	}

	db_lasterror=OL_EHANDLE; 
	return false; 	
}

bool db_hatchboundobjslink::addBoundaryObj(db_handitem *newObj)
{
	db_hatchboundobjs *newLink = new db_hatchboundobjs;

	if (!newLink) return false;
	
	// set string, next pointer is already initialized to NULL
	bool result = newLink->setBoundaryObj(newObj);

	//set list pointers
	if (!m_first)
		m_first = newLink;
	else
		m_last->setNext(newLink);

	m_last = newLink;

	return result;
}

bool db_hatchboundobjslink::addBoundaryFixup(db_drawing *dp,db_objhandle newObj)
{
	void **fixuploc;

	if(m_drawing && newObj != NULL)
    { 
		if(!addBoundaryFixup(&fixuploc))
		{
			db_lasterror = OL_EHANDLE;						/*D.G.*/// Added this line.
			return false;
		}
		dp->AddHandleToFixupTable(newObj, fixuploc);
		return true;										/*D.G.*/// Added this line.
	}

	db_lasterror = OL_EHANDLE;
	return false; 	
}

bool db_hatchboundobjslink::addBoundaryFixup(void ***fixuploc)
{
	db_hatchboundobjs *newLink = new db_hatchboundobjs;

	if (!newLink) return false;
	
	// set string, next pointer is already initialized to NULL
	newLink->setBoundaryObj(NULL);  // ignore return value

	*fixuploc=(void **)newLink->retp_FixupLoc();
	//set list pointers
	if (!m_first)
		m_first = newLink;
	else
		m_last->setNext(newLink);

	m_last = newLink;

	return true;
}

bool db_hatchboundobjslink::operator == (const db_hatchboundobjslink &objlist) const
{
	db_hatchboundobjs *current = m_first;
	db_hatchboundobjs *objlistcurrent = objlist.m_first;


	while (current && objlistcurrent)
	{
		if (current->retBoundaryObjHand()!=objlistcurrent->retBoundaryObjHand())
			return false;

		current = current->getNext();
		objlistcurrent = objlistcurrent->getNext();
	}

	if (current || objlistcurrent)
		return false;	//uneven list length

	return true;
}

	
bool db_hatchboundobjslink::removeBoundaryObj(const db_objhandle& obj)
{
	if (obj == NULL) 
		return false;

	db_hatchboundobjs *current = m_first;
	db_hatchboundobjs *previous = NULL;

	while (current)
	{
		if (obj==current->retBoundaryObjHand())
		{
			if (!previous)	//first link so reassign m_first to second link
				m_first= m_first->getNext();

			else
				previous->setNext(current->getNext());

			if (m_last == current)
			{
				m_last = previous;
			}

			delete current;
			return true;
		}

		previous = current;
		current = current->getNext();
	}

	return false;
}

bool db_hatchboundobjslink::removeBoundaryObj(db_handitem *obj)
{
	return removeBoundaryObj(obj->RetHandle());
}

bool db_hatchboundobjslink::findBoundaryObj(const db_objhandle &objhandle) const
{
	if (objhandle == NULL) 
		return false;

	db_hatchboundobjs *current = m_first;
	db_hatchboundobjs *previous = NULL;
	while (current)
	{
		if (objhandle == current->retBoundaryObjHand())
			return true;
		previous = current;
		current = current->getNext();
	}
	return false;
}

//Input current position pointer.  If position is NULL the position returned is the first link.  
//If input postion is last link the returned position is NULL
//The character string is just a pointer to the handle and should not be allocated or deleted
//If the input position is the last position newObj returns NULL
bool db_hatchboundobjslink::getNext(db_hatchboundobjs **position, db_objhandle *objHand)
{
	db_handitem *obj;

	bool retval = getNext(position, &obj);

	if (retval && obj)
		*objHand = obj->RetHandle();

	return retval;
}

bool db_hatchboundobjslink::getNext(db_hatchboundobjs **position, db_handitem **newObj)
{
	if (*position == NULL)
		*position = m_first;

	else 
	{
		*position = (*position)->getNext();
	}

	if (!*position)
	{
		*newObj = NULL;
		return false;
	}

	return (*position)->getBoundaryObj(newObj);
}

//Input current position pointer.  If position is NULL the position returned is the first link.  
//If input postion is last link the returned position is NULL
//True is returned if newObj is filled otherwise false
bool db_hatchboundobjslink::getNextObj(db_hatchboundobjs **position)
{
	if (*position == NULL)
		*position = m_first;

	else
		*position = (*position)->getNext();

	if (*position)
		return true;

	return false;
}

/**************************** db_hatchpathlink **********************/

db_hatchpathlink::db_hatchpathlink(db_hatchpathlink &pathlink)
{
	flags = pathlink.flags;
	nedges = pathlink.nedges;

	if (nedges) {
		edge = new db_hatchedgelink[nedges];

		for (int i = 0; i < nedges; i++)
			edge[i] = pathlink.edge[i];
	}
	else
		edge = NULL;
	
	// don't want to copy associations as there won't be a pointer from the reactor back to the hatch
	numBoundObjs = 0;
	boundObjs = NULL;

/*	numBoundObjs = pathlink.numBoundObjs;

	if (numBoundObjs)
		boundObjs = new db_hatchboundobjslink(*(pathlink.boundObjs));
	else
		boundObjs = NULL;
*/
}

void db_hatchpathlink::operator = (const db_hatchpathlink& fromedge)
{
	flags = fromedge.flags;
	nedges = fromedge.nedges;

	if (edge)
		delete [] edge;

	if (nedges) {
		edge = new db_hatchedgelink[nedges];

		for (int i = 0; i < nedges; i++)
			edge[i] = fromedge.edge[i];
	}
	else
		edge = NULL;
	
	// don't want to copy associations as there won't be a pointer from the reactor back to the hatch
	numBoundObjs = 0;
	boundObjs = NULL;

/*	numBoundObjs = fromedge.numBoundObjs;

	if (boundObjs)
		delete boundObjs;

	if (numBoundObjs)
		boundObjs = new db_hatchboundobjslink(*(fromedge.boundObjs));
	else
		boundObjs = NULL;
*/
}

bool db_hatchpatlinelink::operator == (const db_hatchpatlinelink &patList) const
{
	if (ang != patList.ang ||
		base[0] != patList.base[0] ||
		base[1] != patList.base[1] ||
		offset[0] != patList.offset[0] ||
		offset[1] != patList.offset[1] ||
		ndashes != patList.ndashes)
	{
			return false;

	}

	for (int i = 0; i < ndashes; i++)
	{
		if (dash[i] != patList.dash[i])
			return false;
	}

	return true;
}


void db_hatchpatlinelink::operator = (const db_hatchpatlinelink &patList)
{
	ang = patList.ang;
	base[0] = patList.base[0];
	base[1] = patList.base[1];
	offset[0] = patList.offset[0];
	offset[1] = patList.offset[1];
	ndashes = patList.ndashes;

	if (dash)
		delete [] dash;

	dash = new sds_real[ndashes];

	for (int i = 0; i < ndashes; i++)
		dash[i] = patList.dash[i];
}

/**************************** db_hatch *****************************/

/*F*/
db_hatch::db_hatch(void) : db_entity_with_extrusion(DB_HATCH) {
    patname=NULL; patline=NULL; path=NULL; seed=NULL;
    elev=ang=0.0;
    scale=1.0;
    pixelsz=0.001;  /* Something non-0.0 */
    solidfill=assoc=isdouble=0;
    style=pattype=npatlines=npaths=nseeds=0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_hatch::db_hatch(db_drawing *dp) : db_entity_with_extrusion(DB_HATCH,dp) {
    patname=NULL; patline=NULL; path=NULL; seed=NULL;
    elev=ang=0.0;
    scale=1.0;
    pixelsz=0.001;  /* Something non-0.0 */
    solidfill=assoc=isdouble=0;
    style=pattype=npatlines=npaths=nseeds=0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_hatch::db_hatch(const db_hatch &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    int fi1;//,fi2;

    elev     =sour.elev     ; isdouble =sour.isdouble ;
    scale    =sour.scale    ; style    =sour.style    ;
    pixelsz  =sour.pixelsz  ; pattype  =sour.pattype  ;
    ang      =sour.ang      ; npatlines=sour.npatlines;
    solidfill=sour.solidfill; npaths   =sour.npaths   ;
    assoc    =sour.assoc    ; nseeds   =sour.nseeds   ;

    patname=NULL; db_astrncpy(&patname,sour.patname,-1);

    patline=NULL;
    if (npatlines>0 && sour.patline!=NULL) {
        patline=new db_hatchpatlinelink[npatlines];
        memcpy(patline,sour.patline,npatlines*sizeof(patline[0]));

        for (fi1=0; fi1<npatlines; fi1++) {
            patline[fi1].dash=NULL;
            if (patline[fi1].ndashes<1 || sour.patline[fi1].dash==NULL) continue;
            patline[fi1].dash=new sds_real[patline[fi1].ndashes];
            memcpy(patline[fi1].dash,sour.patline[fi1].dash,
                patline[fi1].ndashes*sizeof(sds_real));
        }
    }

    seed=NULL;
    if (nseeds>0 && sour.seed!=NULL) {
        seed=new sds_real[nseeds][2];
        memcpy(seed,sour.seed,nseeds*sizeof(seed[0]));
    }

    path=NULL;
    if (npaths>0 && sour.path!=NULL) {
      path=new db_hatchpathlink[npaths];

      for (fi1=0; fi1<npaths; fi1++) {
   		path[fi1] = sour.path[fi1];

		//we need to copy the boundary objects and create the associations pointers, so it 
		//handles properly at destruction.  If the caller does not want the hatch associated 
		//to the original boundary then it is responsible for modifying it.
		path[fi1].numBoundObjs = sour.path[fi1].numBoundObjs;

		if (sour.path[fi1].boundObjs)
		{
			path[fi1].boundObjs = new db_hatchboundobjslink(*(sour.path[fi1].boundObjs));
//			path[fi1].boundObjs->CreateAssociations(this);
		}
		else 
			path[fi1].boundObjs = NULL;

		/*DG - 14.1.2002*/// All these things have been done earlier in db_hatchpathlink::operator = !!!
		/*
		path[fi1].edge=NULL;
        if (path[fi1].nedges<1 || sour.path[fi1].edge==NULL) continue;
        path[fi1].edge=new db_hatchedgelink[path[fi1].nedges];
        memcpy(path[fi1].edge,sour.path[fi1].edge,
          path[fi1].nedges*sizeof(path[fi1].edge[0]));
        for (fi2=0; fi2<path[fi1].nedges; fi2++) {
          path[fi1].edge[fi2].u.line=NULL;  //Any one of them should do.
          switch (path[fi1].edge[fi2].type) {

            case DB_HATCHEDGELINE:

              path[fi1].edge[fi2].u.line=new db_hatchlinelink;
              memcpy(path[fi1].edge[fi2].u.line,
                sour.path[fi1].edge[fi2].u.line,
                sizeof(*path[fi1].edge[fi2].u.line));
              break;

            case DB_HATCHEDGEARC:

              path[fi1].edge[fi2].u.arc=new db_hatcharclink;
              memcpy(path[fi1].edge[fi2].u.arc,
                sour.path[fi1].edge[fi2].u.arc,
                sizeof(*path[fi1].edge[fi2].u.arc));
              break;

            case DB_HATCHEDGEELLARC:

              path[fi1].edge[fi2].u.ellarc=new db_hatchellarclink;
              memcpy(path[fi1].edge[fi2].u.ellarc,
                sour.path[fi1].edge[fi2].u.ellarc,
                sizeof(*path[fi1].edge[fi2].u.ellarc));
              break;

            case DB_HATCHEDGESPLINE:

              path[fi1].edge[fi2].u.spline=new db_hatchsplinelink;
              memcpy(path[fi1].edge[fi2].u.spline,
                sour.path[fi1].edge[fi2].u.spline,
                sizeof(*path[fi1].edge[fi2].u.spline));

              path[fi1].edge[fi2].u.spline->knot=NULL;
              if (path[fi1].edge[fi2].u.spline->nknots>0 &&
                sour.path[fi1].edge[fi2].u.spline->knot!=NULL) {

                path[fi1].edge[fi2].u.spline->knot=
                  new sds_real[path[fi1].edge[fi2].u.spline->nknots];
                memcpy(path[fi1].edge[fi2].u.spline->knot,
                  sour.path[fi1].edge[fi2].u.spline->knot,
                  path[fi1].edge[fi2].u.spline->nknots*
                    sizeof(path[fi1].edge[fi2].u.spline->knot[0]));
              }

              path[fi1].edge[fi2].u.spline->control=NULL;
              if (path[fi1].edge[fi2].u.spline->ncpts>0 &&
                sour.path[fi1].edge[fi2].u.spline->control!=NULL) {

                path[fi1].edge[fi2].u.spline->control=
                  new db_hatchsplinecontrollink[path[fi1].edge[fi2].u.spline->ncpts];
                memcpy(path[fi1].edge[fi2].u.spline->control,
                  sour.path[fi1].edge[fi2].u.spline->control,
                  path[fi1].edge[fi2].u.spline->ncpts*
                    sizeof(path[fi1].edge[fi2].u.spline->control[0]));
              }

              break;

            case DB_HATCHEDGEPLINE:

              path[fi1].edge[fi2].u.pline=new db_hatchplinelink;
              memcpy(path[fi1].edge[fi2].u.pline,
                sour.path[fi1].edge[fi2].u.pline,
                sizeof(*path[fi1].edge[fi2].u.pline));
              path[fi1].edge[fi2].u.pline->vert=NULL;
              if (path[fi1].edge[fi2].u.pline->nverts>0 &&
                sour.path[fi1].edge[fi2].u.pline->vert!=NULL) {

                path[fi1].edge[fi2].u.pline->vert=
                  new db_hatchplinevertlink[path[fi1].edge[fi2].u.pline->nverts];
                memcpy(path[fi1].edge[fi2].u.pline->vert,
                  sour.path[fi1].edge[fi2].u.pline->vert,
                  path[fi1].edge[fi2].u.pline->nverts*
                    sizeof(path[fi1].edge[fi2].u.pline->vert[0]));
              }

              break;

          }	// End switch edge-type
        } // End for each edge (fi2)
		*/

      }  /* End for each path (fi1) */
    }  /* End if (npaths>0 && sour.path!=NULL) */


    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}

db_hatch::~db_hatch(void) 
{
	RemovePaths();
	
    delete [] patname; delete [] patline; delete [] seed;

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

         /***** THE patline[] ARRAY GETTER *****/

/* Get patline[pidx]'s values, including a ptr to patline[pidx].dash[]: */
DB_API void db_hatch::get_patline(
    int        pidx,
    sds_real  *angp,
    sds_real  *basexp,
    sds_real  *baseyp,
    sds_real  *offsetxp,
    sds_real  *offsetyp,
    int       *ndashesp,
    sds_real **dashpp) {

    if (patline!=NULL && pidx>-1 && pidx<npatlines) {
        if (angp!=NULL)     *angp    =patline[pidx].ang;
        if (basexp!=NULL)   *basexp  =patline[pidx].base[0];
        if (baseyp!=NULL)   *baseyp  =patline[pidx].base[1];
        if (offsetxp!=NULL) *offsetxp=patline[pidx].offset[0];
        if (offsetyp!=NULL) *offsetyp=patline[pidx].offset[1];
        if (ndashesp!=NULL) *ndashesp=patline[pidx].ndashes;
        if (dashpp!=NULL)   *dashpp  =patline[pidx].dash;
    }
}

              /***** THE patline[] ARRAY SETTERS *****/

/* Allocate patline[] and set npatlines: */
bool db_hatch::set_78(int p) {
    if (patline) {
		delete [] patline; 
		patline=NULL; 
		npatlines=0;
	}

    if (p>0) { npatlines=p; patline=new db_hatchpatlinelink[npatlines]; }
    return true;
}
/* Set patline[pidx]'s values and allocate patline[pidx].dash[]: */
DB_API void db_hatch::set_patline(
    int      pidx,
    sds_real ang,
    sds_real basex,
    sds_real basey,
    sds_real offsetx,
    sds_real offsety,
    int      ndashes) {

    if (patline!=NULL && pidx>-1 && pidx<npatlines) {
        ic_normang(&ang,NULL); patline[pidx].ang=ang;
        patline[pidx].base[0]=basex;
        patline[pidx].base[1]=basey;
        patline[pidx].offset[0]=offsetx;
        patline[pidx].offset[1]=offsety;
        delete patline[pidx].dash; patline[pidx].dash=NULL;
        patline[pidx].ndashes=0;
        if (ndashes>0) {
            patline[pidx].ndashes=ndashes;
            patline[pidx].dash=new sds_real[patline[pidx].ndashes];
            memset(patline[pidx].dash,0,
                patline[pidx].ndashes*sizeof(patline[pidx].dash[0]));
        }
    }
}
/* Set patline[lidx].dash[didx]: */
DB_API void db_hatch::set_patlinedash(int pidx, int didx, sds_real dashval) {
    if (patline!=NULL && pidx>-1 && pidx<npatlines &&
        patline[pidx].dash!=NULL && didx>-1 &&
        didx<patline[pidx].ndashes) patline[pidx].dash[didx]=dashval;
}

// intended for undo/redo sets the pattern line to the pattern
DB_API void db_hatch::set_patline(int index, db_hatchpatlinelink *newPatLine)
{
	if (index < 0 || index >= npatlines || !newPatLine) 
		return;

	patline[index] = *newPatLine;
}

                /***** THE path[] ARRAY GETTERS *****/

/* Get the path[pidx] members (except the edge[] array and the boundObjs array): */
DB_API void db_hatch::get_path(int pidx, int *flagsp, int *nedgesp, int *numBoundaryObjs) {
    if (path!=NULL && pidx>-1 && pidx<npaths) {
		if (flagsp != NULL)
			*flagsp=path[pidx].flags;
		
		if (nedgesp != NULL)
			*nedgesp=path[pidx].nedges;

		if (numBoundaryObjs != NULL)
			*numBoundaryObjs = path[pidx].numBoundObjs;
    }
}

/* Return path[pidx].edge[eidx].type: */
DB_API int db_hatch::ret_pathedgetype(int pidx, int eidx) {
    return (path!=NULL && pidx>-1 && pidx<npaths &&
            path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges) ?
                path[pidx].edge[eidx].type : DB_HATCHEDGEUNDEFINED;
}
/* Get the path[pidx].edge[eidx].u.line members: */
DB_API void db_hatch::get_pathedgeline(
    int       pidx,
    int       eidx,
    sds_real *p0xp,
    sds_real *p0yp,
    sds_real *p1xp,
    sds_real *p1yp) {

    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGELINE &&
        path[pidx].edge[eidx].u.line!=NULL) {

        if (p0xp!=NULL) *p0xp=path[pidx].edge[eidx].u.line->pt[0][0];
        if (p0yp!=NULL) *p0yp=path[pidx].edge[eidx].u.line->pt[0][1];
        if (p1xp!=NULL) *p1xp=path[pidx].edge[eidx].u.line->pt[1][0];
        if (p1yp!=NULL) *p1yp=path[pidx].edge[eidx].u.line->pt[1][1];
    }
}
/* Get the path[pidx].edge[eidx].u.arc members: */
DB_API void db_hatch::get_pathedgearc(
    int       pidx,
    int       eidx,
    sds_real *centxp,
    sds_real *centyp,
    sds_real *radp,
    sds_real *sangp,
    sds_real *eangp,
    int      *ccwp) {

    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGEARC &&
        path[pidx].edge[eidx].u.arc!=NULL) {

        if (centxp!=NULL) *centxp=path[pidx].edge[eidx].u.arc->cent[0];
        if (centyp!=NULL) *centyp=path[pidx].edge[eidx].u.arc->cent[1];
        if (radp  !=NULL) *radp  =path[pidx].edge[eidx].u.arc->rad;
        if (sangp !=NULL) *sangp =path[pidx].edge[eidx].u.arc->ang[0];
        if (eangp !=NULL) *eangp =path[pidx].edge[eidx].u.arc->ang[1];
        if (ccwp  !=NULL) *ccwp  =path[pidx].edge[eidx].u.arc->ccw;
    }
}

/* Get the path[pidx].edge[eidx].u.ellarc in the aggregate: */
DB_API void db_hatch::get_pathedgeellarc(int pidx, 
										 int eidx, 
										 db_hatchellarclink * & pEllarc)
	{
	if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGEELLARC &&
        path[pidx].edge[eidx].u.ellarc!=NULL) 
		pEllarc = path[pidx].edge[eidx].u.ellarc;
	else
		pEllarc = NULL;
	}


/* Get the path[pidx].edge[eidx].u.ellarc members: */
DB_API void db_hatch::get_pathedgeellarc(
    int       pidx,
    int       eidx,
    sds_real *centxp,
    sds_real *centyp,
    sds_real *smavectxp,
    sds_real *smavectyp,
    sds_real *min_majp,
    sds_real *sparamp,
    sds_real *eparamp,
    int      *ccwp) {

    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGEELLARC &&
        path[pidx].edge[eidx].u.ellarc!=NULL) {

        if (centxp   !=NULL) *centxp   =path[pidx].edge[eidx].u.ellarc->cent[0];
        if (centyp   !=NULL) *centyp   =path[pidx].edge[eidx].u.ellarc->cent[1];
        if (smavectxp!=NULL) *smavectxp=path[pidx].edge[eidx].u.ellarc->smavect[0];
        if (smavectyp!=NULL) *smavectyp=path[pidx].edge[eidx].u.ellarc->smavect[1];
        if (min_majp !=NULL) *min_majp =path[pidx].edge[eidx].u.ellarc->min_maj;
        if (sparamp  !=NULL) *sparamp  =path[pidx].edge[eidx].u.ellarc->param[0];
        if (eparamp  !=NULL) *eparamp  =path[pidx].edge[eidx].u.ellarc->param[1];
        if (ccwp     !=NULL) *ccwp     =path[pidx].edge[eidx].u.ellarc->ccw;
    }
}

/* Get the path[pidx].edge[eidx].u.spline in the aggregate */
DB_API void db_hatch::get_pathedgespline(int pidx, 
										 int eidx, 
										 db_hatchsplinelink * & pSpline)
	{
	if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGESPLINE &&
        path[pidx].edge[eidx].u.spline!=NULL) 
		pSpline = path[pidx].edge[eidx].u.spline;
	else
		pSpline = NULL;
	}



/* Get the path[pidx].edge[eidx].u.spline members -- including a ptr */
/* to the knot[] array, but excluding the control array: */
DB_API void db_hatch::get_pathedgespline(
    int       pidx,
    int       eidx,
    int      *rationalp,
    int      *periodicp,
    int      *degp,
    int      *nknotsp,
    int      *ncptsp,
    sds_real **knotpp) {

    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGESPLINE &&
        path[pidx].edge[eidx].u.spline!=NULL) {

        if (rationalp!=NULL) *rationalp=((path[pidx].edge[eidx].u.spline->flags&4)!=0);
        if (periodicp!=NULL) *periodicp=((path[pidx].edge[eidx].u.spline->flags&2)!=0);
        if (degp     !=NULL) *degp     =  path[pidx].edge[eidx].u.spline->deg;
        if (nknotsp  !=NULL) *nknotsp  =  path[pidx].edge[eidx].u.spline->nknots;
        if (ncptsp   !=NULL) *ncptsp   =  path[pidx].edge[eidx].u.spline->ncpts;
        if (knotpp   !=NULL) *knotpp   =  path[pidx].edge[eidx].u.spline->knot;
    }
}
/* Get the path[pidx].edge[eidx].u.spline->control[cidx] point: */
DB_API void db_hatch::get_pathedgesplinecontrol(
    int       pidx,
    int       eidx,
    int       cidx,
    sds_real *ptxp,
    sds_real *ptyp) {

    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGESPLINE &&
        path[pidx].edge[eidx].u.spline!=NULL &&
        path[pidx].edge[eidx].u.spline->control!=NULL &&
        cidx>-1 && cidx<path[pidx].edge[eidx].u.spline->ncpts) {

        if (ptxp!=NULL) *ptxp=path[pidx].edge[eidx].u.spline->control[cidx].pt[0];
        if (ptyp!=NULL) *ptyp=path[pidx].edge[eidx].u.spline->control[cidx].pt[1];
    }
}

/* Get the path[pidx].edge[eidx].u.spline->control[cidx] weight: */
DB_API void db_hatch::get_pathedgesplineweight(int pidx, int eidx, int cidx, sds_real * wtp)
{
    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGESPLINE &&
        path[pidx].edge[eidx].u.spline!=NULL &&
        path[pidx].edge[eidx].u.spline->control!=NULL &&
        cidx>-1 && cidx<path[pidx].edge[eidx].u.spline->ncpts) 

        if (wtp !=NULL) *wtp =path[pidx].edge[eidx].u.spline->control[cidx].wt;
}

// Get the pline in the aggregate
DB_API void db_hatch::get_pathedgepline(int pidx,
										db_hatchplinelink * & pLine)
	{
	int eidx = 0;
   if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGEPLINE &&
        path[pidx].edge[eidx].u.pline!=NULL) 
	   pLine = (path[pidx].edge[eidx].u.pline);
   else
	   pLine = NULL;
	}

/* Get the path[pidx].edge[0].u.pline members --  */
/* excluding the vert[] array: */
/* A polyline edge is only possible when it is the only edge present so no edge needs to be specified*/
DB_API void db_hatch::get_pathedgepline(
    int  pidx,
    int *closedp,
    int *hasbulgesp,
    int *nvertsp) {

	int eidx = 0;

    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGEPLINE &&
        path[pidx].edge[eidx].u.pline!=NULL) {

        if (closedp   !=NULL) *closedp   =((path[pidx].edge[eidx].u.pline->flags&1)!=0);
        if (hasbulgesp!=NULL) *hasbulgesp=((path[pidx].edge[eidx].u.pline->flags&2)!=0);
        if (nvertsp   !=NULL) *nvertsp   =  path[pidx].edge[eidx].u.pline->nverts;
    }
}
/* Get the path[pidx].edge[0].u.pline->vert[vidx] members: */
/* A polyline edge is only possible when it is the only edge present so no edge needs to be specified*/
DB_API void db_hatch::get_pathedgeplinevert(
    int  pidx,
    int  vidx,
    sds_real *ptxp,
    sds_real *ptyp,
    sds_real *bulgep) {

	int eidx = 0;

    if (path!=NULL && pidx>-1 && pidx<npaths &&
        path[pidx].edge!=NULL && eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].type==DB_HATCHEDGEPLINE &&
        path[pidx].edge[eidx].u.pline!=NULL &&
        path[pidx].edge[eidx].u.pline->vert!=NULL &&
        vidx>-1 && vidx<path[pidx].edge[eidx].u.pline->nverts) {

        if (ptxp  !=NULL) *ptxp  =path[pidx].edge[eidx].u.pline->vert[vidx].pt[0];
        if (ptyp  !=NULL) *ptyp  =path[pidx].edge[eidx].u.pline->vert[vidx].pt[1];
        if (bulgep!=NULL) *bulgep=path[pidx].edge[eidx].u.pline->vert[vidx].bulge;
    }
}

                /***** THE path[] ARRAY SETTERS *****/

/* Allocate path[] and set npaths: */
bool db_hatch::set_91(int p) 
{    
	RemovePaths();

	if (p > 0) 
	{ 
		npaths=p; 
		path=new db_hatchpathlink[npaths]; 
	}
	else
		npaths=0;


    return true;
}

/* Set path[pidx]'s values and allocate path[pidx].edge[] */
DB_API void db_hatch::set_path(int pidx, int flags, int nedges)
{
    if (path!=NULL && pidx>-1 && pidx<npaths) 
	{
        path[pidx].flags=flags;

		if (path[pidx].edge)
		{
			delete [] path[pidx].edge; 
			path[pidx].edge = NULL;
		}

        if (nedges>0) {
            path[pidx].nedges=nedges;
            path[pidx].edge=new db_hatchedgelink[path[pidx].nedges];
        }
		else
	        path[pidx].nedges = 0;

		// Old boundary objects should not be removed.  The will want to be kept in case of a modify.
/*		path[pidx].numBoundObjs = 0;
		if (path[pidx].boundObjs)
		{
			path[pidx].boundObjs->CleanAssociations(this->RetHandle());
			delete path[pidx].boundObjs; 
			path[pidx].boundObjs = NULL;
		}
*/
	}
}

// intended for undo/redo sets the path to the path link at this position
DB_API void db_hatch::set_path(int index, db_hatchpathlink *newPath)
{
	if (index < 0 || index >= npaths || !newPath) 
		return;

	path[index] = *newPath;
}


// Set path[pidx]'s boundary objects list 
// *** Intended to be used from file load where associations are already set up
// Note that a pointer to the objsList is stored in this class and will be destroyed on
// destruction or when a new objsList is assigned.  The user should not destroy this 
// object.  Also, if the numObjs is 0 the objsList should be NULL.
void db_hatch::set_pathboundaryobj(int pidx, int numObjs, db_hatchboundobjslink *objsList)
{
	if (path[pidx].boundObjs)
	{
		delete path[pidx].boundObjs; 
		path[pidx].boundObjs = NULL;
	}

	path[pidx].numBoundObjs = numObjs;
	path[pidx].boundObjs = objsList;
}

// Set path[pidx]'s boundary objects list 
// *** Intended to be used when a hatch is created/modified as the associations are cleaned and set
// Note that a pointer to the objsList is stored in this class and will be destroyed on
// destruction or when a new objsList is assigned.  The user should not destroy this 
// object.  Also, if the numObjs is 0 the objsList should be NULL.

// The last parameter allow dups is intended to be set to true on initial hatch creation. When 
// a hatch entitiy is created we want to make sure duplicate entries are not added when more 
// than one path uses the same boundary object.  When hatches are copied for undo, duplicates are
// needed, so the association defaults to true;

void db_hatch::set_pathBoundaryAndAssociate(int pidx, int numObjs, db_hatchboundobjslink *objsList, bool allowDups)
{
	if (path[pidx].boundObjs)
	{
		path[pidx].boundObjs->CleanAssociations(this->RetHandle(), NULL);
		delete path[pidx].boundObjs; 
		path[pidx].boundObjs = NULL;
	}

	path[pidx].numBoundObjs = numObjs;
	path[pidx].boundObjs = objsList;

	if (path[pidx].boundObjs)
		if (allowDups)
			path[pidx].boundObjs->CreateAssociations(this);
		else
			path[pidx].boundObjs->CreateAssocNoDups(this);
}

/* This is a utility for the setters below: */
void db_hatch::deledge(int pidx, int eidx) {
    /* Caller must verify that pidx and eidx are in range. */
    /* Free old data (by type, so the correct destructor is called): */
    switch (path[pidx].edge[eidx].type) {
        case DB_HATCHEDGELINE  : delete path[pidx].edge[eidx].u.line  ; break;
        case DB_HATCHEDGEARC   : delete path[pidx].edge[eidx].u.arc   ; break;
        case DB_HATCHEDGEELLARC: delete path[pidx].edge[eidx].u.ellarc; break;
        case DB_HATCHEDGESPLINE: delete path[pidx].edge[eidx].u.spline; break;
        case DB_HATCHEDGEPLINE : delete path[pidx].edge[eidx].u.pline ; break;
    }
    /* No need to set to NULL; caller will immediately re-assign. */
}
/* Set path[pidx].edge[eidx] for a line edge: */
DB_API void db_hatch::set_pathedgeline(
    int pidx,
    int eidx,
    sds_real p0x,
    sds_real p0y,
    sds_real p1x,
    sds_real p1y) {

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges) {

        deledge(pidx,eidx);
        path[pidx].edge[eidx].type=DB_HATCHEDGELINE;
        path[pidx].edge[eidx].u.line=new db_hatchlinelink;
        path[pidx].edge[eidx].u.line->pt[0][0]=p0x;
        path[pidx].edge[eidx].u.line->pt[0][1]=p0y;
        path[pidx].edge[eidx].u.line->pt[1][0]=p1x;
        path[pidx].edge[eidx].u.line->pt[1][1]=p1y;
    }
}
/* Set path[pidx].edge[eidx] for an arc edge: */
DB_API void db_hatch::set_pathedgearc(
    int      pidx,
    int      eidx,
    sds_real centx,
    sds_real centy,
    sds_real rad,
    sds_real sang,
    sds_real eang,
    int      ccw) {

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges) {

        deledge(pidx,eidx);
        path[pidx].edge[eidx].type=DB_HATCHEDGEARC;
        path[pidx].edge[eidx].u.arc=new db_hatcharclink;
        path[pidx].edge[eidx].u.arc->cent[0]=centx;
        path[pidx].edge[eidx].u.arc->cent[1]=centy;
        path[pidx].edge[eidx].u.arc->rad=fabs(rad);
        ic_normang(&sang,NULL); ic_normang(&eang,NULL);
        path[pidx].edge[eidx].u.arc->ang[0]=sang;
        path[pidx].edge[eidx].u.arc->ang[1]=eang;
        path[pidx].edge[eidx].u.arc->ccw=(ccw!=0);
    }
}
/* Set path[pidx].edge[eidx] for an elliptic arc edge: */
DB_API void db_hatch::set_pathedgeellarc(
    int      pidx,
    int      eidx,
    sds_real centx,
    sds_real centy,
    sds_real smavectx,
    sds_real smavecty,
    sds_real min_maj,
    sds_real sparam,
    sds_real eparam,
    int      ccw) {

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges) {

        deledge(pidx,eidx);
        path[pidx].edge[eidx].type=DB_HATCHEDGEELLARC;
        path[pidx].edge[eidx].u.ellarc=new db_hatchellarclink;
        path[pidx].edge[eidx].u.ellarc->cent[0]=centx;
        path[pidx].edge[eidx].u.ellarc->cent[1]=centy;
        path[pidx].edge[eidx].u.ellarc->smavect[0]=smavectx;
        path[pidx].edge[eidx].u.ellarc->smavect[1]=smavecty;
        path[pidx].edge[eidx].u.ellarc->min_maj=min_maj;
        ic_normang(&sparam,NULL); ic_normang(&eparam,NULL);
        path[pidx].edge[eidx].u.ellarc->param[0]=sparam;
        path[pidx].edge[eidx].u.ellarc->param[1]=eparam;
        path[pidx].edge[eidx].u.ellarc->ccw=(ccw!=0);
    }
}
/* Set path[pidx].edge[eidx] for a spline edge. */
/* The knot[] and control[] arrays get allocated -- but not filled. */
DB_API void db_hatch::set_pathedgespline(
    int pidx,
    int eidx,
    int rational,
    int periodic,
    int deg,
    int nknots,
    int ncpts) {

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges) {

        deledge(pidx,eidx);
        path[pidx].edge[eidx].type=DB_HATCHEDGESPLINE;
        path[pidx].edge[eidx].u.spline=new db_hatchsplinelink;
        path[pidx].edge[eidx].u.spline->flags=(rational) ? 4 : 0;
        if (periodic) path[pidx].edge[eidx].u.spline->flags|=2;
        path[pidx].edge[eidx].u.spline->deg=deg;
        if (nknots>0) {
            path[pidx].edge[eidx].u.spline->nknots=nknots;
            path[pidx].edge[eidx].u.spline->knot=new sds_real[nknots];
            memset(path[pidx].edge[eidx].u.spline->knot,0,
                nknots*sizeof(path[pidx].edge[eidx].u.spline->knot[0]));
        }
        if (ncpts>0) {
            path[pidx].edge[eidx].u.spline->ncpts=ncpts;
            path[pidx].edge[eidx].u.spline->control=
                new db_hatchsplinecontrollink[ncpts];
        }
    }
}
/* Set path[pidx].edge[eidx].u.spline->knot[kidx]: */
DB_API void db_hatch::set_pathedgesplineknot(
    int      pidx,
    int      eidx,
    int      kidx,
    sds_real knotval) {

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].u.spline!=NULL && kidx>-1 &&
        kidx<path[pidx].edge[eidx].u.spline->nknots)
            path[pidx].edge[eidx].u.spline->knot[kidx]=knotval;
}
/* Set the path[pidx].edge[eidx].u.spline->control[cidx] members: */
DB_API void db_hatch::set_pathedgesplinecontrol(
    int      pidx,
    int      eidx,
    int      cidx,
    sds_real ptx,
    sds_real pty) {

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].u.spline!=NULL && cidx>-1 &&
        cidx<path[pidx].edge[eidx].u.spline->ncpts) {

        path[pidx].edge[eidx].u.spline->control[cidx].pt[0]=ptx;
        path[pidx].edge[eidx].u.spline->control[cidx].pt[1]=pty;
    }
}
/* Set the path[pidx].edge[eidx].u.spline->control[cidx] weight: */
DB_API void db_hatch::set_pathedgesplineweight(
    int       pidx,
    int       eidx,
    int       cidx,
    sds_real  wt) {
    
	if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].u.spline!=NULL && cidx>-1 &&
        cidx<path[pidx].edge[eidx].u.spline->ncpts) {

        path[pidx].edge[eidx].u.spline->control[cidx].wt=wt;
    }
}

/* Set path[pidx].edge[0] for a pline edge. */
/* The vert[] array gets allocated -- but not filled. */
DB_API void db_hatch::set_pathedgepline(
    int pidx,
    int closed,
    int hasbulges,
    int nverts) {

	int eidx = 0;

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges) {

        deledge(pidx,eidx);
        path[pidx].edge[eidx].type=DB_HATCHEDGEPLINE;
        path[pidx].edge[eidx].u.pline=new db_hatchplinelink;
        path[pidx].edge[eidx].u.pline->flags=(closed!=0);
        if (hasbulges) path[pidx].edge[eidx].u.pline->flags|=2;
        if (nverts>0) {
            path[pidx].edge[eidx].u.pline->nverts=nverts;
            path[pidx].edge[eidx].u.pline->vert=
                new db_hatchplinevertlink[nverts];
        }
    }
}
/* Set the path[pidx].edge[0].u.pline->vert[vidx] members: */
DB_API void db_hatch::set_pathedgeplinevert(
    int      pidx,
    int      vidx,
    sds_real ptx,
    sds_real pty,
    sds_real bulge) {

	int eidx = 0;

    if (path!=NULL && pidx>-1 && pidx<npaths && path[pidx].edge!=NULL &&
        eidx>-1 && eidx<path[pidx].nedges &&
        path[pidx].edge[eidx].u.pline!=NULL && vidx>-1 &&
        vidx<path[pidx].edge[eidx].u.pline->nverts) {

        path[pidx].edge[eidx].u.pline->vert[vidx].pt[0]=ptx;
        path[pidx].edge[eidx].u.pline->vert[vidx].pt[1]=pty;
        path[pidx].edge[eidx].u.pline->vert[vidx].bulge=bulge;
    }
}

//Specific entgetter.  Returns 0 (OK) or -1 (no memory).
int 
db_hatch::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
    short fsh1;
    int rc;
    struct resbuf *sublist[2];
	int i,j,k;
	long nverts, nedges, nboundobj, nknots, nctrlpts;
	sds_point pt;
	short edgetype;

    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    // 10 (pt) Since 10,20 are 0 and remaining are seeds get first point from accessor
	get_10(pt);
    if ((sublist[0]=sublist[1]=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }

	// 210 Extrusion is part of the base class, so get it from accessor
	sds_point extru;
	get_210(extru);
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	//2 hatch pattern name
    if ((sublist[1]->rbnext=db_newrb(2,'S',patname))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// 70 solid flag
    fsh1=(unsigned char)solidfill;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// 71 assoc flag
    fsh1=(unsigned char)assoc;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// 91 num boundary paths
    if ((sublist[1]->rbnext=db_newrb(91,'L',&npaths))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Boundary path data for each path
	for (i = 0; i < npaths; i++)
	{
		//92 boundary path type flag
	    if ((sublist[1]->rbnext=db_newrb(92,'H',&(path[i].flags)))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		if (path[i].flags & 2)			//Edge is polyline
		{

			ASSERT(path[i].edge->type == DB_HATCHEDGEPLINE); //must be a polyline here

			//72 has bulge flag  
			short bulgeflag = (path[i].edge[0].u.pline->flags & 2) > 0; 
			if ((sublist[1]->rbnext=db_newrb(72,'H',&bulgeflag))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
			
			//73 is closed flag
			fsh1 = (path[i].edge[0].u.pline->flags & 1) > 0;  
			if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;

			//93 num polyline vertices
			nverts = path[i].edge[0].u.pline->nverts;
			if ((sublist[1]->rbnext=db_newrb(93,'L',&nverts))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;

			for (j = 0; j < nverts; j++)
			{
				// 10-20 vertex location
				pt[0] = path[i].edge[0].u.pline->vert[j].pt[0];				
				pt[1] = path[i].edge[0].u.pline->vert[j].pt[1];
				pt[2] = 0;
				if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
				sublist[1]=sublist[1]->rbnext;

				if (bulgeflag)
				{
					// 42 bulge
					if ((sublist[1]->rbnext=db_newrb(42,'R', &(path[i].edge[0].u.pline->vert[j].bulge)))==NULL) { rc=-1; goto out; }
					sublist[1]=sublist[1]->rbnext;
				}
			}				
		}

		else							//Non-polyline edge
		{
			//93 num edges in path
			nedges = path[i].nedges;
			if ((sublist[1]->rbnext=db_newrb(93,'L',&nedges))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;

			for (j = 0; j < nedges; j++)
			{
				//72 edge type
				edgetype = path[i].edge[j].type;
				if ((sublist[1]->rbnext=db_newrb(72,'H',&edgetype))==NULL) { rc=-1; goto out; }
				sublist[1]=sublist[1]->rbnext;

				switch (edgetype) {
				case DB_HATCHEDGELINE:
					{
						//10, 20 start point
						pt[0] = path[i].edge[j].u.line->pt[0][0];
						pt[1] = path[i].edge[j].u.line->pt[0][1];
						pt[2] = 0;

						if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//11, 21 end point
						pt[0] = path[i].edge[j].u.line->pt[1][0];
						pt[1] = path[i].edge[j].u.line->pt[1][1];
						pt[2] = 0;

						if ((sublist[1]->rbnext=db_newrb(11,'P',pt))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;
					}
					break;

				case DB_HATCHEDGEARC: 
					{
						//10, 20 center point
						pt[0] = path[i].edge[j].u.arc->cent[0];
						pt[1] = path[i].edge[j].u.arc->cent[1];
						pt[2] = 0;

						if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//40 radius
						if ((sublist[1]->rbnext=db_newrb(40,'R',&(path[i].edge[j].u.arc->rad)))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//50 start angle
						if ((sublist[1]->rbnext=db_newrb(50,'R',&(path[i].edge[j].u.arc->ang[0])))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//51 end angle
						if ((sublist[1]->rbnext=db_newrb(51,'R',&(path[i].edge[j].u.arc->ang[1])))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//73 is counter clockwise flag
						fsh1 = (unsigned char) path[i].edge[j].u.arc->ccw;
						if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;
					}
					break;

				case DB_HATCHEDGEELLARC: 
					{
						//10, 20 center point
						pt[0] = path[i].edge[j].u.ellarc->cent[0];
						pt[1] = path[i].edge[j].u.ellarc->cent[1];
						pt[2] = 0;
						if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//11, 21 end point of major axis relative to center
						pt[0] = path[i].edge[j].u.ellarc->smavect[0];
						pt[1] = path[i].edge[j].u.ellarc->smavect[1];
						pt[2] = 0;
						if ((sublist[1]->rbnext=db_newrb(11,'P',pt))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//40 length minor axis
						if ((sublist[1]->rbnext=db_newrb(40,'R',&(path[i].edge[j].u.ellarc->min_maj)))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//50 start angle
						if ((sublist[1]->rbnext=db_newrb(50,'R',&(path[i].edge[j].u.ellarc->param[0])))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//51 end angle
						if ((sublist[1]->rbnext=db_newrb(51,'R',&(path[i].edge[j].u.ellarc->param[1])))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//73 is counter clockwise flag
						fsh1 = (unsigned char) path[i].edge[j].u.ellarc->ccw;
						if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;			
					}
					break;

				case DB_HATCHEDGESPLINE:
					{
						//94 degree
						if ((sublist[1]->rbnext=db_newrb(94,'L',&(path[i].edge[j].u.spline->deg)))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//73 rational
						fsh1 = (path[i].edge[j].u.spline->flags & IC_SPLINE_RATIONAL) > 0;
						if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//74 periodic
						fsh1 = (path[i].edge[j].u.spline->flags & IC_SPLINE_PERIODIC) > 0;
						if ((sublist[1]->rbnext=db_newrb(74,'H',&fsh1))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//95 num knots
						nknots = path[i].edge[j].u.spline->nknots;
						if ((sublist[1]->rbnext=db_newrb(95,'L',&nknots))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;
						
						//96 num control points
						nctrlpts = path[i].edge[j].u.spline->ncpts;
						if ((sublist[1]->rbnext=db_newrb(96,'L',&nctrlpts))==NULL) { rc=-1; goto out; }
						sublist[1]=sublist[1]->rbnext;

						//40 knot values
						for (k = 0; k < nknots; k++)
						{
							if ((sublist[1]->rbnext=db_newrb(40,'R',&(path[i].edge[j].u.spline->knot[k])))==NULL) { rc=-1; goto out; }
							sublist[1]=sublist[1]->rbnext;					
						}

						//10,20 control points
						for (k = 0; k < nctrlpts; k++)
						{
							pt[0] = path[i].edge[j].u.spline->control[k].pt[0];
							pt[1] = path[i].edge[j].u.spline->control[k].pt[1];
							pt[2] = 0;
							if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
							sublist[1]=sublist[1]->rbnext;					
						}

						//42 weights (only if rational)
						if (path[i].edge[j].u.spline->flags & IC_SPLINE_RATIONAL)
						{
							for (k = 0; k < nctrlpts; k++)
							{
								if ((sublist[1]->rbnext=db_newrb(42,'R',&(path[i].edge[j].u.spline->control->wt)))==NULL) { rc=-1; goto out; }
								sublist[1]=sublist[1]->rbnext;					
							}
						}
					}
					break;

				default:
					ASSERT(FALSE);	// should never get here;
				}

			}  // end for each edge
		} //end if polyline else

		//97 num source boundary objects
		nboundobj = path[i].numBoundObjs;
		db_hatchboundobjs*	objectPos = NULL;
		db_handitem*		itemhandle = NULL;
		if(nboundobj && !path[i].boundObjs->getNext(&objectPos, &itemhandle))	/*D.G.*/
			nboundobj = 0;														//
		if ((sublist[1]->rbnext=db_newrb(97,'L',&nboundobj))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		// add boundary objects
		if (nboundobj > 0)
		{
			sds_name ename;
			objectPos = NULL;														/*D.G.*/
			itemhandle = NULL;														//
			while (path[i].boundObjs->getNext(&objectPos, &itemhandle)) 
			{
				ename[0] = (long) itemhandle;
				ename[1] = (long) dp;

				//330 reference to source boundary objects
				if ((sublist[1]->rbnext=db_newrb(330,'N', ename))==NULL) { rc=-1; goto out; }
				sublist[1]=sublist[1]->rbnext;
			}

		}
	} //end for each path

	// 75 hatch style
	if ((sublist[1]->rbnext=db_newrb(75,'H',&style))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	// 76 hatch pattern type
	if ((sublist[1]->rbnext=db_newrb(76,'H',&pattype))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	// 52 hatch pattern angle
	if ((sublist[1]->rbnext=db_newrb(52,'R',&ang))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	// 41 scale or spacing
	if ((sublist[1]->rbnext=db_newrb(41,'R',&scale))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	// 77 double flag
	fsh1 = (unsigned char) isdouble;
	if ((sublist[1]->rbnext=db_newrb(77,'H',&fsh1))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	// 78 num pattern def lines
	if ((sublist[1]->rbnext=db_newrb(78,'L',&npatlines))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	for (i = 0; i < npatlines; i++)
	{
		// 53 pat line angle
		if ((sublist[1]->rbnext=db_newrb(53,'R',&(patline[i].ang)))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		// 43 base x
		if ((sublist[1]->rbnext=db_newrb(43,'R',&(patline[i].base[0])))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		// 44 base y
		if ((sublist[1]->rbnext=db_newrb(44,'R',&(patline[i].base[1])))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;


		// 45 pattern line x offset
		if ((sublist[1]->rbnext=db_newrb(45,'R',&(patline[i].offset[0])))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		// 46 pattern line y offset
		if ((sublist[1]->rbnext=db_newrb(46,'R',&(patline[i].offset[1])))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		// 79 num dash length items
		if ((sublist[1]->rbnext=db_newrb(79,'R',&(patline[i].ndashes)))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		for (j = 0; j < patline[i].ndashes; j++)
		{
			// 49 dash length
			if ((sublist[1]->rbnext=db_newrb(49,'R',&(patline[i].dash[j])))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
		}	
	}

	// 47 pixel size
	if ((sublist[1]->rbnext=db_newrb(47,'R',&pixelsz))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	// 98 num seed points
	if ((sublist[1]->rbnext=db_newrb(98,'L',&nseeds))==NULL) { rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	for (i = 0; i < nseeds; i++)
	{
		// 10,20 seed points
		pt[0] = seed[i][0];
		pt[1] = seed[i][1];
		pt[2] = 0;
		if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

int db_hatch::entmod(struct resbuf *modll, db_drawing *dp) 
{
/*
**  Returns: See db_handitem::entmod().
*/

// Note: this method will not have the same ability to error check as some of the
// other entities, due to the nested configuration of a hatch.  For example, a 10 can mean the
// elevation point, a polyline vertex, a line start point... To produce a db_hatch from an resbuf
// this method will be dependent on the position inside the resbuf.

    int rc, i, j, k;
    struct resbuf *curlink;
    db_handitem *thiship;
	int edgetype = DB_HATCHEDGEUNDEFINED;
	bool did10undef = false;

    rc=0; thiship=(db_handitem *)this;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    del_xdict();

    curlink=modll;

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (curlink!=NULL &&
        (curlink->restype==-1 ||
         curlink->restype== 0 ||
         curlink->restype== 5)) curlink=curlink->rbnext;

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) {

        /* Call the correct setter: */
        switch (curlink->restype) {
		case 10:
			{
				// Elevation Point.  Seed Points should be set under 98.
				// since this 10 occurs first in file ensures another 10 is not set here
				if (!did10undef) {
					set_10(curlink->resval.rpoint);
					did10undef = true;
				}
			}
			break;

        case 210: 
			set_210(curlink->resval.rpoint); 
			break;

		case 2:
			set_2(curlink->resval.rstring);
			break;

		case 70:
			set_70(curlink->resval.rint);
			break;

		case 71:
			set_71(curlink->resval.rint);
			break;

		case 91:
			{
				// sets 91 and allocates path data
				set_91(curlink->resval.rlong);

				//set path boundary data
				for (i = 0; !rc && i < npaths; i++)
				{
					curlink = curlink->rbnext;
					// first item must be a flag so we can tell if it's a polyline
					if (curlink == NULL || curlink->restype != 92) {
						rc = -5;
						break;
					}
					
					int flag = curlink->resval.rint;

					if (flag & 2) {		//polyline edge
						set_path(i, flag, 1); // polylines only have one edge

						int bulgeflag = 0, closedflag = 0;
						int nplineverts = 0;
						sds_point *vertices = NULL;	//2 d vertex stored in first 2, bulge stored in z

						// 3 items in polyline that are not multiple entries 
						// multi item entries are set under 93
						for (j = 0; !rc && j < 3; j++) 
						{
							curlink = curlink->rbnext;
							if (curlink == NULL || curlink->restype == -3) {
								rc = -5;
								break;
							}

							switch (curlink->restype) 
							{
							case 72:
								bulgeflag = curlink->resval.rint;
								break;

							case 73:
								closedflag = curlink->resval.rint;
								break;

							case 93:
								nplineverts = curlink->resval.rlong;
								vertices = new sds_point[nplineverts];

								for (k = 0; !rc && k < nplineverts; k++)
								{
									curlink = curlink->rbnext;
									// next value must be vertex loc - 10
									if (curlink == NULL || curlink->restype != 10) {
										rc = -5;
										break;
									}
									
									vertices[k][0] = curlink->resval.rpoint[0];
									vertices[k][1] = curlink->resval.rpoint[1];

									// if bulge is specified it should be next value otherwise set to 0;
									if (curlink->rbnext != NULL && curlink->rbnext->restype == 42)
									{
										//bulge specifed add it
										curlink = curlink->rbnext;
										vertices[k][2] = curlink->resval.rreal;
									}
									else	// set bulge to 0
										vertices[k][2] = 0;
								}
							} // switch

						}// for polyline items

						// set polyline and vertex values
						set_pathedgepline(i, closedflag, bulgeflag, nplineverts);

						for (j = 0; j < nplineverts; j++)
							set_pathedgeplinevert(i, j, vertices[j][0], vertices[j][1], vertices[j][2]);

						if (vertices) {
							delete [] vertices;
							vertices = NULL;
						}
					}
					else {				//non polyline edge
						curlink = curlink->rbnext;
						// next item must be number of edges in path
						if (curlink == NULL || curlink->restype != 93) {
							rc = -5;
							break;
						}

						int numedges = curlink->resval.rlong;
						set_path(i, flag, numedges);

						for (j = 0; !rc && j < numedges; j++)
						{
							curlink = curlink->rbnext;
							// next item must be edgetype
							if (curlink == NULL || curlink->restype != 72) {
								rc = -5;
								break;
							}

							switch (curlink->resval.rint)
							{
							case 1: //line
								{
									sds_real startx, starty, endx, endy;
									int did10 = 0, did11 = 0;

									// 2 values in line
									for (k = 0; !rc && k < 2; k++) {
										curlink = curlink->rbnext;
										if (curlink == NULL || curlink->restype == -3) {
											rc = -5;
											break;
										}

										switch (curlink->restype) {
										case 10:
											startx = curlink->resval.rpoint[0];
											starty = curlink->resval.rpoint[1];
											did10 = 1;
											break;
							
										case 11:
											endx = curlink->resval.rpoint[0];
											endy = curlink->resval.rpoint[1];
											did11 = 1;
											break;

										default:
											rc = -5;
										}
									}
									
									if (did10 && did11)
										set_pathedgeline(i, j, startx, starty, endx, endy);
									else
										rc = -5;
								}
								break;

							case 2: //circular arc
								{
									sds_real centx, centy, radius, startang, endang;
									int isccw;
									int did10 = 0, did40 = 0, did50 = 0, did51 = 0, did73 = 0;

									// 5 values in circular arc
									for (k = 0; !rc && k < 5; k++)
									{
										curlink = curlink->rbnext;
										if (curlink == NULL || curlink->restype == -3) {
											rc = -5;
											break;
										}

										switch (curlink->restype) {
										case 10:
											centx = curlink->resval.rpoint[0];
											centy = curlink->resval.rpoint[1];
											did10 = 1;
											break;

										case 40:
											radius = curlink->resval.rreal;
											did40 = 1;
											break;

										case 50:
											startang = curlink->resval.rreal;
											did50 = 1;
											break;

										case 51:
											endang = curlink->resval.rreal;
											did51 = 1;
											break;

										case 73:
											isccw = curlink->resval.rint;
											did73 = 1;
											break;

										default:
											rc = -5;
										}
									}

									if (did10 && did40 && did50 && did51 && did73)
										set_pathedgearc(i, j, centx, centy, radius, startang, endang, isccw);
									
									else
										rc = -5;
								}
								break;

							case 3: // elliptical arc
								{
									sds_real centx, centy, endmajx, endmajy, minorlen, startang, endang;
									int isccw;
									int did10 = 0, did11 = 0, did40 = 0, did50 = 0, did51 = 0, did73 = 0;

									// 6 values in ellipitical arc
									for (k = 0; !rc && k < 6; k++)
									{
										curlink = curlink->rbnext;
										if (curlink == NULL || curlink->restype == -3) {
											rc = -5;
											break;
										}

										switch (curlink->restype) {
										case 10:
											centx = curlink->resval.rpoint[0];
											centy = curlink->resval.rpoint[1];
											did10 = 1;
											break;

										case 11:
											endmajx = curlink->resval.rpoint[0];
											endmajy = curlink->resval.rpoint[1];
											did11 = 1;
											break;

										case 40:
											minorlen = curlink->resval.rreal;
											did40 = 1;
											break;

										case 50:
											startang = curlink->resval.rreal;
											did50 = 1;
											break;

										case 51:
											endang = curlink->resval.rreal;
											did51 = 1;
											break;

										case 73:
											isccw = curlink->resval.rint;
											did73 = 1;
											break;

										default:
											rc = -5;
										}
									}

									if (did10 && did11 && did40 && did50 && did51 && did73)
										set_pathedgeellarc(i, j, centx, centy, endmajx, endmajy, minorlen, startang, endang, isccw);
									
									else
										rc = -5;
								}
								break;

							case 4: // spline
								{
									long degree, numknots, numctrlpts;
									int rational, periodic;
									int did94(0), did73(0), did74(0), did95(0), did96(0);

									// 5 fixed values in spline arc
									for (k = 0; !rc && k < 5; k++)
									{
										curlink = curlink->rbnext;
										if (curlink == NULL || curlink->restype == -3) {
											rc = -5;
											break;
										}

										switch (curlink->restype) {
										case 94:
											degree = curlink->resval.rlong;
											did94 = 1;
											break;

										case 73:
											rational = curlink->resval.rint;
											did73 = 1;
											break;

										case 74:
											periodic = curlink->resval.rint;
											did74 = 1;
											break;

										case 95:
											numknots = curlink->resval.rlong;
											did95 = 1;
											break;

										case 96:
											numctrlpts = curlink->resval.rlong;
											did96 = 1;
											break;

										default:
											rc = -5;
										}
									}

									if (!did94 || !did73 || !did74 || !did95 || !did96) {
										rc = -5;
										break;
									}

									set_pathedgespline(i, j, rational, periodic, degree, numknots, numctrlpts);

									//set knot values;
									for (k = 0; !rc && k < numknots; k++)
									{
										curlink = curlink->rbnext;
										if (curlink == NULL || curlink->restype != 40) {
											rc = -5;
											break;
										}

										set_pathedgesplineknot(i, j, k, curlink->resval.rreal);
									}

									//set control points
									for (k = 0; !rc && k < numctrlpts; k++) {
										curlink = curlink->rbnext;
										if (curlink == NULL || curlink->restype != 10) {
											rc = -5;
											break;
										}

										set_pathedgesplinecontrol(i, j, k, curlink->resval.rpoint[0], curlink->resval.rpoint[1]);
									}
									
									//set weights only if rational
									if (rational)
									{
										for (k = 0; !rc && k < numctrlpts; k++) {
											curlink = curlink->rbnext;
											if (curlink == NULL || curlink->restype != 42) {
												rc = -5;
												break;
											}

											set_pathedgesplineweight(i, j, k, curlink->resval.rreal);
										}
									}
								}

								break;

							default: // undefined
								rc = -5;
							}
	
						}// for each edge
					}// else not polyline

					// boundary objects
					//
					curlink = curlink->rbnext;
					if (curlink == NULL || curlink->restype != 97) {
						rc = -5;
						break;
					}

					int numobjs = curlink->resval.rlong;

					db_hatchboundobjslink *boundobjs = new db_hatchboundobjslink(dp);

					for (j = 0; !rc && j < numobjs; j++)
					{
						curlink = curlink->rbnext;
						if (curlink == NULL || curlink->restype != 330) {
							rc = -5;
							break;
						}

						boundobjs->addBoundaryObj((db_handitem*)curlink->resval.rlname[0]);
					}

					// m_createAssocFlag is usually turned off for commands that copy the
					// entity so the pointers can be resolved before new associations are 
					// added
					if (m_createAssocFlag)
						set_pathBoundaryAndAssociate(i, numobjs, boundobjs);
					else 
						set_pathboundaryobj(i, numobjs, boundobjs);
				}
			}
			break;

		case 75:
			set_75 (curlink->resval.rint);
			break;

		case 76:
			set_76(curlink->resval.rint);
			break;

		case 52:
			set_52(curlink->resval.rreal);
			break;

		case 41:
			set_41(curlink->resval.rreal);
			break;

		case 77:
			set_77(curlink->resval.rint);
			break;

		case 78:
			{
				// sets 78 and allocates 
				set_78(curlink->resval.rlong);

				// set each pattern data subentity.
				for (i = 0; !rc && i < npatlines; i++)
				{
					sds_real angle = 0;
					sds_real basex = 0, basey = 0;
					sds_real offsetx = 0 , offsety = 0;
					int numdashes;
					sds_real *dash = NULL;

					// there are 6 fixed length fields in the pattern data 
					// dash length which can have multiple entries will be set inside
					// 79 - number of dash length entities.
					for (j = 0; !rc && j < 6; j++)
					{
						curlink = curlink->rbnext;
						if (curlink == NULL || curlink->restype == -3) {
							rc = -5;
							break;
						}

						switch (curlink->restype) {
						case 53:
							angle = curlink->resval.rreal;
							break;

						case 43: 
							basex = curlink->resval.rreal;
							break;

						case 44:
							basey = curlink->resval.rreal;
							break;

						case 45: 
							offsetx = curlink->resval.rreal;
							break;

						case 46:
							offsety = curlink->resval.rreal;
							break;

						case 79:
							numdashes = curlink->resval.rlong;
							dash = new sds_real[numdashes];

							for (k = 0; !rc && k < numdashes; k++)
							{
								curlink = curlink->rbnext;
								if (curlink == NULL || curlink->restype != 49) 
									rc = -5;

								else
									dash[k] = curlink->resval.rreal;
							}
							break;
						}

					}

					if (!rc)
					{
						set_patline(i, angle, basex, basey, offsetx, offsety, numdashes);
						
						for (j = 0; j < numdashes; j++)
							set_patlinedash(i, j, dash[j]);

					}

					if (dash) {
						delete [] dash;
						dash = NULL;
					}
				}
			}
			break;

		case 47:
			set_47(curlink->resval.rreal);
			break;

		case 98:	//Also allocates the seed 10,20 pairs
			{
				nseeds = curlink->resval.rlong;

				if (seed)
					delete [] seed;

				if (!nseeds)
				{
					seed = NULL;
					break;
				}

				seed = new sds_real[nseeds][2];

				// these 10/20 values are expected to be right after 98 to 
				// distinguish from other 10 values.
		        for (i = 0; !rc && i < nseeds; i++) {
					curlink = curlink->rbnext;

					if (curlink == NULL || curlink->restype != 10)
					{
						rc = -5;
						break;
					}

					seed[i][0] = curlink->resval.rpoint[0];
					seed[i][1] = curlink->resval.rpoint[1];
				}
			}
			break;

        default:  /* The common entity or table record groups. */
           rc=thiship->entmod1common(&curlink,dp);
	
		}
	}

	if (!assoc)
		BreakAssociation(false);

    if (!rc && curlink!=NULL) rc=set_eed(curlink,dp); // EBATECH(CNBR) - Hatch object can add EED.


out:
    return rc;
}

// additional seed management functions

typedef sds_real sds_seed[2];
bool db_hatch::set_seed(sds_point p, int i)
{
	bool retval;
	
	if ((i < 0) || (i > nseeds))
		retval = false;
	else if (i == nseeds)
	{
		sds_seed * new_seed=new sds_seed[nseeds + 1];
		if (new_seed)
		{
			memcpy(new_seed, seed, nseeds*sizeof(sds_seed));
			new_seed[nseeds][0] = p[0];
			new_seed[nseeds][1] = p[1];
			nseeds = nseeds+1;
			delete [] seed;
			seed = new_seed;
			retval = true;
		}
		else
		{
			retval = false;
		}
	}
	else
	{
		seed[i][0] = p[0];
		seed[i][1] = p[1];
		retval = true;
	}
	return retval;
}

bool db_hatch::get_seed(sds_point p, int i)
{
	bool retval;
	if ((0 <= i) && (i < nseeds) && (p != NULL))
	{
		p[0] = seed[i][0];
		p[1] = seed[i][1];
		p[2] = elev;
		retval = true;
	}
	else
	{
		p[0]=p[1]=p[2]=0.0;
		retval = false;
	}
	return retval;
}

bool db_hatch::del_seed(int i)
{
	bool retval;

	if ( (i < 0) || (i >= nseeds))
	{
		retval = false;
	}
	else
	{
		// we won't reallocate space
		// move every seed value above index i down one
		for (int j = i + 1; j < nseeds; j++)
		{
			seed[j-1][0] = seed[j][0];
			seed[j-1][1] = seed[j][1];
		}
		// then reduce the number of seeds by 1
		--nseeds;
		retval = true;
	}
	return retval;
}

bool db_hatch::ReactorUpdate (db_objhandle &reactor)
{
	bool succeeded = true;

	sds_point pt;
	get_210(pt);

	CBHatchBuilder *hatchBuilder = IcadSharedGlobals::GetHatchFactory()->CreateHatchBuilder(pt);

	//save copy of hatch paths in case the modify fails
	int storednumpaths = npaths;
	db_hatchpathlink *storedpaths = new db_hatchpathlink[npaths];

	for (int i = 0; i < storednumpaths; i++)
		storedpaths[i] = path[i];

	hatchBuilder->set_hatch(this, false);

	if (hatchBuilder->ModifyHatch(&reactor) != SUCCESS)
	{
		// reinstate previous hatch and break the associations
		set_91(storednumpaths);
		for (int i = 0; i < storednumpaths; i++)
			path[i] = storedpaths[i];

		BreakAssociation(true);
		succeeded = false;
	}

	delete [] storedpaths;
	delete hatchBuilder;

	return succeeded;
}

//Removes associations from hatch and cleans up associations from boundary object to hatch
void db_hatch::BreakAssociation(bool dispMessage)
{
	if (dispMessage)
		IcadSharedGlobals::CallCmdMessage(ResourceString(DB_HATCH_ASSOC_BROKEN, "\nHatch associativity removed."));

	for (int i = 0; i < npaths; i++)
		set_pathBoundaryAndAssociate(i, 0, NULL);

	// set assoc flag to false
	set_71(0);  
}

//break the association that is input then call break associations to break other associations (avoids 
//circular reference)
void db_hatch::BreakAssociation(db_handitem *entity, bool dispMessage) 
{	
	for (int i = 0; i < npaths; i++)
	{
		if (path[i].boundObjs)
		{
			path[i].boundObjs->removeBoundaryObj(entity);
		}
	}

	BreakAssociation(dispMessage);
}

bool db_hatch::findBoundaryObj(const db_objhandle &objhandle) const
{
	for (int i = 0; i < npaths; i++)
	{
		if(path[i].boundObjs && path[i].boundObjs->findBoundaryObj(objhandle))
			return true;
	}
	return false;
}

//Basically, it checks if the associated entities are in the selection set. Some entities like hatch
//will need to implement this function so it will break the assoc if the boundary is not in the selection
//set.  Other entities like leader do not break the association on commands like rotate so they do not
//need implement.
void db_hatch::CheckAssociationInSet(db_drawing* drawing, const sds_name selSet)
{
	if(!drawing || !selSet)
		return;

	CSelectionSet*		pSS = (CSelectionSet*)selSet[0];
	db_hatchboundobjs*	pItem;
	db_handitem*		pBoundary;

	for(int i = npaths; i--; )
	{
		pItem = 0;
		pBoundary = 0;

		if(path[i].boundObjs)
		{
			while(path[i].boundObjs->getNext(&pItem, &pBoundary))
			{
				if((long)drawing != pSS->m_drawing && pSS->m_asMap.find(pBoundary->RetHandle()) == pSS->m_asMap.end())
				{
					//break the association if any of the boundary entities are not in the selection set
					BreakAssociation(true);
					return;
				}
			}
		}
	}
}

void db_hatch::RemovePaths()
{
	for (int i = 0; i < npaths; i++)
	{
		if (path[i].boundObjs)
		{
			path[i].boundObjs->CleanAssociations(this->RetHandle(), this);
			delete path[i].boundObjs;
			path[i].boundObjs = NULL;

			path[i].numBoundObjs = 0;
		}
	}

	if (path)
	{
		delete [] path; 
		path = NULL; 
	}
}

bool db_hatch::notifyDeleted(db_handitem* hip)
{
	if(hip == this || findBoundaryObj(hip->RetHandle()))
		return true;
	return false;
}
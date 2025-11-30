/* C:\ICAD\PRJ\LIB\DB\WIPEOUT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

#include "OdaCommon.h"
#include "..\..\..\Licensed\DWGdirect\Include\ResBuf.h" // clash with icadlib\resbuf.h
/**************************** db_wipeout *****************************/


/*F*/
db_wipeout::db_wipeout(void) : db_entity(DB_WIPEOUT) {
	classversion=0;
	pt0[0]=pt0[1]=pt0[2]=0.0;
	size[0]=size[1]=0.0;
	uvec[0]=uvec[1]=uvec[2]=0.0;
	vvec[0]=vvec[1]=vvec[2]=0.0;
	clipping=0;
	brightness=0;
	contrast=0;
	fade=0;
	clipboundtype=0;
	displayprops=0;
	numclipverts=0;
	rectclipvert0[0]=rectclipvert0[1]=rectclipvert1[0]=rectclipvert1[1]=0.0;
	polyclipvertblob=grblob=NULL;
	grblobsz=0;
	imagedefobjhandle=NULL;
	imagedefreactorobjhandle=NULL;
	m_pClipPoints = NULL;
	m_pResbufChain = NULL;
}

db_wipeout::db_wipeout(db_drawing *dp) : db_entity(DB_WIPEOUT,dp) {
	classversion=0;
	pt0[0]=pt0[1]=pt0[2]=0.0;
	size[0]=size[1]=0.0;
	uvec[0]=uvec[1]=uvec[2]=0.0;
	vvec[0]=vvec[1]=vvec[2]=0.0;
	clipping=0;
	brightness=0;
	contrast=0;
	fade=0;
	clipboundtype=0;
	displayprops=0;
	numclipverts=0;
	rectclipvert0[0]=rectclipvert0[1]=rectclipvert1[0]=rectclipvert1[1]=0.0;
	polyclipvertblob=grblob=NULL;
	grblobsz=0;
	imagedefobjhandle=NULL;
	imagedefreactorobjhandle=NULL;
	m_pClipPoints = NULL;
	m_pResbufChain = NULL;
}

db_wipeout::db_wipeout(const db_wipeout &sour) :
    db_entity(sour) {  /* Copy constructor */

	}

db_wipeout::~db_wipeout(void) 
{
	sds_relrb(m_pResbufChain);

	delete [] m_pClipPoints;
    delete [] grblob;
	delete [] polyclipvertblob;
	}

void db_wipeout::set_numclipverts(long cv) 
{ 
	numclipverts=cv; 
	m_pClipPoints = new sds_point[cv];
}

void db_wipeout::set_clipVert(int index, sds_real *r)
{
	ASSERT(index >= 0 && index < numclipverts);
	m_pClipPoints[index][0] = r[0];
	m_pClipPoints[index][1] = r[1];
	m_pClipPoints[index][2] = 0.0;

}

void db_wipeout::get_clipVert(int index, sds_real *r)
{
	ASSERT(index >= 0 && index < numclipverts);
	r[0] = m_pClipPoints[index][0];
	r[1] = m_pClipPoints[index][1];
	r[2] = 0.0;
}
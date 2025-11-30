/* C:\ICAD\PRJ\LIB\DB\HANDITEM.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "appid.h"
#include "dimstyletabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "ucstabrec.h"
#include "viewtabrec.h"
#include "vporttabrec.h"
#include "vxtabrec.h"
#include "objects.h"
#include "DbAcisEntity.h"
#include "DbGroup.h"
#include "DbPlotSettings.h"
#include "DbLayout.h"
#include "DbMText.h"
#include "DbSortentsTable.h"

// This is preventing the new DWGdirect Libs version 1.11 from
// linking. MHB
//#ifdef _DEBUG	
//#undef DEBUG_NEW  // this is necessary if the file is MFC-dependent  
//#define _CRTDBG_MAP_ALLOC	
//#include "crtdbg.h"  
//#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
//#define new DEBUG_NEW 
//#endif	

char *db_handitem::db_preactmarker="{ACAD_REACTORS";
char *db_xdictmarker ="{ACAD_XDICTIONARY";

static int checkXdSize(const struct sds_resbuf *prbEntData, long *plMemUsed) ;


/*************************** db_handitem ****************************/


/*F*/
db_handitem::db_handitem(void) : m_reactors(NULL) {
	type=0;

	eed=NULL;
	xdict=NULL;
	next=NULL;

	m_reactors = new ReactorSubject;
	m_createAssocFlag = true;

	m_parentBlock = NULL;
}
db_handitem::db_handitem(int ptype) : m_reactors(NULL) {
	type=(char)((ptype>-1 && ptype<DB_NHITYPES) ? ptype : 0);

	eed=NULL;

	xdict=NULL;
	next=NULL;

	m_reactors = new ReactorSubject;
	m_createAssocFlag = true;

	m_parentBlock = NULL;
}

//Don't want to copy reactors or association will be incorrect.  If new entity should be associated
//then that should be added separately
db_handitem::db_handitem(const db_handitem &sour) : m_reactors(NULL) {	/* Copy constructor */
	next=NULL;
	type=sour.type;

	handle=sour.RetHandle();
	eed=NULL;
	if (sour.eed!=NULL) {
		short fsh1;
		memcpy(&fsh1,sour.eed,2);
		if (fsh1>0) {
			fsh1+=2;
			eed=new char[fsh1];
			memcpy(eed,sour.eed,fsh1);
		}
	}

	xdict=NULL; if (sour.xdict!=NULL) xdict=new db_hireflink(*sour.xdict);

	m_reactors = new ReactorSubject;
	m_createAssocFlag = true;

	m_parentBlock = NULL;
}

db_handitem::~db_handitem(void)
{
//4M Spiros Item:154->
/*
	delete [] eed;
	delete xdict;
*/
	if(eed) delete [] eed;
   if(xdict) delete xdict;
	eed=NULL; xdict=NULL;
//4M Spiros Item:154<-

	// note currently association has only been considered for entities - it will need to be implemented
	// as appropriate for tables, and objects
	if (m_reactors)
	{
		if (db_hitypeinfo[type].typetype == DB_ENTITY)
			m_reactors->BreakAssociation(this, false);

		delete m_reactors;
	}
}

int ObjhandleCompare(db_objhandle *a, db_objhandle *b)
	{
	if (a->handle[1]!=b->handle[1])
		return a->handle[1]-b->handle[1];
	else return a->handle[0]-b->handle[0];
	}


// this is a slightly modified copy of adIncrementObjhandle
short incrementobjhandle(unsigned char *cthsptr)
{
	register short i;
	short carry;

	carry=1;
	for (i=7; i>=0; i--) {
		cthsptr[i]++;
		if (cthsptr[i]!='\0') {
			carry=0;
			break;
		}
	}
	return !carry;
}

/*F*/
void db_handitem::assignhand(db_drawing *dp)
	{
/*
**	If handles are on and handles haven't overflowed and no
**	handle has been assigned yet, assign it from HANDSEED and
**	increment HANDSEED.  Note the direct-access to the header
**	buffer and that we're counting on DOS byte order (low byte
**	first):
*/
	char *headerbuf, fs1[DB_MAXHANDSTRLEN+10];

	if (dp==NULL)
		return;
	if ((headerbuf=dp->ret_headerbuf())==NULL)
		return;
	if (!headerbuf[db_oldsysvar[DB_QHANDLES].bc])
		return;
	if (dp->ret_handoverflow())
		return;
	if (RetHandle()!=NULL)	// has handle already
		return;

	db_convhand(headerbuf+db_oldsysvar[DB_QHANDSEED].bc,fs1,0,2);
	if (!fs1[0] ||
		(fs1[0]=='0' && !fs1[1]))
		{
		fs1[0]='1';
		fs1[1]=0;
		}
	handle=fs1;
	if (!incrementobjhandle((unsigned char *)(headerbuf+db_oldsysvar[DB_QHANDSEED].bc)))
		{
			memset(headerbuf+db_oldsysvar[DB_QHANDSEED].bc,0xFF,8);
			dp->set_handoverflow(1);
		}
	}

/*F*/
db_handitem *db_handitem::newcopy(void) {
/*
**	Looks at the type and creates a new copy via the appropriate
**	copy constructor.  Returns a pointer to the new entity,
**	table record, or object as a db_handitem pointer.
*/
	db_handitem *newhip=NULL;

	switch (type) {
		case DB_APPIDTABREC:	   newhip=new		db_appidtabrec(*((		db_appidtabrec *)this)); break;
		case DB_BLOCKTABREC:	   newhip=new		db_blocktabrec(*((		db_blocktabrec *)this)); break;
		case DB_DIMSTYLETABREC:    newhip=new	 db_dimstyletabrec(*((	 db_dimstyletabrec *)this)); break;
		case DB_LAYERTABREC:	   newhip=new		db_layertabrec(*((		db_layertabrec *)this)); break;
		case DB_LTYPETABREC:	   newhip=new		db_ltypetabrec(*((		db_ltypetabrec *)this)); break;
		case DB_STYLETABREC:	   newhip=new		db_styletabrec(*((		db_styletabrec *)this)); break;
		case DB_UCSTABREC:		   newhip=new		  db_ucstabrec(*((		  db_ucstabrec *)this)); break;
		case DB_VIEWTABREC: 	   newhip=new		 db_viewtabrec(*((		 db_viewtabrec *)this)); break;
		case DB_VPORTTABREC:	   newhip=new		db_vporttabrec(*((		db_vporttabrec *)this)); break;
		case DB_VXTABREC:		   newhip=new		   db_vxtabrec(*((		   db_vxtabrec *)this)); break;
		case DB_3DFACE: 		   newhip=new			 db_3dface(*((			 db_3dface *)this)); break;
		case DB_3DSOLID:		   newhip=new			db_3dsolid(*((			db_3dsolid *)this)); break;
		case DB_ARC:			   newhip=new				db_arc(*((				db_arc *)this)); break;
		case DB_ATTDEF: 		   newhip=new			 db_attdef(*((			 db_attdef *)this)); break;
		case DB_ATTRIB: 		   newhip=new			 db_attrib(*((			 db_attrib *)this)); break;
		case DB_BLOCK:			   newhip=new			  db_block(*((			  db_block *)this)); break;
		case DB_BODY:			   newhip=new			   db_body(*((			   db_body *)this)); break;
		case DB_CIRCLE: 		   newhip=new			 db_circle(*((			 db_circle *)this)); break;
		case DB_DIMENSION:		   newhip=new		  db_dimension(*((		  db_dimension *)this)); break;
		case DB_ELLIPSE:		   newhip=new			db_ellipse(*((			db_ellipse *)this)); break;
		case DB_ENDBLK: 		   newhip=new			 db_endblk(*((			 db_endblk *)this)); break;
		case DB_HATCH:			   newhip=new			  db_hatch(*((			  db_hatch *)this)); break;
		case DB_IMAGE:			   newhip=new			  db_image(*((			  db_image *)this)); break;
		case DB_INSERT: 		   newhip=new			 db_insert(*((			 db_insert *)this)); break;
		case DB_LEADER: 		   newhip=new			 db_leader(*((			 db_leader *)this)); break;
		case DB_LINE:			   newhip=new			   db_line(*((			   db_line *)this)); break;
		case DB_LWPOLYLINE: 	   newhip=new		 db_lwpolyline(*((		 db_lwpolyline *)this)); break;
		case DB_MLINE:			   newhip=new			  db_mline(*((			  db_mline *)this)); break;
		case DB_MTEXT:			   newhip=new			  CDbMText(*((			  CDbMText *)this)); break;
		case DB_OLE2FRAME:		   newhip=new		  db_ole2frame(*((		  db_ole2frame *)this)); break;
		case DB_POINT:			   newhip=new			  db_point(*((			  db_point *)this)); break;
		case DB_POLYLINE:		   newhip=new		   db_polyline(*((		   db_polyline *)this)); break;
		case DB_RAY:			   newhip=new				db_ray(*((				db_ray *)this)); break;
		case DB_REGION: 		   newhip=new			 db_region(*((			 db_region *)this)); break;
		case DB_SEQEND: 		   newhip=new			 db_seqend(*((			 db_seqend *)this)); break;
		case DB_SHAPE:			   newhip=new			  db_shape(*((			  db_shape *)this)); break;
		case DB_SOLID:			   newhip=new			  db_solid(*((			  db_solid *)this)); break;
		case DB_SPLINE: 		   newhip=new			 db_spline(*((			 db_spline *)this)); break;
		case DB_TEXT:			   newhip=new			   db_text(*((			   db_text *)this)); break;
		case DB_TOLERANCE:		   newhip=new		  db_tolerance(*((		  db_tolerance *)this)); break;
		case DB_TRACE:			   newhip=new			  db_trace(*((			  db_trace *)this)); break;
		case DB_VERTEX: 		   newhip=new			 db_vertex(*((			 db_vertex *)this)); break;
		case DB_VIEWPORT:		   newhip=new		   db_viewport(*((		   db_viewport *)this)); break;
		case DB_XLINE:			   newhip=new			  db_xline(*((			  db_xline *)this)); break;
		case DB_DICTIONARY: 	   newhip=new		 db_dictionary(*((		 db_dictionary *)this)); break;
		case DB_DICTIONARYVAR:	   newhip=new	  db_dictionaryvar(*((	  db_dictionaryvar *)this)); break;
		case DB_GROUP:			   newhip=new			  CDbGroup(*((			  CDbGroup *)this)); break;
		case DB_IDBUFFER:		   newhip=new		   db_idbuffer(*((		   db_idbuffer *)this)); break;
		case DB_IMAGEDEF:		   newhip=new		   db_imagedef(*((		   db_imagedef *)this)); break;
		case DB_IMAGEDEF_REACTOR:  newhip=new  db_imagedef_reactor(*(( db_imagedef_reactor *)this)); break;
		case DB_MLINESTYLE: 	   newhip=new		 db_mlinestyle(*((		 db_mlinestyle *)this)); break;
		case DB_ACAD_PROXY_OBJECT: newhip=new db_acad_proxy_object(*((db_acad_proxy_object *)this)); break;
		case DB_RASTERVARIABLES:   newhip=new	db_rastervariables(*((	db_rastervariables *)this)); break;
		case DB_SPATIAL_FILTER:    newhip=new	 db_spatial_filter(*((	 db_spatial_filter *)this)); break;
		case DB_XRECORD:		   newhip=new			db_xrecord(*((			db_xrecord *)this)); break;
		case DB_ACAD_PROXY_ENTITY: newhip=new db_acad_proxy_entity(*((db_acad_proxy_entity *)this)); break;
		case DB_LAYER_INDEX:	   newhip=new		db_layer_index(*((		db_layer_index *)this)); break;
		case DB_OBJECT_PTR: 	   newhip=new		 db_object_ptr(*((		 db_object_ptr *)this)); break;
		case DB_SORTENTSTABLE:	   newhip=new	  CDbSortentsTable(*((	  CDbSortentsTable *)this)); break;
		case DB_SPATIAL_INDEX:	   newhip=new	  db_spatial_index(*((	  db_spatial_index *)this)); break;
		case DB_PLOTSETTINGS:	   newhip=new	   CDbPlotSettings(*((	   CDbPlotSettings *)this)); break;
		case DB_LAYOUT: 		   newhip=new			 CDbLayout(*((			 CDbLayout *)this)); break;
		case DB_PLACEHOLDER:	   newhip=new		db_placeholder(*((		db_placeholder *)this)); break;
		case DB_DICTIONARYWDFLT:   newhip=new	db_dictionarywdflt(*((	db_dictionarywdflt *)this)); break;
		case DB_WIPEOUTVARIABLES:  newhip=new	db_wipeoutvariables(*((  db_wipeoutvariables *)this)); break;
		case DB_VBAPROJECT: 	   newhip=new		 db_vbaproject(*((		 db_vbaproject *)this)); break;
	}

	return newhip;
}


/*F*/
struct resbuf *db_handitem::entgetx(const struct resbuf *apps, db_drawing *dp) {
/*
**	Builds the entget llist for all handitems.
**
**	Returns a pointer to the generated list -- or NULL if
**	unsuccessful (out of memory).  rc tracks the error code (0 or -1),
**	even though it is never returned to the caller.
*/
	short fsh1;
	int rc;
	sds_name en;
	struct resbuf *elist[2],*sublist[2];
	db_handitem *thip1;
	char asciihandle[17];



	rc=0; elist[0]=elist[1]=NULL;


	/* db_handitem groups (except eed): */

	/* -1 (ename) */
	en[0]=(long)this; en[1]=(long)dp;
	if ((elist[0]=elist[1]=db_newrb(-1,'N',en))==NULL) { rc=-1; goto out; }

	/* 0 (type) */
	if ((elist[1]->rbnext=db_newrb(0,'S',(db_is_validhitype(type)) ?
		db_hitypeinfo[type].name : db_str_invalid))==NULL) { rc=-1; goto out; }
	elist[1]=elist[1]->rbnext;

	/* 5 or 105 (handle) */
	fsh1=(type==DB_DIMSTYLETABREC) ? 105 : 5;
	RetHandle().ToAscii(asciihandle);

	if ((elist[1]->rbnext=db_newrb(fsh1,'S',asciihandle))==NULL) { rc=-1; goto out; }
	elist[1]=elist[1]->rbnext;

	/* 330 (block owner) */	if(m_parentBlock 
		&& (m_parentBlock->ret_type() != DB_BLOCKTABREC 
			|| !(((db_blocktabrec*)m_parentBlock)->ret_flags() & IC_BLOCK_XREF)))
	{
		en[0] = (long)m_parentBlock;
		if ((elist[1]->rbnext = db_newrb(330,'N',en))==NULL) { rc=-1; goto out; }
		elist[1] = elist[1]->rbnext;
	}

	/* The persistent reactors: */
	if (m_reactors->GetCountNotDeleted(dp) > 0) {
		if ((elist[1]->rbnext=db_newrb(102,'S', db_preactmarker))==NULL) { rc=-1; goto out; }
		elist[1]=elist[1]->rbnext;

		m_reactors->ResetIterator();
		db_handitem *reactor = NULL;
		while (m_reactors->GetNextHanditem(dp, &reactor))
		{
			en[0]=en[1]=0;
			if(reactor && !reactor->ret_deleted())
			{
				en[0]=(long)reactor; en[1]=(long)dp;
				if ((elist[1]->rbnext=db_newrb(330,'N',en))==NULL) { rc=-1; goto out; }
				elist[1]=elist[1]->rbnext;
			}
		}

		if ((elist[1]->rbnext=db_newrb(102,'S',"}"))==NULL) { rc=-1; goto out; }
		elist[1]=elist[1]->rbnext;
	}

	/* The extension dictionary.  (We currently think there can only be one.) */
	if (xdict!=NULL) {
		if ((elist[1]->rbnext=db_newrb(102,'S',db_xdictmarker))==NULL) { rc=-1; goto out; }
		elist[1]=elist[1]->rbnext;

		en[0]=en[1]=0;
		if ((thip1=xdict->ret_hip(dp))!=NULL)
			{ en[0]=(long)thip1; en[1]=(long)dp; }
		if ((elist[1]->rbnext=db_newrb(360,'N',en))==NULL) { rc=-1; goto out; }
		elist[1]=elist[1]->rbnext;

		if ((elist[1]->rbnext=db_newrb(102,'S',"}"))==NULL) { rc=-1; goto out; }
		elist[1]=elist[1]->rbnext;
	}

	/* db_entity or db_tablerecord groups: */
	//EBATEHC(CNBR) 2002/9/14 Add dp parameter for plotstylehip
	if ((rc=entgetcommon(sublist,sublist+1,dp))!=0) goto out;
	//if ((rc=entgetcommon(sublist,sublist+1))!=0) goto out;
	if (sublist[0]!=NULL && sublist[1]!=NULL)
		{ elist[1]->rbnext=sublist[0]; elist[1]=sublist[1]; }
	sublist[0]=sublist[1]=NULL;


	/* Entity-specific or table-record-specific groups: */

	if ((rc=entgetspecific(sublist,sublist+1,dp))!=0) goto out;
	if (sublist[0]!=NULL && sublist[1]!=NULL)
		{ elist[1]->rbnext=sublist[0]; elist[1]=sublist[1]; }


	/* EED */
	if (apps!=NULL) {
		elist[1]->rbnext=get_eed(apps,&fsh1);
		if (fsh1==-2) { rc=-1; goto out; }
	}

out:
	if (rc && elist[0]!=NULL) { sds_relrb(elist[0]); elist[0]=elist[1]=NULL; }
	return elist[0];
}


/*F*/
//int db_handitem::entgetcommon(struct resbuf **begpp, struct resbuf **endpp)
int db_handitem::entgetcommon(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**	The default.  Should never get called, since db_entity and
**	db_tablerecord have their own.
*/

	if (begpp!=NULL) *begpp=NULL;
	if (endpp!=NULL) *endpp=NULL;

	return 0;
}


/*F*/
int db_handitem::entgetspecific(
	struct resbuf **begpp,
	struct resbuf **endpp,
	db_drawing	   *dp) {
/*
**	The default entity-specific entgetter (for entities that don't have
**	their own yet).
**
**	Returns:
**		 0 : OK
**		-1 : No memory
*/
	char *fcp1;
	int istabrec,isobject,isent,fint1,rc;
	long fl1;
	struct resbuf *sublist[2];
	sds_name en;
	sds_real ar1;
	sds_point ap1;
	db_handitem *thip1;

	int pstylemode = DB_BYCOL_PSTYLEMODE;
	{
		struct resbuf rb;
		if( db_getvar(NULL,DB_QPSTYLEMODE,&rb,dp,NULL,NULL) == RTNORM )
		{
			pstylemode = rb.resval.rint;
		}
	}

	rc=0; sublist[0]=sublist[1]=NULL;

	if (begpp==NULL || endpp==NULL) goto out;

	/*
	**	Usually, when making a llist in a linear fashion like this
	**	(not in a loop), we KNOW what the first link is, so
	**	we can assign it and then use the "last link's next"
	**	thereafter.  This time, we DON'T know when we've started
	**	the llist, so, rather than do an if-else every time,
	**	let's start the llist (sublist[0] with a dummy header
	**	link).	That way, EVERY addition can link onto the
	**	last link's next.
	*/
	if ((sublist[0]=sublist[1]=db_alloc_resbuf())==NULL) { rc=-1; goto out; }

	istabrec=db_is_tabrectype(type);
	isobject=db_is_objecttype(type);
	isent=(!istabrec && !isobject);

	if (get_1(&fcp1)==true) {
		if ((sublist[1]->rbnext=db_newrb(1,'S',fcp1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  if (!istabrec) {	/* 2 already done for table recs */
	if (get_2(&fcp1)==true) {
		if ((sublist[1]->rbnext=db_newrb(2,'S',fcp1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  }
	if (get_3(&fcp1)==true) {
		if ((sublist[1]->rbnext=db_newrb(3,'S',fcp1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_4(&fcp1)==true) {
		if ((sublist[1]->rbnext=db_newrb(4,'S',fcp1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  /* 5 is handle and therefore already done -- except for DIMSTYLE: */
	// if dimstyle record, then 5 is DIMBLK value
	//
	if (type==DB_DIMSTYLETABREC)
		{
		// Cast to our real type
		// Ugh!  This is the reason it's better not to call these methods by their DXF
		// codes
		//
		db_dimstyletabrec *pThis = (db_dimstyletabrec *)this;
		if (pThis->get_5(&fcp1)==true)
			{
			if ((sublist[1]->rbnext=db_newrb(5,'S',fcp1))==NULL)
				{ rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
			}
		}
	if (get_6(&fcp1)==true) {
		if ((sublist[1]->rbnext=db_newrb(6,'S',fcp1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_7(&fcp1)==true) {
		if ((sublist[1]->rbnext=db_newrb(7,'S',fcp1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  if (!isent) {  /* Already done for entities */
	if (get_8(&fcp1)==true) {
		if ((sublist[1]->rbnext=db_newrb(8,'S',fcp1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  }


	if (get_10(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(10,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_11(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(11,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_12(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(12,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_13(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(13,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_14(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(14,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_15(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(15,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_16(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(16,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_17(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(17,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	if (get_39(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(39,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_40(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(40,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_41(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(41,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_42(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(42,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_43(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(43,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_44(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(44,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_45(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(45,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_46(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(46,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_47(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(47,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  if (!isent) {  /* Already done for entities */
	if (get_48(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(48,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  }
	if (get_49(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(49,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_50(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(50,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_51(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(51,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_52(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(52,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_53(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(53,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

  if (!isent) {  /* Already done for entities */
	if (get_60(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(60,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  }
  if (!isent) {  /* Already done for entities */
	if (get_62(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(62,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  }
	if (get_66(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(66,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  if (!isent) {  /* Already done for entities */
	if (get_67(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(67,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  }
	if (get_68(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(68,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_69(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(69,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  if (!istabrec) {	/* Already done for table records */
	if (get_70(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(70,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
  }
	if (get_71(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(71,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_72(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(72,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_73(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(73,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_74(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(74,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_75(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(75,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_76(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(76,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_77(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(77,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_78(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(78,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) -[ Bugzilla#78443 DIMSTYLE/DIMAZIN 2003/2/7
	if (get_79(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(79,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) ]-

	/* This one used to be the ADE lock, but ACAD stopped documenting it */
	/* as such, and it's shown up in the IMAGEDEF object: */
	if (get_90(&fint1)==true) {
		fl1=fint1;
		if ((sublist[1]->rbnext=db_newrb(90,'L',&fl1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	/* No 105.	That's a DIMSTYLE record handle, which we've already done. */

	if (get_140(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(140,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_141(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(141,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_142(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(142,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_143(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(143,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_144(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(144,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_145(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(145,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_146(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(146,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_147(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(147,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_148(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(148,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_149(&ar1)==true) {
		if ((sublist[1]->rbnext=db_newrb(149,'R',&ar1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	if (get_170(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(170,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_171(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(171,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_172(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(172,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_173(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(173,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_174(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(174,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_175(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(175,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_176(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(176,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_177(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(177,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_178(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(178,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) -[ Bugzilla#78443 DIMSTYLE/DIMAZIN 2003/2/7
	if (get_179(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(179,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) ]-

	if (get_210(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(210,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_211(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(211,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_212(ap1)==true) {
		if ((sublist[1]->rbnext=db_newrb(212,'P',ap1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	if (get_270(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(270,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_271(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(271,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_272(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(272,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_273(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(273,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_274(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(274,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_275(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(275,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	// EBATECH(watanabe)-[dimfrac
	if (get_276(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(276,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// ]-EBATECH(watanabe)

	// EBATECH(watanabe)-[dimlunit
	if (get_277(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(277,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// ]-EBATECH(watanabe)

	// EBATECH(CNBR) -[ DIMSTYLE/DIMDSEP 2003/3/31
	if (get_278(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(278,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) ]-

	// EBATECH(watanabe)-[dimtmove
	if (get_279(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(279,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// ]-EBATECH(watanabe)

	if (get_280(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(280,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_281(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(281,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_282(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(282,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_283(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(283,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_284(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(284,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_285(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(285,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_286(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(286,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_287(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(287,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	if (get_288(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(288,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	// EBATECH(watanabe)-[dimatfit
	if (get_289(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(289,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// ]-EBATECH(watanabe)

	/*DG - 11.6.2002*/// dxf290 of layers is supported since dxf 2000.
	if (get_290(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(290,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	if (get_340(&thip1)==true) {
		en[0]=(long)thip1; en[1]=(long)dp;
		if ((sublist[1]->rbnext=db_newrb(340,'N',en))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(watanabe)-[dimldrblk
	// SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
	//if (get_341(&fcp1)==true) {
	//	  if ((sublist[1]->rbnext=db_newrb(341,'S',fcp1))==NULL) { rc=-1; goto out; }
	if (get_341(&thip1) == true)
	  {
		en[0] = (long)thip1;
		en[1] = (long)dp;
		sublist[1]->rbnext = db_newrb(341,'N',en);
		if (sublist[1]->rbnext == NULL)
		  {
			rc = -1;
			goto out;
		  }
	// ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
		sublist[1]=sublist[1]->rbnext;
	}
	// ]-EBATECH(watanabe)
	/*DG - 11.6.2002*/// dxf370 of layers is supported since dxf 2000.
	if (type==DB_LAYERTABREC)
	//if (!isent)  /* 370 already done for all entities */
	{
		if (get_370(&fint1)==true) {
			if ((sublist[1]->rbnext=db_newrb(370,'I',&fint1))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
		}
	}

	// EBATECH(CNBR) -[ DIMSTYLE/DIMLWD 2003/3/31
	if (get_371(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(371,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) ]-
	// EBATECH(CNBR) -[ DIMSTYLE/DIMLWE 2003/3/31
	if (get_372(&fint1)==true) {
		if ((sublist[1]->rbnext=db_newrb(372,'I',&fint1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) ]-

	/* 390 plotstyle object name is already done for all entities */
	if( type==DB_LAYERTABREC && pstylemode == DB_NAMED_PSTYLEMODE ) {
		if (get_390(&thip1)==true) {
			en[0]=(long)thip1; en[1]=(long)dp;
			if ((sublist[1]->rbnext=db_newrb(390,'N',en))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
		}
	}

out:

	if (begpp!=NULL && endpp!=NULL) {
		if (rc && sublist[0]!=NULL)
			{ sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
		if (sublist[0]==NULL || sublist[0]->rbnext==NULL) {
			*begpp=*endpp=NULL;
		} else {
			*begpp=sublist[0]->rbnext;	/* Take links AFTER the dummy. */
			sublist[0]->rbnext=NULL;	/* Chop sublist[0] to just dummy link. */
			*endpp=sublist[1];
		}
		if (sublist[0]!=NULL) sds_relrb(sublist[0]);
	}

	return rc;
}


// ************************************
// PUBLIC METHOD -- class db_handitem
//
// Compare with the other's eed without looking inside
//
bool
db_handitem::equal_eed( db_handitem *pOther )
	{
	ASSERT( pOther != NULL );


	// If both NULL, they're equal
	//
	if ( (this->eed == NULL) &&
		 (pOther->eed == NULL) )
		{
		return true;
		}

	// If only one is NULL, they're not equal
	//
	if ( (this->eed == NULL) ||
		 (pOther->eed == NULL) )
		{
		return false;
		}

	// Now we know both are non-NULL
	bool bRetval = false;


	unsigned short iLength;

	// length is stored in first two bytes of eed.	Total length of the data is that number, plus
	// the two bytes where the length is stored...
	//
	iLength = *((short *)this->eed) + sizeof( short );

	// If the lengths of the two are not equal, the eed isn't equal
	//
	if ( iLength != ((*((short *)pOther->eed))	+ sizeof( short ) ))
		{
		return false;
		}

	// Finally, do a full binary compare
	//
	return ( ::memcmp( this->eed, pOther->eed, iLength ) == 0 );
	}

// ************************************
// PUBLIC METHOD -- class db_handitem
//
// This method makes a copy of the eed in a handitem
// and returns it in a opaque pointer.	Don't make assumptions
// about what the void* contains.  Only dup_eed and
// restore_dup_eed should know the format inside the void*.
//
// Used for undo
//
void *
db_handitem::dup_eed( void )
	{
	void *pRetval = NULL;
	if ( this->eed != NULL )
		{
		short iLength;

		// length is stored in first two bytes of eed.	Total length of the data is that number, plus
		// the two bytes where the length is stored...
		//
		iLength = *((short *)this->eed) + sizeof( short );

		// better have some data if we're storing the length
		// This assumption may be wrong???
		//
		ASSERT( iLength > 2 );

		void *pTemp = new char[ iLength ];
		if ( pTemp != NULL )
			{
			::memcpy( pTemp, this->eed, iLength );
			pRetval = pTemp;
			}
		}

	return pRetval;
	}

// ************************************
// PUBLIC METHOD -- class db_handitem
//
// This method restores ee data from the void*
// returned by dup_eed above.
//
// Used for undo
//
bool
db_handitem::restore_dup_eed( void *pData )
	{
	if ( this->eed != NULL )
		{
		delete [] this->eed;
		this->eed = NULL;
		}

	this->eed = (char *)pData;

	return true;
	}

// ******************************************
// PUBLIC METHOD -- class db_handitem
//
// This method frees any memory allocated by dup_eed
// This method is STATIC -- a class method
//
//
bool
db_handitem::free_dup_eed( void *pdupeed )
	{
	// Hey, it was either NULL, or we allocated it above in dup_eed
	// and the first WORD is the length.  Sanity check it
	//
	ASSERT( (pdupeed == NULL) ||
		( *((short *)pdupeed) > 0 ) );


	if ( pdupeed != NULL )
		{
		// Set the length to 0 so we can error check for it later
		//
		*((short *)pdupeed) = 0;


		delete pdupeed;
		}

	return true;
	}



// ************************************************
/*F*/
struct resbuf *db_handitem::get_eed(
	const struct resbuf *apps,
	short		  *rcp) {
/*
**	Adapted from dwg_getappeedll().
**
**	This function scans through the full EED buffer (this->eed --
**	our internal form) and allocates a llist of resbufs (beginning with
**	a -3 sentinel) that has the EED for the applications specified
**	by apps.  apps must be a llist of resbufs containing wildcard
**	pattern strings -- or NULL, which means "get all EED", just like
**	a single link whose value is "*".
**
**	IF THIS FUNCTION RETURNS NON-NULL, THE CALLER MUST EVENTUALLY
**	FREE THE ALLOCATED LLIST (with sds_relrb()).
**
**	WARNING: Don't use db_astrncpy() in here or anything else that
**	uses new/delete; resbufs are still alloc/free items.
**
**	Returns a ptr to the allocated llist, or NULL if no applications
**	wildcard-match apps or there is insufficient memory.
**	If rcp!=NULL, *rcp is set to one of the following return codes:
**		 0 : OK
**		-1 : No EED in for the applications specified by apps
**		-2 : No memory
*/
	char fs1[DB_ACADBUF+1],*fcp1;
	short take,rc;
	unsigned short sidx,bufsz,fus1;
	struct resbuf *parteed,*last;
	const struct resbuf	*trbp1;
	db_handitem *thip1;

	rc=0; parteed=last=NULL;
	if (rcp!=NULL) *rcp=0;

	if (eed==NULL) { rc=-1; goto out; }

	/* Start with -3 sentinel: */
	if ((parteed=db_newrb(-3,0,NULL))==NULL) { rc=-2; goto out; }
	last=parteed;

	/* Get eed's size: */
	memcpy(&bufsz,eed,sizeof(bufsz)); bufsz+=2;

	sidx=2;  /* Start after length short */
	take=0;  /* Not capturing data yet */
	while (sidx<bufsz) {
		if		  (eed[sidx]==0) {
			fus1=(unsigned char)eed[sidx+1];  /* Yes -- unsigned CHAR! */
			if (take) {
				*fs1=0;
				if (fus1) strncpy(fs1,eed+sidx+4,fus1);
				fs1[fus1]=0;
				if ((last->rbnext=db_newrb(1000,'S',fs1))==NULL) { rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=fus1+4;
		} else if (eed[sidx]==1) {	/* App hip */
			char *ucappname,*ucwcspec;

			ucappname=ucwcspec=NULL;

			/* DON'T goto out IN THIS BLOCK; goto appnameout. */

			memcpy(&thip1,eed+sidx+1,sizeof(thip1));  /* App hip */
			/* Get the app name: */
			fcp1=NULL; if (thip1!=NULL) thip1->get_2(&fcp1);
			take=0;
			if (fcp1!=NULL) {
				db_astrncpy(&ucappname,fcp1,-1); _tcsupr(ucappname);
				if (apps==NULL) take=1;
				else {
					for (trbp1=apps; trbp1!=NULL; trbp1=trbp1->rbnext) {
						if (trbp1->restype!=RTSTR && trbp1->restype!=1001)
							continue;
						db_astrncpy(&ucwcspec,trbp1->resval.rstring,-1);
						_tcsupr(ucwcspec);
						if (sds_wcmatch(ucappname,ucwcspec)==RTNORM) break;
					}
					take=(trbp1!=NULL);
				}
			}
			if (take) {
				if ((last->rbnext=db_newrb(1001,'S',ucappname))==NULL)
					{ rc=-2; goto appnameout; }
				last=last->rbnext;
			}
			sidx+=5;

		  appnameout:

			delete [] ucappname; delete [] ucwcspec;
			if (rc) { goto out; }

		} else if (eed[sidx]==2) {	/* Brace */
			if (take) {
				fcp1=(!eed[sidx+1]) ? "{" : "}";
				if ((last->rbnext=db_newrb(1002,'S',fcp1))==NULL)
					{ rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=2;
		} else if (eed[sidx]==3) {	/* Layer idx */
			if (take) {
				memcpy(&thip1,eed+sidx+1,sizeof(thip1));  /* Lay hip */
				/* Get the layer name: */
				fcp1=NULL; if (thip1!=NULL) thip1->get_2(&fcp1);
				if (fcp1!=NULL) {
					if ((last->rbnext=db_newrb(1003,'S',fcp1))==NULL) { rc=-2; goto out; }
					last=last->rbnext;
				}
			}
			sidx+=5;
		} else if (eed[sidx]==4) {	/* Binary chunk */
			if (take) {
				if ((last->rbnext=db_newrb(1004,'B',eed+sidx+1))==NULL)
					{ rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=(unsigned short)eed[sidx+1]+2;
		} else if (eed[sidx]==5) {
			if (take) {
				db_convhand(eed+sidx+1,fs1,0,2);
				if ((last->rbnext=db_newrb(1005,'S',fs1))==NULL) { rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=9;
		} else if (eed[sidx]> 9 && eed[sidx]<14) {
			if (take) {
				if ((last->rbnext=db_newrb((short)(1000+eed[sidx]),'P',
					eed+sidx+1))==NULL) { rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=25;
		} else if (eed[sidx]>39 && eed[sidx]<43) {
			if (take) {
				if ((last->rbnext=db_newrb((short)(1000+eed[sidx]),'R',
					eed+sidx+1))==NULL) { rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=9;
		} else if (eed[sidx]==70) {
			if (take) {
				if ((last->rbnext=db_newrb(1070,'H',eed+sidx+1))==NULL)
					{ rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=3;
		} else if (eed[sidx]==71) {
			if (take) {
				if ((last->rbnext=db_newrb(1071,'L',eed+sidx+1))==NULL)
					{ rc=-2; goto out; }
				last=last->rbnext;
			}
			sidx+=5;
		} else {  /* Unrecognized code?  Just take what we've got, I guess. */
			break;
		}
	}

out:
	/* Check for nothing, or just the sentinel; didn't find any. */
	if (last==NULL || last->restype==-3) rc=-1;
	if (rcp!=NULL) *rcp=rc;
	if (rc) {
		if (parteed!=NULL) { sds_relrb(parteed); parteed=last=NULL; }
	}

	return parteed;
}


int db_handitem::set_eed(
	struct resbuf *elist,
	db_drawing	  *dp) {
/*
**	This is really a "mod" function, since it modifies existing EED,
**	but we're calling it "set" so that it's consistent with
**	the other set/get functions.  To actually SET the EED from
**	elist, make sure this->eed is NULL (no existing EED).
**
**	The first part of this function merges EED from elist with the EED
**	already present in this->eed (the entmod scenario).
**
**	The second part of this function has been adapted from my
**	dwg_eedmake() (which was adapted from my dwg_12geteed()),
**	which created our internal EED from elist.
**
**	Together, they perform the EED part of an entmake or entmod.
**
**	Our char buffer is ALMOST just a memcpy() of the R12 binary form.
**	The only differences are these:
**
**		Strings are expanded to the R13 form (the two "mystery bytes"
**		are added between the length byte and the value).  The
**		"mystery bytes" are being set here 0x00 and 0x0C for now.
**		That's what they were when I first saw them.  Unfortunately,
**		they vary (or at least the 2nd one does) and we don't know
**		what they mean.  We'll have to figure it out before we write
**		R13 files.	(Matt indicates that they form a codepage specifier.)
**
**		Table references are hip's (4 bytes), not idx's (2 bytes).
**
**	Returns: See db_handitem::entmod().  Sets db_lasterror.
*/
	char *neweed;
	short fi1;
	unsigned short bidx,fus1;
	int cd,pass,useexisting,rbpchainsz,cidx,rc,fint1;
	struct resbuf *existing,**rbpchain,*trbp1,*trbp2;
	db_handitem *thip1;


	rc=0; existing=NULL; rbpchain=NULL; neweed=NULL;
	long newEEDSize = 0;

	db_lasterror=0;

	if (dp==NULL) goto out;

	/* Get ALL of the existing EED in resbuf form: */
	existing=get_eed(NULL,&fi1); if (fi1==-2) { rc=-2; goto out; }

	/*
	**	Create rbpchain[] such that it points to the correct links
	**	in elist and "existing", in the correct order, to define
	**	the modified EED.  Here are the rules:
	**
	**		If elist and "existing" both have data for an app,
	**		take elist's; otherwise, take the data from whichever
	**		list has it.
	**
	**	Scan "existing" first to maintain its order of apps.
	*/

	/* Count the total EED links in both llists and alloc rbpchain[]: */
	rbpchainsz=0;
	for (trbp1=existing; trbp1!=NULL && trbp1->restype!=-3; trbp1=trbp1->rbnext);
	while (trbp1!=NULL && trbp1->restype!=1001) trbp1=trbp1->rbnext;
	while (trbp1!=NULL) { rbpchainsz++; trbp1=trbp1->rbnext; }
	fint1=0;
	for (trbp2=elist; trbp2!=NULL && trbp2->restype!=-3; trbp2=trbp2->rbnext);
	while (trbp2!=NULL && trbp2->restype!=1001) trbp2=trbp2->rbnext;
	while (trbp2!=NULL) { fint1++; trbp2=trbp2->rbnext; }
	if (fint1<1) goto out;	/* No EED in elist; leave the current EED alone. */
	rbpchainsz+=fint1;
	if ((rbpchain= new struct resbuf * [ rbpchainsz ] )==NULL) { rc=-2; goto out; }
	memset(rbpchain,0,sizeof(struct resbuf *)*rbpchainsz);
	/* This is at least enough elements -- and over-kill in many cases. */

	cidx=0;  /* Idx into rbpchain[] */

	/* Scan "existing" first: */
	for (trbp1=existing; trbp1!=NULL && trbp1->restype!=-3; trbp1=trbp1->rbnext);
	while (trbp1!=NULL) {
		if (trbp1->restype!=1001) { trbp1=trbp1->rbnext; continue; }

		/* Does this app exist in elist? */
		for (trbp2=elist; trbp2!=NULL && trbp2->restype!=-3; trbp2=trbp2->rbnext);
		while (trbp2!=NULL) {
			for (trbp2=trbp2->rbnext; trbp2!=NULL &&
				trbp2->restype!=1001; trbp2=trbp2->rbnext);
			if (trbp2==NULL ||
				strisame(trbp1->resval.rstring,trbp2->resval.rstring))
					break;
		}

		useexisting=0;
		if (trbp2==NULL)  /* Nope.	Take from "existing". */
			{ trbp2=trbp1; useexisting=1; }

		do {
			rbpchain[cidx++]=trbp2; trbp2=trbp2->rbnext;
		} while (trbp2!=NULL && trbp2->restype!=1001);

		/* Advance trbp1 ... */
		if (useexisting) trbp1=trbp2;  /* ... to next 1001 */
		else trbp1=trbp1->rbnext;	   /* ... past current 1001 */
	}

	/* Now add the apps that are ONLY in elist: */
	for (trbp2=elist; trbp2!=NULL && trbp2->restype!=-3; trbp2=trbp2->rbnext);
	while (trbp2!=NULL) {
		if (trbp2->restype!=1001) { trbp2=trbp2->rbnext; continue; }

		/* Does this app exist in "existing"? */
		for (trbp1=existing; trbp1!=NULL && trbp1->restype!=-3; trbp1=trbp1->rbnext);
		while (trbp1!=NULL) {
			for (trbp1=trbp1->rbnext; trbp1!=NULL &&
				trbp1->restype!=1001; trbp1=trbp1->rbnext);
			if (trbp1==NULL ||
				strisame(trbp1->resval.rstring,trbp2->resval.rstring))
					break;
		}

		if (trbp1!=NULL)  /* Yep; it's in both; already took it. */
			{ trbp2=trbp2->rbnext; continue; }	/* Advance past current 1001 */

		/* Not in "existing"; use elist: */
		do {
			rbpchain[cidx++]=trbp2; trbp2=trbp2->rbnext;
		} while (trbp2!=NULL && trbp2->restype!=1001);
	}

	/* Note that we may not have filled all of the members of */
	/* rbpchain[].	The first NULL element (from the calloc) is the end. */


	/***** CONVERT FROM RESBUF LLIST TO OUR INTERNAL FORMAT *****/


	/* Walk rbpchain[] and convert: */

	/*
	**	Strategy: During first pass, count the bytes needed (strings
	**	will need 2 extra).  During the second pass, alloc the buffer
	**	and create the data, inserting the two special R13 bytes for
	**	each string.
	*/

	// checks if EED are in boundaries ( more than 0 and less than 16383 )
	checkXdSize( *rbpchain, &newEEDSize );
	if( newEEDSize == 0 ) goto out;

	bidx=2;  /* Start with 2 for the length short */
	for (pass=0; pass<2; pass++) {

		if (pass) {  /* Alloc the buffer and start it with the length short */
			if (bidx<3) break;	/* No EED */
			delete [] neweed;  /* Precaution */
			neweed=new char[bidx]; fus1=bidx-2; memcpy(neweed,&fus1,2);
			bidx=2;
		}

		/* Careful!  The actual end of rbpchain[] is the first element */
		/* that is NULL (from the calloc) -- not element idx rbpchainsz-1. */
		for (cidx=0; cidx<rbpchainsz && rbpchain[cidx]!=NULL; cidx++) {

			cd=rbpchain[cidx]->restype-1000;

			if (cidx>0 && rbpchain[cidx-1]->restype==1001) { /* App idx */
				if (cd==1) continue;  /* No empty apps */
				if (pass) {
					if ((thip1=dp->findtabrec(DB_APPIDTAB,
						rbpchain[cidx-1]->resval.rstring,1))==NULL)
							{ rc=-3; db_lasterror=OL_ESNVALID; goto out; }
					neweed[bidx]=1; memcpy(neweed+bidx+1,&thip1,sizeof(thip1));
				}
				bidx+=5;
			}  /* NOT part of the "else if" stuff below! */

			if (cd==0) {  /* String */
				if (rbpchain[cidx]->resval.rstring!=NULL) {
					if ((fus1=strlen(rbpchain[cidx]->resval.rstring))>255) fus1=255;
					if (pass) {
						neweed[bidx]=(char)cd;
						neweed[bidx+1]=(char)fus1;
						neweed[bidx+2]=0;	  /* TODO: 1st mystery byte */
						neweed[bidx+3]=0x0C;  /* TODO: 2nd mystery byte */
						memcpy(neweed+bidx+4,rbpchain[cidx]->resval.rstring,fus1);
					}
					bidx+=fus1+4;  /* 1 for code, 1 for length byte, */
								   /* 2 for mystery bytes. */
				}
			} else if (cd==70) {  /* Short */
				if (pass) {
					neweed[bidx]=(char)cd;
					memcpy(neweed+bidx+1,&rbpchain[cidx]->resval.rint,2);
				}
				bidx+=3;
			} else if (cd==2) {  /* Brace */
				if (pass) {
					neweed[bidx]=(char)cd;
					/*{*/ neweed[bidx+1]=(rbpchain[cidx]->resval.rstring[0]=='}');
				}
				bidx+=2;
			} else if (cd>39 && cd<43) {  /* Real */
				if (pass) {
					neweed[bidx]=(char)cd;
					memcpy(neweed+bidx+1,&rbpchain[cidx]->resval.rreal,8);
				}
				bidx+=9;
			} else if (cd>9 && cd<14) {  /* Point */
				if (pass) {
					neweed[bidx]=(char)cd;
					memcpy(neweed+bidx+1,&rbpchain[cidx]->resval.rpoint,24);
				}
				bidx+=25;
			} else if (cd==71) {  /* Long */
				if (pass) {
					neweed[bidx]=(char)cd;
					memcpy(neweed+bidx+1,&rbpchain[cidx]->resval.rlong,4);
				}
				bidx+=5;
			} else if (cd==5) {  /* Entity handle, real */
				/* TODO: Take their word for it now.  Someday, we'll want to */
				/* make sure the handle exists. */

				if (pass) {
					neweed[bidx]=(char)cd;
					if (db_convhand(rbpchain[cidx]->resval.rstring,
						neweed+bidx+1,2,0)) { rc=-4; db_lasterror=OL_EHANDLE; goto out; }
				}
				bidx+=9;
			} else if (cd==3) {  /* Layer idx */
				if (pass) {
					if ((thip1=dp->findtabrec(DB_LAYERTAB,
						rbpchain[cidx]->resval.rstring,1))==NULL)
							{ rc=-3; db_lasterror=OL_ESNVALID; goto out; }
					neweed[bidx]=(char)cd;
					memcpy(neweed+bidx+1,&thip1,sizeof(thip1));
				}
				bidx+=5;
			} else if (cd==4) {  /* Binary chunk */
				if (rbpchain[cidx]->resval.rbinary.clen>0 &&
					rbpchain[cidx]->resval.rbinary.buf!=NULL) {

					if ((fus1=rbpchain[cidx]->resval.rbinary.clen)>127) fus1=127;
					if (pass) {
						neweed[bidx]=(char)cd;
						neweed[bidx+1]=(char)fus1;
						memcpy(neweed+bidx+2,rbpchain[cidx]->resval.rbinary.buf,fus1);
					}
					bidx+=fus1+2;
					/* 1 for code, 1 for length byte. */
				}
			}

		}  /* End for each wrbp link */

	}  /* End for each pass */

	/* If no errors, delete the old and take the new: */
	if (!rc) { delete [] eed; eed=neweed; neweed=NULL; }

out:
	delete [] neweed;  /* Should be NULL by now -- unless an error occurred */
	if (existing!=NULL) sds_relrb(existing);
	if (rbpchain!=NULL) delete rbpchain;

	return rc;
}


/*F*/
int db_handitem::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**	This one's virtual; it only gets called when we don't have
**	an entity-specific one.  The specific ones are better for
**	speed (and essential for the entities that have arrays),
**	but this one is a good default.
**
**	Returns (for all the entmod()'s, the entmod1common()'s, and set_eed()):
**		 0 : OK
**		-1 : Bad pass parameter
**		-2 : No memory
**		-3 : Invalid table entry specified
**		-4 : Invalid handle (EED 1005)
**		-5 : Invalid modification llist (modll).  (Should only occur
**			   with something like MLINE where the order and number
**			   of things is very important.)
**		-6 : A "set" function set db_lasterror (probably an attempt to
**			   set a table reference to a non-existant table record).
*/
	int rc;
	struct resbuf *curlink;


	rc=0; db_lasterror=0;


	if (modll==NULL || dp==NULL) { rc=-1; goto out; }

	del_xdict();

	curlink=modll;
	//Added Cybage AJK 29/10/2001 [
	//Reason--Fix for bug no 77865 from Bugzilla
	if(!validateEnt(modll))
	{
		rc=-6;
		goto out;
	}
	//Added Cybage AJK 29/10/2001 DD/MM/YYYY ]

	/*
	**	If modll has been built properly, the first few groups
	**	are things we don't allow to be modified (ename, type, handle),
	**	so let's walk over these right now:
	*/
	while (curlink!=NULL &&
		(curlink->restype==-1 ||
		 curlink->restype ==0 ||
		(type==DB_DIMSTYLETABREC && curlink->restype==105) ||
		(type!=DB_DIMSTYLETABREC && curlink->restype  ==5)))
			curlink=curlink->rbnext;

	/* Walk; stop at the end or the EED sentinel (-3): */
	for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) {

		/* Call the correct setter: */
		switch (curlink->restype) {
			/*
			**	Some of the following strings are used to set table
			**	record references, requiring the set_n(char *, db_drawing *)
			**	form.  In those cases, we try BOTH calls, taking the
			**	one that returns true.
			*/
			case   1: set_1(curlink->resval.rstring); break;
			case   2:
				if (set_2(curlink->resval.rstring,dp)==false)
					set_2(curlink->resval.rstring);
				break;
			case   3:
				if (set_3(curlink->resval.rstring,dp)==false)
					set_3(curlink->resval.rstring);
				break;
			case   4:
				if (set_4(curlink->resval.rstring,dp)==false)
					set_4(curlink->resval.rstring);
				break;
			case   5:
				if (type==DB_DIMSTYLETABREC)
						{
						// Cast to a pointer to the real type
						//
						db_dimstyletabrec *pThis = (db_dimstyletabrec *)this;

						// and call the function, since it doesn't exist on
						// the base db_handitem type
						//
						pThis->set_5(curlink->resval.rstring);
				}
						break;
			case   6:
				if (set_6(curlink->resval.rstring,dp)==false)
					set_6(curlink->resval.rstring);
				break;
			case   7:
				if (set_7(curlink->resval.rstring,dp)==false)
					set_7(curlink->resval.rstring);
				break;
			case   8:
				if (set_8(curlink->resval.rstring,dp)==false)
					set_8(curlink->resval.rstring);
				break;

			case  10: set_10(curlink->resval.rpoint); break;
			case  11: set_11(curlink->resval.rpoint); break;
			case  12: set_12(curlink->resval.rpoint); break;
			case  13: set_13(curlink->resval.rpoint); break;
			case  14: set_14(curlink->resval.rpoint); break;
			case  15: set_15(curlink->resval.rpoint); break;
			case  16: set_16(curlink->resval.rpoint); break;
			case  17: set_17(curlink->resval.rpoint); break;
			case  18: set_18(curlink->resval.rpoint); break; // new

			case  39: set_39(curlink->resval.rreal ); break;
			case  40: set_40(curlink->resval.rreal ); break;
			case  41: set_41(curlink->resval.rreal ); break;
			case  42: set_42(curlink->resval.rreal ); break;
			case  43: set_43(curlink->resval.rreal ); break;
			case  44: set_44(curlink->resval.rreal ); break;
			case  45: set_45(curlink->resval.rreal ); break;
			case  46: set_46(curlink->resval.rreal ); break;
			case  47: set_47(curlink->resval.rreal ); break;
			case  48: set_48(curlink->resval.rreal ); break;
			case  49: set_49(curlink->resval.rreal ); break;
			case  50: set_50(curlink->resval.rreal ); break;
			case  51: set_51(curlink->resval.rreal ); break;
			case  52: set_52(curlink->resval.rreal ); break;
			case  53: set_53(curlink->resval.rreal ); break;
			case  54: set_54(curlink->resval.rreal ); break;
			case  55: set_55(curlink->resval.rreal ); break; // new
			case  56: set_56(curlink->resval.rreal ); break; // new
			case  57: set_57(curlink->resval.rreal ); break; // new
			case  58: set_58(curlink->resval.rreal ); break; // new

			case  60: set_60(curlink->resval.rint  ); break;
			case  62: set_62(curlink->resval.rint  ); break;
			case  66: set_66(curlink->resval.rint  ); break;
			case  67: set_67(curlink->resval.rint  ); break;
			case  68: set_68(curlink->resval.rint  ); break;
			case  69: set_69(curlink->resval.rint  ); break;
			case  70: set_70(curlink->resval.rint  ); break;
			case  71: set_71(curlink->resval.rint  ); break;
			case  72: set_72(curlink->resval.rint  ); break;
			case  73: set_73(curlink->resval.rint  ); break;
			case  74: set_74(curlink->resval.rint  ); break;
			case  75: set_75(curlink->resval.rint  ); break;
			case  76: set_76(curlink->resval.rint  ); break;
			case  77: set_77(curlink->resval.rint  ); break;
			case  78: set_78(curlink->resval.rint  ); break;
			case  79: set_79(curlink->resval.rint  ); break; // EBATECH(CNBR) 2002/6/19 new
			case  90: set_90(curlink->resval.rint  ); break;
			case  91: set_91(curlink->resval.rint  ); break;
			case  92: set_92(curlink->resval.rint  ); break;
			case  93: set_93(curlink->resval.rint  ); break;
			case  94: set_94(curlink->resval.rint  ); break;
			case  95: set_95(curlink->resval.rint  ); break;
			case  96: set_96(curlink->resval.rint  ); break;
			case  97: set_97(curlink->resval.rint  ); break;
			case  98: set_98(curlink->resval.rint  ); break;
			case  99: set_99(curlink->resval.rint  ); break;

			// 102 handled by entmodhi
			// 110-112 EBATECH(CNBR) -[ 2002/6/19 new item
			case 110: set_110(curlink->resval.rpoint); break;
			case 111: set_111(curlink->resval.rpoint); break;
			case 112: set_112(curlink->resval.rpoint); break;
			// EBATECH(CNBR) ]-

			case 140: set_140(curlink->resval.rreal); break;
			case 141: set_141(curlink->resval.rreal); break;
			case 142: set_142(curlink->resval.rreal); break;
			case 143: set_143(curlink->resval.rreal); break;
			case 144: set_144(curlink->resval.rreal); break;
			case 145: set_145(curlink->resval.rreal); break;
			case 146: set_146(curlink->resval.rreal); break;
			case 147: set_147(curlink->resval.rreal); break;
			case 148: set_148(curlink->resval.rreal); break; // EBATECH(CNBR) DIMSTYLE/DIMALTRND 2003/3/31
			case 149: set_149(curlink->resval.rreal); break; // EBATECH(CNBR) PLOTSETTINGS 2003/6/26

			case 170: set_170(curlink->resval.rint ); break;
			case 171: set_171(curlink->resval.rint ); break;
			case 172: set_172(curlink->resval.rint ); break;
			case 173: set_173(curlink->resval.rint ); break;
			case 174: set_174(curlink->resval.rint ); break;
			case 175: set_175(curlink->resval.rint ); break;
			case 176: set_176(curlink->resval.rint ); break;
			case 177: set_177(curlink->resval.rint ); break;
			case 178: set_178(curlink->resval.rint ); break;
			case 179: set_179(curlink->resval.rint ); break; // EBATECH(CNBR) DIMSTYLE/DIMAZIN 2003/2/7 new

			case 210: set_210(curlink->resval.rpoint); break;
			case 211: set_211(curlink->resval.rpoint); break;
			case 212: set_212(curlink->resval.rpoint); break;

			case 270: set_270(curlink->resval.rint ); break;
			case 271: set_271(curlink->resval.rint ); break;
			case 272: set_272(curlink->resval.rint ); break;
			case 273: set_273(curlink->resval.rint ); break;
			case 274: set_274(curlink->resval.rint ); break;
			case 275: set_275(curlink->resval.rint ); break;
			case 278: set_278(curlink->resval.rint ); break; // EBATECH(CNBR) DIMSTYLE/DIMDSEP 2003/3/31

			case 280: set_280(curlink->resval.rint ); break;
			case 281: set_281(curlink->resval.rint ); break;
			case 282: set_282(curlink->resval.rint ); break;
			case 283: set_283(curlink->resval.rint ); break;
			case 284: set_284(curlink->resval.rint ); break;
			case 285: set_285(curlink->resval.rint ); break;
			case 286: set_286(curlink->resval.rint ); break;
			case 287: set_287(curlink->resval.rint ); break;
			case 288: set_288(curlink->resval.rint ); break;
			case 289: set_289(curlink->resval.rint ); break;
			case 290: set_290(curlink->resval.rint ); break;
			case 291: set_291(curlink->resval.rint ); break;
			case 292: set_292(curlink->resval.rint ); break;
			case 293: set_293(curlink->resval.rint ); break;
			case 294: set_294(curlink->resval.rint ); break;
			case 295: set_295(curlink->resval.rint ); break;
			case 296: set_296(curlink->resval.rint ); break;
			case 297: set_297(curlink->resval.rint ); break;
			case 298: set_298(curlink->resval.rint ); break;
			case 299: set_299(curlink->resval.rint ); break;

			case 340: set_340((db_handitem *)curlink->resval.rlname[0]); break;

			// SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
			case 341:
				set_341((db_handitem *)curlink->resval.rlname[0]);
				break;
			// ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

			// EBATECH(CNBR)-[ 2002/6/19 new item
			case 345: set_345((db_handitem *)curlink->resval.rlname[0]); break;
			case 346: set_346((db_handitem *)curlink->resval.rlname[0]); break;
			// EBATECH(CNBR) ]-

			case 370: set_370(curlink->resval.rint ); break;
			case 371: set_371(curlink->resval.rint ); break;
			case 372: set_372(curlink->resval.rint ); break;
			case 373: set_373(curlink->resval.rint ); break;
			case 374: set_374(curlink->resval.rint ); break;
			case 375: set_375(curlink->resval.rint ); break;
			case 376: set_376(curlink->resval.rint ); break;
			case 377: set_377(curlink->resval.rint ); break;
			case 378: set_378(curlink->resval.rint ); break;
			case 379: set_379(curlink->resval.rint ); break;
			case 380: set_380(curlink->resval.rint ); break;
			case 381: set_381(curlink->resval.rint ); break;
			case 382: set_382(curlink->resval.rint ); break;
			case 383: set_383(curlink->resval.rint ); break;
			case 384: set_384(curlink->resval.rint ); break;
			case 385: set_385(curlink->resval.rint ); break;
			case 386: set_386(curlink->resval.rint ); break;
			case 387: set_387(curlink->resval.rint ); break;
			case 388: set_388(curlink->resval.rint ); break;
			case 389: set_389(curlink->resval.rint ); break;

				// 390's --enames?
			case 390: set_390((db_handitem *)curlink->resval.rlname[0] ); break;
			case 391: set_391((db_handitem *)curlink->resval.rlname[0] ); break;
			case 392: set_392((db_handitem *)curlink->resval.rlname[0] ); break;
			case 393: set_393((db_handitem *)curlink->resval.rlname[0] ); break;
			case 394: set_394((db_handitem *)curlink->resval.rlname[0] ); break;
			case 395: set_395((db_handitem *)curlink->resval.rlname[0] ); break;
			case 396: set_396((db_handitem *)curlink->resval.rlname[0] ); break;
			case 397: set_397((db_handitem *)curlink->resval.rlname[0] ); break;
			case 398: set_398((db_handitem *)curlink->resval.rlname[0] ); break;
			case 399: set_399((db_handitem *)curlink->resval.rlname[0] ); break;

			case 400: set_400(curlink->resval.rint ); break;
			case 401: set_400(curlink->resval.rint ); break;
			case 402: set_400(curlink->resval.rint ); break;
			case 403: set_400(curlink->resval.rint ); break;
			case 404: set_400(curlink->resval.rint ); break;
			case 405: set_400(curlink->resval.rint ); break;
			case 406: set_400(curlink->resval.rint ); break;
			case 407: set_400(curlink->resval.rint ); break;
			case 408: set_400(curlink->resval.rint ); break;
			case 409: set_400(curlink->resval.rint ); break;

			case   410:
				if (set_410(curlink->resval.rstring,dp)==false)
					set_410(curlink->resval.rstring);
				break;
			case   411:
				if (set_411(curlink->resval.rstring,dp)==false)
					set_411(curlink->resval.rstring);
				break;
			case   412:
				if (set_412(curlink->resval.rstring,dp)==false)
					set_412(curlink->resval.rstring);
				break;
			case   413:
				if (set_413(curlink->resval.rstring,dp)==false)
					set_413(curlink->resval.rstring);
				break;
			case   414:
				if (set_414(curlink->resval.rstring,dp)==false)
					set_414(curlink->resval.rstring);
				break;
			case   415:
				if (set_415(curlink->resval.rstring,dp)==false)
					set_415(curlink->resval.rstring);
				break;
			case   416:
				if (set_416(curlink->resval.rstring,dp)==false)
					set_416(curlink->resval.rstring);
				break;
			case   417:
				if (set_417(curlink->resval.rstring,dp)==false)
					set_417(curlink->resval.rstring);
				break;
			case   418:
				if (set_418(curlink->resval.rstring,dp)==false)
					set_418(curlink->resval.rstring);
				break;
			case   419:
				if (set_419(curlink->resval.rstring,dp)==false)
					set_419(curlink->resval.rstring);
				break;
			default:  entmodhi(&curlink,dp);		  break;
				/* Catches persistent reactors and extension dictionary. */
		}

	}  /* End for each link */

	if (db_lasterror) { rc=-6; goto out; }

	/* Do the EED: */
	if (!rc && curlink!=NULL)
		rc=set_eed(curlink,dp);

out:

	return rc;
}

/*F*/
int db_handitem::entmodhi(struct resbuf **rblinkpp, db_drawing *dp) {
/*
**	This one is called to entmod members of db_handitem itself:
**	the persistent reactors and the extension dictionary.
**	Both of these use DXF 102 markers.
**	It's called by the entmod1common() functions when even THEY
**	fail to find a DXF code, and by db_handitem::entmod()
**	(the default (generic) function).
**
**	This function is the last ditch effort to do something about a
**	code present in the elist that hasn't been processed yet, so it's
**	a good place to trap unresolved cases.	We print a message
**	if DEBUG is defined.
**
**	Because this function processes more than 1 link, it has to
**	set the caller's current pointer (*rblinkpp) to the
**	last link processed so that the caller's stepping process
**	will work.
**
**	Returns: See db_handitem::entmod().
*/
	int rc;
	db_handitem *thip1;
	resbuf *cur,*lastprocessed;


	rc=0;

	if (rblinkpp==NULL || *rblinkpp==NULL || dp==NULL) { rc=-1; goto out; }

	cur=lastprocessed=*rblinkpp;

	if (cur->restype!=102) {
		goto out;  /* We're only interested in 102's beyond here. */
	}

	if (cur->resval.rstring!=NULL) {
		if (strisame(cur->resval.rstring,db_preactmarker)) {

			/* Delete the old llist: */
			m_reactors->RemoveAll();

			/* Build the new one: */
			for (cur=cur->rbnext; cur!=NULL &&
				cur->restype==330; cur=cur->rbnext) {

				db_handitem *reactor = (db_handitem *)cur->resval.rlname[0];

				if (reactor)
					m_reactors->AttachReactorAllowDups(reactor->RetHandle(), reactor, ReactorItem::SOFT_POINTER);

				lastprocessed=cur;
			}

		} else if (strisame(cur->resval.rstring,db_xdictmarker)) {

			delete xdict; xdict=NULL;

			cur=cur->rbnext;
			if (cur!=NULL && cur->restype==360) {
				thip1=(db_handitem *)cur->resval.rlname[0];
				xdict=new db_hireflink(
					DB_HARD_OWNER,
					(thip1!=NULL) ? thip1->RetHandle() : NULL,
					thip1);
				lastprocessed=cur;
			}

		}

		/* Step lastprocessed to the closing brace, if there is one */
		/* and we're not there yet: */
		if ( lastprocessed->rbnext				  !=NULL &&
			 lastprocessed->rbnext->restype 	  ==102  &&
			*lastprocessed->rbnext->resval.rstring=='}')
				lastprocessed=lastprocessed->rbnext;
	}


out:
	if (rblinkpp!=NULL && *rblinkpp!=NULL) *rblinkpp=lastprocessed;

	return rc;
}

ReactorSubject* db_handitem::getReactor()
{
	return m_reactors;
}

void
db_handitem::RemovePersistentReactors(void)
{
	m_reactors->RemoveAll();
}

bool db_handitem::addReactor(db_objhandle &handle, db_drawing *drawing, ReactorItem::Type type)
{
	void **fixuploc = NULL;
	bool result =  m_reactors->AttachReactor(handle, &fixuploc, type);

//	Don't fix up handles for non entity items - this needs more research before we implement it
//	drawing->AddHandleToFixupTable(handle,fixuploc);

	return result;
}

bool db_handitem::addReactor(db_objhandle &handle, db_handitem *item, ReactorItem::Type type)
{
	return m_reactors->AttachReactor(handle, item, type);
}

// need an attach method that allows duplicate associations.  Temporary entities sometimes need
// to be created such as savelp in entmod.	This allows undo to have the proper association.
bool db_handitem::addReactorAllowDups(db_objhandle &handle, db_handitem *item, ReactorItem::Type type)
{
	return m_reactors->AttachReactorAllowDups(handle, item, type);
}

bool db_handitem::removeReactor(db_objhandle &removeHandle)
{
	return m_reactors->DetachReactor(removeHandle);
}

bool db_handitem::Has1005Eed(db_drawing *dp)
{
	struct resbuf *rb;
	struct resbuf rbApps;
	bool ret;

	rbApps.restype = RTSTR;
	rbApps.resval.rstring = "*"/*DNT*/;

	rb = entgetx(&rbApps, dp);
	if (ic_ret_assoc(rb, 1005))
		ret = true;
	else
		ret = false;
	sds_relrb(rb);
	return ret;
}

// This function duplicates sds_xdsize() API. It is used internally in DB.dll instead of
// original one to avoid dependencies DB.dll from Icad.exe (where those API functions are
// actually called from) and propagating implementation of original through the static linkage
// in case we would include this implementation in sds_resbuf.cpp together with sds_relrb()
int checkXdSize(const struct sds_resbuf *prbEntData, long *plMemUsed)
	{

	struct resbuf *rbtmp=NULL;
	long fl1=0;

	if(plMemUsed==NULL)
		{
		return(RTERROR);
		}

	if( prbEntData==NULL ||
		(prbEntData->restype!=(-3) &&
		prbEntData->restype!=1001))
		{
		*plMemUsed=0; return(RTERROR);
		}

	for(rbtmp=(struct resbuf *)prbEntData; rbtmp!=NULL; rbtmp=rbtmp->rbnext)
		{
		switch(rbtmp->restype)
			{
			case (-3):
				break;

			case 1000:
				if (NULL == rbtmp->resval.rstring)
					{
					*plMemUsed = 0;
					return RTERROR;
					}

				fl1 += (long) (strlen(rbtmp->resval.rstring) + 2L);
				break;

			case 1001:
				fl1+=3L;
				break;

			case 1002:
				fl1+=2L;
				break;

			case 1003:
				fl1+=3L;
				break;

			case 1004:
				fl1+=(long)(rbtmp->resval.rbinary.clen+2L);
				break;

			case 1005:
				fl1+=9L;
				break;

			case 1010:
				fl1+=25L;
				break;

			case 1011:
				fl1+=25L;
				break;

			case 1012:
				fl1+=25L;
				break;

			case 1013:
				fl1+=25L;
				break;

			case 1040:
				fl1+=9L;
				break;

			case 1041:
				fl1+=9L;
				break;

			case 1042:
				fl1+=9L;
				break;

			case 1070:
				fl1+=3L;
				break;

			case 1071:
				fl1+=5L;
				break;

			default:
				*plMemUsed=0;
				return(RTERROR);
			}
		if(fl1>16383L)
			{
			fl1=0; break;
			}
		}

	*plMemUsed=fl1;
	return(RTNORM);

	}

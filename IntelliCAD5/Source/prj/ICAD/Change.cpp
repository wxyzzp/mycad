#include "Icad.h"
#include "binchange.h"
#include "HatchChange.h"
#include "GroupChange.h"
#include "DictionaryChange.h"
#include "IdBufferChange.h"
#include "MapChange.h"

#include "IcadApi.h"
#include "dimstyletabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "ucstabrec.h"
#include "viewtabrec.h"
#include "vporttabrec.h"
#include "vxtabrec.h"
#include "objects.h"
#include "DbGroup.h"
#include "DbMText.h"
#include "DbPlotSettings.h"
#include "DbLayout.h"
#include "DbSortentsTable.h"
#include "DbAcisEntity.h"

int SDS_CopyEntLink(db_handitem **dest,db_handitem *elp)
	{

	struct resbuf rb;
	SDS_getvar(NULL,DB_QUNDOCTL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!(rb.resval.rint & IC_UNDOCTL_ON))
		{
		return(RTNORM);
		}

	if((*dest=elp->newcopy())==NULL)
		{
		return(RTERROR);
		}
	return(RTNORM);
	}


int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldelp,db_handitem *newelp)
	{

	/* This function compares 2 db_handitem structures. */

	short fi1;
	struct db_BinChange *begll=NULL, *curll=NULL;

	struct resbuf rb;
	SDS_getvar(NULL,DB_QUNDOCTL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!(rb.resval.rint & IC_UNDOCTL_ON))
		{
		return(RTNORM);
		}

	sds_point defextru={0.0,0.0,1.0};
	if (oldelp==NULL || newelp==NULL)
		{
		return RTERROR;
		}

	// sanity
	//
	ASSERT( CHECKSTRUCTPTR( oldelp ) );
	ASSERT( CHECKSTRUCTPTR( newelp ) );

	// Can't change type
	ASSERT( oldelp->type == newelp->type );
	if(oldelp->type!=newelp->type)
		{
		return(RTERROR);
		}

	// sanity
	//
	ASSERT( db_is_validhitype( newelp->ret_type() ) );

	// Check handle.
	db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldelp->handle,(void *)&newelp->handle,sizeof(newelp->handle));

	// Check EED.
	db_BinChange::CacheChangedEED( &begll, &curll, newelp, oldelp );

	// Is it a entity or a table.
	if(db_is_tabrectype(oldelp->type))
		{
		db_tablerecord *newtab=(db_tablerecord *)newelp;
		db_tablerecord *oldtab=(db_tablerecord *)oldelp;
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldtab->m_pName,(void *)&newtab->m_pName,0);
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldtab->m_flags,(void *)&newtab->m_flags,sizeof(oldtab->m_flags));
		}
	else if (db_is_objecttype(oldelp->type))
		{

		// currently nothing to do for objects

		}
	else if (db_is_entitytype(oldelp->type))
		{
		db_entity *newent=(db_entity *)newelp;
		db_entity *oldent=(db_entity *)oldelp;
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->layerhip,(void *)&newent->layerhip,sizeof(oldent->layerhip));
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->ltypehip,(void *)&newent->ltypehip, sizeof(oldent->ltypehip));
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->ltscale, (void *)&newent->ltscale, sizeof(oldent->ltscale));
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->color,(void *)&newent->color,sizeof(oldent->color));
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->eflags,(void *)&newent->eflags,sizeof(oldent->eflags));
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->lineweight,(void *)&newent->lineweight,sizeof(oldent->lineweight));
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->plotstyletype,(void *)&newent->plotstyletype,sizeof(oldent->plotstyletype));
		db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldent->plotstylehip,(void *)&newent->plotstylehip,sizeof(oldent->plotstylehip));
		}
	else
		{
		// What is this?????
		//
		ASSERT( false );
		return RTERROR;
		}

	// Check the specific data.
	switch(newelp->ret_type()) {
		case DB_LINE:
			{
				db_line *newspec=(db_line *)newelp;
				db_line *oldspec=(db_line *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt[0],(void *)newspec->pt[0],sizeof(oldspec->pt[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt[1],(void *)newspec->pt[1],sizeof(oldspec->pt[1]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_ARC:
			{
				db_arc *newspec=(db_arc *)newelp;
				db_arc *oldspec=(db_arc *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->cent,(void *)newspec->cent,sizeof(oldspec->cent));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rad,(void *)&newspec->rad,sizeof(oldspec->rad));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ang[0],(void *)&newspec->ang[0],sizeof(oldspec->ang[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ang[1],(void *)&newspec->ang[1],sizeof(oldspec->ang[1]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_CIRCLE:
			{
				db_circle *newspec=(db_circle *)newelp;
				db_circle *oldspec=(db_circle *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->cent,(void *)&newspec->cent,sizeof(oldspec->cent));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rad,(void *)&newspec->rad,sizeof(oldspec->rad));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_POINT:
			{
				db_point *newspec=(db_point *)newelp;
				db_point *oldspec=(db_point *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rot,(void *)&newspec->rot,sizeof(oldspec->rot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_INSERT:
			{
				db_insert *newspec=(db_insert *)newelp;
				db_insert *oldspec=(db_insert *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->afollow,(void *)&newspec->afollow,sizeof(oldspec->afollow));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->bthip,(void *)&newspec->bthip,sizeof(oldspec->bthip));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->scl[0],(void *)&newspec->scl[0],sizeof(oldspec->scl[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->scl[1],(void *)&newspec->scl[1],sizeof(oldspec->scl[1]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->scl[2],(void *)&newspec->scl[2],sizeof(oldspec->scl[2]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rot,(void *)&newspec->rot,sizeof(oldspec->rot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->nrows,(void *)&newspec->nrows,sizeof(oldspec->nrows));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ncols,(void *)&newspec->ncols,sizeof(oldspec->ncols));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rowsp,(void *)&newspec->rowsp,sizeof(oldspec->rowsp));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->colsp,(void *)&newspec->colsp,sizeof(oldspec->colsp));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_SHAPE:
			{
				db_shape *newspec=(db_shape *)newelp;
				db_shape *oldspec=(db_shape *)oldelp;
				// EBATECH(CNBR) -[ 2002/4/24 shname is obsolute.
				//db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->shname,(void *)newspec->shname,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->shnum,(void *)&newspec->shnum,sizeof(oldspec->shnum));
				// EBATECH(CNBR) ]-
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->sz,(void *)&newspec->sz,sizeof(oldspec->sz));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rot,(void *)&newspec->rot,sizeof(oldspec->rot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->xscl,(void *)&newspec->xscl,sizeof(oldspec->xscl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->obl,(void *)&newspec->obl,sizeof(oldspec->obl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_SOLID:
			{
				db_solid *newspec=(db_solid *)newelp;
				db_solid *oldspec=(db_solid *)oldelp;
				for(fi1=0; fi1<4; ++fi1) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->pt[fi1],(void *)&newspec->pt[fi1],sizeof(oldspec->pt[fi1]));
				}
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_TRACE:
			{
				db_trace *newspec=(db_trace *)newelp;
				db_trace *oldspec=(db_trace *)oldelp;
				for(fi1=0; fi1<4; ++fi1) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->pt[fi1],(void *)&newspec->pt[fi1],sizeof(oldspec->pt[fi1]));
				}
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_3DFACE:
			{
				db_3dface *newspec=(db_3dface *)newelp;
				db_3dface *oldspec=(db_3dface *)oldelp;
				for(fi1=0; fi1<4; ++fi1) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt[fi1],(void *)newspec->pt[fi1],sizeof(oldspec->pt[fi1]));
				}
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,(void *)&newspec->flags,sizeof(oldspec->flags));
			}
			break;
		case DB_TEXT:
			{
				db_text *newspec=(db_text *)newelp;
				db_text *oldspec=(db_text *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->val,(void *)&newspec->val,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->ipt,(void *)newspec->ipt,sizeof(oldspec->ipt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->apt,(void *)newspec->apt,sizeof(oldspec->apt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ht,(void *)&newspec->ht,sizeof(oldspec->ht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->xscl,(void *)&newspec->xscl,sizeof(oldspec->xscl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rot,(void *)&newspec->rot,sizeof(oldspec->rot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->obl,(void *)&newspec->obl,sizeof(oldspec->obl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tship,(void *)&newspec->tship,sizeof(oldspec->tship));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->gen,(void *)&newspec->gen,sizeof(oldspec->gen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->hjust,(void *)&newspec->hjust,sizeof(oldspec->hjust));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->vjust,(void *)&newspec->vjust,sizeof(oldspec->vjust));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_ATTDEF:
			{
				db_attdef *newspec=(db_attdef *)newelp;
				db_attdef *oldspec=(db_attdef *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->defval,(void *)&newspec->defval,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->prompt,(void *)&newspec->prompt,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tag,(void *)&newspec->tag,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->ipt,(void *)newspec->ipt,sizeof(oldspec->ipt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->apt,(void *)newspec->apt,sizeof(oldspec->apt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ht,(void *)&newspec->ht,sizeof(oldspec->ht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->xscl,(void *)&newspec->xscl,sizeof(oldspec->xscl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rot,(void *)&newspec->rot,sizeof(oldspec->rot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->obl,(void *)&newspec->obl,sizeof(oldspec->obl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tship,(void *)&newspec->tship,sizeof(oldspec->tship));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->gen,(void *)&newspec->gen,sizeof(oldspec->gen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,(void *)&newspec->flags,sizeof(oldspec->flags));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->fldlen,(void *)&newspec->fldlen,sizeof(oldspec->fldlen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->hjust,(void *)&newspec->hjust,sizeof(oldspec->hjust));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->vjust,(void *)&newspec->vjust,sizeof(oldspec->vjust));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_ATTRIB:
			{
				db_attrib *newspec=(db_attrib *)newelp;
				db_attrib *oldspec=(db_attrib *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->val,(void *)&newspec->val,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tag,(void *)&newspec->tag,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->ipt,(void *)newspec->ipt,sizeof(oldspec->ipt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->apt,(void *)newspec->apt,sizeof(oldspec->apt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ht,(void *)&newspec->ht,sizeof(oldspec->ht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->xscl,(void *)&newspec->xscl,sizeof(oldspec->xscl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rot,(void *)&newspec->rot,sizeof(oldspec->rot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->obl,(void *)&newspec->obl,sizeof(oldspec->obl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tship,(void *)&newspec->tship,sizeof(oldspec->tship));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->gen,(void *)&newspec->gen,sizeof(oldspec->gen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,(void *)&newspec->flags,sizeof(oldspec->flags));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->fldlen,(void *)&newspec->fldlen,sizeof(oldspec->fldlen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->hjust,(void *)&newspec->hjust,sizeof(oldspec->hjust));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->vjust,(void *)&newspec->vjust,sizeof(oldspec->vjust));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_VERTEX:
			{
				db_vertex *newspec=(db_vertex *)newelp;
				db_vertex *oldspec=(db_vertex *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->wid[0],(void *)&newspec->wid[0],sizeof(oldspec->wid[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->wid[1],(void *)&newspec->wid[1],sizeof(oldspec->wid[1]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->bulge,(void *)&newspec->bulge,sizeof(oldspec->bulge));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,(void *)&newspec->flags,sizeof(oldspec->flags));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tandir,(void *)&newspec->tandir,sizeof(oldspec->tandir));
				if(newspec->vidx!=NULL && oldspec->vidx==NULL) {
					oldspec->vidx=new int[4];
					memcpy(oldspec->vidx,newspec->vidx,4*sizeof(int));
				}
				if(oldspec->vidx && newspec->vidx) {
					for(fi1=0; fi1<4; ++fi1) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->vidx[fi1],(void *)&newspec->vidx[fi1],sizeof(oldspec->vidx[fi1]));
					}
				}
			}
			break;
		case DB_POLYLINE:
			{
				db_polyline *newspec=(db_polyline *)newelp;
				db_polyline *oldspec=(db_polyline *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->elev,(void *)&newspec->elev,sizeof(oldspec->elev));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,(void *)&newspec->flags,sizeof(oldspec->flags));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->defwid[0],(void *)&newspec->defwid[0],sizeof(oldspec->defwid[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->defwid[1],(void *)&newspec->defwid[1],sizeof(oldspec->defwid[1]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->count[0],(void *)&newspec->count[0],sizeof(oldspec->count[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->count[1],(void *)&newspec->count[1],sizeof(oldspec->count[1]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dens[0],(void *)&newspec->dens[0],sizeof(oldspec->dens[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dens[1],(void *)&newspec->dens[1],sizeof(oldspec->dens[1]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->stype,(void *)&newspec->stype,sizeof(oldspec->stype));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,(void *)&newspec->thick,sizeof(oldspec->thick));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_BLOCK:
			{
				db_block *newspec=(db_block *)newelp;
				db_block *oldspec=(db_block *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->base,(void *)newspec->base,sizeof(oldspec->base));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->xrefpn,(void *)&newspec->xrefpn,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->name,(void *)&newspec->name,0);
			}
			break;
		case DB_SEQEND:
		case DB_ENDBLK:
			{
				// Nothing to check.
			}
			break;
		case DB_DIMENSION:
			{
				db_dimension *newspec=(db_dimension *)newelp;
				db_dimension *oldspec=(db_dimension *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dimtext,(void *)&newspec->dimtext,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->bthip,(void *)&newspec->bthip,sizeof(oldspec->bthip));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dimtype,(void *)&newspec->dimtype,sizeof(oldspec->dimtype));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->leadlen,(void *)&newspec->leadlen,sizeof(oldspec->leadlen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ang,(void *)&newspec->ang,sizeof(oldspec->ang));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->textrot,(void *)&newspec->textrot,sizeof(oldspec->textrot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->hdir,(void *)&newspec->hdir,sizeof(oldspec->hdir));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dship,(void *)&newspec->dship,sizeof(oldspec->dship));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->slant,(void *)&newspec->slant,sizeof(oldspec->slant));
				for(fi1=0; fi1<7; ++fi1) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt[fi1],(void *)newspec->pt[fi1],sizeof(oldspec->pt[fi1]));
				}
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_ELLIPSE:
			{
				db_ellipse *newspec=(db_ellipse *)newelp;
				db_ellipse *oldspec=(db_ellipse *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->cent,(void *)newspec->cent,sizeof(oldspec->cent));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->smavect,(void *)newspec->smavect,sizeof(oldspec->smavect));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->min_maj,(void *)&newspec->min_maj,sizeof(oldspec->min_maj));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->param[0],(void *)&newspec->param[0],sizeof(oldspec->param[0]));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->param[1],(void *)&newspec->param[1],sizeof(oldspec->param[1]));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_VIEWPORT:
			{
				db_viewport *newspec=(db_viewport *)newelp;
				db_viewport *oldspec=(db_viewport *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->cent,(void *)newspec->cent,sizeof(oldspec->cent));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->wd,(void *)&newspec->wd,sizeof(oldspec->wd));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ht,(void *)&newspec->ht,sizeof(oldspec->ht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->id,(void *)&newspec->id,sizeof(oldspec->id));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->stat,(void *)&newspec->stat,sizeof(oldspec->stat));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,(void *)&newspec->flags,sizeof(oldspec->flags));
			}
			break;
		case DB_RAY:
			{
				db_ray *newspec=(db_ray *)newelp;
				db_ray *oldspec=(db_ray *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->vect,(void *)newspec->vect,sizeof(oldspec->vect));
			}
			break;
		case DB_XLINE:
			{
				db_xline *newspec=(db_xline *)newelp;
				db_xline *oldspec=(db_xline *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->vect,(void *)newspec->vect,sizeof(oldspec->vect));
			}
			break;
		case DB_MTEXT:
			{
				CDbMText *newspec=(CDbMText *)newelp;
				CDbMText *oldspec=(CDbMText *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_text,(void *)&newspec->m_text,0,db_BinChange::DBSTRING);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_insertionPoint,(void *)newspec->m_insertionPoint,sizeof(oldspec->m_insertionPoint));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_ox,(void *)newspec->m_ox,sizeof(oldspec->m_ox));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_initialWidth,(void *)&newspec->m_initialWidth,sizeof(oldspec->m_initialWidth));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_initialHeight,(void *)&newspec->m_initialHeight,sizeof(oldspec->m_initialHeight));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_pTextStyle,(void *)&newspec->m_pTextStyle,sizeof(oldspec->m_pTextStyle));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_attachTo,(void *)&newspec->m_attachTo,sizeof(oldspec->m_attachTo));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_direction,(void *)&newspec->m_direction,sizeof(oldspec->m_direction));

				//EBATECH(CNBR) -[ 2002/6/10 somtimes get_entext() returns (0,0,0)-(0,0,0)
				//db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_actualWidth,(void *)&newspec->m_actualWidth,sizeof(oldspec->m_actualWidth));
				//db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->m_actualHeight,(void *)&newspec->m_actualHeight,sizeof(oldspec->m_actualHeight));
				//EBATECH(CNBR) ]-
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_SPLINE:
			{
				db_spline *newspec=(db_spline *)newelp;
				db_spline *oldspec=(db_spline *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->scenario,(void *)&newspec->scenario,sizeof(&oldspec->scenario));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->tandir,(void *)newspec->tandir,sizeof(oldspec->tandir));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ktol,(void *)&newspec->ktol,sizeof(oldspec->ktol));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ctol,(void *)&newspec->ctol,sizeof(oldspec->ctol));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ftol,(void *)&newspec->ftol,sizeof(oldspec->ftol));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,(void *)&newspec->flags,sizeof(oldspec->flags));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->deg,(void *)&newspec->deg,sizeof(oldspec->deg));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->nknots,(void *)&newspec->nknots,sizeof(oldspec->nknots));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ncpts,(void *)&newspec->ncpts,sizeof(oldspec->ncpts));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->nfpts,(void *)&newspec->nfpts,sizeof(oldspec->nfpts));

				/*DG - 6.11.2001*/// Use this method for allocated data members (which length may be changed).
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->fpt, (void**)&newspec->fpt, oldspec->nfpts * sizeof(*oldspec->fpt));
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->knot, (void**)&newspec->knot, oldspec->nknots * sizeof(*oldspec->knot));
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->control, (void**)&newspec->control, oldspec->ncpts * sizeof(*oldspec->control));

/*				if(oldspec->nfpts!=newspec->nfpts) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->fpt,(void *)newspec->fpt,sizeof(sds_point)*oldspec->nfpts*-1);
				} else {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->fpt,(void *)newspec->fpt,sizeof(sds_point)*oldspec->nfpts);
				}
				if(oldspec->nknots!=newspec->nknots) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->knot,(void *)newspec->knot,sizeof(sds_real)*oldspec->nknots*-1);
				} else {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->knot,(void *)newspec->knot,sizeof(sds_real)*oldspec->nknots);
				}
				if(oldspec->ncpts!=newspec->ncpts) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->control,(void *)newspec->control,sizeof(db_splinecontrollink)*oldspec->ncpts*-1);
				} else {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->control,(void *)newspec->control,sizeof(db_splinecontrollink)*oldspec->ncpts);
				}*/

				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_TOLERANCE:
			{
				db_tolerance *newspec=(db_tolerance *)newelp;
				db_tolerance *oldspec=(db_tolerance *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->primtext,(void *)&newspec->primtext,0);
				/*DG - 11.1.2002*/// Use addresses of dship, i.e. &dship :
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dship,(void *)&newspec->dship,sizeof(oldspec->dship));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->xdir,(void *)&newspec->xdir,sizeof(oldspec->xdir));
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_IMAGE:
			{
				db_image *newspec=(db_image *)newelp;
				db_image *oldspec=(db_image *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pt,(void *)newspec->pt,sizeof(oldspec->pt));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->pixelvect,(void *)newspec->pixelvect,sizeof(oldspec->pixelvect));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->imagesz,(void *)newspec->imagesz,sizeof(oldspec->imagesz));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dispprops,(void *)&newspec->dispprops,sizeof(oldspec->dispprops));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->boundarytype,(void *)&newspec->boundarytype,sizeof(oldspec->boundarytype));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->classver,(void *)&newspec->classver,sizeof(oldspec->classver));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->nclipverts,(void *)&newspec->nclipverts,sizeof(oldspec->nclipverts));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->clipon,(void *)&newspec->clipon,sizeof(oldspec->clipon));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->brightness,(void *)&newspec->brightness,sizeof(oldspec->brightness));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->contrast,(void *)&newspec->contrast,sizeof(oldspec->contrast));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->fade,(void *)&newspec->fade,sizeof(oldspec->fade));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->clipon,(void *)&newspec->clipon,sizeof(oldspec->clipon));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->clipon,(void *)&newspec->clipon,sizeof(oldspec->clipon));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->clipon,(void *)&newspec->clipon,sizeof(oldspec->clipon));

				/*DG - 6.11.2001*/// Use this method for allocated data members (which length may be changed).
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->clipvert, (void**)&newspec->clipvert, oldspec->nclipverts * sizeof(*oldspec->clipvert));

				/*if(oldspec->nclipverts!=newspec->nclipverts) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->clipvert,(void *)newspec->clipvert,sizeof(sds_real)*2*oldspec->nclipverts*-1);
				} else {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->clipvert,(void *)newspec->clipvert,sizeof(sds_real)*2*oldspec->nclipverts);
				}*/

				/*DG - 11.1.2002*/// Use addresses of defref and defreactref, i.e. &defref and &defreactref:
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->defref,(void *)&newspec->defref,sizeof(oldspec->defref));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->defreactref,(void *)&newspec->defreactref,sizeof(oldspec->defreactref));
				// TODO Someday
				// char 	   *grdata; 	   /* MarComp needs this */
				// int			grdatasz;	   /* MarComp needs this */
			}
			break;
		case DB_HATCH:
			{
				//TODO

				// 6/98 fixed crash 8322 by adding break so DB_HATCH case does not fall throught to DB_LEADER. I started
				// to implement this for db_hatch, but I realized it has some larger undo implications. db_BinChange::CacheChangedData
				// does not handle data variable length lists ie. hatch pattern lines. Changes will be also be needed in
				// SDS_UndoGroup to handle different length lists. Since hatches are only displayable and not editable at this
				// point, I decided not to risk making such significant changes without at least being able to easily test it.
				// Therefore I am deferring this implementation until editable hatches are implemented.  Commented below
				// is my start to the implementation, with some comments on where I left off.

				// skip disp, & extent	other entities to save these for undo

				// below are the straight forward entities that need to be compared
				db_hatch *newspec=(db_hatch *)newelp;
				db_hatch *oldspec=(db_hatch *)oldelp;
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ang,(void *)&newspec->ang,sizeof(oldspec->ang),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->assoc,(void *)&newspec->assoc,sizeof(oldspec->assoc),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->elev,(void *)&newspec->elev,sizeof(oldspec->elev),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->isdouble,(void *)&newspec->isdouble,sizeof(oldspec->isdouble),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)oldspec->patname,(void *)newspec->patname,sizeof(oldspec->patname),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->pattype,(void *)&newspec->pattype,sizeof(oldspec->pattype),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->pixelsz,(void *)&newspec->pixelsz,sizeof(oldspec->pixelsz),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->scale,(void *)&newspec->scale,sizeof(oldspec->scale),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->solidfill,(void *)&newspec->solidfill,sizeof(oldspec->solidfill),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->style,(void *)&newspec->style,sizeof(oldspec->style),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->npatlines,(void *)&newspec->npatlines,sizeof(oldspec->npatlines),db_HatchChange::PARENT);
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)&oldspec->nseeds,(void *)&newspec->nseeds,sizeof(oldspec->nseeds),db_HatchChange::PARENT);

				// don't save npaths separately because that is done when the paths are stored.
				// save paths
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)oldspec,(void *)newspec,0,db_HatchChange::BOUNDARY);

				// save pattern info
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)oldspec,(void *)newspec,0,db_HatchChange::PATTERN);

				// save seed points
				db_HatchChange::CacheChangedData(&begll,&curll,(void *)oldspec,(void *)newspec,0,db_HatchChange::SEED);
			}
			break;
		case DB_LWPOLYLINE:
			{
				db_lwpolyline *newspec=(db_lwpolyline *)newelp;
				db_lwpolyline *oldspec=(db_lwpolyline *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flags,	 (void *)&newspec->flags,	 sizeof(oldspec->flags));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->nverts,	 (void *)&newspec->nverts,	 sizeof(oldspec->nverts));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->thick,	 (void *)&newspec->thick,	 sizeof(oldspec->thick));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->elev,	 (void *)&newspec->elev,	 sizeof(oldspec->elev));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->constwid, (void *)&newspec->constwid, sizeof(oldspec->constwid));

				// DP: number of vertices may be changed during for example BREAK command
				//db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->vert,		 (void *)newspec->vert, 	 oldspec->nverts * sizeof( *oldspec->vert));
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->vert, (void**)&newspec->vert, oldspec->nverts * sizeof(*oldspec->vert));

				/*DG - 6.11.2001*/// Use this method for allocated data members (which length may be changed).
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->wid, (void**)&newspec->wid, oldspec->nverts * sizeof(*oldspec->wid));
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->bulge, (void**)&newspec->bulge, oldspec->nverts * sizeof(*oldspec->bulge));

				/*db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->wid,	   (void *)newspec->wid,	   oldspec->nverts * sizeof( *oldspec->wid));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->bulge,	 (void *)newspec->bulge,	 oldspec->nverts * sizeof( *oldspec->bulge));*/

				// Checking for the change of extrusion dir /210 group
				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_LEADER:
			{
				db_leader *newspec=(db_leader *)newelp;
				db_leader *oldspec=(db_leader *)oldelp;
				//Bugzilla No. 78112 ; 15-04-2002
				//db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->dship,(void *)newspec->dship,sizeof(oldspec->dship));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dship,(void *)&newspec->dship,sizeof(oldspec->dship));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->hdir,(void *)newspec->hdir,sizeof(oldspec->hdir));
				//Bugzilla No. 78044 ; 25-03-2002
				//db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->anoffset,(void *)newspec->anoffset,sizeof(oldspec->anoffset));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->offsettoblockinspt,(void *)newspec->offsettoblockinspt,sizeof(oldspec->offsettoblockinspt));
				//Bugzilla No. 78112 ; 15-04-2002
				//db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->annothip,(void *)newspec->annothip,sizeof(oldspec->annothip));
				db_LeaderChange::CacheChangedData(&begll,&curll,(void *)&oldspec->annothip,(void *)&newspec->annothip, -sizeof(oldspec->annothip));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->antextht,(void *)&newspec->antextht,sizeof(oldspec->antextht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->antextwd,(void *)&newspec->antextwd,sizeof(oldspec->antextwd));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->arrow,(void *)&newspec->arrow,sizeof(oldspec->arrow));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->pathtype,(void *)&newspec->pathtype,sizeof(oldspec->pathtype));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->creation,(void *)&newspec->creation,sizeof(oldspec->creation));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->hookdir,(void *)&newspec->hookdir,sizeof(oldspec->hookdir));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->hook,(void *)&newspec->hook,sizeof(oldspec->hook));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->nverts,(void *)&newspec->nverts,sizeof(oldspec->nverts));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->bbcolor,(void *)&newspec->bbcolor,sizeof(oldspec->bbcolor));
				//Bugzilla No. 78013 ; 01-04-2002
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->txtoffset,(void *)&newspec->txtoffset,sizeof(oldspec->txtoffset));

				/*DG - 6.11.2001*/// Use this method for allocated data members (which length may be changed).
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->vert, (void**)&newspec->vert, oldspec->nverts * sizeof(*oldspec->vert));

				/*if(oldspec->nverts!=newspec->nverts) {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->vert,(void *)newspec->vert,sizeof(sds_point)*oldspec->nverts*-1);
				} else {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->vert,(void *)newspec->vert,sizeof(sds_point)*oldspec->nverts);
				}*/

				if(newspec->m_Extrusion) {
					if(oldspec->m_Extrusion==NULL) {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)defextru,(void *)newspec->m_Extrusion,sizeof(sds_point));
					} else {
						db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_Extrusion,(void *)newspec->m_Extrusion,sizeof(sds_point));
					}
				}
			}
			break;
		case DB_MLINE:
			{
				db_mline *newspec=(db_mline *)newelp;
				db_mline *oldspec=(db_mline *)oldelp;
			}
			break;
		case DB_OLE2FRAME:
			{
				db_ole2frame *newspec=(db_ole2frame *)newelp;
				db_ole2frame *oldspec=(db_ole2frame *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->corner,(void *)newspec->corner,sizeof(oldspec->corner));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->unk3,(void *)newspec->unk3,sizeof(oldspec->unk3));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->unk1,(void *)&newspec->unk1,sizeof(oldspec->unk1));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->unk2,(void *)&newspec->unk2,sizeof(oldspec->unk2));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ver,(void *)&newspec->ver,sizeof(oldspec->ver));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->oletype,(void *)&newspec->oletype,sizeof(oldspec->oletype));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tilemodedesc,(void *)&newspec->tilemodedesc,sizeof(oldspec->tilemodedesc));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->desc,(void *)&newspec->desc,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->datasz,(void *)&newspec->datasz,sizeof(oldspec->datasz));

				/*DG - 6.11.2001*/// Use this method for allocated data members (which length may be changed).
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->data, (void**)&newspec->data, oldspec->datasz * sizeof(*oldspec->data));

				/*if(oldspec->datasz!=newspec->datasz) {
					// !!!!!!!!!!!!!!!
					// !!!!!!!!!!!!!!!
					// !!!!!!!!!!!!!!!
					// !!!!!!!!!!!!!!!
					// BAD BAD BAD -- passing negative for length is supposed to deal with unequal lengths, but
					// the code doesn't properly allocate the right amount of space.  We had to change this
					// for eed, and we'll have to change it for OLE
					//
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->data,(void *)newspec->data,oldspec->datasz*-1);
				} else {
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->data,(void *)newspec->data,oldspec->datasz);
				}*/
			}
			break;
		case DB_BODY:
		case DB_3DSOLID:
		case DB_REGION:
			{
				// TODO
				CDbAcisEntity* oldspec = (CDbAcisEntity*)oldelp;
				CDbAcisEntity* newspec = (CDbAcisEntity*)newelp;
				const CDbAcisEntityData& olddata = oldspec->getData();
				const CDbAcisEntityData& newdata = newspec->getData();

				db_BinChange::CacheChangedData(&begll,&curll,const_cast<icl::string*>(olddata.getAcisData()),
												const_cast<icl::string*>(newdata.getAcisData()));
			}
			break;
		case DB_APPIDTABREC:
		case DB_BLOCKTABREC:
			// Nothing to do.
			break;
		case DB_DIMSTYLETABREC:
			{
				// NOTE : EBATECH(CNBR) 2003-3-31 add notes for old fasion system variables such as DIMFIT=DIMATFIT+DIMTMOVE.
				// keep DIMAZIN(79),DIMALTRND(148),DMIADEC(179),DIMDSEP(278),DIMLDRBLK(341),DIMLWD(371),DIMLWE(372)

				db_dimstyletabrec *newspec=(db_dimstyletabrec *)newelp;
				db_dimstyletabrec *oldspec=(db_dimstyletabrec *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->post,(void *)&newspec->post,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->apost,(void *)&newspec->apost,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->blk,(void *)&newspec->blk,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->blk1,(void *)&newspec->blk1,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->blk2,(void *)&newspec->blk2,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->scale,(void *)&newspec->scale,sizeof(oldspec->scale));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->asz,(void *)&newspec->asz,sizeof(oldspec->asz));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->exo,(void *)&newspec->exo,sizeof(oldspec->exo));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dli,(void *)&newspec->dli,sizeof(oldspec->dli));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->exe,(void *)&newspec->exe,sizeof(oldspec->exe));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->rnd,(void *)&newspec->rnd,sizeof(oldspec->rnd));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dle,(void *)&newspec->dle,sizeof(oldspec->dle));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tp,(void *)&newspec->tp,sizeof(oldspec->tp));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tm,(void *)&newspec->tm,sizeof(oldspec->tm));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tol,(void *)&newspec->tol,sizeof(oldspec->tol));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->lim,(void *)&newspec->lim,sizeof(oldspec->lim));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tih,(void *)&newspec->tih,sizeof(oldspec->tih));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->toh,(void *)&newspec->toh,sizeof(oldspec->toh));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->se1,(void *)&newspec->se1,sizeof(oldspec->se1));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->se2,(void *)&newspec->se2,sizeof(oldspec->se2));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->tad,(void *)&newspec->tad,sizeof(oldspec->tad));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->zin,(void *)&newspec->zin,sizeof(oldspec->zin));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->azin,(void *)&newspec->azin,sizeof(oldspec->azin));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->txt,(void*)&newspec->txt,sizeof(oldspec->txt));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->cen,(void*)&newspec->cen,sizeof(oldspec->cen));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tsz,(void*)&newspec->tsz,sizeof(oldspec->tsz));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->altf,(void*)&newspec->altf,sizeof(oldspec->altf));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->lfac,(void*)&newspec->lfac,sizeof(oldspec->lfac));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tvp,(void*)&newspec->tvp,sizeof(oldspec->tvp));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tfac,(void*)&newspec->tfac,sizeof(oldspec->tfac));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->gap,(void*)&newspec->gap,sizeof(oldspec->gap));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->altrnd,(void*)&newspec->altrnd,sizeof(oldspec->altrnd));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->alt,(void*)&newspec->alt,sizeof(oldspec->alt));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->altd,(void*)&newspec->altd,sizeof(oldspec->altd));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tofl,(void*)&newspec->tofl,sizeof(oldspec->tofl));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->sah,(void*)&newspec->sah,sizeof(oldspec->sah));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tix,(void*)&newspec->tix,sizeof(oldspec->tix));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->soxd,(void*)&newspec->soxd,sizeof(oldspec->soxd));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->clrd,(void*)&newspec->clrd,sizeof(oldspec->clrd));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->clre,(void*)&newspec->clre,sizeof(oldspec->clre));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->clrt,(void*)&newspec->clrt,sizeof(oldspec->clrt));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->adec,(void*)&newspec->adec,sizeof(oldspec->adec));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->unit,(void*)&newspec->unit,sizeof(oldspec->unit));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->dec,(void*)&newspec->dec,sizeof(oldspec->dec));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tdec,(void*)&newspec->tdec,sizeof(oldspec->tdec));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->altu,(void*)&newspec->altu,sizeof(oldspec->altu));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->alttd,(void*)&newspec->alttd,sizeof(oldspec->alttd));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->aunit,(void*)&newspec->aunit,sizeof(oldspec->aunit));
				// Note: dimfrac(276) -> dimunit(270)
				// Note: dimlunit(277) -> dimunit(270)
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->dsep,(void*)&newspec->dsep,sizeof(oldspec->dsep));
				// Note: dimtmove(279)->dimfit(287)
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->just,(void*)&newspec->just,sizeof(oldspec->just));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->sd1,(void*)&newspec->sd1,sizeof(oldspec->sd1));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->sd2,(void*)&newspec->sd2,sizeof(oldspec->sd2));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tolj,(void*)&newspec->tolj,sizeof(oldspec->tolj));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->tzin,(void*)&newspec->tzin,sizeof(oldspec->tzin));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->altz,(void*)&newspec->altz,sizeof(oldspec->altz));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->alttz,(void*)&newspec->alttz,sizeof(oldspec->alttz));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->fit,(void*)&newspec->fit,sizeof(oldspec->fit));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->upt,(void*)&newspec->upt,sizeof(oldspec->upt));
				// Note: dimatfit(289) -> dimfit(287)
				/*DG - 11.1.2002*/// Use addresses of txsty, i.e. &txsty :
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->txsty,(void *)&newspec->txsty,sizeof(oldspec->txsty));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->ldrblk,(void*)&newspec->ldrblk,sizeof(oldspec->ldrblk));
				// Note: dimblk(342) -> dimblk(5)
				// Note: dimblk1(343) -> dimblk1(6)
				// Note: dimblk2(344) -> dimblk2(7)
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->lwd,(void*)&newspec->lwd,sizeof(oldspec->lwd));
				db_BinChange::CacheChangedData(&begll,&curll,(void*)&oldspec->lwe,(void*)&newspec->lwe,sizeof(oldspec->lwe));
			}
			break;
		case DB_LAYERTABREC:
			{
				db_layertabrec *newspec=(db_layertabrec *)newelp;
				db_layertabrec *oldspec=(db_layertabrec *)oldelp;
				/*DG - 11.1.2002*/// Use addresses of ltypehip, i.e. &ltypehip :
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ltypehip,(void *)&newspec->ltypehip,sizeof(oldspec->ltypehip));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->color,(void *)&newspec->color,sizeof(oldspec->color));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->lineweight,(void *)&newspec->lineweight,sizeof(oldspec->lineweight));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->plottable,(void *)&newspec->plottable,sizeof(oldspec->plottable));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->plotstylehip,(void *)&newspec->plotstylehip,sizeof(oldspec->plotstylehip));
			}
			break;
		case DB_LTYPETABREC:
			{
				db_ltypetabrec *newspec=(db_ltypetabrec *)newelp;
				db_ltypetabrec *oldspec=(db_ltypetabrec *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->desc,(void *)&newspec->desc,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->aligncd,(void *)&newspec->aligncd,sizeof(oldspec->aligncd));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ndashes,(void *)&newspec->ndashes,sizeof(oldspec->ndashes));
				/*DG - 11.1.2002*/// Use this method for allocated data members (which length may be changed).
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&oldspec->dash, (void**)&newspec->dash, oldspec->ndashes * sizeof(*oldspec->dash));
//				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->dash,(void *)newspec->dash,sizeof(newspec->dash));
			}
			break;
		case DB_STYLETABREC:
			{
				db_styletabrec *newspec=(db_styletabrec *)newelp;
				db_styletabrec *oldspec=(db_styletabrec *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->fontpn,(void *)&newspec->fontpn,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->bigfontpn,(void *)&newspec->bigfontpn,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->fixedht,(void *)&newspec->fixedht,sizeof(oldspec->fixedht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->xscl,(void *)&newspec->xscl,sizeof(oldspec->xscl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->lastht,(void *)&newspec->lastht,sizeof(oldspec->lastht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->obl,(void *)&newspec->obl,sizeof(oldspec->obl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->gen,(void *)&newspec->gen,sizeof(oldspec->gen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->localName,(void *)&newspec->localName,0); // EBATECH(CNBR)
				 /*DG - 10.1.2002*/// Cache the 'font' field too.
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->font,(void *)&newspec->font, sizeof(oldspec->font));
			}
			break;
		case DB_UCSTABREC:
			{
				db_ucstabrec *newspec=(db_ucstabrec *)newelp;
				db_ucstabrec *oldspec=(db_ucstabrec *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_orig,(void *)newspec->m_orig,sizeof(oldspec->m_orig));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_xdir,(void *)newspec->m_xdir,sizeof(oldspec->m_xdir));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->m_ydir,(void *)newspec->m_ydir,sizeof(oldspec->m_ydir));
			}
			break;
		case DB_VIEWTABREC:
			{
				db_viewtabrec *newspec=(db_viewtabrec *)newelp;
				db_viewtabrec *oldspec=(db_viewtabrec *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->cent,(void *)newspec->cent,sizeof(oldspec->cent));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->dir,(void *)newspec->dir,sizeof(oldspec->dir));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->target,(void *)newspec->target,sizeof(oldspec->target));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ht,(void *)&newspec->ht,sizeof(oldspec->ht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->wd,(void *)&newspec->wd,sizeof(oldspec->wd));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->lenslen,(void *)&newspec->lenslen,sizeof(oldspec->lenslen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->clip,(void *)newspec->clip,sizeof(oldspec->clip));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->twist,(void *)&newspec->twist,sizeof(oldspec->twist));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->mode,(void *)&newspec->mode,sizeof(oldspec->mode));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->unk,(void *)&newspec->unk,sizeof(oldspec->unk));
			}
			break;
		case DB_VPORTTABREC:
			{
				db_vporttabrec *newspec=(db_vporttabrec *)newelp;
				db_vporttabrec *oldspec=(db_vporttabrec *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->corner,(void *)newspec->corner,sizeof(oldspec->corner));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->cent,(void *)newspec->cent,sizeof(oldspec->cent));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->snapbase,(void *)newspec->snapbase,sizeof(oldspec->snapbase));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->snapspace,(void *)newspec->snapspace,sizeof(oldspec->snapspace));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->gridspace,(void *)newspec->gridspace,sizeof(oldspec->gridspace));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->dir,(void *)newspec->dir,sizeof(oldspec->dir));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->target,(void *)newspec->target,sizeof(oldspec->target));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ht,(void *)&newspec->ht,sizeof(oldspec->ht));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->asp,(void *)&newspec->asp,sizeof(oldspec->asp));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->lenslen,(void *)&newspec->lenslen,sizeof(oldspec->lenslen));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->clip,(void *)newspec->clip,sizeof(oldspec->clip));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->snaprot,(void *)&newspec->snaprot,sizeof(oldspec->snaprot));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->twist,(void *)&newspec->twist,sizeof(oldspec->twist));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->stat,(void *)&newspec->stat,sizeof(oldspec->stat));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->id,(void *)&newspec->id,sizeof(oldspec->id));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->mode,(void *)&newspec->mode,sizeof(oldspec->mode));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->czoom,(void *)&newspec->czoom,sizeof(oldspec->czoom));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->fzoom,(void *)&newspec->fzoom,sizeof(oldspec->fzoom));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->ucsicon,(void *)&newspec->ucsicon,sizeof(oldspec->ucsicon));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->snap,(void *)&newspec->snap,sizeof(oldspec->snap));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->grid,(void *)&newspec->grid,sizeof(oldspec->grid));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->snapstyl,(void *)&newspec->snapstyl,sizeof(oldspec->snapstyl));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->snapisopair,(void *)&newspec->snapisopair,sizeof(oldspec->snapisopair));
			}
			break;
		case DB_VXTABREC:
			{
				db_vxtabrec *newspec=(db_vxtabrec *)newelp;
				db_vxtabrec *oldspec=(db_vxtabrec *)oldelp;
				/*DG - 11.1.2002*/// Use addresses of vpehip, i.e. &vpehip :
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->vpehip,(void *)&newspec->vpehip,sizeof(oldspec->vpehip));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->flag,(void *)&newspec->flag,sizeof(oldspec->flag));
			}
			break;
		case DB_IMAGEDEF:
			{
				db_imagedef *newspec=(db_imagedef *)newelp;
				db_imagedef *oldspec=(db_imagedef *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->filepn,(void *)&newspec->filepn,0);
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->deleted,(void *)&newspec->deleted,sizeof(oldspec->deleted));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->looked4image,(void *)&newspec->looked4image,sizeof(oldspec->looked4image));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dxf90,(void *)&newspec->dxf90,sizeof(oldspec->dxf90));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dxf280,(void *)&newspec->dxf280,sizeof(oldspec->dxf280));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dxf281,(void *)&newspec->dxf281,sizeof(oldspec->dxf281));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->imagesz,(void *)newspec->imagesz,sizeof(oldspec->imagesz));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)oldspec->dxf11,(void *)newspec->dxf11,sizeof(oldspec->dxf11));
/*
				// if this is ever enabled - need to use db_handitem::getReactor as this has replaced the
				// get_preactllends()

				// Linked list of db_imagedef_reactors
				db_hireflink *newfirstpp, *newlastpp, *oldfirstpp, *oldlastpp;
				if (newspec != NULL)
					newspec->get_preactllends(&newfirstpp, &newlastpp);
				if (oldspec != NULL)
					oldspec->get_preactllends(&oldfirstpp, &oldlastpp);

				while(newfirstpp != NULL || oldfirstpp != NULL) {
					int  *newtypep, *oldtypep, newtype, oldtype;
					char *newhandpp, *oldhandpp;
					db_handitem *newhipp, *oldhipp;

					if (newfirstpp != NULL) {
						// get_data want a int*, db_BinChange::CacheChangedData wants a (void **)
						newfirstpp->get_data(&newtype, &newhandpp, &newhipp, (db_drawing*)SDS_CURDWG);
						newtypep = &newtype;
					}
					if (oldfirstpp != NULL) {
						// get_data want a int*, db_BinChange::CacheChangedData wants a (void **)
						oldfirstpp->get_data(&oldtype, &oldhandpp, &oldhipp, (db_drawing*)SDS_CURDWG);
						oldtypep = &oldtype;
					} else {
						oldtypep  = (int *)CALLOC(1,sizeof(int));
						oldhandpp = (char *)CALLOC(1, sizeof(char));
						oldhipp   = (db_handitem *) CALLOC(1, sizeof(db_handitem));
					}
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldtypep,	(void *)newtypep,  sizeof(oldtypep));
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldhandpp, (void *)newhandpp, sizeof(oldtypep));
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldhipp,	(void *)newhipp,   sizeof(oldtypep));

					if(newfirstpp != NULL)
						newfirstpp = newfirstpp->next;
					if(oldfirstpp != NULL) {
						oldfirstpp = oldfirstpp->next;
					} else {
						free(oldtypep);
						free(oldhandpp);
						free(oldhipp);
					}
				}
*/
			}
			break;
		case DB_IMAGEDEF_REACTOR:
			{
				db_imagedef_reactor *newspec=(db_imagedef_reactor *)newelp;
				db_imagedef_reactor *oldspec=(db_imagedef_reactor *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->deleted,(void *)&newspec->deleted,sizeof(oldspec->deleted));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->classver,(void *)&newspec->classver,sizeof(oldspec->classver));
/*
				// Linked list of images. Note that even though there can only be one,
				// for some reason it has been coded as possibly haveing multiple in the
				// class def.
				db_hireflink *newfirstpp, *newlastpp, *oldfirstpp, *oldlastpp;
				if (newspec != NULL)
					newspec->get_hirefllends(&newfirstpp, &newlastpp);
				if (oldspec != NULL)
					oldspec->get_hirefllends(&oldfirstpp, &oldlastpp);

				while(newfirstpp != NULL || oldfirstpp != NULL) {
					int  *newtypep, *oldtypep, newtype, oldtype;
					char *newhandpp, *oldhandpp;
					db_handitem *newhipp, *oldhipp;

					if (newfirstpp != NULL) {
						// get_data want a int*, db_BinChange::CacheChangedData wants a (void **)
						newfirstpp->get_data(&newtype, &newhandpp, &newhipp, (db_drawing*)SDS_CURDWG);
						newtypep = &newtype;
					}
					if (oldfirstpp != NULL) {
						// get_data want a int*, db_BinChange::CacheChangedData wants a (void **)
						oldfirstpp->get_data(&oldtype, &oldhandpp, &oldhipp, (db_drawing*)SDS_CURDWG);
						oldtypep = &oldtype;
					} else {
						oldtypep  = (int *)CALLOC(1,sizeof(int));
						oldhandpp = (char *)CALLOC(1, sizeof(char));
						oldhipp   = (db_handitem *) CALLOC(1, sizeof(db_handitem));
					}

					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldtypep,	(void *)newtypep,  sizeof(oldtypep));
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldhandpp, (void *)newhandpp, sizeof(oldtypep));
					db_BinChange::CacheChangedData(&begll,&curll,(void *)oldhipp,	(void *)newhipp,   sizeof(oldtypep));

					if(newfirstpp != NULL)
						newfirstpp = newfirstpp->next;
					if(oldfirstpp != NULL) {
						oldfirstpp = oldfirstpp->next;
					} else {
						free(oldtypep);
						free(oldhandpp);
						free(oldhipp);
					}
				}
*/
			}
			break;
		case DB_RASTERVARIABLES:
			{
				db_rastervariables *newspec=(db_rastervariables *)newelp;
				db_rastervariables *oldspec=(db_rastervariables *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->deleted,(void *)&newspec->deleted,sizeof(oldspec->deleted));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dxf70,(void *)&newspec->dxf70,sizeof(oldspec->dxf70));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dxf71,(void *)&newspec->dxf71,sizeof(oldspec->dxf71));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dxf72,(void *)&newspec->dxf72,sizeof(oldspec->dxf72));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->dxf90,(void *)&newspec->dxf90,sizeof(oldspec->dxf90));
			}
			break;
		case DB_XRECORD:
			{
				db_xrecord *newspec=(db_xrecord *)newelp;
				db_xrecord *oldspec=(db_xrecord *)oldelp;
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&oldspec->deleted,    (void *)&newspec->deleted,	 sizeof(oldspec->deleted));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)(sds_resbuf*)((db_xrecord *)oldspec)->getResbufChain(),   
															 (void *)(sds_resbuf*)((db_xrecord *)newspec)->getResbufChain(),  sizeof(sds_resbuf*));
				break;
			}
		case DB_DICTIONARY:
			{
				db_dictionary* pNew = (db_dictionary*)newelp;
				db_dictionary* pOld = (db_dictionary*)oldelp;
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_deleted, (void*)&pNew->m_deleted, sizeof(pOld->m_deleted));
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->hardownerflag, (void*)&pNew->hardownerflag, sizeof(pOld->hardownerflag)); // EBATECH(CNBR) 2002/6/30 Bug#78229
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->cloningflag, (void*)&pNew->cloningflag, sizeof(pOld->cloningflag));

				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->recll, (void*)&pNew->recll, -1);
				break;
			}
			// EBATECH(CNBR) -[ 2002/6/30 Bug#78227
		case DB_DICTIONARYWDFLT:
			{
				db_dictionarywdflt* pNew = (db_dictionarywdflt*)newelp;
				db_dictionarywdflt* pOld = (db_dictionarywdflt*)oldelp;
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->deleted, (void*)&pNew->deleted, sizeof(pOld->deleted));
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->numitems, (void*)&pNew->numitems, sizeof(pOld->numitems));
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->cloningflag, (void*)&pNew->cloningflag, sizeof(pOld->cloningflag));
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->defaultobject, (void*)&pNew->defaultobject, sizeof(pOld->defaultobject));
				db_DictionaryChange::CacheChangedData(&begll, &curll, (void*)&pOld->recll, (void*)&pNew->recll, -1);
				break;
			}
			// EBATECH(CNBR) ]-
			// EBATECH(CNBR) -[ 2002/9/6
		case DB_PLACEHOLDER:
			{
				db_placeholder *pNew=(db_placeholder *)newelp;
				db_placeholder *pOld=(db_placeholder *)oldelp;
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->deleted, (void*)&pNew->deleted, sizeof(pOld->deleted));
				db_BinChange::CacheChangedData(&begll,&curll,(void *)&pOld->ownerItem,(void *)&pNew->ownerItem,sizeof(pOld->ownerItem));
			}
			break;
			// EBATECH(CNBR) ]-
		case DB_GROUP:
			{
				CDbGroup* pNew = (CDbGroup*)newelp;
				CDbGroup* pOld = (CDbGroup*)oldelp;
				db_GroupChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_deleted, (void*)&pNew->m_deleted, sizeof(pOld->m_deleted));
				db_GroupChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_flags, (void*)&pNew->m_flags, sizeof(pOld->m_flags));
				db_GroupChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_pDescription, (void*)&pNew->m_pDescription, 0);

				db_GroupChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_pEntitiesList, (void*)&pNew->m_pEntitiesList, -1);
				break;
			}

		case DB_SPATIAL_FILTER:
			{
				db_spatial_filter	*pNew = (db_spatial_filter*)newelp,
									*pOld = (db_spatial_filter*)oldelp;

				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->deleted, (void*)&pNew->deleted, sizeof(pOld->deleted));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->npts, (void*)&pNew->npts, sizeof(pOld->npts));
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&pOld->pt, (void**)&pNew->pt, pOld->npts * sizeof(sds_point));
				if(pNew->extru)
				{
					if(!pOld->extru)
						db_BinChange::CacheChangedData(&begll, &curll, (void*)defextru, (void*)pNew->extru, sizeof(sds_point));
					else
						db_BinChange::CacheChangedData(&begll, &curll, (void*)pOld->extru, (void*)pNew->extru, sizeof(sds_point));
				}
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->clipon[0], (void*)&pNew->clipon[0], sizeof(pOld->clipon[0]));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->clipon[1], (void*)&pNew->clipon[1], sizeof(pOld->clipon[1]));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->clip[0], (void*)&pNew->clip[0], sizeof(pOld->clip[0]));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->clip[1], (void*)&pNew->clip[1], sizeof(pOld->clip[1]));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->dxf11, (void*)&pNew->dxf11, sizeof(pOld->dxf11));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->dxf71, (void*)&pNew->dxf71, sizeof(pOld->dxf71));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)pOld->inverseblockmat, (void*)pNew->inverseblockmat, sizeof(sds_point) * 4);
				db_BinChange::CacheChangedData(&begll, &curll, (void*)pOld->clipboundmat, (void*)pNew->clipboundmat, sizeof(sds_point) * 4);

				break;
			}
		// EBATECH(CNBR) -[ 2002/10/10 Bugzilla#78218
		//case DB_ACAD_PROXY_OBJECT:
		case DB_DICTIONARYVAR:
			{
				db_dictionaryvar	*pNew = (db_dictionaryvar*)newelp,
									*pOld = (db_dictionaryvar*)oldelp;

				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->deleted, (void*)&pNew->deleted, sizeof(pOld->deleted));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->unkint, (void*)&pNew->unkint, sizeof(pOld->unkint));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->unkstr, (void*)&pNew->unkstr, 0);
				break;
			}
		case DB_IDBUFFER:
			{
				db_idbuffer	*pNew = (db_idbuffer*)newelp,
							*pOld = (db_idbuffer*)oldelp;

				db_IdBufferChange::CacheChangedData(&begll, &curll, (void*)&pOld->deleted, (void*)&pNew->deleted, sizeof(pOld->deleted));
				db_IdBufferChange::CacheChangedData(&begll, &curll, (void*)&(pOld->hirefll[0]), (void*)&(pNew->hirefll[0]), -1);
				break;
			}
		//case DB_LAYER_INDEX:
		//case DB_MLINESTYLE: -- It's need.
		//case DB_OBJECT_PTR:
		case DB_SORTENTSTABLE:
			{
				CDbSortentsTable* pNew = (CDbSortentsTable*)newelp;
				CDbSortentsTable* pOld = (CDbSortentsTable*)oldelp;
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_deleted, (void*)&pNew->m_deleted, sizeof(pOld->m_deleted));

				db_MapChange<db_objhandle, db_objhandle>::CacheChangedData(&begll, &curll, (void*)&pOld->m_pTable, (void*)&pNew->m_pTable, -1);
				break;
			}
		//case DB_SPATIAL_INDEX:
		case DB_WIPEOUTVARIABLES:
			{
				db_wipeoutvariables	*pNew = (db_wipeoutvariables*)newelp,
								*pOld = (db_wipeoutvariables*)oldelp;

				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->deleted, (void*)&pNew->deleted, sizeof(pOld->deleted));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->dxf70, (void*)&pNew->dxf70, sizeof(pOld->dxf70));
				break;
			}
		case DB_VBAPROJECT:
			{
				db_vbaproject	*pNew = (db_vbaproject*)newelp,
								*pOld = (db_vbaproject*)oldelp;

				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->deleted, (void*)&pNew->deleted, sizeof(pOld->deleted));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->datasize, (void*)&pNew->datasize, sizeof(pOld->datasize));
				db_BinChange::CacheChangedBinaryData(&begll, &curll, (void**)&pOld->data, (void**)&pNew->data, pOld->datasize);
				break;
			}
		//case DB_DIMASSOC:
		//case DB_ARCALIGNEDTEXT:
		//case DB_RTEXT:
		//case DB_WIPEOUT:
		case DB_LAYOUT:
			{
				CDbLayout	*pNew = (CDbLayout*)newelp,
							*pOld = (CDbLayout*)oldelp;
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_deleted, (void*)&pNew->m_deleted, sizeof(pOld->m_deleted));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_layoutFlag, (void*)&pNew->m_layoutFlag, sizeof(short));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_tabOrder, (void*)&pNew->m_tabOrder, sizeof(short));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_limMin[0], (void*)&pNew->m_limMin[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_limMin[1], (void*)&pNew->m_limMin[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_limMax[0], (void*)&pNew->m_limMax[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_limMax[1], (void*)&pNew->m_limMax[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_insPoint, (void*)&pNew->m_insPoint, sizeof(pOld->m_insPoint));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_extMin, (void*)&pNew->m_extMin, sizeof(pOld->m_extMin));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_extMax, (void*)&pNew->m_extMax, sizeof(pOld->m_extMax));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_elevation, (void*)&pNew->m_elevation, sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_ucsO, (void*)&pNew->m_ucsO, sizeof(pOld->m_ucsO));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_ucsX, (void*)&pNew->m_ucsX, sizeof(pOld->m_ucsX));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_ucsY, (void*)&pNew->m_ucsY, sizeof(pOld->m_ucsY));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_ucsViewType, (void*)&pNew->m_ucsViewType, sizeof(pOld->m_ucsViewType));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_pBlock, (void*)&pNew->m_pBlock, sizeof(pOld->m_pBlock));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_pLastViewport, (void*)&pNew->m_pLastViewport, sizeof(pOld->m_pLastViewport));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_pUCS, (void*)&pNew->m_pUCS, sizeof(pOld->m_pUCS));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_pBaseUCS, (void*)&pNew->m_pBaseUCS, sizeof(pOld->m_pBaseUCS));
				// Here is no break, stack below CDbPlotSettings member.
			}
		case DB_PLOTSETTINGS:
			{
				CDbPlotSettings	*pNew = (CDbPlotSettings*)newelp,
								*pOld = (CDbPlotSettings*)oldelp;

				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_deleted, (void*)&pNew->m_deleted, sizeof(pOld->m_deleted));
				// NOTE: These 4 properties are not alloc string. so
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotConfig, (void*)&pNew->m_plotConfig, sizeof(pOld->m_plotConfig));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_paperSize, (void*)&pNew->m_paperSize, sizeof(pOld->m_paperSize));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotView, (void*)&pNew->m_plotView, sizeof(pOld->m_plotView));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_curStyleSheet, (void*)&pNew->m_curStyleSheet, sizeof(pOld->m_curStyleSheet));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_leftBottomMargin[0], (void*)&pNew->m_leftBottomMargin[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_leftBottomMargin[1], (void*)&pNew->m_leftBottomMargin[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_rightTopMargin[0], (void*)&pNew->m_rightTopMargin[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_rightTopMargin[1], (void*)&pNew->m_rightTopMargin[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotPaperSize[0], (void*)&pNew->m_plotPaperSize[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotPaperSize[1], (void*)&pNew->m_plotPaperSize[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotOrigin[0], (void*)&pNew->m_plotOrigin[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotOrigin[1], (void*)&pNew->m_plotOrigin[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_winMin[0], (void*)&pNew->m_winMin[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_winMin[1], (void*)&pNew->m_winMin[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_winMax[0], (void*)&pNew->m_winMax[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_winMax[1], (void*)&pNew->m_winMax[1], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_customScaleNumerator, (void*)&pNew->m_customScaleNumerator, sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_customScaleDenominator, (void*)&pNew->m_customScaleDenominator, sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotLayoutFlag, (void*)&pNew->m_plotLayoutFlag, sizeof(short));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotPaperUnits, (void*)&pNew->m_plotPaperUnits, sizeof(pOld->m_plotPaperUnits));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotRotation, (void*)&pNew->m_plotRotation, sizeof(pOld->m_plotRotation));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_plotType, (void*)&pNew->m_plotType, sizeof(pOld->m_plotType));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_scaleType, (void*)&pNew->m_scaleType, sizeof(pOld->m_scaleType));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_scaleFactor, (void*)&pNew->m_scaleFactor, sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_paperImageOrigin[0], (void*)&pNew->m_paperImageOrigin[0], sizeof(double));
				db_BinChange::CacheChangedData(&begll, &curll, (void*)&pOld->m_paperImageOrigin[1], (void*)&pNew->m_paperImageOrigin[1], sizeof(double));
				break;
			}
		// EBATECH(CNBR) ]-
		default:
			#if defined(DEBUG)
				sds_printf(ResourceString(IDC_ICADUNDOREDO__NINTERNAL_13, "\nINTERNAL WARNING: Unknown Entity passed to db_CompEntLinks()." ));				// Debug only, no cmd_ErrorPrompt needed
			#endif
			goto out;
	}

	*chglst=begll;

	return(RTNORM);

	out: ;

	return(RTERROR);
}



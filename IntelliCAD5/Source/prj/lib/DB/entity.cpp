/* C:\ICAD\PRJ\LIB\DB\ENTITY.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 *
 * <TODO : Add Abstract here>
 *
 */

/**************************** db_entity *****************************/

#include "db.h"
#include "layertabrec.h"

/*F*/
db_entity::db_entity(void) {
    ltypehip=layerhip=NULL;
    ltscale=1.0; color=DB_BLCOLOR; eflags=0;
	lineweight=DB_BLWEIGHT;
	plotstyletype=DB_BL_PLSTYLE;
	plotstylehip=NULL;
}
db_entity::db_entity(int ptype, db_drawing *dp) : db_handitem(ptype) {
    int tmode,cvport;
    char *bufp;

    ltscale=1.0; color=DB_BLCOLOR;
    if (dp==NULL) {
        ltypehip=layerhip=NULL; eflags=0;
		lineweight=DB_BLWEIGHT;
		plotstyletype=DB_BL_PLSTYLE;
		plotstylehip=NULL;
    } else {
        ltypehip=dp->ret_currenttabrec(DB_QCELTYPE);
        layerhip=dp->ret_currenttabrec(DB_QCLAYER);
        bufp=dp->ret_headerbuf();
        db_qgetvar(DB_QCELTSCALE,bufp,&ltscale,DB_DOUBLE,0);
        db_qgetvar(DB_QCECOLOR,bufp,&color  ,DB_INT   ,0);

        db_qgetvar(DB_QTILEMODE,bufp,&tmode ,DB_INT   ,0);
        db_qgetvar(DB_QCVPORT  ,bufp,&cvport,DB_INT   ,0);
        eflags=(!tmode && cvport==1) ? DB_PSPACE : 0;
        db_qgetvar(DB_QCELWEIGHT,bufp,&lineweight,DB_INT   ,0);
		db_current_plotstyle( dp, &plotstyletype, &plotstylehip );
    }
    if (ptype==DB_VIEWPORT) eflags=DB_PSPACE;
}

/*F*/
//int db_entity::entgetcommon(struct resbuf **begpp, struct resbuf **endpp)
int db_entity::entgetcommon(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp )
{
/*
**  Builds the part of the entget llist for the group codes
**  common to all entities.
**
**  Returns:
**       0 : OK
**      -1 : No memory
*/
    short fsh1;
    char *fcp1;
    int rc;
    struct resbuf *sublist[2];
	int pstylemode;
	sds_name en;

    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 67 (pspace) */
    fsh1=((eflags&'\x02')!=0);
    if ((sublist[0]=sublist[1]=db_newrb(67,'H',&fsh1))==NULL) { rc=-1; goto out; }

    /* 8 (layer) */
    get_8(&fcp1);
    if ((sublist[1]->rbnext=db_newrb(8,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 62 (color) */
    fsh1=(short)color;
    if ((sublist[1]->rbnext=db_newrb(62,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 6 (linetype) */
    get_6(&fcp1);
    if ((sublist[1]->rbnext=db_newrb(6,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	/* 370 (lineweight) */
    fsh1=(short)lineweight;
    if ((sublist[1]->rbnext=db_newrb(370,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	if( db_qgetvar(DB_QPSTYLEMODE,dp->ret_headerbuf(),&pstylemode,DB_INT   ,0) != 0 )
	{ rc=-2; goto out; }

	if(!pstylemode)
	{
	/* 380 (plotstyle type ) */
		fsh1=(short)plotstyletype;
		if ((sublist[1]->rbnext=db_newrb(380,'H',&fsh1))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

	/* 390 (plotstyle pointer ) */
		if(plotstyletype==DB_NM_PLSTYLE)
		{
			en[0]=en[1]=0;
			if(plotstylehip!=NULL)
			{
				en[0]=(long)plotstylehip; en[1]=(long)dp;
			}
			if((sublist[1]->rbnext=db_newrb(390,'N',en))==NULL) { rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
		}
	}

    /* 48 (linetype scale) */
    if ((sublist[1]->rbnext=db_newrb(48,'R',&ltscale))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 60 (invisibility) */
    fsh1=((eflags&'\x01')!=0);
    if ((sublist[1]->rbnext=db_newrb(60,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

/*F*/
int db_entity::entmod1common(struct resbuf **rblinkpp, db_drawing *dp) {
/*
**  Returns: See db_handitem::entmod().
*/
    int rc;
    db_handitem *thiship;

    rc=0; thiship=(db_handitem *)this;

    if (rblinkpp==NULL || *rblinkpp==NULL || dp==NULL) { rc=-1; goto out; }

    /* Call the correct setter: */
    switch ((*rblinkpp)->restype) {
        case  6: thiship->set_6( (*rblinkpp)->resval.rstring,dp); break;
        case  8: thiship->set_8( (*rblinkpp)->resval.rstring,dp); break;
        case 48: thiship->set_48((*rblinkpp)->resval.rreal     ); break;
        case 60: thiship->set_60((*rblinkpp)->resval.rint      ); break;
        case 62: thiship->set_62((*rblinkpp)->resval.rint      ); break;
        case 67: thiship->set_67((*rblinkpp)->resval.rint      ); break;
        case 90: thiship->set_90((*rblinkpp)->resval.rlong      ); break;
		case 370:	thiship->set_370((*rblinkpp)->resval.rint	   ); break;
		case 380:	thiship->set_380((*rblinkpp)->resval.rint	   ); break;
		case 390:	thiship->set_390((db_handitem *)((*rblinkpp)->resval.rlname[0])); break;
        default: entmodhi(rblinkpp,dp);                           break;
            /* Catches persistent reactors and extension dictionary. */
    }

out:
    return rc;
}


bool
db_entity:: set_8(char *p, db_drawing *dp)
{
	db_handitem *thip1=NULL;

	if (p==NULL || !*p || dp==NULL)
		{ db_lasterror=OL_ESNVALID; return false; }

	/* Special case: if it doesn't exist, MAKE it (with default settings): */

	/*D.G.*/// I find it strange: creating layers from entities. Ok, if it is necessary for
	// some APIs (at least for sds_entmake & sds_entmod), layers should be created on more
	// higher level of processing these APIs. But, if we just create some temporary entity
	// (for example, in snapping to xrefs (sdsengine_osnap>SDS_xforment>...>set_8 call)),
	// who will remove these new layers?!
	// So, it's for "TO DO".

	if ((thip1=dp->findtabrec(DB_LAYERTAB,p,1))==NULL) {
		dp->addhanditem(new db_layertabrec(p,dp));
	    if ((thip1=dp->findtabrec(DB_LAYERTAB,p,1))==NULL)
		    { db_lasterror=OL_ESNVALID; return false; }
	}

	layerhip=thip1;

/*
**  In the case of INSERTs with ATTRIBs, to follow ACAD, we DON'T
**  set it for the ATTRIBS, but we DO for the SEQEND.
*/
	thip1=next;
	if (this->ret_type()==DB_INSERT) {
		while (thip1!=NULL && thip1->ret_type()==DB_ATTRIB) thip1=thip1->next;
		if (thip1!=NULL && thip1->ret_type()==DB_SEQEND)
			thip1->set_layerhip(layerhip);
	}

	return true;
}

void db_entity::set_deleted(int p)
{
	if (p)
	{
		eflags|=DB_DELETED;

		// call break assoc in case this is a hatch, leader ...
		BreakAssociation(true);

		if (m_reactors)
		{
			m_reactors->BreakAssociation(this, true);
			//erase reactors list in ReactorSubject::BreakAssociation()
			//m_reactors->RemoveAll();
		}
	}
	else
		eflags&=~DB_DELETED;
}

bool db_entity::addReactor(db_objhandle &handle, db_drawing *drawing, ReactorItem::Type type)
{
	void **fixuploc = NULL;
	bool result =  m_reactors->AttachReactor(handle, &fixuploc, type);

	if (result == true)
		drawing->AddHandleToFixupTable(handle,fixuploc);

	return result;
}



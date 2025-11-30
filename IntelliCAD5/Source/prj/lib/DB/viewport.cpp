/* C:\ICAD\PRJ\LIB\DB\VIEWPORT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


#include "db.h"
#include "viewport.h"
#include <assert.h>

static char *db_thisfile=__FILE__;

extern int g_nClosingDrawings;

#define TRACEMSGS	0
#if TRACEMSGS
	#define DEBUGMSG( s, p )   { char t[256]; ::sprintf( t, (s), p );	::OutputDebugString( t ); }
#else
	#define DEBUGMSG( s, p )
#endif

/************************** db_viewport *****************************/

/*F*/
db_viewport::db_viewport(void) : db_entity(DB_VIEWPORT) {

    cent[0]=cent[1]=cent[2]=0.0; wd=ht=1.0; stat=id=flags=0;
    disp=NULL; memset(extent,0,sizeof(extent));
	clipbound=NULL;
	statusflags=32800L;
	rendermode=0;			// 281
	stylesheetname[0]=0;	// 1
	ucsvp=0;				// 71
	ucsiconatucsorigin=1;	// 74
	ucsorg[0]=ucsorg[1]=ucsorg[2]=0.0;
	ucsxdir[0]=ucsydir[1]=1.0;
	ucsxdir[1]=ucsxdir[2]=ucsydir[0]=ucsydir[2]=0.0;
	ucsorthoviewtype=0;	// 79
	ucselevation=0.0;	// 146
	ucs=baseucs=NULL;
	center[0]=center[1]=center[2]=target[0]=target[1]=target[2]=dir[0]=dir[1]=0.0;
	dir[2]=1.0;
	twist=0.0;
	size=0.0;
	lenslength=50.0;
	frontclip=backclip=0.0;
	circlezoom=100;
	snapunit[0]=snapunit[1]=0.5;
	gridunit[0]=gridunit[1]=0.5;
	snapunit[2]=gridunit[2]=0.0;
	snapangle=0.0;
	snapbase[0]=snapbase[1]=snapbase[2]=0.0;

	m_pVpLayerData = new VpLayerData();
}

/*F*/
db_viewport::db_viewport(int numfrozitems) : db_entity(DB_VIEWPORT) {

    cent[0]=cent[1]=cent[2]=0.0; wd=ht=1.0; stat=id=flags=0;
    disp=NULL; memset(extent,0,sizeof(extent));
	clipbound=NULL;
	statusflags=32800L;
	rendermode=0;			// 281
	stylesheetname[0]=0;	// 1
	ucsvp=0;				// 71
	ucsiconatucsorigin=1;	// 74
	ucsorg[0]=ucsorg[1]=ucsorg[2]=0.0;
	ucsxdir[0]=ucsydir[1]=1.0;
	ucsxdir[1]=ucsxdir[2]=ucsydir[0]=ucsydir[2]=0.0;
	ucsorthoviewtype=0;	// 79
	ucselevation=0.0;	// 146
	ucs=baseucs=NULL;
	center[0]=center[1]=center[2]=target[0]=target[1]=target[2]=dir[0]=dir[1]=0.0;
	dir[2]=1.0;
	twist=0.0;
	size=0.0;
	lenslength=50.0;
	frontclip=backclip=0.0;
	circlezoom=100;
	snapunit[0]=snapunit[1]=0.5;
	gridunit[0]=gridunit[1]=0.5;
	snapunit[2]=gridunit[2]=0.0;
	snapangle=0.0;
	snapbase[0]=snapbase[1]=snapbase[2]=0.0;

	m_pVpLayerData = new VpLayerData(numfrozitems);
}

db_viewport::db_viewport(db_drawing *dp) : db_entity(DB_VIEWPORT,dp) {

    cent[0]=cent[1]=cent[2]=0.0; wd=ht=1.0; stat=id=flags=0;
    disp=NULL; memset(extent,0,sizeof(extent));
	clipbound=NULL;
	statusflags=32800L;
	rendermode=0;			// 281
	stylesheetname[0]=0;	// 1
	ucsvp=0;				// 71
	ucsiconatucsorigin=1;	// 74
	ucsorg[0]=ucsorg[1]=ucsorg[2]=0.0;
	ucsxdir[0]=ucsydir[1]=1.0;
	ucsxdir[1]=ucsxdir[2]=ucsydir[0]=ucsydir[2]=0.0;
	ucsorthoviewtype=0;	// 79
	ucselevation=0.0;	// 146
	ucs=baseucs=NULL;
	center[0]=center[1]=center[2]=target[0]=target[1]=target[2]=dir[0]=dir[1]=0.0;
	dir[2]=1.0;
	twist=0.0;
	size=0.0;
	lenslength=50.0;
	frontclip=backclip=0.0;
	circlezoom=100;
	snapunit[0]=snapunit[1]=0.5;
	gridunit[0]=gridunit[1]=0.5;
	snapunit[2]=gridunit[2]=0.0;
	snapangle=0.0;
	snapbase[0]=snapbase[1]=snapbase[2]=0.0;

	m_pVpLayerData = new VpLayerData();

}

db_viewport::db_viewport(const db_viewport &sour) :
    db_entity(sour) {  /* Copy constructor */

    DB_PTCPY(cent,sour.cent);

    wd=sour.wd; ht=sour.ht; stat=sour.stat; id=sour.id; flags=sour.flags;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
	clipbound=sour.clipbound;
	statusflags=sour.statusflags;
	rendermode=sour.rendermode;			// 281
	strcpy(stylesheetname,sour.stylesheetname);	// 1
	ucsvp=sour.ucsvp;				// 71
	ucsiconatucsorigin=sour.ucsiconatucsorigin;	// 74
	ic_ptcpy(ucsorg,sour.ucsorg);
	ic_ptcpy(ucsxdir,sour.ucsxdir);
	ic_ptcpy(ucsydir,sour.ucsydir);
	ucsorthoviewtype=sour.ucsorthoviewtype;	// 79
	ucselevation=sour.ucselevation;
	ucs=sour.ucs;
	baseucs=sour.baseucs;
	ic_ptcpy(center,sour.center);
	ic_ptcpy(target,sour.target);
	ic_ptcpy(dir,sour.dir);
	twist=sour.twist;
	size=sour.size;
	lenslength=sour.lenslength;
	frontclip=sour.frontclip;
	backclip=sour.backclip;
	circlezoom=sour.circlezoom;
	ic_ptcpy(snapunit,sour.snapunit);
	ic_ptcpy(gridunit,sour.gridunit);
	snapangle=sour.snapangle;
	ic_ptcpy(snapbase,sour.snapbase);

    disp=NULL;  /* Don't copy display objects. */

	m_pVpLayerData = new VpLayerData();

}
db_viewport::~db_viewport(void)
	{

    if (db_grafreedofp!=NULL)
		{
		db_grafreedofp(disp);
		}

	if ( m_pVpLayerData != NULL )
		{
		delete m_pVpLayerData;
		m_pVpLayerData = NULL;
		}
	db_handitem * pClipbound;
	pClipbound = *(this->retp_clipbound());

	if (0 == g_nClosingDrawings && pClipbound)
		pClipbound->removeReactor(this->RetHandle());

	}

bool
db_viewport::set_340(db_handitem *theclipbound)
	{
	if (clipbound)
		BreakAssociation();
	clipbound = theclipbound;
	return true;
	}

	// when creating by drawing (instead of load) need to add association
bool
db_viewport::set_340andAssociate(db_handitem *theclipbound)
	{
	if (set_340(theclipbound))
		{
		//Add assocation from hip back to hatch
		clipbound->addReactor(this->RetHandle(), this, ReactorItem::HARD_POINTER);
		return true;
		}
	return false;
	}

/*F*/
int db_viewport::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    short fsh1;
    int rc;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 10 (cent) */
    if ((sublist[0]=sublist[1]=db_newrb(10,'P',cent))==NULL) { rc=-1; goto out; }

    /* 40 (wd) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&wd))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 (ht) */
    if ((sublist[1]->rbnext=db_newrb(41,'R',&ht))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 68 (stat) */
    fsh1=(short)stat;
    if ((sublist[1]->rbnext=db_newrb(68,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 69 (id) */
    fsh1=(short)id;
    if ((sublist[1]->rbnext=db_newrb(69,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// EBAETCH(CNBR) -[ 2002/6/19 flags(70) is never used.
    ///* 70 (flags) */
    //fsh1=(short)flags;
    //if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    //sublist[1]=sublist[1]->rbnext;
    // EBAETCH(CNBR) ]-

    // 12 (view center)
	if ((sublist[1]->rbnext=db_newrb(12,'P', center))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 13 (snap base)
	if ((sublist[1]->rbnext=db_newrb(13,'P', snapbase))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 14 (snap spacing)
	if ((sublist[1]->rbnext=db_newrb(14,'P', snapunit))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 15 (grid spacing)
	if ((sublist[1]->rbnext=db_newrb(15,'P', gridunit))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 16 (view direction)
	if ((sublist[1]->rbnext=db_newrb(16,'P', dir))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 17 (view target)
	if ((sublist[1]->rbnext=db_newrb(17,'P', target))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 42 (lenslength)
	if ((sublist[1]->rbnext=db_newrb(42,'R', &lenslength))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 43 (frontclip)
	if ((sublist[1]->rbnext=db_newrb(43,'R', &frontclip))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 44 (backclip)
	if ((sublist[1]->rbnext=db_newrb(44,'R', &backclip))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 45 (height)
	if ((sublist[1]->rbnext=db_newrb(45,'R', &size))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 50 (snapangle)
	if ((sublist[1]->rbnext=db_newrb(50,'R', &snapangle))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 51 (twist)
	if ((sublist[1]->rbnext=db_newrb(51,'R', &twist))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 72 (circlezoom)
	if ((sublist[1]->rbnext=db_newrb(72,'H', &circlezoom))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // EBATECH(CNBR) -[ 2002/6/19 Bugzilla #78208 wrong elist
	// 341(frozenlayers)
	if( m_pVpLayerData ){
		int i, n;
		sds_name name;

		n = m_pVpLayerData->GetFrozenLayerCount();
		name[1] = (long)dp;
		for( i = 0 ; i < n ; i++ ) {
			name[0] = (long)(m_pVpLayerData->GetFrozenLayer(i));
			if ((sublist[1]->rbnext=db_newrb(341,'N', name))==NULL) { rc=-1; goto out; }
    		sublist[1]=sublist[1]->rbnext;
		}
	}
	// EBATECH(CNBR) ]-

    // 90 (statusflags)
	if ((sublist[1]->rbnext=db_newrb(90,'L', &statusflags))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 281 (rendermode)
    // EBATECH(CNBR) -[ 2002/6/19 Bugzilla #78208 wrong elist
	if ((sublist[1]->rbnext=db_newrb(281,'H', &rendermode))==NULL) { rc=-1; goto out; }
	//if ((sublist[1]->rbnext=db_newrb(90,'H', &rendermode))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 71 (ucsvp)
	if ((sublist[1]->rbnext=db_newrb(71,'H', &ucsvp))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 74 (ucsiconatucsorigin)
	if ((sublist[1]->rbnext=db_newrb(74,'H', &ucsiconatucsorigin))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 110 (ucsorg)
	if ((sublist[1]->rbnext=db_newrb(110,'P', ucsorg))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 111 (ucsxdir)
	if ((sublist[1]->rbnext=db_newrb(111,'P', ucsxdir))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 112 (ucsydir)
	if ((sublist[1]->rbnext=db_newrb(112,'P', ucsydir))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // EBATECH(CNBR) -[ 2002/6/19 Bugzilla #78208 wrong elist
    // 345 (ucs)
    if( ucs && ucsorthoviewtype == 0 ) {
		sds_name name;
		name[0] = (long)ucs; name[1] = (long)dp;
		if ((sublist[1]->rbnext=db_newrb(345,'N', name))==NULL) { rc=-1; goto out; }
	    sublist[1]=sublist[1]->rbnext;
	}
    // 346 (baseuc)
    if( baseucs && ucsorthoviewtype > 0) {
		sds_name name;
		name[0] = (long)baseucs; name[1] = (long)dp;
		if ((sublist[1]->rbnext=db_newrb(346,'N', name))==NULL) { rc=-1; goto out; }
	    sublist[1]=sublist[1]->rbnext;
	}
	// EBATECH(CNBR) ]-

    // 79 (ucsorthoviewtype)
	if ((sublist[1]->rbnext=db_newrb(79,'H', &ucsorthoviewtype))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    // 146 (ucselevation)
	if ((sublist[1]->rbnext=db_newrb(146,'R', &ucselevation))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

// EBATECH(CNBR) -[ 2002/6/19 Bugzilla #78208 Viewport can't set frozen layers via entity list.
int
db_viewport::entmod(struct resbuf *modll, db_drawing *dp)
{
/*
**  Entity/table-record specific.  Uses the setters for range-correction.
**
**  There's some order-dependency here; each 49 signals the beginning
**  of a new set of dash data (and signals the end of the previous set).
**
**  Returns: See db_handitem::entmod().
*/
	int result = 0;
    struct resbuf *pInitialListItem;
	struct resbuf *pCurrentListItem;
    db_lasterror = 0;

    if(modll == NULL || dp == NULL)
	{
		result = -1;
		return result;
	}
    del_xdict();
    pInitialListItem = modll;
    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (pInitialListItem != NULL &&
        (pInitialListItem->restype == -1 ||
         pInitialListItem->restype == 0 ||
         pInitialListItem->restype == 5))
		 pInitialListItem = pInitialListItem->rbnext;

    // Initialize frozen layer records
	if( m_pVpLayerData ) {
		delete m_pVpLayerData;
		m_pVpLayerData = NULL;
	}
	m_pVpLayerData = new VpLayerData;

    // walk until the end or the EED sentinel (-3):
    for (pCurrentListItem = pInitialListItem; pCurrentListItem != NULL && pCurrentListItem->restype != -3; pCurrentListItem = pCurrentListItem->rbnext)
	{
        switch (pCurrentListItem->restype)
		{
			case    10:	set_10( pCurrentListItem->resval.rpoint );	break;
			case    40:	set_40( pCurrentListItem->resval.rreal );	break;
			case    41:	set_41( pCurrentListItem->resval.rreal );	break;
			case    68:	set_68( pCurrentListItem->resval.rint );	break;
			case    69:	set_69( pCurrentListItem->resval.rint );	break;
			case    12:	set_12( pCurrentListItem->resval.rpoint );	break;
			case    13:	set_13( pCurrentListItem->resval.rpoint );	break;
			case    14:	set_14( pCurrentListItem->resval.rpoint );	break;
			case    15:	set_15( pCurrentListItem->resval.rpoint );	break;
			case    16:	set_16( pCurrentListItem->resval.rpoint );	break;
			case    17:	set_17( pCurrentListItem->resval.rpoint );	break;
			case    42:	set_42( pCurrentListItem->resval.rreal );	break;
			case    43:	set_43( pCurrentListItem->resval.rreal );	break;
			case    44:	set_44( pCurrentListItem->resval.rreal );	break;
			case    45:	set_45( pCurrentListItem->resval.rreal );	break;
			case    50:	set_50( pCurrentListItem->resval.rreal );	break;
			case    51:	set_51( pCurrentListItem->resval.rreal );	break;
			case    72:	set_72( pCurrentListItem->resval.rint );	break;

            case   341: // Frozen layer(s)
				AddLayerToVpLayerData( (db_layertabrec*)(pCurrentListItem->resval.rlname[0]));
				break;

			case   340:	set_340((db_handitem*)(pCurrentListItem->resval.rlname[0]));	break;
			case     1:	set_1( pCurrentListItem->resval.rstring );	break;
			case   281:	set_281( pCurrentListItem->resval.rint );	break;
			case    71:	set_71( pCurrentListItem->resval.rint );	break;
			case    74:	set_74( pCurrentListItem->resval.rint );	break;
			case   110:	set_110( pCurrentListItem->resval.rpoint );	break;
			case   111:	set_111( pCurrentListItem->resval.rpoint );	break;
			case   112:	set_112( pCurrentListItem->resval.rpoint );	break;
			case   345:	set_345((db_handitem*)(pCurrentListItem->resval.rlname[0]));	break;
			case   346:	set_346((db_handitem*)(pCurrentListItem->resval.rlname[0]));	break;
			case    79:	set_79( pCurrentListItem->resval.rint );	break;
			case   146:	set_146( pCurrentListItem->resval.rreal );	break;

            default:
				// the common entity or table record groups
                result = entmod1common(&pCurrentListItem, dp);
				if(result)
					return result;
        }
    }
    if(db_lasterror)
		result = -6;
    // do the EED
    if(!result && pInitialListItem != NULL)
		result = set_eed(pInitialListItem, dp);
    return result;
}
// EBATECH(CNBR) ]-

bool
db_viewport::CreateNewViewInfo( db_drawing *dp  )
	{
// like its predecessor, this will not undo
	struct resbuf rb;

	db_qgetvar(DB_QTARGET ,dp->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	ic_ptcpy(target,rb.resval.rpoint);

	db_qgetvar(DB_QVIEWDIR ,dp->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	ic_ptcpy(dir,rb.resval.rpoint);

	db_getvar(NULL,DB_QVIEWTWIST,&rb,dp,NULL,NULL);
	twist=rb.resval.rreal;

	db_getvar(NULL,DB_QVIEWSIZE,&rb,dp,NULL,NULL);
	size=rb.resval.rreal;

	db_qgetvar(DB_QVIEWCTR ,dp->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	center[0]=rb.resval.rpoint[0];
	center[1]=rb.resval.rpoint[1];

	db_getvar(NULL,DB_QLENSLENGTH,&rb,dp,NULL,NULL);
	lenslength=rb.resval.rreal;
	db_getvar(NULL,DB_QFRONTZ,&rb,dp,NULL,NULL);
	frontclip=rb.resval.rreal;

	db_getvar(NULL,DB_QBACKZ,&rb,dp,NULL,NULL);
	backclip=rb.resval.rreal;

	db_getvar(NULL,DB_QVIEWMODE,&rb,dp,NULL,NULL);
	statusflags=0L;
	if (rb.resval.rint & 1)
		statusflags |= IC_VPENT_STATUS_PERSPECTIVEMODE;
	if (rb.resval.rint & 2)
		statusflags |= IC_VPENT_STATUS_FRONTCLIPON;
	if (rb.resval.rint & 4)
		statusflags |= IC_VPENT_STATUS_BACKCLIPON;
	if (rb.resval.rint & 8)
		statusflags |= IC_VPENT_STATUS_UCSFOLLOW;
	if (rb.resval.rint & 16)
		statusflags |= IC_VPENT_STATUS_FRONTCLIPNOTATEYE;

	db_getvar(NULL,DB_QZOOMPERCENT,&rb,dp,NULL,NULL);
	circlezoom=rb.resval.rint;

	db_getvar(NULL,DB_QFASTZOOM,&rb,dp,NULL,NULL);
	if (rb.resval.rint)
		statusflags |= IC_VPENT_STATUS_FASTZOOM;

	db_getvar(NULL,DB_QUCSICON,&rb,dp,NULL,NULL);
	if (rb.resval.rint)
		statusflags |= IC_VPENT_STATUS_UCSICON;

	db_getvar(NULL,DB_QSNAPMODE,&rb,dp,NULL,NULL);
	if (rb.resval.rint)
		statusflags |= IC_VPENT_STATUS_SNAPON;

	db_getvar(NULL,DB_QGRIDMODE,&rb,dp,NULL,NULL);
	if (rb.resval.rint)
		statusflags |= IC_VPENT_STATUS_GRIDON;

	db_getvar(NULL,DB_QSNAPSTYL,&rb,dp,NULL,NULL);
	if (rb.resval.rint)
		statusflags |= IC_VPENT_STATUS_SNAPSTYLEISO;

	db_getvar(NULL,DB_QSNAPISOPAIR,&rb,dp,NULL,NULL);
	if (rb.resval.rint==1)
		statusflags |= IC_VPENT_STATUS_SNAPISOPAIRTOP;
	else if (rb.resval.rint==2)
		statusflags |= IC_VPENT_STATUS_SNAPISOPAIRRIGHT;

	db_getvar(NULL,DB_QSNAPANG,&rb,dp,NULL,NULL);
	snapangle=rb.resval.rreal;

	db_getvar(NULL,DB_QSNAPBASE,&rb,dp,NULL,NULL);
	snapbase[0]=rb.resval.rpoint[0];
	snapbase[1]=rb.resval.rpoint[1];

	db_getvar(NULL,DB_QSNAPUNIT,&rb,dp,NULL,NULL);
	snapunit[0]=rb.resval.rpoint[0];
	snapunit[1]=rb.resval.rpoint[1];

	db_getvar(NULL,DB_QGRIDUNIT,&rb,dp,NULL,NULL);
	gridunit[0]=rb.resval.rpoint[0];
	gridunit[1]=rb.resval.rpoint[1];

	return true;
	}


// We're going to try just modifying the existing extended entity data for
// the viewport
//
DB_API
bool
db_viewport::SetViewInfoToDefault( db_drawing *dp )
	{

// like its predecessor, this probably will not undo
	struct resbuf rbAcad;
	// get MVIEW info for the viewport
	rbAcad.restype=RTSTR;
	rbAcad.resval.rstring="ACAD";
	rbAcad.rbnext=NULL;

	struct resbuf *prbExtendedData = this->get_eed(&rbAcad,NULL);

	// If no eed yet, we create new
	//
	if ( prbExtendedData == NULL )
		{
		return this->CreateNewViewInfo( dp );
		}

	// Otherwise we try to edit what's already there
	//

	struct resbuf rbFetch;

	db_qgetvar(DB_QTARGET ,dp->ret_headerbuf(),rbFetch.resval.rpoint ,DB_3DPOINT,0);
	ic_ptcpy(target,rbFetch.resval.rpoint);

	db_qgetvar(DB_QVIEWDIR ,dp->ret_headerbuf(),rbFetch.resval.rpoint ,DB_3DPOINT,0);
	ic_ptcpy(dir,rbFetch.resval.rpoint);

	db_getvar(NULL,DB_QVIEWTWIST,&rbFetch,dp,NULL,NULL);
	twist=rbFetch.resval.rreal;

	db_getvar(NULL,DB_QVIEWSIZE,&rbFetch,dp,NULL,NULL);
	size=rbFetch.resval.rreal;

	db_qgetvar(DB_QVIEWCTR ,dp->ret_headerbuf(),rbFetch.resval.rpoint ,DB_3DPOINT,0);
	center[0]=rbFetch.resval.rpoint[0];
	center[1]=rbFetch.resval.rpoint[1];


	db_getvar(NULL,DB_QLENSLENGTH,&rbFetch,dp,NULL,NULL);
	lenslength=rbFetch.resval.rreal;


	db_getvar(NULL,DB_QFRONTZ,&rbFetch,dp,NULL,NULL);
	frontclip=rbFetch.resval.rreal;

	db_getvar(NULL,DB_QBACKZ,&rbFetch,dp,NULL,NULL);
	backclip=rbFetch.resval.rreal;

	statusflags=0L;
	db_getvar(NULL,DB_QVIEWMODE,&rbFetch,dp,NULL,NULL);
	if (rbFetch.resval.rint & 1)
		statusflags |= IC_VPENT_STATUS_PERSPECTIVEMODE;
	if (rbFetch.resval.rint & 2)
		statusflags |= IC_VPENT_STATUS_FRONTCLIPON;
	if (rbFetch.resval.rint & 4)
		statusflags |= IC_VPENT_STATUS_BACKCLIPON;
	if (rbFetch.resval.rint & 8)
		statusflags |= IC_VPENT_STATUS_UCSFOLLOW;
	if (rbFetch.resval.rint & 16)
		statusflags |= IC_VPENT_STATUS_FRONTCLIPNOTATEYE;

	db_getvar(NULL,DB_QZOOMPERCENT,&rbFetch,dp,NULL,NULL);
	circlezoom=rbFetch.resval.rint;

	db_getvar(NULL,DB_QFASTZOOM,&rbFetch,dp,NULL,NULL);
	if (rbFetch.resval.rint)
		statusflags |= IC_VPENT_STATUS_FASTZOOM;

	db_getvar(NULL,DB_QUCSICON,&rbFetch,dp,NULL,NULL);
	if (rbFetch.resval.rint)
		statusflags |= IC_VPENT_STATUS_UCSICON;

	db_getvar(NULL,DB_QSNAPMODE,&rbFetch,dp,NULL,NULL);
	if (rbFetch.resval.rint)
		statusflags |= IC_VPENT_STATUS_SNAPON;

	db_getvar(NULL,DB_QGRIDMODE,&rbFetch,dp,NULL,NULL);
	if (rbFetch.resval.rint)
		statusflags |= IC_VPENT_STATUS_GRIDON;

	db_getvar(NULL,DB_QSNAPSTYL,&rbFetch,dp,NULL,NULL);
	if (rbFetch.resval.rint)
		statusflags |= IC_VPENT_STATUS_SNAPSTYLEISO;

	db_getvar(NULL,DB_QSNAPISOPAIR,&rbFetch,dp,NULL,NULL);
	if (rbFetch.resval.rint==1)
		statusflags |= IC_VPENT_STATUS_SNAPISOPAIRTOP;
	else if (rbFetch.resval.rint==2)
		statusflags |= IC_VPENT_STATUS_SNAPISOPAIRRIGHT;

	db_getvar(NULL,DB_QSNAPANG,&rbFetch,dp,NULL,NULL);
	snapangle=rbFetch.resval.rreal;

	db_getvar(NULL,DB_QSNAPBASE,&rbFetch,dp,NULL,NULL);
	snapbase[0]=rbFetch.resval.rpoint[0];
	snapbase[1]=rbFetch.resval.rpoint[1];

	db_getvar(NULL,DB_QSNAPUNIT,&rbFetch,dp,NULL,NULL);
	snapunit[0]=rbFetch.resval.rpoint[0];
	snapunit[1]=rbFetch.resval.rpoint[1];

	db_getvar(NULL,DB_QGRIDUNIT,&rbFetch,dp,NULL,NULL);
	gridunit[0]=rbFetch.resval.rpoint[0];
	gridunit[1]=rbFetch.resval.rpoint[1];

	return true;
	}

// *****************************************
// PUBLIC METHOD -- class db_viewport
//
// SetView
// used by sds_setview--same interface
//
//
DB_API
bool
db_viewport::SetView( db_drawing *dp, const struct resbuf *prbViewData )
	{
// again, won't work with undo
	ASSERT( CHECKSTRUCTPTR( prbViewData ) );
	if ( prbViewData == NULL )
		{
		return false;
		}

	sds_point ttarget;
	sds_real tviewsize;
	sds_point tviewdir;
	sds_point tcenter;
	sds_real tlenslength;
	sds_real tfrontz;
	sds_real tbackz;
	int tvmode;
	sds_real tvtwist;

		// Get the defaults from the header values
		{

		struct resbuf rbFetch;
		db_qgetvar(DB_QTARGET ,dp->ret_headerbuf(),ttarget ,DB_3DPOINT,0);
		db_qgetvar(DB_QVIEWDIR ,dp->ret_headerbuf(),tviewdir ,DB_3DPOINT,0);
		db_getvar(NULL,DB_QVIEWTWIST,&rbFetch,dp,NULL,NULL);
		tvtwist = rbFetch.resval.rreal;
		db_getvar(NULL,DB_QVIEWSIZE,&rbFetch,dp,NULL,NULL);
		tviewsize = rbFetch.resval.rreal;
		db_qgetvar(DB_QVIEWCTR ,dp->ret_headerbuf(),tcenter ,DB_3DPOINT,0);
		db_getvar(NULL,DB_QFRONTZ,&rbFetch,dp,NULL,NULL);
		tfrontz = rbFetch.resval.rreal;
		db_getvar(NULL,DB_QBACKZ,&rbFetch,dp,NULL,NULL);
		tbackz = rbFetch.resval.rreal;
		db_getvar(NULL,DB_QVIEWMODE,&rbFetch,dp,NULL,NULL);
		tvmode = rbFetch.resval.rint;
		db_getvar(NULL,DB_QLENSLENGTH,&rbFetch,dp,NULL,NULL);
		tlenslength = rbFetch.resval.rreal;

		}

	// Then override with anything found in prbViewData passed in
	//
	const struct resbuf *prbViewIterator;
	for(prbViewIterator = prbViewData; prbViewIterator!=NULL; prbViewIterator=prbViewIterator->rbnext) {
		switch(prbViewIterator->restype) {
			case 10:
				ic_ptcpy(center,prbViewIterator->resval.rpoint);
				break;
			case 11:
				if(icadRealEqual(tviewdir[0],0.0) && icadRealEqual(tviewdir[1],0.0) && icadRealEqual(tviewdir[2],0.0)) break;
				ic_ptcpy(tviewdir,prbViewIterator->resval.rpoint);
				break;
			case 12:
				ic_ptcpy(ttarget,prbViewIterator->resval.rpoint);
				break;
			case 40:
				tviewsize=prbViewIterator->resval.rreal;
				break;
			case 42:
				tlenslength = prbViewIterator->resval.rreal;
			case 43:
				tfrontz=prbViewIterator->resval.rreal;
				break;
			case 44:
				tbackz=prbViewIterator->resval.rreal;
				break;
			case 50:
				tvtwist=prbViewIterator->resval.rint;
				break;
			case 71:
				tvmode=prbViewIterator->resval.rint;
				break;
		}
	}

	ic_ptcpy(target,ttarget);
	ic_ptcpy(dir,tviewdir);
	twist=tvtwist;
	size=tviewsize;
	center[0]=tcenter[0];
	center[1]=tcenter[1];
	lenslength=tlenslength;
	frontclip=tfrontz;
	backclip=tbackz;

	if (tvmode & 1)
		statusflags |= IC_VPENT_STATUS_PERSPECTIVEMODE;
	else
		statusflags &= ~IC_VPENT_STATUS_PERSPECTIVEMODE;

	if (tvmode & 2)
		statusflags |= IC_VPENT_STATUS_FRONTCLIPON;
	else
		statusflags &= ~IC_VPENT_STATUS_FRONTCLIPON;

	if (tvmode & 4)
		statusflags |= IC_VPENT_STATUS_BACKCLIPON;
	else
		statusflags &= ~IC_VPENT_STATUS_BACKCLIPON;

	if (tvmode & 8)
		statusflags |= IC_VPENT_STATUS_UCSFOLLOW;
	else
		statusflags &= ~IC_VPENT_STATUS_UCSFOLLOW;

	if (tvmode & 16)
		statusflags |= IC_VPENT_STATUS_FRONTCLIPNOTATEYE;
	else
		statusflags &= ~IC_VPENT_STATUS_FRONTCLIPNOTATEYE;


	return true;
	}

// *************************************************
// *************************************************
// *************************************************
// *************************************************
// *************************************************
// VPLAYER METHODS
// *************************************************
// *************************************************
// *************************************************


// *****************************
// RemoveLayerFromVpLayerData -- public method
//

#ifdef REMOVED
int
VpSetDbmod( db_drawing *dp)
	{
	sds_resbuf	rb;
	int			original;
	int			result = RTNORM;

	db_getvar(NULL,DB_QDBMOD,&rb,dp,NULL,NULL);

								// save initial status for UI notification
	original = rb.resval.rint;
	if( !(rb.resval.rint & IC_DBMOD_ENTITIES_MODIFIED) )
		{
		rb.resval.rint |= (IC_DBMOD_ENTITIES_MODIFIED | IC_DBMOD_DATABASEVBL_MODIFIED);
		result = db_setvar(NULL,DB_QDBMOD,&rb,dp, NULL, NULL,0);

								// notify UI if file changed
		if ( !(original & IC_DBMOD_DATABASEVBL_MODIFIED) )
			ExecuteUIAction( ICAD_WNDACTION_DBMODON );
		}

	return result;
	}
#endif


bool
db_viewport::RemoveLayerFromVpLayerData( db_layertabrec *pLayerToRemove )
	{
	DEBUGMSG("db_viewport::removeLayerFromVpLayerData -- viewport %d\n",this->id );
	return m_pVpLayerData->RemoveFrozenLayer(pLayerToRemove);
	}


// ***************************************
// addLayerToVpLayerData -- public method
//
bool
db_viewport::AddLayerToVpLayerData( db_layertabrec *pLayerToAdd )
	{
	DEBUGMSG("db_viewport::addLayerToVpLayerData -- viewport %d\n",this->id );
	return m_pVpLayerData->AddFrozenLayer(pLayerToAdd);
	}

// ********************************************
// PUBLIC VPLAYER METHODS
//

// ******************************
// VpLayerFreezeLayer -- public method
//
//
//
DB_API
bool
db_viewport::VpLayerFreezeLayer( const char *szLayerName, db_drawing *pDrawing , bool *bChanged)
	{
	bool bRetval = false;

	DEBUGMSG("db_viewport::VpLayerFreezeLayer %s\n",szLayerName );
	db_handitem *pLayer = pDrawing->findtabrec(DB_LAYERTAB, szLayerName,1);

	if ( pLayer != NULL )
		{
		ASSERT( pLayer->ret_type() == DB_LAYERTABREC );
		bRetval=this->AddLayerToVpLayerData( (db_layertabrec *)pLayer );
		if (bRetval)
			*bChanged=true;
		}

	return bRetval;
	}

// ******************************
// VpLayerThawLayer -- public method
//
//
//
DB_API
bool
db_viewport::VpLayerThawLayer( const char *szLayerName, db_drawing *pDrawing , bool *bChanged)
	{
	bool bRetval = false;

	DEBUGMSG("db_viewport::VpLayerThawLayer %s\n",szLayerName );
	db_handitem *pLayer = pDrawing->findtabrec(DB_LAYERTAB, szLayerName,1);

	if ( pLayer != NULL )
		{
		ASSERT( pLayer->ret_type() == DB_LAYERTABREC );
// we have only 6 layer entries in pDrawing upon returning from the call below
		bRetval=this->RemoveLayerFromVpLayerData( (db_layertabrec *)pLayer );
		if (bRetval)
			*bChanged=true;
		}

	return bRetval;
	}

// ******************************
// VpLayerResetLayer -- public method
//
//
//
DB_API
bool
db_viewport::VpLayerResetLayer( const char *szLayerName, db_drawing *pDrawing , bool *bChanged)
	{
	bool bRetval = false;

	db_layertabrec *pLayer = (db_layertabrec *)pDrawing->findtabrec(DB_LAYERTAB, szLayerName,1);

	if ( pLayer != NULL )
		{
		ASSERT( pLayer->ret_type() == DB_LAYERTABREC );
		// Freeze or thaw as appropriate
		//
		if ( pLayer->isVpLayerFrozenByDefault() )
			{
			bRetval = this->AddLayerToVpLayerData( pLayer );
			}
		else
			{
			bRetval = this->RemoveLayerFromVpLayerData( pLayer );
			}
		if (bRetval)
			*bChanged=true;
		}

	return bRetval;
	}

// ******************************
// IsLayerVpLayerFrozen -- public method
//
//
//
DB_API
bool
db_viewport::IsLayerVpLayerFrozen( db_layertabrec *pLayer )
	{
	ASSERT( pLayer->ret_type() == DB_LAYERTABREC );

	if ( m_pVpLayerData == NULL )
		return false;   // what else???

	return m_pVpLayerData->IsLayerFrozen( pLayer );

	}


// ******************************
// GetVpLayerFrozenLayerCount -- public method
//
//
//
DB_API
int
db_viewport::GetVpLayerFrozenLayerCount( void )
	{
	if ( m_pVpLayerData == NULL)
		return 0;  // I guess
	return m_pVpLayerData->GetFrozenLayerCount();
	}

// ******************************
// GetVpLayerFrozenLayerName -- public method
//
//
//
DB_API
char *
db_viewport::GetVpLayerFrozenLayerName( int index )
	{
	char *pRetval = NULL;
	if ( m_pVpLayerData == NULL )
		return NULL;   // what else???

	db_layertabrec *pLayer = m_pVpLayerData->GetFrozenLayer( index );

	if ( pLayer != NULL )
		{
		ASSERT( pLayer->ret_type() == DB_LAYERTABREC );
		pLayer->get_2( &pRetval );
		}

	return pRetval;
	}

// *****************************************
// PUBLIC METHOD -- class db_viewport
//
// SetVpLayerToDefaults
//
// Set all VPLAYER data to the default settings for the layers
// in the drawing
//
DB_API
bool
db_viewport::SetVpLayerToDefaults( db_drawing *pDrawing )
	{
	ASSERT( CHECKSTRUCTPTR( pDrawing ) );
	ASSERT( CHECKSTRUCTPTR( this ) );
	ASSERT( this->ret_type() == DB_VIEWPORT );

	// delete the old
	delete m_pVpLayerData;
	// add new layers that are frozen by default
	m_pVpLayerData=new VpLayerData;

	db_layertabrec *pLayer = (db_layertabrec *)pDrawing->tblnext(DB_LAYERTABREC, 1 );
	while( pLayer != NULL )
		{
		// Add layers that are frozen by default
		//
		if ( pLayer->isVpLayerFrozenByDefault() )
			this->AddLayerToVpLayerData(pLayer);
		pLayer = (db_layertabrec *)pDrawing->tblnext( DB_LAYERTABREC, 0 );
		}
	return true;  // not sure how this could fail
	}

// ********************************************************************
// public internal class db_viewport::VpLayerData
//
//

// *********************
// VpLayerData ctor
//
db_viewport::VpLayerData::VpLayerData( void )
	{
	m_pFrozenLayers = NULL;
	m_iValidFrozenLayerCount = 0;
	m_iNumStorageLocs = 0;
	}

// *********************
// VpLayerData ctor
//
db_viewport::VpLayerData::VpLayerData( int numfrozitems )
	{
	m_pFrozenLayers = new db_layertabrec *[numfrozitems];
	m_iValidFrozenLayerCount = 0;
	m_iNumStorageLocs = numfrozitems;
	}

// *********************
// VpLayerData dtor
//
db_viewport::VpLayerData::~VpLayerData( void )
	{
	// We're just storing un-copied pointers.  Don't touch the contents
	// of the frozen layers array
	//
	if ( m_pFrozenLayers != NULL )
		{
		// sanity check
		//
// right after the constructor this is no longer true, so removed
// ASSERT( m_iValidFrozenLayerCount > 0 );

		delete [] m_pFrozenLayers ;
		m_pFrozenLayers = NULL;
		}

	m_iValidFrozenLayerCount = 0;
	m_iNumStorageLocs = 0;
	}

// *********************
// VpLayerData
//
// AddFrozenLayer operation
//
bool
db_viewport::VpLayerData::AddFrozenLayer( db_layertabrec *pNewFrozenLayer )
	{
	ASSERT( pNewFrozenLayer==NULL || pNewFrozenLayer->ret_type() == DB_LAYERTABREC );  // NULL is OK when setting up a fixup

	if ( m_pFrozenLayers == NULL )
		{
		// We're just storing pointers
		//
		m_pFrozenLayers = new db_layertabrec *[1];
		m_iValidFrozenLayerCount = 1;
		m_iNumStorageLocs=1;
		}
	else
		{
//		ASSERT( m_iValidFrozenLayerCount > 0 );
		m_iValidFrozenLayerCount++;

		if (m_iValidFrozenLayerCount>m_iNumStorageLocs)
			{
			db_layertabrec ** temp;
			temp = new db_layertabrec *[m_iValidFrozenLayerCount + 10];
			memcpy(temp, m_pFrozenLayers, sizeof(db_layertabrec *) * m_iValidFrozenLayerCount);
			delete [] m_pFrozenLayers;
			m_pFrozenLayers = temp;
			m_iNumStorageLocs=m_iValidFrozenLayerCount+10;  // allocate more than we need
			}
		}
	ASSERT( m_iValidFrozenLayerCount > 0 );
	ASSERT( m_pFrozenLayers != NULL );
	if ( m_pFrozenLayers == NULL )
		{
		return false;	// very bad--allocation must have failed
		}

	m_pFrozenLayers[ m_iValidFrozenLayerCount - 1] = pNewFrozenLayer;

	return true;
	}

// *********************
// VpLayerData
//
// RemoveFrozenLayer operation
//
bool
db_viewport::VpLayerData::RemoveFrozenLayer( db_layertabrec *pLayerToRemove )
	{
	bool ret=false;
	ASSERT( pLayerToRemove->ret_type() == DB_LAYERTABREC );

	if ( m_pFrozenLayers == NULL )
		return false;
	else
		{
		ASSERT( m_iValidFrozenLayerCount > 0 );
		for (int i=0; i<m_iValidFrozenLayerCount; i++)		// we just decrement the count and slide the data
			if (m_pFrozenLayers[i]==pLayerToRemove)		// down.  We do NOT realloc (why bother?)
				{
				memmove(&m_pFrozenLayers[i],&m_pFrozenLayers[i+1],(m_iValidFrozenLayerCount-i-1)*sizeof(db_layertabrec *));
				m_iValidFrozenLayerCount--;
				ret=true;
				}
		if (m_iValidFrozenLayerCount==0)
			{
			delete [] m_pFrozenLayers;
			m_pFrozenLayers=NULL;
			m_iNumStorageLocs=0;
			}
		}
	return ret;
	}


// ************************
// VpLayerData
//
// IsLayerFrozen operation
//
// Basically, if it is in the list, it is.
//
bool
db_viewport::VpLayerData::IsLayerFrozen( db_layertabrec *pLayerRecord )
	{
	// If no frozen layers, the layer ain't frozen
	//
	if ( m_iValidFrozenLayerCount < 1 )
		{
//   		ASSERT( m_pFrozenLayers == NULL ); // sanity
		return false;
		}
	ASSERT( m_pFrozenLayers != NULL ); // only got here if m_iValidFrozenLayerCount >= 1

	ASSERT( pLayerRecord->ret_type() == DB_LAYERTABREC );

	bool bRetval = false;
	int i;
	for( i = 0; i < m_iValidFrozenLayerCount; i++ )
		{
		if(!m_pFrozenLayers[i])		/*D.G.*/// Some files have invalid 341 handles of frozen layers in viewport entities.
			continue;				// So, according layertabrecs pointers are stay NULL after "dummying" in drw_makeentlist > AddLayerToVpLayerData(NULL).
		if ( pLayerRecord->RetHandle()==m_pFrozenLayers[i]->RetHandle())
			{
			bRetval = true;
			break;
			}
		}

		return bRetval;
	}
// ************************
// VpLayerData
//
// GetFrozenLayerCount
//
//
int
db_viewport::VpLayerData::GetFrozenLayerCount( void )
	{
	return m_iValidFrozenLayerCount;
	}

// ************************
// VpLayerData
//
// GetFrozenLayer
//
//
db_layertabrec *
db_viewport::VpLayerData::GetFrozenLayer( int index )
	{
	db_layertabrec *pRetval = NULL;

	// get it if it's within range
	//
	if ( (index >= 0) &&
		 (index < m_iValidFrozenLayerCount) )
		{
		pRetval = m_pFrozenLayers[index];
		}

	return pRetval;
	}

void db_viewport::BreakAssociation(db_handitem * entity, bool dispMessage)
	{
	BreakAssociation(dispMessage);
	}

void db_viewport::BreakAssociation(bool dispMessage)
	{
	if (0 == g_nClosingDrawings && clipbound)
		clipbound->removeReactor(this->RetHandle());

	// set the clipbound to NULL
	clipbound = NULL;
	}

/* C:\ICAD\PRJ\LIB\DB\LWPOLYLINE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_lwpolyline *****************************/

/*F*/
db_lwpolyline::db_lwpolyline(void) : db_entity_with_extrusion(DB_LWPOLYLINE) {
    thick=0.0;

    elev=constwid=0.0; wid=NULL; bulge=NULL; flags=0;

    /* Make 2 (0,0) verts -- just like ACAD's entmake with no verts: */
    nverts=2; vert=new sds_point[nverts];
    vert[0][0]=vert[0][1]=vert[1][0]=vert[1][1]=0.0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_lwpolyline::db_lwpolyline(db_drawing *dp) : db_entity_with_extrusion(DB_LWPOLYLINE,dp) {
    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);


    elev=constwid=0.0; wid=NULL; bulge=NULL; flags=0;

    /* Make 2 (0,0) verts -- just like ACAD's entmake with no verts: */
    nverts=2; vert=new sds_point[nverts];
    vert[0][0]=vert[0][1]=vert[1][0]=vert[1][1]=0.0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_lwpolyline::db_lwpolyline(const db_lwpolyline &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    elev=sour.elev;
    constwid=sour.constwid;
    flags=sour.flags;
    nverts=sour.nverts;
    vert=NULL;
    if (sour.vert!=NULL && nverts>0) {
        vert=new sds_point[nverts];
        memcpy(vert,sour.vert,nverts*sizeof(vert[0]));
    }
    wid=NULL;
    if (sour.wid!=NULL && nverts>0) {
        wid=new sds_real[nverts][2];
        memcpy(wid,sour.wid,nverts*sizeof(wid[0]));
    }
    bulge=NULL;
    if (sour.bulge!=NULL && nverts>0) {
        bulge=new sds_real[nverts];
        memcpy(bulge,sour.bulge,nverts*sizeof(bulge[0]));
    }

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_lwpolyline::~db_lwpolyline(void) {
    delete [] vert; delete [] wid; delete [] bulge;
    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

//Specific entgetter.  Returns 0 (OK) or -1 (no memory).
int db_lwpolyline::entgetspecific(resbuf **begpp, resbuf **endpp, db_drawing *dp)
	{
    int rc;
    struct resbuf *sublist[2];
	int i;
	sds_point pt;
	short flag; 
	sds_real constantwidth,elev,thickness,startwidth,endwidth,bulge;

    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

	// 90 (numverts)
	nverts=ret_90();
    if ((sublist[0]=sublist[1]=db_newrb(90,'L',&nverts))==NULL) { rc=-1; goto out; }

	flag=ret_70();
    if ((sublist[1]=sublist[1]->rbnext=db_newrb(70,'H',&flag))==NULL) { rc=-1; goto out; }

	constantwidth=ret_43();
    if ((sublist[1]=sublist[1]->rbnext=db_newrb(43,'R',&constantwidth))==NULL) { rc=-1; goto out; }

	elev=ret_38();
    if ((sublist[1]=sublist[1]->rbnext=db_newrb(38,'R',&elev))==NULL) { rc=-1; goto out; }

	thickness=ret_39();
    if ((sublist[1]=sublist[1]->rbnext=db_newrb(39,'R',&thickness))==NULL) { rc=-1; goto out; }

	// 210 Extrusion is part of the base class, so get it from accessor
	sds_point extru;
	get_210(extru);
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	for (i=0; i<nverts; i++)
		{
		get_10(pt,i);
		pt[2]=elev;
		get_40(&startwidth,i);
		get_41(&endwidth,i);
		get_42(&bulge,i);
	    if ((sublist[1]=sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
	    if ((sublist[1]=sublist[1]->rbnext=db_newrb(40,'R',&startwidth))==NULL) { rc=-1; goto out; }
	    if ((sublist[1]=sublist[1]->rbnext=db_newrb(41,'R',&endwidth))==NULL) { rc=-1; goto out; }
	    if ((sublist[1]=sublist[1]->rbnext=db_newrb(42,'R',&bulge))==NULL) { rc=-1; goto out; }
		}
out:
	if (begpp!=NULL && endpp!=NULL)
		{
		if (rc && sublist[0]!=NULL)
			{
			sds_relrb(sublist[0]);
			sublist[0]=sublist[1]=NULL;
			}
		*begpp=sublist[0];
		*endpp=sublist[1];
		}

	return rc;
	}

int db_lwpolyline::entmod(resbuf *modll, db_drawing *dp)
{
    int rc, i;
    struct resbuf *curlink, *tmplink = NULL;
    db_handitem *thiship;

	bool bUseConstWidth = false;
	sds_real constWidth = 0.0;

    rc=0; thiship=(db_handitem *)this;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) 
		{
		rc=-1;
		goto out;
		}

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
	
	// save curlink pointer for further processing of vertices
	tmplink = curlink;

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) 
		{

        /* Call the correct setter: */
        switch (curlink->restype)
			{
			case 90:
				if (curlink->resval.rlong<2) // at least two required
					{
					rc=-5;
					goto out;
					}
				nverts=curlink->resval.rlong;	// numverts
// remove points, widths,bulges, allocate points
		        delete [] vert;
				vert=NULL;
		        delete [] wid;
				wid=NULL;
				delete [] bulge;
				bulge=NULL;
		        flags&=0x80;  // Zero all but PLINEGEN
				vert=new sds_point[nverts];
				break;

			case 210:
				set_210(curlink->resval.rpoint);
				break;
			
			case 70:
				set_70(curlink->resval.rint);	// flag
				break;

			case 43:
				if(!icadRealEqual(curlink->resval.rreal,0.0))
				{
					set_43(curlink->resval.rreal);	// constantwidth
					bUseConstWidth = true;
					constWidth = curlink->resval.rreal;
				}
				break;

			case 38:
				set_38(curlink->resval.rreal);	// elev
				break;

			case 39:
				set_39(curlink->resval.rreal);	// thickness
				break;

	        default:  // The common entity or table record groups.
	           rc=thiship->entmod1common(&curlink,dp);
			} ;
		}

	// process vertices now
	i = 0;
    for( curlink = tmplink; curlink != NULL && curlink->restype != -3 && i < nverts; curlink = curlink->rbnext ) 
	{
        /* Call the correct setter: */
        if( curlink->restype != 10 )
			continue;

		set_10( curlink->resval.rpoint,i );
		set_40( constWidth, i );
		set_41( constWidth, i );
		set_42( 0.0, i );

		bool goOut = false;
		for( struct sds_resbuf *link = curlink->rbnext; link && !goOut; link = link->rbnext )
		{
			switch( link->restype )
			{
			case 40:
				if( !bUseConstWidth )
					set_40( link->resval.rreal, i );
				break;
			case 41:
				if( !bUseConstWidth )
					set_41( link->resval.rreal, i );
				break;
			case 42:
				set_42( link->resval.rreal, i );
				break;
			default:
				goOut = true;
				break;
			}
		}
		i++;
	}
	if( i != nverts ) // error
	{
		rc=-5;
		goto out;
	}
//////
    if (db_lasterror) { rc=-6; goto out; }

    /* Do the EED: */
    if (!rc && curlink!=NULL)
		rc=set_eed(curlink,dp);

out:
	return rc;
}

int db_lwpolyline::check(db_drawing* pDrawing)
{
	if(!db_entity_with_extrusion::check(pDrawing))
		return 0;

	if(bulge != NULL)
	{
		// DP: limit vertex bulge value
		for(int i = 0; i < nverts; ++i)
			if(fabs(bulge[i]) > FLT_MAX)
				bulge[i] = FLT_MAX;
	}
	return 1;
}




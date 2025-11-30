/* C:\ICAD\PRJ\LIB\DB\POLYLINE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1.12.1 $ $Date: 2003/08/29 01:34:03 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_polyline *****************************/

/*F*/
db_polyline::db_polyline(void) : db_entity_with_extrusion(DB_POLYLINE) {
    db_qgetvar(DB_QTHICKNESS,NULL,&thick ,DB_DOUBLE,0);
    elev=defwid[0]=defwid[1]=0.0; flags=stype=0;
    count[0]=count[1]=dens[0]=dens[1]=0; 
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_polyline::db_polyline(db_drawing *dp) : db_entity_with_extrusion(DB_POLYLINE,dp) {
    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);

    elev=defwid[0]=defwid[1]=0.0; flags=stype=0;
    count[0]=count[1]=dens[0]=dens[1]=0;
    disp=NULL; memset(extent,0,sizeof(extent));

	//set complex main so it will be set correctly for undo/redo of vertices that are created before adding to the drawing.
	char          pendflags;
    db_handitem  *pendentmake0;
    db_handitem  *pendentmake1;
    db_handitem  *complexmain;
    db_handitem  *lastblockdef;

	dp->get_pendstuff(&pendflags, &pendentmake0, &pendentmake1,&complexmain,&lastblockdef);
    dp->set_pendstuff(pendflags, pendentmake0, pendentmake1, this, lastblockdef);

}
db_polyline::db_polyline(const db_polyline &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    elev   =sour.elev   ; defwid[0]=sour.defwid[0]; defwid[1]=sour.defwid[1];
    flags  =sour.flags  ;  count[0]=sour.count[0] ; count[1]=sour.count[1];
    dens[0]=sour.dens[0];  dens[1]=sour.dens[1]   ; stype=sour.stype;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_polyline::~db_polyline(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_polyline::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    short fsh1;
    int rc;
    sds_point ap1;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 66 (verts follow) */
    fsh1=1;
    if ((sublist[0]=sublist[1]=db_newrb(66,'H',&fsh1))==NULL) { rc=-1; goto out; }

    /* 10 (0,0,elev) */
    ap1[0]=ap1[1]=0.0; ap1[2]=elev;
    if ((sublist[1]->rbnext=db_newrb(10,'P',ap1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 39 (thick) */
    if ((sublist[1]->rbnext=db_newrb(39,'R',&thick))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 70 (flags) */
    fsh1=(unsigned char)flags;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (defwid[0]) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',defwid))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 (defwid[1]) */
    if ((sublist[1]->rbnext=db_newrb(41,'R',defwid+1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 71 (count[0]) */
    fsh1=(short)count[0];
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 72 (count[1]) */
    fsh1=(short)count[1];
    if ((sublist[1]->rbnext=db_newrb(72,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 73 (dens[0]) */
    fsh1=(short)dens[0];
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 74 (dens[1]) */
    fsh1=(short)dens[1];
    if ((sublist[1]->rbnext=db_newrb(74,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 75 (stype) */
    fsh1=(short)stype;
    if ((sublist[1]->rbnext=db_newrb(75,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}


bool 
db_polyline:: set_8(char *p, db_drawing *dp) 
	{
	ASSERT( ret_type() EQ DB_POLYLINE );
	bool result = db_entity::set_8( p, dp);

	if ( result )
		{
	/*
	**  If it's a POLYLINE, set this layer for all subentities, too.
	*/
		db_handitem *thip1=next;

		for (
				; 
				thip1!=NULL && (thip1->ret_type()==DB_VERTEX ||thip1->ret_type()==DB_SEQEND); 
				thip1=thip1->next
				)
			{
			int vertexFlags;

								// always set layer for true vertex
			thip1->get_70( &vertexFlags);
			if ( vertexFlags & IC_VERTEX_3DMESHVERT )
				thip1->set_layerhip(layerhip);

								// only set layer for mesh if missing
			else if ( !thip1->ret_layerhip() )
				thip1->set_layerhip(layerhip);
			}
		}

	return result;
	}

// This function counts the number of ordinary vertices in a polyline
// It will return false and set *nverts to zero if the polyline is 
// a 3D polygon mesh or a polyface mesh.  Spline frame vertices will
// not be included in the value of *nverts.
bool db_polyline::count_ordinary_vertices(int * nverts)
	{
	bool ordinary;
	*nverts = 0;

	int flags;
	this->get_70(&flags);

	if ((flags & IC_PLINE_3DMESH) || (flags & IC_PLINE_POLYFACEMESH))
		{
		ordinary = false;
		}
	else
		{
		ordinary = true;

		bool done = false;
		db_handitem * hip = this;
		while (!done)
			{
			hip = hip->next;
			if (hip!=NULL)
				{  // begin case of hip!=NULL
				if (hip->ret_type() == DB_VERTEX)
					{   // begin case of == DB_VERTEX
					hip->get_70(&flags);
					// only increment the count if it is not a splineframe vertex
					if (!(flags & IC_VERTEX_SPLINEFRAME))
						++(*nverts);
					}   //   end case of == DB_VERTEX
				else
					{   // begin case of != DB_VERTEX
					// quit as soon as we encounter a non-DB_VERTEX
					done = true;
					}   //   end case of != DB_VERTEX
				}  //   end case of hip!=NULL
			else
				{  // begin case of hip==NULL
				// quit if we encounter a NULL hip
				done = true;
				}  //   end case of hip==NULL
			}
		}

	return ordinary;
	}


int db_polyline::check(db_drawing* pDrawing)
{
	if(!db_entity_with_extrusion::check(pDrawing))
		return 0;

	int vertexFlags, sourceVertexFlags;
	db_handitem* hip = this->next;
	while(hip && hip->ret_type() == DB_VERTEX)
	{
		if(hip->get_70(&vertexFlags) && !vertexFlags)
		{
			// Try to recover vertex's flags (dfx70)
			sourceVertexFlags = vertexFlags;

			// If polyline is a 3D polyline(flags=8) then vertex should be 3D polyline vertex(32).
			// If polyline is a 3D polygon mesh(flags=16) then vertex should be 3D polygon mesh(64).
			// If polyline is a polyface mesh(flags=64) then vertex should be polyface mesh vertex(128).
			// Only these bits can be changed in the vertex's flags. 

			if(flags & IC_PLINE_3DPLINE && !(vertexFlags & IC_VERTEX_3DPLINE))
			{
				vertexFlags |= IC_VERTEX_3DPLINE;
				vertexFlags &= ~(IC_VERTEX_3DMESHVERT|IC_VERTEX_FACELIST);
			}
			else if(flags & IC_PLINE_3DMESH && !(vertexFlags & IC_VERTEX_3DMESHVERT))
			{
				vertexFlags |= IC_VERTEX_3DMESHVERT;
				vertexFlags &= ~IC_VERTEX_3DPLINE;
			}
			else if(flags & IC_PLINE_POLYFACEMESH && !(vertexFlags & IC_VERTEX_FACELIST))
			{
				vertexFlags |= IC_VERTEX_FACELIST;
				vertexFlags &= ~IC_VERTEX_3DPLINE;
			}


			// To check 64 bit for Polyface mesh vertex 
			if(vertexFlags & IC_VERTEX_FACELIST)
			{
				int vertexIndex;
				bool bVertexDefinesFace = (hip->get_71(&vertexIndex) && vertexIndex
						|| hip->get_72(&vertexIndex) && vertexIndex
						|| hip->get_73(&vertexIndex) && vertexIndex
						|| hip->get_74(&vertexIndex) && vertexIndex);
				if(bVertexDefinesFace)
					// If the vertex defines a face of the mesh, its vertex flags group has the 128 bit set but not the 64 bit. 
					vertexFlags &= ~IC_VERTEX_3DMESHVERT; 
				else
					vertexFlags |= IC_VERTEX_3DMESHVERT;
			}

			if(sourceVertexFlags != vertexFlags)
			{
				//ASSERT(0);
				hip->set_70(vertexFlags);
			}
		}		
		hip = hip->next;
	}
	return 1;
}

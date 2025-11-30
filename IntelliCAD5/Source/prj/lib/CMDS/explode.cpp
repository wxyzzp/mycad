/* F:\DEV\PRJ\LIB\CMDS\EXPLODE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * EXPLODE command
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "db.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadApi.h"
#include "fixup1005.h"
#include "DbAcisEntity.h"
#include "configure.h"
#include "Modeler.h"
#include "IcadDoc.h"  //EBATECH(FUTA) 2001-01-15
#include "Explode.h"  // SystemMetrix(watanabe) explode LEADER
#include "GeoData.h"
#include "CmdCurveConstructor.h"
#include "xstd.h"
#include "transf.h"
using icl::transf;
#include "gvector.h"
using icl::gvector;
using icl::point;
#include "CmdQueryTools.h" //wcs2ucs, ucs2wcs

// Proto
int   cmd_xplode_addinherit(sds_resbuf *elist,sds_resbuf *rbinherit);
int   cmd_xplode_getinheritmode(int *mode, sds_resbuf **rbprop);
int   cmd_explode_pfacemesh(struct resbuf *elbeg,sds_name ename, sds_name ssetout=NULL);
int	  cmd_explode_plmesh(struct resbuf *elbeg,sds_name ename, int mcnt, int ncnt, int plflag, sds_name ssetout=NULL);

static
int createPolyfaceMesh
(
const xstd<point>::vector& points,
const xstd<int>::vector& faces,
const char* szLayer,
int color
);
static
int makePolyfaceMeshHeader
(
int nPoints,
int nFaces,
const char* layer,
int color
);
static
int makePolyfaceMeshPointVertices
(
const xstd<point>::vector& points,
const char* layer,
int color
);
static
int makePolyfaceMeshFaceVertices
(
const xstd<int>::vector& faces,
const char* layer,
int color
);
static
int makeSeqend
(
const char* layer,
int color
);
static
int createBodiesOrRegions
(
const xstd<CDbAcisEntityData>::vector& objs,
const char* szLayer,
int color
);
static
int createCurves
(
xstd<geodata::curve*>::vector&,
const char* szLayer,
int color
);

//Modified Cybage SBD 13/11/2001 DD/MM/YYYY
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla
extern BOOL bdimUpdate;

short cmd_explode(void){
	//short ret=cmd_explode_objects(0);
	//EBATECH(FUTA)-[  2001-01-15
	// select "Previous" exploded entities

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	sds_name ss;
	short ret=cmd_explode_objects(0,true,NULL,ss);
	if (ret==RTNORM && SDS_CURDOC){
		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_sscpy(SDS_CURDOC->m_pPrevSelection,ss);
	}
	sds_ssfree(ss);
	//]-EBATECH(FUTA)
	return ret;
}
short cmd_xplode(void){
	//short ret=cmd_explode_objects(1);
	//EBATECH(FUTA)-[  2001-01-15
	sds_name ss;
	short ret=cmd_explode_objects(1,true,NULL,ss);
	if (ret==RTNORM && SDS_CURDOC){
		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_sscpy(SDS_CURDOC->m_pPrevSelection,ss);
	}
	sds_ssfree(ss);
	//]-EBATECH(FUTA)
	return ret;
}

short cmd_explode_objects(int inheritmode, bool bInteractive, sds_name ssetin, sds_name ssetout)
	{

//*** This function breaks a compound entity into its component entities.
//*** If
//*** Enhancement - This function explodes MINSERT entities.
//***
//*** ~| TODO - Right now I am only exploding INSERT, POLYLINE, and DIMENSION entities.
//***		  - added 3DFACEs, TRACEs, and SOLIDs.
//***         - Still need: 3D MESH, GROUP, MLINE, POLYFACE MESH,
//***           POLYGON MESH and nonuniformly scaled entities within an INSERT.
//***
//*** RETURNS: RTNORM  - Success.
//***          RTCAN   - User cancel.
//***          RTERROR - Error returned from an ads function.
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF];
    struct resbuf *filter,*elist,*tmprb,*rbp,*elbeg,*elcur,*elbegt,*rbinherit,app,rbecs,rbwcs;
    sds_name ss1,ename,enttmp,enttmp2,ss2;
    sds_real xscl,yscl,zscl,rotang,rowspace,colspace,rad,stang,endang,fr2,
        lastbulge;
    sds_point inspt,pt2,pt3,blkinspt,lastpt,firstpt,pt210;
    int ret,pdel,isdim=0,errmsg=0;
    int fi1,fi2,colct,rowct,attribflg,initflg,plineflag=0,psflag;
    long ssct,rc;
    sds_point cmd_zeropt={0.0,0.0,0.0};

	resbuf*		subEntities = NULL;

		// track the eed 1005 handles so that they can be updated
		// to the new handle information later
	SDS_CURDWG->Make1005FixupTables();

    rc=0L;
	rbinherit=filter=elist=tmprb=elbeg=elbegt=elcur=NULL;
    //*** Build filter list.
    // SystemMetrix(watanabe)-[explode LEADER
    //if ((filter=sds_buildlist(RTDXF0,"INSERT,POLYLINE,LWPOLYLINE,DIMENSION,3DFACE,TRACE,SOLID,3DSOLID,REGION,BODY"/*DNT*/,0))==NULL)
    filter = sds_buildlist(
//4M Item:131->
// Explode BHatch
//                RTDXF0, "INSERT,POLYLINE,LWPOLYLINE,DIMENSION,3DFACE,TRACE,SOLID,3DSOLID,REGION,BODY,LEADER"/*DNT*/,
// EBATECH(CNBR) -[ MTEXT
//                RTDXF0, "INSERT,POLYLINE,LWPOLYLINE,DIMENSION,3DFACE,TRACE,SOLID,3DSOLID,REGION,BODY,LEADER,HATCH"/*DNT*/,
                RTDXF0, "INSERT,POLYLINE,LWPOLYLINE,DIMENSION,3DFACE,TRACE,SOLID,3DSOLID,REGION,BODY,LEADER,HATCH,MTEXT"/*DNT*/,
// EBATECH(CNBR) ]-
//<-4M Item:131
                0);
    if (filter == NULL)
    // ]-SystemMetrix(watanabe) explode LEADER
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-__LINE__);
		goto bail;
		}
    //*** Get selection set of entities.
//    if ((ret=sds_pmtssget("\nSelect entities to explode: ",NULL,NULL,NULL,filter,ss1,1))==RTERROR) {
	//NOTE: don't put ents in prev sset, because we're going to delete them if we explode them

	/*temp ssget call used w/temp code below*/

	// We have a special case for explode only!!
	// If this command is run from a script file, sds_command(), or (command)
	// you are prompted for one selection only.
	//NOTE: don't put ents in prev sset, because we're going to delete them if we explode them
	extern int lsp_cmdhasmore;

	if (bInteractive)
		{
		if ((ret=sds_pmtssget(ResourceString(IDC_EXPLODE__NSELECT_ENTITIE_0, "\nSelect entities to explode: " ),
		   (GetActiveCommandQueue()->IsEmpty() && lsp_cmdhasmore==0) ? NULL : (LPCTSTR)""/*DNT*/ ,
			NULL,NULL,filter,ss1,1))==RTERROR)
			{
			cmd_ErrorPrompt(CMD_ERR_NOENT,0);
			goto bail;
			}
		else if (ret==RTCAN)
			{
			rc=(-1L);
			goto bail;
			}
		}
	else
		{
		if (ssetin == NULL)
			goto bail;

		ic_encpy(ss1, ssetin);
		}

//4M Item:131->
// Explode BHatch
/*
    if (RemoveHatchFromSS(ss1) == RTNIL)
		{
		rc = -1L;
		goto bail;
		}
*/
//<-4M Item:131

	//initialize the output selection set
	if (ssetout)
		sds_ssadd (NULL, NULL, ssetout);


	//**********************CODE UNIQUE TO XPLODE COMMAND**************************
	if (inheritmode)
		{
		//We'll use inheritmode as follows:
		//	Bit 0 (1) = Individual prompts for all ents in selection set.  If bit is set,
		//				we'll keep coming back to this point
		//	Bit 1 (2) = inherit layer
		//	Bit 2 (4) = inherit color
		//	Bit 3 (8) = inherit linetype
		//	Bit 4 (16)= inherit linetype scale
		//	Bit 5 (32)= inherit from parent
		//  Bit 6 (64)= multiple mode(used for command looping only)
		//  Bit 7(128)= NO INHERITING TO BE PERFORMED
		inheritmode=0;

		if (bInteractive)
			{
			sds_initget(0,ResourceString(IDC_EXPLODE_INITGET_SEPARATE_1, "Separately ~Individually ~Globally All ~_Separately ~_Individually ~_Globally ~_All" ));
			rc=sds_getkword(ResourceString(IDC_EXPLODE_XPLODE_OBJECTS___2, "Xplode objects: Separately/<All>: " ),fs1);
			if (rc==RTERROR || rc==RTCAN){rc=-1L;goto bail;}
			if (strisame(fs1,"SEPARATELY"/*DNT*/ ) || strisame(fs1,"INDIVIDUALLY"/*DNT*/ ))
			   inheritmode|=1;
			//all other cases inheritmode&1=0
			}
		else
			inheritmode = 128;
		}
	else
		inheritmode=128;


	//*** Loop through the selection set and make new entities.
    app.restype=RTSTR; app.resval.rstring="*"/*DNT*/ ; app.rbnext=NULL;

#ifdef BUILD_WITH_ACIS_SUPPORT
    /*D.G.*/ //<ALM>
    if (bInteractive)
    {
	// Since we don't know if the user has selected an ACIS entity to explode, we will
	// preprocess the selection set, looking for ACIS entities. The real problem here is
	// inserts. They could contain ACIS entities. If we find any, we will warn the user
	// and ask if we should proceed.
	// WE COULD SIMPLY REMOVE THE THING AND EXPLODE WHATS LEFT. That decision is up to
	// someone else.
	for (ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++)
		{
		bool foundACISent = FALSE;
		switch(IC_TYPE_FROM_ENAME(ename))
			{
			case DB_3DSOLID:
			case DB_REGION:
			case DB_BODY:
				foundACISent = TRUE;
				break;
			case DB_INSERT:
				{
				db_handitem *currentEnt, *lastEnt;
				db_blocktabrec *block = ((db_insert*)ename[0])->ret_bthip();
				block->get_blockllends(&currentEnt, &lastEnt);
				while (currentEnt != NULL)
					{
					int entType = currentEnt->ret_type();
					if (entType == DB_3DSOLID || entType == DB_REGION || entType == DB_BODY)
						{
						foundACISent = TRUE;
						break;
						}
					currentEnt = currentEnt->next;
					}
				}
				break;
			default:
				break;
			}	// switch
		if (foundACISent == TRUE)
            {
            if (!CModeler::get()->isStarted())
                {
				char warning[IC_ACADBUF];
				char caption[IC_ACADBUF];
				_snprintf(warning,sizeof(warning),	ResourceString(IDC_ERRORPROMPT_EXPLODEACIS,"At least one entity in the selection set contains ACIS information.\nExploding it will remove this information.\nDo you want to explode selection anyway?") );
				_snprintf(caption,sizeof(caption),	ResourceString(IDC_ERRORPROMPT_EXPLODEACIS_1,"Warning: Possible data lose.") );
				if (MessageBox(IcadSharedGlobals::GetMainHWND(), warning, caption, MB_YESNO) == IDNO)
					{
					sds_redraw(ename,IC_REDRAW_UNHILITE);
					rc = -1L;
					goto bail;
					}
                }
		    }
            break;
		}	// for( thru entities
	}	// ACIS modeler is not available
#endif //BUILD_WITH_ACIS_SUPPORT

    for (ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++)
		{
		// Ebatech(CNBR) Bugzilla#78093 Set the Undo so we corectly redraw the entity Redo.
		SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)ename[0],NULL,NULL,SDS_CURDWG);
		// ]-

		if (0==(inheritmode&128) && (ssct==0L || inheritmode&1))
			{
			if (rbinherit!=NULL){sds_relrb(rbinherit);rbinherit=NULL;}
			if (inheritmode&1)
				{
				inheritmode=1;
				sds_redraw(ename,IC_REDRAW_UNHILITE);
				}
			//have user pick new properties for subent to inherit.
			if (0!=cmd_xplode_getinheritmode(&inheritmode,&rbinherit))
				{
				rc=-1L;
				goto bail;
				}
			}

        rowct=colct=1;
		colspace=rowspace=0.0;
		attribflg=0;
		xscl=yscl=zscl=1.0;
		rotang=0.0;
        //*** Check for a Cancel from the user.
        if (sds_usrbrk()) { rc=(-1L); goto bail; }
        if ((elist=sds_entgetx(ename,&app))==NULL)
			{
            cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            rc=(-__LINE__);
			goto bail;
			}
		if (inheritmode&32)
			{
			//if user wants to inherit properties of the parent, get 'em....
			sds_relrb(rbinherit);rbinherit=tmprb=NULL;
			if (0==ic_assoc(elist,62))
				{
				if (NULL==(tmprb=rbinherit=sds_buildlist(62,ic_rbassoc->resval.rint,0)))
					{
					rc=-1L;
					goto bail;
					}
				}
			if (0==ic_assoc(elist,48))
				{
				if (tmprb!=NULL)
					{
					if (NULL==(tmprb=tmprb->rbnext=sds_buildlist(48,ic_rbassoc->resval.rreal,0)))
						{
						rc=-1L;
						goto bail;
						}
					}
				else
					{
					if (NULL==(tmprb=rbinherit=sds_buildlist(48,ic_rbassoc->resval.rreal,0)))
						{
						rc=-1L;
						goto bail;
						}
					}
				}
			if (0==ic_assoc(elist,8))
				{
				if (tmprb!=NULL)
					{
					if (NULL==(tmprb=tmprb->rbnext=sds_buildlist(8,ic_rbassoc->resval.rstring,0)))
						{
						rc=-1L;
						goto bail;
						}
					}
				else
					{
					if (NULL==(tmprb=rbinherit=sds_buildlist(8,ic_rbassoc->resval.rstring,0)))
						{
						rc=-1L;
						goto bail;
						}
					}
				}
			if (0==ic_assoc(elist,6))
				{
				if (tmprb!=NULL)
					{
					if (NULL==(tmprb=tmprb->rbnext=sds_buildlist(6,ic_rbassoc->resval.rstring,0)))
						{
						rc=-1L;
						goto bail;
						}
					}
				else
					{
					if (NULL==(tmprb=rbinherit=sds_buildlist(6,ic_rbassoc->resval.rstring,0)))
						{
						rc=-1L;
						goto bail;
						}
					}
				}
			}
		if (ic_assoc(elist,67))
			psflag=0/*1*/;	// Default is Model 2003-4-30 Ebatech(cnbr)
		else
			psflag=ic_rbassoc->resval.rint;
        if (ic_assoc(elist,0)!=0)
			{
			rc=(-__LINE__);
			goto bail;
			}
        if (strsame("INSERT"/*DNT*/,ic_rbassoc->resval.rstring) || strsame("DIMENSION"/*DNT*/,ic_rbassoc->resval.rstring))
			{
			if (strsame("DIMENSION"/*DNT*/,ic_rbassoc->resval.rstring))
				isdim=1;
			//Modified Cybage SBD 10/11/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No. 77921 from Bugzilla
			else
				isdim=0;
			//Modified Cybage SBD 10/11/2001 DD/MM/YYYY ]
            if (ic_assoc(elist,2)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}
            strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
            if ((tmprb=sds_tblsearch("BLOCK"/*DNT*/,fs1,0))==NULL)
				{
				rc=(-__LINE__);
				goto bail;
				}
			ic_assoc(tmprb,70);
			if (ic_rbassoc->resval.rint&(IC_BLOCK_XREF|IC_ENTRY_XREF_DEPENDENT))
				{
				sds_redraw(ename,IC_REDRAW_UNHILITE);
				//only give error msg once
				if (0==(errmsg&1))
					cmd_ErrorPrompt(CMD_ERR_XPLODXREF,0);
				errmsg&=1;
				IC_RELRB(tmprb);
				continue;
				}
            if (ic_assoc(tmprb,-2)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}
            ic_encpy(enttmp,ic_rbassoc->resval.rlname);
            if (ic_assoc(tmprb,10)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}
            ic_ptcpy(blkinspt,ic_rbassoc->resval.rpoint);
            IC_RELRB(tmprb);
			pt210[0]=pt210[1]=0.0;
			pt210[2]=1.0;
            //*** Get the insertion pt, scale, and rotation of the insert.
			if (isdim)
				{
				// Ebatech(CNBR) -[ Bugzilla#77798 2003/4/30 Exploding dimension changes their location.
				if (0==ic_assoc(elist,12))
					ic_ptcpy(inspt,ic_rbassoc->resval.rpoint);
				else
					ic_ptcpy(inspt,cmd_zeropt);
				// EBATECH(CNBR) ]-
				if (0==ic_assoc(elist,210))
					ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
				}
			else
				{
				xscl=yscl=zscl=1.0;
				rotang=colspace=rowspace=0.0;
				colct=rowct=1;
				ic_ptcpy(inspt,cmd_zeropt);
				for (tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext)
					{
					if (tmprb->restype==10 && !isdim)
						ic_ptcpy(inspt,tmprb->resval.rpoint);
					//I think this next line's bogus
					//else if (tmprb->restype==3)ic_ptcpy(inspt,cmd_zeropt);
					else if (tmprb->restype==41)
						xscl=tmprb->resval.rreal;
					else if (tmprb->restype==42)
						yscl=tmprb->resval.rreal;
					else if (tmprb->restype==43)
						zscl=tmprb->resval.rreal;
					else if (tmprb->restype==50)
						rotang=tmprb->resval.rreal;
					else if (tmprb->restype==70)
						{
						colct=tmprb->resval.rint;
						tmprb->resval.rint=1/*0*/;
						}
					else if (tmprb->restype==71)
						{
						rowct=tmprb->resval.rint;
						tmprb->resval.rint=1/*0*/;
						}
					else if (tmprb->restype==44)
						{
						colspace=tmprb->resval.rreal;
						tmprb->resval.rreal=0.0;
						}
					else if (tmprb->restype==45)
						{
						rowspace=tmprb->resval.rreal;
						tmprb->resval.rreal=0.0;
						}
					else if (tmprb->restype==210)
						ic_ptcpy(pt210,tmprb->resval.rpoint);
					}
				}

			fi2 = colct * rowct;
            if (fi2>1)
				{
				// EBATECH(CNBR) 2003/4/30 Bugzilla#78510 Exploding MINSERT.
				rbwcs.restype=RTSHORT;
				rbwcs.resval.rint=0;
				rbecs.restype=RT3DPOINT;
				ic_ptcpy(rbecs.resval.rpoint,pt210);
                //*** Explode a MINSERT by creating an array of inserts
                if (ic_assoc(elist,66)==0 && ic_rbassoc->resval.rint)
					attribflg=1;
				//change the inherited properties if specified
				cmd_xplode_addinherit(elist,rbinherit);
				for (fi1=0; fi1<rowct; fi1++)
					{
					for (fi2=0; fi2<colct; fi2++)
						{
						//*** Modify the first INSERT
						//*** It is necesery for reset MINSERT properties and affect XPLODE properties.
						if (0==fi1+fi2)
							{
							if (sds_entmod(elist)!=RTNORM)
								{
								rc=(-__LINE__);
								goto bail;
								}
							// Remove reactor list for new entities. Bugzilla#78510 2003/4/30
							RemovePersistentReactors(elist);
							// Add previous selection set.
							if (ssetout)
								{
								sds_ssadd (ename, ssetout, ssetout);
								}
							}
                        //*** Make the INSERT
						else
							{
							//*** Increment the insertion point of the entity
							if (ic_assoc(elist,10)!=0)
								{
								rc=(-__LINE__);
								goto bail;
								}
							sds_polar(inspt,rotang+(IC_PI/2.0),(rowspace*fi1),pt2);
							sds_polar(pt2,rotang,(colspace*fi2),ic_rbassoc->resval.rpoint);

							if (sds_entmakex(elist, enttmp)!=RTNORM)
								{
								rc=(-__LINE__);
								goto bail;
								}
							// EBATECH(CNBR) 2003/4/30 Bugzilla#78510 Forget to move GC=1011
								{
								bool bMod = false;

								if (ssetout)
									{
									sds_ssadd (enttmp, ssetout, ssetout);
									}
								IC_RELRB(tmprb);
								if ((tmprb=sds_entgetx(enttmp,&app))==NULL)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
									}
								for(rbp=tmprb;rbp;rbp=rbp->rbnext)
									{
									if(rbp->restype==1011)
										{
											sds_trans(rbp->resval.rpoint,&rbwcs,&rbecs,0,pt2);
											sds_polar(pt2,rotang+(IC_PI/2.0),(rowspace*fi1),pt3);
											sds_polar(pt3,rotang,(colspace*fi2),pt2);
											sds_trans(pt2,&rbecs,&rbwcs,0,rbp->resval.rpoint);
											bMod =true;
										}
									}
								if ( bMod == true && (sds_entmod(tmprb))!=RTNORM)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
									}
								}
							}
							// EBATECH(CNBR)]-
						//*** Make the ATTRIBs
						// EBATECH(CNBR) -[ 2003/4/30 Bugzilla#78510 Explogin MINSERT
						// (1) Create SEQEND , (2) Copy XPLODE properties, (3) Move 1011 (4) Don't add ATTRIB to ssetout
				        // Copy inherit properties to ATTRIB's.
						if (attribflg && (fi1+fi2)==0 && rbinherit != NULL)
							{
							if (sds_entnext_noxref(ename,enttmp2)!=RTNORM)
								{
								rc=(-__LINE__);
								goto bail;
								}
							for(;;)
								{
								IC_RELRB(tmprb);
								if ((tmprb=sds_entgetx(enttmp2,&app))==NULL)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
									}
								// Match properties for XPLODE.
								cmd_xplode_addinherit(tmprb,rbinherit);
								if (sds_entmod(tmprb)!=RTNORM)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
									}
								// Exit loop when SEQEND is created.
								if (ic_assoc(tmprb,0)!=0)
									{
									rc=(-__LINE__);
									goto bail;
									}
								if(strisame("SEQEND"/*DNT*/,ic_rbassoc->resval.rstring))
									{
									sds_entupd(enttmp); // Call with new INSERT's ename.
									break;
									}
								// Proceed next ATTRIB.
								if (sds_entnext_noxref(enttmp2,enttmp2)!=RTNORM)
									{
									rc=(-__LINE__);
									goto bail;
									}
								} // EndFor
							}
						// Create ATTRIB for new INSERT.
						if (attribflg && (fi1+fi2)>0)
							{
							if (sds_entnext_noxref(ename,enttmp2)!=RTNORM)
								{
								rc=(-__LINE__);
								goto bail;
								}
							for(;;)
								{
								IC_RELRB(tmprb);
								if ((tmprb=sds_entgetx(enttmp2,&app))==NULL)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
									}
								//*** Increment the insertion point (10,11 and 1011).
								for(rbp=tmprb;rbp;rbp=rbp->rbnext)
									{
										if(rbp->restype == 10 || rbp->restype == 11) /*ECS*/
										{
											sds_polar(rbp->resval.rpoint,rotang+(IC_PI/2.0),(rowspace*fi1),pt2);
											sds_polar(pt2,rotang,(colspace*fi2),rbp->resval.rpoint);
										}
										if(rbp->restype == 1011) /*WCS*/
										{
											sds_trans(rbp->resval.rpoint,&rbwcs,&rbecs,0,pt2);
											sds_polar(pt2,rotang+(IC_PI/2.0),(rowspace*fi1),pt3);
											sds_polar(pt3,rotang,(colspace*fi2),pt2);
											sds_trans(pt2,&rbecs,&rbwcs,0,rbp->resval.rpoint);
										}
									}
								if (sds_entmake(tmprb)!=RTNORM)
									{
									rc=(-__LINE__);
									goto bail;
									}
								// Exit loop when SEQEND is created.
								if (ic_assoc(tmprb,0)!=0)
									{
									rc=(-__LINE__);
									goto bail;
									}
								if(strisame("SEQEND"/*DNT*/,ic_rbassoc->resval.rstring))
									{
									sds_entupd(enttmp); // Call with new INSERT's ename.
									break;
									}
								// Proceed next ATTRIB.
								if (sds_entnext_noxref(enttmp2,enttmp2)!=RTNORM)
									{
									rc=(-__LINE__);
									goto bail;
									}
								} // EndFor
								// ]- EBATECH(CNBR) 2003/4/30 ]-
							IC_RELRB(tmprb);
							}
						}
					}
				}
			else
				{
				//explode the insert

				subEntities = NULL;

				//EBATECH(FUTA)-[ 2001-01-15
				sds_name ss3,enttmp3;
				sds_ssadd(NULL,NULL,ss3);// ]-EBATECH(FUTA)

				// Ebatech(CNBR) -[ Bugzilla#78090 correct non-uniform inserts to error block.
				sds_name ss4;
				sds_ssadd(NULL,NULL,ss4);
				// EBATECH(CNBR) ]-

				ReactorFixups reactorFixup;
				SDS_CURDWG->SetCopyingFlag(true);

                do // the loop through the entities in the block
				{
/*D.Gavrilov*/// We should explode a LWPOLYLINE in the block if abs(xscl)!=abs(yscl).
// I've taken this code from the exploding of a lwpolylines (see below) considering
// that we shouldn't delete the lwpolyline from the block.
// Also we need some temp vars.
				    struct resbuf app_tmp;
					sds_point inspt_save, pt210_save;
					app_tmp.restype=RTSTR; app_tmp.resval.rstring="*"/*DNT*/ ; app_tmp.rbnext=NULL;
					/*DG - 15.5.2002*/// If the 1st subent is deleted then go to the next one (if any).
					if( reinterpret_cast<db_handitem*>(enttmp[0])->ret_deleted() &&
						sds_entnext_noxref(enttmp, enttmp) != RTNORM )
						break;
					if((subEntities=sds_entgetx(enttmp,&app_tmp))==NULL)
					{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__); goto bail;
					}
					if (ic_assoc(subEntities,0)!=0)
					{
						rc=(-__LINE__);
						goto bail;
					}

					if(	strsame("LWPOLYLINE", ic_rbassoc->resval.rstring) &&
						( (fabs(pt210[0]) > IC_ZRO && fabs(yscl - zscl) > IC_ZRO) ||
						  (fabs(pt210[1]) > IC_ZRO && fabs(xscl - zscl) > IC_ZRO) ||
						  (fabs(pt210[2]) > IC_ZRO && fabs(xscl - yscl) > IC_ZRO) )
					  )
					{
						ic_ptcpy(inspt_save, inspt);
						ic_ptcpy(pt210_save, pt210);

						db_lwpolyline *lwpl=(db_lwpolyline *)enttmp[0];

						//NOTE:  if spline vertices have been added, they will be lost!
						if (ic_assoc(subEntities,70)==0)
							plineflag=ic_rbassoc->resval.rint;
						else
							plineflag=0;
						rbecs.restype=rbwcs.restype=RTSHORT;
						rbecs.resval.rint=rbwcs.resval.rint=0;
						pt210[0]=pt210[1]=0.0;
						pt210[2]=1.0;

						//LAYER is inheritable in xplode, but we'll put it in anyway and subtitute later.
						//because we need to at least have one resbuf in new ent's llist
						if(ic_assoc(subEntities,8)!=0)
						{
							rc=(-__LINE__);
							goto bail;
						}
						if((elbeg=elbegt=sds_buildlist(8,ic_rbassoc->resval.rstring,67,psflag,0))==NULL)
						{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-__LINE__);
							goto bail;
						}
						//walk elcur to end of llist
						for(elcur=elbeg; elcur!=NULL && elcur->rbnext!=NULL; elcur=elcur->rbnext) ;

						//now loop and pick off necessary data from entity
						for(tmprb=subEntities;tmprb!=NULL;tmprb=tmprb->rbnext)
						{
							if(tmprb->restype==210)
							{
								ic_ptcpy(pt210,tmprb->resval.rpoint);//needed for line extru
								// EBATECH(CNBR) 2003/4/30 Here comes only LWPOLYLINE, so it isn't 3D polyline.
								//if(!(plineflag&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
								//{
									rbecs.restype=RT3DPOINT;
									ic_ptcpy(rbecs.resval.rpoint,tmprb->resval.rpoint);
								//}
							}
							//Linetype is inheritable in xplode command, so only add it if we're not going to replace later
							if(tmprb->restype==6 && 0==(inheritmode&8))
							{
								if((elcur=elcur->rbnext=sds_buildlist(6,tmprb->resval.rstring,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
							}
							//ltscale is inheritable
							if(tmprb->restype==48 && 0==(inheritmode&16))
							{
								if((elcur=elcur->rbnext=sds_buildlist(48,tmprb->resval.rreal,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
							}
							if(tmprb->restype==39)
							{
								if((elcur=elcur->rbnext=sds_buildlist(39,tmprb->resval.rreal,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
							}
							//color is also inheritable in xplode
							if(tmprb->restype==62 && 0==(inheritmode&4))
							{
								if((elcur=elcur->rbnext=sds_buildlist(62,tmprb->resval.rint,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
							}
							// GC=67 spaces is already set in elbeg. 2003/4/30 Ebatech(CNBR)
							//if(tmprb->restype==67)
							//{
							//	if((elcur=elcur->rbnext=sds_buildlist(67,tmprb->resval.rint,0))==NULL)
							//	{
							//		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							//		rc=(-__LINE__);
							//		goto bail;
							//	}
							//}
						}
						if((elcur=elcur->rbnext=sds_buildlist(210,pt210,0))==NULL)
						{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-__LINE__);
							goto bail;
						}
						//add 210 to all lines and arcs, but not 3dfaces
						//if we're xploding and we have properties to inherit....
						if(rbinherit!=NULL)
							cmd_xplode_addinherit(elbeg,rbinherit);
						//we may have added items to the llist, so be sure to move elcur to new end
						for(;elcur->rbnext!=NULL;elcur=elcur->rbnext) ;
						sds_ssadd(NULL,NULL,ss2);//blank ss
						initflg=1;
						lastbulge=0.0;
						for(int i=0; i<lwpl->ret_90(); i++)
						{
// if seqend taken from here
							lwpl->get_10(pt2,i);
							if(initflg)
							{
								ic_ptcpy(firstpt,pt2);
								ic_ptcpy(lastpt,pt2);
								lwpl->get_42(&fr2,i);
								if(!icadRealEqual(fr2,0.0))
									lastbulge=fr2;
								initflg=0;
								continue;
							}

							//*** ~| TODO - I need to check the vertex flags for other than spline
							//*** Here comes LWPOLYLINE only, so don't worry about non-linear polyline..
							if(!icadRealEqual(lastbulge,0.0))
							{
								if(ic_bulge2arc(lastpt,pt2,fr2,inspt,&rad,&stang,&endang)==0)
								{
									//no need to convert arc's points as long as you make it w/correct 210
									inspt[2]=lastpt[2];
									if((elbeg=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0))==NULL)
									{
										cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
										rc=(-__LINE__);
										goto bail;
									}
									elbeg->rbnext=elbegt;
									if((elcur->rbnext=sds_buildlist(10,inspt,40,rad,50,stang,51,endang,0))==NULL)
									{
										cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
										rc=(-__LINE__);
										goto bail;
									}
								}
							}
							else
							{
								if((elbeg=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
								elbeg->rbnext=elbegt;
								//note: always keep pts in original coordinates, and only xform if a line is needed
								sds_trans(lastpt,&rbecs,&rbwcs,0,inspt);
								sds_trans(pt2,&rbecs,&rbwcs,0,pt3);
								if((elcur->rbnext=sds_buildlist(10,inspt,11,pt3,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
							}
							if(sds_entmake(elbeg)!=RTNORM)
							{
								rc=(-__LINE__);
								goto bail;
							}
							if(ssetout)
							{
								sds_name tmpename;
								sds_entlast(tmpename);
								sds_ssadd (tmpename, ssetout, ssetout);
							}
							sds_entlast(enttmp2);
							SDS_ssadd(enttmp2, ss2, ss2);

							/*D.Gavrilov*/// now let's make pline's segment as belonging to block,
							// then delete this segment and add the one made by cmd_explode_makesubents

							SDS_ssadd(enttmp2,ss3,ss3);	//EBATECH(FUTA) 2001-01-15

							IC_RELRB(subEntities);

							rc = cmd_explode_makesubents(enttmp2, inspt_save,
									blkinspt, xscl,yscl,zscl, rotang, pt210_save,
									psflag, rbinherit, &subEntities);
							if(rc<0) goto bail;
							rc = sds_entmake(subEntities);
							if(rc==RTNORM)
							{
								IC_RELRB(subEntities);
								if(ssetout)
								{
									sds_name tmpename;
									sds_entlast(tmpename);
									sds_ssadd (tmpename, ssetout, ssetout);
								}
								sds_entlast(enttmp3);		//EBATECH(FUTA)
								SDS_ssadd(enttmp3,ss3,ss3);	//EBATECH(FUTA)
							}
							// set reactor info. Maybe we should do it for all the pline?
							db_handitem *oldEnt = (db_handitem*) enttmp2[0];
							reactorFixup.AddHanditem(SDS_LastHandItem);
							reactorFixup.AddHandleMap(oldEnt->RetHandle(), SDS_LastHandItem->RetHandle());

							sds_entdel(enttmp2);

							lwpl->get_42(&fr2,i);
							lastbulge=fr2;
							ic_ptcpy(lastpt,pt2);
							//*** FREE the first link and set the beg pointer to the second link.
							elbeg->rbnext=NULL;
							IC_RELRB(elbeg);
							elbeg=elbegt;
							IC_RELRB(tmprb);
							IC_RELRB(elcur->rbnext);
						} // end of loop over verts
// if SEQEND
						if(plineflag & IC_PLINE_CLOSED)
						{
							if(icadRealEqual(lastbulge,0.0))
							{
								if((elbeg=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
								elbeg->rbnext=elbegt;
								sds_trans(lastpt,&rbecs,&rbwcs,0,inspt);
								sds_trans(firstpt,&rbecs,&rbwcs,0,pt3);
								if((elcur->rbnext=sds_buildlist(10,inspt,11,pt3,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
								}
							}
							else
							{
								//*** Create an arc because the last vertex had a bulge
								if((elbeg=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__); goto bail;
								}
								elbeg->rbnext=elbegt;
								if(ic_bulge2arc(lastpt,firstpt,lastbulge,pt2,&rad,&stang,&endang)!=0)
								{
									rc=(-__LINE__);
									goto bail;
								}
								if((elcur->rbnext=sds_buildlist(10,pt2,40,rad,50,stang,51,endang,0))==NULL)
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__); goto bail;
								}
							}
							if(sds_entmake(elbeg)!=RTNORM)
							{
								rc=(-__LINE__);
								goto bail;
							}
							if(ssetout)
							{
								sds_name tmpename;
								sds_entlast(tmpename);
								sds_ssadd (tmpename, ssetout, ssetout);
							}
							sds_entlast(enttmp2);
							SDS_ssadd(enttmp2, ss2, ss2);

							/*D.Gavrilov*/// now let's make pline's segment as belonging to block,
							// then delete this segment and add the one made by cmd_explode_makesubents

							SDS_ssadd(enttmp3,ss3,ss3);	//EBATECH(FUTA)
							IC_RELRB(subEntities);

							rc = cmd_explode_makesubents(enttmp2, inspt_save,
									blkinspt, xscl, yscl, zscl, rotang, pt210_save,
									psflag, rbinherit, &subEntities);
							if(rc<0) goto bail;
							rc = sds_entmake(subEntities);
							if(rc==RTNORM)
							{
								IC_RELRB(subEntities);
								if(ssetout)
								{
									sds_name tmpename;
									sds_entlast(tmpename);
									sds_ssadd (tmpename, ssetout, ssetout);
								}
								sds_entlast(enttmp3);			//EBATECH(FUTA)
								SDS_ssadd(enttmp3,ss3,ss3);	//EBATECH(FUTA)
							}
							// set reactor info. Maybe we should do it for all the pline?
							db_handitem *oldEnt = (db_handitem*) enttmp2[0];
							reactorFixup.AddHanditem(SDS_LastHandItem);
							reactorFixup.AddHandleMap(oldEnt->RetHandle(), SDS_LastHandItem->RetHandle());

							sds_entdel(enttmp2);
						}
// END OF IF SEQEND
						sds_ssfree(ss2);
						IC_RELRB(tmprb);
						IC_RELRB(elbeg);
						//end of regular pline section
						sds_relrb(elbeg);
						elbeg=tmprb=elbegt=NULL;
						IC_RELRB(subEntities);
						ic_ptcpy(inspt, inspt_save);
						ic_ptcpy(pt210, pt210_save);
					}
					else
					{

					IC_RELRB(subEntities);

					rc=cmd_explode_makesubents(enttmp,inspt,blkinspt,xscl,yscl,zscl,rotang,pt210,psflag,rbinherit, &subEntities);
					// EBATECH(CNBR) -[ 2003/4/30 Bugzilla#78090
					if( rc < 0 )
						{
						if( rc == -1 )
							{
							sds_ssadd(enttmp, ss4, ss4);
							goto skip1;
							}
						else
							goto bail;
						}
					// EBATECH(CNBR) ]-
					rc=RTNORM;
					if (subEntities)
						{
                        ic_assoc(subEntities, 0);
                        if (strisame(ic_rbassoc->resval.rstring, "DIMENSION"/*DNT*/))
                            {
                            struct sds_resbuf rbb;
                            SDS_getvar(NULL,DB_QDIMSTYLE,&rbb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

                            ic_assoc(subEntities, 3);
                            struct sds_resbuf *thisDimsStyle = sds_tblsearch("DIMSTYLE"/*DNT*/,ic_rbassoc->resval.rstring,0);
                            cmd_setActiveDimStyle (thisDimsStyle);
                            IC_RELRB(thisDimsStyle);

							//Modified Cybage SBD 13/11/2001 DD/MM/YYYY
							//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla
							bdimUpdate=TRUE;
                            rc = sds_entmake(subEntities);
							//Modified Cybage SBD 13/11/2001 DD/MM/YYYY
							//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla
							bdimUpdate=FALSE;
                            if (rc==RTNORM)
								{
								IC_RELRB(subEntities);
								if (ssetout)
									{
									sds_name tmpename;
									sds_entlast(tmpename);
									sds_ssadd (tmpename, ssetout, ssetout);
									}
								sds_entlast(enttmp3);			//EBATECH(FUTA)
								SDS_ssadd(enttmp3,ss3,ss3);	//EBATECH(FUTA)
								}

                            struct sds_resbuf *DimStyleToRestore = sds_tblsearch("DIMSTYLE"/*DNT*/,rbb.resval.rstring,0);
                            cmd_setActiveDimStyle (DimStyleToRestore);
                            IC_RELRB(DimStyleToRestore);
                            IC_FREE(rbb.resval.rstring);

                            }
                        else
                            {
							if(isdim)
							{
								int dwgVersion = IC_ACAD2004; 
								if (elist != NULL)
									dwgVersion = ((db_drawing*)elist->resval.rlname[1])->ret_FileVersion();

								if (dwgVersion > IC_ACAD14 && 0==ic_assoc(elist,8))
								{	
									char layerName[255];
									strcpy(layerName, ic_rbassoc->resval.rstring);

									// If this drawing is > R14, explode subentities on to the layer the
									// dimension was created in, NOT on layer zero as it is for <= R14.
									if (0==ic_assoc(subEntities,8))
									{
										if (!strisame(layerName, ic_rbassoc->resval.rstring))
										{
											delete [] ic_rbassoc->resval.rstring;
											ic_rbassoc->resval.rstring = new char[strlen(layerName) + 1];
											strcpy(ic_rbassoc->resval.rstring, layerName);
										}
									}
								}

								if (0!=ic_assoc(subEntities,62))
								{
									cmd_ErrorPrompt(CMD_ERR_CANTGETDATA,0);
									rc=(-__LINE__);
									goto bail;
								}
								if( (ic_rbassoc->resval.rint==DB_BBCOLOR) && (0==(inheritmode&4)) )//if byblock then get parents colour
								{
									struct resbuf * rbtemp=ic_rbassoc;
									if(0!=ic_assoc(elist,62))
									{
										cmd_ErrorPrompt(CMD_ERR_CANTGETDATA,0);
										rc=(-__LINE__);
										goto bail;
									}
									rbtemp->resval.rint=ic_rbassoc->resval.rint;
								}

								if(0!=ic_assoc(subEntities,6))
								{
									cmd_ErrorPrompt(CMD_ERR_CANTGETDATA,0);
									rc=(-__LINE__);
									goto bail;
								}
								if( strisame(ic_rbassoc->resval.rstring,"BYBLOCK"/*DNT*/) && (0==(inheritmode&8)) )//if byblock then get the parents linetype
								{
									struct resbuf * rbtemp=ic_rbassoc;
									if(0!=ic_assoc(elist,6))
									{
										cmd_ErrorPrompt(CMD_ERR_CANTGETDATA,0);
										rc=(-__LINE__);
										goto bail;
									}
									if(NULL==(rbtemp->resval.rstring=ic_realloc_char(rbtemp->resval.rstring,strlen(ic_rbassoc->resval.rstring)+1)))
									{
										cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
										rc=(-__LINE__);
										goto bail;
									}
									strcpy(rbtemp->resval.rstring,ic_rbassoc->resval.rstring);
								}

							}
							//Modified Cybage SBD 22/10/2001 DD/MM/YYYY ]
						    //*** Make it.
						    rc = sds_entmake(subEntities);
						    if (rc==RTNORM)
								{
								IC_RELRB(subEntities);
								if (ssetout)
									{
									sds_name tmpename;
									sds_entlast(tmpename);
									sds_ssadd (tmpename, ssetout, ssetout);
									}
								sds_entlast(enttmp3);			//EBATECH(FUTA)
								SDS_ssadd(enttmp3,ss3,ss3);	//EBATECH(FUTA)
								}
                            }
						}

						if(rc != RTNORM)
							goto bail;

						// set reactor info
						db_handitem *oldEnt = (db_handitem*) enttmp[0];
						reactorFixup.AddHanditem(SDS_LastHandItem);
						reactorFixup.AddHandleMap(oldEnt->RetHandle(), SDS_LastHandItem->RetHandle());
					}
skip1:;
				} // the end of the loop through the entities in the block
				while(RTNORM==sds_entnext_noxref(enttmp,enttmp));

				//EBATECH(CNBR)-[ 2003/4/30 Bugzilla#78090 Pack non-uniform scale insert.
				{
					int rc1;
					long l, lCnt;
					sds_name bname, ename1;
					char* blkname=NULL;
					sds_resbuf* ptr=NULL;
					rc1 = sds_sslength(ss4, &lCnt);
					if( rc1 == RTNORM && lCnt > 0 )
					{
						sds_printf(ResourceString(IDC_EXPLODE_DIFFSCALE_INSERT, "\nCannot explode %d block(s)."), lCnt);
						if(( ptr = sds_buildlist( RTDXF0, "BLOCK", 2, "*E", 10, blkinspt, NULL )) == NULL )
						{ rc1 = RTERROR; goto skip2; }
						if(( rc1 = sds_entmakex( ptr, bname )) != RTNORM ) goto skip2;
						IC_RELRB(ptr);
						for( l = 0 ; l < lCnt ; ++l )
						{
							IC_RELRB(ptr);
							sds_ssname(ss4, l, ename1 );
							if(( ptr = sds_entgetx( ename1, &app )) == NULL )
							{ rc1 = RTERROR; goto skip2; }
							if(( rc1 = sds_entmake( ptr )) != RTNORM ) goto skip2;
						}
						IC_RELRB(ptr);
						if(( ptr = sds_buildlist( RTDXF0, "ENDBLK", NULL )) == NULL )
							{ rc1 = RTERROR; goto skip2; }
						if(( rc1 = sds_entmake( ptr )) != RTKWORD ) goto skip2;
						rc1 = RTNORM;
						IC_RELRB(ptr);
						if(( ptr = sds_entget(bname)) == NULL )
							{ rc1 = RTERROR; goto skip2; }
						ic_assoc( ptr, 2);
						blkname = NULL;
						db_astrncpy(&blkname, ic_rbassoc->resval.rstring, -1);
						if(blkname == NULL ) goto skip2;
						IC_RELRB(ptr);
						if(( ptr = sds_entgetx(ename, &app )) == NULL )
							{ rc1 = RTERROR; goto skip2; }
						ic_assoc( ptr, 2);
						ic_rbassoc->resval.rstring = (char*)sds_realloc(ic_rbassoc->resval.rstring, strlen(blkname)+1);
						strcpy(ic_rbassoc->resval.rstring,blkname);
						if(( rc1 = sds_entmakex(ptr, bname)) != RTNORM ) goto skip2;
						sds_ssadd(bname,ss3,ss3);
				skip2:
						if( blkname ) IC_FREE(blkname);
						IC_RELRB(ptr);
					}
					sds_ssfree(ss4);
					if( rc1 != RTNORM )
					{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
					}
				}
				//EBATECH(CNBR)]-

				sds_entdel(ename);	/*D.Gavrilov*///delete the insert on succesful exploding
				//EBATECH(FUTA)-[ 2001-01-15
				//sds_redraw(NULL,0);

				reactorFixup.ApplyFixups(SDS_CURDWG);
				SDS_CURDWG->SetCopyingFlag(false);
				for (fi1=0;sds_ssname(ss3,fi1,enttmp3)==RTNORM;fi1++)
					{
					// Set the Undo so we corectly redraw the entity Redo.
					SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)enttmp3[0],NULL,NULL,SDS_CURDWG);
					sds_redraw(enttmp3,IC_REDRAW_DRAW);
					}
				sds_ssfree(ss3);
				// ]- EBATECH(FUTA)

				}

            IC_RELRB(tmprb);
			}
		else if (strsame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring))
			{

			// Set the Undo so we corectly redraw the entity Undo.
			SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)ename[0],NULL,NULL,SDS_CURDWG);

			pdel=0;//flag to see if we want to delete the pline
			//NOTE:  if spline vertices have been added, they will be lost!
            if (ic_assoc(elist,70)==0)
                plineflag=ic_rbassoc->resval.rint;
            else
				plineflag=0;
			rbecs.restype=rbwcs.restype=RTSHORT;
			rbecs.resval.rint=rbwcs.resval.rint=0;
			pt210[0]=pt210[1]=0.0;
			pt210[2]=1.0;

			//LAYER is inheritable in xplode, but we'll put it in anyway and subtitute later.
			//	because we need to at least have one resbuf in new ent's llist
			if (ic_assoc(elist,8)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}
			if ((elbeg=elbegt=sds_buildlist(8,ic_rbassoc->resval.rstring,67,psflag,0))==NULL)
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-__LINE__);
				goto bail;
				}
			//walk elcur to end of llist
			for (elcur=elbeg; elcur!=NULL && elcur->rbnext!=NULL; elcur=elcur->rbnext);

			//now loop and pick off necessary data from entity
			for (tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext)
				{
				if (tmprb->restype==210)
					{
					ic_ptcpy(pt210,tmprb->resval.rpoint);//needed for line extru
					if (!(plineflag&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
						{
						rbecs.restype=RT3DPOINT;
						ic_ptcpy(rbecs.resval.rpoint,tmprb->resval.rpoint);
						}
					}
				//Linetype is inheritable in xplode command, so only add it if we're not going to replace later
				if (tmprb->restype==6 && 0==(inheritmode&8))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(6,tmprb->resval.rstring,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				//ltscale is inheritable
				if (tmprb->restype==48 && 0==(inheritmode&16))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(48,tmprb->resval.rreal,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (tmprb->restype==39)
					{
					if ((elcur=elcur->rbnext=sds_buildlist(39,tmprb->resval.rreal,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				//color is also inheritable in xplode
				if (tmprb->restype==62 && 0==(inheritmode&4))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(62,tmprb->resval.rint,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (tmprb->restype==67)
					{
					if ((elcur=elcur->rbnext=sds_buildlist(67,tmprb->resval.rint,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				}
			if (!(plineflag & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)))
				{
				if ((elcur=elcur->rbnext=sds_buildlist(210,pt210,0))==NULL)
					{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					rc=(-__LINE__);
					goto bail;
					}
				}//add 210 to all lines and arcs, but not 3dfaces
			//if we're xploding and we have properties to inherit....
			if (rbinherit!=NULL)
				cmd_xplode_addinherit(elbeg,rbinherit);
			//we may have added items to the llist, so be sure to move elcur to new end
			for (;elcur->rbnext!=NULL;elcur=elcur->rbnext);
			if (!(plineflag & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)))	//if it's not a 3d mesh or pface mesh...
				{
				//sds_entdel(ename);
				sds_ssadd(NULL,NULL,ss2);//blank ss
				ic_encpy(enttmp,ename);
				initflg=1;
				lastbulge=0.0;
				while(sds_entnext_noxref(enttmp,enttmp)==RTNORM)
					{
					if ((tmprb=sds_entgetx(enttmp,&app))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					if (ic_assoc(tmprb,0)!=0)
						{
						rc=(-__LINE__);
						goto bail;
						}
					if (strsame("SEQEND"/*DNT*/,ic_rbassoc->resval.rstring))
						{
						if (plineflag & IC_PLINE_CLOSED)
							{
							if (icadRealEqual(lastbulge,0.0))
								{
								if ((elbeg=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),0))==NULL)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
									}
								elbeg->rbnext=elbegt;
								sds_trans(lastpt,&rbecs,&rbwcs,0,inspt);
								sds_trans(firstpt,&rbecs,&rbwcs,0,pt3);
								if ((elcur->rbnext=sds_buildlist(10,inspt,11,pt3,0))==NULL)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__);
									goto bail;
									}
								}
							else
								{
								//*** Create an arc because the last vertex had a bulge
                               if ((elbeg=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0))==NULL)
								   {
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__); goto bail;
									}
								elbeg->rbnext=elbegt;
								if (ic_bulge2arc(lastpt,firstpt,lastbulge,pt2,&rad,&stang,&endang)!=0)
									{
									rc=(-__LINE__);
									goto bail;
									}
								if ((elcur->rbnext=sds_buildlist(10,pt2,40,rad,50,stang,51,endang,0))==NULL)
									{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									rc=(-__LINE__); goto bail;
									}
								}
							if (sds_entmake(elbeg)!=RTNORM)
								{
								rc=(-__LINE__);
								goto bail;
								}
							if (ssetout)
								{
								sds_name tmpename;
								sds_entlast(tmpename);
								sds_ssadd (tmpename, ssetout, ssetout);
								}
							sds_entlast(enttmp2);
							SDS_ssadd(enttmp2, ss2, ss2);
							pdel=1;
							}
						break;
						}
					if (plineflag & IC_PLINE_SPLINEFIT)	//spline fit pline...
						{
						if (ic_assoc(tmprb,70)!=0)
							{
							rc=(-__LINE__);
							goto bail;
							}
						if (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME)
							{
							//don't include control frame's vertices
							//elbeg->rbnext=NULL; IC_RELRB(elbeg); elbeg=elbegt;
							IC_RELRB(tmprb);
							IC_RELRB(elcur->rbnext);
							continue;
							}
						}
					if (ic_assoc(tmprb,10)!=0)
						{
						rc=(-__LINE__);
						goto bail;
						}
					ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
					if (initflg)
						{
						ic_ptcpy(firstpt,pt2);
						ic_ptcpy(lastpt,pt2);
						if (ic_assoc(tmprb,42)==0 && !icadRealEqual((fr2=ic_rbassoc->resval.rreal),0.0))
							lastbulge=fr2;
						IC_RELRB(tmprb);
						initflg=0; continue;
						}

					//*** ~| TODO - I need to check the vertex flags for other than spline
					if (!icadRealEqual(lastbulge,0.0))
						{
						if (ic_bulge2arc(lastpt,pt2,fr2,inspt,&rad,&stang,&endang)==0)
							{
							//no need to convert arc's points as long as you make it w/correct 210
							inspt[2]=lastpt[2];
							if ((elbeg=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0))==NULL)
								{
								cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
								rc=(-__LINE__);
								goto bail;
								}
							elbeg->rbnext=elbegt;
							if ((elcur->rbnext=sds_buildlist(10,inspt,40,rad,50,stang,51,endang,0))==NULL)
								{
								cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
								rc=(-__LINE__);
								goto bail;
								}
							}
						}
					else
						{
						if ((elbeg=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),0))==NULL)
							{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-__LINE__);
							goto bail;
							}
						elbeg->rbnext=elbegt;
						//note: always keep pts in original coordinates, and only xform if a line is needed
						sds_trans(lastpt,&rbecs,&rbwcs,0,inspt);
						sds_trans(pt2,&rbecs,&rbwcs,0,pt3);
						if ((elcur->rbnext=sds_buildlist(10,inspt,11,pt3,0))==NULL)
							{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-__LINE__);
							goto bail;
							}
						}
					if (sds_entmake(elbeg)!=RTNORM)
						{
						rc=(-__LINE__);
						goto bail;
						}
					if (ssetout)
						{
						sds_name tmpename;
						sds_entlast(tmpename);
						sds_ssadd (tmpename, ssetout, ssetout);
						}
					pdel=1;
					sds_entlast(enttmp2);
					SDS_ssadd(enttmp2, ss2, ss2);
					if (ic_assoc(tmprb,42)==0 && !icadRealEqual((fr2=ic_rbassoc->resval.rreal),0.0))
						lastbulge=fr2;
					else
						lastbulge=0.0;
					ic_ptcpy(lastpt,pt2);
					//*** FREE the first link and set the beg pointer to the second link.
					elbeg->rbnext=NULL;
					IC_RELRB(elbeg);
					elbeg=elbegt;
					IC_RELRB(tmprb);
					IC_RELRB(elcur->rbnext);
					}
				if (pdel)
					{
					sds_entdel(ename);
					for (fi1=0;sds_ssname(ss2,fi1,enttmp2)==RTNORM;fi1++)
						{
						// Set the Undo so we corectly redraw the entity Redo.
						SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)enttmp2[0],NULL,NULL,SDS_CURDWG);
						sds_redraw(enttmp2,IC_REDRAW_DRAW);
						}
					}
				sds_ssfree(ss2);
				IC_RELRB(tmprb);
				IC_RELRB(elbeg);
				//end of regular pline section
				}
			else if (plineflag & IC_PLINE_3DMESH)
				{
				//A Polygon mesh.  No transformations needed.  line pts are wcs, as are pline pts
				if (ic_assoc(elist, 71))
					{
					ret=RTERROR;
					goto bail;
					}
				fi1=ic_rbassoc->resval.rint;
				if (ic_assoc(elist, 72))
					{
					ret=RTERROR;
					goto bail;
					}
				fi2=ic_rbassoc->resval.rint;
				if ((elbeg=sds_buildlist(RTDXF0,"3DFACE"/*DNT*/,10,firstpt,11,firstpt,12,firstpt,13,firstpt,70,0,0))==NULL)
					{
					ret=RTERROR;
					goto bail;
					}
				FreeResbufIfNotNull(&tmprb);
				for (tmprb=elbeg;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
				tmprb->rbnext=elbegt;
				tmprb=elbegt=NULL;
				if ((ret=cmd_explode_plmesh(elbeg,ename,fi1,fi2,plineflag, ssetout))!=RTNORM)
					goto bail;
					//NOTE:sds_entdel(ename) performed by subroutine if ret==RTNORM
				}
			else if (plineflag & IC_PLINE_POLYFACEMESH)
				{
				//A polyface mesh...
				if ((elbeg=sds_buildlist(RTDXF0,"3DFACE"/*DNT*/,10,firstpt,11,firstpt,12,firstpt,13,firstpt,70,0,0))==NULL)
					{
					ret=RTERROR;
					goto bail;
					}
				FreeResbufIfNotNull(&tmprb);
				for (tmprb=elbeg;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
				tmprb->rbnext=elbegt;
				tmprb=elbegt=NULL;
				if ((ret=cmd_explode_pfacemesh(elbeg,ename, ssetout))!=RTNORM)
					goto bail;
				//NOTE:sds_entdel(ename) performed by subroutine if ret==RTNORM
				}
			sds_relrb(elbeg);
			elbeg=tmprb=elbegt=NULL;
	        }
		else if (strsame("LWPOLYLINE"/*DNT*/,ic_rbassoc->resval.rstring))
			{
			db_lwpolyline *lwpl=(db_lwpolyline *)ename[0];

			// Set the Undo so we corectly redraw the entity Undo.
			SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)ename[0],NULL,NULL,SDS_CURDWG);

			pdel=0;//flag to see if we want to delete the pline
			//NOTE:  if spline vertices have been added, they will be lost!
            if (ic_assoc(elist,70)==0)
                plineflag=ic_rbassoc->resval.rint;
            else
				plineflag=0;
			rbecs.restype=rbwcs.restype=RTSHORT;
			rbecs.resval.rint=rbwcs.resval.rint=0;
			pt210[0]=pt210[1]=0.0;
			pt210[2]=1.0;

			//LAYER is inheritable in xplode, but we'll put it in anyway and subtitute later.
			//	because we need to at least have one resbuf in new ent's llist
			if (ic_assoc(elist,8)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}
			if ((elbeg=elbegt=sds_buildlist(8,ic_rbassoc->resval.rstring,67,psflag,0))==NULL)
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-__LINE__);
				goto bail;
				}
			//walk elcur to end of llist
			for (elcur=elbeg; elcur!=NULL && elcur->rbnext!=NULL; elcur=elcur->rbnext);

			//now loop and pick off necessary data from entity
			for (tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext)
				{
				if (tmprb->restype==210)
					{
					ic_ptcpy(pt210,tmprb->resval.rpoint);//needed for line extru
					if (!(plineflag&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
						{
						rbecs.restype=RT3DPOINT;
						ic_ptcpy(rbecs.resval.rpoint,tmprb->resval.rpoint);
						}
					}
				//Linetype is inheritable in xplode command, so only add it if we're not going to replace later
				if (tmprb->restype==6 && 0==(inheritmode&8))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(6,tmprb->resval.rstring,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				//ltscale is inheritable
				if (tmprb->restype==48 && 0==(inheritmode&16))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(48,tmprb->resval.rreal,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (tmprb->restype==39)
					{
					if ((elcur=elcur->rbnext=sds_buildlist(39,tmprb->resval.rreal,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				//color is also inheritable in xplode
				if (tmprb->restype==62 && 0==(inheritmode&4))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(62,tmprb->resval.rint,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (tmprb->restype==67)
					{
					if ((elcur=elcur->rbnext=sds_buildlist(67,tmprb->resval.rint,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				}
			if ((elcur=elcur->rbnext=sds_buildlist(210,pt210,0))==NULL)
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-__LINE__);
				goto bail;
				}
			//add 210 to all lines and arcs, but not 3dfaces
			//if we're xploding and we have properties to inherit....
			if (rbinherit!=NULL)
				cmd_xplode_addinherit(elbeg,rbinherit);
			//we may have added items to the llist, so be sure to move elcur to new end
			for (;elcur->rbnext!=NULL;elcur=elcur->rbnext);
			//sds_entdel(ename);
			sds_ssadd(NULL,NULL,ss2);//blank ss
			ic_encpy(enttmp,ename);
			initflg=1;
			lastbulge=0.0;
			for (int i=0; i<lwpl->ret_90(); i++)
				{
// if seqend taken from here
				lwpl->get_10(pt2,i);
				if (initflg)
					{
					ic_ptcpy(firstpt,pt2);
					ic_ptcpy(lastpt,pt2);
					lwpl->get_42(&fr2,i);
					if (!icadRealEqual(fr2,0.0))
						lastbulge=fr2;
					initflg=0;
					continue;
					}

				//*** ~| TODO - I need to check the vertex flags for other than spline
				if (!icadRealEqual(lastbulge,0.0))
					{
					if (ic_bulge2arc(lastpt,pt2,fr2,inspt,&rad,&stang,&endang)==0)
						{
						//no need to convert arc's points as long as you make it w/correct 210
						inspt[2]=lastpt[2];
						if ((elbeg=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0))==NULL)
							{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-__LINE__);
							goto bail;
							}
						elbeg->rbnext=elbegt;
						if ((elcur->rbnext=sds_buildlist(10,inspt,40,rad,50,stang,51,endang,0))==NULL)
							{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-__LINE__);
							goto bail;
							}
						}
					}
				else
					{
					if ((elbeg=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					elbeg->rbnext=elbegt;
					//note: always keep pts in original coordinates, and only xform if a line is needed
					sds_trans(lastpt,&rbecs,&rbwcs,0,inspt);
					sds_trans(pt2,&rbecs,&rbwcs,0,pt3);
					if ((elcur->rbnext=sds_buildlist(10,inspt,11,pt3,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (sds_entmake(elbeg)!=RTNORM)
					{
					rc=(-__LINE__);
					goto bail;
					}
				if (ssetout)
					{
					sds_name tmpename;
					sds_entlast(tmpename);
					sds_ssadd (tmpename, ssetout, ssetout);
					}
				pdel=1;
				sds_entlast(enttmp2);
				SDS_ssadd(enttmp2, ss2, ss2);
				lwpl->get_42(&fr2,i);
				if (!icadRealEqual(fr2,0.0))			// isn't this the same as saying lastbulge=fr2?
					lastbulge=fr2;
				else
					lastbulge=0.0;
				ic_ptcpy(lastpt,pt2);
				//*** FREE the first link and set the beg pointer to the second link.
				elbeg->rbnext=NULL;
				IC_RELRB(elbeg);
				elbeg=elbegt;
				IC_RELRB(tmprb);
				IC_RELRB(elcur->rbnext);
				} // end of loop over verts
// if SEQEND
			if (plineflag & IC_PLINE_CLOSED)
				{
				if (icadRealEqual(lastbulge,0.0))
					{
					if ((elbeg=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					elbeg->rbnext=elbegt;
					sds_trans(lastpt,&rbecs,&rbwcs,0,inspt);
					sds_trans(firstpt,&rbecs,&rbwcs,0,pt3);
					if ((elcur->rbnext=sds_buildlist(10,inspt,11,pt3,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				else
					{
					//*** Create an arc because the last vertex had a bulge
                   if ((elbeg=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0))==NULL)
					   {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__); goto bail;
						}
					elbeg->rbnext=elbegt;
					if (ic_bulge2arc(lastpt,firstpt,lastbulge,pt2,&rad,&stang,&endang)!=0)
						{
						rc=(-__LINE__);
						goto bail;
						}
					if ((elcur->rbnext=sds_buildlist(10,pt2,40,rad,50,stang,51,endang,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__); goto bail;
						}
					}
				if (sds_entmake(elbeg)!=RTNORM)
					{
					rc=(-__LINE__);
					goto bail;
					}
				if (ssetout)
					{
					sds_name tmpename;
					sds_entlast(tmpename);
					sds_ssadd (tmpename, ssetout, ssetout);
					}
				sds_entlast(enttmp2);
				SDS_ssadd(enttmp2, ss2, ss2);
				pdel=1;
				}
// END OF IF SEQEND

			if (pdel)
				{
				sds_entdel(ename);
				for (fi1=0;sds_ssname(ss2,fi1,enttmp2)==RTNORM;fi1++)
					{
					// Set the Undo so we corectly redraw the entity Redo.
					SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)enttmp2[0],NULL,NULL,SDS_CURDWG);
					sds_redraw(enttmp2,IC_REDRAW_DRAW);
					}
				}
			sds_ssfree(ss2);
			IC_RELRB(tmprb);
			IC_RELRB(elbeg);
			//end of regular pline section
			sds_relrb(elbeg);
			elbeg=tmprb=elbegt=NULL;
	        }
		else if (sds_wcmatch(ic_rbassoc->resval.rstring,"TRACE,SOLID,3DFACE"/*DNT*/)==RTNORM)
			{
			// Delete the ent first then create the ents in its place.
			//note: trace and solid are ecs, face is wcs.  we'll always make wcs lines
			strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
			sds_entdel(ename);

			rbecs.restype=rbwcs.restype=RTSHORT;
			rbecs.resval.rint=rbwcs.resval.rint=0;
			pt210[0]=pt210[1]=0.0;
			pt210[2]=1.0;

			if ((elbeg=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),67,psflag,0))==NULL)
				{
                cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
                rc=(-__LINE__);
				goto bail;
				}
            for (elcur=elbeg; elcur!=NULL && elcur->rbnext!=NULL; elcur=elcur->rbnext);
			for (tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext)
				{
				if (tmprb->restype==210)
					{
					ic_ptcpy(pt210,tmprb->resval.rpoint);
					if (!strsame(fs1,"3DFACE"/*DNT*/))
						{
						rbecs.restype=RT3DPOINT;
						ic_ptcpy(rbecs.resval.rpoint,tmprb->resval.rpoint);
						}
					}
				//layer's inheritable
				if (8==tmprb->restype)
					{
					if ((elcur=elcur->rbnext=sds_buildlist(8,tmprb->resval.rstring,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				//lt is inheritable
				if (6==tmprb->restype && 0==(inheritmode&8))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(6,tmprb->resval.rstring,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				//ltscale is inheritable
				if (48==tmprb->restype && 0==(inheritmode&16))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(48,tmprb->resval.rreal,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (39==tmprb->restype)
					{
					if ((elcur=elcur->rbnext=sds_buildlist(39,tmprb->resval.rreal,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (62==tmprb->restype && 0==(inheritmode&4))
					{
					if ((elcur=elcur->rbnext=sds_buildlist(62,tmprb->resval.rint,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				if (67==tmprb->restype)
					{
					if ((elcur=elcur->rbnext=sds_buildlist(67,tmprb->resval.rint,0))==NULL)
						{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__);
						goto bail;
						}
					}
				}
			//add 210 to all lines
			if ((elcur=elcur->rbnext=sds_buildlist(210,pt210,0))==NULL)
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-__LINE__);
				goto bail;
				}
			if (rbinherit!=NULL)
				cmd_xplode_addinherit(elist,rbinherit);
			fi1=0;
			for (tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext)
				{
				if (tmprb->restype==10)
					sds_trans(tmprb->resval.rpoint,&rbecs,&rbwcs,0,firstpt);
				if (tmprb->restype==11)
					sds_trans(tmprb->resval.rpoint,&rbecs,&rbwcs,0,lastpt);
				if (tmprb->restype==12)
					sds_trans(tmprb->resval.rpoint,&rbecs,&rbwcs,0,inspt);
				if (tmprb->restype==13)
					sds_trans(tmprb->resval.rpoint,&rbecs,&rbwcs,0,pt2);
				if (tmprb->restype==70)
					fi1=tmprb->resval.rint;
				}
			//make the rest of the list using first two pts
			if ((elcur=elcur->rbnext=sds_buildlist(10,firstpt,0))==NULL)
				{
                cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
                rc=(-__LINE__);
				goto bail;
				}
			elbegt=elcur;//save resbuf for later use
			if ((elcur=elcur->rbnext=sds_buildlist(11,lastpt,0))==NULL)
				{
                cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
                rc=(-__LINE__);
				goto bail;
	            }
			tmprb=elcur;//save resbuf for later use
			if ((0==(fi1 & 1)))
				{
				if (sds_entmake(elbeg)!=RTNORM)
					{
					ret=RTERROR;
					goto bail;
					}
				if (ssetout)
					{
					sds_name tmpename;
					sds_entlast(tmpename);
					sds_ssadd (tmpename, ssetout, ssetout);
					}
				}
			if (strsame(fs1,"TRACE"/*DNT*/) || 0==icadPointEqual(inspt,pt2))
				{
				if (0==(fi1 & 4))
					{
					ic_ptcpy(elbegt->resval.rpoint,inspt);
					ic_ptcpy(tmprb->resval.rpoint,pt2);
					if (sds_entmake(elbeg)!=RTNORM)
						{
						ret=RTERROR;
						goto bail;
						}
					if (ssetout)
						{
						sds_name tmpename;
						sds_entlast(tmpename);
						sds_ssadd (tmpename, ssetout, ssetout);
						}
					}
				}
			if (strsame(fs1,"3DFACE"/*DNT*/))
				{
				if (!(fi1 & 2))
					{
					ic_ptcpy(elbegt->resval.rpoint,lastpt);
					ic_ptcpy(tmprb->resval.rpoint,inspt);
					if (sds_entmake(elbeg)!=RTNORM)
						{
						ret=RTERROR;
						goto bail;
						}
					if (ssetout)
						{
						sds_name tmpename;
						sds_entlast(tmpename);
						sds_ssadd (tmpename, ssetout, ssetout);
						}
					}
				if (!(fi1 & 8))
					{
					ic_ptcpy(elbegt->resval.rpoint,pt2);
					ic_ptcpy(tmprb->resval.rpoint,firstpt);
					if (sds_entmake(elbeg)!=RTNORM)
						{
						ret=RTERROR;
						goto bail;
						}
					if (ssetout)
						{
						sds_name tmpename;
						sds_entlast(tmpename);
						sds_ssadd (tmpename, ssetout, ssetout);
						}
					}
				}
			else
				{
				ic_ptcpy(elbegt->resval.rpoint,lastpt);
				ic_ptcpy(tmprb->resval.rpoint,pt2);
				if (sds_entmake(elbeg)!=RTNORM)
					{
					ret=RTERROR;
					goto bail;
					}
				if (ssetout)
					{
					sds_name tmpename;
					sds_entlast(tmpename);
					sds_ssadd (tmpename, ssetout, ssetout);
					}
				ic_ptcpy(elbegt->resval.rpoint,firstpt);
				ic_ptcpy(tmprb->resval.rpoint,inspt);
				if (sds_entmake(elbeg)!=RTNORM)
					{
					ret=RTERROR;
					goto bail;
					}
				if (ssetout)
					{
					sds_name tmpename;
					sds_entlast(tmpename);
					sds_ssadd (tmpename, ssetout, ssetout);
					}
				}
			sds_relrb(elbeg);
			elbeg=tmprb=elbegt=NULL;
			}
         else if(strsame("HATCH"/*DNT*/, ic_rbassoc->resval.rstring))
		 {
            db_hatch* pHatch = (db_hatch*)ename[0];
			char* pHatchPattern;
			pHatch->get_2(&pHatchPattern);
            struct gr_displayobject *pDOListBegin = NULL, *pDOListEnd = NULL, *pDOCur = NULL;
			// DP: TODO: remove somewhen call for list since with 3D DOs they are stored in entity
			if(!strisame(pHatchPattern, "SOLID") && !SDS_getdispobjs(SDS_CURDWG, (db_handitem**)&pHatch,
				&pDOListBegin, &pDOListEnd, NULL, SDS_CURDWG, SDS_CURGRVW, 0, NULL))
			{
				pHatch = (db_hatch*)ename[0];
				int color;
				pHatch->get_62(&color);
				char* pLinetype;
				pHatch->get_6(&pLinetype);
				char* pLayer;
				pHatch->get_8(&pLayer);

				struct resbuf wcs, ucs;
				wcs.restype = RTSHORT;
				wcs.resval.rint = 0;
				// returned display objects are in UCS
				ucs.restype = RTSHORT;
				ucs.resval.rint = 1;

				sds_point prevPoint, curPoint;
				struct resbuf* pLine;
				for(pDOCur = pDOListBegin; pDOCur != NULL; pDOCur = pDOCur->next)
				{
					ASSERT(pDOCur->type & DISP_OBJ_PTS_3D);
					if(!(pDOCur->type & DISP_OBJ_PTS_3D) || pDOCur->npts < 1 || pDOCur->chain == NULL)
						continue;

					sds_trans(&pDOCur->chain[0], &ucs, &wcs, 0, prevPoint);
					for(int i = 1; i < pDOCur->npts; i++)
					{
						sds_trans(&pDOCur->chain[3 * i], &ucs, &wcs, 0, curPoint);
						pLine = sds_buildlist(RTDXF0, "LINE", 8, pLayer, 62, color, 6, pLinetype,
								10, prevPoint, 11, curPoint, RTNONE);
						sds_entmake(pLine);
						sds_relrb(pLine);
						ic_ptcpy(prevPoint, curPoint);
					}
				}
				gr_freedisplayobjectll(pDOListBegin);
				sds_entdel(ename);
            }
         }
            //<ALM>
            //else if (sds_wcmatch(ic_rbassoc->resval.rstring,"3DSOLID") == RTNORM)
            else if (strisame(ic_rbassoc->resval.rstring,"3DSOLID"))	// EBATECH(CNBR) wcmatch->strisame
            {
                CModeler* pModeler = CModeler::get();
                // Explode 3DSOLID into BODIES and REGIONS
                if (pModeler->canModify())
                {
                    CDbAcisEntity* pObj = static_cast<CDbAcisEntity*>(reinterpret_cast<db_handitem*>(ename[0]));

                    char* szLayer = 0;
                    pObj->get_8(&szLayer);

                    // Get parent's color
                    int color = -1;
                    if (!(inheritmode&4))
                    {
                        pObj->get_62(&color);
                    }

                    xstd<CDbAcisEntityData>::vector objs;
                    if (pModeler->explodeIntoBodies(pObj->getData(), objs))
                    {
						if (createBodiesOrRegions(objs, szLayer, color))
						{
							sds_entdel(ename);
						}
                    }
                }
            }
            else if (sds_wcmatch(ic_rbassoc->resval.rstring,"REGION,BODY") == RTNORM)
            {
                CModeler* pModeler = CModeler::get();
                // If REGION has several faces, explode it into separate faces (REGIONs)
                // Else explode it on boundary curves
                if (pModeler->canModify())
                {
                    CDbAcisEntity* pObj = static_cast<CDbAcisEntity*>(reinterpret_cast<db_handitem*>(ename[0]));

                    char* szLayer;
                    pObj->get_8(&szLayer);

                    // Get parent's color
                    int color = -1;
                    if (!(inheritmode&4))
                    {
                        pObj->get_62(&color);
                    }

                    do
                    {
                        int nfaces = 0;
                        if (!pModeler->getNumberOfFaces(pObj->getData(), nfaces))
                            break;

                        if (nfaces == 1)
                        {
                            xstd<geodata::curve*>::vector curves;
                            if (!pModeler->explodeIntoCurves(pObj->getData(), curves))
                                break;

                            if (sds_entdel(ename) == RTNORM)
                                createCurves(curves, szLayer, color);

                            {
                                xstd<geodata::curve*>::vector::iterator icrv = curves.begin();
                                for (; icrv != curves.end(); ++icrv)
                                    pModeler->free(*icrv);
                            }
                        }
                        else
                        {
                            xstd<CDbAcisEntityData>::vector objs;
                            xstd<bool>::vector isregion;
                            if (!pModeler->explodeIntoBodies(pObj->getData(), objs))
                                break;

                            if (sds_entdel(ename) == RTNORM)
                                createBodiesOrRegions(objs, szLayer, color);
                        }
                    }
                    while (false);
                }
            }
            //</ALM>

            // SystemMetrix(watanabe)-[explode LEADER
            //else if (sds_wcmatch(ic_rbassoc->resval.rstring, "LEADER") == RTNORM)
            else if (strisame(ic_rbassoc->resval.rstring,"LEADER"))	// EBATECH(CNBR) wcmatch->strisame
              {
                int irt = ExplodeLeader(ename, elist, psflag);
                if (irt == FALSE)
                  {
                    rc = (-__LINE__);
                    goto bail;
                  }
              }
            // ]-SystemMetrix(watanabe) explode LEADER
            // EBATECH(CNBR)-[ 04/02/2002 explode MTEXT
            else if (strisame(ic_rbassoc->resval.rstring, "MTEXT"))
              {
				CDC *pDC = SDS_CURVIEW->GetTargetDeviceCDC();
				std::vector<long> hips;
				sds_ssadd(NULL, NULL, ss2 );
                int irt = ExplodeMtext(ename, hips, SDS_CURGRVW, SDS_CURDWG, pDC);
                if (irt == FALSE)
                  {
                    rc = (-__LINE__);
                    goto bail;
                  }
                else
                {
                    if (sds_entdel(ename) == RTNORM)
					{
						sds_resbuf* plist = NULL;
						std::vector<long>::iterator ihips = hips.begin();
						sds_name ename2;

						for(; ihips != hips.end() ; ihips++ )
						{
							plist = (sds_resbuf*)(*ihips);
							if( sds_entmake(plist) == RTNORM )
							{
								sds_entlast(ename2);
								if (ssetout)
								{
									sds_ssadd (ename2, ssetout, ssetout);
								}
							}
							IC_RELRB(plist);
						}
					}
				}
              }
            // ]-EBATECH(CNBR) 04/02/2002 explode MTEXT
        IC_FREEIT
		}

    bail:

    IC_RELRB(filter);
	IC_RELRB(tmprb);
	IC_RELRB(elbeg);
	IC_RELRB(rbinherit);
	IC_FREEIT
    sds_ssfree(ss1);
	sds_entmake(NULL);

    if ((0L == rc) || (RTNORM == rc))
		{
		if (SDS_CURDWG->FixingUp1005s())
			{
			SDS_CURDWG->Apply1005Fixups();
			SDS_CURDWG->Delete1005FixupTables();
			}

		return(RTNORM);
		}

	if (SDS_CURDWG->FixingUp1005s())
		SDS_CURDWG->Delete1005FixupTables();

    if (rc==(-1L))
		return(RTCAN);
    if (rc<(-1L))
		CMD_INTERNAL_MSG(rc);
    return(RTERROR);
}

int cmd_xplode_getinheritmode(int *mode, sds_resbuf **rbprop){
	//does all the grunt work for xplode and alloc's llist of properties to change.
	//caller must free memory, and passed *resbuf must be NULL
	int inheritmode,rc=0,ret;
	struct resbuf *rbinherit,rb,*tmprb=NULL;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];

	if ((*rbprop)!=NULL){rc=-1L;goto bail;}
	inheritmode=*mode;
	rbinherit=*rbprop;

	do{
		*fs1=0;
		if (0==(inheritmode&64)){
           sds_initget(0,ResourceString(IDC_EXPLODE_INITGET_ALL_LIST_5, "All_Listed|All Multiple ~ LAyer Color LType ltScale ~ Inherit_All_from_Parent|Inherit ~_All ~_Multiple ~_ ~_LAyer ~_Color ~_LType ~_ltScale ~_ ~_Inherit" ));
			ret=sds_getkword(ResourceString(IDC_EXPLODE__NALL_MULTIPLE_C_6, "\nAll/Multiple/Color/LAyer/LType/ltScale/Inherit/<Xplode>" ),fs1);
		}else{
			if (inheritmode&(2+4+8+16)){
				sds_printf(ResourceString(IDC_EXPLODE__NALTERING___7, "\nAltering: " ));
				if (inheritmode&2) sds_printf(ResourceString(IDC_EXPLODE_LAYER__8, "Layer " ));
				if (inheritmode&4) sds_printf(ResourceString(IDC_EXPLODE_COLOR__9, "Color " ));
				if (inheritmode&8) sds_printf(ResourceString(IDC_EXPLODE_LINETYPE__10, "Linetype " ));
				if (inheritmode&16)sds_printf(ResourceString(IDC_EXPLODE_LTSCALE__11, "LTScale " ));
			}
           sds_initget(0,ResourceString(IDC_EXPLODE_INITGET_LAYER_C_12, "LAyer Color LType ltScale ~ Done ~_LAyer ~_Color ~_LType ~_ltScale ~_ ~_Done" ));
			ret=sds_getkword(ResourceString(IDC_EXPLODE__NCOLOR_LAYER_L_13, "\nColor/LAyer/LType/ltScale/<Xplode>" ),fs1);
		}
		if (ret==RTERROR || ret==RTCAN){rc=-1L;goto bail;}
		if (ret==RTNONE && (inheritmode&64))inheritmode-=64;
		if (ret==RTNORM){
           if (strisame(fs1,"ALL"/*DNT*/))
               inheritmode|=(2 | 4 | 8 | 16);
           else if (strisame(fs1,"MULTIPLE"/*DNT*/))    inheritmode|=64;
           else if (strisame(fs1,"INHERIT"/*DNT*/))     inheritmode|=32;
           else if (strisame(fs1,"LAYER"/*DNT*/))       inheritmode|=2;
           else if (strisame(fs1,"COLOR"/*DNT*/))       inheritmode|=4;
           else if (strisame(fs1,"LTYPE"/*DNT*/))       inheritmode|=8;
           else if (strisame(fs1,"LTSCALE"/*DNT*/))     inheritmode|=16;
           else if (strisame(fs1,"DONE"/*DNT*/) && (inheritmode & 64))inheritmode &= ~64;
		}
	}while(inheritmode&64);
	if (inheritmode&4){
		//get color to use
		ret=cmd_color_fromuser(&rb,1);
		if (ret==RTNORM){
			rbinherit=sds_buildlist(62,1,0);
			rbinherit->resval.rint=rb.resval.rint;
		}
		else if (ret=RTERROR || ret==RTCAN){rc=-1L;goto bail;}
		//if rtnone, don't build resbuf - we'll just use default color
	}
	if (inheritmode&8){
		//get ltype to use
        for (;;) {
			if (SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
            sprintf(fs1,ResourceString(IDC_EXPLODE__NLINETYPE_FOR__22, "\nLinetype for new entities <%s>: " ),rb.resval.rstring);
            if ((ret=sds_getstring(0,fs1,fs2))==RTERROR) {
                rc=(-__LINE__); IC_FREE(rb.resval.rstring); goto bail;
            } else if (ret==RTCAN) {
                rc=(-1L); IC_FREE(rb.resval.rstring); goto bail;
            }
            if (!*fs2) strncpy(fs2,rb.resval.rstring,sizeof(fs2)-1);;
			IC_FREE(rb.resval.rstring);
			if (strisame(fs2,"BYLAYER"/*DNT*/))break;
			if (!strisame(fs2,"BYBLOCK"/*DNT*/)){
				if (tmprb!=NULL) { rc=(-__LINE__); goto bail; }
               if ((tmprb=sds_tblsearch("LTYPE"/*DNT*/,fs2,0))==NULL) {
					sds_printf(ResourceString(IDC_EXPLODE__NLINETYPE__S_N_23, "\nLinetype %s not found.  You can use LINETYPE to load it." ),fs2);
					continue;
				}
				sds_relrb(tmprb);tmprb=NULL;
			}
            break;
        }
		if (*fs2){
			if (rbinherit!=NULL){
				//we know only one resbuf in llist
				if (NULL==(rbinherit->rbnext=sds_buildlist(6,fs2,0))){rc=(-__LINE__); goto bail; }
			}else{
				if (NULL==(rbinherit=sds_buildlist(6,fs2,0))){rc=(-__LINE__); goto bail; }
			}
		}
	}
	if (inheritmode&2){
		//get layer to use
        for (;;) {
			if (SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
            sprintf(fs1,ResourceString(IDC_EXPLODE__NLAYER_FOR_NEW_24, "\nLayer for new entities <%s>: " ),rb.resval.rstring);
            if ((ret=sds_getstring(0,fs1,fs2))==RTERROR) {
                rc=(-__LINE__); IC_FREE(rb.resval.rstring);goto bail;
            } else if (ret==RTCAN) {
                rc=(-1L); IC_FREE(rb.resval.rstring);goto bail;
            }
            if (!*fs2) strncpy(fs2,rb.resval.rstring,sizeof(fs2)-1);
			IC_FREE(rb.resval.rstring);
            strupr(fs2);
			if (!strisame(fs2,"0"/*DNT*/)){
				if (tmprb!=NULL) { rc=(-__LINE__); goto bail; }
               if ((tmprb=sds_tblsearch("LAYER"/*DNT*/,fs2,0))==NULL) {
					sds_printf(ResourceString(IDC_EXPLODE__NLAYER__S_NOT__25, "\nLayer %s not found." ),fs2);
					continue;
				}
				sds_relrb(tmprb);tmprb=NULL;
			}
            break;
        }
		if (*fs2){
			if (tmprb!=NULL) { rc=(-__LINE__); goto bail; }
			if (rbinherit!=NULL){
				for (tmprb=rbinherit;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
				if (NULL==(tmprb->rbnext=sds_buildlist(8,fs2,0))){tmprb=NULL;rc=(-__LINE__); goto bail; }
				tmprb=NULL;
			}else{
				if (NULL==(rbinherit=sds_buildlist(8,fs2,0))){rc=(-__LINE__); goto bail; }
			}
		}
	}
	if (inheritmode&16){
		//get ltscale
		if (SDS_getvar(NULL,DB_QCELTSCALE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
		sds_rtos(rb.resval.rreal,-1,-1,fs2);
        sprintf(fs1,ResourceString(IDC_EXPLODE__NLINETYPE_SCAL_26, "\nLinetype scale for new entities <%s>: " ),fs2);
		sds_initget(RSG_NOZERO,NULL);
        if ((ret=sds_getreal(fs1,&rb.resval.rreal))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if (ret==RTCAN) {
            rc=(-1L); goto bail;
		}else
			rb.resval.rreal=fabs(rb.resval.rreal);
		if (tmprb!=NULL) { rc=(-__LINE__); goto bail; }
		if (rbinherit!=NULL){
			for (tmprb=rbinherit;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
			if (NULL==(tmprb->rbnext=sds_buildlist(48,rb.resval.rreal,0))){tmprb=NULL;rc=(-__LINE__); goto bail; }
			tmprb=NULL;
		}else{
			if (NULL==(rbinherit=sds_buildlist(48,rb.resval.rreal,0))){rc=(-__LINE__); goto bail; }
		}
	}
	bail:
	*rbprop=rbinherit;
	*mode=inheritmode;
	return(rc);
}




int cmd_xplode_addinherit(sds_resbuf *elist,sds_resbuf *rbinherit){
	//used to change properties in elist to match those in rbinherit.  Adds rbinherit's
	//	properties if not found in llist.  used for layer, color, ltype, & ltscale ONLY.
	//NOTE:  CALLER MUST FREE BOTH LLISTS!!!
	//returns 0 if ok, -1 if error
	int ret=0,inheritmode=0;
	struct resbuf *tmprb,*rb8,*rb62,*rb6,*rb48;

	rb8=rb62=rb6=rb48=NULL;
	for (tmprb=rbinherit;tmprb!=NULL;tmprb=tmprb->rbnext){
		if (tmprb->restype==8){rb8=tmprb;inheritmode|=2;}
		else if (tmprb->restype==62){rb62=tmprb;inheritmode|=4;}
		else if (tmprb->restype==6) {rb6=tmprb;inheritmode|=8;}
		else if (tmprb->restype==48){rb48=tmprb;inheritmode|=16;}
	}
	if (!inheritmode || rbinherit==NULL)return(0);
	if (inheritmode&2){
		if (0==ic_assoc(elist,8)){
			if (NULL==(ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(rb8->resval.rstring)+1))){ret=-1;goto exit;}
			strcpy(ic_rbassoc->resval.rstring,rb8->resval.rstring);
		}else{
			for (tmprb=elist;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
			if (NULL==(tmprb->rbnext=sds_buildlist(8,rb8->resval.rstring,0))){ret=-1;goto exit;}
		}
	}
	if (inheritmode&4){
		if (0==ic_assoc(elist,62)){
			ic_rbassoc->resval.rint=rb62->resval.rint;
		}else{
			for (tmprb=elist;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
			if (NULL==(tmprb->rbnext=sds_buildlist(62,rb62->resval.rint,0))){ret=-1;goto exit;}
		}
	}
	if (inheritmode&8){
		if (0==ic_assoc(elist,6)){
			if (NULL==(ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(rb6->resval.rstring)+1))){ret=-1;goto exit;}
			strcpy(ic_rbassoc->resval.rstring,rb6->resval.rstring);
		}else{
			for (tmprb=elist;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
			if (NULL==(tmprb->rbnext=sds_buildlist(6,rb6->resval.rstring,0))){ret=-1;goto exit;}
		}

	}
	if (inheritmode&16){
		if (0==ic_assoc(elist,48)){
			ic_rbassoc->resval.rreal=rb48->resval.rreal;
		}else{
			for (tmprb=elist;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
			if (NULL==(tmprb->rbnext=sds_buildlist(48,rb48->resval.rreal,0))){ret=-1;goto exit;}
		}
	}
	exit:
	return(ret);
}



int cmd_explode_pfacemesh(struct resbuf *elbeg,sds_name ename, sds_name ssetout)
	{
	//f'n makes 3d faces from polyface mesh entity.  elbeg is llist pointer to
	//everything we need to make entity.  This f'n examines ename and
	//supplies pts to make all the ents.  Calling f'n must release llist elbeg
	//This f'n deletes ename if everything OK.

	int ret=RTNORM;
	int fi0,fi1,fi2,fi3,fi4;
	long vtxno,vtxcnt;
	struct resbuf *rbtemp,*rbwalk,*rbtemp10,*rbtemp11,*rbtemp12,*rbtemp13,*rbtemp70;
	sds_point *ptarray;
	sds_name etemp,elast,regenset;

	ptarray=NULL;
	rbtemp=NULL;
	regenset[0]=0L;



	//setup pointers into llist points
	fi1=0;
	for (rbtemp=elbeg;rbtemp!=NULL && fi1!=31;rbtemp=rbtemp->rbnext){
		if (rbtemp->restype==10){rbtemp10=rbtemp;fi1+=1;}
		if (rbtemp->restype==11){rbtemp11=rbtemp;fi1+=2;}
		if (rbtemp->restype==12){rbtemp12=rbtemp;fi1+=4;}
		if (rbtemp->restype==13){rbtemp13=rbtemp;fi1+=8;}
		if (rbtemp->restype==70){rbtemp70=rbtemp;fi1+=16;}

	}
	if (fi1!=31){ret=RTERROR;goto exit;}
	rbtemp=NULL;
	ic_encpy(etemp,ename);

	//count how many point-type vertices we have (both flags set)
	for (vtxcnt=0L;;vtxcnt++){
		if (sds_entnext_noxref(etemp,etemp)!=RTNORM){ret=RTERROR;goto exit;}
		if (rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=NULL;}
		if ((rbtemp=sds_entget(etemp))==NULL){ret=RTERROR;goto exit;}
		if (0!=ic_assoc(rbtemp,70)){ret=RTERROR;goto exit;}
		if (192!=(ic_rbassoc->resval.rint & (64+128))) break;
	}
	//allocate an array of points
	if ((ptarray= new sds_point [vtxcnt] )==NULL){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
	memset(ptarray,0,sizeof(sds_point)*vtxcnt);
	//now walk again to populate the array....
	ic_encpy(etemp,ename);
	for (vtxno=0L;vtxno<vtxcnt;vtxno++){
		if (sds_entnext_noxref(etemp,etemp)!=RTNORM){ret=RTERROR;goto exit;}
		if (rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=NULL;}
		if ((rbtemp=sds_entget(etemp))==NULL){ret=RTERROR;goto exit;}
		ic_assoc(rbtemp,10);
		ic_ptcpy(ptarray[vtxno],ic_rbassoc->resval.rpoint);
	}
	//now we have the 0-based order for the points to use.  etemp is on the
	//last point-defining vertex.  Now walk the face-defining vertices...
	do{
		if (sds_entnext_noxref(etemp,etemp)!=RTNORM){ret=RTERROR;goto exit;}
		if (rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=NULL;}
		if ((rbtemp=sds_entget(etemp))==NULL){ret=RTERROR;goto exit;}
		fi0=0;
		for (rbwalk=rbtemp;rbwalk!=NULL;rbwalk=rbwalk->rbnext){
			if (0==rbwalk->restype && !strsame(rbwalk->resval.rstring,"VERTEX"/*DNT*/))break;
			if (71==rbwalk->restype){fi1=rbwalk->resval.rint;fi0+=1;}
			if (72==rbwalk->restype){fi2=rbwalk->resval.rint;fi0+=2;}
			if (73==rbwalk->restype){fi3=rbwalk->resval.rint;fi0+=4;}
			if (74==rbwalk->restype){fi4=rbwalk->resval.rint;fi0+=8;}
		}
		if (rbwalk!=NULL && !strsame(rbwalk->resval.rstring,"VERTEX"/*DNT*/))break;
		if (fi0!=15){ret=RTERROR;goto exit;}
		//stuff the points in and go for it!
		fi0=(abs(fi1)-1);//go from 1-based to 0-based counter
		ic_ptcpy(rbtemp10->resval.rpoint,ptarray[fi0]);
		fi0=(abs(fi2)-1);
		ic_ptcpy(rbtemp11->resval.rpoint,ptarray[fi0]);
		if (abs(fi3)!=abs(fi1))fi0=(abs(fi3)-1);
		ic_ptcpy(rbtemp12->resval.rpoint,ptarray[fi0]);
		if (fi4!=0 && (abs(fi4)!=abs(fi1)))fi0=(abs(fi4)-1);	//don't alter fi0 if fi4 is 0
		ic_ptcpy(rbtemp13->resval.rpoint,ptarray[fi0]);
		//set the visibility flag....
		fi0=0;
		if (fi1<=0)fi0+=1;
		if (fi2<=0)fi0+=2;
		if (fi3<=0 || abs(fi3)==abs(fi1))fi0+=4;
		if (fi4<=0 || abs(fi4)==abs(fi1))fi0+=8;
		rbtemp70->resval.rint=fi0;


		// *****************************
		// Okay, these faces may have extended entity data associated with them, and this
		// extended entity data should be added to the new 3D Face that is created.
		//  Easiest way to get a hold of the EED is to use the entity object
		//
		db_entity *pEntity = (db_entity *)etemp[0];
		ASSERT( CHECKSTRUCTPTR( pEntity ) );
		if ( pEntity->ret_type() != DB_VERTEX )
			{
			ASSERT( false );
			ret = RTERROR;
			goto exit;
			}

		// get_eed(NULL,...) gets eed associated with any and all applications
		//
		struct resbuf *prbEED = pEntity->get_eed( NULL, NULL );
		// If this object actually has EED
		//
		struct resbuf *prbEndOfEntityList = NULL;
		if ( prbEED != NULL )
			{
			// find the end of the entity list
			//
			prbEndOfEntityList = elbeg;
			ASSERT( prbEndOfEntityList != NULL );
			while( prbEndOfEntityList->rbnext != NULL )
				{
				prbEndOfEntityList = prbEndOfEntityList->rbnext;
				}
			// add the eed list to the end of the entity list
			//
			prbEndOfEntityList->rbnext = prbEED;

			}

		if ((ret=sds_entmake(elbeg))!=RTNORM)
			{
			goto exit;
			}
		if (ssetout)
			{
			sds_name tmpename;
			sds_entlast(tmpename);
			sds_ssadd (tmpename, ssetout, ssetout);
			}

		// If there was EED added, we need to get it out of elbeg and release it
		//
		if ( prbEndOfEntityList != NULL )
			{
			prbEndOfEntityList->rbnext = NULL;
			}
		if ( prbEED != NULL )
			{
			sds_relrb( prbEED );
			prbEED = NULL;
			}


		sds_entlast(elast);
		if (0L!=regenset[0])
			SDS_ssadd(elast, regenset, regenset);
		else
			sds_ssadd(elast,NULL,regenset);
	}while(1);

	exit:
	if (ptarray!=NULL) IC_FREE(ptarray);
	if (rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=rbwalk=NULL;}
	if (ret==RTNORM && 0L!=regenset[0]){
		sds_entdel(ename);
		for (vtxcnt=0L;sds_ssname(regenset,vtxcnt,elast)==RTNORM;vtxcnt++){
			SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)elast[0],NULL,NULL,SDS_CURDWG);
			sds_redraw(elast,IC_REDRAW_DRAW);
		}
	}
	if (0L!=regenset[0])sds_ssfree(regenset);
	return(ret);

}

int cmd_explode_plmesh(struct resbuf *elbeg,sds_name ename, int mcnt, int ncnt,int plflag, sds_name ssetout)
	{
	//f'n makes 3D faces from 3D mesh entity ename.  elbeg is llist pointer to
	//everything we need to make entity.  This f'n examines ename and
	//supplies pts to make all the ents.  Calling f'n must release llist elbeg
	//and delete ename.

	int ret=RTNORM;
	int fi1;
	long vtxno,vtxcnt,fl1,fl2,fl3;
	struct resbuf *rbtemp,*rbtemp10,*rbtemp11,*rbtemp12,*rbtemp13;
	sds_point *ptarray;
	sds_name etemp,elast,regenset;

	ptarray=NULL;
	rbtemp=NULL;
	regenset[0]=0L;

	//setup pointers into llist points
	fi1=0;
	for (rbtemp=elbeg;rbtemp!=NULL && fi1!=15;rbtemp=rbtemp->rbnext){
		if (rbtemp->restype==10){rbtemp10=rbtemp;fi1+=1;}
		if (rbtemp->restype==11){rbtemp11=rbtemp;fi1+=2;}
		if (rbtemp->restype==12){rbtemp12=rbtemp;fi1+=4;}
		if (rbtemp->restype==13){rbtemp13=rbtemp;fi1+=8;}
	}
	if (fi1!=15){ret=RTERROR;goto exit;}
	rbtemp=NULL;

	//allocate a llist of points....
	vtxcnt=mcnt*ncnt;
	if ((ptarray= new sds_point [vtxcnt] )==NULL){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
	memset(ptarray,0,sizeof(sds_point)*vtxcnt);
	//populate the list
	ic_encpy(etemp,ename);
	if (rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=NULL;}
	for (vtxno=0L;vtxno<vtxcnt;vtxno++){
		if (sds_entnext_noxref(etemp,etemp)!=RTNORM){ret=RTERROR;goto exit;}
		if ((rbtemp=sds_entget(etemp))==NULL){ret=RTERROR;goto exit;}
		ic_assoc(rbtemp,10);
		ic_ptcpy(ptarray[vtxno],ic_rbassoc->resval.rpoint);
		if (rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=NULL;}
	}

	for (vtxno=0L;vtxno<vtxcnt;vtxno++){
		//vtxno will track lower left corner (10)
		//fl1 will track lower right corner (11)
		if (((vtxno+1)%ncnt)==0){	//if we're at the n edge
			if (0==(plflag & 32)) continue;
			fl1=vtxno-ncnt+1;
		}else fl1=vtxno+1;
		//fl2 will track upper left corner (13)
		//fl3 will track upper right corner (12)
		if ((vtxno+1) > (mcnt-1)*ncnt){ //if lower left vtx has reached top row...
			if (0==(plflag & 1)) break;  //if not closed in m direction, break
			fl2=vtxno-((mcnt-1)*ncnt);
		}else fl2=vtxno+ncnt;
		if (((fl2+1)%ncnt)==0){
			if (0==(plflag & 32)) continue;
			fl3=fl2-ncnt+1;
		}else fl3=fl2+1;
		ic_ptcpy(rbtemp10->resval.rpoint,ptarray[vtxno]);
		ic_ptcpy(rbtemp11->resval.rpoint,ptarray[fl1]);
		ic_ptcpy(rbtemp12->resval.rpoint,ptarray[fl3]);
		ic_ptcpy(rbtemp13->resval.rpoint,ptarray[fl2]);
		if ((ret=sds_entmake(elbeg))!=RTNORM)goto exit;
		if (ssetout)
			{
			sds_name tmpename;
			sds_entlast(tmpename);
			sds_ssadd (tmpename, ssetout, ssetout);
			}
		sds_entlast(elast);
		if (0L!=regenset[0])
			SDS_ssadd(elast, regenset, regenset);
		else
			sds_ssadd(elast,NULL,regenset);
	}

	exit:
	if (ptarray!=NULL) IC_FREE(ptarray);
	if (rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=NULL;}
	if (ret==RTNORM && 0L!=regenset[0]){
		sds_entdel(ename);
		for (vtxcnt=0L;sds_ssname(regenset,vtxcnt,elast)==RTNORM;vtxcnt++){
			SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)elast[0],NULL,NULL,SDS_CURDWG);
			sds_redraw(elast,IC_REDRAW_DRAW);
		}
	}
	if (0L!=regenset[0])sds_ssfree(regenset);
	return(ret);
}



// This function creates elliptic arc subentties after the insert is exploded. The function
// return 0 when it success and a non-zero otherwise. - SWH
//
long cmd_explode_makesubents_ellipse(sds_name enttmp,		// the entity name of elliptic arc
									 sds_point inspt,		// the insert point
									 sds_point blkinspt,	// block insert point
                                     sds_real xscl,			// x-scale
									 sds_real yscl,			// y-scale
									 sds_real zscl,			// z-scale
									 sds_real rotang,		// the rotation angle of the insert
									 sds_point extru,		// the extrude dir of th parent entity
									 int psflag,			// paper space flag
									 struct resbuf *rbinherit, // inherited parent's properties
									 resbuf **subEntities)  // the resbuf list of new elliptic arc
{
    sds_real Cos = cos(rotang), Sin = sin(rotang);
    long rc = 0L;

    struct resbuf rbwcs,rbparent;
	rbparent.restype=RT3DPOINT;
	ic_ptcpy(rbparent.resval.rpoint,extru);
	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;

	db_ellipse* pEllipse = NULL;

    if (enttmp==NULL)
		return(-__LINE__);

	pEllipse = (db_ellipse*)enttmp[0];
	ASSERT(pEllipse != NULL);

	sds_real rratius;
	pEllipse->get_40(&rratius);

	double sParam, eParam;
	pEllipse->get_41(&sParam);
	pEllipse->get_42(&eParam);

	int lineweight;
	pEllipse->get_370(&lineweight);

    char* szLayer = NULL, *szLinetype = NULL;
    pEllipse->get_8(&szLayer);
	pEllipse->get_6(&szLinetype);

	// transform center
	//
	sds_point center;
	pEllipse->get_10(center);
	for (register k = 0; k < 3; k++)
		center[k] -= blkinspt[k];

	center[0] *= xscl;
	center[1] *= yscl;
	center[2] *= fabs(zscl);

	if (!icadAngleEqual(rotang, 0.0)) {
		sds_real x = center[0] * Cos - center[1] * Sin;
		sds_real y = center[1] * Cos + center[0] * Sin;
		center[0] = x; center[1] = y;
	}
	for (k = 0; k < 3; k++)
		center[k] += inspt[k];

	sds_trans(center,&rbparent,&rbwcs,0,center);

	// transform major, minor axes, and normal
	//
	sds_point majAxis, minAxis, normal;
	pEllipse->get_11(majAxis);
	pEllipse->get_210(normal);
	ic_crossproduct(normal, majAxis, minAxis);
	double r = rratius * ic_veclength(majAxis) / ic_veclength(minAxis);
	for (k = 0; k < 3; k++)
		minAxis[k] *= r;

	majAxis[0] *= xscl; majAxis[1] *= yscl; majAxis[2] *= fabs(zscl);
	minAxis[0] *= xscl; minAxis[1] *= yscl; minAxis[2] *= fabs(zscl);
	normal[0] *= xscl; normal[1] *= yscl; normal[2] *= fabs(zscl);

	if (!icadAngleEqual(rotang, 0.0)) {
		sds_real x = majAxis[0] * Cos - majAxis[1] * Sin;
		sds_real y = majAxis[1] * Cos + majAxis[0] * Sin;
		majAxis[0] = x; majAxis[1] = y;

		x = minAxis[0] * Cos - minAxis[1] * Sin;
		y = minAxis[1] * Cos + minAxis[0] * Sin;
		minAxis[0] = x; minAxis[1] = y;

		sds_real xNormal = normal[0] * Cos - normal[1] * Sin;
		sds_real yNormal = normal[1] * Cos + normal[0] * Sin;
		normal[0] = xNormal; normal[1] = yNormal;
	}

	if (!icadRealEqual(xscl, yscl)) { // non-uniform scale
		// After non-uniform scaled, the major and minor axes are not orthogonal
		// so we need to replace the axes with orthogonal axes that represent the
		// same ellipse. From the equation
		//
		//      P(t) = Center + majAxis * cos(t) + minAxis * sin(t),
		//
		// we can know the distance "D(t)" between a point on the ellipse and the
		// center of the ellipse is || P(t) - center ||. That is,
		//
		//       D(t) = ||majAxis * cos(t) + minAxis * sin(t)||.
		//
		// Mathematically, the end points of the axes on the ellipse satisfy the
		// condition D'(t) = 0, or said
		//
		//       d (||majAxis * cos(t) + minAxis * sin(t)||) / dt = 0.
		//
		// Hence, we have
		//
		//   t = arctan(2.0 * majAxis * minAxis / (majAxis * majAxis - minAxis * minAxis)) / 2.0;
		//
		double s = ic_dotproduct(majAxis, minAxis);
		r = ic_dotproduct(majAxis, majAxis) - ic_dotproduct(minAxis, minAxis);
		double t0 = atan2(2.0 * s, r) / 2.0;

		// the domain will be shifted so we also need to shift the start and end parameters.
		//
		sParam -= t0;
		eParam -= t0;

		// Get the new major and minor
		//
		s = sin(t0); r = cos(t0);
		sds_point major, minor;
		for (k = 0; k < 3; k++) {
			major[k] = majAxis[k] * r + minAxis[k] * s;
			minor[k] = majAxis[k] * (-s) + minAxis[k] * r;
		}
		ic_ptcpy(majAxis, major);
		rratius = ic_veclength(minor) / ic_veclength(major);
	}

	if (!rbinherit)
		*subEntities = sds_buildlist(RTDXF0,"ELLIPSE"/*DNT*/,
									6, szLinetype,
									8, szLayer,
									10,center,
									11,majAxis,
									40,rratius,
									41,sParam,
									42,eParam,
									48,pEllipse->ret_ltscale(),
									62,pEllipse->ret_color(),
									67,psflag,
									370, lineweight,
									210, normal,
									0);
	else { // inherit parent's attributes
		ASSERT(*subEntities != NULL);
		(*subEntities)->rbnext = sds_buildlist(RTDXF0,"ELLIPSE"/*DNT*/,
									           10,center,
									           11,majAxis,
									           40,rratius,
									           41,sParam,
									           42,eParam,
									           67,psflag,
									           210, normal,
									           0);
	}
    return(rc);
}


//10/98 - modified this function to pass out the subEntities instead of calling entmake
//on these entities.  This allows the flexability of creating temporary entities that are not
//actually added to the drawing. Caller is responsible for calling sds_entmake or sds_entmakex.

long cmd_explode_makesubents(sds_name enttmp, sds_point inspt, sds_point blkinspt,
    sds_real xscl, sds_real yscl, sds_real zscl, sds_real rotang, sds_point extru,
    int psflag, struct resbuf *rbinherit, resbuf **subEntities) {
//*** This function creates all the subentities for an INSERT and is called
//*** recursively for polylines verticies.
//*** psflag is 67 (paper space) flag to use on all subents
//***
//*** RETURNS:    0 - Success.
//***             1 - SEQEND was just made for attributes
//***            -1 - User cancel.
//***          < -1 - RTERROR returned from an ads function.
//***	NOTE: if NULL!=rbinherit, subents will inherit the properties in that llist
//***			extru is the extrusion of the parent insert
//*** If isnested!=0, then this is a nested block and we DON"T convert attribs to attdefs

    struct resbuf *typerb,*rbsearch,*rbpinsert,rbwcs,rbucs,rbent,rbnewent,rbparent,app;
    sds_point pt2,pt3,pt210,newpt1,newpt2,newpt3,newpt4,smavect;
    sds_real fr2,rad, Cos = cos(rotang), Sin = sin(rotang);
    long rc;
    int fi1,textent,
		toellipse = 0; //flag telling us if we're converting arc or circle into elliptical arc
	static int entinwcs;//needed for recursive calls
	static sds_point vtx210;//needed for successive calls because 2D pline vertex ents
							//	don't have a 210 group - just the main ent does

	bool hatchellipse = false; //needed to determine if the code 11 is a elliptical edge of a hatch
							   //if so the insertion point should not be added to it as it is a vector.
	bool hatchpolyline = false; //also needed to determine if code 11 is an elliptical arc.  If it is a polyline then
								//the flag for hatchellipse should be false.
	bool isNonUniformScaling;	/*D.Gavrilov*/// true if an extrusion vector of an entity and the given scales show us that
								// we should scale the entity
	bool isMatrixReady = false; // just for computing the following matrix only once
	sds_matrix rotxform;		// for 210, 1012 & 1013 groups
	bool isSwapXYscalforIns = false;

	resbuf *hatchpatternxrb = NULL;

	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;
	rbparent.restype=rbent.restype=RT3DPOINT;
	ic_ptcpy(rbparent.resval.rpoint,extru);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	bool diment = false;

    rc=0L;
    if (enttmp==NULL) return(-__LINE__);
    app.restype=RTSTR; app.resval.rstring="*"/*DNT*/ ; app.rbnext=NULL;
    if ((*subEntities=sds_entgetx(enttmp,&app))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-__LINE__); goto bail;
    }
	//if we're inheriting any properties from a parent entity, substitute them now
	if (rbinherit!=NULL){
		cmd_xplode_addinherit(*subEntities,rbinherit);
	}
    if (ic_assoc(*subEntities,0)!=0) {
		rc=(-__LINE__);
		goto bail;
	}

	typerb=ic_rbassoc;

	// call special case function for ellipse
	if (strsame("ELLIPSE"/*DNT*/,typerb->resval.rstring)) {
		return cmd_explode_makesubents_ellipse(enttmp, inspt, blkinspt, xscl, yscl, zscl,
			                                   rotang, extru, psflag, rbinherit, subEntities);
	}

    if (strsame("ENDBLK"/*DNT*/,typerb->resval.rstring))
		goto bail;

	//reset entinwcs for all ents except VERTEX, which gets it from last setting of static
	if(RTNORM==sds_wcmatch(typerb->resval.rstring,"SOLID,TRACE,TEXT,ATTRIB,ATTDEF,SHAPE,INSERT,MTEXT,LWPOLYLINE,HATCH,ARC,CIRCLE"/*DNT*/))
		entinwcs=0;
	else
		if(strsame(typerb->resval.rstring,"POLYLINE"/*DNT*/))
		{
			ic_assoc(*subEntities,70);
			if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
				entinwcs=1;
			else
			{
				entinwcs=0;
				ic_assoc(*subEntities,210);
				ic_ptcpy(vtx210,ic_rbassoc->resval.rpoint);
			}
		}
		else
			if(!strsame(typerb->resval.rstring,"VERTEX"/*DNT*/))
				entinwcs=1;

	if(RTNORM==sds_wcmatch(typerb->resval.rstring,"ATTDEF,ATTRIB,TEXT,SHAPE"/*DNT*/))
		textent=1;
	else
		textent=0;

	if(strisame(typerb->resval.rstring,"DIMENSION"/*DNT*/))
		diment = true;

    //*** Entity extru vector
	if (!entinwcs){
		if (strsame(typerb->resval.rstring,"VERTEX"/*DNT*/)){
			for (rbsearch=*subEntities;rbsearch->rbnext!=NULL;rbsearch=rbsearch->rbnext);
			rbsearch->rbnext=sds_buildlist(210,vtx210,0);
			rbsearch=rbsearch->rbnext;
			ic_ptcpy(pt210,vtx210);
		}else if (ic_assoc(*subEntities,210)){
			pt210[0]=pt210[1]=0.0;
			pt210[2]=1.0;
			for (rbsearch=*subEntities;rbsearch->rbnext!=NULL;rbsearch=rbsearch->rbnext);
			rbsearch->rbnext=sds_buildlist(210,pt210,0);
			rbsearch=rbsearch->rbnext;
		}else{
			ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
			rbsearch=ic_rbassoc;
		}
		rbent.restype=RT3DPOINT;
		ic_ptcpy(rbent.resval.rpoint,pt210);
		ic_ptcpy(pt2,pt210);
		pt2[0]*=xscl;
		pt2[1]*=yscl;
		pt2[2]*=zscl;
		pt3[0]=pt2[0]*Cos-pt2[1]*Sin;
		pt3[1]=pt2[1]*Cos+pt2[0]*Sin;
		pt3[2]=pt2[2];
		fr2=sqrt(pt3[0]*pt3[0]+pt3[1]*pt3[1]+pt3[2]*pt3[2]);
		pt3[0]/=fr2;
		pt3[1]/=fr2;
		pt3[2]/=fr2;
		sds_trans(pt3,&rbparent,&rbwcs,1,rbsearch->resval.rpoint);
	}else{
		rbent.restype=RTSHORT;
		rbent.resval.rint=0;
		rbnewent.restype=RTSHORT;
		rbnewent.resval.rint=0;
	}

	isNonUniformScaling =
		(fabs(pt210[0]) > IC_ZRO && fabs(yscl - zscl) > IC_ZRO) ||
		(fabs(pt210[1]) > IC_ZRO && fabs(xscl - zscl) > IC_ZRO) ||
		(fabs(pt210[2]) > IC_ZRO && fabs(xscl - yscl) > IC_ZRO);

	/*D.Gavrilov*/// don't explode in the following case
	// (we cannot obtain a correct INSERT from this INSERT).
	// ACAD also doesn't do it...
	// TODO: Maybe we should explode included (inner) INSERT for additional functionality
	//       (although ACAD doesn't do it).
	if(strsame("INSERT",typerb->resval.rstring) && isNonUniformScaling)
	{
	    if(ic_assoc(*subEntities, 50) != 0)
		{
			rc=(-__LINE__); goto bail;
		}
		// EBATECH(CNBR) -[ 2003/4/30 Bugzilla#78090
		//if((!icadAngleEqual(ic_rbassoc->resval.rreal,0.0)) || !icadAngleEqual(ic_rbassoc->resval.rreal,IC_TWOPI))
		double tmpRot = ic_rbassoc->resval.rreal;
		if(icadAngleEqual(tmpRot,IC_PI/2) || icadAngleEqual(tmpRot,IC_PI/2*3))
		{
			isSwapXYscalforIns = true;
		}
		else if( icadAngleEqual(tmpRot,0.0) || icadAngleEqual(tmpRot,IC_PI) || icadAngleEqual(tmpRot,IC_TWOPI))
		{
			isSwapXYscalforIns = false;
		}
		else
		{
			//sds_printf("\nUnable to explode this insertion");
			rc = -1; goto bail;
		}
		// EBATECH(CNBR) ]-
	}

	if( RTNORM == sds_wcmatch(typerb->resval.rstring, "ARC,CIRCLE") &&
		isNonUniformScaling )
	{
		if(strsame(typerb->resval.rstring,"ARC"/*DNT*/))
			toellipse=1;
		else
			toellipse=2;
		typerb->resval.rstring = ic_realloc_char(typerb->resval.rstring,
												 strlen("ELLIPSE"/*DNT*/)+1);
		strcpy(typerb->resval.rstring, "ELLIPSE"/*DNT*/);
	}

	if(!entinwcs)
	{
		if(toellipse == 0)
		{
			rbnewent.restype=RT3DPOINT;
			ic_ptcpy(rbnewent.resval.rpoint, rbsearch->resval.rpoint);
		}
		else
		{
			rbnewent.restype=RTSHORT;
			rbnewent.resval.rint=0;
		}
	}
    //<ALM>
	if ((sds_wcmatch(typerb->resval.rstring, "BODY,3DSOLID,REGION") == RTNORM))
	{
        // form 'insert block' transformation
        point insert_point(inspt);
        point blk_insert_point(blkinspt);
        point zero(0.,0.,0.);

        transf t = icl::translation(blk_insert_point - zero) *
                   icl::scaling(xscl, yscl, zscl) *
                   icl::rotation(rotang, gvector(0.,0.,1.)) *
                   icl::translation(insert_point - zero);

        // form UCS->WCS WCS->UCS transformations
        wcs2ucs2wcs(t);

		resbuf*	pSatRb = ic_ret_assoc(*subEntities, 1);
        if (pSatRb)
        {
			CDbAcisEntityData oldObj(pSatRb->resval.rstring, typerb->resval.rstring, CDbAcisEntityData::eDbType);
			CDbAcisEntityData newObj;

            if (CModeler::get()->transform(oldObj, newObj, t))
            {
				char*& sat = pSatRb->resval.rstring;
                IC_FREE(sat);
                sat = new char [newObj.getSat().size()+1];
                strcpy(sat, newObj.getSat().c_str());
            }
        }
    }
    //</ALM>
    // the main loop : changing values in resbufs
	for (rbsearch=*subEntities;rbsearch!=NULL;rbsearch=rbsearch->rbnext){

        //NOTE:  Don't convert the attribs to attdefs!
        //  This is for sub-nested ents only, and we only explode down 1 level at a time.
		if (toellipse){
			// convert circle or arc into an ellipse entity w/axis ratio of 1.0, and then let
			// standard scaling stuff below alter its shape
			if (rbsearch->restype==10){
				ic_ptcpy(newpt3,rbsearch->resval.rpoint);	/*D.Gavrilov*///don't trans/change
				sds_trans(newpt3,&rbent,&rbwcs,0,newpt1);	//rbsearch->resval.rpoint here
			}else if (rbsearch->restype==40){
				rad=rbsearch->resval.rreal;
				newpt2[0]=rbsearch->resval.rreal*xscl;//rotating of vector done below
				newpt2[1]=0.0;
				newpt2[2]=0.0;
				sds_trans(newpt2,&rbent,&rbwcs,1,smavect);
				rbpinsert=sds_buildlist(11,smavect,0);
				rbpinsert->rbnext=rbsearch->rbnext;
				rbsearch->rbnext=rbpinsert;
				rbsearch->resval.rreal=yscl/xscl;//we'll correct it below - we're starting w/a circle
				if (2==toellipse){
					//if the ent is a circle, we need to add 41 and 42 groups to the new ellipse
					rbpinsert=sds_buildlist(41,0.0,42,IC_TWOPI,0);
					rbpinsert->rbnext->rbnext=rbsearch->rbnext;
					rbsearch->rbnext=rbpinsert;
				}
			}else if (1==toellipse && rbsearch->restype==50){//must be for an arc - circles don't have 41's
				newpt4[0]=newpt3[0]+rad*fabs(xscl)*cos(rbsearch->resval.rreal);
				newpt4[1]=newpt3[1]+rad*fabs(yscl)*sin(rbsearch->resval.rreal);
				newpt4[2]=newpt3[2];
				sds_trans(newpt4,&rbent,&rbwcs,0,newpt4);
				ic_angpar(sds_angle(newpt1,newpt4),&(rbsearch->resval.rreal),fabs(xscl),fabs(yscl),0);
				rbsearch->restype=41;
			}else if (1==toellipse && rbsearch->restype==51){//must be for an arc - circles don't have 41's
				newpt4[0]=newpt3[0]+rad*fabs(xscl)*cos(rbsearch->resval.rreal);
				newpt4[1]=newpt3[1]+rad*fabs(yscl)*sin(rbsearch->resval.rreal);
				newpt4[2]=newpt3[2];
				sds_trans(newpt4,&rbent,&rbwcs,0,newpt4);
				ic_angpar(sds_angle(newpt1,newpt4),&(rbsearch->resval.rreal),fabs(xscl),fabs(yscl),0);
				rbsearch->restype=42;
			}
		}
		if (rbsearch->restype==67)rbsearch->resval.rint=psflag;
		if (10<=rbsearch->restype && 18>=rbsearch->restype){
			fi1=0;//default to non-vector
			if (rbsearch->restype==11){
				//if (strsame(typerb->resval.rstring,"RAY"/*DNT*/) || strsame(typerb->resval.rstring,"XLINE"/*DNT*/)||
                //   strsame(typerb->resval.rstring,"TOLERANCE"/*DNT*/) || strsame(typerb->resval.rstring,"MTEXT"/*DNT*/) || strsame(typerb->resval.rstring,"IMAGE"/*DNT*/))
				if(RTNORM==sds_wcmatch(typerb->resval.rstring,"RAY,XLINE,TOLERANCE,MTEXT"/*DNT*/))
					fi1=1;//unitized
 				else if (strsame(typerb->resval.rstring,"ELLIPSE"/*DNT*/) || hatchellipse){
					//NOTE: When you scale an "ellipse"(a circle with uniform axes) non-uniformly,
					//  you CANNOT scale the major axis by these independent values to arrive at the new
					//  axis points. The axis will skew toward the direction of largest scale.
					ic_ptcpy(smavect,rbsearch->resval.rpoint);
					fi1=2;
				}
			}else if ((rbsearch->restype==12 || rbsearch->restype==13)&& strsame(typerb->resval.rstring,"SPLINE"/*DNT*/)){
				pt3[0]=pt3[1]=pt3[2]=0.0;
				if (icadPointEqual(rbsearch->resval.rpoint,pt3))continue;
				fi1=1;//unitized
			}else if (rbsearch->restype==12 && strsame(typerb->resval.rstring,"DIMENSION"/*DNT*/)){
				rbsearch->resval.rpoint[0]=rbsearch->resval.rpoint[1]=rbsearch->resval.rpoint[2]=0.0;
				continue;
			}else if (strsame(typerb->resval.rstring, "IMAGE"/*DNT*/))
			{
				// DP: groups 11 and 12 are vectors, group 11 is checked in the first condition above
				if(rbsearch->restype == 12)
					fi1 = 1;
				else
					// DP: do not transform size of image (in pixels)
					if(rbsearch->restype == 13)
						continue;
			}
			sds_trans(rbsearch->resval.rpoint,&rbent,&rbwcs,fi1,pt2);
			if (0==fi1){
				pt2[0]-=blkinspt[0];
				pt2[1]-=blkinspt[1];
				pt2[2]-=blkinspt[2];
			}
			if (0==toellipse || rbsearch->restype!=11){//we already accounted for non-uniform scaling if converting to ellipse
				pt2[0]*=xscl;
				pt2[1]*=yscl;
				pt2[2]*=zscl;
			}
			pt3[0]=pt2[0];pt3[1]=pt2[1];
			pt2[0]=pt3[0]*Cos-pt3[1]*Sin;
			pt2[1]=pt3[1]*Cos+pt3[0]*Sin;
			if (0==fi1){
				pt2[0]+=inspt[0];
				pt2[1]+=inspt[1];
				pt2[2]+=inspt[2];
			}else if (1==fi1){
				//unitize the vector
				fr2=sqrt(pt2[0]*pt2[0]+pt2[1]*pt2[1]+pt2[2]*pt2[2]);
				pt2[0]/=fr2;
				pt2[1]/=fr2;
				pt2[2]/=fr2;
			}else fi1=1;	//if it was 2, set it to 1 before doing sds_trans below
			sds_trans(pt2,&rbparent,&rbnewent,fi1,rbsearch->resval.rpoint);

			if (!entinwcs && strsame(typerb->resval.rstring,"POLYLINE"/*DNT*/)){
				rbsearch->resval.rpoint[0]=rbsearch->resval.rpoint[1]=0.0;
				}
		}else if (40==rbsearch->restype){
			if (strsame(typerb->resval.rstring,"LWPOLYLINE"/*DNT*/))
				continue;	/*D.Gavrilov*/// don't change the start width

			// For non-uniform scaled circle, we have replace the radius in this 
			// resbuf with new value of major-minor ratio so this value should not
			// be set again here..
			//
			if (strsame(typerb->resval.rstring,"ELLIPSE"/*DNT*/) || hatchellipse)
				continue;
			else if (textent){
				rbsearch->resval.rreal*=yscl;
			}else rbsearch->resval.rreal*=xscl;
		}else if (41==rbsearch->restype){
			if (strsame(typerb->resval.rstring,"LWPOLYLINE"/*DNT*/))
				continue;	/*D.Gavrilov*/// don't change the end width

			if (strsame(typerb->resval.rstring,"ELLIPSE"/*DNT*/))
				continue;

			if (strsame(typerb->resval.rstring,"INSERT"/*DNT*/)) {
				rbsearch->resval.rreal*= (isSwapXYscalforIns == false ? xscl : yscl);
				if (zscl<0.0) rbsearch->resval.rreal = -rbsearch->resval.rreal;  // flip x scale if z scale negative
			}
			else if (!textent)
				rbsearch->resval.rreal*=yscl;
			else{
				if (!icadRealEqual(xscl*yscl,0.0))//attdef,attrib,text,shape
					rbsearch->resval.rreal*=fabs(xscl/yscl);
			}
		}else if (42==rbsearch->restype){
			if( strsame(typerb->resval.rstring,"VERTEX"/*DNT*/) )
				continue;
			if( strsame(typerb->resval.rstring,"LWPOLYLINE"/*DNT*/) ||
				strsame(typerb->resval.rstring,"HATCH"/*DNT*/) )
			{	/*D.Gavrilov*/// we should change bulges in these cases
				if((xscl<0.0 && yscl>0.0) || (xscl>0.0 && yscl<0.0))
					rbsearch->resval.rreal = -rbsearch->resval.rreal;
				continue;
			}
			if (strsame(typerb->resval.rstring,"INSERT"/*DNT*/)) {
				rbsearch->resval.rreal*= (isSwapXYscalforIns == false ? yscl : xscl);
				if (zscl<0.0) rbsearch->resval.rreal = -rbsearch->resval.rreal; // flip y scale if z scale negative
			}
			else
				rbsearch->resval.rreal*=zscl;//???
		}else if (strsame(typerb->resval.rstring,"HATCH"/*DNT*/) && (43 == rbsearch->restype)) {
			hatchpatternxrb = rbsearch;

		}else if (strsame(typerb->resval.rstring,"HATCH"/*DNT*/) && (44 == rbsearch->restype)) {
			if (!hatchpatternxrb)
				continue;

			sds_point patternpt = {hatchpatternxrb->resval.rreal, rbsearch->resval.rreal,pt210[2]};
			sds_trans(patternpt,&rbent,&rbwcs,0,pt2);

			pt2[0]-=blkinspt[0];
			pt2[1]-=blkinspt[1];
			pt2[2]-=blkinspt[2];

			pt2[0]*=xscl;
			pt2[1]*=yscl;
			pt2[2]*=zscl;

			pt3[0]=pt2[0];
			pt3[1]=pt2[1];

			pt2[0]=pt3[0]*Cos-pt3[1]*Sin;
			pt2[1]=pt3[1]*Cos+pt3[0]*Sin;

			pt2[0]+=inspt[0];
			pt2[1]+=inspt[1];
			pt2[2]+=inspt[2];

			sds_trans(pt2,&rbparent,&rbnewent,0,patternpt);

			hatchpatternxrb->resval.rreal = patternpt[0];
			rbsearch->resval.rreal = patternpt[1];

			hatchpatternxrb = NULL;

		}else if ((45 == rbsearch->restype) && strsame(typerb->resval.rstring,ResourceString(IDC_EXPLODE_HATCH_27, "HATCH" ))) {
			hatchpatternxrb = rbsearch;
		}else if ((46 == rbsearch->restype) && strsame(typerb->resval.rstring,ResourceString(IDC_EXPLODE_HATCH_27, "HATCH" ))) {
			if (!hatchpatternxrb)
				continue;

			sds_point patternpt = {hatchpatternxrb->resval.rreal, rbsearch->resval.rreal,pt210[2]};
			sds_trans(patternpt,&rbent,&rbwcs,0,pt2);

			pt3[0]=pt2[0];
			pt3[1]=pt2[1];

			pt2[0] = pt3[0]*Cos-pt3[1]*Sin;
			pt2[1] = pt3[1]*Cos+pt3[0]*Sin;

			pt2[0]*=xscl;
			pt2[1]*=yscl;

			sds_trans(pt2,&rbparent,&rbnewent,0,patternpt);

			hatchpatternxrb->resval.rreal = patternpt[0];
			rbsearch->resval.rreal = patternpt[1];

			hatchpatternxrb = NULL;
		}else if (43==rbsearch->restype || 39==rbsearch->restype){
			rbsearch->resval.rreal*=zscl;
		}else if (44<=rbsearch->restype && 49>=rbsearch->restype){
			if( 48 == rbsearch->restype ) continue; // EBATECH(CNBR) 2002/8/1 Bugzilla78263
			rbsearch->resval.rreal*=xscl;
		}else if (50<=rbsearch->restype && 59>=rbsearch->restype){

			if ( diment )
				{
				if ( (rbsearch->restype == 51) ||
					 (rbsearch->restype == 52) ||
					 (rbsearch->restype == 53) ||
					 (rbsearch->restype == 54) )
					 continue;

				}
			if (textent && 51==rbsearch->restype){
				if (xscl<0.0){
					//we'll flip upside down bit later, so flip angle
					rbsearch->resval.rreal+=IC_PI;
					ic_normang(&rbsearch->resval.rreal,NULL);
				}
				continue;
			}
			pt2[2]=0.0;
			pt2[0]=cos(rbsearch->resval.rreal);
			pt2[1]=sin(rbsearch->resval.rreal);
			sds_trans(pt2,&rbent,&rbwcs,0,pt3);
			pt3[0]*=xscl;
			pt3[1]*=yscl;
			pt3[2]*=zscl;
			pt2[0]=pt3[0]*Cos-pt3[1]*Sin;
			pt2[1]=pt3[1]*Cos+pt3[0]*Sin;
			pt2[2]=pt3[2];
			sds_trans(pt2,&rbparent,&rbnewent,1,pt3);
			rbsearch->resval.rreal=atan2(pt3[1],pt3[0]);
			if(strsame("INSERT",typerb->resval.rstring) && (xscl<0.0))
			{
				rbsearch->resval.rreal += IC_PI;
				ic_normang(&rbsearch->resval.rreal, NULL);
			}
		}else if (71==rbsearch->restype && textent && yscl*xscl<0.0){
			//flip bit for upside down
			//if (yscl<0.0){
				if (rbsearch->resval.rint&4)rbsearch->resval.rint-=4;
				else rbsearch->resval.rint+=4;
			//}else{
			//	if (rbsearch->resval.rint&2)rbsearch->resval.rint-=2;
			//	else rbsearch->resval.rint+=2;
			//}
		}else if (rbsearch->restype==210 && entinwcs){
			//wcs entity with an extrusion(e.g. mtext, point, line, ellipse)

			pt3[0]=rbsearch->resval.rpoint[0]*Cos-rbsearch->resval.rpoint[1]*Sin;
			pt3[1]=rbsearch->resval.rpoint[1]*Cos+rbsearch->resval.rpoint[0]*Sin;
			pt3[2]=rbsearch->resval.rpoint[2];
			pt3[0]*=xscl;
			pt3[1]*=yscl;
			pt3[2]*=zscl;

			ic_unitize(pt3);

			if(icadRealEqual(extru[0],0.0) && icadRealEqual(extru[1],0.0) && icadRealEqual(extru[2],1.0))	/*D.Gavrilov*/
				ic_ptcpy(rbsearch->resval.rpoint, pt3);			//some optimization
			else												//(don't rotate if needn't)
			{
				// 1.  build a pure rotation transformation rotxform that takes (0,0,1) to extru
				// 2.  apply rotxform to the unitized pt3 and put the result into rbsearch->resval.rpoint.
				if(!isMatrixReady)
				{
					sds_point Zdir;
					Zdir[0] = 0.0;
					Zdir[1] = 0.0;
					Zdir[2] = 1.0;
					ic_rotxform(Zdir, extru, rotxform);
					isMatrixReady = true;
				}
				ic_xformpt(pt3, rbsearch->resval.rpoint, rotxform);
			}
		}else if ((rbsearch->restype == 92) && strsame(typerb->resval.rstring, ResourceString(IDC_EXPLODE_HATCH_27, "HATCH" ))) {
			if (rbsearch->resval.rint & 2) {
				hatchpolyline = true;
				hatchellipse = false;
			}
			else
				hatchpolyline = false;
		}else if ((rbsearch->restype == 72) && strsame(typerb->resval.rstring, ResourceString(IDC_EXPLODE_HATCH_27, "HATCH" ))) {
			if (!hatchpolyline) {
				if (rbsearch->resval.rint == 3)
					hatchellipse = true;
				else
					hatchellipse = false;
			}
		}else if(rbsearch->restype == 1011)	{	// Point in WCS
			ic_ptcpy(pt2, rbsearch->resval.rpoint);
			pt2[0] -= blkinspt[0];
			pt2[1] -= blkinspt[1];
			pt2[2] -= blkinspt[2];
			pt2[0] *= xscl;
			pt2[1] *= yscl;
			pt2[2] *= zscl;
			pt3[0] = pt2[0]*Cos - pt2[1]*Sin;
			pt3[1] = pt2[1]*Cos + pt2[0]*Sin;
			pt3[2] = pt2[2];
			pt3[0] += inspt[0];
			pt3[1] += inspt[1];
			pt3[2] += inspt[2];
			ic_ptcpy(rbsearch->resval.rpoint, pt3);
			if(!entinwcs && strsame(typerb->resval.rstring, "POLYLINE"/*DNT*/))
				rbsearch->resval.rpoint[0] = rbsearch->resval.rpoint[1] = 0.0;
		}else if(rbsearch->restype == 1012 || rbsearch->restype == 1013) {
			pt3[0] = rbsearch->resval.rpoint[0]*Cos - rbsearch->resval.rpoint[1]*Sin;
			pt3[1] = rbsearch->resval.rpoint[1]*Cos + rbsearch->resval.rpoint[0]*Sin;
			pt3[2] = rbsearch->resval.rpoint[2];
			if(rbsearch->restype == 1012)
			{
				pt3[0] *= xscl;
				pt3[1] *= yscl;
				pt3[2] *= zscl;
			}

			ic_unitize(pt3);

			if(icadRealEqual(extru[0],0.0) && icadRealEqual(extru[1],0.0) && icadRealEqual(extru[2],1.0))
				ic_ptcpy(rbsearch->resval.rpoint, pt3);
			else
			{
				if(!isMatrixReady)
				{
					sds_point Zdir;
					Zdir[0] = 0.0;
					Zdir[1] = 0.0;
					Zdir[2] = 1.0;
					ic_rotxform(Zdir, extru, rotxform);
					isMatrixReady = true;
				}
				ic_xformpt(pt3, rbsearch->resval.rpoint, rotxform);
			}
		}

	} //for


// HACK if it's an arc, and the x and y scaling are appropriate, swap the start and end rotations
// pulled ellipse swap 12/16/98
	if (strsame("ARC",typerb->resval.rstring)) {
		if ((xscl<0.0 && yscl>0.0) || (xscl>0.0 && yscl<0.0)) {
			sds_real rot;
			struct resbuf *rotrb1=ic_ret_assoc(*subEntities, 50);
			rot=rotrb1->resval.rreal;
			struct resbuf *rotrb2=ic_ret_assoc(*subEntities, 51);
			rotrb1->resval.rreal=rotrb2->resval.rreal;
			rotrb2->resval.rreal=rot;
		}
	}
	/*D.Gavrilov*/// swap the start and end rotations for ELLIPSE (if need)
	// (this code is like the code for mirroring of ellipses)
	if(	strsame("ELLIPSE",typerb->resval.rstring) &&
		((xscl<0.0 && yscl>0.0) || (xscl>0.0 && yscl<0.0)) )
	{
		struct resbuf* rotrb1 = ic_ret_assoc(*subEntities, 41);
		struct resbuf* rotrb2 = ic_ret_assoc(*subEntities, 42);
		if (!icadAngleEqual(rotrb1->resval.rreal,0.0) || !icadAngleEqual(rotrb2->resval.rreal,IC_TWOPI))
		{
			//adjust start & end params
			sds_real rot = IC_TWOPI - rotrb1->resval.rreal;
			rotrb1->resval.rreal = IC_TWOPI - rotrb2->resval.rreal;
			rotrb2->resval.rreal = rot;
		}
	}

	/*D.Gavrilov*/// We should set the correct elevation.
	if(strsame("LWPOLYLINE"/*DNT*/, typerb->resval.rstring))
	{
		struct resbuf* prb10 = ic_ret_assoc(*subEntities, 10);
		struct resbuf* prb38 = ic_ret_assoc(*subEntities, 38);
		prb38->resval.rreal = prb10->resval.rpoint[2];
	}


//*** If the entity is a POLYLINE call this function recursively to make the verticies
//    if (strsame("POLYLINE"/*DNT*/, typerb->resval.rstring)) {
//        while(sds_entnext_noxref(enttmp,enttmp)==RTNORM) {
//            if ((rc=cmd_explode_makesubents(enttmp,inspt,blkinspt,xscl,yscl,zscl,rotang,extru,psflag,rbinherit))<0) goto bail;
//            if (rc==1L) rc=0L; goto bail;
//        }
//    } else if (strsame("SEQEND"/*DNT*/,typerb->resval.rstring)) rc=1L;

	return rc;

    bail:

    IC_RELRB(*subEntities);
    return(rc);
}

//<ALM>

short cmd_3dconvert()
{
    if (!CModeler::get()->canModify())
        return RTERROR;

    // Get Acis objects
    resbuf filter;
	filter.restype = 0;
	filter.resval.rstring = "REGION,3DSOLID,BODY"/*DNT*/;
	filter.rbnext = 0;

    sds_name ss1;
    RT rc;
	if ((rc = sds_pmtssget("\nSelect ACIS entities to convert: ",
	                 (GetActiveCommandQueue()->IsEmpty() && lsp_cmdhasmore==0) ? NULL : (LPCTSTR)""/*DNT*/,
		              NULL,NULL,&filter,ss1,1)) == RTERROR)
    {
		cmd_ErrorPrompt(CMD_ERR_NOENT,0);
		return rc;
	}
	else if (rc == RTCAN)
		return rc;

    sds_name ename;
	for (long ssct=0l; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++)
	{
        CDbAcisEntity* pObj = static_cast<CDbAcisEntity*>(reinterpret_cast<db_handitem*>(ename[0]));
        assert(pObj->isAcisEntity());

        xstd<point>::vector points;
        xstd<int>::vector faces;
        // Get mesh from object
        if (!CModeler::get()->getMeshRepresentation(pObj->getData(), points, faces))
            return RTERROR;

        // Get parent's layer
        char* szLayer = 0;
        pObj->get_8(&szLayer);

        // Get parent's color
        int color = -1;
        pObj->get_62(&color);

        // Create polyface mesh
        if (!createPolyfaceMesh(points, faces, szLayer, color))
            return RTERROR;

        if ((rc = sds_entdel(ename)) != RTNORM)
            return rc;
    }

    return RTNORM;
}

static
int createPolyfaceMesh
(
const xstd<point>::vector& aPoints,
const xstd<int>::vector& aFaces,
const char* szLayer,
int color
)
{
    if (!makePolyfaceMeshHeader(aPoints.size(), aFaces.size()>>2, szLayer, color))
        return 0;

    if (!makePolyfaceMeshPointVertices(aPoints, szLayer, color))
        return 0;

    if (!makePolyfaceMeshFaceVertices(aFaces, szLayer, color))
        return 0;

    if (!makeSeqend(szLayer, color))
        return 0;

    return 1;
}

static
int makePolyfaceMeshHeader(int nPoints, int nFaces, const char* layer, int color)
{
    // Calculate extrusion direction (as Z axe of UCS)
    resbuf rb;
    SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    sds_point vXDir;
    ic_ptcpy(vXDir, rb.resval.rpoint);

    SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    sds_point vYDir;
    ic_ptcpy(vYDir, rb.resval.rpoint);

    sds_point vZDir;
    ic_crossproduct(vXDir, vYDir, vZDir);

    sds_point ptDummy = {0., 0., 0.};

    // Build header list
    resbuf* elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                            8,layer,
                            62,color,
                            66,1,
                            70,64,
                            10,ptDummy,
                            71,nPoints,
                            72,nFaces,
                           210,vZDir,
                             0);
    if (!elist)
        return 0;

    // Make entity
    if (sds_entmake(elist)!=RTNORM)
    {
        IC_RELRB(elist);
        return 0;
    }

    IC_RELRB(elist);
    return 1;
}

static
int makePolyfaceMeshPointVertices
(
const xstd<point>::vector& aPoints,
const char* layer,
int color
)
{
    sds_point ptDummy = {0.,0.,0.};

    resbuf* elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                     8, layer,
                                    62, color,
                                    70,192,
                                    10,ptDummy,
                                    0);
    if (!elist)
        return 0;

    resbuf* g10 = elist;
    for (int iPoint = 0, nPoint = aPoints.size(); iPoint < nPoint; iPoint++)
    {
        ic_ptcpy(g10->rbnext->rbnext->rbnext->rbnext->resval.rpoint,
                                                            aPoints[iPoint]);
        if (sds_entmake(elist) != RTNORM)
        {
            IC_RELRB(elist);
            return 0;
        }
    }

    IC_RELRB(elist);
    return 1;
}

static
int makePolyfaceMeshFaceVertices
(
const xstd<int>::vector& faces,
const char* layer,
int color
)
{
    sds_point dummy = {0.,0.,0.};

    resbuf* elist = sds_buildlist(RTDXF0,"VERTEX" /*DNT*/,
                                 8, layer,
                                62, color,
                                70,128,
                                10,dummy,
                                71,0,
                                72,0,
                                73,0,
                                74,0,
                                 0);
    if (!elist)
        return 0;

    for (int iface = 0, nfaces = faces.size()>>2; iface < nfaces; ++iface)
    {
        const int* face = &faces[4*iface];

        for (resbuf* p = elist; p; p = p->rbnext)
        {
            int i = p->restype - 71;
            if (i>=0 && i<4)
                p->resval.rint = face[i];
        }

        if (sds_entmake(elist)!=RTNORM)
        {
            IC_RELRB(elist);
            return 0;
        }
    }

    IC_RELRB(elist);
    return 1;
}

static
int makeSeqend
(
const char* layer,
int color
)
{
    // Make the SEQEND:
    resbuf* elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                            8,layer,
                            62,color,
                            0);
    if (!elist)
        return 0;

    if (sds_entmake(elist)!=RTNORM)
    {
        IC_RELRB(elist);
        return 0;
    }

    IC_RELRB(elist);
    return 1;
}

static
int createBodiesOrRegions
(
const xstd<CDbAcisEntityData>::vector& objs,
const char* szLayer,
int color
)
{
    const char region[] = "REGION";
    const char body[] = "BODY";

    for (int i = 0, n = objs.size(); i < n; i++)
    {
        const char* type = body;
        if (objs[i].getDbtype() == DB_REGION)
            type = region;

        resbuf* elist = 0;
        if (szLayer && color != -1)
            elist = sds_buildlist(RTDXF0, type, 1, objs[i].getSat().c_str(),
                                                8, szLayer,
                                                62, color, 0);
        else if (szLayer)
            elist = sds_buildlist(RTDXF0, type, 1, objs[i].getSat().c_str(),
                                                8, szLayer, 0);
        else if (color >= 0)
            elist = sds_buildlist(RTDXF0, type, 1, objs[i].getSat().c_str(),
                                                62, color, 0);
        else
            elist = sds_buildlist(RTDXF0, type, 1, objs[i].getSat().c_str(), 0);

		if (!elist)
            break;

        if (sds_entmake(elist) != RTNORM)
        {
            sds_relrb(elist);
            break;
        }
        sds_relrb(elist);
    }
    return i == n;
}

static
int createCurves
(
xstd<geodata::curve*>::vector& curves,
const char* szLayer,
int color
)
{
    std::vector<geodata::curve*>::iterator icrv = curves.begin();
    for (; icrv != curves.end(); ++icrv)
    {
        if (!dbcstr::create(**icrv, szLayer, color))
            break;
    }

    return icrv == curves.end();
}

//</ALM>

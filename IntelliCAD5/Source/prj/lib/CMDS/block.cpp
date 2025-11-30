/* F:\DEV\PRJ\LIB\CMDS\BLOCK.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * //	BLOCK  command.
 */
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "db.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "fixup1005.h"/*DNT*/
#include "DbAcisEntity.h"/*DNT*/
#include "Modeler.h"/*DNT*/
#include "transf.h"/*DNT*/
using icl::transf;
#include "gvector.h"/*DNT*/
using icl::point;
using icl::gvector;
#include "IcadTextScrn.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/ // EBATECH(FUTA) 2001-01-17
#include "CmdQueryTools.h" //ucs2wcs(), wcs2ucs()

//Bugzilla No 78036 29-04-2002
#define IC_MINNAMELEN  32

//Bugzilla No 78136 06-05-2002
extern int	  SDS_IgnoreMenuWQuote;
//** Protos
long  cmd_block_makesubents(sds_name argename, sds_point inspt,sds_point pline_extru, ReactorFixups *reactorFixup);
void cmd_block_transEED(resbuf* elist, sds_point inspt, const resbuf* rbecs1, const resbuf* rbecs2, const resbuf* rbwcs, const resbuf* rbucs);

short cmd_block(void) 
{
//*** This function creates a block definition from a set of entities.
//***
//*** RETURNS:	RTNORM - Success.
//***			 RTCAN - User cancel.
//***		   RTERROR - RTERROR returned from an ads function.
//***

	if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) 
		return RTERROR;

	char blkname[IC_ACADNMLEN],fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	RT ret;
	int expert;					// Expert level for prompting.
	short alreadyset=0;
	struct resbuf *tmprb=NULL;
	struct resbuf setgetrb;		// For finding the value of "EXPERT"
	long rc=0L;
	sds_name ss2regen,etemp;

	ss2regen[0]=ss2regen[1]=0L;
	if (SDS_getvar(NULL,DB_QEXPERT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM) 
	{ 
		rc=RTERROR; 
		goto bail;
	}
	expert=setgetrb.resval.rint;

	//*** Block name
	for(;;) 
	{
		//Bugzilla No 78136 06-05-2002 [
		//if((ret=sds_getstring(0,ResourceString(IDC_BLOCK__NNAME_FOR_NEW_BLO_0, "\nName for new block, or ? to list existing blocks: " ),fs1))==RTERROR) {
		SDS_IgnoreMenuWQuote=1;
		if((ret=sds_getstring(1,ResourceString(IDC_BLOCK__NNAME_FOR_NEW_BLO_0, "\nName for new block, or ? to list existing blocks: " ),fs1))==RTERROR) 
		{
			rc=(-__LINE__); 
			goto bail;
		} 
		else 
			if (ret==RTCAN) 
			{
				rc=(-1L); 
				goto bail;
			}
		SDS_IgnoreMenuWQuote=0;
		//if (!*fs1) { rc=(-1L); goto bail; }
		ic_trim(fs1,"be");	// No.78136 2002/10/20 Block name accepts any spaces except beginning or tail spaces.
		//Bugzilla No 78136 06-05-2002 ]
		if ((*fs1=='?'/*DNT*/ && fs1[1]==0) || strisame(fs1, "_?"/*DNT*/)) 
		{
			rc=cmd_block_list(0); 
			goto bail;
		} 
		else 
		{
			strupr(fs1);
			if(!ic_isvalidname(fs1))
			{
				cmd_ErrorPrompt(CMD_ERR_BLOCKNAME,0);
				continue;
			}
			strncpy(blkname,fs1,sizeof(blkname)-1);
			if((tmprb=sds_tblsearch("BLOCK"/*DNT*/,blkname,0))!=NULL) 
			{
				IC_RELRB(tmprb);
				//Modified Cybage MM 03/12/2001 DD/MM/YYYY
				//Reason - Fix for Bug# 77940 from BugZilla
				char fs3[IC_ACADBUF];
				//*** If the block already exists, ask to redefine it.
				for (;;) 
				{
					if (expert<2) 
					{		// This prompt is suppressed at expert level 2 and above.  Assumes "Yes".
						sprintf(fs2,ResourceString(IDC_BLOCK__NTHE_BLOCK__S_ARE_1, "\nThe block %s already exists. Do you want to redefine it? <N> " ),fs1);
						sds_initget(RSG_OTHER,ResourceString(IDC_BLOCK_INITGET_YES_REDEFI_2, "Yes-Redefine_block|Yes No-Do_not_redefine|No ~_Yes ~_No" ));
					   	//Modified Cybage MM 03/12/2001 DD/MM/YYYY
						//Reason - Fix for Bug# 77940 from BugZilla[
						//ret=sds_getkword(fs2,fs1);
						ret=sds_getkword(fs2,fs3);
						//Modified Cybage MM 03/12/2001 DD/MM/YYYY ]
						if (ret==RTERROR) 
						{
							rc=(-__LINE__); 
							goto bail;
						} 
						else 
							if (ret==RTCAN) 
							{
								rc=(-1L); 
								goto bail;
							} 
							else 
								if (ret==RTNONE) 
								{
									rc=0L; 
									goto bail;
								} 
								else 
									if (ret==RTNORM) 
									{
										//Modified Cybage MM 03/12/2001 DD/MM/YYYY
										//Reason - Fix for Bug# 77940 from BugZilla[
										//if (strisame(fs1,"NO"/*DNT*/ )) {
										if (strisame(fs3,"NO"/*DNT*/ )) 
										{
											//Modified Cybage MM 03/12/2001 DD/MM/YYYY ]
											rc=0L; 
											goto bail;
										}
										//Modified Cybage MM 03/12/2001 DD/MM/YYYY
										//Reason - Fix for Bug# 77940 from BugZilla[
										//else if (strisame(fs1,"YES"/*DNT*/ )) {
										else 
											if (strisame(fs3,"YES"/*DNT*/ )) 
											{
												//Modified Cybage MM 03/12/2001 DD/MM/YYYY ]
												if(NULL==(tmprb=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,blkname,0)))
												{
													rc=(-1L);
													goto bail;
												}
												//grab set to update, but if there are none just continue w/o error
												if(RTNORM!=(rc=sds_pmtssget(NULL,"A"/*DNT*/ ,NULL,NULL,tmprb,ss2regen,0)))
												{
													ss2regen[0]=ss2regen[1]=0L;
												}
												IC_RELRB(tmprb);
												rc=0L; 
												break;
											} 
											else 
											{
												cmd_ErrorPrompt(CMD_ERR_YESNO,0);  // "Yes or No, please"
												continue;
											}
									}
					} 
					else 
					{
						if(NULL==(tmprb=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,blkname,0)))
						{
							rc=(-1L);
							goto bail;
						}
						if(RTNORM!=(rc=sds_pmtssget(NULL,"A"/*DNT*/ ,NULL,NULL,tmprb,ss2regen,0)))
						{
							ss2regen[0]=ss2regen[1]=0L;
						}
						IC_RELRB(tmprb);
						rc=0L; 
						break;
					}
				}
			}
		}
		break;
	}

	ret=cmd_make_block(blkname);
	if(ret==RTNORM && 0L!=ss2regen[0])
	{
		for(rc=0L;RTNORM==sds_ssname(ss2regen,rc,etemp);sds_entupd(etemp),rc++);
	}
	return(ret);

bail:
	//Bugzilla No 78136 06-05-2002
	SDS_IgnoreMenuWQuote=0;
	if(rc==0L) 
		return(RTNORM);
	if(rc==(-1L)) 
		return(RTCAN);
	if(rc<(-1L)) 
		CMD_INTERNAL_MSG(rc);
	if(0L!=ss2regen[0])
		sds_ssfree(ss2regen);
	return(RTERROR);
}

short cmd_checkBlockSS(const char* pBlockName, sds_name blockSS, short& blockFlag70)
{
	struct resbuf* pEntityList = NULL;
	sds_name sdsEntityName;
	long ssct = 0;
	sds_sslength(blockSS, &ssct);
	if(ssct!=0L)
	{
		// Make the ents of the block in reverse order so attribs are prompted for correctly.
		for(--ssct; sds_ssname(blockSS, ssct, sdsEntityName) == RTNORM; ssct--) 
		{
			pEntityList = sds_entget(sdsEntityName);
			ASSERT(pEntityList != NULL);

			//*** Check for self-referencing block
			// this check needs to be expanded to check for nested circularity
			if(ic_assoc(pEntityList, 0) == 0 && strsame("INSERT"/*DNT*/, ic_rbassoc->resval.rstring)) 
			{
				if(ic_assoc(pEntityList, 2) == 0 && strsame(pBlockName, ic_rbassoc->resval.rstring)) 
				{
					sds_printf(ResourceString(IDC_BLOCK__N___BLOCK__S_CANN_7, "\n-- Block %s cannot be self referencing. --" ), pBlockName);
					for(ssct = 0L; sds_ssname(blockSS, ssct, sdsEntityName) == RTNORM; ssct++)
						sds_redraw(sdsEntityName, IC_REDRAW_UNHILITE);
					sds_relrb(pEntityList);
					return 0;
				}
			}
			//*** Set the attributes follow flag if there are any ATTDEFs in selection set
			if(ic_assoc(pEntityList, 0) == 0 && strsame("ATTDEF"/*DNT*/, ic_rbassoc->resval.rstring) && !(blockFlag70 & 0x02)) 
				blockFlag70 |= 0x02;
			sds_relrb(pEntityList);
		}
	}
	return 1;
}

short cmd_createBlock(sds_name sdsBlockName, const char* pBlockName, short blockFlag70, sds_point insertPoint, sds_name blockSS, db_drawing* pDrawing)
{
	sds_name sdsEntityName;
	sds_point ip = {0.0, 0.0, 0.0};
	struct resbuf* pEntityList = NULL;
	ReactorFixups reactorFixup;
	sds_name copySS;
	// track the 1005 handles in eed so that they
	// can be fixed up to be in sync later
	pDrawing->Make1005FixupTables();

	//*** Create block.
	pEntityList = sds_buildlist(RTDXF0, "BLOCK"/*DNT*/,
		2, pBlockName,
		70, blockFlag70,
		10, ip,
		RTNONE);
	ASSERT(pEntityList != NULL);

	int result = sds_entmake(pEntityList);
	ASSERT(result == RTNORM);
	sds_relrb(pEntityList);

	// Send notification to apps with selection set of objects selected for the new block
	SDS_CallUserCallbackFunc(SDS_CBBEGINCLONE, (void*)blockSS, NULL, NULL);

	// Track Reactors
	pDrawing->SetCopyingFlag(true);

	//*** Create block subentities
	for(long ssct = 0L; sds_ssname(blockSS, ssct, sdsEntityName) == RTNORM; ssct++) 
	{
		//*** Function for subentities (recursive for nested inserts).
		result = cmd_block_makesubents(sdsEntityName, insertPoint, NULL, &reactorFixup);
		if(result != 0)
		{
			sds_entmake(NULL);
			pDrawing->Delete1005FixupTables();
			return 0;
		}
	}

	//fix reactors
	reactorFixup.ApplyFixups(SDS_CURDWG);
	pDrawing->SetCopyingFlag(false);

	//*** Create the ENDBLK subentity
	pEntityList = sds_buildlist(RTDXF0, "ENDBLK"/*DNT*/, RTNONE);
	ASSERT(pEntityList != NULL);

	result = sds_entmake(pEntityList);
	ASSERT(result == RTKWORD);
	sds_relrb(pEntityList);

	// Send notification to apps with final selection set
	db_blocktabrec*	pBlock = (db_blocktabrec*)pDrawing->ret_lastblockdef();
	sdsBlockName[0] = (long)pBlock;
	sdsBlockName[1] = (long)pDrawing;
	SDS_sscpy(copySS, blockSS);
	SDS_CallUserCallbackFunc(SDS_CBENDCLONE, (void *)copySS, (void*)sdsBlockName, (void*)1);
	sds_ssfree(copySS);

	// make sure the 1005 handles are in sync in eed
	if(pDrawing->FixingUp1005s())
	{
		pDrawing->Apply1005Fixups();
		pDrawing->Delete1005FixupTables();
	}
	return 1;
}

short cmd_make_block(char *blkname) 
{
//*** This function creates a block definition.
//***
//*** RETURNS:	RTNORM - Success.
//***			 RTCAN - User cancel.
//***		   RTERROR - RTERROR returned from an ads function.
//***
	struct resbuf *tmprb=NULL;
	sds_point pt1;
	sds_name ss1 = {0L, 0L};			// Initialized to stop Purify warnings
	sds_name sdsBlockName = {0L, 0L};	// Initialized to stop Purify warnings
	int ret;
	short blockFlag70 = 0;
	long rc=0L;

	//*** Insertion point.
	for(;;) 
	{
		if (sds_initget(RSG_NONULL,NULL)!=RTNORM) 
		{ 
			rc=(-__LINE__); 
			goto bail; 
		}
		if ((ret=sds_getpoint(NULL,ResourceString(IDC_BLOCK__NINSERTION_POINT__5, "\nInsertion point for new block: " ),pt1))==RTERROR) 
		{
			rc=(-__LINE__); 
			goto bail;
		} 
		else 
			if(ret==RTCAN) 
			{
				rc=(-1L); 
				goto bail;
			} 
			else 
				if(ret==RTNORM) 
				{
					break;
				}
	}
	//*** Get selection set of entities.
	if((ret=sds_pmtssget(ResourceString(IDC_BLOCK__NSELECT_ENTITIES__6, "\nSelect entities for block: " ),NULL,&tmprb,NULL,NULL,ss1,0))==RTCAN) 
	{
		rc=(-1L); 
		goto bail;
	} 
	else 
		if(ret==RTERROR) 
		{
			sds_ssadd(NULL,NULL,ss1);
			ret=RTNORM;
		}
	if(tmprb != NULL)
	{
		sds_relrb(tmprb);
		tmprb=NULL;
	}

	if(cmd_checkBlockSS(blkname, ss1, blockFlag70) == 0)
	{
		rc = (-__LINE__); 
		goto bail; 
	}

	if(cmd_createBlock(sdsBlockName, blkname, blockFlag70, pt1, ss1, SDS_CURDWG) == 0)
	{
		ASSERT(false);
		goto bail;
	}

	//*** Delete the entities
	ret = cmd_erase_and_oops(ss1, 0);
	if(ret == RTERROR) 
	{
		rc = (-__LINE__); 
		goto bail;
	} 
	else 
		if(ret == RTCAN) 
		{
			rc = (-1L); 
			goto bail;
		}

	struct resbuf setgetrb;
	setgetrb.restype = RTSTR;
	setgetrb.resval.rstring = blkname;
	if(sds_setvar("INSNAME"/*DNT*/,&setgetrb) != RTNORM) 
	{ 
		rc = (-__LINE__); 
		goto bail; 
	}

bail:
	sds_entmake(NULL); 
	sds_ssfree(ss1);
	IC_RELRB(tmprb);

	if (SDS_CURDWG->FixingUp1005s())
		SDS_CURDWG->Delete1005FixupTables();

	// EBATECH(FUTA)-[ 2001-01-17
	// if RTNORM ,clear PrevSelection
	if((rc == 0L) && SDS_CURDOC)
	{
		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_ssadd(NULL, NULL, SDS_CURDOC->m_pPrevSelection);
	}// ]-EBATECH(FUTA)

	if(rc == 0L) 
		return(RTNORM);
	if(rc == (-1L)) 
		return(RTCAN);
	if(rc < (-1L)) 
		CMD_INTERNAL_MSG(rc);
	return(RTERROR);
}

long cmd_block_makesubents(sds_name argename, sds_point inspt,sds_point pline_extru, ReactorFixups *reactorFixup) 
{
//*** This function is recursive for blocks inside blocks inside blocks...
//*** It also adds the attributes for inserts and the vertecies for polylines.
//*** Isnested is used to determine if this is a block definition call, or we're
//***	we're making an insert of an already-defined block (no point translation)
//***
//*** RETURNS:	0 - Success.
//***			1 - SEQEND was just made for attributes
//***		   -1 - User cancel.
//***		   -2 - RTERROR returned from an ads function.
//***
//** when making a vertex entity, pass pline extru for point conversion
	struct resbuf *elist,*trb,rbecs1,rbecs2,rbwcs,rbucs;
	sds_point extru,pt1;
	sds_name ename;
	long rc;
	int type,fi1,pl3d;
	BOOL did210;
	BOOL did38 = FALSE;
	db_handitem *hip;
	sds_point temppt;

	bool hatchellipse = false; //needed to determine if the code 11 is a elliptical edge of a hatch
							   //if so the insertion point should not be added to it as it is a vector.
	bool hatchpolyline = false; //also needed to determine if code 11 is an elliptical arc.  If it is a polyline then
								//the flag for hatchellipse should be false.

	rc=0L; elist=NULL;
	if(argename==NULL) 
		return(-2);
	ic_encpy(ename,argename);
	hip=(db_handitem *)ename[0];
	type=hip->ret_type();
	rbecs1.restype=RTSTR; 
	rbecs1.resval.rstring="*"/*DNT*/ ; 
	rbecs1.rbnext=NULL;
	if((elist=sds_entgetx(ename,&rbecs1))==NULL) 
	{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		rc=(-__LINE__); 
		goto bail;
	}
	//set up transformation resbufs.  for planar ents, we'll reset them to the old and new extru
	rbecs1.rbnext=rbecs2.rbnext=rbucs.rbnext=rbwcs.rbnext=NULL;
	rbecs1.restype=rbecs2.restype=rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=rbecs1.resval.rint=rbecs2.resval.rint=0;

	//*** Check for an insert with attibutes.
	//NOTE: We're going to play a trick w/type. If type is DB_POLYLINE or DB_VERTEX, we need to
	//know if its 3D or not.  If they're 3d, we'll set them to their negative.	(neither is 0)
	if(type==DB_INSERT || type==DB_POLYLINE || type==DB_DIMENSION)
	{
		pl3d=0;
		if(type!=DB_POLYLINE)
			hip->get_66(&fi1);
		if(type==DB_POLYLINE || 1==fi1) 
		{
			hip->get_210(extru);
			if(type==DB_POLYLINE)
			{
				hip->get_70(&pl3d);
				pl3d=pl3d&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH);
				if (pl3d) 
					type = -type; // negate type for 3d poly!!!
			}
			//before we entmake the polyline, we need to get it's new extrusion & elevation...
			if(!pl3d)
			{
				rbecs1.restype=rbecs2.restype=RT3DPOINT;
				ic_ptcpy(rbecs1.resval.rpoint,extru);
				sds_trans(extru,&rbwcs,&rbucs,1,rbecs2.resval.rpoint);
				ic_assoc(elist,210);
				ic_ptcpy(ic_rbassoc->resval.rpoint,rbecs2.resval.rpoint);
			}
			ic_assoc(elist,10);
			sds_trans(ic_rbassoc->resval.rpoint,&rbecs1,&rbucs,0,ic_rbassoc->resval.rpoint);
			ic_rbassoc->resval.rpoint[0]-=inspt[0];
			ic_rbassoc->resval.rpoint[1]-=inspt[1];
			ic_rbassoc->resval.rpoint[2]-=inspt[2];
			sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbecs2,0,ic_rbassoc->resval.rpoint);
			if(type==DB_POLYLINE)
				ic_rbassoc->resval.rpoint[0]=ic_rbassoc->resval.rpoint[1]=0.0;//just need z

			/*D.Gavrilov*/// EED should be processed too.
			cmd_block_transEED(elist, inspt, &rbecs1, &rbecs2, &rbwcs, &rbucs);

			if(sds_entmake(elist)!=RTNORM) 
			{ 
				rc=(-__LINE__); 
				goto bail; 
			}

			// store reactors
			if (reactorFixup)
			{
				reactorFixup->AddHanditem(SDS_LastHandItem);
				reactorFixup->AddHandleMap(hip->RetHandle(), SDS_LastHandItem->RetHandle());
			}

			while(sds_entnext_noxref(ename,ename)==RTNORM) 
			{
				if((rc=cmd_block_makesubents(ename,inspt,(type==DB_POLYLINE && !pl3d)? extru : NULL, reactorFixup))<0L) 
					goto bail;
				if(rc==1L) 
				{ 
					rc=0L; 
					goto bail; 
				}
			}
		}
	} 
	else 
		if(type==DB_SEQEND) 
		{
			if(sds_entmake(elist)!=RTNORM) 
			{ 
				rc=(-__LINE__); 
				goto bail; 
			}
			rc=1L; 
			goto bail;
		} 
		else 
			if(type==DB_VERTEX && pline_extru==NULL)	
				type= -type;//negate type for 3d vertex

	did210=FALSE;
	switch (type)
	{
		case DB_CIRCLE:
		case DB_ARC:
		case DB_SOLID:
		case DB_POLYLINE:	//2d only
		case DB_VERTEX:		//2d only
		case DB_TRACE:
		case DB_TEXT:
		case DB_ATTRIB:
		case DB_ATTDEF:
		case DB_MTEXT:
		case DB_SHAPE:
		case DB_INSERT:
		case DB_HATCH:
		// DP: added case for DB_LWPOLYLINE
		case DB_LWPOLYLINE:
			{
				//now reset ecs1 resbuf correctly for ecs entity
				if(type==DB_VERTEX)
				{	//remember, 2d here
					if(pline_extru==NULL)
					{ 
						rc=(-__LINE__); 
						goto bail; 
					}
					ic_ptcpy(rbecs1.resval.rpoint,pline_extru);
				}
				else
				{
					hip->get_210(rbecs1.resval.rpoint);
				}
				sds_trans(rbecs1.resval.rpoint,&rbwcs,&rbucs,1,rbecs2.resval.rpoint);	//rbecs is set up for NEW orientation of ent
				rbecs1.restype=rbecs2.restype=RT3DPOINT;
				did210=TRUE;
			}
			break;

		//<ALM>
		case DB_BODY	:
		case DB_3DSOLID :
		case DB_REGION	:
			resbuf*	pSatRb = ic_ret_assoc(elist, 1);
			if (pSatRb)
			{
				point insert(inspt);
				insert *= ucs2wcs();

				transf t;
				t.translate(-(insert - point(0.,0.,0.)));
				t *= wcs2ucs();

				CDbAcisEntityData newObj;
				CDbAcisEntity* pObj = static_cast<CDbAcisEntity*>(hip);
				if (CModeler::get()->transform(pObj->getData(), newObj, t))
				{
					char*& sat = pSatRb->resval.rstring;
					IC_FREE(sat);
					sat = new char [newObj.getSat().size()+1];
					strcpy(sat, newObj.getSat().c_str());
				}
			}
		//</ALM>
	}	// switch(type)

	for(trb=elist;trb!=NULL;trb=trb->rbnext)
	{
		//		if(trb->restype==-3)break;//TODO - see what ACAD does w/EED
		if (trb->restype==1011) // translate WCS point
		{
			//			sds_trans(trb->resval.rpoint,&rbecs1,&rbucs,0,trb->resval.rpoint);	/*D.G.*/// It's WCS point!
			trb->resval.rpoint[0]-=inspt[0];
			trb->resval.rpoint[1]-=inspt[1];
			trb->resval.rpoint[2]-=inspt[2];
			//			sds_trans(trb->resval.rpoint,&rbwcs,&rbecs2,0,trb->resval.rpoint);	/*D.G.*/
		}
		else 
			if (trb->restype==1012 || trb->restype==1013) // WCS vectors
			{
				/* transform (0,0,0) and the old vector to get the new: */
				temppt[0]=temppt[1]=temppt[2]=0.0;
				//			sds_trans(temppt,&rbecs1,&rbucs,0,temppt);							/*D.G.*/// It's WCS point!
				//			sds_trans(trb->resval.rpoint,&rbecs1,&rbucs,0,trb->resval.rpoint);	/*D.G.*/// It's WCS point!
				trb->resval.rpoint[0]-=inspt[0];
				trb->resval.rpoint[1]-=inspt[1];
				trb->resval.rpoint[2]-=inspt[2];
				temppt[0]-=inspt[0];
				temppt[1]-=inspt[1];
				temppt[2]-=inspt[2];
				//			sds_trans(temppt,&rbwcs,&rbecs2,0,temppt);							/*D.G.*/
				//			sds_trans(trb->resval.rpoint,&rbwcs,&rbecs2,0,trb->resval.rpoint);	/*D.G.*/
				trb->resval.rpoint[0]-=temppt[0];
				trb->resval.rpoint[1]-=temppt[1];
				trb->resval.rpoint[2]-=temppt[2];
			}
			else 
				if(trb->restype==210)
				{
					if(did210)
						ic_ptcpy(trb->resval.rpoint,rbecs2.resval.rpoint);
					//wcs ent w/210, so the transform just reduces to this...
					else 
						sds_trans(trb->resval.rpoint,&rbwcs,&rbucs,1,trb->resval.rpoint);
				}
				else 
					if(trb->restype<=18 && trb->restype>=10)
					{
						//**POINTS**
						//make sure point is not vector
						if(trb->restype==11 && (type==DB_RAY || type==DB_XLINE || type==DB_ELLIPSE || type==DB_TOLERANCE || type==DB_MTEXT || type==DB_IMAGE || hatchellipse))
						{
							//vector
							sds_trans(trb->resval.rpoint,&rbecs1,&rbucs,1,trb->resval.rpoint);
							sds_trans(trb->resval.rpoint,&rbwcs,&rbecs2,1,trb->resval.rpoint);
						}
						else 
							if((trb->restype==12 || trb->restype==13) && (type==DB_SPLINE || type ==DB_IMAGE))
							{
								//vector
								sds_trans(trb->resval.rpoint,&rbecs1,&rbucs,1,trb->resval.rpoint);
								sds_trans(trb->resval.rpoint,&rbwcs,&rbecs2,1,trb->resval.rpoint);
							}
							else
							{
								//point.  we've already setup the resbufs accordingly
								sds_trans(trb->resval.rpoint,&rbecs1,&rbucs,0,trb->resval.rpoint);
								trb->resval.rpoint[0]-=inspt[0];
								trb->resval.rpoint[1]-=inspt[1];
								trb->resval.rpoint[2]-=inspt[2];
								sds_trans(trb->resval.rpoint,&rbwcs,&rbecs2,0,trb->resval.rpoint);
								
								// DP: we need to update elevation
								// DP: TODO: it's not well solution, so we need another
								if(!did38 && type == DB_LWPOLYLINE)
								{
									ic_assoc(elist, 38);
									ic_rbassoc->resval.rreal = trb->resval.rpoint[2];
									did38 = TRUE;
								}
							}
					}
					else 
						if(trb->restype==50 || trb->restype==51 || trb->restype==52 || trb->restype==53)
						{
							//angles
							pt1[0]=cos(trb->resval.rreal);
							pt1[1]=sin(trb->resval.rreal);
							pt1[2]=0.0;
							sds_trans(pt1,&rbecs1,&rbucs,1,pt1);
							sds_trans(pt1,&rbwcs,&rbecs2,1,pt1);
							pt1[2]=sqrt(pt1[0]*pt1[0]+pt1[1]*pt1[1]);
							if(fabs(pt1[2])<CMD_FUZZ)trb->resval.rreal=0.0;
							else{
								pt1[0]/=pt1[2];
								pt1[1]/=pt1[2];
								pt1[2]=0.0;
								trb->resval.rreal=atan2(pt1[1],pt1[0]);
							}
						}
						else
							if((trb->restype == 92) && (type == DB_HATCH)) 
							{
								if (trb->resval.rint & 2) {
									hatchpolyline = true;
									hatchellipse = false;
								}
								else
									hatchpolyline = false;
							}
							else 
								if ((trb->restype == 72) && (type == DB_HATCH)) 
								{
									if (!hatchpolyline) {
										if (trb->resval.rint == 3)
											hatchellipse = true;
										else
											hatchellipse = false;
									}
								}
	}

	if(sds_entmake(elist)!=RTNORM) 
	{ 
		rc=(-__LINE__); 
		goto bail; 
	}

	// store reactors
	if (reactorFixup)
	{
		reactorFixup->AddHanditem(SDS_LastHandItem);
		reactorFixup->AddHandleMap(hip->RetHandle(), SDS_LastHandItem->RetHandle());
	}

bail:
	IC_FREEIT;
	return(rc);
}

long cmd_block_list(bool mode) 
{
//*** mode	0=list all blocks
//***		1=list xrefs only
//*** This function lists defined block information.
//***
	char blkstr[IC_ACADBUF],xPath[IC_ACADBUF],*pstr=NULL,szTmp[IC_FILESIZE+IC_PATHSIZE],*path,prompt[IC_ACADBUF];
	struct resbuf *tmprb=NULL,*rbUnSort=NULL,*rbbUnSort=NULL,*rbbSort=NULL,setgetrb,*pRb=NULL;
	int ret,scrollmax,scrollcur,maxsort;
	long rc=0L;
	int allflg=0,usrblks=0,unnamed=0,exrefs=0,depend=0,overlay=0,xFlag=0;

	SDS_getvar(NULL,DB_QMAXSORT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	maxsort=setgetrb.resval.rint;
	if(mode)
		sprintf(prompt,ResourceString(IDC_BLOCK__NXREF_S__TO_LIST__8, "\nXref(s) to list <* for all>: " ));
	else
		sprintf(prompt,ResourceString(IDC_BLOCK__NBLOCK_S__TO_LIST_9, "\nBlock(s) to list <* for all>: " ));

	if((ret=sds_getstring(0,prompt,blkstr))==RTERROR) 
	{
		rc=(-__LINE__); 
		goto bail;
	} 
	else 
		if(ret==RTCAN) 
		{
			rc=(-1L); 
			goto bail;
		} 
		else 
			if(*blkstr==0)
			{
				blkstr[0]='*';
				blkstr[1]=0;
			}
	sds_textscr();

	tmprb=sds_tblnext("BLOCK"/*DNT*/,1); //*** No need to check what this returns - If it returns NULL there are no blocks in the drawing.
	while(tmprb!=NULL) 
	{
		if(sds_usrbrk()) 
		{ 
			rc=(-1L); 
			goto bail; 
		}
		//*** Increment type flags
		if(ic_assoc(tmprb,70)!=0) 
		{ 
			rc=(-__LINE__); 
			goto bail; 
		}
		if(ic_rbassoc->resval.rint & IC_BLOCK_ANONYMOUS) 
			unnamed++;
		if(ic_rbassoc->resval.rint & IC_BLOCK_XREF)  
		{
			xFlag|=IC_BLOCK_XREF;  
			exrefs++;
		}
		if(ic_rbassoc->resval.rint & IC_BLOCK_XREF_OVERLAID)  
		{
			xFlag|=IC_BLOCK_XREF_OVERLAID;  
			overlay++;
		}
		if(ic_rbassoc->resval.rint & IC_ENTRY_XREF_DEPENDENT) 
		{
			xFlag|=IC_ENTRY_XREF_DEPENDENT; 
			depend++;
		}
		//*** Xref path.
		/*DG - 28.3.2003*/// Currently acad returns the path in dxf1, icad - in dxf3; so check both.
		if(mode == 1 &&	xFlag & (IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID) &&	(!ic_assoc(tmprb, 1) || !ic_assoc(tmprb, 3)))
			path = ic_rbassoc->resval.rstring;
		else
			path = NULL;
		//*** Block names.
		if(ic_assoc(tmprb,2)==0) 
		{
			//*** If it is an anonymous block or layout block, ignore it.
			if(*ic_rbassoc->resval.rstring!='*') 
			{
				if(sds_wcmatch(strupr(ic_rbassoc->resval.rstring),blkstr)==RTNORM) 
				{
					if(mode==1)
					{
						if(xFlag & ( IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID ))
						{
							if(rbbUnSort==NULL)
								rbbUnSort=rbUnSort=sds_newrb(RTSTR);
							else
								rbUnSort=rbUnSort->rbnext=sds_newrb(RTSTR);
							rbUnSort->rbnext=NULL;
							if(xFlag & IC_BLOCK_XREF_OVERLAID ) 
							{
								//Bugzilla No 78036 29-04-2002 [
								sprintf(szTmp,"%-*s%s %s",IC_MINNAMELEN,ic_rbassoc->resval.rstring,ResourceString(IDC_CMDS3_OVERLAY_490	,	"Overlay"),path);
								rbUnSort->resval.rstring= new char [strlen(szTmp)+1];
								strcpy(rbUnSort->resval.rstring,szTmp);
							}
							else 
								if(xFlag & IC_BLOCK_XREF) 
								{
									sprintf(szTmp,"%-*s%s  %s",IC_MINNAMELEN,ic_rbassoc->resval.rstring,ResourceString(IDC_CMDS3_ATTACH_489	,	"Attach"),path);
									//Bugzilla No 78036 29-04-2002 ]
									rbUnSort->resval.rstring= new char [strlen(szTmp)+1];
									strcpy(rbUnSort->resval.rstring,szTmp);
								}
						}
						else
						{
							usrblks--;
						}
					}
					else
					{
						if(rbbUnSort==NULL)
							rbbUnSort=rbUnSort=sds_newrb(RTSTR);
						else
							rbUnSort=rbUnSort->rbnext=sds_newrb(RTSTR);
						rbUnSort->rbnext=NULL;
						if(xFlag & IC_BLOCK_XREF) 
						{
							rbUnSort->resval.rstring= new char [strlen(ic_rbassoc->resval.rstring)+strlen(ResourceString(IDC_BLOCK_____XREF__12, "   (xRef)" ))+1];
							strcpy(rbUnSort->resval.rstring,strupr(ic_rbassoc->resval.rstring));
							strcat(rbUnSort->resval.rstring,ResourceString(IDC_BLOCK_____XREF__12, "   (xRef)" ));
						}
						else 
							if(xFlag & IC_ENTRY_XREF_DEPENDENT) 
							{
								pstr=strchr(ic_rbassoc->resval.rstring,CMD_XREF_TBLDIVIDER);
								*pstr=*xPath=0;
								if(pstr) 
									strcpy(xPath,pstr+1);
								rbUnSort->resval.rstring= new char [strlen(ic_rbassoc->resval.rstring)+strlen(xPath)+strlen(ResourceString(IDC_BLOCK_____XDEP__13, "   (xDep)" ))+8];
								strcpy(rbUnSort->resval.rstring,strupr(xPath));
								strcat(rbUnSort->resval.rstring,ResourceString(IDC_BLOCK_____XDEP__13, "   (xDep)" ));
								strcat(rbUnSort->resval.rstring,strupr(ic_rbassoc->resval.rstring));
							}
							else
							{
								rbUnSort->resval.rstring= new char [strlen(ic_rbassoc->resval.rstring)+1];
								strcpy(rbUnSort->resval.rstring,strupr(ic_rbassoc->resval.rstring));
							}
					}
					xFlag=0;
				}
			}
		}
		usrblks++;
		IC_RELRB(tmprb);
		//*** Do NOT check for NULL here.  NULL is checked in the while().
		tmprb=sds_tblnext("BLOCK"/*DNT*/,0);
	}
	if(usrblks==0)
	{
		if(mode)
			sds_printf(ResourceString(IDC_BLOCK_NO_XREFS_DEFINED_14, "No Xrefs defined" ));
		else
			sds_printf(ResourceString(IDC_BLOCK_NO_BLOCKS_DEFINED_15, "No Blocks defined" ));
		goto bail; //if there are no blocks exit;
	}
	else
	{
		if(mode)
		{
			sds_printf(ResourceString(IDC_BLOCK__NXREFS_DEFINED_I_16, "\nXrefs defined in this drawing: " ));
			sds_printf(ResourceString(IDC_BLOCK__N__NAME__________17, "\n  Name                            Type    Path" ));
			sds_printf(ResourceString(IDC_BLOCK__N________________18, "\n  ----                            ----    ----" ));
		}
		else
		{
			sds_printf(ResourceString(IDC_BLOCK__NBLOCKS_DEFINED__19, "\nBlocks defined in this drawing: " ));
		}
	}

	if(SDS_getvar(NULL,DB_QSCRLHIST,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)	
	{ 
		rc=(-1L); 
		goto bail; 
	}
	scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,setgetrb.resval.rint);
	scrollmax-=3;
	scrollcur=0;

	if(maxsort>=usrblks)
	{
		rbbSort=cmd_listsorter(rbbUnSort);
	}
	else
	{
		rbbSort=rbbUnSort;
		rbbUnSort=NULL;
	}

	for(pRb=rbbSort;pRb!=NULL;pRb=pRb->rbnext)
	{
			sds_printf(ResourceString(IDC_BLOCK__N___S_20, "\n  %s" ),pRb->resval.rstring);
			if(cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) 
			{
				rc=(-1L);
				goto bail;
			}
	}

	if(mode==0)
	{
		usrblks-=(exrefs+depend+unnamed);
		sds_printf(ResourceString(IDC_BLOCK__N_NUSER_____EXTE_21, "\n\nUser     External     Dependent   Unnamed" ));
		if(cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) {rc=(-1L);goto bail;}
		sds_printf(ResourceString(IDC_BLOCK__NBLOCKS___REFERE_22, "\nBlocks   References   Blocks      Blocks" ));
		if(cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) {rc=(-1L);goto bail;}
		sds_printf(ResourceString(IDC_BLOCK__N_5D______5D_____23, "\n%5d     %5d       %5d       %5d\n" ),usrblks,exrefs,depend,unnamed);
		if(cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) {rc=(-1L);goto bail;}
	}
bail:

	IC_RELRB(tmprb);
	IC_RELRB(rbbSort);
	IC_RELRB(rbbUnSort);
	return(rc);
}

/*D.Gavrilov*/// Called from cmd_block_makesubents.
void cmd_block_transEED(resbuf* elist, sds_point inspt, const resbuf* rbecs1, const resbuf* rbecs2, const resbuf* rbwcs, const resbuf* rbucs)
{
	resbuf* trb;
	sds_point temppt;
	for(trb = elist; trb != NULL; trb = trb->rbnext)
	{
		switch(trb->restype)
		{
		case 1011:		// translate WCS point
//			sds_trans(trb->resval.rpoint, rbecs1, rbucs, 0, trb->resval.rpoint);	/*D.G.*/// It's WCS point!
			trb->resval.rpoint[0] -= inspt[0];
			trb->resval.rpoint[1] -= inspt[1];
			trb->resval.rpoint[2] -= inspt[2];
//			sds_trans(trb->resval.rpoint, rbwcs, rbecs2, 0, trb->resval.rpoint);	/*D.G.*/
			break;
		case 1012:
		case 1013:		// WCS vectors
			/* transform (0,0,0) and the old vector to get the new: */
			temppt[0] = temppt[1] = temppt[2] = 0.0;
//			sds_trans(temppt, rbecs1, rbucs, 0, temppt);							/*D.G.*/// It's WCS point!
//			sds_trans(trb->resval.rpoint, rbecs1, rbucs, 0, trb->resval.rpoint);	/*D.G.*/// It's WCS point!
			trb->resval.rpoint[0] -= inspt[0];
			trb->resval.rpoint[1] -= inspt[1];
			trb->resval.rpoint[2] -= inspt[2];
			temppt[0] -= inspt[0];
			temppt[1] -= inspt[1];
			temppt[2] -= inspt[2];
//			sds_trans(temppt, rbwcs, rbecs2, 0, temppt);							/*D.G.*/
//			sds_trans(trb->resval.rpoint, rbwcs, rbecs2, 0, trb->resval.rpoint);	/*D.G.*/
			trb->resval.rpoint[0] -= temppt[0];
			trb->resval.rpoint[1] -= temppt[1];
			trb->resval.rpoint[2] -= temppt[2];
			break;
		}
	}
}


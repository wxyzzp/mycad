/* G:\ICADDEV\PRJ\LIB\CMDS\READ.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "styletabrec.h"
#include "layertabrec.h"
#include "ltypetabrec.h"
#include "IcadApi.h"
#include "fixup1005.h"
#include "modeler.h"

bool IsAnonBlkName(char *str)
{
	if (str[0]=='*'/*DNT*/ && 
		(str[1]=='D' || str[1]=='X' || str[1]=='U' || str[1]=='B'))	/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
		return true;
	return false;
}


bool AnonTypeMatchesChar(int thetype,char thechar)
{
	if (0==thetype && thechar=='D'/*DNT*/) return true;
	if (1==thetype && thechar=='X'/*DNT*/) return true;
	if (2==thetype && (thechar=='U'/*DNT*/ || thechar=='B'/*DNT*/))	/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
		return true;
	return false;
}

void RemovePersistentReactors(struct resbuf *rb)
// removes the persistent reactors from a resbuf list
// this function assumes that the persistent reactors are not the FIRST thing in the list, which they should never be
// Ebatech(CNBR) 2003/4/30 Bugzilla#78510 Use RemovePersistentReactors in Exploding MINSERT, too.
{
	struct resbuf *trb,*rb102,*tmprb;

	ASSERT(rb);
	if (rb==NULL) return;

	for (trb=rb; trb!=NULL; trb=trb->rbnext)
	{
		if (trb->rbnext!=NULL && trb->rbnext->restype==102)
		{
			rb102=trb->rbnext;
			for (tmprb=rb102->rbnext; tmprb!=NULL; tmprb=tmprb->rbnext)
			{
				if (tmprb->restype==102)
				{
					trb->rbnext=tmprb->rbnext;
					tmprb->rbnext=NULL;
					sds_relrb(rb102);
					rb102=tmprb=NULL;
					break;
				}
			}
			ASSERT(rb102==NULL);
			if (rb102!=NULL) continue; // error condition, didn't find match
		}
	}
}

//TODO!! -  remove this temp f'n after we make f'n to insert one
//	dwg into another (yeah, right!  this pile o'crap does far too much stuff)
int cmd_readdwg_makeblks(struct resbuf *blkelist, db_drawing *flp,char *xrefstr,struct cmd_anon_blklst *anonbeg, struct cmd_anon_blklst **anoncurrent,int xrefmode,int xrefidx,
						 sds_matrix transmat)
{
//*** This is a recursive function that is called to create blocks and nested blocks.
	//if xrefmode==IC_INSMODE_XREF_ATTACH, IC_INSMODE_XREF_OVERLAY, or
	// IC_INSMODE_XREF_UPDATE, we're making an xref.  xrefidx is id# of block which is the xref

	//temp for ver 1.0, we can make blocks w/mode IC_INSMODE_XREF_BIND or mode IC_INSMODE_XREF_INSERT;

	char fs1[IC_ACADBUF],fs3[IC_ACADBUF],xref[IC_ACADNMLEN],numstr[6],*fcp1;
	struct resbuf *elist=NULL,*tmprb,*trb,*trb2,app;
	sds_name ename,ename2;
	RT  ret=RTNORM;
	int fi1;
	struct cmd_anon_blklst *anontemp,*anoncur;
	//Bugzilla No 77860,78047 22-04-2002 [
	// set reactor fixup tables
	SDS_CURDWG->SetCopyingFlag(true);
	ReactorFixups reactorFixup;
	//Bugzilla No 77860,78047 22-04-2002 ]

	app.restype=RTSTR; app.resval.rstring="*"/*DNT*/; app.rbnext=NULL;

	anontemp=anoncur=NULL;
	trb=trb2=NULL;
	//*** If this BLOCK is already defined then return.
	if (ic_assoc(blkelist,2)!=0)
	{
		ret=RTERROR; 
		goto bail; 
	}
	tmprb=ic_rbassoc;//DON'T MOVE TMPRB - it points at blk name

	if (IsAnonBlkName(tmprb->resval.rstring))
	{
		if (NULL==anoncurrent || NULL==anonbeg)
		{
			//if an anon blk and no link to update, then return error
			ret=RTERROR;
			goto bail;
		}
		anoncur=*anoncurrent;
	}

	if (xrefmode==IC_INSMODE_XREF_BIND && *tmprb->resval.rstring!='*'/*DNT*/)
	{
		ic_assoc(blkelist,70);
		if (!(ic_rbassoc->resval.rint & IC_BLOCK_XREF))
		{//if not the xref itself...
			//this is the tricky one.  We're going to convert the block name w/dividers
			//	We'll do this differently than the anon blks.  we'll make the block under 
			//	its new name,and then just 
			for (fi1=0;fi1<9999;fi1++)
			{
				sprintf(fs1,"%s%c%-d%c%s",xrefstr,CMD_XREF_TBLBINDER,fi1,CMD_XREF_TBLBINDER,tmprb->resval.rstring);
				fs1[IC_ACADNMLEN]=0;  // it would be better to return error, but this emulates the original code

				//now we have the block to make, but first, check to see if new block already exists
				if (trb!=NULL)
				{
					sds_relrb(trb);trb=NULL;
				}
				if (NULL==(trb=sds_tblsearch("BLOCK"/*DNT*/,fs1,0)))break;
			}
			if (NULL==(tmprb->resval.rstring=ic_realloc_char(tmprb->resval.rstring,strlen(fs1)+1)))
			{
				if (elist!=NULL)
				{
					sds_relrb(elist);elist=NULL;
				}
				return(RTERROR);
			}
			strcpy(tmprb->resval.rstring,fs1);
		}
	}//else
	if (xrefstr==NULL || *xrefstr==0)
	{
		fcp1=xref;
		xref[0]=0;
	}
	else if (*tmprb->resval.rstring=='*'/*DNT*/ &&
			 (xrefmode==IC_INSMODE_XREF_ATTACH ||
			  xrefmode==IC_INSMODE_XREF_OVERLAY ||
			  xrefmode==IC_INSMODE_XREF_UPDATE))
	{
		/*This stuff should never get used in ver 1.0 because we're not importing blocks
			when we attach an xref
		if(NULL==anoncur){	//block can't be both xref and anonymous
			ret=RTERROR;
			goto bail;
		}
		strncpy(xref,tmprb->resval.rstring,2);
		fcp1=&xref[2];
		*fcp1=CMD_XREF_TBLDIVIDER;
		fcp1++;
		*fcp1=0;
		strncat(xref,"REF",(IC_ACADNMLEN-strlen(xref)-1));
		if(NULL==(tmprb->resval.rstring=(char *)realloc(tmprb->resval.rstring,strlen(xref)+1))){
			if(elist!=NULL){sds_relrb(elist);elist=NULL;}
			return(RTERROR);
		}
		strcpy(tmprb->resval.rstring,xref);
		fcp1+=3;
		//*fcp1=0;
		*/
	}
	else if (xrefmode!=IC_INSMODE_XREF_BIND)
	{
		if (NULL!=anoncur)
		{
			ret=RTERROR;
			goto bail;
		}
		strncpy(xref,xrefstr,sizeof(xref)-1);
		for (fcp1=xref;*fcp1!=0;fcp1++);
		strncat(xref,tmprb->resval.rstring,(IC_ACADNMLEN-strlen(xref)-1));
		if (NULL==(tmprb->resval.rstring=ic_realloc_char(tmprb->resval.rstring,strlen(xref)+1)))
		{
			if (elist!=NULL)
			{
				sds_relrb(elist);elist=NULL;
			}
			return(RTERROR);
		}
		strcpy(tmprb->resval.rstring,xref);
		*fcp1=0;
	}
	if (anoncur)
	{
		//lop off the number portion of the anon block name
		*(tmprb->resval.rstring+2)=0;
	}

	if (NULL==anoncur && (trb=sds_tblsearch("BLOCK"/*DNT*/,tmprb->resval.rstring,0))!=NULL)
	{
		sds_relrb(trb);
		ret=RTNORM; goto bail;
	}

	tmprb=NULL;//Marker - tmprb no longer pointing at name

	//*** Get the ename of the first entity in the BLOCK.
	for (tmprb=blkelist; tmprb!=NULL; tmprb=tmprb->rbnext)
	{
		if (tmprb->restype==-2)
		{
			ic_encpy(ename,tmprb->resval.rlname);
		}
		if (tmprb->restype==70)
		{
			if (xrefstr!=NULL && *xrefstr)
			{
				if (xrefmode<IC_INSMODE_XREF_BIND) tmprb->resval.rint|=(IC_XREF_RESOLVED | IC_ENTRY_XREF_DEPENDENT);
				else tmprb->resval.rint&=~(IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID | IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED);
			}
		}
	}
	//*** Search the BLOCK's entities for an INSERT and check to see if the BLOCK has
	//*** been defined for that INSERT, if it has not - call this function recursively.
	for (ic_encpy(ename2,ename); ename2[0]!=0L; ename2[0]=(long)flp->entnext_noxref((db_handitem *)ename2[0]))
	{
		//*** Empty block bug! (same as an ENDBLK) ~|
		if (ename2[0]==0 || ename2[1]==0) break;
		fi1=IC_TYPE_FROM_ENAME(ename2);
		if (fi1==DB_ENDBLK)	break;//*** This check needs to be here in case the -2 of ename is an ENDBLK.
		if (fi1!=DB_INSERT && fi1!=DB_DIMENSION) continue;

		if (elist!=NULL)
		{
			sds_relrb(elist);
			elist=NULL;
		}
		if ((elist=((db_handitem *)ename2[0])->entgetx(&app,flp))==NULL)
		{
			cmd_ErrorPrompt(CMD_ERR_CREATEBLOCK,0,(void *)(__LINE__));
			ret=RTERROR; goto bail;
		}

//        if(ic_assoc(elist,0)!=0) { ret=RTERROR; goto bail; }

		//there's a very very slight chance for error here if binding an xref with
		//	the same name as a previously bound one which also has a duplicate block
		//	name which is forward-nesting.  We always grab the last binding block 
		//	definition (the one w/the biggest # between the $'s), and assume that it
		//	came from the current drawing.  If this block is forward-nesting AND there's
		//	a previously bound xref w/the same block, we'll use that block definition
		//	instead of the forward-nested one in the current drawing.
		if ((tmprb=ic_ret_assoc(elist,2))==NULL)
		{
			ret=RTERROR; 
			goto bail; 
		}
		if (IsAnonBlkName(tmprb->resval.rstring))
		{
			//if our block has an anonymous block nested within it, check to see if
			//	we've already read in the anonymous block...
			for (anontemp=anonbeg; anontemp!=NULL; anontemp=anontemp->next)
			{
				if (!AnonTypeMatchesChar(anontemp->type,tmprb->resval.rstring[1])) continue;
				if (anontemp->oldnum == atoi(tmprb->resval.rstring+2)) break;
			}
			if (anontemp!=NULL)continue;
			if (NULL==(anontemp= new cmd_anon_blklst))
			{
				ret=RTERROR;
				goto bail;
			}
			anontemp->next=NULL;
			if (tmprb->resval.rstring[1]=='D'/*DNT*/)anontemp->type=0;
			else if (tmprb->resval.rstring[1]=='X'/*DNT*/)anontemp->type=1;
			else anontemp->type=2;
			anontemp->oldnum=atoi(tmprb->resval.rstring+2);	/*D.G.*/// Use tmprb instead of ic_rbassoc.
			if (anonbeg==NULL)
				anonbeg=anoncur=anontemp;
			else
			{
				anoncur->next=anontemp;
				anoncur=anoncur->next;
			}
			trb=SDS_tblgetter("BLOCK"/*DNT*/,tmprb->resval.rstring,0,flp);
			if (elist!=NULL)
			{
				sds_relrb(elist);
				elist=NULL;
			}
			elist=trb;trb=tmprb=NULL;
			if (elist!=NULL)
			{
				if ((ret=cmd_readdwg_makeblks(elist,flp,xrefstr,anonbeg,&anoncur,xrefmode,xrefidx,NULL))!=RTNORM) goto bail;
			}

		}
		else if (xrefmode==IC_INSMODE_XREF_BIND)
		{
			//realloc the insert so that it has the last version of the block(highest
			//	# between the $'s
			for (fi1=0;fi1<9999;fi1++)
			{
				sprintf(fs1,"%s%c%-d%c%s",xrefstr,CMD_XREF_TBLBINDER,fi1,CMD_XREF_TBLBINDER,tmprb->resval.rstring);
				fs1[IC_ACADNMLEN]=0; // again it would be better to return an error, but this emulates the original code
				//now we have the block to make, but first, check to see if new block already exists
				if (trb!=NULL)
				{
					sds_relrb(trb);
					trb=NULL;
				}
				if (NULL==(trb=sds_tblsearch("BLOCK"/*DNT*/,fs1,0))) break;
			}
			//fi1 has the FIRST number which doesn't appear in the drawing as a bound block.
			//if fi1=0, then this block must be forward-nesting.  Make a recursive call
			// and it will give us back the block name in the 2 group of the elist, which
			// we know will match fs1;
			if (0==fi1)
			{
				trb=SDS_tblgetter("BLOCK"/*DNT*/,tmprb->resval.rstring,0,flp);
				if (elist!=NULL)
				{
					sds_relrb(elist);
					elist=NULL;
				}
				elist=trb;
				trb=NULL;
				if (elist!=NULL)
				{
					if ((ret=cmd_readdwg_makeblks(elist,flp,xrefstr,anonbeg,&anoncur,xrefmode,xrefidx,NULL))!=RTNORM) goto bail;
					//tmprb->resval.rstring=(char *)realloc(tmprb->resval.rstring,strlen(fs1)+1);
					//strcpy(tmprb->resval.rstring,fs1);
				}
			}
			/*else{
				fi1--;//step back to last valid bound block definition
				strncpy(fs1,xrefstr,sizeof(fs1)-1);
				for(fcp2=fs1;*fcp2!=0;fcp2++);
				*fcp2=CMD_XREF_TBLBINDER;
				fcp2++;
				*fcp2=0;
				itoa(fi1,numstr,10);
				strcat(fs2,numstr);
				//LEAVE FCP2 where it is - we may need to increment it if the block exists
				do{
					fcp2++;
				}while((*fcp2)!=0);
				*fcp2=CMD_XREF_TBLBINDER;
				*(fcp2+1)=0;
				strncat(fs1,tmprb->resval.rstring,sizeof(fs2)-1);
				tmprb->resval.rstring=(char *)realloc(tmprb->resval.rstring,strlen(fs1)+1);
				strcpy(tmprb->resval.rstring,fs1);
			}*/

		}
		else if ((trb=sds_tblsearch("BLOCK"/*DNT*/,tmprb->resval.rstring,0))==NULL)
		{
			//*** Find the block in table
			//NOTE: tmprb points into elist, so don't free elist
			trb=SDS_tblgetter("BLOCK"/*DNT*/,tmprb->resval.rstring,0,flp);
			if (elist!=NULL)
			{
				sds_relrb(elist);elist=NULL;
			}
			elist=trb;trb=tmprb=NULL;
			if (elist!=NULL)
			{
				if ((ret=cmd_readdwg_makeblks(elist,flp,xrefstr,anonbeg,&anoncur,xrefmode,xrefidx,NULL))!=RTNORM) goto bail;
			}
		}
		else sds_relrb(trb);

	}
	if (elist!=NULL)
	{
		sds_relrb(elist);elist=NULL;
	}
	tmprb=NULL;//pointed into name of blk in elist

	//Note: if anon blk, we've already deleted number from name
	if (sds_entmake(blkelist)==RTERROR)
	{
		cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
		#if defined(DEBUG)
		sds_textscr();
		ic_listdxf(blkelist);blkelist=NULL;//lister frees llist
		#endif
		//sds_getstring(0,"\nPaused...",fs1);
		ret=RTERROR; 
		goto bail;
	}
	else
	{
		for (ret=RTNORM; ename[0]!=0L; ename[0]=(long)flp->entnext_noxref((db_handitem *)ename[0]))
		{
			//*** Empty block bug! (same as an ENDBLK) ~|
			if (ename[0]==0 || ename[1]==0)	break;
			if (elist!=NULL)
			{
				sds_relrb(elist); elist=NULL;
			}
			if ((elist=((db_handitem *)ename[0])->entgetx(&app,flp))==NULL)
			{
				cmd_ErrorPrompt(CMD_ERR_CREATEBLOCK,0,(void *)(__LINE__));
				ret=RTERROR; goto bail;
			}

//			RemovePersistentReactors(elist);

			for (trb=elist;trb!=NULL;trb=trb->rbnext)
			{
				if (trb->restype==0)
				{
					if (strsame("ENDBLK"/*DNT*/,trb->resval.rstring)) goto mak_endblk;
					//DON'T make endblk in this loop!  We need to trap return code
					else ic_rbassoc=trb;
					//XREFs
				}
				else if (xrefmode==IC_INSMODE_XREF_BIND)
				{
					if ((trb->restype==6 &&  RTNORM!=sds_wcmatch(trb->resval.rstring,"BYLAYER,BYBLOCK,CONTINUOUS,ByLayer,ByBlock,Continuous"/*DNT*/))||
						(trb->restype==8 && RTNORM!=sds_wcmatch(trb->resval.rstring,"0,DEFPOINTS,Defpoints"/*DNT*/))||
						(trb->restype==7) ||
						(trb->restype==3 && strsame(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/))	/* || */
#ifdef REMOVED
						(trb->restype==1001 && RTNORM!=sds_wcmatch(trb->resval.rstring,"ACAD"/*DNT*/))
#endif
					   )
					{
						for (fi1=0;fi1<9999;fi1++)
						{
							sprintf(fs1,"%s%c%-d%c%s",xrefstr,CMD_XREF_TBLBINDER,fi1,CMD_XREF_TBLBINDER,trb->resval.rstring);
							fs1[IC_ACADNMLEN]=0; // for the third time, it would be better to return error but we emulate the original code
							//now we have the block to make, but first, check to see if new block already exists
							if (trb2!=NULL)
							{
								sds_relrb(trb2);trb2=NULL;
							}
							switch (trb->restype)
							{
							case 6:  trb2=sds_tblsearch("LTYPE"/*DNT*/,fs1,0);break;
							case 8:  trb2=sds_tblsearch("LAYER"/*DNT*/,fs1,0);break;
							case 7:  trb2=sds_tblsearch("STYLE"/*DNT*/,fs1,0);break;
							case 3:  trb2=sds_tblsearch("DIMSTYLE"/*DNT*/,fs1,0);break;
							default: trb2=sds_tblsearch("APPID"/*DNT*/,fs1,0);break;
							}
							if (NULL==trb2)break;
							strncpy(fs3,fs1,sizeof(fs3)-1);
						}
						if (0==fi1 /*&& trb->restype!=7*/)
						{ // style is an exception
							ret=RTERROR;
							goto bail;
						}
						trb->resval.rstring=ic_realloc_char(trb->resval.rstring,strlen(fs3)+1);
						strcpy(trb->resval.rstring,fs3);

					}
				}
				else if (xrefmode>=IC_INSMODE_XREF_ATTACH)
				{
					// should never happen in release 1.0 because we're not reading block entities on bind
//					if((trb->restype==6 &&  RTNORM!=sds_wcmatch(trb->resval.rstring,"BYLAYER,BYBLOCK,CONTINUOUS"/*DNT*/))||
//						(trb->restype==8 && RTNORM!=sds_wcmatch(trb->resval.rstring,"0,DEFPOINTS"/*DNT*/))||
//						(trb->restype==7 && RTNORM!=sds_wcmatch(trb->resval.rstring,"TEXT,ATTDEF"/*DNT*/))||
//						(trb->restype==3 && strsame(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/))||
//						(trb->restype==1001)){
//							strncpy(fs1,xrefstr,sizeof(fs1)-1);
//							strncat(fs1,trb->resval.rstring,sizeof(fs1)-strlen(fs1)-1);
//							trb->resval.rstring=(char *)realloc(trb->resval.rstring,strlen(fs1)+1);
//							strcpy(trb->resval.rstring,fs1);
//					}

				}
				//ANONYMOUS BLKs
				if (trb->restype==2 && *trb->resval.rstring=='*'/*DNT*/ && 
					RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,"INSERT,DIMENSION"/*DNT*/))
				{
					//substitute the new anonymous name from the llist...
					for (anontemp=anonbeg; anontemp!=NULL; anontemp=anontemp->next)
					{
						if (!AnonTypeMatchesChar(anontemp->type,trb->resval.rstring[1])) continue;
						if (anontemp->oldnum == atoi(trb->resval.rstring+2))break;
					}
					if (anontemp!=NULL)
					{
						//if we found the new number to use...
						itoa(anontemp->newnum,numstr,10);
						char UserAnBlkChar = 'U'/*DNT*/;
						/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
						if (trb->resval.rstring[1] == 'B'/*DNT*/)
							UserAnBlkChar = 'B'/*DNT*/;
						switch (anontemp->type)
						{
						case 0: 
							if (xrefmode<7 || xrefmode>=10)sprintf(fs1,"*D%s"/*DNT*/,numstr);
							else sprintf(fs1,"*D|REF%s"/*DNT*/,numstr);
							break;
						case 1:     
							if (xrefmode<7 || xrefmode>=10)sprintf(fs1,"*X%s"/*DNT*/,numstr);
							else sprintf(fs1,"*X|REF%s"/*DNT*/,numstr);
							break;
						default:
							if (xrefmode<7 || xrefmode>=10)sprintf(fs1,"*%c%s"/*DNT*/, UserAnBlkChar, numstr);
							else sprintf(fs1,"*%c|REF%s"/*DNT*/, UserAnBlkChar, numstr);
						}
						if (strlen(fs1)!=strlen(trb->resval.rstring))
						{
							//if the string lengths of old & new numbers don't match, 
							//	then realloc the things
							if (NULL==(trb->resval.rstring=ic_realloc_char(trb->resval.rstring,strlen(fs1)+1)))
							{
								if (elist!=NULL)
								{
									sds_relrb(elist);elist=NULL;
								}
								return(RTERROR);
							}
						}
						strcpy(trb->resval.rstring,fs1);
					}
				}
				else if (xrefmode==IC_INSMODE_XREF_BIND && trb->restype==2 && *trb->resval.rstring!='*'/*DNT*/ && strsame(ic_rbassoc->resval.rstring,"INSERT"/*DNT*/))
				{
					*fs3=0;
					for (fi1=0; fi1<9999; fi1++)
					{
						sprintf(fs1,"%s%c%-d%c%s",xrefstr,CMD_XREF_TBLBINDER,fi1,CMD_XREF_TBLBINDER,trb->resval.rstring);
						fs1[IC_ACADNMLEN]=0;  // fourth time
						//now we have the block to make, but first, check to see if new block already exists
						if (tmprb!=NULL)
						{
							sds_relrb(tmprb);tmprb=NULL;
						}
						if (NULL==(tmprb=sds_tblsearch("BLOCK"/*DNT*/,fs1,0)))break;
						strncpy(fs3,fs1,sizeof(fs3)-1);
					}
					if (!*fs3)
					{
						ret=RTERROR;
						goto bail;
					}
					if (strlen(fs3)!=strlen(trb->resval.rstring))
					{
						//if the string lengths of old & new numbers don't match, 
						//	then realloc the things
						if (NULL==(trb->resval.rstring=ic_realloc_char(trb->resval.rstring,strlen(fs3)+1)))
						{
							if (elist!=NULL)
							{
								sds_relrb(elist);elist=NULL;
							}
							return(RTERROR);
						}
					}
					strcpy(trb->resval.rstring,fs3);
				}
			}   

// translate entities in dimensions iff we are doing a top level dimension, indicated by the presence of a transmat
			if (!IC_IS_XREF_INSMODE(xrefmode) && transmat!=NULL)
			{
				SDS_simplexforment(NULL,elist,transmat);
			}

			if (sds_entmake(elist)!=RTNORM)
			{
				// TODO - Are Building and Defining a block the same thing?
				// If not, we need another error message to cover this.
				cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
				// sds_printf("\nError building block.");
				#if defined(DEBUG)
				sds_textscr();
				ic_listdxf(elist);elist=NULL;//lister frees llist
				#endif
				//if(RTCAN==(ret=sds_getstring(0,"\nPaused...",fs1)))goto bail;
				ret=RTERROR;
				goto bail;
			}
			ic_assoc(elist,-1);
			ic_encpy(ename,ic_rbassoc->resval.rlname);

			if (IC_TYPE_FROM_ENAME(ename)==DB_MLINE)
			{
				sds_name newename;
				newename[0] = (long)SDS_LastHandItem;
				newename[1] = (long)SDS_CURDWG;;
				SDS_updateMLine(ename, newename);
			}
			//Bugzilla No 77860,78047 22-04-2002 [
			reactorFixup.AddHanditem(SDS_LastHandItem);
			reactorFixup.AddHandleMap(((db_handitem*)ename[0])->RetHandle(), SDS_LastHandItem->RetHandle());

		}
		// apply reactor fixups
		reactorFixup.ApplyFixups(SDS_CURDWG);
		SDS_CURDWG->SetCopyingFlag(false);
		//Bugzilla No 77860,78047 22-04-2002 ]
		if (elist!=NULL)
		{
			sds_relrb(elist);elist=NULL;
		}
	}
	mak_endblk:
	if (elist!=NULL)
	{
		sds_relrb(elist);elist=NULL;
	}
	if ((elist=sds_buildlist(RTDXF0,"ENDBLK"/*DNT*/,0))==NULL)
	{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR; goto bail;
	}
	if ((ret=sds_entmake(elist))==RTERROR || ret==RTREJ)
	{
		// TODO - Can we get away with this not specifically saying "endblock"?
		// If not, this will need a new error message...
		cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
		#if defined(DEBUG)
		sds_textscr();
		ic_listdxf(elist);elist=NULL;//lister frees llist
		#endif
		//if(RTCAN==(ret=sds_getstring(0,"\nPaused...",fs1)))goto bail;
		goto bail;      
	}

	if (ret==RTKWORD)
	{
		sds_getinput(fs1);
		if (NULL!=anoncur && *fs1=='*'/*DNT*/)
		{
			if (!AnonTypeMatchesChar(anoncur->type,fs1[1]))
				ret=RTERROR;
			if (ret==RTERROR) goto bail;
			for (fcp1=fs1; *fcp1!=0 && !isdigit((unsigned char) *fcp1); fcp1++);
			if (*fcp1) anoncur->newnum=atoi(fcp1);
		}
		db_handitem *hip;
		hip=SDS_CURDWG->findtabrec(DB_BLOCKTAB,fs1,1);
		if (NULL!=hip)
			((db_tablerecord *)hip)->set_xrefidx(xrefidx);
	}
	ret=RTNORM;

	bail:

	if (elist!=NULL)
	{
		sds_relrb(elist);elist=NULL;
	}

	return(ret);
}


short TableNameLengthsLegalForXref(db_drawing *xrefdp,char *blkname)
// Determines whether the table entry names, as they would be listed in the top level (i.e. BLOCK|TBLNAME), are legal for xrefing.
// To be legal, they must end up with length <=255 characters
// most probably this is a "flexible" 255 characters, where Unicode chars would count as only 1 character despite taking up
// 7 bytes, but this is a subtlety we will have to handle later.
{
	db_handitem *hip;
	db_layertabrec *xreflayhip;
	db_ltypetabrec *xrefltypehip;
	db_ltypedashlink *ltdash;
	char *ltypename;

	ASSERT(xrefdp!=NULL);
	ASSERT(blkname!=NULL);

	for (hip=xrefdp->tblnext(DB_LAYERTABREC,1); hip!=NULL; hip=xrefdp->tblnext(DB_LAYERTABREC,0))
	{
		xreflayhip=(db_layertabrec *)hip;
		if (strsame(xreflayhip->retp_name(),"0"/*DNT*/)) continue; // don't add layer 0
		if (strchr(xreflayhip->retp_name(),'|')!=NULL) continue;  // if this layer is itself from an xref below, don't mess with it
		if (strlen(blkname)+strlen(xreflayhip->retp_name())+1 > IC_ACADNMLEN-1)
		{
			sds_printf(ResourceString(IDS_XREF_LAYNAME_TOO_LONG,"The combination of block name %s and layer name %s\nexceeds the maximum allowable table name size of \n255 characters.  Xref operation aborted."),
					   blkname,xreflayhip->retp_name());
			return RTERROR;
		}

		xrefltypehip=(db_ltypetabrec *)xreflayhip->ret_ltype();
		ltypename=xrefltypehip->retp_name();
		if (!strisame(ltypename,"BYLAYER"/*DNT*/) && !strisame(ltypename,"BYBLOCK"/*DNT*/) && !strisame(ltypename,"CONTINUOUS"/*DNT*/))
		{
			if (strlen(blkname)+strlen(ltypename)+1 > IC_ACADNMLEN-1)
			{
				sds_printf(ResourceString(IDS_XREF_LTYPENAME_TOO_LONG,"The combination of block name %s and linetype name %s\nexceeds the maximum allowable table name size of \n255 characters.  Xref operation aborted."),
						   blkname,ltypename);
				return RTERROR;
			}
			ltdash=xrefltypehip->retp_dash();
			if (ltdash!=NULL)
			{
				for (int i=0; i<xrefltypehip->ret_ndashes() && i<DB_MAXLTDASHES; i++,ltdash++)
				{
					if (ltdash->stylehip!=NULL)
					{
						if (!(strlen(((db_styletabrec *)ltdash->stylehip)->retp_name())==0 &&
							  (((db_styletabrec *)ltdash->stylehip)->ret_flags() & IC_SHAPEFILE_SHAPES)))
						{
							if (strlen(blkname)+strlen(((db_styletabrec *)ltdash->stylehip)->retp_name()+1) > IC_ACADNMLEN-1)
							{
								sds_printf(ResourceString(IDS_XREF_STYLENAME_TOO_LONG,"The combination of block name %s and style name %s\nexceeds the maximum allowable table name size of \n255 characters.  Xref operation aborted."),
										   blkname,((db_styletabrec *)ltdash->stylehip)->retp_name());
								return RTERROR;
							}
						}
					}
				}
			}
		}
	}
	return(RTNORM);
}

// This class allows to set the current UCS to world (temporarily).
// Its constructor and destructor perform all the necessary work.
// Author: Dmitry Gavrilov.
class CUcsDisabler
{
public :
	CUcsDisabler(db_drawing* pDrw, db_charbuflink* pCfg, db_charbuflink* pSes);
	~CUcsDisabler();
private :
	resbuf          m_ucsorg, m_ucsxdir, m_ucsydir, m_worlducs;
	db_drawing*     m_pDrw;
	db_charbuflink  *m_pCfg, *m_pSes;
};

CUcsDisabler::CUcsDisabler(db_drawing* pDrw, db_charbuflink* pCfg, db_charbuflink* pSes) : m_pDrw(pDrw), m_pCfg(pCfg), m_pSes(pSes)
{
	SDS_getvar(NULL, DB_QWORLDUCS, &m_worlducs, m_pDrw, m_pCfg, m_pSes);
	if (m_worlducs.resval.rint)
		return;

	// get current ucs values
	SDS_getvar(NULL, DB_QUCSORG,  &m_ucsorg,  m_pDrw, m_pCfg, m_pSes);
	SDS_getvar(NULL, DB_QUCSXDIR, &m_ucsxdir, m_pDrw, m_pCfg, m_pSes);
	SDS_getvar(NULL, DB_QUCSYDIR, &m_ucsydir, m_pDrw, m_pCfg, m_pSes);

	resbuf  rb;
	memset(&rb, 0, sizeof(rb));

	// set ucs to world
	rb.restype = RT3DPOINT;
	SDS_setvar(NULL, DB_QUCSORG,  &rb, m_pDrw, m_pCfg, m_pSes, 0);
	rb.resval.rpoint[0] = 1.0;
	SDS_setvar(NULL, DB_QUCSXDIR, &rb, m_pDrw, m_pCfg, m_pSes, 0);
	rb.resval.rpoint[0] = 0.0;
	rb.resval.rpoint[1] = 1.0;
	SDS_setvar(NULL, DB_QUCSYDIR, &rb, m_pDrw, m_pCfg, m_pSes, 0);

	// set WORLDUCS sysvar too
	rb.restype = RTSHORT;
	rb.resval.rint = 1;
	SDS_setvar(NULL, DB_QWORLDUCS, &rb, m_pDrw, m_pCfg, m_pSes, 0);
}

CUcsDisabler::~CUcsDisabler()
{
	if (m_worlducs.resval.rint)
		return;

	SDS_setvar(NULL, DB_QUCSORG,   &m_ucsorg,   m_pDrw, m_pCfg, m_pSes, 0);
	SDS_setvar(NULL, DB_QUCSXDIR,  &m_ucsxdir,  m_pDrw, m_pCfg, m_pSes, 0);
	SDS_setvar(NULL, DB_QUCSYDIR,  &m_ucsydir,  m_pDrw, m_pCfg, m_pSes, 0);
	SDS_setvar(NULL, DB_QWORLDUCS, &m_worlducs, m_pDrw, m_pCfg, m_pSes, 0);
}


int cmd_ReadAsBlock(char *dwgname, char *newname, int xrefmode, db_drawing *xrefdep)
{

	// If newname!=NULL, it's the name of the block after insertion.

	//NOTE: for these three xrefmodes, we only create the table entries.
	//		No block entities or blocks are read, and no dwg ents are read.
	//	If mode==IC_INSMODE_XREF_ATTACH,  insert as ATTACHed xref (MAY BE RECURSIVE CALL)
	//	If mode==IC_INSMODE_XREF_OVERLAY, insert as OVERLAYed xref (MAY BE RECURSIVE CALL)
	//	If mode==IC_INSMODE_XREF_UPDATE,  update existing xref by deleting old tables and creating new ones (MAY BE RECURSIVE CALL)
	//************
	//NOTE: For bind and insert, table/block entries MAY already exist in proper format.  
	//			If file was read from R13, no xref table entries will be present. 
	//			We need to create all blocks and we need to create new block def 
	//			containing drawing entities.  Tables must be converted to proper format for all ents
	//
	//	If mode==IC_INSMODE_XREF_BIND, bind existing xref.  This f'n just reads in dwg entities in proper format (reads tables if req'd)
	//	If mode==IC_INSMODE_XREF_INSERT, insert existing xref.  This f'n just reads in dwg entities in proper format (reads tables if req'd)
	//
	//		modes UPDATE, BIND and INSERT require xrefdep as the file to read from!

	char fs4[IC_ACADBUF],fs5[IC_ACADBUF],*fcp1,*fcp2; //,curdwg[IC_FILESIZE+IC_PATHSIZE];
	char *tablenameptr;
	char searchfilename[IC_ACADBUF],fullfilepath[IC_ACADBUF],blockname[IC_ACADBUF],tableprefix[IC_ACADBUF];
	char recursive_dwg_name[IC_ACADBUF],recursive_blk_name[IC_ACADBUF];
	char anonblkname[IC_ACADBUF];
	sds_point insbase;
	sds_matrix mtx;
	int fi1,fi2,attflag,xrefidx=-1,ret=RTNORM;

	// Named constant indices, used for iterating over (x)tblnames arrays 
	enum
	{
		READASBLOCK_TABLE_BEGIN = 0,
		READASBLOCK_TABLE_APPID = READASBLOCK_TABLE_BEGIN,
		READASBLOCK_TABLE_STYLE,
		READASBLOCK_TABLE_LTYPE,
		READASBLOCK_TABLE_LAYER,
		READASBLOCK_TABLE_DIMSTYLE,
		READASBLOCK_TABLE_NCOUNT
	};
	//TODO we need to be able to insert one dwg into another here
	//NOTE: ltype before layers, because layers depend upon ltypes, style before ltypes for same reason
	//ALM: APPID table is first since now (20.01.2003).
	//     Because other tabrecs can have eed maintained by some application, so application tabrecs should be created first
	char *tblnames[READASBLOCK_TABLE_NCOUNT] ={"APPID"/*DNT*/,"STYLE"/*DNT*/,"LTYPE"/*DNT*/,"LAYER"/*DNT*/,"DIMSTYLE"/*DNT*/};
	char *xtblnames[READASBLOCK_TABLE_NCOUNT]={"APPID"/*DNT*/,"STYLE"/*DNT*/,"LTYPE"/*DNT*/,"LAYER"/*DNT*/,"DIMSTYLE"/*DNT*/};

	char numstr[8];
	db_drawing *flp,*flp_recursive;
	db_handitem *hip;
	int ct,rewind;
	sds_name ename;
	struct resbuf *elist=NULL,rb;
	struct resbuf *tmprb=NULL,*trb,*rbname,app;
	struct cmd_anon_blklst *anonbeg,*anoncur,*anontemp;
	resbuf plrb;
	bool convertpl;

	// set reactor fixup tables
	SDS_CURDWG->SetCopyingFlag(true);
	ReactorFixups reactorFixup;


	SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (plrb.resval.rint==2)
		convertpl=true;
	else
		convertpl=false;


	app.restype=RTSTR; app.resval.rstring="*"/*DNT*/; app.rbnext=NULL;

	if (xrefmode==IC_INSMODE_XREF_UPDATE || xrefmode==IC_INSMODE_XREF_BIND ||
		xrefmode==IC_INSMODE_XREF_INSERT)
	{
		if (NULL==xrefdep)return(RTERROR);
		flp=xrefdep;
	}
	else if ((xrefmode==IC_INSMODE_XREF_ATTACH ||
			  xrefmode==IC_INSMODE_XREF_OVERLAY) && xrefdep!=NULL)
	{
		//this only happens on recursive xref call (nested xref)
		flp=xrefdep;
		strncpy(fullfilepath,dwgname,sizeof(fullfilepath)-1);
	}
	else
	{
		// if called with drawing pointer, then ignore dwgname.  This
		// happens if we're loading an xref w/o table entries
		// (the above comment is actually referring to what happens in the "else" clause)
		if (NULL==xrefdep)
		{
			strncpy(searchfilename,dwgname,sizeof(searchfilename)-1); 
			if ((fcp1=strrchr(searchfilename,'.'/*DNT*/))==NULL) ic_setext(searchfilename,".dwg"/*DNT*/);
			if (sds_findfile(searchfilename,fullfilepath)!=RTNORM) return(RTERROR);

// it appears to be here that all the table entries from the loaded drawing are brought into the
// current drawing
// I'm not so sure about that -- they are brought in further down.  This is just the load of the file.

			if ((flp=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,fullfilepath,&fi1,convertpl))==NULL)
			{
				cmd_ErrorPrompt(CMD_ERR_CANTOPENDWG,0,fullfilepath);
				return(RTERROR);
			}
		}
		else
		{
			flp=xrefdep;
			strncpy(fullfilepath,dwgname,sizeof(fullfilepath)-1);
		}
	}


	//fullfilepath is full name of file
	//set blockname to name of xref/block to create
	if (newname!=NULL && *newname)
	{
		//fcp1=newname;
		strncpy(blockname,newname,sizeof(blockname)-1);
		//Bugzilla No. 78119 ; 22-04-2002 
		//strupr(blockname);
	}
	else
	{
		strncpy(blockname,dwgname,sizeof(blockname)-1); 
		//Bugzilla No. 78119 ; 22-04-2002 
		//strupr(blockname);
		if ((fcp1=strrchr(blockname,'.'/*DNT*/))!=NULL)*fcp1=0;
		if ((fcp1=strrchr(blockname,'\\'/*DNT*/))!=NULL)
		{
			*fcp1=0;
			fcp1++;
			strncpy(blockname,fcp1,sizeof(blockname)-1);
			fcp1=NULL;
		}
		else if (IC_IS_XREF_INSMODE(xrefmode))return(RTERROR);
		//need path if it's an xref
	}

// if we are attaching or overlaying, and not recursively
	if ((xrefmode==IC_INSMODE_XREF_ATTACH || xrefmode==IC_INSMODE_XREF_OVERLAY) && xrefdep==NULL)
	{
		if (TableNameLengthsLegalForXref(flp,blockname)!=RTNORM)
		{
			delete flp;
			return(RTERROR);
		}
	}

	char          pendflagsp;
	db_handitem  *pendentmakep0;
	db_handitem  *pendentmakep1;
	db_handitem  *complexmainp;
	db_handitem  *lastblockdefp;

	if (SDS_CURDWG)	SDS_CURDWG->get_pendstuff(&pendflagsp,&pendentmakep0,&pendentmakep1,&complexmainp,&lastblockdefp);
	if (SDS_CURDWG)	SDS_CURDWG->set_pendstuff(0,NULL,NULL,NULL,NULL);


	//insert dwg into current dwg as a block, and set 
	//	block handitem dependency on xref file.
	if (xrefmode==IC_INSMODE_XREF_ATTACH || xrefmode==IC_INSMODE_XREF_OVERLAY)
	{
		if (xrefdep!=NULL)//nested call
			attflag=IC_XREF_RESOLVED | IC_BLOCK_XREF;
		else if (xrefmode==IC_INSMODE_XREF_ATTACH)
			attflag=IC_XREF_RESOLVED | IC_BLOCK_XREF;
		else if (xrefmode==IC_INSMODE_XREF_OVERLAY)
			attflag=IC_XREF_RESOLVED | IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID;
		//get the index of the NEXT block to be created, which will be our XREF block.
		//all sub-blocks of this xref will get that xrefidx!!! NOTE: don't use sds_tblnext
		//	because we need to count blocks which are marked for deletion as well.
		xrefidx=0;
		for (hip=SDS_CURDWG->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=hip->next)
		{
			xrefidx++;
		}
		SDS_getvar(NULL,DB_QINSBASE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		elist=sds_buildlist(RTDXF0,"BLOCK"/*DNT*/,1,fullfilepath,2,blockname,70,attflag,10,rb.resval.rpoint,0);
		if (elist==NULL)
		{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=RTERROR;
			goto exit;
		}
		if ((ret=sds_entmake(elist))==RTERROR)
		{
			cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
			ret=RTERROR;
			goto exit;
		}

		IC_RELRB(elist);

		//we opened flp, so set the xref dependency pointer in the blocktabrec
		(hip=SDS_LastHandItem)->set_xrefdp(flp);
		hip->set_looked4xref(1);

		if ((elist=sds_buildlist(RTDXF0,"ENDBLK"/*DNT*/,0))==NULL)
		{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			return(RTERROR);
		}
		if (RTREJ==(ret=sds_entmake(elist)))
		{
			cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
			return(RTERROR);
		}ret=RTNORM;

		IC_RELRB(elist);
	}

	if (xrefmode==IC_INSMODE_XREF_ATTACH || xrefmode==IC_INSMODE_XREF_OVERLAY)
	{
		bool GaveACISMsg=false;
		bool GaveProxyMsg=false;
		bool GaveHatchMsg=false;
		bool GaveA2KEntMsg=false;
		sds_name ename;
		ename[1]=(long)flp;
		for (ename[0]=(long)flp->entnext_noxref(NULL); ename[0]!=0L; ename[0]=(long)flp->entnext_noxref((db_handitem *)ename[0]))
		{
			if (GaveProxyMsg && GaveACISMsg && GaveHatchMsg && GaveA2KEntMsg) break;
//			if(IC_TYPE_FROM_ENAME(ename)==DB_HATCH && GaveHatchMsg==0) {
//				cmd_ErrorPrompt(CMD_ERR_INSERTHATCH,1);		
//				GaveHatchMsg=1;
//				continue;
//			}
			if (IC_TYPE_FROM_ENAME(ename)==DB_ACAD_PROXY_ENTITY && !GaveProxyMsg)
			{
				cmd_ErrorPrompt(CMD_ERR_INSERTPROXY,1);     
				GaveProxyMsg=true;
				continue;
			}
#ifdef BUILD_WITH_ACIS_SUPPORT
			if ((IC_TYPE_FROM_ENAME(ename)==DB_3DSOLID || IC_TYPE_FROM_ENAME(ename)==DB_REGION || IC_TYPE_FROM_ENAME(ename)==DB_BODY) 
				&& !GaveACISMsg
				&& !CModeler::get()->isStarted())
			{
				cmd_ErrorPrompt(CMD_ERR_INSERTACIS,1);      
				GaveACISMsg=true;
				continue;
			}
#endif
			if ((IC_TYPE_FROM_ENAME(ename)==DB_RTEXT || IC_TYPE_FROM_ENAME(ename)==DB_WIPEOUT || IC_TYPE_FROM_ENAME(ename)==DB_ARCALIGNEDTEXT) 
				&& !GaveA2KEntMsg)
			{
				cmd_ErrorPrompt(CMD_ERR_UNSUPPORTEDA2KENT,1);       
				GaveA2KEntMsg=true;
				continue;
			}
		}
	}

	//begin by inserting tables from flp(child) into current dwg
	//we'll need to make a conversion table so we know which table
	//entry in flp(child) will correspond to which entry in the current 
	//drawing
	//**************************************************
	//BEGINNING OF TEMP CODE
	//**************************************************

	anonbeg=anoncur=anontemp=NULL;

	elist=tmprb=trb=NULL;


	//NOTE: INSBASE is stored as UCS point, so get it straight from db
	db_qgetvar(DB_QINSBASE  ,flp->ret_headerbuf(),insbase  ,DB_3DPOINT,0);

	//NOTE:  Even though external dwg has insbase not at origin, block MUST be
	//			defined w/all points translated so that origin comes out at 0,0,0
	//		 Block is ALWAYS created by ACAD w/origin at 0,0,0 unless we're binding or
	//			inserting an xref, which use INSBASE from native dwg (don't ask me why)

	if (xrefmode==IC_INSMODE_XREF_ATTACH ||
		xrefmode==IC_INSMODE_XREF_OVERLAY ||
		xrefmode==IC_INSMODE_XREF_UPDATE)
	{

		strncpy(tableprefix,blockname,sizeof(tableprefix)-1);
		strcat(tableprefix,CMD_XREF_TBLDIVIDERSTR);	   // sets tableprefix to "BLOCKNAME|"
		fcp1=tableprefix+strlen(tableprefix); // put fcp1 at end of tableprefix
		//set fcp1 to where we want to copy stuff into, so
		//DON'T MOVE FCP1
	}
	else if (xrefmode==IC_INSMODE_XREF_BIND)
	{
		strncpy(tableprefix,blockname,sizeof(tableprefix)-1);
		fcp1=tableprefix+strlen(tableprefix); // put fcp1 at end of tableprefix
	}
	else
	{
		tableprefix[0]=0;
		fcp1=NULL;
	}
	//NOTE: in ver 1.0, we don't have any xref block data in current dwg.  If binding, we need to 
	//add it, but we've already done the table stuff so we can skip all this...

	if (xrefmode!=IC_INSMODE_XREF_BIND && xrefmode!=IC_INSMODE_XREF_INSERT)
	{
		// Define all of the tables.
		//NOTE: both instances of tblnames below have 5 elements, so we can use same counter
		for (ct=READASBLOCK_TABLE_BEGIN; ct < READASBLOCK_TABLE_NCOUNT; ++ct)
		{
			if (xrefmode==IC_INSMODE_XREF_UPDATE)
			{
				if (fcp1)*fcp1=0;
				//delete all the existing xref records in the current dwg table.  we'll restore 
				//	the ones which need to be reloaded
				if (elist!=NULL)
				{
					sds_relrb(elist);elist=NULL;
				}
				elist=sds_tblnext(xtblnames[ct],1);
				while (elist!=NULL)
				{
					ic_assoc(elist,2);
					if (strnsame(ic_rbassoc->resval.rstring,tableprefix,strlen(tableprefix)))
						SDS_tblmesser(elist,IC_TBLMESSER_DELETE,SDS_CURDWG);
					sds_relrb(elist);elist=NULL;
					elist=sds_tblnext(xtblnames[ct],0);
				}

			}
			for (rewind=1;;rewind=0)
			{

				if (elist!=NULL)
				{
					sds_relrb(elist);elist=NULL;
				}
				if (IC_IS_XREF_INSMODE(xrefmode))
				{
					//Attach, Overlay, or Update
					elist=SDS_tblgetter(xtblnames[ct],NULL,rewind,flp);
					if (elist==NULL)break;//go on to the next table
					ic_assoc(elist,2);
					rbname=ic_rbassoc;
					//find out if it's a table entry we don't mess with
					if (ct==READASBLOCK_TABLE_LAYER && strsame(rbname->resval.rstring,"0"/*DNT*/)) continue;//layer 0
					////don't import STANDARD style, or standard dimstyle
					//else if((2==ct || 4==ct) && strsame(rbname->resval.rstring,"STANDARD"))continue;
					else if (ct==READASBLOCK_TABLE_LTYPE && (
															strisame(rbname->resval.rstring,"CONTINUOUS"/*DNT*/) ||
															strisame(rbname->resval.rstring,"BYBLOCK"/*DNT*/) ||
															strisame(rbname->resval.rstring,"BYLAYER"/*DNT*/)))continue;//ltypes
					else if (ct==READASBLOCK_TABLE_APPID && strsame(rbname->resval.rstring,"ACAD"/*DNT*/))continue;//appid

					if (ct==READASBLOCK_TABLE_LAYER && strisame(rbname->resval.rstring,"DEFPOINTS"/*DNT*/))
					{
						//if the defpts layer exists in the xref dwg, skip it unless it doesn't
						//exist in current drawing.  If not in current drawing, copy xref entry
						if (trb!=NULL)
						{
							sds_relrb(trb);trb=NULL;
						}
						if (NULL!=(trb=SDS_tblgetter(xtblnames[ct],"DEFPOINTS"/*DNT*/,0,SDS_CURDWG)))
						{
							sds_relrb(trb);trb=NULL;
							continue;
						}
					}
					if (READASBLOCK_TABLE_DIMSTYLE==ct && 0==ic_assoc(elist,340))
					{
						//for dimstyles, the 340 group points to the entity name with the text style for
						//	this dimstyle.  We need to get the old record's name, convert it to
						//  new record's name, and then track down its ename.  
						//Also, if it points to the STANDARD style in the old dwg, point it to STANDARD in this one
						trb=ic_rbassoc;
						for (hip=flp->tblnext(DB_STYLETABREC,1); hip!=NULL; hip=hip->next)
						{
							if (trb->resval.rlname[0]==(long)hip)
							{
								hip->get_2(&tablenameptr);
								if (fcp1)*fcp1=0;
								//if(strsame(rbname->resval.rstring,"STANDARD")){
								//	strncpy(fs4,"STANDARD",sizeof(fs4)-1);
								//}else{
								strncat(tableprefix,tablenameptr,(IC_ACADNMLEN-strlen(tableprefix)-1));
								strncpy(fs4,tableprefix,sizeof(fs4)-1);
								//}
								sds_resbuf *rblook=NULL;
								if (NULL==(rblook=sds_tblsearch("STYLE"/*DNT*/,fs4,0)))
								{
									hip=NULL;break;
								}
								ic_assoc(rblook,-1);
								ic_encpy(trb->resval.rlname,ic_rbassoc->resval.rlname);
								if (fcp1) *fcp1=0;
								sds_relrb(rblook);
								break;//don't let hip walk to NULL
							}
						}
						if (hip==NULL)continue;//go on to next entry?
					}
					if (fcp1) *fcp1=0;
					ic_assoc(elist,70);
					//don't allocate string longer than IC_ACADNMLEN
					if ((ct!=READASBLOCK_TABLE_LAYER || !strisame(rbname->resval.rstring,"DEFPOINTS"/*DNT*/))
						&& (!(ic_rbassoc->resval.rint & IC_ENTRY_XREF_DEPENDENT)))
					{
						//you can add the defpoints layer, but don't modify it
						//also, you can add definitions from nested xrefs, but don't modify 'em
						//	all other cases we modify or we already did a continue
						ic_rbassoc->resval.rint|=(IC_XREF_RESOLVED | IC_ENTRY_XREF_DEPENDENT);
// fix the "appid virus" bug.  Do not prepend the tableprefix onto the table entry for appids
						if (ct!=READASBLOCK_TABLE_APPID)
						{
							strncat(tableprefix,rbname->resval.rstring,(sizeof(tableprefix)-strlen(tableprefix)-1));
							if (NULL==(rbname->resval.rstring=ic_realloc_char(rbname->resval.rstring,strlen(tableprefix)+1)))
							{
								sds_relrb(elist);
								return(RTERROR);
							}
							strcpy(rbname->resval.rstring,tableprefix);
							if (fcp1) *fcp1=0;
						}
// otherwise, for appid, just leave it in rbname->resval.rstring

						//ADD XREF TO LTYPE NAME
						if (READASBLOCK_TABLE_LAYER==ct && 0==ic_assoc(elist,6) && RTNORM!=sds_wcmatch(ic_rbassoc->resval.rstring,"CONTINUOUS,BYBLOCK,BYLAYER,Continuous,ByBlock,ByLayer"/*DNT*/))
						{
							if (fcp1) *fcp1=0;
							strncat(tableprefix,ic_rbassoc->resval.rstring,(sizeof(tableprefix)-1-strlen(tableprefix)));
							if (NULL==(ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(tableprefix)+1)))
							{
								sds_relrb(elist);
								return(RTERROR);
							}
							strcpy(ic_rbassoc->resval.rstring,tableprefix);
							if (fcp1) *fcp1=0;
						}
					}

				}
				else
				{
					elist=SDS_tblgetter(tblnames[ct],NULL,rewind,flp);
					if (elist==NULL)break;//go on to the next table
					ic_assoc(elist,2);
					rbname=ic_rbassoc;
				}

//				RemovePersistentReactors(elist);

				//at this point, elist is setup to be the way we want it in the current dwg -- NOT REALLY -- SEE BELOW
				//search drawing into which we'll place the entry
				trb=NULL;
				if (IC_INSMODE_XREF_UPDATE==xrefmode)
				{
					//check manually to see if old tblrec exists w/this name.  If so, 
					//	undelete and modify it
					fi1=1;//rewind flag
					fi2=0;
					for (hip=SDS_CURDWG->tblnext(db_str2hitype(xtblnames[ct]),fi1); hip!=NULL; hip=hip->next)
					{
						hip->get_2(&tablenameptr);
						if (strsame(tablenameptr,rbname->resval.rstring))
						{
							if (hip->ret_deleted())
							{
								sds_name ename;
								ename[0] = (long)hip;
								ename[1] = (long)SDS_CURDWG;
								//Note - the sds_entdel will undelete the xref, and the SDS_tblmesser will modify it.
								if (RTNORM!=sds_entdel(ename) || RTNORM!=SDS_tblmesser(elist,IC_TBLMESSER_MODIFY,SDS_CURDWG))
								{
									cmd_ErrorPrompt(CMD_ERR_REBUILDTABLE,0,(void *)((!IC_IS_XREF_INSMODE(xrefmode))?tblnames[ct]:xtblnames[ct]));
									#if defined(DEBUG)
									sds_textscr();
									ic_listdxf(elist);elist=NULL; //listdxf frees elist
									#endif
									ret=RTERROR;
									goto exit;
								}
							}
							fi2=1;//flag set - entry is present
							break;//for loop thru tbl
						}
						fi1=0;//don't rewind to beginning of table
					}


					if (fi2!=1)
					{ //if we didn't find the thing in target dwg
						struct resbuf *namerb;
						char *origname;
						char newname[IC_ACADBUF];
						short ret;

						namerb=ic_ret_assoc(elist,2);					// find the original name
						origname=namerb->resval.rstring;				// make a new one with the table prefix

						if ((ct==READASBLOCK_TABLE_LTYPE && (strisame(origname,"CONTINUOUS"/*DNT*/) || 
															 strisame(origname,"BYLAYER"/*DNT*/) ||
															 strisame(origname,"BYBLOCK"/*DNT*/))) ||
							(ct==READASBLOCK_TABLE_LAYER && strsame(origname,"0"/*DNT*/))) continue;

						sprintf(newname,"%s%s",tableprefix,origname);
						namerb->resval.rstring=newname;
						ret=sds_entmake(elist);							// use the new name in the entmake
						namerb->resval.rstring=origname;				// put this back

						if (ret!=RTNORM)
						{
							cmd_ErrorPrompt(CMD_ERR_BUILDTABLE,0,(void *)((!IC_IS_XREF_INSMODE(xrefmode))?tblnames[ct]:xtblnames[ct]));
							#if defined(DEBUG)
							sds_textscr();
							ic_listdxf(elist);elist=NULL; //listdxf frees elist
							#endif
							ret=RTERROR;
							goto exit;
						}
						else
						{
							if (NULL!=SDS_LastHandItem)
							{
								((db_tablerecord *)SDS_LastHandItem)->set_xrefidx(xrefidx);
							}
						}
					}
				} // end of xrefmode==update
				else if (NULL==(trb=sds_tblsearch((!IC_IS_XREF_INSMODE(xrefmode))?tblnames[ct]:xtblnames[ct],rbname->resval.rstring,0)))
				{
					//add new table record
					//if(RTNORM!=SDS_tblmesser(elist,IC_TBLMESSER_MAKE,SDS_CURDWG)) //can't use tblmesser - it doesn't set LastHandItem!!
// 6/1/98
// fix bug 8280 -- insert of a drawing containing a dimstyle did NOT properly set the dimstyle's
// txsty field to point to the correct style entry in the target drawing.  This in turn would cause
// a crash at save time.  It is a MYSTERY why this same problem does not seem to occur when inserting
// a drawing containing a layer with a linetype, wherein the layer should be adjusted to point to the linetype
// in the target drawing, because I see nothing here that fixes that case.  So if problems crop up in that regard 
// it would make sense to put the fix in this location as well.

// Upon further reflection (9/17/98) it looks like the reason this is different is that the 340 interface expects
// a hip, whereas the interface for things like a layer's linetype expects a string

					if (ct==READASBLOCK_TABLE_DIMSTYLE)
					{	// DIMSTYLE
						struct resbuf *dimsttmprb;
						dimsttmprb=elist;
						while (dimsttmprb!=NULL && dimsttmprb->restype!=340)
						{  // find the 340 buffer
							dimsttmprb=dimsttmprb->rbnext;
						}
						if (dimsttmprb==NULL || dimsttmprb->restype!=340)
						{
							sds_relrb(elist);
							elist=NULL;
							ret=RTERROR;
							goto exit;
						}

						char *cptr;	// get the name of the style for this dimstyle
									// note that this is coming from the SOURCE drawing.
						((db_handitem *)(dimsttmprb->resval.rstring))->get_2(&cptr);

						// now find the text style in the TARGET drawing
						int tblindex=db_tabrec2tabfn(db_str2hitype("STYLE"/*DNT*/));
// use this tblsearch rather than sds_tblsearch to get the hip pointer rather than the resbufs
						if (((db_handitem *)hip=SDS_CURDWG->tblsearch(tblindex,cptr,0))==NULL)
						{
							sds_relrb(elist);
							elist=NULL;
							ret=RTERROR;
							goto exit;
						}
						dimsttmprb->resval.rlname[0]=(long)hip;	 // point to that style

						//Bugzilla No. 78015 ; 18-03-2002 [
						//DIMLDRBLK
						dimsttmprb=ic_ret_assoc(elist,341);

						if (dimsttmprb==NULL)
						{
							ret=RTERROR;
							goto exit;
						}

						if (dimsttmprb->resval.rlname[0]!=NULL)
						{
							((db_handitem *)(dimsttmprb->resval.rlname[0]))->get_2(&cptr);

							//search the current drawing for the block
							tblindex=db_tabrec2tabfn(db_str2hitype("BLOCK"/*DNT*/));
							if (((db_handitem *)hip=SDS_CURDWG->tblsearch(tblindex,cptr,0))==NULL)
							{
								//not found in current drawing - create the block
								struct resbuf * telist = NULL;
								if ((telist = SDS_tblgetter("BLOCK"/*DNT*/,cptr,0,flp))==NULL)
								{
									ret=RTERROR;
									goto exit;
								}

								if ((ret=cmd_readdwg_makeblks(telist,flp,tableprefix,NULL,NULL,xrefmode,xrefidx,NULL))!=RTNORM)
								{
									sds_relrb(telist);
									telist=NULL;
									goto exit;
								}
								sds_relrb(telist);
								telist=NULL;

								//search for hip in current drawing
								if (((db_handitem *)hip=SDS_CURDWG->tblsearch(tblindex,cptr,0))==NULL)
								{
									ret=RTERROR;
									goto exit;
								}
							}
							dimsttmprb->resval.rlname[0]=(long)hip;
						}
						else
							dimsttmprb->resval.rlname[0]=0L;
						//Bugzilla No. 78015 ; 18-03-2002 ]

					}

// 11/19/98 -- fixup for linetypes with embedded text styles, if any
					if (ct==READASBLOCK_TABLE_LTYPE)
					{	// LTYPE
						struct resbuf *ltypetmprb;
						ltypetmprb=elist;
						while (ltypetmprb!=NULL)
						{
							if (ltypetmprb->restype==340 && ltypetmprb->resval.rlname[0]!=0L)
							{

								char *cptr;	// get the name of the style for this linetype
											// note that this is coming from the SOURCE drawing.
								((db_handitem *)(ltypetmprb->resval.rlname[0]))->get_2(&cptr);

								// now find the text style in the TARGET drawing
								int tblindex=db_tabrec2tabfn(db_str2hitype("STYLE"/*DNT*/));
// use this tblsearch rather than sds_tblsearch to get the hip pointer rather than the resbufs
								if (((db_handitem *)hip=SDS_CURDWG->tblsearch(tblindex,cptr,0))==NULL)
								{
									sds_relrb(elist);
									elist=NULL;
									ret=RTERROR;
									goto exit;
								}
								ltypetmprb->resval.rlname[0]=(long)hip;	 // point to that style
							}
							ltypetmprb=ltypetmprb->rbnext;
						}
					}
// end of fix for bug 8280

					if (RTNORM!=sds_entmake(elist))
					{
						cmd_ErrorPrompt(CMD_ERR_BUILDTABLE,0,(void *)((!IC_IS_XREF_INSMODE(xrefmode))?tblnames[ct]:xtblnames[ct]));

						#if defined(DEBUG)
						sds_textscr();
						ic_listdxf(elist);elist=NULL; //listdxf frees elist
						#endif
						ret=RTERROR;
						goto exit;
						//if(RTCAN==sds_getstring(0,"\nPaused...",blockname)) return(RTCAN);
					}
					else if (IC_INSMODE_XREF_ATTACH==xrefmode || IC_INSMODE_XREF_OVERLAY==xrefmode)
					{
						if (NULL!=SDS_LastHandItem)((db_tablerecord *)SDS_LastHandItem)->set_xrefidx(xrefidx);
					}

				}
				else
				{
					sds_relrb(trb);
					trb=NULL;
				}

			}//end for loop within table (for rewind)
		}//end for loop thru each table  (loop over ct)
	}//end of table looping we skip if inserting or binding

	/*since in release 1.0 we're not importing any blocks, this is commented out
	if(IC_INSMODE_XREF_UPDATE==xrefmode){
		//delete all blocks for xref
		if(elist!=NULL){sds_relrb(elist);elist=NULL;}
		for(elist=sds_tblnext("BLOCK"DNT,1);elist!=NULL;elist=sds_tblnext("BLOCK"DNT,0)){
			ic_assoc(elist,70);
			if(ic_rbassoc->resval.rint&32){
				ic_assoc(elist,2);
				if(strnsame(ic_rbassoc->resval.rstring,tableprefix,strlen(tableprefix)))
					SDS_tblmesser(elist,IC_TBLMESSER_DELETE,SDS_CURDWG);
				sds_relrb(elist);elist=NULL;
			}
		}
	}
	*/
	//if(xrefmode>=7 && xrefmoce<=9) goto exit;

	// Rename & make all of the blocks.
	for (elist=SDS_tblgetter("BLOCK"/*DNT*/,NULL,1,flp); elist!=NULL; elist=SDS_tblgetter("BLOCK"/*DNT*/,NULL,0,flp))
	{
		//in release 1.0, we're not xrefing in any block stuff except for NESTED xrefs
		if (xrefmode==IC_INSMODE_XREF_ATTACH ||
			xrefmode==IC_INSMODE_XREF_OVERLAY ||
			xrefmode==IC_INSMODE_XREF_UPDATE)
		{

			//I'll use ic_assoc here, it's a short elist
			ic_assoc(elist,70);
			if (ic_rbassoc->resval.rint & IC_BLOCK_XREF_OVERLAID)
			{
				IC_RELRB(elist);
				continue;//overlay, which cannot be nested
			}
			if (!(ic_rbassoc->resval.rint & IC_BLOCK_XREF))
			{
				IC_RELRB(elist);
				continue;
			}
			//if it's an xref, get the file and name and make a recursive call...
			ic_assoc(elist,2);
			strncpy(recursive_blk_name,ic_rbassoc->resval.rstring,sizeof(recursive_blk_name)-1);
			ic_assoc(elist,1);
			strncpy(recursive_dwg_name,ic_rbassoc->resval.rstring,sizeof(recursive_dwg_name)-1); 
			if ((fcp1=strrchr(recursive_dwg_name,'.'/*DNT*/))==NULL) ic_setext(recursive_dwg_name,".dwg"/*DNT*/);  // was fs1 at end here (blockname) -- seems wrong. (MR)
			if (sds_findfile(recursive_dwg_name,recursive_dwg_name)!=RTNORM)
			{
				cmd_ErrorPrompt(CMD_ERR_FINDXREF,0);
				IC_RELRB(elist);
				continue;
			}

			if ((flp_recursive=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,recursive_dwg_name,NULL,convertpl))==NULL)
			{
				cmd_ErrorPrompt(CMD_ERR_OPENXREF,0);
				IC_RELRB(elist);
				continue;
			}
			//make recursive file with open drawing.  If dwg open, we know call is recursive
			if (RTNORM!=cmd_ReadAsBlock(recursive_dwg_name,recursive_blk_name,xrefmode,flp_recursive))
			{
				cmd_ErrorPrompt(CMD_ERR_LOADXREF,0);
				ret=RTERROR; 
				goto exit;
			}
			IC_RELRB(elist);
			continue;
		}

		ic_assoc(elist,2);
		rbname=ic_rbassoc;
		if (*rbname->resval.rstring=='*'/*DNT*/)
		{
			//ANONYMOUS BLOCKS
			if (!IC_IS_XREF_INSMODE(xrefmode) ||
				IC_INSMODE_XREF_BIND==xrefmode ||
				IC_INSMODE_XREF_INSERT==xrefmode)
			{	//OR added for release 1.0
				//we may possibly have already defined this anon blk because it was part
				//of a previously defined block
				for (anontemp=anonbeg;anontemp!=NULL;anontemp=anontemp->next)
				{
					ASSERT(rbname->resval.rstring[1]=='D' || 
						   rbname->resval.rstring[1]=='U' || 
						   rbname->resval.rstring[1]=='X' || 
						   rbname->resval.rstring[1]=='B' ||
						   (strnisame("*Paper_Space", rbname->resval.rstring, 12)) == true    ||
						   (strisame("*Model_Space", rbname->resval.rstring)) == true
						   );

					if (0 == anontemp->type && rbname->resval.rstring[1] != 'D'/*DNT*/)
						continue;
					else
						if (1 == anontemp->type && rbname->resval.rstring[1] != 'X'/*DNT*/)
						continue;
					else
						if (2 == anontemp->type && !(rbname->resval.rstring[1] == 'U'/*DNT*/ || rbname->resval.rstring[1] == 'B'/*DNT*/))
						/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
						continue;
					if (anontemp->oldnum == atoi(rbname->resval.rstring+2))break;
				}
				if (anontemp==NULL)
				{
					//alloc linked list element to track old number and new number for anon blks
					if (NULL==(anontemp= new cmd_anon_blklst ))
					{
						ret=RTERROR;
						goto exit;
					}
					anontemp->next=NULL;
					anontemp->newnum=-1;
					if (rbname->resval.rstring[1]=='D'/*DNT*/)anontemp->type=0;
					else if (rbname->resval.rstring[1]=='X'/*DNT*/)anontemp->type=1;
					else anontemp->type=2;
					fcp2=rbname->resval.rstring+2;
					anontemp->oldnum=atoi(fcp2);
					if (anonbeg==NULL)
						anonbeg=anoncur=anontemp;
					else
					{
						anoncur->next=anontemp;
						anoncur=anoncur->next;
					}
					//now call the block inserter with the newly alloc'd link to create 
					//	subents and put new anon block number into llist
					// tableprefix in the following call seems VERY wrong to me -- shd be blockname?
// calculate matrix to use to shift DIM blocks only
					//setup matrix to use for transforming objects
					for (fi1=0; fi1<=3; fi1++)
					{
						for (fi2=0; fi2<3; fi2++)
						{
							if (fi1==fi2)mtx[fi1][fi2]=1.0;
							else mtx[fi1][fi2]=0.0;
						}

						if (fi1 < 3)
							mtx[fi1][3]= (anontemp->type==0)?(-insbase[fi1]):(0.0);
					}
					mtx[3][3]=1.0;

					if ((ret=cmd_readdwg_makeblks(elist,flp,tableprefix,anonbeg,&anoncur,xrefmode,xrefidx,
												  ((anontemp->type==0)?mtx:NULL)))!=RTNORM)goto exit;
				}
			}
			/*remember, for release 1.0 we don't import blocks when attaching
			else{	//for 7,8,&9 (attach, overlay, & reload)
				if(xrefmode==8){
					ic_assoc(elist,70);
					if(ic_rbassoc->resval.rint&64){
						//if in overlay mode, don't insert any blocks which are nested xrefs
						IC_FREEIT;
						continue;
					}
				}
				//alloc linked list element to track old number and new number for anon blks
				if(NULL==(anontemp=(cmd_anon_blklst *)malloc(sizeof(struct cmd_anon_blklst)))){
					ret=RTERROR;
					goto exit;
				}
				anontemp->next=NULL;
				anontemp->newnum=-1;
				if(rbname->resval.rstring[1]=='D')anontemp->type=0;
				else if(rbname->resval.rstring[1]=='X')anontemp->type=1;
				else anontemp->type=2;
				fcp2=rbname->resval.rstring+2;
				anontemp->oldnum=atoi(fcp2);
				if(anonbeg==NULL)
					anonbeg=anoncur=anontemp;
				else{
					anoncur->next=anontemp;
					anoncur=anoncur->next;
				}
				//note: tableprefix is xref string
				if((ret=cmd_readdwg_makeblks(elist,flp,fs3,anonbeg,&anoncur,xrefmode,xrefidx))!=RTNORM){sds_relrb(elist);goto exit;}
				
				
				/*
				//reset name of anon blk				
				sprintf(fs4,'*%c|REF-%s',ic_rbassoc->resval.rstring[1],ic_rbassoc->resval.rstring+2);
				tmprb=sds_buildlist(1,fs4,0);
				tmprb->rbnext=elist;
				elist=tmprb;
				SDS_tblmesser(elist,IC_TBLMESSER_RENAME,flp);
				//now set the 70 flag appropriately.  First, omit old 2 rb and use new
				//	1 rb as block name 
				elist->restype=2;
				for(tmprb=elist;tmprb!=NULL && tmprb->rbnext!=ic_rbassoc;tmprb=tmprb->rbnext);
				if(tmprb->rbnext==ic_rbassoc){
					tmprb->rbnext=ic_rbassoc->rbnext;
					ic_rbassoc->rbnext=NULL;
					sds_relrb(ic_rbassoc);
					ic_rbassoc=tmprb=NULL;
				}
				ic_assoc(elist,70);
				ic_rbassoc->resval.rint=(1+16+32+64);
				SDS_tblmesser(elist,IC_TBLMESSER_MODIFY,flp);
				
			}*/
		}
		else
		{
			//NON-ANONYMOUS BLOCKS
			// check for incoming block which has same name as this one
			resbuf *nameres=ic_ret_assoc(elist,2);
			if (newname && nameres && strisame(nameres->resval.rstring,newname))
			{
				sds_printf("Block references itself -- aborting insert.\n");
				ret=RTERROR;
				goto exit;
			}
			if ((ret=cmd_readdwg_makeblks(elist,flp,tableprefix,NULL,NULL,xrefmode,xrefidx,NULL))!=RTNORM)goto exit;
		}
		IC_RELRB(elist);
	}  // end of for elist
	//TODO - We need to get rid of the search for attdefs (if xrefmode<7)

	// Now read all of the entities in the drawing.
	//if there are any ATTDEF's in the drawing, we need
	//to flag the block when we create it in the table 
	//and set the 70 flag. Also, if making an xref, we
	//need to set the flag to the appropriate value
	//NOTE: we need to translate all pts so that they're relative to 0,0,0(WCS)
	//	instead of being relative to INSBASE unless binding/inserting an xref, which
	//	have their block table entries w/non-origin base points

	//if xrefmode==IC_IS_XREF_UPDATE, attflag comes from existing block - see below
	if (!IC_IS_XREF_INSMODE(xrefmode) && xrefdep==NULL)
	{
		attflag=0;
		for (ename[0]=(long)flp->entnext_noxref(NULL); ename[0]!=0L; ename[0]=(long)flp->entnext_noxref((db_handitem *)ename[0]))
		{
			if (IC_TYPE_FROM_ENAME(ename)==DB_ATTDEF) attflag=2;
			if (attflag)break;
		}
	}
	else if (xrefmode==IC_INSMODE_XREF_BIND || xrefmode==IC_INSMODE_XREF_INSERT)attflag=0;
	if (IC_INSMODE_XREF_UPDATE==xrefmode)
	{
		goto exit;//we don't need to make block/endblock stuff
		//get list for current drawing block, and delete it from drawing
		//if((ret=SDS_tblmesser(elist,IC_TBLMESSER_DELETE,SDS_CURDWG))!=RTNORM){sds_relrb(elist);goto exit;}
	}
	else if (!IC_IS_XREF_INSMODE(xrefmode) ||
			 xrefmode==IC_INSMODE_XREF_BIND ||
			 xrefmode==IC_INSMODE_XREF_INSERT)
	{//OR added for release 1.0 to import ents
		//NOTE:  the xref is created w/a base point of whatever INSBASE is in the
		//	xref drawing
		if (elist!=NULL)
		{
			sds_relrb(elist);elist=NULL;
		}
		if (!IC_IS_XREF_INSMODE(xrefmode))
		{
			if (NULL==xrefdep)
			{
				//don't use insbase here, because we want to define block w/base at 0,0,0
				rb.resval.rpoint[0]=rb.resval.rpoint[1]=rb.resval.rpoint[2]=0.0;
				elist=sds_buildlist(RTDXF0,"BLOCK"/*DNT*/,2,blockname,1,""/*DNT*/,70,attflag,10,rb.resval.rpoint,0);
			}
			else
			{
				elist=sds_buildlist(RTDXF0,"BLOCK"/*DNT*/,2,blockname,1,""/*DNT*/,70,attflag,10,insbase,0);
			}
			if (elist==NULL)
			{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=RTERROR;
				goto exit;
			}
			ret=sds_entmake(elist);

		}
		else
		{
			elist=sds_tblsearch("BLOCK"/*DNT*/,blockname,0);
			//get list for current drawing block, and delete it from drawing
			if (!elist || (ret=SDS_tblmesser(elist,IC_TBLMESSER_DELETE,SDS_CURDWG))!=RTNORM)goto exit;
			for (trb=elist;trb!=NULL;trb=trb->rbnext)
			{
				if (trb->restype==70)trb->resval.rint&=~(IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID | IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED);
				else if (trb->restype==1 || trb->restype==3)trb->resval.rstring[0]=0;
			}
			ret=sds_entmake(elist);
		}
		if (ret==RTERROR)
		{
			cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
			ret=RTERROR;
			goto exit;
		}
		IC_RELRB(elist);
	}
	if (!IC_IS_XREF_INSMODE(xrefmode) ||
		xrefmode==IC_INSMODE_XREF_BIND ||
		xrefmode==IC_INSMODE_XREF_INSERT)
	{//OR added for release 1.0 to import ents

		//setup matrix to use for transforming objects
		for (fi1=0; fi1<=3; fi1++)
		{
			for (fi2=0; fi2<3; fi2++)
			{
				if (fi1==fi2)mtx[fi1][fi2]=1.0;
				else mtx[fi1][fi2]=0.0;
			}

			if (fi1 < 3)
				mtx[fi1][3]=(!IC_IS_XREF_INSMODE(xrefmode))?(-insbase[fi1]):(0.0);
		}
		mtx[3][3]=1.0;

		rb.restype=RTSTR; rb.resval.rstring="*"/*DNT*/; rb.rbnext=NULL;
		bool GaveACISMsg=false;
		bool GaveProxyMsg=false;
		bool GaveHatchMsg=false;
		bool GaveA2KEntMsg=false;
		ename[1]=(long)flp;
// IMPORT THE ENTITIES
		for (ename[0]=(long)flp->entnext_noxref(NULL); ename[0]!=0L; ename[0]=(long)flp->entnext_noxref((db_handitem *)ename[0]))
		{
			((db_handitem *)ename[0])->get_67(&fi1);
			if (1==fi1)continue;

			//		if(IC_TYPE_FROM_ENAME(ename)==DB_HATCH){
			//			if(GaveHatchMsg==0) {
			//				cmd_ErrorPrompt(CMD_ERR_INSERTHATCH,1);		
			//				GaveHatchMsg=1;
			//			}
//				continue;
//			}
			if (IC_TYPE_FROM_ENAME(ename)==DB_ACAD_PROXY_ENTITY)
			{
				if (!GaveProxyMsg)
				{
					cmd_ErrorPrompt(CMD_ERR_INSERTPROXY,1);     
					GaveProxyMsg=true;
				}
				continue;
			}
#ifdef BUILD_WITH_ACIS_SUPPORT
			if ((IC_TYPE_FROM_ENAME(ename)==DB_3DSOLID || 
				 IC_TYPE_FROM_ENAME(ename)==DB_REGION || 
				 IC_TYPE_FROM_ENAME(ename)==DB_BODY) &&
				 !CModeler::get()->isStarted())
			{
				if (!GaveACISMsg)
				{
					cmd_ErrorPrompt(CMD_ERR_INSERTACIS,1);      
					GaveACISMsg=true;
				}
				continue;
			}
#endif
			if ((IC_TYPE_FROM_ENAME(ename)==DB_WIPEOUT || IC_TYPE_FROM_ENAME(ename)==DB_ARCALIGNEDTEXT || 
				 IC_TYPE_FROM_ENAME(ename)==DB_RTEXT))
			{
				if (!GaveA2KEntMsg)
				{
					cmd_ErrorPrompt(CMD_ERR_UNSUPPORTEDA2KENT,1);       
					GaveA2KEntMsg=true;
				}
				continue;
			}
			if (elist!=NULL)
			{
				sds_relrb(elist);elist=NULL;
			}
			elist=((db_handitem *)ename[0])->entgetx(&rb,flp);
			/*DG - 17.12.2002*/// Since arriving entities come from another drawing they should be transformed
			// regardless the current UCS in this drawing. So, set UCS to world temporarily.
			// (Note, another approach can be transforming mtx according to the current UCS.)
			{
				CUcsDisabler    ucsDisabler(SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				SDS_simplexforment(NULL,elist,mtx);
			}
//			RemovePersistentReactors(elist);
			for (trb=elist;trb!=NULL;trb=trb->rbnext)
			{
				if (trb->restype==0)
				{
					if (strsame(trb->resval.rstring,"ATTDEF"/*DNT*/))attflag=1;
					else if (RTNORM==sds_wcmatch(trb->resval.rstring,"INSERT,DIMENSION"/*DNT*/))
					{
						//check to see if blk is anonymous & needs renaming
						ic_assoc(elist,2);
						rbname=ic_rbassoc;
						if ('*'/*DNT*/==rbname->resval.rstring[0])
						{
							//substitute the new anonymous name from the llist...

/*D.G.*/// Note, we support the '*B' names too.
							ASSERT(rbname->resval.rstring[1]=='D' || rbname->resval.rstring[1]=='U' || rbname->resval.rstring[1]=='X' || rbname->resval.rstring[1]=='B');

							for (anontemp=anonbeg; anontemp!=NULL; anontemp=anontemp->next)
							{
								if (0 == anontemp->type && rbname->resval.rstring[1] != 'D'/*DNT*/)
									continue;
								else
									if (1 == anontemp->type && rbname->resval.rstring[1] != 'X'/*DNT*/)
									continue;
								else
									if (2 == anontemp->type && !(rbname->resval.rstring[1] == 'U'/*DNT*/ || rbname->resval.rstring[1] == 'B'/*DNT*/))
									/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
									continue;
								if (anontemp->oldnum == atoi(rbname->resval.rstring+2))break;
							}
							if (anontemp!=NULL)
							{
								//if we found the new number to use...
								itoa(anontemp->newnum,numstr,10);

// this code replaces the below 
								char anonchar='U'/*DNT*/;
								/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
								if (rbname->resval.rstring[1] == 'B'/*DNT*/)
									anonchar='B'/*DNT*/;
								if (anontemp->type==0) anonchar='D'/*DNT*/;
								else if (anontemp->type==1)	anonchar='X'/*DNT*/;
								if (xrefmode==IC_INSMODE_XREF_ATTACH ||
									xrefmode==IC_INSMODE_XREF_OVERLAY ||
									xrefmode==IC_INSMODE_XREF_UPDATE)
									sprintf(anonblkname,"*%c|REF%s"/*DNT*/,anonchar,numstr);
								else sprintf(anonblkname,"*%c%s"/*DNT*/,anonchar,numstr);

//									switch (anontemp->type){
//									case 0: 
//										if(xrefmode>=7 && xrefmode<=9)sprintf(anonblkname,"*D|REF%s",numstr);
//											else sprintf(anonblkname,"*D%s",numstr);
//											break;
//									case 1:		
//											if(xrefmode>=7 && xrefmode<=9)sprintf(anonblkname,"*X|REF%s",numstr);
//											else sprintf(anonblkname,"*X%s",numstr);
//											break;
//									default:
//											if(xrefmode>=7 && xrefmode<=9)sprintf(anonblkname,"*U|REF%s",numstr);
//											else sprintf(anonblkname,"*U%s",numstr);
//								}
								if (strlen(anonblkname)!=strlen(rbname->resval.rstring))
								{
									//if the string lengths of old & new numbers don't match, 
									//	then realloc the things
									if (NULL==(rbname->resval.rstring=ic_realloc_char(rbname->resval.rstring,strlen(anonblkname)+1)))
									{
										if (elist!=NULL)
										{
											sds_relrb(elist);elist=NULL;
										}
										return(RTERROR);
									}
								}
								strcpy(rbname->resval.rstring,anonblkname);
							}
						}
						else if (IC_INSMODE_XREF_BIND==xrefmode)
						{
// I think we need a better algorithm here that would determine the maximum acceptable lengths for these
// various strings, then just concatenate them together with sprintf as follows:
// sprintf(fs4,"%s$%-d$%s",trimmed(blockname),fi1,trimmed(rbname->resval.rstring));
// which would be a lot cleaner (and probably more correct) than what we have here.
// this would need some examination in ACAD to see what THEY do.

							//realloc the insert so that it has the last version of the block(highest
							//	# between the $'s
							for (fi1=0;fi1<9999;fi1++)
							{
								strncpy(fs4,blockname,sizeof(fs4)-1);
								strcat(fs4,CMD_XREF_TBLBINDERSTR);	  // sets fs4 to "BLOCKNAME$"

								itoa(fi1,numstr,10);
								strcat(fs4,numstr);
								strcat(fs4,CMD_XREF_TBLBINDERSTR);
								fcp2=fs4+strlen(fs4); // put fcp2 at end of fs4
								//LEAVE FCP2 where it is - we may need to increment it if the block exists

								strncat(fs4,rbname->resval.rstring,sizeof(fs4)-strlen(fs4)-1);
								//now we have the block to make, but first, check to see if new block already exists
								if (tmprb!=NULL)
								{
									sds_relrb(tmprb);tmprb=NULL;
								}
								if (NULL==(tmprb=sds_tblsearch("BLOCK"/*DNT*/,fs4,0)))break;
								strncpy(fs5,fs4,sizeof(fs5)-1);
							}
							//fi1 has the FIRST number which doesn't appear in the drawing as a bound block.
							//if fi1=0, then this block must be forward-nesting.  Make a recursive call
							// and it will give us back the block name in the 2 group of the elist, which
							// we know will match fs4;
							if (0==fi1)
							{
								tmprb=SDS_tblgetter("BLOCK"/*DNT*/,rbname->resval.rstring,0,flp);
								if (tmprb!=NULL)
								{
									ret=cmd_readdwg_makeblks(tmprb,flp,blockname,anonbeg,&anoncur,xrefmode,xrefidx,NULL);
									sds_relrb(tmprb);tmprb=NULL;
									if (ret!=RTNORM)
									{
										continue;
									}
									//no need to realloc in this case, becase _makeblks does it for us
								}
							}
							else
							{
								//fs5 has the last entry we actually found
								rbname->resval.rstring=ic_realloc_char(rbname->resval.rstring,strlen(fs5)+1);
								strcpy(rbname->resval.rstring,fs5);
							}

						}
					}
					ic_rbassoc=trb;
				}
				else if (xrefmode==IC_INSMODE_XREF_ATTACH ||
						 xrefmode==IC_INSMODE_XREF_OVERLAY ||
						 xrefmode==IC_INSMODE_XREF_UPDATE)
				{
					if ((trb->restype==6 &&  RTNORM!=sds_wcmatch(trb->resval.rstring,"BYLAYER,BYBLOCK,CONTINUOUS,ByLayer,ByBlock,Continuous"/*DNT*/))||
						(trb->restype==8 && RTNORM!=sds_wcmatch(trb->resval.rstring,"0,DEFPOINTS,Defpoints"/*DNT*/))||
						(trb->restype==7) ||
						(trb->restype==3 && strsame(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/))	/* || */
#ifdef REMOVED
						(trb->restype==1001 && RTNORM!=sds_wcmatch(trb->resval.rstring,"ACAD"/*DNT*/))
#endif
					   )
					{
						if (fcp1) *fcp1=0;
						strncat(tableprefix,trb->resval.rstring,(sizeof(tableprefix)-1-strlen(tableprefix)));
						if (NULL==(trb->resval.rstring=ic_realloc_char(trb->resval.rstring,strlen(tableprefix)+1)))
						{
							ret=RTERROR;
							goto exit;
						}
						strcpy(trb->resval.rstring,tableprefix);
					}
				}
			}

			if (IC_INSMODE_XREF_BIND==xrefmode)
			{
				tmprb=NULL;
				for (trb=elist;trb!=NULL;trb=trb->rbnext)
				{
					if ((trb->restype==6 &&  RTNORM!=sds_wcmatch(trb->resval.rstring,"BYLAYER,BYBLOCK,CONTINUOUS,ByLayer,ByBlock,Continuous"/*DNT*/))||
						(trb->restype==8 && RTNORM!=sds_wcmatch(trb->resval.rstring,"0,DEFPOINTS,Defpoints"/*DNT*/))||
						(trb->restype==7)||
						(trb->restype==3 && strsame(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/))	/*||*/
#ifdef REMOVED
						(trb->restype==1001 && RTNORM!=sds_wcmatch(trb->resval.rstring,"ACAD"/*DNT*/))
#endif
					   )
					{
						for (fi1=0;fi1<9999;fi1++)
						{
							strncpy(fs4,blockname,sizeof(fs4)-1);
							strcat(fs4,CMD_XREF_TBLBINDERSTR);	  // sets fs4 to "BLOCKNAME$"
							itoa(fi1,numstr,10);
							strcat(fs4,numstr);
							strcat(fs4,CMD_XREF_TBLBINDERSTR);
							strncat(fs4,trb->resval.rstring,sizeof(fs4)-strlen(fs4)-1);
							fcp2=fs4+strlen(fs4); // put fcp2 at end of fs4
							//LEAVE FCP2 where it is - we may need to increment it if the block exists

							if (0==fi1)strncpy(fs5,fs4,sizeof(fs5)-1);

							//now we have the str to make, but first, check to see if new block already exists
							if (tmprb!=NULL)
							{
								sds_relrb(tmprb);tmprb=NULL;
							}
							switch (trb->restype)
							{
							case 6:  tmprb=sds_tblsearch("LTYPE"/*DNT*/,fs4,0);break;
							case 8:  tmprb=sds_tblsearch("LAYER"/*DNT*/,fs4,0);break;
							case 7:  tmprb=sds_tblsearch("STYLE"/*DNT*/,fs4,0);break;
							case 3:  tmprb=sds_tblsearch("DIMSTYLE"/*DNT*/,fs4,0);break;
							default: tmprb=sds_tblsearch("APPID"/*DNT*/,fs4,0);break;
							}
							if (NULL==tmprb)break;
							strncpy(fs5,fs4,sizeof(fs5)-1);
						}

						trb->resval.rstring=ic_realloc_char(trb->resval.rstring,strlen(fs5)+1);
						strcpy(trb->resval.rstring,fs5);

					}
				}

			}
			if (IC_TYPE_FROM_ENAME(ename)==DB_IMAGE)
			{
				if (RTREJ==(ret=SDS_entmakeImageInsert(elist,NULL)))
				{
					cmd_ErrorPrompt(CMD_ERR_BUILDENT,0);
					#if defined(DEBUG)
					sds_textscr();
					ic_listdxf(elist);elist=NULL;//lister frees llist
					#endif
					ret=RTERROR;
					goto exit;
				}
			}
			else if (RTREJ==(ret=sds_entmake(elist)))
			{
				cmd_ErrorPrompt(CMD_ERR_BUILDENT,0);
				#if defined(DEBUG)
				sds_textscr();
				ic_listdxf(elist);elist=NULL;//lister frees llist
				#endif
				ret=RTERROR;
				goto exit;
			}

			reactorFixup.AddHanditem(SDS_LastHandItem);
			reactorFixup.AddHandleMap(((db_handitem*)ename[0])->RetHandle(), SDS_LastHandItem->RetHandle());

//ATTN: MARKB
			if (IC_TYPE_FROM_ENAME(ename)==DB_MLINE)
			{
				sds_name newename;
				newename[0] = (long)SDS_LastHandItem;
				newename[1] = (long)SDS_CURDWG;;
				SDS_updateMLine(ename, newename);
			}
		}
		if (elist!=NULL)
		{
			sds_relrb(elist);elist=NULL;
		}

		if ((elist=sds_buildlist(RTDXF0,"ENDBLK"/*DNT*/,0))==NULL)
		{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			return(RTERROR);
		}
		if (RTREJ==(ret=sds_entmake(elist)))
		{
			cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
			return(RTERROR);
		}

		#if defined(DEBUG)
		sds_getinput(fs5);
		#endif
		IC_RELRB(elist);
		ret=RTNORM;
	}


	// apply reactor fixups
	reactorFixup.ApplyFixups(SDS_CURDWG);
	SDS_CURDWG->SetCopyingFlag(false);

	//*** Free the tables and the entities from the dwg linked list in memory
	//**************************************************
	//END OF TEMP CODE
	//**************************************************

	exit:
	if (SDS_CURDWG)	SDS_CURDWG->set_pendstuff(pendflagsp,pendentmakep0,pendentmakep1,complexmainp,lastblockdefp);

	if (xrefmode<=6)delete flp;
	if (elist!=NULL)
	{
		sds_relrb(elist);elist=NULL;
	}
	if (anonbeg!=NULL)
	{
		for (anontemp=anonbeg;anontemp!=NULL;anonbeg=anontemp)
		{
			anontemp=anonbeg->next;
			anonbeg->next=NULL;
			delete anonbeg;
		}
	}

	return(ret);
}



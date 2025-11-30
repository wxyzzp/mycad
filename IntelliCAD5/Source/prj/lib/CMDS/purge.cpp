/* F:\DEV\PRJ\LIB\CMDS\PURGE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "layertabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "IcadApi.h"
#include "dimstyletabrec.h"

void SetPurgeBlockName(CString &, int& ,int&,CString &);


short cmd_purge(void)   {

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs3[IC_ACADBUF];
	RT ret;

	db_drawing *dp=SDS_CURDWG;

	if(NULL==dp) return(RTERROR);

    if(sds_initget(0,ResourceString(IDC_PURGE_INITGET_BATCH_ALL__0, "Batch_all|baTch ~ Purge_all|All ~ Blocks Dimension_styles|Dimstyles Layers|LAyers Linetypes|LTypes Text_styles|STyles ~_baTch ~_ ~_All ~_ ~_Blocks ~_Dimstyles ~_LAyers ~_LTypes ~_STyles"))!=RTNORM) return(RTERROR);
    if(RTNORM!=(ret=sds_getkword(ResourceString(IDC_PURGE__NPURGE___BATCH_AL_1, "\nPurge:  baTch/All/Blocks/Dimstyles/LAyers/LTypes/STyles: " ),fs3))) {
		return(ret);
    }

	return purge_drawing (dp, fs3);
}


short prepare_drawing_for_purge (db_drawing *dp)
{
	char *fcp1;
	int	fi1;
	struct resbuf clayer,celtype,dimstyle,dimtxsty,textstyle;
	db_handitem *hip,*bhip,*blkhip,*ehip,*thip;
	char dimblkBuf[256];


    if(SDS_getvar(NULL,DB_QCLAYER,&clayer,dp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
    if(SDS_getvar(NULL,DB_QCELTYPE,&celtype,dp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
    if(SDS_getvar(NULL,DB_QDIMSTYLE,&dimstyle,dp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
    if(SDS_getvar(NULL,DB_QDIMTXSTY,&dimtxsty,dp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
    if(SDS_getvar(NULL,DB_QTEXTSTYLE,&textstyle,dp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);


	//* Mark blocks as unreferenced.
	for (hip=dp->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=dp->tblnext(DB_BLOCKTABREC,0)) {
		char name[512];
		if(hip->ret_deleted()) continue;
		hip->get_2(name,512);
		if (!strnicmp(name,"*PAPER_SPACE",12)) // set these as referenced -- do not purge
			{
			hip->get_70(&fi1); 
			fi1 |= IC_REFERENCED;
			hip->set_70(fi1);
			}
		else
			{
			hip->get_70(&fi1);
			if(fi1 & IC_REFERENCED) 
				{
				fi1 &= ~IC_REFERENCED; 
				hip->set_70(fi1); 
				}
			}
		}
	//* Mark layers as unreferenced.
	for (hip=dp->tblnext(DB_LAYERTABREC,1); hip!=NULL; hip=dp->tblnext(DB_LAYERTABREC,0)) {
		if(hip->ret_deleted()) continue;
		hip->get_2(&fcp1);
		if(strisame("0"/*DNT*/,fcp1) || strisame("DEFPOINTS"/*DNT*/,fcp1) || strisame(clayer.resval.rstring,fcp1)) {
			hip->get_70(&fi1); fi1 |= IC_REFERENCED; hip->set_70(fi1); continue;
		}
		hip->get_70(&fi1); if(fi1 & IC_REFERENCED) { fi1 &= ~IC_REFERENCED; hip->set_70(fi1); }
	}
	//* Mark linetypes as unreferenced.
	for (hip=dp->tblnext(DB_LTYPETABREC,1); hip!=NULL; hip=dp->tblnext(DB_LTYPETABREC,0)) {
		if(hip->ret_deleted()) continue;
		hip->get_2(&fcp1);
		if(strisame(fcp1,"BYBLOCK"/*DNT*/) || strisame(fcp1,"BYLAYER"/*DNT*/) || strisame(fcp1,"CONTINUOUS"/*DNT*/) || strisame(celtype.resval.rstring,fcp1)) {
			hip->get_70(&fi1); fi1|=IC_REFERENCED; hip->set_70(fi1); continue;
		}
		hip->get_70(&fi1); if(fi1 & IC_REFERENCED) { fi1 &= ~IC_REFERENCED; hip->set_70(fi1); }
	}
	//* Mark dimension styles as unreferenced.
	for (hip=dp->tblnext(DB_DIMSTYLETABREC,1); hip!=NULL; hip=dp->tblnext(DB_DIMSTYLETABREC,0)) {
		if(hip->ret_deleted()) continue;
		hip->get_2(&fcp1);
		if(strisame("STANDARD"/*DNT*/,fcp1) || strisame(dimstyle.resval.rstring,fcp1)) {
			hip->get_70(&fi1); fi1 |= IC_REFERENCED; hip->set_70(fi1); continue;
		}
		hip->get_70(&fi1); if(fi1 & IC_REFERENCED) { fi1 &= ~IC_REFERENCED; hip->set_70(fi1); }
	}
	//* Mark text styles as unreferenced.
	for (hip=dp->tblnext(DB_STYLETABREC,1); hip!=NULL; hip=dp->tblnext(DB_STYLETABREC,0)) {
		if(hip->ret_deleted()) continue;
		// EBATECH(CNBR) -[ 2002/4/16 Keep shape font and call get_70() at once.
		hip->get_70(&fi1);
		if(fi1 & IC_SHAPEFILE_SHAPES ) {
			fi1 &= ~IC_REFERENCED;	// 2002/4/29 EBATECH(CNBR) Purging unused SHAPE is ready.
			//fi1 |= IC_REFERENCED;
			hip->set_70(fi1);
			 continue;
		}
		hip->get_2(&fcp1);
		if(strisame("STANDARD"/*DNT*/,fcp1) || strisame(textstyle.resval.rstring,fcp1) || strisame(dimtxsty.resval.rstring,fcp1)) {
			//hip->get_70(&fi1);
			fi1 |= IC_REFERENCED;
			hip->set_70(fi1);
			continue;
		}
		//hip->get_70(&fi1);
		if(fi1 & IC_REFERENCED) { 
			fi1 &= ~IC_REFERENCED; 
			hip->set_70(fi1);
		}
		// EBATECH(CNBR) ]-
	}
	//* mark all xref-ed tables
	for (hip=dp->tblnext(DB_STYLETABREC,1); hip!=NULL; hip=dp->tblnext(DB_STYLETABREC,0)) {
		if(hip->ret_deleted()) continue;
		// xrefblock
		thip=((db_styletabrec *)hip)->ret_xrefblkhip();
		//Modified Cybage MM 29/10/2001 DD/MM/YYYY [
		//Reason : Fix for Bug No.77859 from Bugzilla
		hip->get_70(&fi1);
		bool current = false;
		if(fi1 & IC_REFERENCED)
			current = true; 
		//Modified Cybage MM 29/10/2001 DD/MM/YYYY ]
		if (thip!=NULL) {
			//Modified Cybage MM 29/10/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No.77859 from Bugzilla
			hip=dp->entnext_noxref(NULL); 
			if((hip!=NULL && (hip->ret_type() == DB_INSERT)) || current) // To check if the entity is either in use or the current entity  
			{
				thip->get_70(&fi1); 
				fi1 |= IC_REFERENCED;
				thip->set_70(fi1); 
			}
			//Modified Cybage MM 29/10/2001 DD/MM/YYYY ]
		}
	}

	for (hip=dp->tblnext(DB_LTYPETABREC,1); hip!=NULL; hip=dp->tblnext(DB_LTYPETABREC,0)) {
		if(hip->ret_deleted()) continue;
		// xrefblock
		thip=((db_ltypetabrec *)hip)->ret_xrefblkhip();
		if (thip!=NULL) {
			//Modified Cybage MM 29/10/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No.77859 from Bugzilla
			hip=dp->entnext_noxref(NULL); 
			if(hip!=NULL && (hip->ret_type() == DB_INSERT)) // To check if the linetype is in use
			{
				thip->get_70(&fi1);
				fi1 |= IC_REFERENCED;
				thip->set_70(fi1); 
			}
			//Modified Cybage MM 29/10/2001 DD/MM/YYYY ]
		}
	}

	for (hip=dp->tblnext(DB_LAYERTABREC,1); hip!=NULL; hip=dp->tblnext(DB_LAYERTABREC,0)) {
		if(hip->ret_deleted()) continue;
		// Linetypes
		hip->get_6(&thip);
		if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
		// xrefblock
		thip=((db_layertabrec *)hip)->ret_xrefblkhip();
		if (thip!=NULL) { 
			//Modified Cybage MM 29/10/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No.77859 from Bugzilla
			hip=dp->entnext_noxref(NULL); 
			if(hip!=NULL && (hip->ret_type() == DB_INSERT)) // To check if the layer is in use
			{
				thip->get_70(&fi1);
				fi1 |= IC_REFERENCED;
				thip->set_70(fi1);
			}
			//Modified Cybage MM 29/10/2001 DD/MM/YYYY ]
		}
	}

	for (hip=dp->tblnext(DB_DIMSTYLETABREC,1); hip!=NULL; hip=dp->tblnext(DB_DIMSTYLETABREC,0)) {
		if(hip->ret_deleted()) continue;

		// Styles
		hip->get_340(&thip);
		if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }

		// Blocks ( check dimblk, dimblk1 and dimblk2 for this dimstyle )
		((db_dimstyletabrec*)hip)->get_5(dimblkBuf,sizeof(dimblkBuf)-1);
		if( dimblkBuf[0] != NULL )
		{ // dimblk
			thip = (db_blocktabrec *)dp->findtabrec(DB_BLOCKTAB,dimblkBuf,1);
			if( thip )
			{
				thip->get_70( &fi1 );
				fi1 |= IC_REFERENCED;
				thip->set_70( fi1 );
			}
		}
		
		((db_dimstyletabrec*)hip)->get_6(dimblkBuf,sizeof(dimblkBuf)-1);
		if( dimblkBuf[0] != NULL )
		{ // dimblk1
			thip = (db_blocktabrec *)dp->findtabrec(DB_BLOCKTAB,dimblkBuf,1);
			if( thip )
			{
				thip->get_70( &fi1 );
				fi1 |= IC_REFERENCED;
				thip->set_70( fi1 );
			}
		}
	
		((db_dimstyletabrec*)hip)->get_7(dimblkBuf,sizeof(dimblkBuf)-1);
		if( dimblkBuf[0] != NULL )
		{ // dimblk2
			thip = (db_blocktabrec *)dp->findtabrec(DB_BLOCKTAB,dimblkBuf,1);
			if( thip )
			{
				thip->get_70( &fi1 );
				fi1 |= IC_REFERENCED;
				thip->set_70( fi1 );
			}
		}

	}
	
	//* Mark used table in all blocks section
	for (hip=dp->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=dp->tblnext(DB_BLOCKTABREC,0)) {
		if(hip->ret_deleted()) continue;

		// Now check all of the parts to the block.
		((db_blocktabrec *)hip)->get_blockllends(&blkhip,&ehip);
		for( ; blkhip!=NULL; blkhip=blkhip->next) {
			if(blkhip->ret_deleted()) continue;

			// Layers
			blkhip->get_8(&thip);
			if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
			// Linetypes
			blkhip->get_6(&thip);
			if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
			// Inserts
			if(blkhip->ret_type()==DB_INSERT || blkhip->ret_type()==DB_DIMENSION) {
				blkhip->get_2(&bhip);
				if(bhip) { bhip->get_70(&fi1); fi1 |= IC_REFERENCED; bhip->set_70(fi1); }
			}
			// Styles
			// EBATECH(CNBR) -[ 2002/4/29 SHAPE doesn't have DXF=7 in fact.
			if(blkhip->ret_type()==DB_TEXT || blkhip->ret_type()==DB_ATTDEF || blkhip->ret_type()==DB_ATTRIB || blkhip->ret_type()==DB_MTEXT ) {
				blkhip->get_7(&bhip);
				if(bhip) { bhip->get_70(&fi1); fi1 |= IC_REFERENCED; bhip->set_70(fi1); }
			}
			// Shape Style
			if( blkhip->ret_type()==DB_SHAPE) {
				bhip = ((db_shape*)blkhip)->ret_tship();
				if(bhip) { bhip->get_70(&fi1); fi1 |= IC_REFERENCED; bhip->set_70(fi1); }
			} // EBATECH(CNBR) ]-
			// Dim styles
			if(blkhip->ret_type()==DB_DIMENSION || blkhip->ret_type()==DB_LEADER || blkhip->ret_type()==DB_TOLERANCE) {
				blkhip->get_3(&thip);
				if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
			}
		}
	}
	
	//* Mark used table in all entities section
	for(hip=dp->entnext_noxref(NULL); hip!=NULL; hip=dp->entnext_noxref(hip)) {
		if(hip->ret_deleted()) continue;
		// Layers
		hip->get_8(&thip);
		if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
		// Linetypes
		hip->get_6(&thip);
		if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
		// Inserts
		if(hip->ret_type()==DB_INSERT || hip->ret_type()==DB_DIMENSION) {
			hip->get_2(&thip);
			if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
		}
		// Styles
		// EBATECH(CNBR) -[ 2002/4/29 SHAPE doesn't have DXF=7 in fact.
		if(hip->ret_type()==DB_TEXT || hip->ret_type()==DB_ATTDEF || hip->ret_type()==DB_ATTRIB || hip->ret_type()==DB_MTEXT ) {
			hip->get_7(&thip);
			if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
		}
		// Shape Styles
		if( hip->ret_type()==DB_SHAPE) {
			thip = ((db_shape*)hip)->ret_tship();
			if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
		} // EBATECH(CNBR) ]-
		// Dim styles
		if(hip->ret_type()==DB_DIMENSION || hip->ret_type()==DB_LEADER || hip->ret_type()==DB_TOLERANCE) {
			hip->get_3(&thip);
			if(thip) { thip->get_70(&fi1); fi1 |= IC_REFERENCED; thip->set_70(fi1); }
		}
	}
	
	// EBATECH(CNBR) -[ 2002/4/29 Check used Complex linetype
	for (hip=dp->tblnext(DB_LTYPETABREC,1); hip!=NULL; hip=dp->tblnext(DB_LTYPETABREC,0)) {
		if(hip->ret_deleted()) continue;
		hip->get_70(&fi1);
		if( fi1 & IC_REFERENCED ) {
			int i, n;
			db_ltypedashlink dash;
			hip->get_73(&n);
			for(i=0;i<n;i++){
				dash = ((db_ltypetabrec*)hip)->element(i);
				if(dash.type){ //0:none +1:rotation type +2:text +4:shape
					if(dash.stylehip){
						dash.stylehip->get_70(&fi1);
						fi1 |= IC_REFERENCED;
						dash.stylehip->set_70(fi1);
					}
				}
			}
		}
	}
	// EBATECH(CNBR) ]-

	IC_FREE(clayer.resval.rstring);
	IC_FREE(celtype.resval.rstring);
	IC_FREE(dimstyle.resval.rstring);
	IC_FREE(dimtxsty.resval.rstring);
	IC_FREE(textstyle.resval.rstring);

	return RTNORM;
}

short purge_drawing (db_drawing *dp, char *fs3)
{
	if(NULL==dp) return(RTERROR);
    char fs1[IC_ACADBUF];
    char fs2[IC_ACADBUF];
	char *fcp1;
	RT ret;
	int	fi1;
	int yestoall=0;
	db_handitem *hip;

	bool bIsBlockFound = false;
	bool bPurgeByName = false;
	char szBlockNames[IC_ACADBUF];
	CString strBlockName,strListOfBlockNames;
	int nCurrentIndex = 0 ,nPreviousIndex = -1 ;
	bool bCheckForEntries = true;
	bool bPurgeAll = false;
	int nret;
	
    
	



	if (prepare_drawing_for_purge (dp) != RTNORM)
		return RTERROR;

	int gotone=0;
	sds_name ename;
	ename[1]=(long)dp;
	if (strisame(fs3,"BATCH"/*DNT*/ ))
	{
		strcpy(fs3, "ALL"/*DNT*/);  yestoall=1;
	}
	if(strisame(fs3, "ALL"/*DNT*/))
		bPurgeAll = true;
	if (strisame(fs3,"LAYERS"/*DNT*/ ) || strisame(fs3,"ALL"/*DNT*/ ))
	{

		
		nret = sds_getstring(1,ResourceString(IDC_PURGE_BLOCK_ALL_NAME ,"\n Enter name(s) to purge <*> : "),szBlockNames);
		if(!strisame("*",szBlockNames) && strlen(szBlockNames) > 0 ) 
			bPurgeByName = true;

		strListOfBlockNames = szBlockNames;
		nCurrentIndex = 0 ;
		nPreviousIndex = -1;
		while (nCurrentIndex != -1 && nret != RTCAN)
		{

			if(bPurgeByName)
				SetPurgeBlockName(strListOfBlockNames,nCurrentIndex,nPreviousIndex,strBlockName);
			else
				nCurrentIndex = -1;

			for (hip=dp->tblnext(DB_LAYERTABREC,1); hip!=NULL; hip=dp->tblnext(DB_LAYERTABREC,0))
			{
				if(hip->ret_deleted()) continue;
				hip->get_70(&fi1);
				if(!(fi1 & IC_REFERENCED) && !(fi1 & IC_ENTRY_XREF_DEPENDENT))
				{
					hip->get_2(&fcp1);
					if(bPurgeByName)
					{
						if(strisame(strBlockName ,fcp1))
						{
							bIsBlockFound = true;
							bCheckForEntries = false;
						}
					}
					if(!bPurgeByName)
					{
						if(!yestoall)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__NPURGE_LAYER__S___5, "\nPurge layer %s? (Yes/No/yes to All)? <No>: " ),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE_INITGET_YES_TO_ALL_6, "Yes_to_all|All ~ Yes No ~_All ~_ ~_Yes ~_No" ))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if((ret!=RTNORM) || strisame("NO"/*DNT*/ ,fs1)) continue;
							if(strisame("ALL"/*DNT*/ ,fs1)) yestoall=1;
						} 
						else
						{
							sds_printf(ResourceString(IDC_PURGE__NLAYER__S_PURGED__8, "\nLayer %s purged." ),fcp1);
						}
						bCheckForEntries = false;
					}
					gotone=1;
					ename[0]=(long)hip;
					if(!bPurgeByName)
						sds_entdel(ename);
					else
					{
						if(bIsBlockFound)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__LAYER__BY__NAME,"\nPurge block %s? (Yes/No)? <No>: "),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE__INITGATE__YES_NO,"Yes No ~_Yes ~_No"))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if(strisame("YES"/*DNT*/ ,fs1))
								sds_entdel(ename);
							break;
						}
					}
				}
			}
			bIsBlockFound = false;
		if(!gotone && bPurgeAll && !bPurgeByName)
		{

			sds_printf(ResourceString(IDC_PURGE__NNO_UN_REFERENCED_9, "\nNo un-referenced layers." ));
		}

			}
		
	}
	gotone=0;
	if (strisame(fs3,"LTYPES" ) || strisame(fs3,"ALL" ))
	{

		
		if( !bPurgeAll)
		{
			nret = sds_getstring(1,ResourceString(IDC_PURGE_BLOCK_ALL_NAME ,"\n Enter name(s) to purge <*> : "),szBlockNames);
		if(!strisame("*",szBlockNames) && strlen(szBlockNames) > 0 ) 
			bPurgeByName = true;
		}
		if(nret != RTCAN)
			nret = RTNORM;
		strListOfBlockNames = szBlockNames;
		nCurrentIndex = 0 ;
		nPreviousIndex = -1;
		

		while (nCurrentIndex != -1 && nret != RTCAN)
		{

		
			if(bPurgeByName)
				SetPurgeBlockName(strListOfBlockNames,nCurrentIndex,nPreviousIndex,strBlockName);
			else
				nCurrentIndex = -1;

		
			for (hip=dp->tblnext(DB_LTYPETABREC,1); hip!=NULL; hip=dp->tblnext(DB_LTYPETABREC,0))
			{
				if(hip->ret_deleted()) continue;
				hip->get_70(&fi1);
				if(!(fi1 & IC_REFERENCED) && !(fi1 & IC_ENTRY_XREF_DEPENDENT))
				{
					hip->get_2(&fcp1);
					if(bPurgeByName)
					{
						if(strisame(strBlockName ,fcp1))
						{
							bIsBlockFound = true;
							bCheckForEntries = false;
						}
					}
					if(!bPurgeByName)
					{
						if(!yestoall)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__NPURGE_LINETYPE__11, "\nPurge linetype %s? (Yes/No/yes to All)? <No>: " ),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE_INITGET_YES_TO_ALL_6, "Yes_to_all|All ~ Yes No ~_All ~_ ~_Yes ~_No" ))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if((ret!=RTNORM) || strisame("NO"/*DNT*/ ,fs1)) continue;
							if(strisame("ALL"/*DNT*/ ,fs1)) yestoall=1;
						}
						else
						{
							sds_printf(ResourceString(IDC_PURGE__NLINETYPE__S_PUR_12, "\nLinetype %s purged." ),fcp1);
					}
						bCheckForEntries = false;
					}
				gotone=1;
				ename[0]=(long)hip;
				if(!bPurgeByName)
					sds_entdel(ename);
				else
				{
					if(bIsBlockFound)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__LINETYPE__BY__NAME,"\nPurge block %s? (Yes/No)? <No>: "),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE__INITGATE__YES_NO,"Yes No ~_Yes ~_No"))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if(strisame("YES"/*DNT*/ ,fs1))
							sds_entdel(ename);
							break;
						}
							
				}		
				
			}
		}
		bIsBlockFound = false;
		if(!gotone && bPurgeAll && !bPurgeByName)
		{
			sds_printf(ResourceString(IDC_PURGE__NNO_UN_REFERENCE_13, "\nNo un-referenced linetypes." ));
		}
		}
		
	}
	gotone=0;
	if (strisame(fs3,"BLOCKS" ) || strisame(fs3,"ALL" )) {
		//BugZilla No. 77859; 17-02-2003
		struct resbuf *rbpXref_Detach_List = NULL;		
		struct resbuf *rbpNew= NULL;
		struct resbuf *rbpTemp= NULL;
		

		

		
		
		if( !bPurgeAll)
		{
			nret = sds_getstring(1,ResourceString(IDC_PURGE_BLOCK_ALL_NAME ,"\n Enter name(s) to purge <*> : "),szBlockNames);
		if(!strisame("*",szBlockNames) && strlen(szBlockNames) > 0 ) 
			bPurgeByName = true;
		}
		if(nret != RTCAN)
			nret = RTNORM;
		strListOfBlockNames = szBlockNames;
		nCurrentIndex = 0 ;
		nPreviousIndex = -1;

		while (nCurrentIndex != -1 && nret != RTCAN)
		{
			
			if(bPurgeByName)
				SetPurgeBlockName(strListOfBlockNames,nCurrentIndex,nPreviousIndex,strBlockName);
			else
				nCurrentIndex = -1;
				
			for (hip=dp->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=dp->tblnext(DB_BLOCKTABREC,0)) 
			{
				if(hip->ret_deleted()) continue;
				if(!stricmp("*MODEL_SPACE"/*DNT*/,((db_tablerecord *)hip)->retp_name()))
					continue;
				hip->get_70(&fi1);
				if(!(fi1 & IC_REFERENCED) && !(fi1 & IC_ENTRY_XREF_DEPENDENT)) 
				{
					hip->get_2(&fcp1);
					if(bPurgeByName)
					{
						if(strisame(strBlockName ,fcp1))
						{
							bIsBlockFound = true;
							bCheckForEntries = false;
						}
					}

					if(!bPurgeByName)
					{
						if(!yestoall) 
						{
							sprintf(fs2,ResourceString(IDC_PURGE__NPURGE_BLOCK__S__15, "\nPurge block %s? (Yes/No/yes to All)? <No>: " ),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE_INITGET_YES_TO_ALL_6, "Yes_to_all|All ~ Yes No ~_All ~_ ~_Yes ~_No" ))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if((ret!=RTNORM) || strisame("NO"/*DNT*/ ,fs1)) continue;
							if(strisame("ALL"/*DNT*/ ,fs1)) yestoall=1;
						} 
						else 
						{
							sds_printf(ResourceString(IDC_PURGE__NBLOCK__S_PURGED_16, "\nBlock %s purged." ),fcp1);
					}
						bCheckForEntries = false;
					}
					gotone=1;
					ename[0]=(long)hip;
				//Modified Cybage MM 29/10/2001 DD/MM/YYYY [
				//Reason : Fix for Bug No.77859 from Bugzilla
				if(fi1 & IC_BLOCK_XREF)// To check if the block is a xref
				{
					//BugZilla No. 77859; 17-02-2003
					//cmd_xref_uncouple(fcp1, IC_UNCOUPLE_DETACH, false);
					rbpNew =sds_buildlist(1,fcp1,0);
					if ( rbpXref_Detach_List  == NULL )
						rbpXref_Detach_List  = rbpNew;
					else
					{
						//BugZilla No. 77859; 03-03-2003
						rbpNew->rbnext = rbpXref_Detach_List;
						rbpXref_Detach_List = rbpNew;
						//for (rbpTemp = rbpXref_Detach_List; rbpTemp->rbnext != NULL; rbpTemp = rbpTemp->rbnext );
						//rbpTemp->rbnext = rbpNew;
					}
				}
				else
					{
						if(!bPurgeByName)
							sds_entdel(ename);
						else
						{
		
							if(bIsBlockFound)
							{
								sprintf(fs2,ResourceString(IDC_PURGE__BLOCK__BY__NAME,"\nPurge block %s? (Yes/No)? <No>: "),fcp1);
								if(sds_initget(0,ResourceString(IDC_PURGE__INITGATE__YES_NO,"Yes No ~_Yes ~_No"))!=RTNORM) return(RTERROR);
								if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
								if(strisame("YES"/*DNT*/ ,fs1))
								sds_entdel(ename);
								break;
							}
		

						}		
				//Modified Cybage MM 29/10/2001 DD/MM/YYYY ]
					}

					
				}
			
			}
			
			bIsBlockFound = false;
		//BugZilla No. 77859; 17-02-2003
			for (rbpTemp = rbpXref_Detach_List; rbpTemp != NULL; rbpTemp = rbpTemp->rbnext )
			{
				cmd_xref_uncouple(rbpTemp->resval.rstring, IC_UNCOUPLE_DETACH, false);
			}

			FreeResbufIfNotNull(&rbpXref_Detach_List);
			if(!gotone && bPurgeAll && !bPurgeByName) 
			{
				sds_printf(ResourceString(IDC_PURGE__NNO_UN_REFERENCE_17, "\nNo un-referenced blocks." ));
				break;
			}
		}

		

	}
	gotone=0;
	if (strisame(fs3,"STYLES" ) || strisame(fs3,"ALL" ))
	{

		
		if( !bPurgeAll)
		{
			nret = sds_getstring(1,ResourceString(IDC_PURGE_BLOCK_ALL_NAME ,"\n Enter name(s) to purge <*> : "),szBlockNames);
		if(!strisame("*",szBlockNames) && strlen(szBlockNames) > 0 ) 
			bPurgeByName = true;
		}
		if(nret != RTCAN)
			nret = RTNORM;
		strListOfBlockNames = szBlockNames;
		nCurrentIndex = 0 ;
		nPreviousIndex = -1;
		
		while (nCurrentIndex != -1 && nret != RTCAN)
		{

		
			if(bPurgeByName)
				SetPurgeBlockName(strListOfBlockNames,nCurrentIndex,nPreviousIndex,strBlockName);
			else
				nCurrentIndex = -1;

			for (hip=dp->tblnext(DB_STYLETABREC,1); hip!=NULL; hip=dp->tblnext(DB_STYLETABREC,0)) 
			{
				if(hip->ret_deleted()) continue;
				hip->get_70(&fi1);
				if(!(fi1 & IC_REFERENCED) && !(fi1 & IC_ENTRY_XREF_DEPENDENT))
				{
				// EBATECH(CNBR) -[  2001/4/16 Shape font's style name is null.
					if( fi1 & IC_SHAPEFILE_SHAPES )
						hip->get_3(&fcp1);
					else
						hip->get_2(&fcp1);
				//hip->get_2(&fcp1);
				// EBATECH(CNBR) ]- 2001/4/16
					
					if(bPurgeByName)
					{
						if(strisame(strBlockName ,fcp1))
						{
							bIsBlockFound = true;
							bCheckForEntries = false;
						}
					}
					
					if(!bPurgeByName)
					{
						if(!yestoall)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__NPURGE_TEXT_STYL_19, "\nPurge text style %s? (Yes/No/yes to All)? <No>: " ),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE_INITGET_YES_TO_ALL_6, "Yes_to_all|All ~ Yes No ~_All ~_ ~_Yes ~_No" ))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if((ret!=RTNORM) || strisame("NO" ,fs1)) continue;
							if(strisame("ALL" ,fs1)) yestoall=1;
						} 
						else
						{
							sds_printf(ResourceString(IDC_PURGE__NTEXT_STYLE__S_P_20, "\nText style %s purged." ),fcp1);
						}

					
					}
					gotone=1;
					ename[0]=(long)hip;

					if(!bPurgeByName)
						sds_entdel(ename);
					else
					{
						
						if(bIsBlockFound)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__TEXTSTYLE__BY__NAME,"\nPurge block %s? (Yes/No)? <No>: "),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE__INITGATE__YES_NO,"Yes No ~_Yes ~_No"))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if(strisame("YES"/*DNT*/ ,fs1))
							sds_entdel(ename);
							break;
						}
			
					}		
				
			}
		}
		bIsBlockFound = false;	
			if(!gotone && bPurgeAll && !bPurgeByName)
		{
				sds_printf(ResourceString(IDC_PURGE__NNO_UN_REFERENCE_21, "\nNo un-referenced text styles." ));
			}
		}
		
	}
	gotone=0;
	if (strisame(fs3,"DIMSTYLES"/*DNT*/ ) || strisame(fs3,"ALL"/*DNT*/ ))
	{
	
		if( !bPurgeAll)
		{
			nret = sds_getstring(1,ResourceString(IDC_PURGE_BLOCK_ALL_NAME ,"\n Enter name(s) to purge <*> : "),szBlockNames);
		if(!strisame("*",szBlockNames) && strlen(szBlockNames) > 0 ) 
			bPurgeByName = true;
		}
		if(nret != RTCAN)
			nret = RTNORM;
		strListOfBlockNames = szBlockNames;
		nCurrentIndex = 0 ;
		nPreviousIndex = -1;
		

		while (nCurrentIndex != -1 && nret != RTCAN)
		{

		
			if(bPurgeByName)
				SetPurgeBlockName(strListOfBlockNames,nCurrentIndex,nPreviousIndex,strBlockName);
			else
				nCurrentIndex = -1;

			for (hip=dp->tblnext(DB_DIMSTYLETABREC,1); hip!=NULL; hip=dp->tblnext(DB_DIMSTYLETABREC,0)) 
			{
				if(hip->ret_deleted()) continue;
				hip->get_70(&fi1);
				if(!(fi1 & IC_REFERENCED) && !(fi1 & IC_ENTRY_XREF_DEPENDENT))
				{
					hip->get_2(&fcp1);
					if(bPurgeByName)
					{
						if(strisame(strBlockName ,fcp1))
						{
							bIsBlockFound = true;
							bCheckForEntries = false;
						}
					}
	
					if(!bPurgeByName)
					{
						if(!yestoall)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__NPURGE_DIMENSION_23, "\nPurge dimension style %s? (Yes/No/yes to All)? <No>: " ),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE_INITGET_YES_TO_ALL_6, "Yes_to_all|All ~ Yes No ~_All ~_ ~_Yes ~_No" ))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if((ret!=RTNORM) || strisame("NO"/*DNT*/ ,fs1)) continue;
							if(strisame("ALL"/*DNT*/ ,fs1)) yestoall=1;
						} 
						else
						{
							sds_printf(ResourceString(IDC_PURGE__NDIMENSION_STYLE_24, "\nDimension style %s purged." ),fcp1);
						}
						bCheckForEntries = false;
					}
					gotone=1;
					ename[0]=(long)hip;
					if(!bPurgeByName)
						sds_entdel(ename);
					else
					{
						
						if(bIsBlockFound)
						{
							sprintf(fs2,ResourceString(IDC_PURGE__DIMSTYLE__BY__NAME,"\nPurge block %s? (Yes/No)? <No>: "),fcp1);
							if(sds_initget(0,ResourceString(IDC_PURGE__INITGATE__YES_NO,"Yes No ~_Yes ~_No"))!=RTNORM) return(RTERROR);
							if(RTCAN==(ret=sds_getkword(fs2,fs1))) return(ret);
							if(strisame("YES"/*DNT*/ ,fs1))
							sds_entdel(ename);
							break;
						}
					
					}
					
					
				}
			}
		bIsBlockFound = false;	
		if(!gotone && bPurgeAll && !bPurgeByName)
		{
			sds_printf(ResourceString(IDC_PURGE__NNO_UN_REFERENCE_25, "\nNo un-referenced dimension styles." ));
		}
	}
		

	}

		if(bCheckForEntries && nret != RTCAN)
		{
			if(bPurgeByName)
			sds_printf(ResourceString(IDC_PURGE_NO_ENTRIES, "\nNo entries found with the name %s " ),szBlockNames);

	}

    return(RTNORM);
}



/********************************************************************************
 * Author:	Mohan Nayak.
 *
 * Purpose:	To set the individual block name from block name list.
 *
 * Returns:	void
 ********************************************************************************/

void SetPurgeBlockName(CString & tmpBlockList, int& tmpCurrentIndex ,int& tmpPrevIndex,CString & tmpBlockName)
{

	
	tmpCurrentIndex = tmpBlockList.Find(",",tmpCurrentIndex + 1); 
	if (tmpCurrentIndex != -1)
		tmpBlockName = tmpBlockList.Mid(tmpPrevIndex + 1, tmpCurrentIndex - tmpPrevIndex -1); 
	else
		tmpBlockName = tmpBlockList.Mid(tmpPrevIndex + 1,tmpBlockList.GetLength() - tmpPrevIndex -1);
	
	tmpPrevIndex = tmpCurrentIndex;
	
}


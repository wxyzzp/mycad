// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadView.h"
#include "cmdsDimBlock.h"

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern                        SDS_AtNodeDrag;
extern struct cmd_dimvars plinkvar[];
//BugZilla No. 78155; 27-05-2002
//extern struct cmd_dimlastpt lastdimpts;
extern struct cmd_dimeparlink* plink;
//Modified Cybage SBD 03/12/2001 DD/MM/YYYY 
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla ( DIMTEDIT case - 1 a )
extern BOOL bdimUpdate;


// ============================================================================================================================================================
// ========================================================= DIMTEDIT FUNCTIONS ===============================================================================
// ============================================================================================================================================================
//
// This area has only the functions related to DIMTEDIT function. this function is called to change some attributs on other dimension types.
//
//
// ============================================================================================================================================================
// EBATECH(CNBR) -[ Never use UpdateDimjust()
#if 0
//Bugzilla No. 78023 ; date : 18-03-2002 [
// This function updates dimjust value in entity extended data list. If dimjust node not 
// present then adds such node in it. If such list not present then creates one.
void UpdateDimjust(struct resbuf **elist, int nDimjust )
{ 
	struct resbuf* rbt = NULL;
	struct resbuf* rbptr = NULL;
	// Search for ACAD application name.
	for(rbptr=rbt=*elist; rbt!=NULL ;rbt=rbt->rbnext)
	{
		if ( rbt->restype == AD_XD_APPOBJHANDLE && strisame(rbt->resval.rstring, DIM_ACADAPP) ) 
			break;
	}
	if (rbt==NULL)
	{
		struct resbuf* exlist_remaining_part = rbptr->rbnext;
		
		//Bugzilla No. 78104 ; 15-04-2002 [
		rbptr->rbnext=sds_newrb(AD_XD_APPOBJHANDLE); rbptr=rbptr->rbnext; rbptr->resval.rstring= new char [strlen(DIM_ACADAPP)+1]; strcpy(rbptr->resval.rstring,DIM_ACADAPP);
		rbptr->rbnext=sds_newrb(AD_XD_STRING); rbptr=rbptr->rbnext; rbptr->resval.rstring= new char [strlen(DIM_DSTYLE)+1]; strcpy(rbptr->resval.rstring,DIM_DSTYLE);
		rbptr->rbnext=sds_newrb(AD_XD_CONTROL); rbptr=rbptr->rbnext; rbptr->resval.rstring= new char [2]; strcpy(rbptr->resval.rstring,"{"/*DNT*/ );
		//Bugzilla No. 78104 ; 15-04-2002 ]
		// Add the dimjust var settings.
		rbt = rbptr->rbnext=sds_newrb(AD_XD_INT); rbptr=rbptr->rbnext; rbptr->resval.rint=GC_DIMJUST;
		rbptr->rbnext=sds_newrb(AD_XD_INT); rbptr=rbptr->rbnext; rbptr->resval.rint=nDimjust;
		//Bugzilla No. 78104 ; 15-04-2002 
		// Add the closing of the list.
		rbptr->rbnext=sds_newrb(AD_XD_CONTROL); rbptr=rbptr->rbnext; rbptr->resval.rstring= new char [2]; strcpy(rbptr->resval.rstring,"}"/*DNT*/ );
		
		rbptr->rbnext = exlist_remaining_part;
	}
	else
	{ 
		// If application exists, Search for DSTYLE  name.
		rbptr = rbt;
		for( rbt=rbt->rbnext;rbt!=NULL;rbt=rbt->rbnext)
		{
			if ( rbt->restype == AD_XD_STRING && strisame(rbt->resval.rstring,DIM_DSTYLE) )
				break;   
		}
		if (rbt==NULL)
		{
			struct resbuf* exlist_remaining_part = rbptr->rbnext;
			
			//Bugzilla No. 78104 ; 15-04-2002 [
			rbptr->rbnext=sds_newrb(AD_XD_STRING); rbptr=rbptr->rbnext; rbptr->resval.rstring= new char [strlen(DIM_DSTYLE)+1]; strcpy(rbptr->resval.rstring,DIM_DSTYLE);
			rbptr->rbnext=sds_newrb(AD_XD_CONTROL); rbptr=rbptr->rbnext; rbptr->resval.rstring= new char [2]; strcpy(rbptr->resval.rstring,"{"/*DNT*/ );
			//Bugzilla No. 78104 ; 15-04-2002 ]
			// Add the dimjust var settings.
			rbt = rbptr->rbnext=sds_newrb(AD_XD_INT); rbptr=rbptr->rbnext; rbptr->resval.rint=GC_DIMJUST;
			rbptr->rbnext=sds_newrb(AD_XD_INT); rbptr=rbptr->rbnext; rbptr->resval.rint=nDimjust;
			//Bugzilla No. 78104 ; 15-04-2002 
			// Add the closing of the list.
			rbptr->rbnext=sds_newrb(AD_XD_CONTROL); rbptr=rbptr->rbnext; rbptr->resval.rstring= new char [2]; strcpy(rbptr->resval.rstring,"}"/*DNT*/ );
			
			rbptr->rbnext = exlist_remaining_part;
			
		}
		else
		{
		//If ACAD node and DSTYLE node is present then atleast one dimension variable should be overriden
		// hence DSTYLE node followed by "{"  should also be present
		for( rbt=rbt->rbnext;rbt!=NULL ;rbt=rbt->rbnext)
		{
			if(rbt->restype == AD_XD_CONTROL && strisame(rbt->resval.rstring,"{"/*DNT*/) )    
				break;       
		}   
		
		ASSERT(rbt);		
		if ( rbt == NULL ) return ;

		// Move till the end of dimension override list.
		rbptr=rbt;
		for( rbt=rbt->rbnext;rbt!=NULL ;rbt=rbt->rbnext)
		{
			if(rbt->restype == AD_XD_CONTROL && strisame(rbt->resval.rstring,"}"/*DNT*/) )
			{
				rbt = NULL;
				break;   
			}
			if(rbt->restype == AD_XD_INT && rbt->resval.rint == GC_DIMJUST)
			{
				rbt=rbt->rbnext; rbt->resval.rint=nDimjust;
				break;
			}
		}
		if (rbt==NULL)
		{   
			struct resbuf* exlist_remaining_part = rbptr->rbnext;     
			
			// Add the dimjust var settings.
			rbt = rbptr->rbnext=sds_newrb(AD_XD_INT); rbptr=rbptr->rbnext; rbptr->resval.rint=GC_DIMJUST;
			rbptr->rbnext=sds_newrb(AD_XD_INT); rbptr=rbptr->rbnext; rbptr->resval.rint=nDimjust;     
			
			rbptr->rbnext = exlist_remaining_part;   
		}
		}		
	}  	
}
//Bugzilla No. 78023 ; date : 18-03-2002 ]
#endif

short cmd_dimtedit(void) {
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    struct resbuf setgetrb,
                  rb,
                  rbwcs,
                  rbucs,
                  rbecs,
                 *elist=NULL,
                 //*newlist=NULL,
                 *rbb=NULL,
                 *rbt=NULL;
    sds_name      ename = {0L, 0L},
                  ename2 = {0L, 0L};
    int           ret = 0,
                  type = 0;
    sds_point     pt10 = {0.0, 0.0, 0.0},
                  prvpt = {0.0, 0.0, 0.0};
	sds_point     pt15 = {0.0, 0.0, 0.0},
				  pt14 = {0.0, 0.0, 0.0};	
    db_handitem  *elp=NULL;
    CString		  pmt;
    TCHAR         fs1[IC_ACADBUF] = ""/*DNT*/;
    sds_real      fr1 = 0.0;
	bool bGroup53 = false;
	struct resbuf* pGroup53 = NULL;
	int dimtmove = 0;		// Bugzilla#78455 13-03-2003 Use DIMTMOVE

    cmd_initialize();
    cmd_setparlink(SDS_CURDWG);

    plink->txtangflag=FALSE;
    // Make sure we get all the applictions in the dimension.
    setgetrb.restype=RTSTR; setgetrb.resval.rstring="*"/*DNT*/; setgetrb.rbnext=NULL;
    // Select the dimension to edit loop.
    for (;;) {
        if ((ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NSELECT_DIM_64, "\nSelect dimension to reposition text: " ),ename,pt10))==RTERROR) {
            if (SDS_getvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
            if (rb.resval.rint==OL_ENTSELPICK) continue; else goto exit;
        } else if (ret==RTCAN) goto exit;
        if ((elist=sds_entgetx(ename,&setgetrb))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
        if (ic_assoc(elist,0)!=0) goto exit;
        if (!strsame("DIMENSION"/*DNT*/,ic_rbassoc->resval.rstring)) {
			IC_RELRB(elist);			// If it's not a dimension, lose it, and try again. 
			cmd_ErrorPrompt(CMD_ERR_DIMENSION,0);
			continue;
		}
        break;
    }
    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,SDS_CURDWG);
    for (rbb=elist;rbb;rbb=rbb->rbnext) {
        switch(rbb->restype) {
            case  10: ic_ptcpy(plink->defpt,rbb->resval.rpoint); break;
            case  11: ic_ptcpy(plink->textmidpt,rbb->resval.rpoint); break;
            case  12: sds_trans(rbb->resval.rpoint,&rbwcs,&rbecs,0,plink->blkipt); break;
            case  13: ic_ptcpy(plink->pt13,rbb->resval.rpoint); break;
            case  14: ic_ptcpy(plink->pt14,rbb->resval.rpoint); break;
            case  15: ic_ptcpy(plink->pt15,rbb->resval.rpoint); break;
            case  16: ic_ptcpy(plink->pt16,rbb->resval.rpoint); break;
            case  40: plink->leadlen=rbb->resval.rreal; break;
            case  50: plink->dlnang =rbb->resval.rreal; break;
            case  51: plink->hdir   =rbb->resval.rreal; break;
            case  52: plink->elnang =rbb->resval.rreal; break;
            case  53: plink->textrot=rbb->resval.rreal; break;
            case  70: plink->flags  =rbb->resval.rint; break;
            case   1: strcpy(plink->text,rbb->resval.rstring); break;
			case 210: ic_ptcpy(plink->pt210,rbb->resval.rpoint); break;
        }
    }

    type=plink->flags;
    //if (type&DIM_TEXT_MOVED) type -= DIM_TEXT_MOVED;
    type &= ~DIM_TEXT_MOVED;
    //if (type&DIM_BIT_6)      type -= DIM_BIT_6;
    type &= ~DIM_BIT_6;

	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003
	SDS_AtNodeDrag = 1;
	cmd_geteedvalues((db_handitem *)ename[0],(db_drawing *)ename[1]);
	SDS_AtNodeDrag = 0;
	dimtmove = plinkvar[DIMTMOVE].dval.ival;
	// EBATECH(CNBR) ]-

    // Command prompt.
	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Add Center option
	if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_SPEC_65, "Specify_Angle|Angle ~ Left_justify|Left Center_justify|Center Right_justify|Right ~ Restore_text|Home ~REstore ~_Angle ~_ ~_Left ~_Center ~_Right ~_ ~_Home ~_REstore" ))!=RTNORM)
    		goto exit;
	pmt=ResourceString(IDC_DIMENSIONS__NENTER_TEXT_66, "\nSpecify or enter text location:  REstore/Angle/Left/Center/Right: " );
	//if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_SPEC_65, "Specify_Angle|Angle ~ Left_justify|Left Right_justify|Right ~ Restore_text|Home ~REstore ~_Angle ~_ ~_Left ~_Right ~_ ~_Home ~_REstore" ))!=RTNORM) goto exit;
	//pmt=ResourceString(IDC_DIMENSIONS__NENTER_TEXT_66, "\nEnter text location:  REstore/Angle/Left/Right: " );
	//EBATECH(CNBR) ]-

    // Delete the old dimension.
	//sds_entdel(ename);
	sds_redraw(ename, IC_REDRAW_UNDRAW);

	//Bugzilla No. 78035 ; 19-08-2002 [
	elp = (db_handitem *)ename[0];
    /*elp=new db_dimension(SDS_CURDWG);
    elp->set_10(plink->defpt);
    elp->set_11(plink->textmidpt);
    elp->set_12(plink->blkipt);
    elp->set_70(plink->flags);
    elp->set_1(plink->text);
    elp->set_13(plink->pt13);
    elp->set_14(plink->pt14);
    elp->set_15(plink->pt15);
    elp->set_16(plink->pt16);
    elp->set_40(plink->leadlen);
    elp->set_50(plink->dlnang);
    elp->set_51(plink->hdir);
    elp->set_52(plink->elnang);
    elp->set_53(plink->textrot);
	elp->set_210(plink->pt210);*/
	//Bugzilla No. 78035 ; 19-08-2002 ]

    ename2[0]=(long)elp;
    ename2[1]=(long)SDS_CURDWG;
    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE == 1 or 2
	if( dimtmove == 2 || ( dimtmove == 1 && type != DIM_x_ORDINATE && type != DIM_Y_ORDINATE ))
	{
		SDS_AtNodeDrag=2;
		ic_ptcpy(pt10,plink->defpt);
		ic_ptcpy(prvpt,plink->textmidpt);
		ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_11(),0.0,pmt,NULL,plink->textmidpt,NULL);
		if( ret == RTCAN || ret == RTNONE || ret == RTKWORD)
		{
			ic_ptcpy(plink->textmidpt,prvpt);
		}
		else if( ret != RTKWORD )
		{
			sds_trans(plink->textmidpt,&rbucs,&rbwcs,0,plink->textmidpt);
		}
	}
	else
	{
	// EBATECH(CNBR) ]-
		switch(type)
		{
        case DIM_ANGULAR_3P:
            SDS_AtNodeDrag=1;
            ic_ptcpy(prvpt,plink->defpt);
			// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Use all same prompt.
			ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_10(),0.0,pmt,NULL,pt10,NULL);
			//ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_10(),0.0,ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_67, "\nAngle/Text/<Location of dimension arc>: " ),NULL,pt10,NULL);
			//EBATECH(CNBR) ]-
			if (ret==RTCAN || ret==RTNONE)
			{
                ic_ptcpy(plink->defpt,prvpt);
            }
			//Bugzilla No. 78035 ; 19-08-2002 
            //delete elp; elp=NULL;
            break;
        case DIM_ANGULAR:
            SDS_AtNodeDrag=1;
            ic_ptcpy(prvpt,plink->pt16);
			// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Use all same prompt.
			ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_16(),0.0,pmt,NULL,pt10,NULL);
			//ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_16(),0.0,ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_67, "\nAngle/Text/<Location of dimension arc>: " ),NULL,pt10,NULL);
			//EBATECH(CNBR) ]-
			if (ret==RTCAN || ret==RTNONE)
			{
                ic_ptcpy(plink->pt16,prvpt);
            }
			//Bugzilla No. 78035 ; 19-08-2002 
            //delete elp; elp=NULL;
            break;
        case DIM_x_ORDINATE:
        case DIM_Y_ORDINATE:
			// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE==2
			if( plinkvar[DIMTMOVE].dval.ival == 2 )
			{
				ic_ptcpy(pt10,plink->defpt);
				SDS_AtNodeDrag=2;
				ic_ptcpy(prvpt,plink->textmidpt);
				ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_11(),0.0,pmt,NULL,plink->textmidpt,NULL);
				if (ret == RTCAN || ret == RTNONE || ret == RTKWORD)
				{
					ic_ptcpy(plink->textmidpt,prvpt);
				}
				else if( ret != RTKWORD )
				{
					sds_trans(plink->textmidpt,&rbucs,&rbwcs,0,plink->textmidpt);
				}
			}
			else
			{
				SDS_AtNodeDrag=1;
				ic_ptcpy(prvpt,plink->pt14);
				// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Use all same prompt.
				ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_14(),0.0,pmt,NULL,pt10,NULL);
				//ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_14(),0.0,ResourceString(IDC_DIMENSIONS__NTEXT_X_DAT_68, "\nText/X datum/Y datum/<Leader endpoint>: " ),NULL,pt10,NULL);
				//EBATECH(CNBR) ]-
				if (ret==RTCAN || ret==RTNONE)
				{
					ic_ptcpy(plink->pt14,prvpt);
				}
				//Bugzilla No. 78035 ; 19-08-2002 
				//delete elp; elp=NULL;
			}
			// EBATECH(CNBR) ]-
            break;
        case DIM_DIAMETER:
        case DIM_RADIUS:
			ic_ptcpy(pt14,plink->pt14);
			ic_ptcpy(pt15,plink->pt15);
			ic_ptcpy(pt10,plink->defpt);
            SDS_AtNodeDrag=2;
            ic_ptcpy(prvpt,plink->textmidpt);
            ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_11(),0.0,pmt,NULL,plink->textmidpt,NULL);
			if (ret==RTNORM)
				ic_ptcpy(plink->pt14, plink->textmidpt);
			//Bugzilla No. 78180 ; 04-08-2003 [
			else if (ret==RTCAN || ret==RTNONE) 
			{
				elp->set_10(pt10);
			    elp->set_11(prvpt);
			    elp->set_14(pt14);
			    elp->set_15(pt15);
			}
			//Bugzilla No. 78180 ; 04-08-2003 ]
            else if(ret==RTKWORD) 
			{
				ic_ptcpy(plink->textmidpt,prvpt);
				ic_ptcpy(plink->pt15,pt15);
			}
			
			//Bugzilla No. 78180 ; 04-08-2003
			if (ret!=RTKWORD && ret!=RTCAN && ret!=RTNONE) 
				sds_trans(plink->textmidpt,&rbucs,&rbwcs,0,plink->textmidpt);
            break;
        case DIM_ALIGNED:
        case DIM_ROT_HOR_VER:
            ic_ptcpy(pt10,plink->defpt);
            SDS_AtNodeDrag=2;
            ic_ptcpy(prvpt,plink->textmidpt);
            ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_11(),0.0,pmt,NULL,plink->textmidpt,NULL);
			//Bugzilla No. 78179 ; 10-06-2002 [
            //if (ret==RTCAN || ret==RTNONE) ic_ptcpy(plink->textmidpt,prvpt); else if (ret!=RTKWORD) sds_trans(plink->textmidpt,&rbucs,&rbwcs,0,plink->textmidpt);
			if (ret == RTCAN || ret == RTNONE || ret == RTKWORD)
			{
				ic_ptcpy(plink->textmidpt,prvpt);
			}
			else if( ret != RTKWORD )
			{
				sds_trans(plink->textmidpt,&rbucs,&rbwcs,0,plink->textmidpt);
			}
			//Bugzilla No. 78179 ; 10-06-2002 ]
			//Bugzilla No. 78035 ; 19-08-2002 
            //delete elp; elp=NULL;
            break;
        default:
			//Bugzilla No. 78035 ; 19-08-2002 
            //delete elp; elp=NULL;
            goto exit;
    }
	}
    // Set exlist to the begining of the xdata if any exists.
    //for(exlist = elist; exlist && exlist->restype != -3; exlist = exlist->rbnext);
    // Now set exlist to the dimjust var if any.
    //if(exlist)
    //{
	//	for(rbt = exlist; rbt; rbt = rbt->rbnext)
	//	{
	//		if(rbt->restype == AD_XD_INT && rbt->resval.rint == GC_DIMJUST)
	//			break;
	//	}
	//}
    // Here there needs to be somthing. ===========================================
	// Bugzilla No. 78023 ; date : 18-03-2002 
	// Deleted the code present here and created a fucntion :UpdateDimjust() for the same.
	if (ret==RTERROR)
	{
		goto exit;
	}
	if (ret==RTKWORD)
	{
        rb.restype=RTSHORT;
		if(sds_getinput(fs1)!=RTNORM)
		{
			goto exit;
		}
        strupr(fs1);
		// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTEDIT for ORDINATE Dimension rises an error.
		if(( type == DIM_x_ORDINATE || type == DIM_Y_ORDINATE ) && 
		 ( strsame("LEFT"/*DNT*/ ,fs1) || strsame("CENTER"/*DNT*/, fs1 ) || strsame("RIGHT"/*DNT*/, fs1)))
		{
			cmd_ErrorPrompt(CMD_ERR_CANTJUSTFY,0);
			goto exit;
		}
		// EBATECH(CNBR) ]-
        if(strsame("LEFT"/*DNT*/ ,fs1)) {
			plink->flags=type;
            SDS_AtNodeDrag=0;
            ic_ptcpy(plink->defpt,pt10);
			// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 UpdateDimjust create elist from NULL.
#if 1
			plink->HomeText=TRUE;
			plinkvar[DIMJUST].dval.ival = 1;
#else
            if (!exlist) {
                // Start the begining of the list.
				bEEDCreated = true;
                rbb=exlist=sds_newrb(-3);
				//Bugzilla No. 78104 ; 15-04-2002 [
                rbb->rbnext=sds_newrb(AD_XD_APPOBJHANDLE); rbb=rbb->rbnext; rbb->resval.rstring= new char [strlen(DIM_ACADAPP)+1]; strcpy(rbb->resval.rstring,DIM_ACADAPP);
                rbb->rbnext=sds_newrb(AD_XD_STRING); rbb=rbb->rbnext; rbb->resval.rstring= new char [strlen(DIM_DSTYLE)+1]; strcpy(rbb->resval.rstring,DIM_DSTYLE);
                rbb->rbnext=sds_newrb(AD_XD_CONTROL); rbb=rbb->rbnext; rbb->resval.rstring= new char [2]; strcpy(rbb->resval.rstring,"{"/*DNT*/ );
				//Bugzilla No. 78104 ; 15-04-2002 ]
                // Add the dimjust var settings.
                rbb->rbnext=sds_newrb(AD_XD_INT); rbb=rbb->rbnext; rbb->resval.rint=GC_DIMJUST;
                rbb->rbnext=sds_newrb(AD_XD_INT); rbb=rbb->rbnext; rbb->resval.rint=1;
                //Bugzilla No. 78104 ; 15-04-2002 
				// Add the closing of the list.
                rbb->rbnext=sds_newrb(AD_XD_CONTROL); rbb=rbb->rbnext; rbb->resval.rstring= new char [2]; strcpy(rbb->resval.rstring,"}"/*DNT*/ );
            } else {
				//rbt=rbt->rbnext; rbt->resval.rint=1;
				//Bugzilla No. 78023 ; date : 18-03-2002
				UpdateDimjust(&exlist,1);
            }
#endif
			// EBATECH(CNBR) ]-
            // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
            // native format for compatability when someone does an entmake from the command line.
            sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
            sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
            sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
            sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
            sds_trans(plink->pt16,&rbucs,&rbecs,0,plink->pt16);
		// EBATECH(CNBR) -[ Bugzilla#78455 DIMTEDIT Center option is added.
		} else if(strsame("CENTER"/*DNT*/ ,fs1)) {
			plink->flags=type;
			SDS_AtNodeDrag=0;
			ic_ptcpy(plink->defpt,pt10);
			plink->HomeText=TRUE;
			plinkvar[DIMJUST].dval.ival=0;
			// Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
			// native format for compatability when someone does an entmake from the command line.
			sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
			sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
			sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
			sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
			sds_trans(plink->pt16,&rbucs,&rbecs,0,plink->pt16);
		// EBATECH(CNBR) ]-
        } else if(strsame("RIGHT"/*DNT*/ ,fs1)) {
			plink->flags=type;
            SDS_AtNodeDrag=0;
            ic_ptcpy(plink->defpt,pt10);
			// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 UpdateDimjust create elist from NULL.
#if 1
			plink->HomeText=TRUE;
			plinkvar[DIMJUST].dval.ival=2;
#else
            if (!exlist) {
                // Start the begining of the list.
				bEEDCreated = true;
                rbb=exlist=sds_newrb(-3);
                //Bugzilla No. 78104 ; 15-04-2002 [
				rbb->rbnext=sds_newrb(AD_XD_APPOBJHANDLE); rbb=rbb->rbnext; rbb->resval.rstring= new char [strlen(DIM_ACADAPP)+1]; strcpy(rbb->resval.rstring,DIM_ACADAPP);
                rbb->rbnext=sds_newrb(AD_XD_STRING); rbb=rbb->rbnext; rbb->resval.rstring= new char [strlen(DIM_DSTYLE)+1]; strcpy(rbb->resval.rstring,DIM_DSTYLE);
                rbb->rbnext=sds_newrb(AD_XD_CONTROL); rbb=rbb->rbnext; rbb->resval.rstring= new char [2]; strcpy(rbb->resval.rstring,"{"/*DNT*/ );
				//Bugzilla No. 78104 ; 15-04-2002 ]
                // Add the dimjust var settings.
                rbb->rbnext=sds_newrb(AD_XD_INT); rbb=rbb->rbnext; rbb->resval.rint =GC_DIMJUST;
                rbb->rbnext=sds_newrb(AD_XD_INT); rbb=rbb->rbnext; rbb->resval.rint=2;
                //Bugzilla No. 78104 ; 15-04-2002 
				// Add the closing of the list.
                rbb->rbnext=sds_newrb(AD_XD_CONTROL); rbb=rbb->rbnext; rbb->resval.rstring= new char [2]; strcpy(rbb->resval.rstring,"}"/*DNT*/ );
            } else {
				//rbt=rbt->rbnext; rbt->resval.rint=2;
				//Bugzilla No. 78023 ; date : 18-03-2002
				UpdateDimjust(&exlist,2);
            }
#endif
			// EBATECH(CNBR) ]-
            // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
            // native format for compatability when someone does an entmake from the command line.
            sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
            sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
            sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
            sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
            sds_trans(plink->pt16,&rbucs,&rbecs,0,plink->pt16);
        } else if((strsame("HOME"/*DNT*/ ,fs1)) || (strsame("RESTORE"/*DNT*/ ,fs1))) {	// I added "Restore" for consistency with dimedit.
			plink->flags=type;
            SDS_AtNodeDrag=0;
            ic_ptcpy(plink->defpt,pt10);
			//Bugzilla No. 78179 ; 10-06-2002
            plink->textrot=0.0;
			plink->HomeText=TRUE;
			plink->flags &= ~DIM_TEXT_MOVED;
            // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
            // native format for compatability when someone does an entmake from the command line.
            sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
            sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
            sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
            sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
            sds_trans(plink->pt16,&rbucs,&rbecs,0,plink->pt16);
        } else if(strsame("ANGLE"/*DNT*/ ,fs1)) {
            SDS_AtNodeDrag=0;
            ic_ptcpy(plink->defpt,pt10);
            if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NANGLE_FOR__73, "\nAngle for dimension text: " ),&fr1))==RTERROR) goto exit; else if (ret==RTCAN) goto exit;
            plink->txtangflag=TRUE;
            plink->textrot=fr1;
            // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
            // native format for compatability when someone does an entmake from the command line.
            sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
            sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
            sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
            sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
            sds_trans(plink->pt16,&rbucs,&rbecs,0,plink->pt16);
        }
    }
#if 0
	/*D.Gavrilov*/// We must save the old dim's layer in the new one.
	struct resbuf *tmp_rb;
	char* LayerName;
	tmp_rb = ic_ret_assoc(elist, 8);
    if(tmp_rb == NULL)
	{
	    if(SDS_getvar(NULL, DB_QCLAYER, tmp_rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
			goto exit;
	}	
	LayerName= new char [strlen(tmp_rb->resval.rstring)+1];	
    strcpy(LayerName, tmp_rb->resval.rstring);

    if ((newlist=sds_buildlist(RTDXF0  ,"DIMENSION"/*DNT*/,
									8  ,LayerName,
                                    1  ,plink->text,
                                    10 ,plink->defpt,
                                    11 ,plink->textmidpt,
                                    12 ,plink->blkipt,
                                    13 ,plink->pt13,
                                    14 ,plink->pt14,
                                    15 ,plink->pt15,
                                    16 ,plink->pt16,
                                    40 ,plink->leadlen,
                                    50 ,plink->dlnang,
                                    51 ,plink->hdir,
                                    52 ,0.0,
                                    53 ,plink->textrot,
                                    70 ,plink->flags,
									210,plink->pt210,0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
    //if (exlist) 
	//{
    //    for (rbb=elist;rbb;rbb=rbb->rbnext) { if (!rbb->rbnext || rbb->rbnext->restype==-3) break; }
    //    rbb->rbnext=exlist;
    //}
#endif
	for(rbt = elist; rbt; rbt = rbt->rbnext)
	{
		switch(rbt->restype)
		{
		case 1:
			if(strcmp(rbt->resval.rstring, plink->text ) != 0 )
			{
			rbt->resval.rstring = ic_realloc_char(rbt->resval.rstring, strlen(plink->text) + 1);
			strcpy(rbt->resval.rstring, plink->text);
			}
			break;
		case 10:
			ic_ptcpy(rbt->resval.rpoint, plink->defpt);
			break;
		case 11:
			ic_ptcpy(rbt->resval.rpoint, plink->textmidpt);
			break;
		case 12:
			ic_ptcpy(rbt->resval.rpoint, plink->blkipt);
			break;
		case 13:
			ic_ptcpy(rbt->resval.rpoint, plink->pt13);
			break;
		case 14:
			ic_ptcpy(rbt->resval.rpoint, plink->pt14);
			break;
		case 15:
			ic_ptcpy(rbt->resval.rpoint, plink->pt15);
			break;
		case 16:
			ic_ptcpy(rbt->resval.rpoint, plink->pt16);
			break;
		case 40:
			rbt->resval.rreal = plink->leadlen;
			break;
		case 50:
			rbt->resval.rreal = plink->dlnang;
			break;
		case 51:
			rbt->resval.rreal = plink->hdir;
			break;
		case 52:
			rbt->resval.rreal = plink->elnang;
			break;
		case 53:
			bGroup53 = true;
			rbt->resval.rreal = plink->textrot;
			break;
		case 70:
			rbt->resval.rint = plink->flags;
			break;
		case 210:
			ic_ptcpy(rbt->resval.rpoint, plink->pt210);
			break;
		}
	}
	if(!bGroup53)
	{
		// group 53 is optional so create it
		for(rbt = elist; rbt->rbnext != NULL || rbt->rbnext->restype != -3; rbt = rbt->rbnext);
		pGroup53 = sds_newrb(RTREAL);
		pGroup53->resval.rreal = plink->textrot;
		pGroup53->rbnext = rbt->rbnext;
		rbt->rbnext = pGroup53;
	}
	//if(bEEDCreated && exlist != NULL)
	//{
	//	for(rbt = elist; rbt->rbnext != NULL; rbt = rbt->rbnext);
	//	rbt->rbnext = exlist;
	//}

	//Modified Cybage SBD 03/12/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla ( DIMTEDIT case - 1 a )
	bdimUpdate=TRUE;
    //if ((ret=sds_entmake(newlist))!=RTNORM)         goto exit;
    // EBATECH(CNBR) -[ Use plinkvar
    // NOTE: Here and CMainWindow::IcadDimDia() clear all rest XDATA APPID=ACAD.
	// attach eed to the edited dimension resbuf list
	for(rbt = elist; rbt->rbnext && rbt->rbnext->restype != -3; rbt = rbt->rbnext);
	if(rbt->rbnext)
	{
		IC_RELRB(rbt->rbnext);
		rbt->rbnext = NULL;
	}
	cmd_buildEEdList(elist, TRUE);
	for(rbt = elist; rbt->rbnext && rbt->rbnext->restype != -3; rbt = rbt->rbnext);
	if( !rbt->rbnext )
	{
		rbt->rbnext = sds_buildlist( -3, 1001, "ACAD", NULL );
	}
    // EBATECH(CNBR) ]-
	//Bugzilla No. 78180 ; 04-08-2003 [
	if(ret == RTCAN)
		sds_redraw(ename,IC_REDRAW_DRAW);
	else if((ret = sds_entmod(elist)) != RTNORM)
		goto exit;
	//Bugzilla No. 78180 ; 04-08-2003 ]
	//Modified Cybage SBD 03/12/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla ( DIMTEDIT case - 1 a )
	bdimUpdate=FALSE;
    //if ((ret=sds_entlast(lastdimpts.ename))!=RTNORM) goto exit;
	//BugZilla No. 78155; 27-05-2002
	//memcpy(lastdimpts.ename, ename, sizeof(sds_name));

    SDS_AtNodeDrag=0;
	IC_RELRB(elist);
	//IC_RELRB(newlist);	
	plink->HomeText=FALSE;
	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
    return(RTNORM);
    exit:
	//Modified Cybage SBD 03/12/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla ( DIMTEDIT case - 1 a )
	bdimUpdate=FALSE;
    SDS_AtNodeDrag=0;
	IC_RELRB(elist);		
	//IC_RELRB(newlist);		
	plink->HomeText=FALSE;
	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return(RTERROR);
}


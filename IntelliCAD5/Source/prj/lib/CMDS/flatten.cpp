/* G:\ICADDEV\PRJ\LIB\CMDS\FLATTEN.CPP
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
#include "IcadApi.h"

short cmd_flatten(void)   { 

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT ret=RTNORM;
	int	fi1,fi2,ok2mod;
	long fl1=0L,badents;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	sds_name ss,ename,updent;
	double fr1;
	struct resbuf rbucs,rbecs,rbwcs,*rb2use;
	struct resbuf *rbb,*trb;
    rbb=trb=NULL;
	sds_point extru,ptemp;

	SDS_getvar(NULL,DB_QUCSXDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(ptemp,rbucs.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	extru[0]=(ptemp[1]*rbucs.resval.rpoint[2])-(rbucs.resval.rpoint[1]*ptemp[2]);
	extru[1]=(ptemp[2]*rbucs.resval.rpoint[0])-(rbucs.resval.rpoint[2]*ptemp[0]);
	extru[2]=(ptemp[0]*rbucs.resval.rpoint[1])-(rbucs.resval.rpoint[0]*ptemp[1]);

    ret=sds_pmtssget(ResourceString(IDC_FLATTEN__NSELECT_ENTITIE_0, "\nSelect entities to flatten: " ),NULL,NULL,NULL,NULL,ss,0);
	if(ret!=RTNORM) return(ret);

	if (RemoveHatchFromSS(ss) == RTNIL) {
		sds_ssfree(ss);
		return -1;
	}

	SDS_getvar(NULL,DB_QELEVATION,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_rtos(fr1=rbucs.resval.rreal,-1,-1,fs2);
	sprintf(fs1,ResourceString(IDC_FLATTEN__NNEW_UCS_ELEVAT_1, "\nNew UCS elevation <%s>: " ),fs2);
    ret=sds_getreal(fs1,&fr1);
    if(ret!=RTNORM && ret!=RTNONE) {
		while(RTNORM==sds_ssname(ss,fl1,ename)) {
			sds_redraw(ename,IC_REDRAW_UNHILITE);
			++fl1;
		}
		sds_ssfree(ss);
		return(ret);
	}
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	rbecs.restype=RT3DPOINT;
	ic_ptcpy(rbecs.resval.rpoint,extru);

    for(badents=fl1=0L;RTNORM==sds_ssname(ss,fl1,ename);fl1++) {
		nextent:
		if(rbb!=NULL){sds_relrb(rbb);rbb=NULL;}
		rbb=sds_entget(ename);
		fi1=IC_TYPE_FROM_ENAME(ename);
		if(fi1==DB_SEQEND){
			//this only gets hit once we're done with entire entity & move on to next in set
			sds_entupd(updent);
			continue;
		}
		if(fi1==DB_CIRCLE || fi1==DB_ARC || fi1==DB_SOLID || fi1==DB_ELLIPSE ||
		   fi1==DB_POLYLINE ||fi1==DB_TEXT || fi1==DB_MTEXT || fi1==DB_INSERT ||
		   fi1==DB_TRACE || fi1==DB_ATTDEF ||fi1==DB_SHAPE || fi1==DB_ATTRIB || fi1==DB_SPLINE){
			if(fi1==DB_POLYLINE){
				((db_handitem *)ename[0])->get_70(&fi2);
				if(fi2&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)){
					rb2use=&rbwcs;
					goto doit;
				}
			}else if(fi1==DB_SPLINE){
				((db_handitem *)ename[0])->get_70(&fi2);
				if(0==(fi2&8)){
					rb2use=&rbwcs;
					goto doit;
				}
			}
			if((ic_assoc(rbb,210)!=0 && !icadRealEqual(fabs(extru[2]),1.0)) ||
			   (!icadPointEqual(ic_rbassoc->resval.rpoint,extru))){
				badents++;
				sds_redraw(ename,IC_REDRAW_UNHILITE);
				if(fi1==DB_ATTRIB)goto nextent;
				else continue;
			}else{
				rb2use=&rbecs;
			}
		}else rb2use=&rbwcs;

		doit:
		ok2mod=1;
		for(trb=rbb; trb!=NULL; trb=trb->rbnext) {
			if(trb->restype==11 && (fi1==DB_ELLIPSE || fi1==DB_MTEXT))
				continue;//already in plane (210 matches)
			if(trb->restype==11 && (fi1==DB_XLINE || fi1==DB_RAY)){	//flatten vector
				sds_trans(trb->resval.rpoint,rb2use,&rbucs,1,ptemp);
				ptemp[2]=sqrt(ptemp[0]*ptemp[0]+ptemp[1]*ptemp[1]);//magnitude of XY components, we'll zero Z
				if(fabs(ptemp[2])<CMD_FUZZ){
					badents++;
					ok2mod=0;//ray perp to current XY plane
					break;
				}
				ptemp[0]/=ptemp[2];
				ptemp[1]/=ptemp[2];
				ptemp[2]=0.0;
				sds_trans(ptemp,&rbucs,rb2use,1,trb->resval.rpoint);
			}else if(trb->restype>=10 && trb->restype<=19){
				sds_trans(trb->resval.rpoint,rb2use,&rbucs,0,ptemp);
				ptemp[2]=fr1;
				sds_trans(ptemp,&rbucs,rb2use,0,trb->resval.rpoint);
			}
			if(trb->restype==39) trb->resval.rreal=0.0;
		}
		if(ok2mod)sds_entmod(rbb);

		if(fi1==DB_VERTEX || fi1==DB_POLYLINE || fi1==DB_ATTRIB){
			if(fi1==DB_POLYLINE)ic_encpy(updent,ename);
			sds_entnext_noxref(ename,ename);
			goto nextent;
		}
		if(fi1==DB_INSERT){
			ic_assoc(rbb,66);
			if(ic_rbassoc->resval.rint==1){
				ic_encpy(updent,ename);
				sds_entnext_noxref(ename,ename);
				goto nextent;
			}
		}
	}

	if(rbb!=NULL){sds_relrb(rbb);rbb=NULL;}
	if(fl1-badents>0L) cmd_ErrorPrompt(CMD_ERR_FLATTENED,0,(void *)(fl1-badents));
	if(badents>0L) cmd_ErrorPrompt(CMD_ERR_NOTPARALLEL,0,(void *)(badents));
	sds_ssfree(ss);
	return(RTNORM); 
} 




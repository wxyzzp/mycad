/* F:\DEV\PRJ\LIB\CMDS\STRETCH.CPP
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
//Bugzilla No. 60619 ; 17-10-2002
#include "dimensions.h"

// Globals

extern struct cmd_drag_globalpacket cmd_drag_gpak;
extern int SDS_PointDisplayMode;
extern sds_point SDS_PointDisplay;
extern int SDS_AtNodeDrag;

short    cmd_DimStyleFunc(short mode,db_drawing *flp);


short cmd_stretch(void){
	/*NOTES: This command works just like Acad's, which isn't good.  We call
	pmtssget, which allows user to select by several methods.  We only support
	Crossing and Crossing Polygon.  If they select by another method,
	then we have to quit.  The selection options need to be narrowed & corrected.
	Also, implementation of the CCIRCLE should be a snap - no use of ptsll or
	conversion of pmtssget's ptsllist should be req'd.
	*/

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	//we'll keep an array of points llists - one element for each set of picking pts user
	//	defines in a stretch bdry
	struct ptsll_array{
		sds_real xmin;
		sds_real xmax;
		sds_real ymin;
		sds_real ymax;
		struct ic_pointsll *listbeg;
		struct ptsll_array *next;
	}*ptarray_beg,*ptarray_cur,*ptarray_temp;

    sds_point p0,p1,p2,p3,p4,p5,p6,p7,ptemp;
    sds_name ss1,ss2,ss3,etemp1,ssdel;
	RT	ret=0;
	int fi1=0,fi2;
	long fl1;
	struct resbuf *elist, *rbpts, *rbtemp, *rbloop,rb,rbucs,rbent,rbdcs;
    sds_matrix mtx;
    sds_real fr1,fr2,fr3,fr4;

	ic_pointsll *ptstmp,*ptscur;
    struct cmd_drag_globalpacket *gpak;

	rbucs.restype=rbdcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbdcs.resval.rint=2;

    gpak=&cmd_drag_gpak;
    gpak->reference=0; gpak->refval=0.0; gpak->rband=1;
	ss3[0]=ss3[1]=0;

	ptarray_beg=ptarray_cur=NULL;
	ptstmp=ptscur=NULL;
	rbpts=elist=rbtemp=NULL;

	do{
		if(rbpts!=NULL){
			sds_relrb(rbpts);rbpts=NULL;
			if(RTNORM==sds_ssname(ss1,0L,etemp1))sds_redraw(etemp1,IC_REDRAW_UNHILITE);
            sds_ssfree(ss1);
            ss1[0]=0L;
		}

        ret=sds_pmtssget(ResourceString(IDC_STRETCH__NSELECT_ENTITIE_0, "\nSelect entities to stretch by crossing-window or crossing-polygon: " ),
			ResourceString(IDC_STRETCH__CROSSING_WINDOW_1, "*Crossing_window|Crossing Crossing_polygon|CPolygon ~ Remove Add ~_Crossing ~_CPolygon ~_ ~_Remove ~_Add" ),&rbpts,NULL,NULL,ss1,0);

		if((ret!=RTNORM) || (RemoveHatchFromSS(ss1) == RTNIL))
			{IC_RELRB(rbpts); sds_ssfree(ss1); return(ret);}
		if(rbpts==NULL || rbpts->rbnext==NULL || (rbpts->restype==RTSTR && rbpts->resval.rstring[0]=='S'/*DNT*/)){
			//if they pre-gripped anything or selected, we have to re-select so we can
			//	capture points which define bdry...
			for(fl1=0L;RTNORM==sds_ssname(ss1,fl1,etemp1);sds_redraw(etemp1,IC_REDRAW_UNHILITE),fl1++);
            sds_ssfree(ss1);
            ss1[0]=0L;
		}
	}while(NULL==rbpts || (rbpts->restype==RTSTR && rbpts->resval.rstring[0]=='S'/*DNT*/));
	//if user did a crossing or box, convert it to polygon format
	SDS_sscpy(ss2,ss1);
	SDS_sscpy(ssdel,ss1);
	rbloop=rbpts;
	while(rbloop!=NULL && rbloop->restype==RTSTR){
        IC_RELRB(rbtemp);
		//NOTE: walking of pointer done inside loop
        if(strisame(rbloop->resval.rstring, "CROSSING"/*DNT*/) || strisame(rbloop->resval.rstring, "BOX"/*DNT*/)) {
			//this is CROSSING or BOX which contained at least one ent
			if(rbloop->rbnext==NULL || rbloop->rbnext->rbnext==NULL){ret=RTERROR;goto exit;}
			//allocate a pointer to the next llist of pts...
			if((ptarray_temp= new struct ptsll_array )==NULL){
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=RTERROR;
				goto exit;
			}
			memset(ptarray_temp,0,sizeof(struct ptsll_array));
			if(ptarray_beg==NULL){
				ptarray_beg=ptarray_cur=ptarray_temp;
			}else{
				ptarray_cur=ptarray_cur->next=ptarray_temp;
			}
            //make 2 pts which contain the "opposite" corners of the boundary rectangle in dcs
            sds_trans(rbloop->rbnext->resval.rpoint,&rbucs,&rbdcs,0,p1);
            sds_trans(rbloop->rbnext->rbnext->resval.rpoint,&rbucs,&rbdcs,0,p3);
            //construct the other 2 corners
            ic_ptcpy(p2,p1);
            p2[1]=p3[1];
            ic_ptcpy(p4,p3);
            p4[1]=p1[1];

			for(fi1=0;fi1<4;fi1++){
				if((ptstmp= new ic_pointsll )==NULL){
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=RTERROR;
					goto exit;
				}
				ptstmp->next=NULL;
				if(ptarray_cur->listbeg==NULL){
					ptarray_cur->listbeg=ptscur=ptstmp;
				}else{
					ptscur->next=ptstmp;
					ptscur=ptscur->next;
				}
				switch(fi1){
					case 0: ic_ptcpy(ptscur->pt,p1);break;
					case 1: ic_ptcpy(ptscur->pt,p2);break;
					case 2: ic_ptcpy(ptscur->pt,p3);break;
					case 3: ic_ptcpy(ptscur->pt,p4);break;
				}
			}
            //setup bounding rectangle
            ptarray_cur->xmin=min(p3[0],p1[0]);
            ptarray_cur->xmax=max(p3[0],p1[0]);
            ptarray_cur->ymin=min(p3[1],p1[1]);
            ptarray_cur->ymax=max(p3[1],p1[1]);

			//conver some temp point back into the ucs for the fence filter
            sds_trans(p1,&rbdcs,&rbucs,0,p1);
            sds_trans(p2,&rbdcs,&rbucs,0,p2);
            sds_trans(p3,&rbdcs,&rbucs,0,p3);
            sds_trans(p4,&rbdcs,&rbucs,0,p4);

            //redo the llist of points & put 'em in rbtemp to be used by ssget
			if((rbtemp=sds_buildlist(RT3DPOINT,p1,RT3DPOINT,p2,RT3DPOINT,p3,RT3DPOINT,p4,RT3DPOINT,p1,0))==NULL){
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=RTERROR;
				goto exit;
			}

			fi2=4;
			rbloop=rbloop->rbnext->rbnext->rbnext;
		}else if(strisame(rbloop->resval.rstring,"CPOLYGON"/*DNT*/)){
			//allocate a pointer to the next llist of pts...
			if((ptarray_temp= new struct ptsll_array )==NULL){
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=RTERROR;
				goto exit;
			}
			memset(ptarray_temp,0,sizeof(struct ptsll_array));
			if(ptarray_beg==NULL){
				ptarray_beg=ptarray_cur=ptarray_temp;
			}else{
				ptarray_cur=ptarray_cur->next=ptarray_temp;
			}
			rbtemp=elist=NULL;
			for(fi2=0,rbloop=rbloop->rbnext;rbloop!=NULL && (rbloop->restype==RTPOINT || rbloop->restype==RT3DPOINT);rbloop=rbloop->rbnext,fi2++){
				if((ptstmp= new ic_pointsll )==NULL){
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=RTERROR;
					goto exit;
				}
				if(ptarray_cur->listbeg==NULL){
					ptarray_cur->listbeg=ptscur=ptstmp;
				}else{
					ptscur->next=ptstmp;
					ptscur=ptscur->next;
				}
                sds_trans(rbloop->resval.rpoint,&rbdcs,&rbucs,0,p6);
			    //create parallel list of pts for use in ent selection w/fence.  These need to be in UCS, like we
                //  got from user.  We'll make 2nd llist structure below for use w/ic_inorout
				if(rbtemp){
					if(NULL==(elist=elist->rbnext=sds_buildlist(RT3DPOINT,p6,0))){
						elist=NULL;
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						ret=RTERROR;
						goto exit;
					}
				}else{
					if(NULL==(rbtemp=elist=sds_buildlist(RT3DPOINT,p6,0))){
						elist=NULL;
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						ret=RTERROR;
						goto exit;
					}
				}
				//need copy of everything in screen coords for ic_inorout()
                ic_ptcpy(ptscur->pt,rbloop->resval.rpoint);
                //sds_trans(rbloop->resval.rpoint,&rbucs,&rbdcs,0,ptscur->pt) - pts in rbloop already in dcs!!!
                //ic_ptcpy(ptscur->pt,rbloop->resval.rpoint);
				if(ptarray_cur->listbeg==ptscur){
					ptarray_cur->xmin=ptarray_cur->xmax=ptarray_cur->listbeg->pt[0];
					ptarray_cur->ymin=ptarray_cur->ymax=ptarray_cur->listbeg->pt[1];
				}else{
					if(ptscur->pt[0]<ptarray_cur->xmin)ptarray_cur->xmin=ptscur->pt[0];
					if(ptscur->pt[0]>ptarray_cur->xmax)ptarray_cur->xmax=ptscur->pt[0];
					if(ptscur->pt[1]<ptarray_cur->ymin)ptarray_cur->ymin=ptscur->pt[1];
					if(ptscur->pt[1]>ptarray_cur->ymax)ptarray_cur->ymax=ptscur->pt[1];
				}
			}
			ptstmp->next=NULL;
			//add one link to close the fence
			if(elist!=NULL)elist->rbnext=sds_buildlist(RT3DPOINT,ptarray_cur->listbeg->pt,0);
			elist=NULL;
		}else{
			for(rbloop=rbloop->rbnext;rbloop!=NULL && rbloop->restype!=RTSTR;rbloop=rbloop->rbnext);
			continue;
		}


		if(ss3[0]!=0 && ss3[1]!=0)sds_ssfree(ss3);
		//NOTE: use pmtssget so that the prev sset isn't disturbed
		if(sds_pmtssget(NULL,"FENCE"/*DNT*/,rbtemp,NULL,NULL,ss3,1)!=RTNORM){
			ss3[0]=ss3[1]=0;
		}else{
			//remove all fence crossing items from selection set chosen by user.
			for(fl1=0;sds_ssname(ss3,fl1,etemp1)==RTNORM;sds_ssdel(etemp1,ss1),fl1++);
			sds_ssfree(ss3);ss3[0]=ss3[1]=0;
		}
		//NOTE: this should work ok for xlines & rays, as they'll always be in fence set and we'll
		//	check later to assure that their 10pts are inside the fence
	}//while
	//free the pts ll now (it's a memory hog)
	if(rbpts!=NULL){sds_relrb(rbpts);rbpts=NULL;}
	if(rbtemp!=NULL){sds_relrb(rbtemp);rbtemp=NULL;}
	if(NULL==ptarray_beg){
		sds_printf(ResourceString(IDC_STRETCH__NENTITIES_MUST__4, "\nEntities must be selected by crossing-window or crossing polygon." ));
		ret=RTERROR;
		goto exit;
	}
	//Bugzilla No. 60619 ; 17-10-2002 [
	//Some entities might be crossing but fence might not return them in ss3 and hence
	//such entities will not be deleted from ss1. We must delete such entities from 
	//ss1 otherwise these entities will be considered as completely selected in the 
	//crossing polygon and will be moved instead of being stretched. 
	//Eg. dimagular dimension drawn in 'V' shaped line is being stretched.	
	for(fl1=0;RTNORM==sds_ssname(ss1,fl1,etemp1);fl1++)
	{
		db_handitem * handent = (db_handitem * )etemp1[0];
		
		if ( handent == NULL )			
			continue;

		switch( handent->ret_type() )
		{
		//Bugzilla No. 77972; 29-07-2002 [
		case DB_LINE :
			{
				for( fi1= 0; fi1 < 2 ;fi1++ )
				{
					switch(fi1)
					{
					case 0 : handent->get_10(p0); break;
					case 1 : handent->get_11(p0); break;						
					}
					if(0==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p0[0],p0[1]))
					{
						sds_ssdel(etemp1,ss1);
						fl1--;
						break;
					}
				}
			}
			break;
		//Bugzilla No. 77972; 29-07-2002 ]
		case DB_DIMENSION :
			{
				
		handent->get_70(&fi1);
		fi1 &= ~DIM_TEXT_MOVED;
		fi1 &= ~DIM_BIT_6;
		
		if ( fi1 != DIM_ANGULAR_3P &&  fi1 != DIM_ANGULAR )
					break;		
		
		for( fi1= 0; fi1 < 4 ;fi1++ )
		{
			switch(fi1)
			{
			case 0 : handent->get_10(p0); break;
			case 1 : handent->get_13(p0); break;
			case 2 : handent->get_15(p0); break;
			case 3 : handent->get_14(p0); break;												
			}
			if(0==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p0[0],p0[1]))
			{
				sds_ssdel(etemp1,ss1);
				fl1--;
				break;
			}
		}		
	}	
			break;
		default :
			break;
			
		}
	}	
	//Bugzilla No. 60619 ; 17-10-2002 ]

	//At this point, ss2 has everything user picked, and ss1 has just the inside items.
	//Remove inside items from ss2 to create set of just crossing items
	for(fl1=0;RTNORM==sds_ssname(ss1,fl1,etemp1);sds_ssdel(etemp1,ss2),fl1++);

    if(sds_getpoint(NULL,ResourceString(IDC_STRETCH__NBASE_POINT_OR__5, "\nBase point or displacement: " ),p0)!=RTNORM){ret=-1;goto exit;}
    sds_initget(0,NULL);
 	sds_sslength(ss1,&fl1);
	SDS_PointDisplayMode=5;
	ic_ptcpy(SDS_PointDisplay,p0);
	if(fl1>0L){
		ic_ptcpy(gpak->pt1,p0);
		fi2=cmd_drag(CMD_TRANSLATE_DRAG,ss1,ResourceString(IDC_STRETCH__NSECOND_POINT_O_6, "\nSecond point of displacement: " ),NULL,p1,NULL);
	}else{
        fi2=sds_getpoint(p0,ResourceString(IDC_STRETCH__NSECOND_POINT_O_6, "\nSecond point of displacement: " ),p1);
	}
	SDS_PointDisplayMode=0;
	if(fi2==RTNONE){
		ic_ptcpy(p1,p0);
		p0[0]=p0[1]=p0[2]=0.0;
		ret=RTNORM;
	}else if(fi2!=RTNORM){
		ret=-1;
		goto exit;
	}
	if(fl1>0L){
		//move the entities totally contained within the boundary
		for(fi1=0;fi1<=3;fi1++)
			for(fi2=0;fi2<=2;fi2++)
				mtx[fi1][fi2]=0.0;
		for(fi1=0;fi1<=3;fi1++)
			mtx[fi1][fi1]=1.0;
		mtx[0][3]=p1[0]-p0[0];
		mtx[1][3]=p1[1]-p0[1];
		mtx[2][3]=p1[2]-p0[2];
		if(SDS_progxformss(ss1,mtx,1)!=RTNORM){ret=-1;goto exit;}
	}

	//Now transform all the entities crossing the selection (ss2)
	rb.restype=RTSTR;
	rb.rbnext=NULL;
	rb.resval.rstring=const_cast<char*>("*"/*DNT*/);
	if(ss2[0]==0 && ss2[1]==0) goto exit;
    for(fl1=0L;sds_ssname(ss2,fl1,etemp1)==RTNORM;fl1++){
        if(elist!=NULL){sds_relrb(elist);elist=NULL;}
		//NOTE:for each ent, set ptemp to be offset vector from p0 to p1.  In each case,
		//	we'll transform it into the proper coordinates.
		//NOTE: for all inside/outside xforms, pts must be in ucs
		ptemp[0]=p1[0]-p0[0];
		ptemp[1]=p1[1]-p0[1];
		ptemp[2]=p1[2]-p0[2];
        if((elist=sds_entgetx(etemp1,&rb))==NULL){ret=-1;goto exit;}
		int enttype=IC_TYPE_FROM_ENAME(etemp1);
		//before we entmod these entities, we need to blank the old ones out
		//completely
		ic_assoc(elist,0);
		/*D.Gavrilov*/// 3D Face stretching added.
		if (enttype==DB_LINE || enttype==DB_TRACE || enttype==DB_SOLID || enttype == DB_3DFACE) {
			if (enttype==DB_LINE || enttype == DB_3DFACE) {
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
			}else{
				rbent.restype=RT3DPOINT;
				rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
				rbent.resval.rpoint[2]=1.0;
			}
			fi1=0;
			for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
				if(rbtemp->restype==10){fi1+=1;ic_ptcpy(p2,rbtemp->resval.rpoint);}
				if(rbtemp->restype==11){fi1+=2;ic_ptcpy(p3,rbtemp->resval.rpoint);}
				if(rbtemp->restype==12){fi1+=4;ic_ptcpy(p4,rbtemp->resval.rpoint);}
				if(rbtemp->restype==13){fi1+=8;ic_ptcpy(p5,rbtemp->resval.rpoint);}
				if(rbtemp->restype==210 &&
				   !strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)) &&
				   !strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_3DFACE)))
					ic_ptcpy(rbent.resval.rpoint,rbtemp->resval.rpoint);
			}
			sds_trans(ptemp,&rbucs,&rbent,1,ptemp);
			//ignore elev difference if moving a planar object
			if(rbent.restype==RT3DPOINT)ptemp[2]=0.0;
			for(ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next){
				if(1==(fi1 & (1+16))){
					//inorout requires UCS points
					sds_trans(p2,&rbent,&rbdcs,0,p6);
					if((fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p6[0],p6[1]))==1){
						p2[0]+=ptemp[0];
						p2[1]+=ptemp[1];
						p2[2]+=ptemp[2];
						fi1+=16;
					}
				}
				if(2==(fi1 & (2+32))){
					sds_trans(p3,&rbent,&rbdcs,0,p6);
					if((fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p6[0],p6[1]))==1){
						p3[0]+=ptemp[0];
						p3[1]+=ptemp[1];
						p3[2]+=ptemp[2];
						fi1+=32;
					}
				}
				if(4==(fi1 & (4+64))){
					sds_trans(p4,&rbent,&rbdcs,0,p6);
					if((fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p6[0],p6[1]))==1){
						p4[0]+=ptemp[0];
						p4[1]+=ptemp[1];
						p4[2]+=ptemp[2];
						fi1+=64;
					}
				}
				if(8==(fi1 & (8+128))){
					sds_trans(p5,&rbent,&rbdcs,0,p6);
					if((fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p6[0],p6[1]))==1){
						p5[0]+=ptemp[0];
						p5[1]+=ptemp[1];
						p5[2]+=ptemp[2];
						fi1+=128;
					}
				}
				if(fi1&(16+32+64+128)){
					for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
						if((fi1 & 16) && rbtemp->restype==10){ic_ptcpy(rbtemp->resval.rpoint,p2);fi1-=1;}
						if((fi1 & 32) && rbtemp->restype==11){ic_ptcpy(rbtemp->resval.rpoint,p3);fi1-=2;}
						if((fi1 & 64) && rbtemp->restype==12){ic_ptcpy(rbtemp->resval.rpoint,p4);fi1-=4;}
						if((fi1 & 128) && rbtemp->restype==13){ic_ptcpy(rbtemp->resval.rpoint,p5);fi1-=8;}
					}
                    if(sds_entmod(elist)!=RTNORM){ret=(-__LINE__);goto exit;}
				}
			}
		}else if (enttype==DB_LEADER) {
			//Added Cybage VSB 22/10/2001 DD/MM/YYYY
			//Reason : Bug No 77882. Dimleader entity crash bug on stretch command
			ptarray_temp = ptarray_beg;
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			sds_trans(ptemp,&rbucs,&rbent,1,ptemp);
			for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
				if(rbtemp->restype==10){
					sds_trans(rbtemp->resval.rpoint,&rbent,&rbdcs,0,p6);
					if((fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p6[0],p6[1]))==1){
						rbtemp->resval.rpoint[0]+=ptemp[0];
						rbtemp->resval.rpoint[1]+=ptemp[1];
						rbtemp->resval.rpoint[2]+=ptemp[2];
					}
				}
			}
            if(sds_entmod(elist)!=RTNORM){ret=(-__LINE__);goto exit;}
		}else if (enttype==DB_VIEWPORT) {
			//	STRETCH A VIEWPORT
			struct resbuf *viewptrx,*viewptry,*viewptrsiz;
			//get entity coords
            for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
			    if(rbtemp->restype==10){ic_ptcpy(p6,rbtemp->resval.rpoint);}
			    if(rbtemp->restype==40){fr1=rbtemp->resval.rreal;}
			    if(rbtemp->restype==41){fr2=rbtemp->resval.rreal;}
			}
            //generate 4 end points
            p2[0]=p6[0]-fr1/2.0;p2[1]=p6[1]+fr2/2.0;p2[2]=p6[2];
            p3[0]=p2[0]+fr1;p3[1]=p2[1];p3[2]=p6[2];
            p4[0]=p3[0];p4[1]=p3[1]-fr2;p4[2]=p6[2];
            p5[0]=p2[0];p5[1]=p4[1];p5[2]=p6[2];

            fi1=0;
            for(ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next){
				if(ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p2[0],p2[1])==1){
					p5[0]=p2[0]+=ptemp[0];
					p3[1]=p2[1]+=ptemp[1];
				    fi1=1;
                }
				if(fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p3[0],p3[1])==1){
					p4[0]=p3[0]+=ptemp[0];
					p2[1]=p3[1]+=ptemp[1];
				    fi1=1;
                }
				if(fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p4[0],p4[1])==1){
					p3[0]=p4[0]+=ptemp[0];
					p5[1]=p4[1]+=ptemp[1];
				    fi1=1;
                }
				if(fi2=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p5[0],p5[1])==1){
					p2[0]=p5[0]+=ptemp[0];
					p4[1]=p5[1]+=ptemp[1];
				    fi1=1;
                }
				//generate the width,height,center point
                p7[0]=(p2[0]+p3[0])/2;
                p7[1]=(p2[1]+p5[1])/2;
				p7[2]=p6[2];
				fr3=fr2;//save original height
                fr1=fabs(p3[0]-p2[0]);
                fr2=fabs(p2[1]-p5[1]);

				if(fr1){
					//update entity
                    for (rbtemp=elist; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
						{
					    if(rbtemp->restype==10)
							ic_ptcpy(rbtemp->resval.rpoint,p7);
					    else if (rbtemp->restype==40)
							rbtemp->resval.rreal=fr1;
					    else if (rbtemp->restype==41)
							rbtemp->resval.rreal=fr2;
						}
					//now update viewport size, position
					viewptrsiz=ic_ret_assoc(elist,45);
					if (NULL==viewptrsiz) {ret=-1;goto exit;}
					fr4=viewptrsiz->resval.rreal/fr3;//unit ratio (fr3 is original height)
					viewptrx=ic_ret_assoc(elist,12);
					if (NULL==viewptrx) {ret=-1;goto exit;}
					viewptry=viewptrx->rbnext;
					if (NULL==viewptry) {ret=-1;goto exit;}
					viewptrsiz->resval.rreal*=fr2/fr3;
					viewptrx->resval.rreal+=fr4*(p7[0]-p6[0]);
					viewptry->resval.rreal+=fr4*(p7[1]-p6[1]);
                    if (sds_entmod(elist)!=RTNORM)
						{
						ret=(-__LINE__);
						goto exit;
						}
			    }
            }
        }else if (enttype==DB_TEXT || enttype==DB_ATTDEF || enttype==DB_ATTRIB) {
			rbent.restype=RT3DPOINT;
			if(ic_assoc(elist,210)){
				rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
				rbent.resval.rpoint[2]=1.0;
			}else
				ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
			    sds_trans(ptemp,&rbucs,&rbent,1,ptemp);
			//ptemp[2]=0.0;
			for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
				if(rbtemp->restype==10 || 11==rbtemp->restype){
					rbtemp->resval.rpoint[0]+=ptemp[0];
					rbtemp->resval.rpoint[1]+=ptemp[1];
					//rbtemp->resval.rpoint[2]+=ptemp[2];
				}
			}
            if(sds_entmod(elist)!=RTNORM){ret=(-__LINE__);goto exit;}
        }else if (enttype==DB_DIMENSION) {
			struct resbuf *rbp[6];
			for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
				if(rbtemp->restype==10)rbp[0]=rbtemp;
				else if(rbtemp->restype==11)rbp[1]=rbtemp;
				else if(rbtemp->restype==13)rbp[2]=rbtemp;
				else if(rbtemp->restype==14)rbp[3]=rbtemp;
				else if(rbtemp->restype==15)rbp[4]=rbtemp;
				else if(rbtemp->restype==16)rbp[5]=rbtemp;
				else if(rbtemp->restype==70)fi1=rbtemp->resval.rint;
				else if(rbtemp->restype==210)ic_ptcpy(rbent.resval.rpoint,rbtemp->resval.rpoint);
			}
			rbent.restype=RT3DPOINT;
            fi1&=~(8+16+32+64+128+256);//remove all high bits so fi1 is 0 thru 6
			rb.restype=RTSHORT;
			rb.resval.rint=0;//set up rb as wcs resbuf
			//ptemp is offset vector in UCS, and all inorout calls use ucs.
			//transform all wcs pts into ucs & test for inorout.  if in, move them and transform
			//back into wcs
			//do the wcs pts first
			sds_trans(rbp[0]->resval.rpoint,&rb,&rbucs,0,p2);
			sds_trans(rbp[2]->resval.rpoint,&rb,&rbucs,0,p4);
			sds_trans(rbp[3]->resval.rpoint,&rb,&rbucs,0,p5);
			sds_trans(rbp[4]->resval.rpoint,&rb,&rbucs,0,p6);
			for(ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next){
				sds_trans(p2,&rbucs,&rbdcs,0,p7);
                if(1==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p7[0],p7[1])){
					p2[0]+=ptemp[0];
					p2[1]+=ptemp[1];
					p2[2]+=ptemp[2];
				}
				sds_trans(p4,&rbucs,&rbdcs,0,p7);
				if(1==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p7[0],p7[1])){
					p4[0]+=ptemp[0];
					p4[1]+=ptemp[1];
					p4[2]+=ptemp[2];
				}
				sds_trans(p5,&rbucs,&rbdcs,0,p7);
				if(1==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p7[0],p7[1])){
					p5[0]+=ptemp[0];
					p5[1]+=ptemp[1];
					p5[2]+=ptemp[2];
				}
				sds_trans(p6,&rbucs,&rbdcs,0,p7);
				if(1==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p7[0],p7[1])){
					p6[0]+=ptemp[0];
					p6[1]+=ptemp[1];
					p6[2]+=ptemp[2];
				}
			}
			sds_trans(p2,&rbucs,&rb,0,rbp[0]->resval.rpoint);
			sds_trans(p4,&rbucs,&rb,0,rbp[2]->resval.rpoint);
			sds_trans(p5,&rbucs,&rb,0,rbp[3]->resval.rpoint);
			sds_trans(p6,&rbucs,&rb,0,rbp[4]->resval.rpoint);
			//now do the ecs points
			sds_trans(rbp[1]->resval.rpoint,&rbent,&rbucs,0,p3);
			sds_trans(rbp[5]->resval.rpoint,&rbent,&rbucs,0,p7);
			for(ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next){
				sds_trans(p3,&rbucs,&rbdcs,0,p4);
				if(1==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p4[0],p4[1])){
					p3[0]+=ptemp[0];
					p3[1]+=ptemp[1];
					p3[2]+=ptemp[2];
				}
				sds_trans(p7,&rbucs,&rbdcs,0,p4);
				if(1==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p4[0],p4[1])){
					p7[0]+=ptemp[0];
					p7[1]+=ptemp[1];
					p7[2]+=ptemp[2];
				}
			}
			sds_trans(p3,&rbucs,&rbent,0,rbp[1]->resval.rpoint);
			sds_trans(p7,&rbucs,&rbent,0,rbp[5]->resval.rpoint);
			//we can't entmod the exist dim.  we need to make a new one
			//first, take out the 2 group
			for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
				if(rbtemp->rbnext!=NULL && rbtemp->rbnext->restype==2){
					ic_rbassoc=rbtemp->rbnext;
					rbtemp->rbnext=ic_rbassoc->rbnext;
					ic_rbassoc->rbnext=NULL;
					sds_relrb(ic_rbassoc);
					ic_rbassoc=rbtemp=NULL;
					break;
				}
			}
			// Bugzilla No: 77874 && 77876 ; 25-03-2002 [
			
			// Donot delete the previous entity, just undraw it
			// sds_entdel(etemp1);			
			sds_redraw(etemp1, IC_REDRAW_UNDRAW);									
			// Use entmod instead of entmake. SDS_AtNodeDrag = 1 is within entmod function now
			if(sds_entmod(elist)!=RTNORM){sds_redraw(etemp1, IC_REDRAW_DRAW );ret=(-__LINE__);goto exit;}			
			
			// Bugzilla No: 77874 && 77876 ; 25-03-2002 ]

		}else if (enttype==DB_RAY || enttype==DB_XLINE) {
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			sds_trans(ptemp,&rbucs,&rbent,1,ptemp);
			ic_assoc(elist,10);
			//inorout requires UCS pts
			sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbdcs,0,p3);
			for(ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next){
				if(1==ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p3[0],p3[1])){
					ic_rbassoc->resval.rpoint[0]+=ptemp[0];
					ic_rbassoc->resval.rpoint[1]+=ptemp[1];
					ic_rbassoc->resval.rpoint[2]+=ptemp[2];
                    if(sds_entmod(elist)!=RTNORM){ret=(-__LINE__);goto exit;}
				}
			}
		}else if (enttype==DB_ARC) {
			//	STRETCH AN ARC....
			rbent.restype=RT3DPOINT;
			rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
			rbent.resval.rpoint[2]=1.0;
			for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
				if(rbtemp->restype==10)ic_ptcpy(p4,rbtemp->resval.rpoint);
				else if(rbtemp->restype==40)fr1=rbtemp->resval.rreal;
				else if(rbtemp->restype==50)fr2=rbtemp->resval.rreal;
				else if(rbtemp->restype==51)fr3=rbtemp->resval.rreal;
				else if(rbtemp->restype==210)ic_ptcpy(rbent.resval.rpoint,rbtemp->resval.rpoint);
			}
			sds_trans(ptemp,&rbucs,&rbent,1,ptemp);
			ptemp[2]=0.0;
            sds_polar(p4,fr2,fr1,p2);
            sds_polar(p4,fr3,fr1,p3);
			//we need to keep constant the distance from the chord midpoint to the arc midpt
			ic_normang(&fr2,&fr3);
			//calc using cosine - faster than by distances since we have angles dist=R(1-cos(angle/2)
			fr1=fr1*(1.0-cos(0.5*(fr3-fr2)));	//THIS IS THE VALUE WE NEED TO KEEP
			fi2=0;
			sds_trans(p2,&rbent,&rbdcs,1,p4);	//for in/out test, we need ucs points
			sds_trans(p3,&rbent,&rbdcs,1,p5);
			for(ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next){
				if(0==(fi2&1) && (fi1=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p4[0],p4[1]))==1){
					//if start point was inside box...
					p2[0]+=ptemp[0];
					p2[1]+=ptemp[1];
					//p2[2]+=ptemp[2];
					fi2|=1;
				}else if(0==(fi2&2) && (fi1=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,p5[0],p5[1]))==1){
					//if start pt wasn't inside, then end pt must've been.
					p3[0]+=ptemp[0];
					p3[1]+=ptemp[1];
					//p3[2]+=ptemp[2];
					fi1|=2;
				}
			}
			//now, we've got revised points for arc & know what arc/chord dist is..
			p4[0]=(p2[0]+p3[0])/2.0;
			p4[1]=(p2[1]+p3[1])/2.0;
            fr2=sds_distance(p2,p3);
			fr3=((fr2*fr2)-(4.0*fr1*fr1))/(8.0*fr1);	//distance from midpt of chord to center
            sds_polar(p4,(sds_angle(p2,p3)+(IC_PI/2.0)),fr3,p5);  //center of new arc
			for(rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
				if(rbtemp->restype==10)ic_ptcpy(rbtemp->resval.rpoint,p5);
                else if(rbtemp->restype==40)rbtemp->resval.rreal=sds_distance(p5,p2);
                else if(rbtemp->restype==50)rbtemp->resval.rreal=sds_angle(p5,p2);
                else if(rbtemp->restype==51)rbtemp->resval.rreal=sds_angle(p5,p3);
			}
            if(sds_entmod(elist)!=RTNORM){ret=-1;goto exit;}
		} else if (enttype==DB_POLYLINE) {
			ic_assoc(elist,70);
			if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)){
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
			}else{
				rbent.restype=RT3DPOINT;
				if(ic_assoc(elist,210)){
					rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
					rbent.resval.rpoint[2]=1.0;
				}else{
					ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
				}
			}
			sds_trans(ptemp,&rbucs,&rbent,1,ptemp);
			if(rbent.restype==RT3DPOINT)ptemp[2]=0.0;
			if((ret=sds_entnext_noxref(etemp1,etemp1))!=RTNORM)goto exit;
			if(rbpts!=NULL){sds_relrb(rbpts);rbpts=NULL;}
			if((rbpts=sds_entget(etemp1))==NULL){ret=RTERROR;goto exit;}
			ic_assoc(rbpts,0);
			fi2=0;
			while(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/)){
				ic_assoc(rbpts,10);
				ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
				sds_trans(p2,&rbent,&rbdcs,0,p6);	//for in/out calc, we need a ucs point
				for(ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next){
					if((fi1=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,
										p6[0],p6[1]))==1){
						ic_rbassoc->resval.rpoint[0]+=ptemp[0];
						ic_rbassoc->resval.rpoint[1]+=ptemp[1];
						ic_rbassoc->resval.rpoint[2]+=ptemp[2];
						if((ret=sds_entmod(rbpts))!=RTNORM)goto exit;
						fi2=1;
						break;
					}
				}
				if((ret=sds_entnext_noxref(etemp1,etemp1))!=RTNORM)goto exit;
				if(rbpts!=NULL){sds_relrb(rbpts);rbpts=NULL;}
				if((rbpts=sds_entget(etemp1))==NULL){ret=RTERROR;goto exit;}
				ic_assoc(rbpts,0);
			}
			if(fi2)sds_entupd(etemp1);
		} else if (enttype==DB_LWPOLYLINE) 
			{
			int nverts;
			resbuf *nvertsrb=ic_ret_assoc(elist,90);
			nverts=nvertsrb->resval.rint;
			ic_assoc(elist,70);
			rbent.restype=RT3DPOINT;
			if(ic_assoc(elist,210))
				{
				rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
				rbent.resval.rpoint[2]=1.0;
				}
			else
				ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
			sds_trans(ptemp,&rbucs,&rbent,1,ptemp);
			if (rbent.restype==RT3DPOINT)
				ptemp[2]=0.0;
			resbuf *rbpts=ic_ret_assoc(elist,10);
			fi2=0;
			for (int i=0; i<nverts; i++) 
				{
				ic_ptcpy(p2,rbpts->resval.rpoint);
				sds_trans(p2,&rbent,&rbdcs,0,p6);	//for in/out calc, we need a ucs point
				for (ptarray_temp=ptarray_beg;ptarray_temp!=NULL;ptarray_temp=ptarray_temp->next)
					{
					if((fi1=ic_inorout(ptarray_temp->listbeg,ptarray_temp->xmin,ptarray_temp->ymin,ptarray_temp->xmax,ptarray_temp->ymax,
										p6[0],p6[1]))==1)
						{
						rbpts->resval.rpoint[0]+=ptemp[0];
						rbpts->resval.rpoint[1]+=ptemp[1];
						rbpts->resval.rpoint[2]+=ptemp[2];
						fi2=1;
						break;
						}
					}
				do
					rbpts=rbpts->rbnext;
				while (rbpts && rbpts->restype!=10);
				}
			if ((ret=sds_entmod(elist))!=RTNORM)
				goto exit;
			if (fi2)
				sds_entupd(etemp1);
			}

		db_handitem *hip = (db_handitem*) etemp1[0];
		if (hip->ret_type() == DB_SEQEND)
			((db_seqend*)hip)->get_mainent(&hip);

		cmd_updateReactors(hip);
	}

	exit:
	FreeResbufIfNotNull(&rbpts);
	FreeResbufIfNotNull(&elist);
	//free all we've piled up....
	if(ptarray_beg!=NULL){
		for(ptarray_cur=ptarray_beg;ptarray_beg!=NULL;ptarray_cur=ptarray_beg){
			ptarray_beg=ptarray_beg->next;
			if(ptarray_cur->listbeg!=NULL){
				for(ptscur=ptarray_cur->listbeg;ptarray_cur->listbeg!=NULL;ptscur=ptarray_cur->listbeg){
					ptarray_cur->listbeg=ptscur->next;
					delete ptscur;
				}
			}
			delete ptarray_cur;
		}
	}

    for(fl1=0L;RTNORM==sds_ssname(ssdel,fl1,etemp1);fl1++){
	    sds_redraw(etemp1,IC_REDRAW_UNHILITE);
    }
    sds_ssfree(ss1);
    sds_ssfree(ssdel);
	if(ss2[0]!=0 && ss2[1]!=0)sds_ssfree(ss2);
	return(ret);
}



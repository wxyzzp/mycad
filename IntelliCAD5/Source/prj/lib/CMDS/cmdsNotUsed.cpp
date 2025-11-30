#include "cmds.h"
#include "cmdsNotUsed.h"
#include "cmdsDimBlock.h"

void cmd_make_51_group(sds_real *ang,db_drawing *pFlp) {
    resbuf		rbucs,
                rbwcs;
    sds_point pt = {0.0, 0.0, 0.0};

    cmd_initialrbcs(NULL,&rbwcs,&rbucs,pFlp);

    pt[0]=1.0;
    pt[1]=pt[2]=0.0;
    sds_trans(pt,&rbucs,&rbwcs,1,pt);    //now z on pt is irrelevant
    *ang=atan2(pt[1],pt[0]);             //get the angle we want
    ic_normang(ang,NULL);                //make sure it's between 0 and 2pi
                                         //pt[2] now has angle to use in 51 group
}

int cmd_pline_maxdim(sds_name pent, int pl70flag, sds_point minpt, sds_point maxpt, int dim2use) {
	//Returns points of max X or Y dimension, de-
	//	pending on if DIM2USE is 0 or 1.
	sds_point	vtx1 = {0.0, 0.0, 0.0},
				vtx2 = {0.0, 0.0, 0.0},
				startpt = {0.0, 0.0, 0.0},
				arcdef1 = {0.0, 0.0, 0.0},
				arcdef2 = {0.0, 0.0, 0.0},
				p1 = {0.0, 0.0, 0.0},
				p2 = {0.0, 0.0, 0.0};
	sds_name	v1 = {0L, 0L},
				v2 = {0L, 0L};
	sds_real	v1bulg = 0.0,
				fr1 = 0.0;
	resbuf		*v1list = NULL,
				*v2list = NULL;
	int			ret = 0,
				breaknow = 0;

	v1list=v2list=NULL;
	if(sds_entnext_noxref(pent,v2)!=RTNORM)return(-1);
	ic_encpy(v1,v2);
	if((v2list=sds_entget(v2))==NULL)return(-1);
	ic_assoc(v2list,0);
	if(!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/)){ret=-1;goto exit;}
	ic_assoc(v2list,10);
	//set startpt, vtx2, minpt, and maxpt to 1st vertex point
	ic_ptcpy(vtx2,ic_rbassoc->resval.rpoint);
	ic_ptcpy(startpt,ic_rbassoc->resval.rpoint);
	ic_ptcpy(minpt,ic_rbassoc->resval.rpoint);
	ic_ptcpy(maxpt,ic_rbassoc->resval.rpoint);
	do{
		ic_ptcpy(vtx1,vtx2);
		IC_RELRB(v1list)		//was:	if(v1list!=NULL){sds_relrb(v1list);v1list=NULL;}
		v1list=v2list;v2list=NULL;
		ic_encpy(v1,v2);
		if(sds_entnext_noxref(v1,v2)!=RTNORM){ret=-1;goto exit;}
   		IC_RELRB(v2list);	
		if((v2list=sds_entget(v2))==NULL){ret=-1;goto exit;}
		ic_assoc(v1list,42);
		v1bulg=ic_rbassoc->resval.rreal;
		ic_assoc(v2list,0);
		if(!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/)){
			if(0==(pl70flag&1))break;
			ic_ptcpy(vtx2,startpt);
			breaknow=1;
		}else{
			ic_assoc(v2list,10);
			ic_ptcpy(vtx2,ic_rbassoc->resval.rpoint);
		}
		if(fabs(v1bulg)<IC_ZRO){
			if(vtx2[dim2use]>maxpt[dim2use])ic_ptcpy(maxpt,vtx2);
			if(vtx2[dim2use]<minpt[dim2use])ic_ptcpy(minpt,vtx2);
		}else{
			if(0!=ic_bulge2arc(vtx1,vtx2,v1bulg,arcdef1,&arcdef2[0],&arcdef2[1],&arcdef2[2]))continue;
			ic_normang(&arcdef2[1],&arcdef2[2]);
			p1[0]=arcdef1[0]+arcdef2[0]*cos(arcdef2[1]);
			p1[1]=arcdef1[1]+arcdef2[0]*sin(arcdef2[1]);
            p1[2]=vtx2[2];
			p2[0]=arcdef1[0]+arcdef2[0]*cos(arcdef2[2]);
			p2[1]=arcdef1[1]+arcdef2[0]*sin(arcdef2[2]);
            p2[2]=vtx2[2];
			if(dim2use==0){
				//get min and max in X direction
				if(arcdef2[2]>IC_TWOPI){
					if(arcdef1[0]+arcdef2[0]>maxpt[0]){
						maxpt[0]=arcdef1[0]+arcdef2[0];
						maxpt[1]=arcdef1[1];
						maxpt[2]=arcdef1[2];
					}
				}else{
					if(p1[0]>maxpt[0])ic_ptcpy(maxpt,p1);
				    if(p2[0]>maxpt[0])ic_ptcpy(maxpt,p2);
				}
				//get min x
				if((fr1=IC_PI)<arcdef2[1])fr1+=IC_TWOPI;
				if(arcdef2[2]>fr1){
					if(arcdef1[0]-arcdef2[0]<minpt[0]){
						minpt[0]=arcdef1[0]-arcdef2[0];
						minpt[1]=arcdef1[1];
						minpt[2]=arcdef1[2];
					}
				}else{
					if(p1[0]<minpt[0])ic_ptcpy(minpt,p1);
					if(p2[0]<minpt[0])ic_ptcpy(minpt,p2);
				}
			}else{	//get min & max in Y direction
				//get max y
				if((fr1=0.5*IC_PI)<arcdef2[1])fr1+=IC_TWOPI;
				if(arcdef2[2]>fr1){
					if(arcdef1[1]+arcdef2[0]>maxpt[1]){
						maxpt[1]=arcdef1[1]+arcdef2[0];
						maxpt[0]=arcdef1[0];
						maxpt[2]=arcdef1[2];
					}
				}else{
					if(p1[1]>maxpt[1])ic_ptcpy(maxpt,p1);
					if(p2[1]>maxpt[1])ic_ptcpy(maxpt,p2);
				}
				//get min y
				if((fr1=1.5*IC_PI)<arcdef2[1])fr1+=IC_TWOPI;
				if(arcdef2[2]>fr1){
					if(arcdef1[1]-arcdef2[0]<minpt[1]){
						minpt[1]=arcdef1[1]-arcdef2[0];
						minpt[0]=arcdef1[0];
						minpt[2]=arcdef1[2];
					}
				}else{
					if(p1[1]<minpt[1])ic_ptcpy(minpt,p1);
					if(p2[1]<minpt[1])ic_ptcpy(minpt,p2);
				}
			}
		}
	}while(breaknow==0);
	exit:
	IC_RELRB(v1list);	// 	was:	if(v1list!=NULL){sds_relrb(v1list);v1list=NULL;}
	IC_RELRB(v2list);	// 	was:	if(v2list!=NULL){sds_relrb(v2list);v2list=NULL;}
	return(ret);
}

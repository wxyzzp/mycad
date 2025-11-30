#include "icadlib.h"/*DNT*/

//Proto
void ss3d_chkpt(sds_point p1, sds_point ll, sds_point ur, short *takeit);

short ic_ss2dexts(sds_name ss, sds_point ll, sds_point ur) 
	{
/*
**  This function produces a 3D limits in WCS.  The
**		elevations of ARCs, CIRCLEs, LINEs, TRACEs, SOLIDs and POLYLINEs 
**		are ignored. 
**	3D LINEs, 3DFACEs and 3D POLYLINEs are accepted, but the Z-values 
**		are ignored.  The resulting intersection points are given the 
**		current elevation for their Z-coordinates. Extrusion directions
**		are IGNORED!
**	Text is accepted, using the sds_textbox f'n for it's extents
**
**  The following entities are ignored when found in the selection sets:
**      All entities except LINEs, ARCs, SOLIDs, CIRCLEs 3DFACEs and POLYLINEs
**      POLYFACE MESHes are ignored
**	This function is used by hatch command to determine boundary
**		prior to beginning more refined hatch calculations.
**	Returns:
**		0:	OK
**	   -1:	Error!
*/
	long ct;
	struct resbuf *elist=NULL,*rbptemp,rbent,rbwcs;
	int closure;
	short takeit=1,gotit;
	sds_name ent,ent2;
	sds_point p1,p2,p3,p4,ptemp;
	sds_real nextbulg;

	rbwcs.restype=RTSHORT;
	rbent.restype=RT3DPOINT;
	rbwcs.resval.rint=0;
	rbent.rbnext=rbwcs.rbnext=NULL;
	
	for (ct=0L;sds_ssname(ss,ct,ent)==RTNORM;ct++)
		{
		FreeResbufIfNotNull(&elist);

        if ((elist=sds_entget(ent))==NULL ||
            ic_assoc(elist,0))
			continue;	//bum ename


		if (RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,3DFACE"/*DNT*/))
			{
			for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
				{
				if (rbptemp->restype>9 && rbptemp->restype<14)
					ss3d_chkpt(rbptemp->resval.rpoint,ll,ur,&takeit);
				}
			}
		else if (RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,"SOLID,TRACE"/*DNT*/))
			{
			ic_assoc(elist,210);
			ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
			for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
				{
				if (rbptemp->restype>9 && rbptemp->restype<14)
					{
					sds_trans(rbptemp->resval.rpoint,&rbent,&rbwcs,0,ptemp);
					ss3d_chkpt(ptemp,ll,ur,&takeit);
					}
				}
			}
		else if (strsame(ic_rbassoc->resval.rstring,"TEXT"/*DNT*/))
			{
			ic_assoc(elist,210);
			ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
			for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
				{
				if (rbptemp->restype==10)
					ic_ptcpy(p3,rbptemp->resval.rpoint);
				else if (rbptemp->restype==40)
					p4[0]=rbptemp->resval.rreal;
				}
			sds_textbox(elist,p1,p2);

			//this f'n assumes text at 0,0 w/rotation of 0.0
			p4[1]=sin(p4[0]);
			p4[2]=cos(p4[0]);
			//1st check the corners reported by sds_textbox
			ptemp[0]=p1[0]*p4[2]-p1[1]*p4[1];
			ptemp[1]=p1[1]*p4[2]+p1[0]*p4[1];
			p1[0]=ptemp[0]+p3[0];
			p1[1]=ptemp[1]+p3[1];
			ptemp[0]=p2[0]*p4[2]-p2[1]*p4[1];
			ptemp[1]=p2[1]*p4[2]+p2[0]*p4[1];
			p2[0]=ptemp[0]+p3[0];
			p2[1]=ptemp[1]+p3[1];
			sds_trans(p1,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			sds_trans(p2,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			if ((fabs(p4[0])) > IC_PI/2.0)
				{
				//now check the other 2 corners....
				ptemp[0]=p1[0]*p4[2]-p2[1]*p4[1];
				ptemp[1]=p2[1]*p4[2]+p1[0]*p4[1];
				ptemp[0]+=p3[0];
				ptemp[1]+=p3[1];
				sds_trans(ptemp,&rbent,&rbwcs,0,ptemp);
				ss3d_chkpt(ptemp,ll,ur,&takeit);
				ptemp[0]=p2[0]*p4[2]-p1[1]*p4[1];
				ptemp[1]=p1[1]*p4[2]+p2[0]*p4[1];
				ptemp[0]+=p3[0];
				ptemp[1]+=p3[1];
				sds_trans(ptemp,&rbent,&rbwcs,0,ptemp);
				ss3d_chkpt(ptemp,ll,ur,&takeit);
				}
			}
		else if (strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))
			{
			for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
				{
				if (rbptemp->restype==10)
					ic_ptcpy(p1,rbptemp->resval.rpoint);
				else if (rbptemp->restype==40)
					p2[0]=rbptemp->resval.rreal;
				else if (rbptemp->restype==50)
					p2[1]=rbptemp->resval.rreal;
				else if (rbptemp->restype==51)
					p2[2]=rbptemp->resval.rreal;
				else if (rbptemp->restype==210)
					ic_ptcpy(rbent.resval.rpoint,rbptemp->resval.rpoint);
				}
			ic_arcextent(p1,p2[0],p2[1],p2[2],p3,p4);
			sds_trans(p3,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			sds_trans(p4,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			ic_ptcpy(ptemp,p3);
			ptemp[0]=p4[0];
			sds_trans(ptemp,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			ic_ptcpy(ptemp,p3);
			ptemp[1]=p4[1];
			sds_trans(ptemp,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			}
		else if (strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/))
			{
			for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
				{
				if (rbptemp->restype==10)
					ic_ptcpy(p1,rbptemp->resval.rpoint);
				else if (rbptemp->restype==40)
					p4[0]=rbptemp->resval.rreal;	//radius of circle
				else if (rbptemp->restype==210)
					ic_ptcpy(rbent.resval.rpoint,rbptemp->resval.rpoint);
				}
			//check at 4 quadrants...
			ic_ptcpy(p2,p1);
			p2[0]+=p4[0];
			p2[1]+=p4[0];
			sds_trans(p2,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			ic_ptcpy(p2,p1);
			p2[0]+=p4[0];
			p2[1]-=p4[0];
			sds_trans(p2,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			ic_ptcpy(p2,p1);
			p2[0]-=p4[0];
			p2[1]-=p4[0];
			sds_trans(p2,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			ic_ptcpy(p2,p1);
			p2[0]-=p4[0];
			p2[1]+=p4[0];
			sds_trans(p2,&rbent,&rbwcs,0,ptemp);
			ss3d_chkpt(ptemp,ll,ur,&takeit);
			}
		else if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
			{
			ic_assoc(elist,70);
			closure=(ic_rbassoc->resval.rint & 1);
			rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
			rbent.resval.rpoint[2]=1.0;
			if (0==(ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
				{
				ic_assoc(elist,210);
				ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
				}
				
			//now walk the pline and check each segment
			gotit=0;
			do
				{
				if (sds_entnext_noxref(ent,ent)!=RTNORM)
					break;
				if (sds_entnext_noxref(ent,ent2)!=RTNORM)
					break;//can only happen if seqend is last ent in dwg
				FreeResbufIfNotNull(&elist);
				if ((elist=sds_entget(ent))==NULL)
					break;
				if (gotit<1)
					{
					for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
						{
						if (rbptemp->restype==0)
							{
							if (!strsame(rbptemp->resval.rstring,"VERTEX"/*DNT*/))
								{
								gotit=-1;
								break;
								}
							}
						else if (rbptemp->restype==10)
							ic_ptcpy(p1,rbptemp->resval.rpoint);
						else if (rbptemp->restype==42)
							p4[2]=rbptemp->resval.rreal;
						}
					if (gotit==-1)
						break;
					gotit=1;
					}
				else
					{
					ic_ptcpy(p1,p2);
					p4[2]=nextbulg;
					}
				FreeResbufIfNotNull(&elist);
				if ((elist=sds_entget(ent2))==NULL)
					break;
				for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
					{
					if (rbptemp->restype==-2)
						{
						if (sds_entnext_noxref(rbptemp->resval.rlname,ent2)!=RTNORM)
							{
							takeit=1;
							goto exit;
							}
						gotit=0;
						FreeResbufIfNotNull(&elist);
						if ((elist=sds_entget(ent2))==NULL)
							{
							takeit=1;
							goto exit;
							}
						rbptemp=elist;
						}
					else if (rbptemp->restype==0)
						{
						if (!strsame(rbptemp->resval.rstring,"VERTEX"/*DNT*/))
							{
							if (!closure) 
								break;
							}
						}
					else if (rbptemp->restype==10)
						ic_ptcpy(p2,rbptemp->resval.rpoint);
					else if (rbptemp->restype==42)
						nextbulg=rbptemp->resval.rreal;
					}
				if (fabs(p4[2])<IC_ZRO)
					{	//a straight segment
					sds_trans(p1,&rbent,&rbwcs,0,ptemp);
					ss3d_chkpt(ptemp,ll,ur,&takeit);
					sds_trans(p2,&rbent,&rbwcs,0,ptemp);
					ss3d_chkpt(ptemp,ll,ur,&takeit);
					}
				else
					{
					if (ic_bulge2arc(p1,p2,p4[2],p3,&p4[0],&p4[1],&p4[2]))
						continue;
					ic_arcextent(p3,p4[0],p4[1],p4[2],p1,ptemp);//NOTE: DON'T overwrite P2!!
					sds_trans(p1,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					sds_trans(ptemp,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					ic_ptcpy(p4,p1);
					p4[0]=ptemp[0];
					sds_trans(p4,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					ic_ptcpy(p4,p1);
					p4[1]=ptemp[1];
					sds_trans(p4,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					}
				}
			while(1);
			}
		else if (strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
			{
			int closedit=0;
			ic_assoc(elist,70);
			closure=(ic_rbassoc->resval.rint & 1);
			rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
			rbent.resval.rpoint[2]=1.0;
//			if (0==(ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
//				{
				ic_assoc(elist,210);
				ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
//				}
				
			//now walk the pline and check each segment
			gotit=0;
			rbptemp=elist;
			do
				{
				if (gotit<1)
					{
					while (rbptemp->restype!=10)
						rbptemp=rbptemp->rbnext;
					ic_ptcpy(p1,rbptemp->resval.rpoint);
					while (rbptemp->restype!=42)
						rbptemp=rbptemp->rbnext;
					p4[2]=rbptemp->resval.rreal;
					gotit=1;
					}
				else
					{
					ic_ptcpy(p1,p2);
					p4[2]=nextbulg;
					}
				while (rbptemp->restype!=10 && rbptemp->rbnext!=NULL)
					rbptemp=rbptemp->rbnext;
				if (rbptemp->rbnext==NULL)
					{
					if (closure)
						{
						rbptemp=elist;
						while (rbptemp->restype!=10)
							rbptemp=rbptemp->rbnext;
						closedit=1;
						}
					else 
						{
						takeit=1;
						goto exit;
						}
					}

				// it's 10 if it gets here
				ic_ptcpy(p2,rbptemp->resval.rpoint);
				while (rbptemp->restype!=42)
					rbptemp=rbptemp->rbnext;
				nextbulg=rbptemp->resval.rreal;

				if (fabs(p4[2])<IC_ZRO)
					{	//a straight segment
					sds_trans(p1,&rbent,&rbwcs,0,ptemp);
					ss3d_chkpt(ptemp,ll,ur,&takeit);
					sds_trans(p2,&rbent,&rbwcs,0,ptemp);
					ss3d_chkpt(ptemp,ll,ur,&takeit);
					}
				else
					{
					if (ic_bulge2arc(p1,p2,p4[2],p3,&p4[0],&p4[1],&p4[2]))
						continue;
					ic_arcextent(p3,p4[0],p4[1],p4[2],p1,ptemp);//NOTE: DON'T overwrite P2!!
					sds_trans(p1,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					sds_trans(ptemp,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					ic_ptcpy(p4,p1);
					p4[0]=ptemp[0];
					sds_trans(p4,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					ic_ptcpy(p4,p1);
					p4[1]=ptemp[1];
					sds_trans(p4,&rbent,&rbwcs,0,p3);
					ss3d_chkpt(p3,ll,ur,&takeit);
					}
				if (closedit)
					break;
				}
			while(1);
			}


		}//ssname
exit:
	FreeResbufIfNotNull(&elist);
	return(takeit);//O if we set the pts, 1 if we didn't
}


void ss3d_chkpt(sds_point p1, sds_point ll, sds_point ur, short *takeit){

	if (*takeit || (p1[0]<ll[0]))
		ll[0]=p1[0];
	if (*takeit || (p1[1]<ll[1]))
		ll[1]=p1[1];
	if (*takeit || (p1[2]<ll[2]))
		ll[2]=p1[2];
	if (*takeit || (p1[0]>ur[0]))
		ur[0]=p1[0];
	if (*takeit || (p1[1]>ur[1]))
		ur[1]=p1[1];
	if (*takeit || (p1[2]>ur[2]))
		ur[2]=p1[2];
	*takeit=0;
	return;
}

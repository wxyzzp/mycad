#include "icadlib.h"/*DNT*/

short ic_ortho(sds_point base, sds_real snapang, sds_point before, sds_point after, int sstyle) {
/*
**  Performs ortho-snapping.
**
**  Given base point (base), the rotation angle of the snap grid relative
**  to the X-axis (snapang), and an initial point (before), this
**  function sets the ortho-snapped point (after).  The "before" point
**  projects onto the closest ortho-axis through the base point to
**  produce the "after" point.
**  If sstyle==-1, standard ortho
**				0, isometric ortho, LEFT
**				1, isometric ortho, TOP
**				2, isometric ortho, RIGHT 
**
**  Returns:
**       0 : OK
**      -1 : Calling error (one or more pts were passed as NULL)
*/
    sds_real bdx,bdy,v1x,v1y,blen,csx,csy,angsin,angcos;
	
    if (base==NULL || before==NULL || after==NULL) return -1;

    after[2]=base[2];  /* Preserve base point's Z. */

	angcos=cos(snapang);angsin=sin(snapang);


	
	if(sstyle<0 || sstyle>2){
		/* Find the closest ortho direction by finding the largest abs val of */
		/* the cos of the angle between the "before" unit vector and an */
		/* ortho unit vector: */

		bdx=before[0]-base[0]; bdy=before[1]-base[1];
		if (icadRealEqual((blen=sqrt(bdx*bdx+bdy*bdy)),0.0))
			{ after[0]=base[0]; after[1]=base[1]; return 0; }
		bdx/=blen; bdy/=blen;  /* Unit "before" vector. */


		csx=  angcos*bdx+angsin*bdy;	/* cos from primary ortho axis */
		csy=-angsin*bdx+angcos*bdy;		/* cos from other ortho axis */

		if (fabs(csx)>=fabs(csy)) {
			v1x= angcos; v1y=angsin; 
		}else{ 
			v1x=-angsin; v1y=angcos; csx=csy; 
		}

		csy=blen*csx;  /* Directed length of the projection. */
		after[0]=base[0]+csy*v1x; 
		after[1]=base[1]+csy*v1y;
	}else{
		//*******************ISOMETRIC SNAP**********************
		sds_point ptemp1,ptemp2;
		//FIRST - account for snapang and rotate before pt to be in coord where snapang=0
		if(icadRealEqual(snapang,0.0)){
			ic_ptcpy(ptemp1,before);
		}else{
			//rotate before pt to account for snapang
			ptemp1[0]=base[0]+(before[0]-base[0])*angcos+(before[1]-base[1])*angsin;
			ptemp1[1]=base[1]+(before[1]-base[1])*angcos-(before[0]-base[0])*angsin;
		}
		bdx=ptemp1[0]-base[0]; bdy=ptemp1[1]-base[1];
		if (icadRealEqual((blen=sqrt(bdx*bdx+bdy*bdy)),0.0))
			{ after[0]=base[0]; after[1]=base[1]; return 0; }

		//V1 represents iso X vector, V2 iso Y vector
		if(sstyle==0){
			//v1x=cos(-IC_PI/6.0)	X unit vector
			//v1y=sin(-IC_PI/6.0)
			//v2x=-sin(0)			Y unit vector
			//v2y=cos(0)
			//take dot product of x vector & point & compare w/ y vector product
			csx=1.732050807568877*0.5*bdx/blen-0.5*bdy/blen;
			csy=bdy/blen;									
			if(fabs(csy)<fabs(csx)){	//We're closer to X axis, so project along it...
				ptemp2[1]=ptemp1[1]-bdy-bdx/1.732050807568877;
				ptemp2[0]=ptemp1[0];
			}else{
				ptemp2[0]=ptemp1[0]-bdx;
				ptemp2[1]=ptemp1[1]+bdx/1.732050807568877;
			}
		}else if(sstyle==1){
			//v1x=cos(-IC_PI/6.0)	X unit vector
			//v1y=sin(-IC_PI/6.0)
			//v2x=cos(IC_PI/6.0)	Y unit vector
			//v2y=sin(IC_PI/6.0)
			csx=1.732050807568877*0.5*bdx/blen-0.5*bdy/blen;	//dot w/X vector
			csy=1.732050807568877*0.5*bdx/blen+0.5*bdy/blen;	//dot w/Y vector
			if(fabs(csy)>fabs(csx)){
				ptemp2[0]=ptemp1[0]-0.5*bdx+0.5*bdy*1.732050807568877;
				ptemp2[1]=ptemp1[1]-0.5*bdy+0.5*bdx/1.732050807568877;
			}else{
				ptemp2[0]=ptemp1[0]-0.5*bdx-0.5*bdy*1.732050807568877;
				ptemp2[1]=ptemp1[1]-0.5*bdy-0.5*bdx/1.732050807568877;
			}
		}else if(sstyle==2){
			//v1x=cos(IC_PI/6.0);	X unit vector
			//v1y=sin(IC_PI/6.0);
			//v2x=-sin(0);			Y unit vector
			//v2y=cos(0);
			csx=1.732050807568877*0.5*bdx/blen+0.5*bdy/blen;	//dot w/X vector
			csy=bdy/blen;										//dot w/Y vector
			if(fabs(csy)>fabs(csx)){
				ptemp2[1]=ptemp1[1]-bdx/1.732050807568877;
				ptemp2[0]=ptemp1[0]-bdx;
			}else{
				ptemp2[0]=ptemp1[0];
				ptemp2[1]=ptemp1[1]-bdy+bdx/1.732050807568877;
			}

		}
		if(icadRealEqual(snapang,0.0)){
			after[0]=ptemp2[0];
			after[1]=ptemp2[1];
		}else{
			//rotate ptemp2 back by snapang
			after[0]=(ptemp2[0]-base[0])*angcos-(ptemp2[1]-base[1])*angsin+base[0];
			after[1]=(ptemp2[1]-base[1])*angcos+(ptemp2[0]-base[0])*angsin+base[1];
		}

	}
    return 0;
}

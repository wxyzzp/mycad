#include "icadlib.h"/*DNT*/


// rotaxis -- rotate around the given axis by angle, return vectors indicating the
// x and y directions for that rotation

void ic_rotaxis(sds_point axis, double angle, sds_point newxdir, sds_point newydir)
{
	struct resbuf rbecs,rbwcs;
	sds_point p1,p2;
	
	rbecs.restype=RT3DPOINT;
	memcpy(rbecs.resval.rpoint,axis,3*sizeof(double));
    rbwcs.restype    =RTSHORT;
    rbwcs.resval.rint=0;

	p1[0]=cos(angle);
	p1[1]=sin(angle);
	p1[2]=0.0;
	sds_trans(p1,&rbecs,&rbwcs,1,newxdir);
	p2[0]=-p1[1];
	p2[1]=p1[0];
	p2[2]=0.0;
	sds_trans(p2,&rbecs,&rbwcs,1,newydir);
}

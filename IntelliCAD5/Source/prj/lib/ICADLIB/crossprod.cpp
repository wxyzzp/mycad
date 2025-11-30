
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

void ic_crossproduct(double *a, double *b, double *c) {
// take the cross product of two 3D vectors
// c = a x b
// use a temp variable so we can do  crossproduct(a,b,a);
	
	double temp[3];

	temp[0]=a[1]*b[2]-a[2]*b[1];
	temp[1]=a[2]*b[0]-a[0]*b[2];
	temp[2]=a[0]*b[1]-a[1]*b[0];
	memcpy(c,temp,3*sizeof(double));
}

double ic_dotproduct(double *a,double *b)
// dot product of two vectors
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void ic_identity(double (*a)[3])
{
	memset(a,0,9*sizeof(double));
	a[0][0]=a[1][1]=a[2][2]=1.0;
}

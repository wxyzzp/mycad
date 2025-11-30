
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

void ic_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint) 
    {
	ptPoint[0]=ptPolarCtr[0]+dDistance*cos(dAngle); 
	ptPoint[1]=ptPolarCtr[1]+dDistance*sin(dAngle); 
	ptPoint[2]=ptPolarCtr[2];
	}

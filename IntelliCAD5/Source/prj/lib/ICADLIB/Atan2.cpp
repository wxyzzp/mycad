
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

double ic_atan2(double yy, double xx) {

    short dexp,inf;
    double absxx,absyy,dmin,fd1,fd2;

    dexp=300; dmin=1.0E-300; inf=0;

    absxx=fabs(xx); absyy=fabs(yy);

    if (absxx<dmin) {
        if (absyy<dmin) return 0.0;  /* Should give error, too */
        inf=1;
    } else if (absyy>=dmin) {
        fd1=log10(absyy); fd2=log10(absxx);
        if (fd1-fd2>dexp) inf=1;
    }
    if (inf) return (yy>0.0) ? IC_PI/2.0 : -IC_PI/2.0;

    fd1=atan(yy/xx);
    if (xx<0.0) fd1+=(yy<0.0) ? -IC_PI : IC_PI;

    return fd1;
}

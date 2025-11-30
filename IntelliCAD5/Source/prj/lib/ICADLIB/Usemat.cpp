#include "icadlib.h"/*DNT*/


int ic_usemat(
    sds_point  sour,        /* Source point */
    sds_point  dest,        /* Destination point (can be the same pt as sour) */
    void      *mat,         /* The transformation matrix */
    int        disp,        /* 0 : sour and dest are pts; !0 : they're vectors */
    int        oldmattype,  /* Which type of matrix? 0 : 4x4; !0 : 4x3 */
    int        inverse      /* 0 : Forward; !0 : Backward */
    ) {
/*
**  Transforms a point (disp==0) or vector (disp!=0) using a matrix.
**
**  For inverse==0:
**
**    If it's an ACAD-style nentselp matrix, it will go from the
**    Block Coordinate System (BCS) to the WCS.  If it's OUR
**    nentselp matrix, it will go from an entity's Native
**    Coordinate System (NCS), which is WCS or ECS, to the UCS.
**
**  For inverse!=0:
**
**    Transforms in the opposite direction from (WCS to BCS, or
**    UCS to NCS).
**
**  For oldmat==0:
**
**    Assumes mat points to an sds_matrix -- the new-style (4x4)
**    matrix (nentselp).
**
**  For oldmat!=0:
**
**    Assumes that mat points to an array of 4 sds_points -- the
**    old-style (4x3) matrix (nentsel).  This function will build a
**    new-style working matrix from it.
**
**  Returns:
**       0 : OK
**      -1 : Bad calling parameter(s)
**      -2 : Unable to comply (inverse only)
*/
    int rc,fi1,fi2;
    sds_real denom,ar[3];
    sds_point *ptp,wsour;
    sds_matrix wmat;


    rc=0;


    if (sour==NULL || dest==NULL || mat==NULL) { rc=-1; goto out; }

    memset(wmat,0,sizeof(wmat));
    wmat[0][0]=wmat[1][1]=wmat[2][2]=wmat[3][3]=1.0;

    wsour[0]=sour[0]; wsour[1]=sour[1]; wsour[2]=sour[2];

    if (oldmattype) {
        ptp=(sds_point *)mat;
        for (fi1=0; fi1<4; fi1++)
            for (fi2=0; fi2<3; fi2++) wmat[fi2][fi1]=ptp[fi1][fi2];
    } else memcpy(wmat,mat,sizeof(wmat));

    if (disp) wmat[0][3]=wmat[1][3]=wmat[2][3]=0.0;

    if (inverse) {  /* 3 equations, 3 unknowns; solve using determinants. */
        if (icadRealEqual((denom=wmat[0][0]*(wmat[1][1]*wmat[2][2]-wmat[1][2]*wmat[2][1])-
                   wmat[0][1]*(wmat[1][0]*wmat[2][2]-wmat[1][2]*wmat[2][0])+
                   wmat[0][2]*(wmat[1][0]*wmat[2][1]-wmat[1][1]*wmat[2][0])),0.0))
                        { rc=-2; goto out; }
        ar[0]=wsour[0]-wmat[0][3];
        ar[1]=wsour[1]-wmat[1][3];
        ar[2]=wsour[2]-wmat[2][3];
        dest[0]=(ar[0]     *(wmat[1][1]*wmat[2][2]-wmat[1][2]*wmat[2][1])-
                 wmat[0][1]*(ar[1]     *wmat[2][2]-wmat[1][2]*ar[2])+
                 wmat[0][2]*(ar[1]     *wmat[2][1]-wmat[1][1]*ar[2]))/denom;
        dest[1]=(wmat[0][0]*(ar[1]     *wmat[2][2]-wmat[1][2]*ar[2])-
                 ar[0]     *(wmat[1][0]*wmat[2][2]-wmat[1][2]*wmat[2][0])+
                 wmat[0][2]*(wmat[1][0]*ar[2]     -ar[1]     *wmat[2][0]))/denom;
        dest[2]=(wmat[0][0]*(wmat[1][1]*ar[2]     -ar[1]     *wmat[2][1])-
                 wmat[0][1]*(wmat[1][0]*ar[2]     -ar[1]     *wmat[2][0])+
                 ar[0]     *(wmat[1][0]*wmat[2][1]-wmat[1][1]*wmat[2][0]))/denom;
    } else {  /* Forward */
        dest[0]=wmat[0][0]*wsour[0]+wmat[0][1]*wsour[1]+wmat[0][2]*wsour[2]+wmat[0][3];
        dest[1]=wmat[1][0]*wsour[0]+wmat[1][1]*wsour[1]+wmat[1][2]*wsour[2]+wmat[1][3];
        dest[2]=wmat[2][0]*wsour[0]+wmat[2][1]*wsour[1]+wmat[2][2]*wsour[2]+wmat[2][3];
    }


out:
    return rc;
}

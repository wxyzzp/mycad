
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

short ic_inorout(struct ic_pointsll *ptsll,
    sds_real extl, sds_real extb, sds_real extr, sds_real extt,
    sds_real x0, sds_real y0) {
/*
**  Llist ptsll must define a polygon (convex or concave) and the vertices
**  must be in order.  The last vertex is considered close to the first.
**  There must be at least 3 links.
**
**  The extents edges of the polygon are expected to be in the "ext?"
**  variables; the must specify a rectangle that precisely encloses the
**  polygon.
**
**  Returns:
**      -1 : Error -- less than 3 vertices
**       0 : (x0,y0) is outside
**       1 : (x0,y0) is inside
*/
    short inside;
    sds_real dx,dy;
    struct ic_pointsll *v0,*v1,*lv,*rv;

    if (ptsll==NULL || ptsll->next==NULL || ptsll->next->next==NULL) return -1;

    /* Pt is out if extents window has no area: */
    if (extr-extl<IC_ZRO || extt-extb<IC_ZRO) return 0;

    /* Do quick in/out check based on extents: */
    if (x0<extl || x0>extr || y0<extb || y0>extt) return 0;

    inside=0;
    for (v0=ptsll; v0!=NULL; v0=v0->next) {
        v1=(v0->next==NULL) ? ptsll : v0->next;
        dx=v1->pt[0]-v0->pt[0]; dy=v1->pt[1]-v0->pt[1];
        if (fabs(dx)<IC_ZRO) continue;

        /* Update in/out status.  (Call the left edge IN (>=) */
        /* and the right out (<) so a borderline case */
        /* only gets caught ONCE.) */
        if (dx>0.0) { lv=v0; rv=v1; } else { lv=v1; rv=v0; }
        if (x0>=lv->pt[0] && x0<rv->pt[0] && y0>=extb &&
            y0<=(x0-v0->pt[0])/dx*dy+v0->pt[1]) {

            if (dx<0.0) inside--; else inside++;
        }
    }  /* end if for each side */

    return (inside!=0);
}


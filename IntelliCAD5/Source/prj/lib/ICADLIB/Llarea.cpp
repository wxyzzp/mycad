#include "icadlib.h"/*DNT*/


sds_real ic_llarea(struct ic_pointsll *ptsll) {
/*
**  Llist ptsll must define a polygon (convex or concave) and the vertices
**  must be in order.  The last vertex is considered close to the first.
**  There must be at least 3 links.
**
**  Returns:
**      -1.0 : Error -- less than 3 vertices
**     >=0.0 : Area
*/
    sds_real area;
    struct ic_pointsll *v0,*v1;

    if (ptsll==NULL || ptsll->next==NULL || ptsll->next->next==NULL)
        return -1.0;

    for (v0=ptsll,area=0.0; v0!=NULL; v0=v0->next) {
        v1=(v0->next==NULL) ? ptsll : v0->next;
        area+=(v1->pt[0]-v0->pt[0])*(v0->pt[1]+v1->pt[1])/2.0;
    }

    return (area<0.0) ? -area : area;
}

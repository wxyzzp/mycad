#include "icadlib.h"/*DNT*/
#include "point.h"
#include "transf.h"
using namespace icl;

short ic_linexline(const sds_point p0,const sds_point p1,const sds_point p2,const sds_point p3,
    sds_point xpt) {

/*D.Gavrilov*/// I've made it 3D for xpt[2].
// If we don't care about xpt[2] here than we should do it in callers
// (in cmd_get_ln_extpt and cmd_trim_object at least).

/*
**  Determines the intersection of the line defined by p0 and p1 with the
**  line defined by p2 and p3.  Sets xpt to the intersection point
**  if return value is non-negative.  (WAS: 2D only; the z-value of xpt is
**  set from p0.)
**
**  Returns:
**      -1 = At least 1 line undefined (coincident points), or both parallel.
**       0 = Intersection is not on either segment.
**       1 = Intersection is on the 1st segment (p0-p1) but not the 2nd.
**       2 = Intersection is on the 2nd segment (p2-p3) but not the 1st.
**       3 = Intersection is on both line segments.
*/
    short on1,on2;
    sds_real dx1,dx2,dy1,dy2,fd1,fd2,fd3;

    dx1=p1[0]-p0[0]; dy1=p1[1]-p0[1]; dx2=p3[0]-p2[0]; dy2=p3[1]-p2[1];
    fd1=dx1*dy2; fd2=dx2*dy1; fd3=fd1-fd2;

    if (fabs(fd3)<1.0E-10) return -1;  /* Line undefined or lines parallel */

    xpt[0]=(p2[0]*fd1-p0[0]*fd2-(p2[1]-p0[1])*dx1*dx2)/fd3;
    if (fabs(dx1)>fabs(dx2)) xpt[1]=dy1/dx1*(xpt[0]-p0[0])+p0[1];
    else                     xpt[1]=dy2/dx2*(xpt[0]-p2[0])+p2[1];
//	xpt[2]=p0[2]; /*D.Gavrilov*/// We'll set it later.

    on1=on2=1;

    /* See if it's on the 1st seg: */
    if (fabs(dx1)>fabs(dy1)) {
        if ((xpt[0]-p0[0])*(xpt[0]-p1[0]) > 1.0E-7/*0.0*/) on1=0;	/*D.G.*/// A fuzz added.
    } else {
        if ((xpt[1]-p0[1])*(xpt[1]-p1[1]) > 1.0E-7/*0.0*/) on1=0;	/*D.G.*/// A fuzz added.
    }

    /* See if it's on the 2nd seg: */
// 12/8/98 -- this was just comparing to 0.0, I added the the fuzz factor.
    if (fabs(dx2)>fabs(dy2)) {
        if ((xpt[0]-p2[0])*(xpt[0]-p3[0])>1.0E-7) on2=0;  // ratcheted this up to E-8 from E-11, per bug 57724
    } else {
        if ((xpt[1]-p2[1])*(xpt[1]-p3[1])>1.0E-7) on2=0;
    }

	/*D.Gavrilov*/// Now let's set xpt[2] as for point of one of the lines
	// parametric and genegal forms of equation of a line.
	if(shortCast(on1+2*on2) == 1)
		xpt[2] = p0[2] + (p1[2] - p0[2]) *
				(fabs(p1[0] - p0[0]) < fabs(p1[1] - p0[1]) ? 
				 (xpt[1] - p0[1]) / (p1[1] - p0[1]) :
				 (xpt[0] - p0[0]) / (p1[0] - p0[0]));
	else
		xpt[2] = p2[2] + (p3[2] - p2[2]) *
				(fabs(p3[0] - p2[0]) < fabs(p3[1] - p2[1]) ? 
				 (xpt[1] - p2[1]) / (p3[1] - p2[1]) :
				 (xpt[0] - p2[0]) / (p3[0] - p2[0]));

    return shortCast(on1+2*on2);
}

int ic_linexplane
(
const sds_point start,
const sds_point end,
const sds_point root,
const sds_point normal,
sds_point x
)
{
	// prepare vars
	point s(start), e(end);
	point r(root);
	point o(0., 0., 0.);
	gvector n(normal);

	// if ends of segments lie on different sides of the plane,
	// the intersection is correct
	double rn = (r - o)%n;

	double d1 = (s - o)%n - rn;
	double d2 = (e - o)%n - rn;
	double d = d1 * d2;

	if (d < -resabs2())
	{
		// intersection exists
		gvector view = e - s; view.normalize();
		
		// find intersection between plan and endless line
		point px = projection(s, r, n, &view);

		ic_ptcpy(x, px);

		return 1;
	}
	else if (d > resabs2())
	{
		// line segment does not intersect the plane
		return 0;
	}
	else
	{
		// both points lie on the plane
		ic_ptcpy(x, start);
		return 1;
	}
}

void ic_nearest
(
const sds_point start,
const sds_point end,
const sds_point pnt,
sds_point nearest
)
{
	// prepare vars
	point s(start), e(end);
	if (s == e)
	{
		ic_ptcpy(nearest , start);
		return;
	}

	gvector normal = e - s; normal.normalize();
	if (!ic_linexplane(start, end, pnt, normal, nearest))
	{
		// segment does not intersect the plane,
		// nearest point is one of the ends
		point p(pnt);
		if ((s - p).norm2() < (e - p).norm2())
			ic_ptcpy(nearest, start);
		else
			ic_ptcpy(nearest, end);
	}
}

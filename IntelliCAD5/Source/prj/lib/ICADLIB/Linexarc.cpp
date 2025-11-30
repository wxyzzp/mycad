#include "icadlib.h"/*DNT*/


short ic_linexarc(sds_point p0, sds_point p1, sds_point cc, sds_real rr,
    sds_real start, sds_real end, sds_point i0, sds_point i1) 
{
/*
**  Determines intersections i0 and i1 of line p0p1 with circle centered
**  at cc with radius rr.
**
**  Returns:
**      -1 : No intersections.  (Line farther than rr from circle --
**             or one or both entities are undefined (line seg has len
**             0, or circle has 0 radius).)
**     >-1 : Bit-coded (bits counted from right):
**             Bit 0 : i0 is on the line
**             Bit 1 : i0 is on the arc
**             Bit 2 : i1 is on the line
**             Bit 3 : i1 is on the arc
*/
    short i0onl,i0ona,i1onl,i1ona;
    sds_real dx,dy,dxl,dyl,i0ang,i1ang,ar1,ar2,ss;
    sds_point ap1;

    if (ic_ptlndist(cc,p0,p1,&ss,ap1) < 0 || ss > rr) // Line doesn't intersect the circle
        return -1;

	// Line intersects with the circle of arc.
	//
	short numOfIntersections = 0;
	if (fabs(rr * rr - ss * ss) <= IC_ZRO) { // Line is tangent to circle
		sds_real t = (ap1[0] - p0[0]) / (p1[0] - p0[0]);
		if (t > - 1.0 * IC_ZRO && t < 1.0 + IC_ZRO) {
			i0[0] = ap1[0]; i0[1] = ap1[1];
			i0onl = 1;
		}
		else
			i0onl = 0;

		i1onl = 0;
		numOfIntersections = 1;
	}
	else { 
		dxl = p1[0] - p0[0]; dyl = p1[1] - p0[1]; 
		sds_real dl = dxl * dxl + dyl * dyl;
		if (dl < 0.0)
			return -1;

		if (ss / rr > IC_ZRO) {  // Line cuts circle but not through center
			dx = p0[0] - cc[0]; dy = p0[1] - cc[1];
				
			sds_real a2 = 2.0 * dl;
			sds_real b = 2.0 * (dx * dxl + dy * dyl);
			sds_real c = dx * dx + dy * dy - rr * rr;
			sds_real d = b * b - 4. * dl * c;
			if (d < 0) 
				return -1;

			sds_real dd = sqrt(d);
			sds_real t0 = (dd - b) / a2;
			sds_real t1 = - (dd + b) / a2;

			i0[0] = p0[0] + t0 * dxl; i0[1] = p0[1] + t0 * dyl;
			i1[0] = p0[0] + t1 * dxl; i1[1] = p0[1] + t1 * dyl;

			i0onl = (t0 > - IC_ZRO && t0 < 1.0 + IC_ZRO);
			i1onl = (t1 > - IC_ZRO && t1 < 1.0 + IC_ZRO);
		}
		else { // Line passes through center
			ar1 = sqrt(dl);
			ar1 = rr / ar1;
			ar2 = dxl * ar1; i0[0] = cc[0] + ar2; i1[0] = cc[0] - ar2;
			ar2 = dyl * ar1; i0[1] = cc[1] + ar2; i1[1] = cc[1] - ar2;

			sds_real t0, t1;
			
			if (!icadIsZero(dxl)) {
				t0 = (i0[0] - p0[0]) / (p1[0] - p0[0]);
				t1 = (i1[0] - p0[0]) / (p1[0] - p0[0]);
			}
			else {
				t0 = (i0[1] - p0[1]) / (p1[1] - p0[1]);
				t1 = (i1[1] - p0[1]) / (p1[1] - p0[1]);
			}
			i0onl = (t0 > - IC_ZRO && t0 < 1.0 + IC_ZRO); 
			i1onl = (t1 > - IC_ZRO && t1 < 1.0 + IC_ZRO);
		}
		numOfIntersections = 2;
	}

    // See if i0 and i1 are on the arc
	//
	ic_normang(&start,&end);
	ar1 = i0[1] - cc[1]; ar2 = i0[0] - cc[0];
	i0ang = ic_atan2(ar1, ar2);
	ic_normang(&start, &i0ang);  // makes sure i0ang >= start
	i0ona = (i0ang <= end);

	// The follow check should be performed only if line and circle have two intersections
	// becuase the value of i1 are not really set in the case there is only single 
	// intersection.
	//
	if (numOfIntersections == 2) {
		ar1 = i1[1] - cc[1]; ar2 = i1[0] - cc[0];
		i1ang = ic_atan2(ar1, ar2);
		ic_normang(&start, &i1ang);
		i1ona = (i1ang <= end);
	}
	else
		i1ona = 0;

    short rc = i0onl;
    if (i0ona) rc|=2;
    if (i1onl) rc|=4;
    if (i1ona) rc|=8;

    return rc;
}


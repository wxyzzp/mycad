
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

short
ic_ellpts
(
 sds_point	cent,
 sds_point	zeropt,
 sds_real	dist2,
 sds_real	sang,
 sds_real	eang,
 short		angmode,
 short		full,
 short		reso,
 short*		nptsp,
 sds_point** parrayp
)
{
/*
**  Allocates an array of sds_points that approximates an ellipse
**  (full or partial).
**
**  Given:
**
**        cent : Center (give the elevation in cent[2]).
**      zeropt : The 0-angle point (defining semi-major or semi-minor
**                 axis).
**       dist2 : Length of the other axis (90 deg CCW from the 1st).
**        sang : Start angle (radians).
**        eang : End angle (radians).
**     angmode : Tells how to interpret sang and eang:
**                 0 : Angle of the line from cent to the point
**                       measured CCW from the zeropt axis.
**                 1 : Eccentric anomaly (ACAD's ResourceString(IDC_ELLPTS_PARAMETER_0, "parameter" ))
**                       (CCW from the zeropt axis).
**        full : Non-0 indicates a full ellipse (sang and eang are
**                 ignored).  (First and last pt are coincident
**                 at zeropt.)
**        reso : Resolution -- number of chords per quadrant (3-100).
**
**  Determines:
**
**       nptsp : Ptr to a short.  *nptsp is set to the number of points
**                 (elements in *parrayp).
**     parrayp : ADDRESS of a ptr to an sds_point.  *parrayp is pointed
**                 to an allocated array of sds_points (CALLER MUST FREE
**                 IF IT'S NON-NULL AFTER THE CALL!).
**
**  Points are generated CCW from sang to eang, like an ARC (0.0 to
**  2PI for a full ellipse).
**
**  Returns:
**       0 : OK
**      -1 : One of the distances is 0.0.
**      -2 : Too skinny (one dist is more than 100 times the other).
**      -3 : No memory.
*/
	short		nchords, rc = 0;
	sds_real	dist1, ar1;
	sds_point	*pa = NULL, axis[2];

	zeropt[2] = axis[0][2] = axis[1][2] = cent[2];
	if(dist2 < 0.0)
		dist2 = -dist2;

	// Set the primary axis vector:
	axis[0][0] = zeropt[0] - cent[0];
	axis[0][1] = zeropt[1] - cent[1];

	// Get primary dist and check both:
	dist1 = sqrt(axis[0][0] * axis[0][0] + axis[0][1] * axis[0][1]);
	if(!dist1 || !dist2)
	{
		rc = -1;
		goto out;
	}
	ar1 = dist2 / dist1;
	if(ar1 < 0.01 || ar1 > 100.0)
	{
		rc = -2;
		goto out;
	}

	// Set the secondary axis vector:
	axis[1][0] = -axis[0][1] / dist1 * dist2;
	axis[1][1] =  axis[0][0] / dist1 * dist2;

	// Set sang and eang to the proper eccentric anomalies:
	if(full)
	{
		sang = 0.0;
		eang = IC_TWOPI;
	}
	else
	{
		if(!angmode)
		{	// Convert to eccentric anomalies.
			ic_angpar(sang, &sang, dist1, dist2, 0);
			ic_angpar(eang, &eang, dist1, dist2, 0);
		}
		ic_normang(&sang, &eang);	// Normalize with eang>sang.
	}

	// Get # of pts needed and the ecc anom increment they specify:
	if(reso < 3)
		reso = 3;
	else
		if(reso > 100)
			reso = 100;
	nchords = shortCast(4 * reso);	// For full ellipse (a multiple of 4, so that a full
									// ellipse will have points on the axes).
	ar1 = eang - sang;
	if(!full)
	{
		nchords = (short)(nchords * ar1 / IC_TWOPI);
		if(nchords < 1)
			nchords = 1;
	}

	// Alloc the array:
	if(!(pa = new sds_point [nchords + 1]))
	{
		rc = -3;
		goto out;
	}
	memset(pa ,0 ,sizeof(sds_point));

	// Fill it:

	/*D.G.*/// Instead of computing sines & cosines on each step of the loop use formulas:
	// cos(s+Nd) = cos(s) * [2cos((N-1)d) * cos(d) - cos((N-2)d)] - sin(s) * [2sin((N-1)d) * cos(d) - sin((N-2)d)],
	// sin(s+Nd) = sin(s) * [2cos((N-1)d) * cos(d) - cos((N-2)d)] + cos(s) * [2sin((N-1)d) * cos(d) - sin((N-2)d)],
	// where s == sang. So, we compute only 2 sines & 2 cosines.
	{	/*D.G.*/// I added this block for avoiding the error C2362 "initialization of 'identifier' is skipped by 'goto out'"
		sds_real	d = ar1 / nchords,
					s0 = sin(sang), c0 = cos(sang), sd = sin(d), cd = cos(d),
					sine = s0 * cd + c0 * sd,  cosine = c0 * cd - s0 * sd;	// sin(s+Nd) and cos(s+Nd) (initialized for N == 1 case)

		// N == 0 and N == 1 cases (nchords == 1 at least)
		pa[0][0] = cent[0] + c0 * axis[0][0] + s0 * axis[1][0];
		pa[0][1] = cent[1] + c0 * axis[0][1] + s0 * axis[1][1];
		pa[1][0] = cent[0] + cosine * axis[0][0] + sine * axis[1][0];
		pa[1][1] = cent[1] + cosine * axis[0][1] + sine * axis[1][1];
		pa[1][2] = pa[0][2] = cent[2];

		if(nchords > 1)
		{
			sds_real	sN, cN,					// sin(Nd) and cos(Nd)
						sN_1 = sd, cN_1 = cd,	// sin((N-1)d) and cos((N-1)d)  - initialized for N == 2
						sN_2 = 0.0, cN_2 = 1.0;	// sin((N-2)d) and cos((N-2)d)  /

			for(short n = 2; n <= nchords; ++n)
			{
				sN = 2.0 * sN_1 * cd - sN_2;
				cN = 2.0 * cN_1 * cd - cN_2;
				sine   = s0 * cN + c0 * sN;
				cosine = c0 * cN - s0 * sN;
				pa[n][0] = cent[0] + cosine * axis[0][0] + sine * axis[1][0];
				pa[n][1] = cent[1] + cosine * axis[0][1] + sine * axis[1][1];
				pa[n][2] = cent[2];
				sN_2 = sN_1;
				cN_2 = cN_1;
				sN_1 = sN;
				cN_1 = cN;
			}
		}
	}	// block for avoiding C2362

out:
	if(rc && pa)
	{
		IC_FREE(pa);
		pa = NULL;
		nchords = 0;
	}
	*parrayp = pa;
	*nptsp = shortCast(nchords + (nchords != 0));

	return rc;
}

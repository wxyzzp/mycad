#include "Icad.h"
#include "IcadHelpers.h"
#include "Gr.h"

/****************************************************************************/
/********************* Helper Class CParallelogram **************************/
/****************************************************************************/

// builds a rectangle represented as parallelogram, assumes P and Q are two opposite corners
CParallelogram::CParallelogram(sds_point P,
							   sds_point Q)
	{
	sds_point diff;
	ic_ptcpy(m_pivot, P);
	ic_sub(P, Q, diff);
	m_uvec[0] = diff[0]; m_uvec[1] =     0.0; m_uvec[2] = 0.0;
	m_vvec[0] =     0.0; m_vvec[1] = diff[1]; m_vvec[2] = 0.0;

	}

CParallelogram::CParallelogram(CreationOption option,
							   sds_point a, 
							   sds_point pivot, // must be pivot point
							   sds_point c,
							   bool & success)
	{
	switch (option)
		{
		case THREEPOINTS_t:
			if ((sds_distance(a,pivot) > IC_ZRO) && (sds_distance(c, pivot) > IC_ZRO))
				{
				ic_sub(pivot, a, m_uvec); // m_uvec = a - pivot
				ic_ptcpy(m_pivot, pivot);
				ic_sub(pivot, c, m_vvec); // m_vvec = c - pivot
				if ((ic_veclength(m_uvec)) > IC_ZRO && (ic_veclength(m_vvec) > IC_ZRO))
					success = true;
				else
					success = false;
				}
			else
				{
				success = false;
				}
			break;
		case VECPTVEC_t:
			if ((ic_veclength(a) > IC_ZRO) && (ic_veclength(c) > IC_ZRO))
				{
				ic_ptcpy(m_uvec, a);
				ic_ptcpy(m_pivot, pivot);
				ic_ptcpy(m_vvec, c);
				success = true;
				}
			else
				{
				success = false;
				}
			
			break;
		default:
			break;
		}

	if (success == true)
		{  // begin checking whether m_uvec and m_vvec have a non-zero cross product
		sds_point crossprod;
		ic_crossproduct(m_uvec, m_vvec, crossprod);
		if (ic_veclength(crossprod) < IC_ZRO)
			success = false;

		
		}  //   end checking whether m_uvec and m_vvec have a non-zero cross product
	}

// In this routine, the user can rely that the path abcda traces around the parallelogram
CParallelogram::Vertices(sds_point & a,
						 sds_point & b,
						 sds_point & c,
						 sds_point & d)
	{
	ic_ptcpy(a, m_pivot);
	ic_add(m_pivot, m_uvec, b);
	ic_add(m_pivot, m_vvec, d);
	ic_add(d, m_uvec, c); // we are relying on the fact that this is a parallelogram
	}

double CParallelogram::ulen()
	{
	return ic_veclength(m_uvec);
	}

double CParallelogram::vlen()
	{
	return ic_veclength(m_vvec);
	}

bool CParallelogram::contains(sds_point P)
	{
	bool does_contain = false;

	sds_point deltap;
	ic_sub(P, m_pivot, deltap);

	// Find real numbers x and y such that
	//
	// deltap = x*m_uvec + y*m_vvec
	//
	// If 0 <= x <= 1 and 0 <= y <= 1
	// then this parallelogram does contain P
	// otherwise it does not.

	// Let m_uvec = (a,b)
	//     m_vvec = (c,d)
	//     deltap = (e,f)

	// then (e,f) = x*(a,b) + y*(c,d)
	// solve for x and y.

	// (e,f) = (ax, bx) + (cy, dy)
	//
	// ax + cy = e
	// bx + dy = f

	// det = a*d - b*c

	// if det != 0 then

	// x = (e*d - f*c)/det
	// y = (a*f - b*e)/det

	double a = m_uvec[0];
	double b = m_uvec[1];
	double c = m_vvec[0];
	double d = m_vvec[1];
	double e = deltap[0];
	double f = deltap[1];

	double det = a*d - b*c;

	if (fabs(det) >= IC_ZRO)
		{  // begin applying Cramer's rule
		double x = (e*d - f*c)/det;
		double y = (a*f - b*e)/det;
		if ( (0 <= x) && (x <= 1) && (0 <= y) && (y <= 1))
			does_contain = true;
		else
			does_contain = false;
		}  //   end applying Cramer's rule
	else
		{
		does_contain = false;
		}


	return does_contain;
	}



/****************************************************************************/
/************************ Helper Class CRectangle ***************************/
/****************************************************************************/

CRectangle::CRectangle(double xmin, double ymin, double xmax, double ymax)
	{
	m_xmin = xmin;
	m_ymin = ymin;
	m_xmax = xmax;
	m_ymax = ymax;
	}

CRectangle::CRectangle(sds_point P, sds_point Q)
	{
	if (P[0] < Q[0])
		{
		m_xmin = P[0];
		m_xmax = Q[0];
		}
	else
		{
		m_xmin = Q[0];
		m_xmax = P[0];
		}
	if (P[1] < Q[1])
		{
		m_ymin = P[1];
		m_ymax = Q[1];
		}
	else
		{
		m_ymin = Q[1];
		m_ymax = P[1];
		}
	}

void CRectangle::extents(double & xmin, 
			             double & ymin,
					     double & xmax,
					     double & ymax)
	{
	xmin = m_xmin;
	ymin = m_ymin;
	xmax = m_xmax;
	ymax = m_ymax;
	}

void CRectangle::LL(sds_point & ll)
	{
	ll[0] = m_xmin;
	ll[1] = m_ymin;
	ll[2] = 0.0;
	}

void CRectangle::UR(sds_point & ur)
	{
	ur[0] = m_xmax;
	ur[1] = m_ymax;
	ur[2] = 0.0;
	}

void CRectangle::UL(sds_point & ul)
	{
	ul[0] = m_xmin;
	ul[1] = m_ymax;
	ul[2] = 0.0;
	}

void CRectangle::LR(sds_point & lr)
	{
	lr[0] = m_xmax;
	lr[1] = m_ymin;
    lr[2] = 0.0;
	}

void CRectangle::Vertices(sds_point & a, sds_point & b, sds_point & c, sds_point & d)
	{
	this->LL(a);
	this->LR(b);
    this->UR(c);
	this->UL(d);
	}

bool CRectangle::contains(double x, double y)
	{
	bool inside;

	if ((m_xmin <= x) && (x <= m_xmax) && (m_ymin <= y) && (y <= m_ymax))
		inside = true;
	else
		inside = false;
	
	return inside;
	}

// this rectangle contains P if P is contained inside or on the boundary of this rectangle
bool CRectangle::contains(sds_point P)
	{

	return this->contains(P[0], P[1]);

	}

// this rectangle contains that if LL and UR of that are contained inside or on the boundary
// of this rectangle.
bool CRectangle::contains(CRectangle & that)
	{

	bool inside;
	sds_point ll;
	sds_point ur;

	that.LL(ll);
	that.UR(ur);

	if (this->contains(ll) && this->contains(ur))
		inside = true;
	else
		inside = false;

	return inside;

	}

// this rectangle contains that Parallelogram if all four vertices of that are contained 
// inside or on the boundary
// of this rectangle.
bool CRectangle::contains(CParallelogram & that)
	{

	bool inside;
	sds_point a,b,c,d;

	that.Vertices(a,b,c,d);

	if (this->contains(a) && 
		this->contains(b) && 
		this->contains(c) && 
		this->contains(d))
		inside = true;
	else
		inside = false;

	return inside;

	}

bool CRectangle::disjoint(CRectangle & that)
	{
	bool are_disjoint;


	if ((this->m_xmax < that.m_xmin) || 
		(this->m_xmin > that.m_xmax) ||
		(this->m_ymax < that.m_ymin) ||
		(this->m_ymin > that.m_ymax))
		are_disjoint = true;
	else
		are_disjoint = false;

	return are_disjoint;
	}

bool CRectangle::intersects(CRectangle & that)
	{
	return !(this->disjoint(that));
	}

bool CRectangle::intersects(CParallelogram & that)
	{

    sds_point P[5];
	sds_point R[5];
	int i;
	int j;

	// check whether any of that parallelogram's vertices are inside this rectangle

	that.Vertices(P[0], P[1], P[2], P[3]);

	for (i = 0; i < 4; i++)
		if (this->contains(P[i]))
			return true;

	// check whether any of this rectangle's vertices are inside that parallelogram
    this->Vertices(R[0], R[1], R[2], R[3]);

	for (i = 0; i < 4; i++)
		if (that.contains(R[i]))
			return true;

	// if we have gotten this far, do the 16 line-line intersections.

    // for ease of bookkeeping do the following
    ic_ptcpy(P[4], P[0]);
	ic_ptcpy(R[4], R[0]);

	double intstatus; // 
	sds_point xpt;

	for (i = 0; i < 4; i++)
		{   // begin i-loop
		for (j = 0; j < 4; j++)
			{   // begin j-loop

			// if line(P[i], P[i+1]) intersects line(R[j], R[j+1]) return true
			intstatus = ic_linexline(P[i], P[i+1],
				                     R[j], R[j+1],
									 xpt);

			if (intstatus == 3) // this means xpt is on both lines
				return true;


			}   //   end j-loop
		}   //   end i-loop


// if we have gotten this far, the rectangle and parallelogram do not intersect
	return false;
	}

bool CRectangle::disjoint(CParallelogram & that)
	{
	return !(this->intersects(that));
	}



/****************************************************************************/
/************************** Helper Class CCircle ****************************/
/****************************************************************************/

CCircle::CCircle(sds_point center, double radius)
	{
	ic_ptcpy(m_center, center);
	m_radius = radius;
	}

void CCircle::extents(double & xmin,
					  double & ymin,
					  double & xmax,
					  double & ymax)
	{
	xmin = m_center[0] - m_radius;
	ymin = m_center[1] - m_radius;
	xmax = m_center[0] + m_radius;
	ymax = m_center[1] + m_radius;
	}

bool CCircle::contains(sds_point P)
	{
	bool does_contain;

	if (sds_distance(P, m_center) <= m_radius)
		does_contain = true;
	else
		does_contain = false;

	return does_contain;
	}

// A circle contains a rectangle if and only
// if it contains all four of its vertices
bool CCircle::contains(CRectangle & that)
	{
	bool does_contain;

	sds_point ll;
	sds_point lr;
	sds_point ul;
	sds_point ur;

	that.LL(ll);
	that.LR(lr);
	that.UL(ul);
	that.UR(ur);

	if (this->contains(ll) &&
		this->contains(lr) &&
		this->contains(ul) &&
		this->contains(ur))
		does_contain = true;
	else
		does_contain = false;

	return does_contain;
	}

// A circle contains a parallelogram if and only
// if it contains all four of its vertices
bool CCircle::contains(CParallelogram & that)
	{
	bool does_contain;

	sds_point a;
	sds_point b;
	sds_point c;
	sds_point d;

	that.Vertices(a,b,c,d);

	if (this->contains(a) &&
		this->contains(b) &&
		this->contains(c) &&
		this->contains(d))
		does_contain = true;
	else
		does_contain = false;

	return does_contain;
	}


bool CCircle::disjoint(CRectangle &that)
	{
	return !(this->intersects(that));
	}

bool CCircle::intersects(CRectangle & that)
	{
	bool does_intersect = false; // guilty until proven innocent
	
	double       rectangle_xmin, rectangle_ymin, rectangle_xmax, rectangle_ymax;
	that.extents(rectangle_xmin, rectangle_ymin, rectangle_xmax, rectangle_ymax);
	
	double xcenter = this->m_center[0];
	double ycenter = this->m_center[1];
	double radius = this->m_radius;
	
	if ((rectangle_xmin <= xcenter) && (xcenter <= rectangle_xmax))
		{
		if ((rectangle_ymin - radius <= ycenter) && (ycenter <= rectangle_ymax + radius))
			does_intersect = true;
		}
	else if ((rectangle_ymin <= ycenter) && (ycenter <= rectangle_ymax))
		{
		if ((rectangle_xmin - radius <= xcenter) && (xcenter <= rectangle_xmax + radius))
			does_intersect = true;
		}
	else
		{  // begin trying the corners
		sds_point ll, lr, ul, ur;
		that.LL(ll); that.LR(lr); that.UL(ul); that.UR(ur);
		if ((sds_distance(ll, m_center) <= m_radius) ||
			(sds_distance(lr, m_center) <= m_radius) ||
			(sds_distance(ul, m_center) <= m_radius) ||
			(sds_distance(ur, m_center) <= m_radius))
			does_intersect = true;
		}  //   end trying the corners
	
	return does_intersect;
	}

bool CCircle::intersects(CParallelogram & that)
	{

    sds_point P[5];

	int i;

	// check whether any of that parallelogram's vertices are inside this circle

	that.Vertices(P[0], P[1], P[2], P[3]);

	for (i = 0; i < 4; i++)
		if (this->contains(P[i]))
			return true;

	// check whether the center of the circle is inside that parallelogram

		if (that.contains(m_center))
			return true;

	// if we have gotten this far, do the 4 line-circle intersections.

    // for ease of bookkeeping do the following
    ic_ptcpy(P[4], P[0]);

	// to be done ... do the 4 line-circle intersections
	double start = 0.0;
	double end = IC_TWOPI;
	sds_point i0;
	sds_point i1;
	short rc;
	for (i = 0; i < 4; i++)
		{   // begin i-loop
		rc = ic_linexarc(P[i], P[i+1],
			             m_center, m_radius,
						 start, end,
						 i0, i1);

		if ( IC_ARCINT_FIRSTINT_ON_BOTH(rc) || IC_ARCINT_SECONDINT_ON_BOTH(rc) )
			return true;

		}   //   end i-loop



// if we have gotten this far, the circle and parallelogram do not intersect
	return false;
	}

bool CCircle::disjoint(CParallelogram & that)
	{
	return !(this->intersects(that));
	}



/****************************************************************************/
/*************************** Helper Class CFence ****************************/
/****************************************************************************/

// construct a CFence object from the resbufs

CFence::CFence(struct resbuf * pt)
	{
	struct resbuf * trb;

	m_numPts = 0;
	m_pt = NULL;

	trb = pt;
	while ( (trb != NULL) && (trb->restype==RT3DPOINT))
		{  // begin while loop
		   ++m_numPts;
		   trb= trb->rbnext;
		}  //   end while loop

	if (m_numPts > 0)
		m_pt = new sds_point[m_numPts];

	trb = pt;
	int i = 0;
	while ( (trb != NULL) && (trb->restype==RT3DPOINT))
		{  // begin while loop
		   ic_ptcpy(m_pt[i], trb->resval.rpoint);
		   trb = trb->rbnext;
		   i = i + 1;
		}  //   end while loop

	}

CFence::~CFence()
	{
	delete [] m_pt;
	}

bool CFence::crosses(CParallelogram & that)
	{

	// check whether any of the lines in the fence cross any of the edges of
	// the parallelogram

	int intstatus;
    int i,j;
	sds_point P[5];
	sds_point xpt;


	that.Vertices(P[0], P[1], P[2], P[3]);
	ic_ptcpy(P[4], P[0]);

	for (i = 0; i < m_numPts - 1; i++)
		{   // begin i-loop
		for (j = 0; j < 4; j++)
			{   // begin j-loop
			intstatus = ic_linexline(m_pt[i], m_pt[i+1], P[j], P[j+1], xpt);
			if (intstatus == 3)
				return true;
			}   //   end j-loop
		}   //   end i-loop


	// if we have gotten this far, the fence does not cross the parallelogram
	return false;
	}



/****************************************************************************/
/*************************** Helper Class CPolygon **************************/
/****************************************************************************/

CPolygon::CPolygon(gr_selectorlink * guts)
	{
	m_guts = guts;
	}


bool CPolygon::contains(CParallelogram & that)
	{
	
	// get the vertices of the parallelogram
	sds_point V[5]; 
	that.Vertices(V[0], V[1], V[2], V[3]);
	
	int i;
	
	short inside;
	
	// if any of the vertices are not contained inside the
	// polygon we can return false immediately
	for (i = 0; i < 4; i++)
		{
		inside = ic_inorout(m_guts->def, 
			m_guts->extent[0][0],
			m_guts->extent[0][1],
			m_guts->extent[1][0],
			m_guts->extent[1][1],
			V[i][0], V[i][1]);
		
		if (inside != 1)
			return false;
		
		}

	// if we have gotten this far, then all the vertices are
	// contained inside the polygon, but it may still be the
	// case that one of the edges of the parallelogram crosses
	// the polygon, so let's check for that.
	ic_ptcpy(V[4], V[0]);

	ic_pointsll * tpp1;
	ic_pointsll * tpp2;
	sds_point xpt;
	short intcode;

	// the following is similar to what is done in GR\selectedent.cpp
	// except here we are using ic_linexline to do the intersection
	// rather than gr_segxseg. The function gr_segxseg is faster, but
	// it is not externally callable.
	for (tpp1 = m_guts->def; tpp1 != NULL; tpp1= tpp1->next)
		{  // begin tpp1 loop
		tpp2=(tpp1->next==NULL) ? m_guts->def : tpp1->next;
		
		for (i = 0; i < 4; i++)
			{  // begin i-loop
			intcode = ic_linexline(tpp1->pt,
			             tpp2->pt,
						 V[i],
						 V[i+1],
						 xpt);
			
			if (intcode == 3)
				return false;
			}  //   end i-loop
		
		}  //   end tpp1 loop

	// if we have gotten this far, the polygon contains the parallelogram
	
	return true;
	}

// This function will return true if that CParallelogram is contained within
// or crosses the boundary of this CPolygon.  
bool CPolygon::intersects(CParallelogram & that)
	{
	// get the vertices of the parallelogram
	sds_point V[5];
	that.Vertices(V[0], V[1], V[2], V[3]);

	int i;

	short inside;



	// if any of the vertices of the parallelogram are contained inside the polygon, 
	// we can return true immediately

	for (i = 0; i < 4; i++)
		{
		inside = ic_inorout(m_guts->def, 
			m_guts->extent[0][0],
			m_guts->extent[0][1],
			m_guts->extent[1][0],
			m_guts->extent[1][1],
			V[i][0], V[i][1]);
		
		if (inside == 1)
			return true;
		}

	// similarly...
    // if any of the vertices of the polygon are contained inside the parallelogram,
	// we can return true immediately

	ic_pointsll * tpp1;
	ic_pointsll * tpp2;

	for (tpp1 = m_guts->def; tpp1 != NULL; tpp1= tpp1->next)
		{  // begin tpp1 loop
		if (that.contains(tpp1->pt))
			return true;
		}  //   end tpp1 loop



	// next..
	// if any of the edges of the polygon intersect any of the edges of the
	// parallelogram, we can return true immediately
	ic_ptcpy(V[4], V[0]);

	sds_point xpt;
	short intcode;

	for (tpp1 = m_guts->def; tpp1 != NULL; tpp1= tpp1->next)
		{  // begin tpp1 loop
		tpp2=(tpp1->next==NULL) ? m_guts->def : tpp1->next;
		
		for (i = 0; i < 4; i++)
			{  // begin i-loop
			intcode = ic_linexline(tpp1->pt,
			             tpp2->pt,
						 V[i],
						 V[i+1],
						 xpt);
			
			if (intcode == 3)
				return true;
			}  //   end i-loop
		
		}  //   end tpp1 loop



    // finally...
	// if we have gotten this far, the polygon and the parallelogram do not intersect.


	return false;
	}

bool CPolygon::disjoint(CParallelogram & that)
	{
	return !(this->intersects(that));
	}

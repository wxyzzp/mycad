/* \PRJ\ICAD\ICADHELPERS.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *	helper enums and classes used in icadapi.cpp and sds_osnap.cpp
 *	classes are defined in icadapi.cpp
 */

/****************************************************************************/
/************************ Helper Enum PickType  *****************************/
/****************************************************************************/
struct gr_selectorlink;

enum PickType{WINDOW_t,
              CROSSING_t,
			  OUTSIDE_t,
			  WCIRCLE_t,
			  OCIRCLE_t,
			  CCIRCLE_t,
			  POINT_t,
			  FENCE_t,
			  WPOLYGON_t,
			  OPOLYGON_t,
			  CPOLYGON_t,
			  X_t,
			  OTHER_t};


// we could have been fancier and created an abstract class CConvexPolygon
// from which CParallelogram and CRectangle are inherited.

/****************************************************************************/
/********************* Helper Class CParallelogram **************************/
/****************************************************************************/
enum CreationOption{THREEPOINTS_t,
			        VECPTVEC_t	
					};

class CParallelogram
	{

	public:

	CParallelogram(sds_point P,
				   sds_point Q);

	CParallelogram(CreationOption option,
		              sds_point a,
	                  sds_point pivot,
					  sds_point c,
					  bool & success);

	Vertices(sds_point & a,
		     sds_point & b,
			 sds_point & c,
			 sds_point & d);

	double ulen();

	double vlen();

	bool contains(double x, double y); // may not need this

	bool contains(sds_point P);

	private:

		// the rectangle is the set of all points of the form
		// {P = m_pivot + u*m_uvec + v*m_vvec : 0 <= u <= 1 and 0 <= v <= 1 }
		sds_point m_uvec;// a non-normalized u vector
		sds_point m_pivot; // one of the corner points
		sds_point m_vvec; // a non-normalized v vector
	};


/****************************************************************************/
/************************ Helper Class CRectangle ***************************/
/****************************************************************************/
class CRectangle
	{
	public:

		CRectangle(double xmin, double ymin, double xmax, double ymax);
		CRectangle(sds_point P, sds_point Q);

		void extents(double & xmin, 
			         double & ymin,
					 double & xmax,
					 double & ymax);

        void LL(sds_point & ll);
		void UR(sds_point & ur);
		void UL(sds_point & ul);
		void LR(sds_point & lr);

		void Vertices(sds_point & a, sds_point & b, sds_point & c, sds_point & d);

		bool contains(double x, double y);

		bool contains(sds_point P);

		bool contains(CRectangle & that);

		bool disjoint(CRectangle & that);

		bool intersects(CRectangle & that);

		bool contains(CParallelogram & that);

		bool disjoint(CParallelogram & that);

		bool intersects(CParallelogram & that);


	private:
		
		double m_xmin;
		double m_ymin;
		double m_xmax;
		double m_ymax;
	};


/****************************************************************************/
/************************** Helper Class CCircle ****************************/
/****************************************************************************/

class CCircle
	{
	public:
		CCircle(sds_point center, double radius);

		void extents(double & xmin, 
			         double & ymin,
					 double & xmax,
					 double & ymax);

		bool contains(sds_point P);

		bool contains(CRectangle & that);

		bool disjoint(CRectangle & that);

		bool intersects(CRectangle & that);

		bool contains(CParallelogram & that);

		bool disjoint(CParallelogram & that);

		bool intersects(CParallelogram & that);


	public:
		sds_point m_center;
		double m_radius;
	};


/****************************************************************************/
/*************************** Helper Class CFence ****************************/
/****************************************************************************/

class CFence
	{
	public:
		CFence(struct resbuf * pt);
		~CFence();

		bool crosses(CParallelogram & that);


	private: 
		int m_numPts;
		sds_point * m_pt;
	};


/****************************************************************************/
/*************************** Helper Class CPolygon **************************/
/****************************************************************************/

// This is essentially a wrapper class for functionality already available in GR

class CPolygon
	{
	public:
		CPolygon(gr_selectorlink * guts);

		bool contains(CParallelogram & that);

		bool disjoint(CParallelogram & that);

		bool intersects(CParallelogram & that);


	private:
		gr_selectorlink * m_guts;
	};

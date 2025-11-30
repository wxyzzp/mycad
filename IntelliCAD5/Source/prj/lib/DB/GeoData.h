// File  : 
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _GEODATA_H_
#define _GEODATA_H_

#undef T
#undef X
#undef Y
#undef Z
#include "DbExport.h"
#include "xstd.h"
#include "gvector.h"
#include "db.h"

namespace geodata
{
    enum
    {
        eRay,
        eLine,
        ePline,
        eEllipse,
        eISpline,
        eASpline
    };

    struct DB_CLASS curve
    {
		curve() : m_pDbEntity(NULL) {}

		// get the normal direction, start and end points and return true if they exist, 
		// and false otherwise. 
		// 
		virtual bool getStartPoint(icl::point &start) const { return false; }
		virtual bool getEndPoint(icl::point &end) const { return false; }
		virtual bool getNormal(icl::gvector &normal) const { return false; }
		virtual bool isClosed() const { return false; }

        virtual int type() const = 0;

		db_handitem*	m_pDbEntity;
    };

    struct DB_CLASS ray: public curve
    {
        icl::point m_root;
        icl::gvector m_dir;

		virtual bool getStartPoint(icl::point &start) const { start = m_root; return true; }

        virtual int type() const {return eRay;}

        extract(db_ray&);
        extract(db_xline&);
    };
    struct DB_CLASS line: public curve
    {
        icl::point m_start;
        icl::point m_end;

		virtual bool getStartPoint(icl::point &start) const { start = m_start; return true; }
		virtual bool getEndPoint(icl::point &end) const { end = m_end; return true; }
        
		virtual int type() const {return eLine;}

        int extract(db_line&);
    };
    struct DB_CLASS pline: public curve
    {
        xstd<icl::point>::vector m_pts;
        xstd<double>::vector m_bulges;
        icl::gvector m_normal;

        //eClosed (eMClosed) - This is a closed polyline (or a polygon mesh closed in the M direction)
        //eCurveFit - Curve-fit vertices have been added.
        //eSplineFit - Spline-fit vertices have been added.
        //e3dPolyline - This is a 3D polyline.
        //ePolygonMesh - This is a 3D polygon mesh.
        //eNClosed - The polygon mesh is closed in the N direction.
        //ePolyfaceMesh - The polyline is a polyface mesh.
        //eLinetype - The linetype pattern is generated continuously around the vertices of this polyline.
        enum
        {
            eClosed = 1<<0,
            eMClosed = eClosed,
            eCurveFit = 1<<1,
            eSplineFit = 1<<2,
            e3dPolyline = 1<<3,
            ePolygonMesh = 1<<4,
            eNClosed = 1<<5,
            ePolyfaceMesh = 1<<6,
            eLinetype = 1<<7
        };
        int m_flags;

		virtual bool getStartPoint(icl::point &start) const;
		virtual bool getEndPoint(icl::point &end) const;
		virtual bool getNormal(icl::gvector &norm) const { norm = m_normal; return true; }        
		virtual bool isClosed() const { return (m_flags & eClosed) ? true : false; }
		
		virtual int type() const {return ePline;}

        int extract(db_polyline&);
        int extract(db_lwpolyline&);
    };
    struct DB_CLASS ellipse: public curve
    {
        icl::point m_center;
        icl::gvector m_normal;
        icl::gvector m_major_axis;
        double m_ratio;
        double m_start;
        double m_end;

        icl::point m_start_pt;
        icl::point m_end_pt;

		virtual bool getStartPoint(icl::point &start) const { start = m_start_pt; return true; }
		virtual bool getEndPoint(icl::point &end) const { end = m_end_pt; return true; }
		virtual bool getNormal(icl::gvector &norm) const { norm = m_normal; return true; }        
		virtual bool isClosed() const { return icadAngleEqual(m_start, m_end); }

        virtual int type() const {return eEllipse;}

        int extract(db_arc&);
        int extract(db_circle&);
        int extract(db_ellipse&);

		void calculateEndPoints();
    };
    struct DB_CLASS spline: public curve
    {
        icl::gvector m_normal;
        int m_degree;

        enum
        {
            eClosed = 1<<0,
            ePeriodic = 1<<1,
            eRational = 1<<2,
            ePlanar = 1<<3,
            eLinear = 1<<4
        };
        int m_flags;

		virtual bool getNormal(icl::gvector &norm) const { norm = m_normal; return true; } 
		virtual bool isClosed() const { return (m_flags & eClosed) ? true : false; }
    };

    struct DB_CLASS ispline: public spline
    {
        xstd<icl::point>::vector m_fpts;
        icl::gvector m_start_tang;
        icl::gvector m_end_tang;
        double m_fittol;

		virtual bool getStartPoint(icl::point &start) const;
		virtual bool getEndPoint(icl::point &end) const;
        virtual int type() const {return eISpline;}

        int extract(db_spline&);
    };

    struct DB_CLASS aspline: public spline
    {
        xstd<icl::point>::vector m_cpts;
        xstd<double>::vector m_knots;
        xstd<double>::vector m_weights;
        double m_knottol;
        double m_cpttol;

        icl::point m_start;
        icl::point m_end;

		virtual bool getStartPoint(icl::point &start) const { start = m_start; return true; }
		virtual bool getEndPoint(icl::point &end) const { end = m_end; return true; }
        virtual int type() const {return eASpline;}

        int extract(db_spline&);
		int calculateEndPoints();
    };

    struct DB_CLASS cs
    {
        icl::point m_o;
        icl::gvector m_x;
        icl::gvector m_y;
        icl::gvector m_z;

        cs();
        cs(const icl::gvector&);
        cs
        (
        const icl::gvector&, 
        const icl::gvector&, 
        const icl::gvector&,
        const icl::point&
        );

        icl::point to_wcs(const icl::point&) const;
        icl::point from_wcs(const icl::point&) const;
    };

    DB_API int extract(db_handitem&, curve*&);
};

#ifndef _DEBUG
#include "GeoData.inl"
#endif

#endif

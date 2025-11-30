// File  : 
// Author: Alexey Malov
#include "stdafx.h"
#include "AsMesh.h"
#include "AsTools.h"
#include <algorithm>
#include "gvector.h"
using icl::gvector;
#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif


// statics
template <typename Bottom, typename Top>
static 
int check
(
Bottom& bottom,
Top& top
);
static int check
(
std::vector<point>& points,
std::vector<int4>& faces
);

enum {eInside, eOutside};
static
int orientation
(
const std::vector<point>& points,
const int4& face,
const point& pt
);

static 
void reverse
(
std::vector<int4>& faces
);

void CAsSimpleIndexedMesh::transform(const SPAtransf& tr)
{
    for (std::vector<::node>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
    {
        *reinterpret_cast<SPAposition*>(i) *= tr;
    }
}

void CAsSimpleIndexedMesh::makeInvisibleEdges()
{
    for (int i = 0, n = m_faces.size(); i < n; i++)
    {
        const int4& face = m_faces[i];
        for (int j = 0, m = face.size(); j < m; j++)
        {
            int icurr = face[j];
            if (icurr < 0)
                continue;

            int inext = abs(face[(j+1)%m]);
            node& no = m_nodes[icurr-1];
            for (int k = 0, q = no.m_faces.size(); k < q; k++)
            {
                if (no.m_faces[k] == i)
                    continue;

                int4& _face = m_faces[no.m_faces[k]];
                for (int l = 0, p = _face.size(); l < p; l++)
                {
                    int _icurr = abs(_face[l]);
                    int _inext = abs(_face[(l+1)%p]);
                    if ((_icurr == icurr && _inext == inext) ||
                        (_icurr == inext && _inext == icurr))
                    {
                        _face[l] = -_icurr;
                        goto next;
                    }
                }
            }
next:;
        }
    }
}

int mesh::wedge
(
const point& p1,
const point& p2,
double h,
std::vector<point>& points,
std::vector<int4>& faces
)
{
    points.resize(6);

	gvector d = p2 - p1;
	gvector xdir(d.x(), 0., 0.);
	gvector ydir(0., d.y(), 0.);
	gvector zdir = xdir*ydir; zdir.normalize(); zdir*=h;
	if (h > 0.0) {
		if (d.x() * d.y() > 0.0) {
			points[0] = p1;
			points[1] = points[0] + xdir;
			points[2] = p2;
			points[3] = points[0] + ydir;
			points[4] = points[0] + zdir;
			points[5] = points[3] + zdir;
		}
		else { 
			points[3] = p1;
			points[0] = points[3] + ydir;
			points[2] = points[3] + xdir;
			points[1] = p2;
			points[4] = points[0] - zdir;
			points[5] = points[3] - zdir;
		}
	}
	else { // h < 0.0
		if (d.x() * d.y() < 0.0) {
			points[0] = p1;
			points[1] = points[0] + xdir;
			points[2] = p2;
			points[3] = points[0] + ydir;
			points[4] = points[0] - zdir;
			points[5] = points[3] - zdir;
		}
		else {
			points[3] = p1;
			points[0] = points[3] + ydir;
			points[2] = points[3] + xdir;
			points[1] = p2;
			points[4] = points[0] + zdir;
			points[5] = points[3] + zdir;
		}
	}

    faces.resize(5);
    faces[0][0] = 1; faces[0][1] = 4; faces[0][2] = 3; faces[0][3] = 2;
    faces[1][0] = 1; faces[1][1] = 5; faces[1][2] = 6; faces[1][3] = 4;
    faces[2][0] = 1; faces[2][1] = 2; faces[2][2] = 5; faces[2][3] = 0;
    faces[3][0] = 4; faces[3][1] = 6; faces[3][2] = 3; faces[3][3] = 0;
    faces[4][0] = 2; faces[4][1] = 3; faces[4][2] = 6; faces[4][3] = 5;

    return 1;
}

int mesh::pyramid
(
const npoint<4>& bottom,
const npoint<2>& top,
std::vector<point>& points,
std::vector<int4>& faces
)
throw (exception)
{
	if (!check(bottom, top))
		throw exception();

    int i;
    int ntop = 2;
    if (top[0] == top[1])
        ntop = 1;

    points.resize(4+ntop);
    for (i = 0; i < 4; i++)
        points[i] = bottom[i];
    for (i = 0; i < ntop; i++)
        points[i+4] = top[i];

    faces.resize(5);
    faces[0][0] = 1; faces[0][1] = 4; faces[0][2] = 3; faces[0][3] = 2;

    if (ntop == 2)
    {
        faces[1][0] = 4; faces[1][1] = 1; faces[1][2] = 5; faces[1][3] = 0;
        faces[2][0] = 1; faces[2][1] = 2; faces[2][2] = 6; faces[2][3] = 5;
        faces[3][0] = 2; faces[3][1] = 3; faces[3][2] = 6; faces[3][3] = 0;
        faces[4][0] = 3; faces[4][1] = 4; faces[4][2] = 5; faces[4][3] = 6;
    }
    else
    {
        faces[1][0] = 4; faces[1][1] = 1; faces[1][2] = 5; faces[1][3] = 0;
        faces[2][0] = 1; faces[2][1] = 2; faces[2][2] = 5; faces[2][3] = 0;
        faces[3][0] = 2; faces[3][1] = 3; faces[3][2] = 5; faces[3][3] = 0;
        faces[4][0] = 3; faces[4][1] = 4; faces[4][2] = 5; faces[4][3] = 0;
    }

    if (orientation(points, faces[0], top[0]) == eInside)
    {
        reverse(faces);
    }

	if (!check(points, faces))
		throw exception();

	return 1;
}

int mesh::pyramid
(
const npoint<4>& bottom,
const npoint<4>& top,
std::vector<point>& points,
std::vector<int4>& faces
)
throw (exception)
{
    if (!check(bottom, top))
        throw exception();

    int i;

    points.resize(8);
    for (i = 0; i < 4; i++)
        points[i] = bottom[i];
    for (i = 0; i < 4; i++)
        points[i+4] = top[i];

    faces.resize(6);
    faces[0][0] = 1; faces[0][1] = 4; faces[0][2] = 3; faces[0][3] = 2;
    faces[1][0] = 5; faces[1][1] = 6; faces[1][2] = 7; faces[1][3] = 8;
    faces[2][0] = 1; faces[2][1] = 2; faces[2][2] = 6; faces[2][3] = 5;
    faces[3][0] = 2; faces[3][1] = 3; faces[3][2] = 7; faces[3][3] = 6;
    faces[4][0] = 3; faces[4][1] = 4; faces[4][2] = 8; faces[4][3] = 7;
    faces[5][0] = 4; faces[5][1] = 1; faces[5][2] = 5; faces[5][3] = 8;

    if (orientation(points, faces[0], top[0]) == eInside)
        reverse(faces);
    
    if (!check(points, faces))
		throw exception();

	return 1;
}

int mesh::pyramid
(
const npoint<3>& bottom,
const npoint<3>& top,
std::vector<point>& points,
std::vector<int4>& faces
)
throw (exception)
{
    if (!check(bottom, top))
        throw exception();

    int i;

    points.resize(6);
    for (i = 0; i < 3; i++)
        points[i] = bottom[i];
    for (i = 0; i < 3; i++)
        points[i+3] = top[i];

    faces.resize(5);
    faces[0][0] = 1; faces[0][1] = 3; faces[0][2] = 2; faces[0][3] = 0;
    faces[1][0] = 4; faces[1][1] = 5; faces[1][2] = 6; faces[1][3] = 0;
    faces[2][0] = 1; faces[2][1] = 2; faces[2][2] = 5; faces[2][3] = 4;
    faces[3][0] = 2; faces[3][1] = 3; faces[3][2] = 6; faces[3][3] = 5;
    faces[4][0] = 3; faces[4][1] = 1; faces[4][2] = 4; faces[4][3] = 6;
    
    if (orientation(points, faces[0], top[0]) == eInside)
        reverse(faces);

    if (!check(points, faces))
		throw exception();

	return 1;
}

int mesh::pyramid
(
const npoint<3>& bottom,
const npoint<1>& top,
std::vector<point>& points,
std::vector<int4>& faces
)
throw (exception)
{
    points.resize(4);
    points[0] = bottom[0];
    points[1] = bottom[1];
    points[2] = bottom[2];
    points[3] = top[0];

    faces.resize(4);
    faces[0][0] = 1; faces[0][1] = 3; faces[0][2] = 2; faces[0][3] = 0;
    faces[1][0] = 1; faces[1][1] = 2; faces[1][2] = 4; faces[1][3] = 0;
    faces[2][0] = 2; faces[2][1] = 3; faces[2][2] = 4; faces[2][3] = 0;
    faces[3][0] = 3; faces[3][1] = 1; faces[3][2] = 4; faces[3][3] = 0;

    if (orientation(points, faces[0], top[0]) == eInside)
        reverse(faces);

	return 1;
}

static
int orientation
(
const std::vector<point>& points,
const int4& face,
const point& pt
)
{
    assert(face.size() > 2);

    SPAvector normal = (points[face[1]-1] - points[face[0]-1])*(points[face[2]-1] - points[face[1]-1]);
    SPAvector dir = pt - points[face[0]-1];

    if (normal % dir > SPAresabs)
        return eInside;
    else
        return eOutside;
}

template <typename Bottom, typename Top>
static
int check
(
Bottom& bottom,
Top& top
)
{
    assert(bottom.size() > 2);

    gvector normal = (bottom[1] - bottom[0])*(bottom[2] - bottom[1]);

    const double d = normal % (top[0] - bottom[0]);
    for (int i = 1; i < top.size(); i++)
    {
        if (d * (normal % (top[i] - bottom[0])) < SPAresabs)
            return 0;
    }
    return 1;
}

static int check
(
std::vector<point>& points,
std::vector<int4>& faces
)
{
	for (int f = 0, nf = faces.size(); f < nf; f++)
	{
		const int4& face = faces[f];
		if (face.size() > 3)
		{
			gvector a = points[face[1]-1] - points[face[0]-1];
			gvector b = points[face[2]-1] - points[face[0]-1];
			gvector c = points[face[3]-1] - points[face[0]-1];

			if (!icl::perpendicular(a*b, c))
				return 0;
		}
	}
	return 1;
}

static 
void reverse
(
std::vector<int4>& faces
)
{
    for (int i = 0, n = faces.size(); i < n; i++)
    {
        int4& face = faces[i];
        for (int j = 0, m = face.size(); j < m/2; j++)
        {
            int t = face[j]; face[j] = face[m-j-1]; face[m-j-1] = t;
        }
    }
}

// File  :
// Author: Alexey Malov

namespace geodata
{

inline
bool pline::getStartPoint(icl::point &start) const 
{ 
	if (m_pts.size() > 0) {
		start = m_pts[0];
		return true; 
	}
	else
		return false;
}
		
inline 
bool pline::getEndPoint(icl::point &end) const 
{ 
	if (m_pts.size() > 0) {
		end = m_pts[m_pts.size() - 1];
		return true; 
	}
	else
		return false;
}

inline
bool ispline::getStartPoint(icl::point &start) const 
{ 
	if (m_fpts.size() > 0) {
		start = m_fpts[0];
		return true; 
	}
	else
		return false;
}
		
inline 
bool ispline::getEndPoint(icl::point &end) const 
{ 
	if (m_fpts.size() > 0) {
		end = m_fpts[m_fpts.size() - 1];
		return true; 
	}
	else
		return false;
}

inline
cs::cs()
:
m_o(0., 0., 0.),
m_x(0., 0., 0.),
m_y(0., 0., 0.),
m_z(0., 0., 0.)
{}

inline
cs::cs
(
const icl::gvector& x,
const icl::gvector& y,
const icl::gvector& z,
const icl::point& o
)
:
m_o(o),
m_x(x),
m_y(y),
m_z(z)
{}

}

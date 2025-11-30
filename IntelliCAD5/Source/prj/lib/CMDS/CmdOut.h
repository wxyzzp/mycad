#pragma once
#ifndef _CMDOUT_H_
#define _CMDOUT_H_

#include <string>
#include "sds.h"
#include "cmds.h"
#include "gvector.h"
#include <math.h>

namespace cmdout
{
struct description
{
	const char* m_descr;
	
	description(const char* s):
	m_descr(s){}
};

struct real
{
	sds_real m_value;

	real(sds_real v):
	m_value(v){}
};

struct area
{
	sds_real m_area;

	area(sds_real r):
	m_area(r){}
};

struct scale
{
	sds_real m_scale;

	scale(sds_real r):
	m_scale(r){}
};

struct angle
{
	sds_real m_angle;

	angle(sds_real r):
	m_angle(r){}
};

struct point2d
{
	const icl::point& m_pt;

	point2d(const icl::point& p):
	m_pt(p){}
};
}

/*-------------------------------------------------------------------------*//**
Util class, helping to print messages to console, with handling the scrolling.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdOut
{
public:
	// is scrolling enabling?
	bool m_bScroll;
	// maximum stored strings in history
	int m_nMaxTrackedHistory;
	// number of string, located on one page
	int m_nStringsPerPage;
	// current position on the page
	int m_iCurrPos;

	enum
	{
		eGood = 0,
		eBad = 1<<0
	};
	int m_state;

public:
	CCmdOut(bool bScroll = true);

	void openPromptWindow();
	void closePromptWindow();

	bool good()
	{
		return m_state == eGood;
	}
	bool bad()
	{
		return (m_state & eBad) != 0;
	}

	CCmdOut& operator << (CCmdOut& (__cdecl *f)(CCmdOut&))
	{
		return (*f)(*this); 
	}
	friend CCmdOut& operator << (CCmdOut&, const std::string&);
	friend CCmdOut& operator << (CCmdOut&, const char*);
	friend CCmdOut& endl(CCmdOut&);

protected:
	bool isInteractive();
	void flush();
};

inline
sds_real value0(const sds_real& r)
{
	return (fabs(r) < CMD_FUZZ)? 0.: r;
}

template <typename _Os>
_Os& operator << 
(
_Os& o,
const cmdout::real& r
)
{
	char rval[30];
	sds_rtos(value0(r.m_value), -1, -1, rval);
		   
	o << rval;
	return o;
}

template <typename _Os>
_Os& operator << 
(
_Os& o,
const sds_real& r
)
{
	char rval[30];
	sds_rtos(value0(r), -1, -1, rval);
		   
	o << rval;
	return o;
}

template <typename _Os> 
_Os& operator <<
(
_Os& o,
int i
)
{
	char buf[33];
	char* p = itoa(i, buf, 10);
	if (p != NULL)
		o << p;

	return o;
}

template <typename _Os> 
_Os& operator << 
(
_Os& o,
long l
)
{
	char buf[33];
	char* p = ltoa(l, buf, 10);
	if (p != NULL)
		o << p;

	return o;
}

template <typename _Os> 
_Os& operator <<
(
_Os& o,
const cmdout::description& d
)
{
	if (d.m_descr != NULL)
	{
		char buf[IC_ACADNMLEN];
		sprintf(buf, "%25s", d.m_descr);
		o << endl << buf;
	}

	return o;
}

template <typename _Os> 
_Os& operator << 
(
_Os& o, 
const cmdout::area& a
)
{
	//if very very near 0.0, print 0.0
	sds_real r = value0(a.m_area);

    char rval[50], buf[IC_ACADNMLEN];
    resbuf rb;
	SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	
	if (rb.resval.rint == 3 || rb.resval.rint == 4)
	{
        cmd_rtos_area(r,-1,-1,rval);
		sprintf(buf, ResourceString(IDC_CMDS9____S_SQ__87, "  %s sq." ), rval);
    }
	else
	{
        sds_rtos(r,-1,-1,rval);
		sds_printf(buf, "  %s"/*DNT*/, rval);
    }
	
	o << buf;
	
	return o;
}


template <typename _Os> 
_Os& operator <<
(
_Os& o,
const cmdout::scale& s
)
{
    char rval[30];
    sds_rtos(value0(s.m_scale), 2, -1, rval);
    
	o << rval;
	
	return o;
}

template <typename _Os> 
_Os& operator <<
(
_Os& o,
const cmdout::angle& a
)
{
    char rval[50];
	sds_angtos(value0(a.m_angle), -1, -1, rval);
	
	o << rval;
	return o;
}

template <typename _Os> 
_Os& operator <<
(
_Os& o,
const icl::point& pnt
)
{
	resbuf rb;
	SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	icl::point p = pnt;
	if (rb.resval.rint != IC_UNITS_SCIENTIFIC)
	{
		p.x() = value0(pnt.x());
		p.y() = value0(pnt.y());
		p.z() = value0(pnt.z());
	}

    char xval[50],yval[50],zval[50];
	sds_rtos(p.x(),-1,-1,xval);
	sds_rtos(p.y(),-1,-1,yval);
	sds_rtos(p.z(),-1,-1,zval);

	char buf[IC_ACADNMLEN];
	sprintf(buf, ResourceString(IDC_CMDS9___X_____S__Y______88, "  X=   %s  Y=   %s  Z=   %s" ),xval,yval,zval);

	o << buf;
	return o;
}

template <typename _Os> 
_Os& operator << 
(
_Os& o, 
const icl::gvector& vec
)
{
	resbuf rb;
	SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	icl::gvector v = vec;
	if (rb.resval.rint != IC_UNITS_SCIENTIFIC)
	{
		v.x() = value0(vec.x());
		v.y() = value0(vec.y());
		v.z() = value0(vec.z());
	}

    char xval[50],yval[50],zval[50];
	sds_rtos(v.x(),-1,-1,xval);
	sds_rtos(v.y(),-1,-1,yval);
	sds_rtos(v.z(),-1,-1,zval);

	char buf[IC_ACADNMLEN];
	sprintf(buf, ResourceString(IDC_CMDS9___X_____S__Y______88, "  X=   %s  Y=   %s  Z=   %s" ),xval,yval,zval);

	o << buf;
	return o;
}

template <typename _Os> 
_Os& operator <<
(
_Os& o,
const cmdout::point2d& pnt
)
{
	resbuf rb;
	SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	icl::point p = pnt.m_pt;
	if (rb.resval.rint != IC_UNITS_SCIENTIFIC)
	{
		p.x() = value0(pnt.m_pt.x());
		p.y() = value0(pnt.m_pt.y());
	}

    char xval[50],yval[50];
	sds_rtos(p.x(),-1,-1,xval);
	sds_rtos(p.y(),-1,-1,yval);

	char buf[IC_ACADNMLEN];
	sds_printf(buf, ResourceString(IDC_CMDS9___X_____S__Y______89, "  X=   %s  Y=   %s" ),xval,yval);

	o << rval;
	return o;
}

inline CCmdOut& endl(CCmdOut& o)
{
	o.m_iCurrPos++;
	return o << "\n";
}

#endif

// IcadToolTip.h: interface for the CIcadToolTip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICADTOOLTIP_H__20A88F07_AA6A_11D4_BC02_00105A717134__INCLUDED_)
#define AFX_ICADTOOLTIP_H__20A88F07_AA6A_11D4_BC02_00105A717134__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIcadToolTip : public CToolTipCtrl  
{
public:
	CIcadToolTip();
	virtual ~CIcadToolTip();

	int m_nLastHit;
	TOOLINFO m_lastInfo;

};

#endif // !defined(AFX_ICADTOOLTIP_H__20A88F07_AA6A_11D4_BC02_00105A717134__INCLUDED_)

/* REALTIME.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * CRealTimexxx classes : declaration
 *
 */ 
#ifndef _REALTIME_H
#define _REALTIME_H


class CIcadView;
enum RealTimeMotions;

class CRealTime
{
	public:
		CRealTime(CIcadView* pView, POINT ptStart);
		~CRealTime(void);

		virtual bool Continue(POINT ptLatest);

		// EBATECH(shiba)-[changed
//		virtual bool Continue(short Delta);
		virtual bool Continue(short Delta, CPoint pt);
		// ]-EBATECH(shiba)

		bool		m_bContinuousOrbitStarted;

	protected:
		CIcadView*	m_pView;
		POINT		m_ptLast;
		int			m_iPrevCursorMode;
		int			m_iViewMode;
		int			m_iTileMode;
		int			m_iCVPort;

		void		RedrawAfter();

	public:
		RealTimeMotions		m_Motion;
};
	

class CRealTimeZoom : public CRealTime
{
	public:
		CRealTimeZoom(CIcadView* pView, POINT ptStart);

		bool Continue(POINT ptLatest);
		// EBATECH(shiba)-[changed
//		bool Continue(short Delta);
		bool Continue(short Delta, CPoint pt);
		// ]-EBATECH(shiba)
};
	

class CRealTimePan : public CRealTime
{
	public:
		CRealTimePan(CIcadView* pView, POINT ptStart);

		bool Continue(POINT ptLatest);
};
	
class CRealTimeRotate_Cylinder : public CRealTime
{
	public:
		enum Axes
		{
			X,
			Y,
			Z
		};

		CRealTimeRotate_Cylinder(CIcadView* pView, POINT ptStart, Axes theAxis);

		bool Continue(POINT ptLatest);

	protected:
		void CalculateRotationMatrix(POINT ptLatest);

	private:
		void CalculateRotation(POINT ptLatest);

		Axes		Axis;
		sds_real	m_CosRotAngle, m_SinRotAngle, m_RotAngle;
};
	
class CRealTimeRotate_Sphere : public CRealTime
{
	public:
		CRealTimeRotate_Sphere(CIcadView* pView, POINT ptStart);

		bool Continue(POINT ptLatest);
	
	private:
		void CalculateRotation(POINT ptLatest);

		sds_matrix	m_RotationMatrix,
					m_RotationMatrixInv;
};
	
#endif // _REALTIME_H

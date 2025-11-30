/* D:\ICADDEV\PRJ\LIB\CMDS\LINE_OR_CIRCLE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * This file contains the implementation of methods of the line_or_circle class,
 * which was created to support the construction of a circle tangent
 * to three objects, each of which may be a line or a circle.
 * 
 * 
 */ 

#include "cmds.h"
#include "line_or_circle.h"	
#include "geometry.h"
#include "curve.h"
#include "geoconvert.h"

// implementation of the line_or_circle class

	// default constructor (does nothing)
	line_or_circle::line_or_circle()
		{
		}

	// constructor from the two-point type
	line_or_circle::line_or_circle(sds_point p1, 
		                           sds_point p2, 
								   int two_point_type,
								   sds_point picpt,
								   sds_resbuf rb)
		{
		// can assign the pick_pt before the switch statement
		ic_ptcpy(m_pick_pt, picpt);

		switch (two_point_type)
			{  // begin switch (two_point_type)
			case 0: // line
				ic_ptcpy(m_start_pt, p1);
				ic_ptcpy(m_end_pt, p2);
				m_type = line;
				break;
			case 1: // arc
				ic_ptcpy(m_center, p1);
				m_radius = p2[0];
				ic_ptcpy(m_extrusion, rb.resval.rpoint);
				m_type = arc;
				break;
            case 2: // circle
				ic_ptcpy(m_center, p1);
				m_radius = p2[0];
				ic_ptcpy(m_extrusion, rb.resval.rpoint);
				m_type = circle;
				break;
			default:
				m_type = other;
				break;
			}  //   end switch (two_point_type)

		}

	// assignment operator

    void line_or_circle::operator=(line_or_circle & source)
		{
		// don't try to assign it to itself, just return in that case
		if (this==&source) return;

		// copy the relevant data members.
		m_type = source.m_type;

		ic_ptcpy(m_pick_pt, source.m_pick_pt);

		switch (m_type)
			{
			case line:
				ic_ptcpy(m_start_pt, source.m_start_pt);
				ic_ptcpy(m_end_pt, source.m_end_pt);
				break;
			case arc:
			case circle:
				ic_ptcpy(m_center, source.m_center);
				m_radius = source.m_radius;
				ic_ptcpy(m_extrusion, source.m_extrusion);
				break;
			default:
				break;
			}
		}


		void line_or_circle::swap(line_or_circle & that)
		{
		line_or_circle temp;

		temp = *this;
		*this = that;
		that = temp;

		}

		// only to be used for lines
		void line_or_circle::WcsToUcs()
			{
			ASSERT(m_type==line);
			switch(m_type)
				{
				case line:
					sds_point P,Q;
					
					get_start_pt(P);
					SdsWcsToUcs(P,Q);
					set_start_pt(Q);
					
					get_end_pt(P);
					SdsWcsToUcs(P,Q);
					set_end_pt(Q);
					
					break;
				case arc:
				case circle:
					break;
				default:
					break;
				}
			}

	    // only to be used for arcs or circles
		// changes the center from ECS to UCS
		void line_or_circle::EcsToUcs()
			{
			ASSERT((m_type==arc)||(m_type==circle));
			switch(m_type)
				{
				case line:
					break;
				case arc:
				case circle:
					sds_point UcsZ, P, Q;
					SdsGetUcsZ(UcsZ);
					get_center(P);
					SdsEcsToUcs(P,Q,UcsZ);
					set_center(Q);
				default:
					break;
				}
			}

		// getters

		void line_or_circle::get_type(line_or_circle_type & t)
			{
			t = this->m_type;
			}

		void line_or_circle::get_pick_pt(sds_point pt)
			{
			ic_ptcpy(pt, m_pick_pt);
			}

		void line_or_circle::get_start_pt(sds_point pt)
			{
			ASSERT(m_type==line);
			ic_ptcpy(pt, m_start_pt);
			}

		void line_or_circle::get_end_pt(sds_point pt)
			{
			ASSERT(m_type==line);
			ic_ptcpy(pt, m_end_pt);
			}

		void line_or_circle::get_center(sds_point pt)
			{
			ASSERT((m_type==arc) || (m_type==circle));
			ic_ptcpy(pt, m_center);
			}

		void line_or_circle::get_radius(double & r)
			{
			ASSERT((m_type==arc) || (m_type==circle));
			r = m_radius;
			}

		void line_or_circle::get_extrusion(sds_point pt)
			{
			ASSERT((m_type==arc) || (m_type==circle));
			ic_ptcpy(pt, m_extrusion);
			}

		void line_or_circle::set_pick_pt(sds_point pt)
			{
			ic_ptcpy(m_pick_pt, pt);
			}
		
		void line_or_circle::set_start_pt(sds_point pt)
			{
			ic_ptcpy(m_start_pt, pt);
			}

		void line_or_circle::set_end_pt(sds_point pt)
			{
			ic_ptcpy(m_end_pt, pt);
			}

		void line_or_circle::set_center(sds_point pt)
			{
			ic_ptcpy(m_center, pt);
			}

		// returns true if and only if both this and that are
		// lines and this is parallel to that
		// I am specifically using "this" here because it makes
		// it more readable.
		bool line_or_circle::isParallelTo(line_or_circle & that)
			{
			bool parallel = false;
			
			// if they are not both lines, just return false immediately
			line_or_circle_type t1, t2;
			this->get_type(t1);
			that.get_type(t2);
			if ((t1 == line) && (t2 == line))
				{  // begin case where both are lines
				sds_point A,B,C,D;
				this->get_start_pt(A);
				this->get_end_pt(B);
				that.get_start_pt(C);
				that.get_end_pt(D);
				sds_point intPt;
				if(ic_linexline(A,B,C,D,intPt) == -1)
					parallel = true;
				else
					parallel = false;
				}  //   end case where both are lines
			else
				parallel = false;
			
			
			
			return parallel;
			}


//***************************************************************


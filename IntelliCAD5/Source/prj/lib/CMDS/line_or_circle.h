/* D:\ICADDEV\PRJ\LIB\CMDS\LINE_OR_CIRCLE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * This file contains the declaration of the line_or_circle class,
 * which was created to support the construction of a circle tangent
 * to three objects, each of which may be a line or a circle.
 * 
 */ 

#ifndef LINE_OR_CIRCLE_H
#define LINE_OR_CIRCLE_H

#include "Icad.h" /*DNT*/


// the following class, line_or_circle, is to manage the various tangent circle
// operations
enum line_or_circle_type {line, arc, circle, other};
class line_or_circle
	{
	private:
		line_or_circle_type m_type;
		
		sds_point m_pick_pt;
		
		// if type is line
		sds_point m_start_pt;
		sds_point m_end_pt;
		
		// if type is arc or circle
		sds_point m_center;
		double m_radius;
		sds_point m_extrusion;
		
	public:
		
		// constructors
		
		// default constructor
		line_or_circle();
		
		// construct from the two-point form
		line_or_circle(sds_point p1, 
			sds_point p2, 
			int two_point_type,
			sds_point picpt,
			sds_resbuf rb);
		
		// getters
		void get_pick_pt(sds_point pt);
		void get_type(line_or_circle_type & t);
		void get_start_pt(sds_point pt);
		void get_end_pt(sds_point pt);
		
		void get_center(sds_point pt);
		void get_radius(double & r);
		void get_extrusion(sds_point pt);
		
		// setters
		void set_pick_pt(sds_point pt);
		void set_start_pt(sds_point pt);
		void set_end_pt(sds_point pt);
		void set_center(sds_point pt);
		
		// assignment operators
		void operator=(line_or_circle & source);
		
		// manipulators
		
		void swap(line_or_circle & that);
		void WcsToUcs();
		void EcsToUcs();

		// analyzers
		bool isParallelTo(line_or_circle & that);
		
	};
#endif // LINE_OR_CIRCLE_H


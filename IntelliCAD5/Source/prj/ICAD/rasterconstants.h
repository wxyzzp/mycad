// **********************************************
// RASTERCONSTANTS.H
//
// Copyright (C) 1998 Visio Corporation. All rights reserved.
//
// Definitions shared by low level drawing classes
//
//


#ifndef _RASTERCONSTANTS_H
#define _RASTERCONSTANTS_H

// **************************************************************
// Constants defined in this class (basically acting as a namespace)
// are for use in low-level drawing routines 
//
class RasterConstants
	{
	public:

	enum DrawMode
		{
		MODE_NONE = 0,
		MODE_COPY,
		MODE_XOR
		};


	enum LineStyle
		{
		LS_NONE = 0,
		LS_SOLID,
		LS_DOT
		};

	};

#endif // _RASTERCONSTANTS_H



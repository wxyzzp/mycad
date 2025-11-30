// INPUTEVENTFILTER.H
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the input event class
// 
#ifndef _INPUTEVENTFILTER_H
#define _INPUTEVENTFILTER_H

// Interface of InputEventFilter uses InputEvent in name
//
class InputEvent;

class InputEventFilter
	{

	// -----------------
	// CTORS and DTORS
	//
	public:
		InputEventFilter( int iFilterFlags );
		virtual ~InputEventFilter( void );

	// -----------------
	// OPERATIONS
	//
	public:
		bool Match( InputEvent *pInputEvent );

	// -----------------
	// internal data
	private:
		int m_iFilterFlags;

	};


#endif // _INPUTEVENTFILTER_H


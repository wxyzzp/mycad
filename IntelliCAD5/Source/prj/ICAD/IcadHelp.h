/* PRJ\ICAD\IcadHelp.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Help definition
 * Conditionally includes HTML help
 * 
 */ 

#ifndef	_ICADHELP_H
#define	_ICADHELP_H

#include "Configure.h"	// HTML help is a configurable option

#ifdef	BUILD_WITH_HTMLHELP
#include "HTMLHelp.h"
#endif

#ifdef	BUILD_WITH_HTMLHELP
	// ordinal used in GetProcAddress for HTML help
const LPCTSTR	ordinalHTMLhelp		= ATOM_HTMLHELP_API_ANSI;

const LPCTSTR	SDS_HLPFILE			= "icad.chm";
const LPCTSTR	SDS_COMMANDHELP		= "icad.chm";

const UINT		ICADHELP_CONTENTS	= HH_DISPLAY_TOPIC;
const UINT		ICADHELP_FINDER		= HH_DISPLAY_TOPIC;
const UINT		ICADHELP_CONTEXT	= HH_HELP_CONTEXT;
const UINT		ICADHELP_HELPONHELP	= HH_DISPLAY_TOPIC;
const UINT		ICADHELP_KEY		= HH_KEYWORD_LOOKUP;
const UINT		ICADHELP_PARTIALKEY	= HH_KEYWORD_LOOKUP;
const UINT		ICADHELP_QUIT		= HH_CLOSE_ALL;

	// equivalents for WinHelp
#else
const LPCTSTR	ordinalHTMLhelp		= NULL;
const LPCTSTR	SDS_HLPFILE			= "icad.hlp";
const LPCTSTR	SDS_COMMANDHELP		= "commandref.hlp";	
const UINT		ICADHELP_CONTENTS	= HELP_CONTENTS;
const UINT		ICADHELP_FINDER		= HELP_FINDER;
const UINT		ICADHELP_CONTEXT	= HELP_CONTEXT;
const UINT		ICADHELP_HELPONHELP	= HELP_HELPONHELP;
const UINT		ICADHELP_PARTIALKEY	= HELP_PARTIALKEY;
const UINT		ICADHELP_KEY		= HELP_KEY;
const UINT		ICADHELP_QUIT		= HELP_QUIT;
#endif

#endif

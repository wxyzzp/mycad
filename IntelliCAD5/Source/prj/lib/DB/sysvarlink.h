/* C:\ICAD\PRJ\LIB\DB\SYSVARLINK.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/*
**  All system variables that releases R10-R12 recognize (plus a
**  few undocumented ones).
**
**  MAINTAIN IN ORDER BY NAME SO THAT bsearch() WILL WORK.
**  2002/6/4 EBATECH(CNBR) add flag value to comment.
*/

#ifndef _INC_SYSVARLINK
#define _INC_SYSVARLINK

struct db_sysvarlink {
    char *name;
    short type;    /* ADS RTtype that GETVAR/SETVAR deal with. */
                   /*   (No RTANG or RTORINT here -- just RTREAL.) */
    char  loc;     /* Where stored: 0=DWG header; 1=Config; 2=Session */
    DWORD spl;     /* Special flags; bit-coded as documented in icaddef.h: */
				   /*	#define IC_SYSVARFLAG_UNUSUAL					0x0001	//   Bit 0 : Unusual form in buffer */
				   /*	#define IC_SYSVARFLAG_SPECIALHANDLING			0x0002	//   Bit 1 : Special handling writing; not independent */
				   /*	#define IC_SYSVARFLAG_UNDOCUMENTED				0x0004	//   Bit 2 : Undocumented */
				   /*	#define IC_SYSVARFLAG_SPECIALSOURCE				0x0008	//   Bit 3 : Special source (not stored in a buffer like DATE. Usually, truly read-only. Should be only certain session vars.) */
				   /*	#define IC_SYSVARFLAG_SPECIALRANGE				0x0010	//   Bit 4 : Special range of allowed vals */
				   /*	#define IC_SYSVARFLAG_DONTDXFOUT  				0x0020	//   Bit 5 : Exclude from DXFOUT  */
				   /*	#define IC_SYSVARFLAG_REQUIRESSCREENCHANGE		0x0040	//   Bit 6 : Requires some sort of screen change */
				   /*	#define IC_SYSVARFLAG_ACADSESSIONVARTOHDR 		0x0080	//   Bit 7 : An ACAD session var that we moved to the header (giving it a slot after the end of the true R12 header).  This makes it "per drawing" instead of "per session". */
				   /*	#define IC_SYSVARFLAG_ANGLE  					0x0100	//   Bit 8 : Angle (it's "type" is RTREAL) */
				   /*	#define IC_SYSVARFLAG_TIMEDATE        			0x0200	//   Bit 9 : Time-date (it's "type" is RTREAL) */
				   /*	#define IC_SYSVARFLAG_DONTSAVEINUNDOLIST 		0x0400	//   Bit 10: Don't save in undo list. */
				   /*	#define IC_SYSVARFLAG_SPECIALVIEWVBL			0x0800	//   Bit 11: Special view variable (from VIEW table). */
				   /*	#define IC_SYSVARFLAG_NEWR13VBL   				0x1000	//   Bit 12: New variable in R13 (exclude from R12 DXFout) */
				   /*	#define IC_SYSVARFLAG_FILEPATH					0x2000	//   Bit 13: A file pathname */
				   /*	#define IC_SYSVARFLAG_USERREADONLY				0x4000	//   Bit 14: User-read-only */
				   /*	#define IC_SYSVARFLAG_UNSUPPORTED				0x8000	//   Bit 15: A var that is currently unsupported.*/
				   /*	#define IC_SYSVARFLAG_PAPERSPACE			0x00010000	//   Bit 16: PaperSpace variant of another variable. */
				   /*	#define IC_SYSVARFLAG_OBJDICTONARY			0x00020000	//   Bit 16: Variable is stored in an object dictionary. */
    short bc;      /* Beginning char in appropriate buffer (based on loc). */
                   /*   (0-based.) */
    short nc;      /* # of chars in field in appropriate buffer. */
    char *range;   /* Range of vals for numeric types: "low_high" */
    char *defval;  /* Default value */
};


#endif


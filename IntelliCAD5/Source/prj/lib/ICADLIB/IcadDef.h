/* F:\ICADDEV\PRJ\LIB\ICADLIB\ICADDEF.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Common definitions to be included in all IntelliCAD code.
 */

#ifndef	_ICADDEF_H
#define	_ICADDEF_H


/*DG - 11.6.2003*/// __FILELINE__ macro allows to add "file(line)" string to pragma messages, e.g.
//  #pragma message(__FILELINE__ "a very important message")
// in a file c:\myfiles\file.cpp at line 13 produces the output on compiling:
//  c:\myfiles\file.cpp(13): a very important message
// so you can double-click it in the output window and the IDE will place your cursor at this location.
// Note, extra level of indirection (sLINE and sLINE_ macros) is very necessary: the preprocessor knows
// exactly what to do with the __LINE__ macro.
// Thanks to Q155196 MSDN article.
#define sLINE_(l) #l
#define sLINE(l) sLINE_(l)
#define __FILELINE__ __FILE__ "("sLINE(__LINE__)"): "


//	*****************************************************
//	if ( Debug ) {} is preferable to #ifdef _DEBUG
//	when the optimizer is adequate

#ifdef	NDEBUG
    const bool Debug = false;
#else
    const bool Debug = true;
#endif


//	*****************************************************
//	Alternative memory managers can be defined here
//	Use the IDE tool for debugging

#ifdef	NDEBUG
    						// some problems have occurred with Purify/SmartHeap in DLLs
    #if !defined(DBDLL) && !defined(GRDLL) && !defined(MINDEBUG)
//		#include "smrtheap.h"	// removed for ITC
    #endif
#endif

#ifdef	_SMARTHEAP_H
#define	USE_SMARTHEAP
#endif


// *****************************************************
// ASSERT and VERIFY macros
//
//  For the moment I've chosen to NOT override MFC's versions if found
//
#if !defined( assert )
    #include <assert.h>
#endif

#if !defined( ASSERT )
    #if defined( NDEBUG )
    	#define ASSERT( expr )		((void)0)
    #else
    	#define ASSERT( expr )		assert( expr )
    #endif // defined( NDEBUG )
#endif // !defined( ASSERT )

#if !defined( VERIFY )
    #if defined( NDEBUG )
    	#define VERIFY( expr ) 		((void)(expr))
    #else
    	#define VERIFY( expr ) 		assert( expr )
    #endif // defined( NDEBUG )
#endif // !defined( VERIFY )


#if defined( NDEBUG )
    #define ASSERTBREAK( expr )			((void)0)
#else
    #define ASSERTBREAK(expr)			( (expr) ? (void)0 : DebugBreak() )
#endif
// *******************************************************

// *****************************************************
// type consistency... force errors for int->char/short mismatches
// #pragma warning(3:4244)

inline char charCast( int i)
    {
    ASSERT( -MINCHAR <= i && i <= MAXCHAR);
    return (char)i;
    }

inline short shortCast( int i)
    {
    ASSERT( -MINSHORT <= i && i <= MAXSHORT);
    return (short)i;
    }

    // utility object for return codes which handles sloppy type usage
struct RT
    {
    RT()		      : value( 0)		{}
    RT( int   _value) : value( _value)	{}
    operator short()	{ return shortCast( value);}

    bool operator &&( bool b)	{ return (*this != 0) && b;}
    bool operator ||( bool b)	{ return (*this != 0) || b;}
    bool operator ==( int i)	{ return value == i;}
    bool operator !=( int i)	{ return value != i;}
    bool operator < ( int i)	{ return value <  i;}
    bool operator < ( long i)	{ return value <  i;}
    bool operator <=( int i)	{ return value <= i;}
    bool operator > ( int i)	{ return value >  i;}
    bool operator >=( int i)	{ return value >= i;}

    int	value;
    };

// *******************************************************
// POINTER CHECKING MACROS
// These macros depend on windows.h to be included.  Rather than set up the includes here,
// perhaps pulling windows into places we don't want it, the macro user had better have it where they use it.
//
#define CHECKDATA( p, l )		((p) && !IsBadReadPtr( (LPVOID)(p), l ) )
#define CHECKPTR( p )			CHECKDATA( p, 1 )
#define CHECKFUNCPTR( p ) 		((p) && !IsBadCodePtr( (FARPROC) (p) ) )
#define CHECKSTR( s )			((s) && !IsBadStringPtr( (LPCSTR)(s), 30000 ) )
#define CHECKSTRUCTPTR( p )		CHECKDATA( p, sizeof( *p ) )

// *******************************************************

//#define	UNICODE

// ******************************************************************
#define	EQ	==

#define DllExport _declspec(dllexport)

const float		INCHES_TO_MM	= (float)25.4;
const double	IC_PI			= 3.14159265358979323846;
const double	IC_TWOPI		= 2 * IC_PI;

inline double InchesToMm( double inches)
    { return inches * INCHES_TO_MM;}

inline double MmToInches( double millimeters)
    { return millimeters / INCHES_TO_MM;}


// Code copied from CRT source
//
inline int __cdecl strings_i_compare(const char * dst, const char * src)
{
        int f,l;

            do {
                if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
                    f -= ('A' - 'a');

                if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
                    l -= ('A' - 'a');
            } while ( f && (f == l) );

        return(f - l);
}

#define strsame(a,b) (!strcmp((a),(b)))
#define strisame(a,b) (!strings_i_compare((a),(b)))
#define strnsame(a,b,c) (!strncmp((a),(b),(c)))
#define strnisame(a,b,c) (!strnicmp((a),(b),(c)))

// defines for file loads/saves.  These MUST be kept in sync with the
// related defines in OpenDWG Toolkit
#define IC_DWG 0
#define IC_DXF 1
#define IC_BDXF 2
#define IC_DWT 3
#define IC_ACAD25 0  /* ac1002 */
#define IC_ACAD26 1  /* ac1003 */
#define IC_ACAD9  2  /* ac1004 */
#define IC_ACAD10 3  /* ac1006 */
#define IC_ACAD11 4  /* ac1009 */
#define IC_ACAD13 5  /* ac1012 */
#define IC_ACAD14 6
#define IC_ACAD2000 7
#define IC_ACAD2004 8
#define IC_MAXACADVER IC_ACAD2004

// defines for sysvarlinks, anders with the "spl" field

#define IC_SYSVARFLAG_UNUSUAL					0x0001	//   Bit 0 : Unusual form in buffer
#define IC_SYSVARFLAG_SPECIALHANDLING			0x0002	//   Bit 1 : Special handling writing; not independent
#define IC_SYSVARFLAG_UNDOCUMENTED				0x0004	//   Bit 2 : Undocumented 
#define IC_SYSVARFLAG_SPECIALSOURCE				0x0008	//   Bit 3 : Special source (not stored in a buffer like DATE. Usually, truly read-only. Should be only certain session vars.)
#define IC_SYSVARFLAG_SPECIALRANGE				0x0010	//   Bit 4 : Special range of allowed vals
#define IC_SYSVARFLAG_DONTDXFOUT  				0x0020	//   Bit 5 : Exclude from DXFOUT 
#define IC_SYSVARFLAG_REQUIRESSCREENCHANGE		0x0040	//   Bit 6 : Requires some sort of screen change
#define IC_SYSVARFLAG_ACADSESSIONVARTOHDR 		0x0080	//   Bit 7 : An ACAD session var that we moved to the header (giving it a slot after the end of the true R12 header).  This makes it "per drawing" instead of "per session".
#define IC_SYSVARFLAG_ANGLE  					0x0100	//   Bit 8 : Angle (it's "type" is RTREAL) 
#define IC_SYSVARFLAG_TIMEDATE        			0x0200	//   Bit 9 : Time-date (it's "type" is RTREAL)
#define IC_SYSVARFLAG_DONTSAVEINUNDOLIST 		0x0400	//   Bit 10: Don't save in undo list.
#define IC_SYSVARFLAG_SPECIALVIEWVBL			0x0800	//   Bit 11: Special view variable (from VIEW table).
#define IC_SYSVARFLAG_NEWR13VBL   				0x1000	//   Bit 12: New variable in R13 (exclude from R12 DXFout) 
#define IC_SYSVARFLAG_FILEPATH					0x2000	//   Bit 13: A file pathname 
#define IC_SYSVARFLAG_USERREADONLY				0x4000	//   Bit 14: User-read-only 
#define IC_SYSVARFLAG_UNSUPPORTED				0x8000	//   Bit 15: A var that is currently unsupported.
#define IC_SYSVARFLAG_PAPERSPACE			0x00010000	//   Bit 16: PaperSpace variant of another variable.
#define IC_SYSVARFLAG_OBJDICTONARY			0x00020000	//   Bit 16: Variable is stored in an object dictionary.


// defines for block insertions
#define IC_INSMODE_INSERT                        0
#define IC_INSMODE_MINSERT                       1
#define IC_INSMODE_FROM_DIALOG_CREATE_ARGBNAME	 5
// the next one probably needs a better name
#define IC_INSMODE_FROM_DIALOG_FILENAME_IN_XREF  6
#define IC_INSMODE_XREF_ATTACH					 7
#define IC_INSMODE_XREF_OVERLAY					 8
#define IC_INSMODE_XREF_UPDATE					 9
#define IC_INSMODE_XREF_BIND					10
#define IC_INSMODE_XREF_INSERT					11
#define IC_IS_XREF_INSMODE(a) ((a)>=IC_INSMODE_XREF_ATTACH)
#define IC_TYPE_FROM_ENAME(a) (((db_handitem *)((a)[0]))->ret_type())

// defines for cmd_xref_uncouple
#define IC_UNCOUPLE_DETACH  0
#define IC_UNCOUPLE_BIND    1
#define IC_UNCOUPLE_INSERT  2

/* flag "anders"  & other constants */
#define IC_FACE3D_FACE1INVIS           1
#define IC_FACE3D_FACE2INVIS           2
#define IC_FACE3D_FACE3INVIS           4
#define IC_FACE3D_FACE4INVIS           8
#define IC_ATTDEF_INVIS                1
#define IC_ATTDEF_CONST                2
#define IC_ATTDEF_VERIFY               4
#define IC_ATTDEF_PRESET               8
#define IC_ATTRIB_INVIS                1
#define IC_ATTRIB_CONST                2
#define IC_ATTRIB_VERIFY               4
#define IC_ATTRIB_PRESET               8
#define IC_DIMTYPE_ROTATED             0
#define IC_DIMTYPE_ALIGNED             1
#define IC_DIMTYPE_ANGULAR             2
#define IC_DIMTYPE_DIAMETER            3
#define IC_DIMTYPE_RADIUS              4
#define IC_DIMTYPE_ANGULAR3PT          5
#define IC_DIMTYPE_ORDINATE            6
#define IC_DIMTYPE_XORDDIM            64
#define IC_DIMTYPE_TEXTPOS           128
#define IC_MTEXT_ATTACH_TOPLEFT        1
#define IC_MTEXT_ATTACH_TOPCENTER      2
#define IC_MTEXT_ATTACH_TOPRIGHT       3
#define IC_MTEXT_ATTACH_MIDDLELEFT     4
#define IC_MTEXT_ATTACH_MIDDLECENTER   5
#define IC_MTEXT_ATTACH_MIDDLERIGHT    6
#define IC_MTEXT_ATTACH_BOTTOMLEFT     7
#define IC_MTEXT_ATTACH_BOTTOMCENTER   8
#define IC_MTEXT_ATTACH_BOTTOMRIGHT    9
#define IC_MTEXT_DRAWDIR_LTOR          1
#define IC_MTEXT_DRAWDIR_RTOL          2
#define IC_MTEXT_DRAWDIR_TTOB          3
#define IC_MTEXT_DRAWDIR_BTOT          4
#define IC_PLINE_CLOSED                1
#define IC_PLINE_CURVEFIT              2
#define IC_PLINE_SPLINEFIT             4
#define IC_PLINE_3DPLINE               8
#define IC_PLINE_3DMESH               16
#define IC_PLINE_CLOSEDN              32
#define IC_PLINE_POLYFACEMESH         64
#define IC_PLINE_CONTINUELT          128
#define IC_PLINE_SURFTYPE_NONE         0
#define IC_PLINE_SURFTYPE_QUAD         5
#define IC_PLINE_SURFTYPE_CUBIC        6
#define IC_PLINE_SURFTYPE_BEZIER       8
#define IC_TEXT_GEN_BACKWARD           2
#define IC_TEXT_GEN_UPSIDEDOWN         4
#define IC_TEXT_JUST_LEFT              0
#define IC_TEXT_JUST_CENTER            1
#define IC_TEXT_JUST_RIGHT             2
#define IC_TEXT_JUST_ALIGNED           3
#define IC_TEXT_JUST_MIDDLE            4
#define IC_TEXT_JUST_FIT               5
#define IC_TEXT_VALIGN_BASELINE        0
#define IC_TEXT_VALIGN_BOTTOM          1
#define IC_TEXT_VALIGN_MIDDLE          2
#define IC_TEXT_VALIGN_TOP             3
#define IC_VERTEX_FROMFIT              1
#define IC_VERTEX_HASTANGENT           2
#define IC_VERTEX_SPLINEVERTEX         8
#define IC_VERTEX_SPLINEFRAME         16
#define IC_VERTEX_3DPLINE             32
#define IC_VERTEX_3DMESHVERT          64
#define IC_VERTEX_FACELIST           128
#define IC_LEADER_PATH_STRAIGHT        0
#define IC_LEADER_PATH_SPLINE          1
#define IC_LEADER_ANNO_TEXT            0
#define IC_LEADER_ANNO_TOLERANCE       1
#define IC_LEADER_ANNO_BLOCK           2
#define IC_LEADER_ANNO_NONE            3
#define IC_SPLINE_CLOSED               1
#define IC_SPLINE_PERIODIC             2
#define IC_SPLINE_RATIONAL             4
#define IC_SPLINE_PLANAR               8
#define IC_SPLINE_LINEAR              16
#define IC_MLINE_JUST_TOP              0
#define IC_MLINE_JUST_ZERO             1
#define IC_MLINE_JUST_BOTTOM           2
#define IC_MLINE_LOCKED                1
#define IC_MLINE_CLOSED                2
#define IC_MLINE_SUPPRESS_START_CAPS   4
#define IC_MLINE_SUPPRESS_END_CAPS     8

#define IC_ENTRY_XREF_DEPENDENT        16
#define IC_XREF_RESOLVED               32
#define IC_REFERENCED                  64
#define IC_BLOCK_ANONYMOUS             1
#define IC_BLOCK_HASATTRIBUTES         2
#define IC_BLOCK_XREF                  4
#define IC_BLOCK_XREF_OVERLAID         8
#define IC_LAYER_OFF                   0
#define IC_LAYER_ON                    1
#define IC_LAYER_THAWED                0
#define IC_LAYER_FROZEN                1
#define IC_LAYER_VPFROZEN              2
#define IC_LAYER_LOCKED                4
#define IC_LAYER_NOCHANGE              -1
#define IC_SHAPEFILE_SHAPES            1
#define IC_SHAPEFILE_VERTICAL          4
#define IC_SHAPEFILE_GEN_BACKWARD      2
#define IC_SHAPEFILE_GEN_UPSIDEDOWN    4
#define IC_VIEW_ISPAPERSPACE           1
#define IC_MLSTYLE_FILL_ON             1
#define IC_MLSTYLE_SHOW_MITERS         2
#define IC_MLSTYLE_START_SQUARE        16
#define IC_MLSTYLE_START_INNER_ARC     32
#define IC_MLSTYLE_START_ROUND         64
#define IC_MLSTYLE_END_SQUARE          256
#define IC_MLSTYLE_END_INNER_ARC       512
#define IC_MLSTYLE_END_ROUND           1024
#define IC_LWPLINE_HAS_CONSTANT_WIDTH  4
#define IC_LWPLINE_HAS_BULGES          16
#define IC_LWPLINE_HAS_WIDTHS          32
#define IC_LWPLINE_PLINEGEN            256
#define IC_LWPLINE_IS_CLOSED           512
#define IC_HATCH_PATHTYPE_LINE         1
#define IC_HATCH_PATHTYPE_CIRCARC      2
#define IC_HATCH_PATHTYPE_ELLIPARC     3
#define IC_HATCH_PATHTYPE_SPLINE       4
#define IC_HATCH_STYLE_NORMAL          0
#define IC_HATCH_STYLE_OUTER           1
#define IC_HATCH_STYLE_IGNORE          2
#define IC_HATCH_PATHFLAG_DEFAULT      0
#define IC_HATCH_PATHFLAG_EXTERNAL     1
#define IC_HATCH_PATHFLAG_POLYLINE     2
#define IC_HATCH_PATHFLAG_DERIVED      4
#define IC_HATCH_PATHFLAG_TEXTBOX      8
#define IC_HATCH_PATHFLAG_OUTERMOST   16
#define IC_HATCH_PATTERNTYPE_USER       0
#define IC_HATCH_PATTERNTYPE_PREDEFINED 1
#define IC_HATCH_PATTERNTYPE_CUSTOM     2
#define IC_IMAGE_CLIPBOUND_RECT        1
#define IC_IMAGE_CLIPBOUND_POLYGON     2
#define IC_IMAGE_DISPPROPS_SHOWIMAGE      1
#define IC_IMAGE_DISPPROPS_SHOWNONALIGNED 2
#define IC_IMAGE_DISPPROPS_USECLIP        4
#define IC_IMAGE_DISPPROPS_TRANSON        8
#define IC_ISDIMTYPE_ROTATED(a)             ((a & 7)==0)
#define IC_ISDIMTYPE_ALIGNED(a)             ((a & 7)==1)
#define IC_ISDIMTYPE_ANGULAR(a)             ((a & 7)==2)
#define IC_ISDIMTYPE_DIAMETER(a)            ((a & 7)==3)
#define IC_ISDIMTYPE_RADIUS(a)              ((a & 7)==4)
#define IC_ISDIMTYPE_ANGULAR3PT(a)          ((a & 7)==5)
#define IC_ISDIMTYPE_ORDINATE(a)            ((a & 7)==6)
#define IC_ISDIMTYPE_XORDDIM(a)             (a & 64)

// UNDO defined constants
#define IC_UNDO_MARK                1
#define IC_UNDO_GROUP_BEGIN         2
#define IC_UNDO_GROUP_END           3
#define IC_UNDO_SETVAR_CHANGE       4
#define IC_UNDO_REDRAW_ON_UNDO      5
#define IC_UNDO_REGEN_ON_UNDO       6
#define IC_UNDO_REDRAW_ON_REDO      7
#define IC_UNDO_REGEN_ON_REDO       8
#define IC_UNDO_NEW_ENT_TAB_CLS     9
#define IC_UNDO_CHG_ENT_TAB_CLS    10
#define IC_UNDO_DEL_ENT_TAB_CLS    11
#define IC_UNDO_COMMAND_BEGIN      12
#define IC_UNDO_COMMAND_END        13
#define IC_UNDO_ZOOM               14
#define IC_UNDO_ENTITY_REDRAW      15
#define IC_UNDO_NOP                16
#define IC_UNDO_HIDE_ON_REDO       17
#define IC_UNDO_SHADE_ON_REDO      18
#define IC_UNDO_CICADVIEW_CREATE   19
#define IC_UNDO_CICADVIEW_SIZE     20
#define IC_UNDO_CICADVIEW_MOVE     21
#define IC_UNDO_CICADVIEW_DELETE   22
#define IC_UNDO_MDI_RESTORE        23
#define IC_UNDO_MDI_MAXIMIZE       24
#define IC_UNDO_PS_MS_VP_CHANGE    25
#define IC_UNDO_REDRAW_VP_ON_UNDO  26
#define IC_UNDO_REGEN_VP_ON_UNDO   27
#define IC_UNDO_REDRAW_VP_ON_REDO  28
#define IC_UNDO_REGEN_VP_ON_REDO   29
#define IC_UNDO_REDEFINE_BLOCK     30
#define IC_UNDO_IMAGE			   31

#define IC_UNDOCTL_ON               1
#define IC_UNDOCTL_ONECOMMANDUNDO   2
#define IC_UNDOCTL_AUTO             4
#define IC_UNDOCTL_GROUPACTIVE      8

// don't care angle for wide segment draws
#define IC_ANGLE_DONTCARE 1.0E+250

#define IC_ARRAYMODE_2D        0
#define IC_ARRAYMODE_3D        1
#define IC_ARRAYMODE_3DROT     2

// rectangle command
#define IC_RECT_ROTATED  1
#define IC_RECT_SQUARE   2
#define IC_RECT_FILLETED 4
#define IC_RECT_CHAMFERED 8
#define IC_RECT_THICKNESSOVERRIDE 16
#define IC_RECT_WIDTHOVERRIDE     32
#define IC_RECT_ELEVATIONOVERRIDE 64
#define IC_RECT_USINGFILLETOVERRIDE 128
#define IC_RECT_USINGCHAMFEROVERRIDE 256

// osnaps
#define IC_OSMODE_NONE       0
#define IC_OSMODE_END        1
#define IC_OSMODE_MID        2
#define IC_OSMODE_CENTER     4
#define IC_OSMODE_NODE       8
#define IC_OSMODE_QUADRANT  16
#define IC_OSMODE_INT       32
#define IC_OSMODE_INS       64
#define IC_OSMODE_PERP     128
#define IC_OSMODE_TAN      256
#define IC_OSMODE_NEAR     512
#define IC_OSMODE_QUICK   1024
#define IC_OSMODE_APP     2048
// this one is not from ACAD, and will have to change if
// they add another snap type.
#define IC_OSMODE_PLAN    4096
//	// Modified CyberAge VSB 12/05/2001
//	Reason: Implementation of Flyover Snapping.[
//4M Item:77->
#define IC_OSMODE_OFF     16384
//<-4M Item:77
//  // Modified CyberAge VSB 12/05/2001 ]
// redraw modes
typedef enum redraw_types_enum {
IC_REDRAW_HILITE_ONE=-3,
IC_REDRAW_DONTCARE=0,
IC_REDRAW_DRAW=1,
IC_REDRAW_UNDRAW=2,
IC_REDRAW_HILITE=3,
IC_REDRAW_UNHILITE=4,
IC_REDRAW_XOR=5 } redraw_mode;

// MENUECHO
#define IC_MENUECHO_SUPPRESS  1
#define IC_MENUECHO_NOPRINT   2
#define IC_MENUECHO_NOTOGGLE  4
#define IC_MENUECHO_DEBUG     8

// DBMOD
#define IC_DBMOD_ENTITIES_MODIFIED      1
#define IC_DBMOD_SYMBOLS_MODIFIED       2
#define IC_DBMOD_DATABASEVBL_MODIFIED   4
#define IC_DBMOD_WINDOW_MODIFIED        8
#define IC_DBMOD_VIEW_MODIFIED         16

typedef enum getangdist_types_enum {
IC_GETANGDIST_GETDIST=0,
IC_GETANGDIST_GETANGLE=1,
IC_GETANGDIST_GETORIENT=2,
IC_GETANGDIST_GETANGLE_NEGOK_NORM=3,
IC_GETANGDIST_GETORIENT_NEGOK_NORM=4,
IC_GETANGDIST_GETANGLE_NEGOK_NONORM=5,
IC_GETANGDIST_GETORIENT_NEGOK_NONORM=6 } getangdist_mode;

#define IC_TRANS_NO_SHORTCUTS				0
#define IC_TRANS_NO_SCALE_OR_ROTATE			1
#define IC_TRANS_NO_SCALE_ROTATE_TRANSLATE	2

//Bugzilla No. 78414 ; 01-30-2003 [
// Commenting following #define, tolerance used for HATCHING will be a constant (0.00001)
// and will NOT change with VIEWSIZE variable.
#define VARYING_HATCH_TOLERANCE

#ifdef VARYING_HATCH_TOLERANCE
	//Constant 0.00000621 is an value derived after comparison with AutoCAD. In AutoCAD, for Bhatch.dwg, when viewsize = 200,hatch works properly.
	//In IntelliCAD, for Bhatch.dwg, Hatch tolerance should be 0.001242 to work properly. Following is used to calculate hatch tolerance baed on VIEWSIZE.
	const double HATCH_TOLERANCE_COEFFICIENT = 0.00000621; 
#endif
//Bugzilla No. 78414 ; 01-30-2003 ]

// Registry
#define REG_QWORD                   ( 11 )   // 64-bit number

// UNITS
#define IC_UNITS_SCIENTIFIC		1
#define IC_UNITS_DECIMAL		2
#define IC_UNITS_ENGINEERING	3
#define IC_UNITS_ARCHITECTURAL	4
#define IC_UNITS_FRACTIONAL		5

// INTERSECTION (ic_linexline and ic_linexarc returns
// line with line
#define IC_LINEINT_UNDEF_OR_PARALLEL -1
#define IC_LINEINT_ON_NEITHER_SEG 0
#define IC_LINEINT_ON_FIRST_SEG   1
#define IC_LINEINT_ON_SECOND_SEG  2
#define IC_LINEINT_ON_BOTH_SEGS(a) (a!=-1 && (a & IC_LINEINT_ON_FIRST_SEG) && (a & IC_LINEINT_ON_SECOND_SEG))

// line with arc
#define IC_ARCINT_NOINTS -1
#define IC_ARCINT_FIRSTINT_ON_LINE  1
#define IC_ARCINT_FIRSTINT_ON_ARC   2
#define IC_ARCINT_FIRSTINT_ON_BOTH(a) (a!=-1 && (a & IC_ARCINT_FIRSTINT_ON_LINE) && (a & IC_ARCINT_FIRSTINT_ON_ARC))
#define IC_ARCINT_SECONDINT_ON_LINE 4
#define IC_ARCINT_SECONDINT_ON_ARC  8
#define IC_ARCINT_SECONDINT_ON_BOTH(a) (a!=-1 && (a & IC_ARCINT_SECONDINT_ON_LINE) && (a & IC_ARCINT_SECONDINT_ON_ARC))

// arc with arc
#define IC_ARCARCINT_NOINTS -1
#define IC_ARCARCINT_FIRSTINT_ON_ARC0  1
#define IC_ARCARCINT_FIRSTINT_ON_ARC1  2
#define IC_ARCARCINT_FIRSTINT_ON_BOTH(a) (a!=-1 && (a & IC_ARCARCINT_FIRSTINT_ON_ARC0) && (a & IC_ARCARCINT_FIRSTINT_ON_ARC1))
#define IC_ARCARCINT_SECONDINT_ON_ARC0 4
#define IC_ARCARCINT_SECONDINT_ON_ARC1 8
#define IC_ARCARCINT_SECONDINT_ON_BOTH(a) (a!=-1 && (a & IC_ARCARCINT_SECONDINT_ON_ARC0) && (a & IC_ARCARCINT_SECONDINT_ON_ARC1))

// viewport entity anders for statusflags
#define IC_VPENT_STATUS_PERSPECTIVEMODE      1
#define IC_VPENT_STATUS_FRONTCLIPON          2
#define IC_VPENT_STATUS_BACKCLIPON           4
#define IC_VPENT_STATUS_UCSFOLLOW            8
#define IC_VPENT_STATUS_FRONTCLIPNOTATEYE   16
#define IC_VPENT_STATUS_UCSICON             32
#define IC_VPENT_STATUS_UCSICONATORIGIN     64
#define IC_VPENT_STATUS_FASTZOOM           128
#define IC_VPENT_STATUS_SNAPON             256
#define IC_VPENT_STATUS_GRIDON             512
#define IC_VPENT_STATUS_SNAPSTYLEISO      1024
#define IC_VPENT_STATUS_HIDDENINPLOT      2048
#define IC_VPENT_STATUS_SNAPISOPAIRTOP    4096
#define IC_VPENT_STATUS_SNAPISOPAIRRIGHT  8192

// render mode (dxf281) values for VIEWPORT entity, VIEW and VPORT symbol tables
#define IC_RENDERMODE_CLASSIC2D					0
#define IC_RENDERMODE_WIREFRAME					1
#define IC_RENDERMODE_HIDDENLINE				2
#define IC_RENDERMODE_FLAT						3
#define IC_RENDERMODE_GOURAUD					4
#define IC_RENDERMODE_FLAT_WITH_WIREFRAME		5
#define IC_RENDERMODE_GOURAUD_WITH_WIREFRAME	6

#define XDATA_CODE_STRING        1000
#define XDATA_CODE_APPLICATION   1001
#define XDATA_CODE_CONTROL_STR   1002
#define XDATA_CODE_LAYER_NAME    1003
#define XDATA_CODE_BINDATA       1004
#define XDATA_CODE_DATABHANDLE   1005
#define XDATA_CODE_3DPOINT       1010
#define XDATA_CODE_WLD_POS       1011
#define XDATA_CODE_WLD_DISP      1012
#define XDATA_CODE_WLD_DIR       1013
#define XDATA_CODE_REAL          1040
#define XDATA_CODE_DISTANCE      1041
#define XDATA_CODE_SCALE_FAC     1042
#define XDATA_CODE_INTEGER       1070
#define XDATA_CODE_LONG          1071

#endif // _ICADDEF_H

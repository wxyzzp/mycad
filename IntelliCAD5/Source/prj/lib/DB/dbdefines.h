/* C:\ICAD\PRJ\LIB\DB\DBDEFINES.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef _INC_DBDEFINES
#define _INC_DBDEFINES

#define DB_ACADBUF		  512  /* To eliminate IC_ACADBUF (still 256) */
#define DB_PATHNAMESZ	  512  /* Max pathname size */
							   /*	(icadlib.h has a puny one.) */
#define DB_MAXHANDSTRLEN   16  /* Max # of chars in the hex string form of */
							   /*	a handle. */
#define DB_MAXLTYPEDESCLEN 64  /* Max description length for a linetype. */


#if defined(DEBUG)
	#define DB_RBUFSZ  32768
#else
	#define DB_RBUFSZ 102400
#endif


#define DB_OURFILEEXT  ".Icd"

#define DB_HSORTINC 500  /* The realloc increment for db_drawing::hsort */

/*
**	Type-specifiers for db_qgetvar() and possibly other things
**	in the future:
*/
#define DB_CHAR 	0
#define DB_SHORT	1
#define DB_LONG 	2
#define DB_INT		3
#define DB_DOUBLE	4
#define DB_2DPOINT	5
#define DB_3DPOINT	6
#define DB_STRING	7  /* Defined or pre-allocated string */
#define DB_ASTRING	8  /* Allocated string */


#define DB_BLCOLOR		256  /* BYLAYER color */
#define DB_BBCOLOR		  0  /* BYBLOCK color */
#define DB_BLLTYPE10	256  /* BYLAYER ltype code (R10) */
#define DB_BLLTYPE12  32767  /* BYLAYER ltype code (R12) */
#define DB_BBLTYPE10	255  /* BYBLOCK ltype code (R10) */
#define DB_BBLTYPE12  32766  /* BYBLOCK ltype code (R12) */
/*DG - 11.6.2002*/// These defines are needed for dxf370 of layers processing which is supported since dxf 2000.
#define DB_BLWEIGHT 	 -1  /* BYLAYER lineweight */
#define DB_BBWEIGHT 	 -2  /* BYBLOCK lineweight */
#define DB_DFWEIGHT 	 -3  /* DEFAULT lineweight */
#define DB_BLWMASK		  1  /* BYLAYER mask bit */
#define DB_BBWMASK		  2  /* BYBLOCK mask bit */
#define DB_DFWMASK		  4  /* DEFAULT mask bit */

// EBATECH(CNBR) -[ 2002/9/19 Support Metric drawing.
#define DB_ENGLISH_MEASUREINIT 0 /* English(Inch) */
#define DB_METRIC_MEASUREINIT 1  /* Metric */

#define DB_NAMED_PSTYLEMODE 0 /* Named plot style mode(use .stb). */
#define DB_BYCOL_PSTYLEMODE 1 /* ByColor plot style mode(use .ctb). */

#define DB_BL_PLSTYLE	  0 /* BYLAYER Plot style */
#define DB_BB_PLSTYLE	  1 /* BYBLOCK Plot style */
#define DB_DF_PLSTYLE	  2 /* DEFAULT Plot style(I can't see it in dwg file) */
#define DB_NM_PLSTYLE	  3 /* Named Plot style(Link with ACDBPLACEHOLDER) */

// EBATECH(CNBR) -[ ODT dependency code.
#ifdef	ODTVER_2_009
#define DB_MAXLTDASHES 30  /* Max number of linetype dashes */
#else	// ODT2.006,2.007,2.008=15
#define DB_MAXLTDASHES 15  /* Max number of linetype dashes */
#endif
//#define DB_MAXLTDASHES 12  /* Max number of linetype dashes */
// EBATECH(CNBR) ]-
/*
**	Handitem flags bit masks:
*/
#define DB_INVISIBLE  '\x01'
#define DB_SHAPESTYLE '\x01'  /* Yep.  Also 1 -- but for a style tab rec. */
#define DB_PSPACE	  '\x02'
// #define DB_ADELOCKED  '\x04'  Apparently no longer used (see LWPOLYLINE)
#define DB_DELETED	  '\x80'

/*
**	Macros for working with points and vectors.
*/
#define DB_DIST(P0,P1) sqrt(((P1)[0]-(P0)[0])*((P1)[0]-(P0)[0])+((P1)[1]-(P0)[1])*((P1)[1]-(P0)[1])+((P1)[2]-(P0)[2])*((P1)[2]-(P0)[2]))
#define DB_PTCPY(P0,P1) (P0)[0]=(P1)[0],(P0)[1]=(P1)[1],(P0)[2]=(P1)[2]
#define DB_VLEN(V) sqrt((V)[0]*(V)[0]+(V)[1]*(V)[1]+(V)[2]*(V)[2])
#define DB_VSCALE(V,S) (V)[0]*=(S),(V)[1]*=(S),(V)[2]*=(S)


/*
**	db_handitem ref types (db_hireflink).  (Let 0 mean undefined.)
*/
#define DB_SOFT_POINTER 1
#define DB_HARD_POINTER 2
#define DB_SOFT_OWNER	3
#define DB_HARD_OWNER	4


/*
**	HATCH edge types.  (We invented one for polyline so that it
**	can be treated like the others.  For some reason, ACAD treats it
**	separately.)
*/
#define DB_HATCHEDGEUNDEFINED  0
#define DB_HATCHEDGELINE	   1
#define DB_HATCHEDGEARC 	   2
#define DB_HATCHEDGEELLARC	   3
#define DB_HATCHEDGESPLINE	   4
#define DB_HATCHEDGEPLINE	1000


/*
**	The db_hitypeinfo[].typetype codes.
*/
#define DB_TABLE  0
#define DB_ENTITY 1
#define DB_OBJECT 2

/*
**	Table types -- indices into db_drawing::table[].
**	(The ...TAB and ...TABREC defines LOOK redundant, but they're not.
**	 The ...TABREC defines DON'T have to be 0-9;
**	 the ...TAB    defines DO	 have to be 0-9.)
**
**	If we add a new table someday, its ...TAB (table type) will be 10 --
**	but its RECORDs' ...TABREC (handitem type) will be in the 40's.
*/
#define DB_APPIDTAB 	0
#define DB_BLOCKTAB 	1
#define DB_DIMSTYLETAB	2
#define DB_LAYERTAB 	3
#define DB_LTYPETAB 	4
#define DB_STYLETAB 	5
#define DB_UCSTAB		6
#define DB_VIEWTAB		7
#define DB_VPORTTAB 	8
#define DB_VXTAB		9

#define DB_NTABS	   10  /* The number of tables (must match above list) */

/*
**	The class db_handitem types (0-based for array indexing).
**	(KEEP THIS CONSISTENT WITH db_hitypeinfo[].)
*/
#define DB_APPIDTABREC		   0
#define DB_BLOCKTABREC		   1
#define DB_DIMSTYLETABREC	   2
#define DB_LAYERTABREC		   3
#define DB_LTYPETABREC		   4
#define DB_STYLETABREC		   5
#define DB_UCSTABREC		   6
#define DB_VIEWTABREC		   7
#define DB_VPORTTABREC		   8
#define DB_VXTABREC 		   9
#define DB_3DFACE			  10
#define DB_3DSOLID			  11
#define DB_ARC				  12
#define DB_ATTDEF			  13
#define DB_ATTRIB			  14
#define DB_BLOCK			  15
#define DB_BODY 			  16
#define DB_CIRCLE			  17
#define DB_DIMENSION		  18
#define DB_ELLIPSE			  19
#define DB_ENDBLK			  20
#define DB_INSERT			  21
#define DB_LEADER			  22
#define DB_LINE 			  23
#define DB_MLINE			  24
#define DB_MTEXT			  25
#define DB_OLE2FRAME		  26
#define DB_POINT			  27
#define DB_POLYLINE 		  28
#define DB_RAY				  29
#define DB_REGION			  30
#define DB_SEQEND			  31
#define DB_SHAPE			  32
#define DB_SOLID			  33
#define DB_SPLINE			  34
#define DB_TEXT 			  35
#define DB_TOLERANCE		  36
#define DB_TRACE			  37
#define DB_VERTEX			  38
#define DB_VIEWPORT 		  39
#define DB_XLINE			  40
#define DB_HATCH			  41
#define DB_IMAGE			  42
#define DB_LWPOLYLINE		  43
#define DB_DICTIONARY		  44
#define DB_DICTIONARYVAR	  45
#define DB_GROUP			  46
#define DB_IDBUFFER 		  47
#define DB_IMAGEDEF 		  48
#define DB_IMAGEDEF_REACTOR   49
#define DB_MLINESTYLE		  50
#define DB_ACAD_PROXY_OBJECT  51
#define DB_RASTERVARIABLES	  52
#define DB_SPATIAL_FILTER	  53
#define DB_XRECORD			  54
#define DB_ACAD_PROXY_ENTITY  55
#define DB_LAYER_INDEX		  56
#define DB_OBJECT_PTR		  57
#define DB_SORTENTSTABLE	  58
#define DB_SPATIAL_INDEX	  59
#define DB_PLOTSETTINGS 	  60
#define DB_LAYOUT			  61
#define DB_PLACEHOLDER		  62
#define DB_DICTIONARYWDFLT	  63
#define DB_WIPEOUTVARIABLES   64
#define DB_ARCALIGNEDTEXT	  65
#define DB_RTEXT			  66
#define DB_WIPEOUT			  67
#define DB_VBAPROJECT		  68
#define DB_DIMASSOC 		  69  // EBATECH(CNBR) 2002/7/10 Bugzilla#78218
#define DB_NHITYPES 		  70  // Must match above list.


/*DG - 26.4.2002*/
// below the following table are indices of stock objects handles stored in db_drawing::stockhand;
// (about ODT defines see docs about adCtrlObjhandlePtr and adShadowObjhandlePtr;
//	see also docs about adNewFile)
// CNBR 2002/10/30 Match ODT2.009 definition.
/*
-----------------------------------------------------------------------------------------------------------
					  | i |
					  | n |
 according ODT define | d | according DB object
					  | e |
					  | x |
-----------------------------------------------------------------------------------------------------------
BLKCTRL 			   0   Block control object
LAYERCTRL			   1   Layer control object
SHPFILECTRL 		   2   Shapefile (style) control object
LTYPECTRL			   3   Linetype control object
VIEWCTRL			   4   View control object
UCSCTRL 			   5   UCS control object
VPORTCTRL			   6   Viewport control object
REGAPPCTRL			   7   Registered application control object
DIMSTYLECTRL		   8   Dimension style control object
VPENTCTRL			   9   Viewport entity control object
MSPACEBLKSHADOW 	   10  Modelspace blockheader
PSPACEBLKSHADOW 	   11  Paperspace blockheader
MSPACEBLKENTSHADOW	   12  Modelspace BLOCK entity
MSPACEENDBLKENTSHADOW  13  Modelspace ENDBLK entity
PSPACEBLKENTSHADOW	   14  Paperspace BLOCK entity
PSPACEENDBLKENTSHADOW  15  Paperspace ENDBLK entity
BYBLOCKLTPSHADOW	   16  BYBLOCK linetype
BYLAYERLTPSHADOW	   17  BYLAYER linetype
					   18  CONTINUOUS linetype
					   19  Layer 0
					   20  Viewport *ACTIVE*
					   21  Registered application ACAD
					   22  Shapefile (style) STANDARD
					   23  Dimension style STANDARD
DICTIONARYSHADOW	   24  Main dictionary object
GROUPDICSHADOW		   25  Group dictionary object	("ACAD_GROUP" entry in main dictionary)
MLINEDICSHADOW		   26  Multi-line style dictionary object  ("ACAD_MLINESTYLE" entry in main dictionary)
					   27  Multi-line style STANDARD  ("Standard" entry in ACAD_MLINESTYLE dictionary)
					   28  Placeholder object  ("Normal" entry in ACAD_PLOTSTYLENAME dictionarywithdefault)
LAYOUTDICSHADOW		   29  Layout dictionary object  ("ACAD_LAYOUT" entry in main dictionary)
PLOTSTYLEDICSHADOW	   30  Dictionary with default object  ("ACAD_PLOTSTYLENAME" entry in main dictionary)
					   31  Model Space layout object  (layout referenced by Modelspace blockheader)
					   32  Paper Space layout object  (layout referenced by Paperspace blockheader)
PLOTSETTINGSDICSHADOW  33  Plotsettings dictionary object  ("ACAD_PLOTSETTINGS" entry in main dictionary)
					   34  Wipeoutvariables object	(optional, "ACAD_WIPEOUT_VARS" entry in main dictionary)
*/

#define DB_SHI_TAB_BLKREC		  0
#define DB_SHI_TAB_LAYER		  1
#define DB_SHI_TAB_STYLE		  2
#define DB_SHI_TAB_LTYPE		  3
#define DB_SHI_TAB_VIEW 		  4
#define DB_SHI_TAB_UCS			  5
#define DB_SHI_TAB_VPORT		  6
#define DB_SHI_TAB_APPID		  7
#define DB_SHI_TAB_DIMSTYLE 	  8
#define DB_SHI_TAB_VX			  9
#define DB_SHI_BLKREC_MSPACE	 10
#define DB_SHI_BLKREC_PSPACE	 11
#define DB_SHI_BLK_MSPACE		 12
#define DB_SHI_ENDBLK_MSPACE	 13
#define DB_SHI_BLK_PSPACE		 14
#define DB_SHI_ENDBLK_PSPACE	 15
#define DB_SHI_LTYPE_BB 		 16
#define DB_SHI_LTYPE_BL 		 17
#define DB_SHI_LTYPE_CONT		 18
#define DB_SHI_LAYER_0			 19
#define DB_SHI_VPORT_STARACTIVE  20
#define DB_SHI_APPID_ACAD		 21
#define DB_SHI_STYLE_STD		 22
#define DB_SHI_DIMSTYLE_STD 	 23
#define DB_SHI_DICT_NO			 24
#define DB_SHI_DICT_GROUP		 25
#define DB_SHI_DICT_MLS 		 26
#define DB_SHI_MLS_STD			 27
#define DB_SHI_PLACEHOLDER		 28
#define DB_SHI_LAYOUTDIC		 29
#define DB_SHI_DICWDFLT 		 30
#define DB_SHI_MSLAYOUT 		 31
#define DB_SHI_PSLAYOUT 		 32
#define DB_SHI_PLOTSETTINGS 	 33
#define DB_SHI_WIPEOUTVARIABLES  34
#define DB_NSTOCKHANDS			 35


/*
**	Indices into db_sysvar[].  KEEP THEM CONSISTENT WITH THAT ARRAY.
**	(See notes at the top of the definition of db_sysvar[].)
**	BE SURE TO CHANGE ALL HYPHENS IN THE NAMES TO UNDERBARS
**	(RE-INIT ==> RE_INIT).
*/

/*
Sub Renumber()
'DESCRIPTION: Renumber the defines


'Begin Recording
	for i=0 to 383
		ActiveDocument.Selection.EndOfLine
		ActiveDocument.Selection.WordLeft dsExtend
		ActiveDocument.Selection.Text = CSTR( i)
		ActiveDocument.Selection.StartOfLine dsFirstText
		ActiveDocument.Selection.LineDown
	next

'End Recording
End Sub
*/

#define DB_QACADLSPASDOC          0			   // Add by Ebatech CNBR 28/12/2001
#define DB_QACADPREFIX	          1
#define DB_QACADVER 	          2
#define DB_QACISOUTVER	          3
#define DB_QAFLAGS		          4
#define DB_QANGBASE 	          5
#define DB_QANGDIR		          6
#define DB_QAPBOX		          7
#define DB_QAPERTURE	          8
#define DB_QAREA		          9
#define DB_QATTDIA		         10
#define DB_QATTMODE 	         11
#define DB_QATTREQ		         12
#define DB_QAUDITCTL	         13
#define DB_QAUNITS		         14
#define DB_QAUPREC		         15
#define DB_QAUTOMENULOAD         16
#define DB_QAUTOSNAP	         17
#define DB_QAXISMODE	         18
#define DB_QAXISUNIT	         19
#define DB_QBACKGROUNDPLOT		 20
#define DB_QBACKZ		         21
#define DB_QBASEFILE	         22
#define DB_QBINDTYPE	         23				// Add by Ebatech CNBR 28/12/2001
#define DB_QBKGCOLOR	         24
#define DB_QBKGREDRAW	         25
#define DB_QBLIPMODE	         26
#define DB_QCDATE		         27
#define DB_QCECOLOR 	         28
#define DB_QCELTSCALE	         29
#define DB_QCELTYPE 	         30
#define DB_QCELWEIGHT	         31				// Add by Ebatech CNBR 28/12/2001
#define DB_QCEPSNTYPE	         32				// Add by Ebatech CNBR 13/01/2002
#define DB_QCHAMFERA	         33
#define DB_QCHAMFERB	         34
#define DB_QCHAMFERC	         35
#define DB_QCHAMFERD	         36
#define DB_QCHAMMODE	         37
#define DB_QCIRCLERAD	         38
#define DB_QCLAYER		         39
#define DB_QCMDACTIVE	         40
#define DB_QCMDDIA		         41
#define DB_QCMDECHO 	         42
#define DB_QCMDLNTEXT	         43
#define DB_QCMDNAMES	         44
#define DB_QCMLJUST 	         45
#define DB_QCMLSCALE	         46
#define DB_QCMLSTYLE	         47
#define DB_QCOLORX		         48
#define DB_QCOLORY		         49
#define DB_QCOLORZ		         50
#define DB_QCOMPASS 	         51				// Add by Ebatech CNBR 28/12/2001
#define DB_QCOORDS		         52
#define DB_QCPLOTSTYLE	         53				// Add by Ebatech CNBR 28/12/2001
#define DB_QCPROFILE	         54				// Add by Ebatech CNBR 28/12/2001
#define DB_QCTAB		         55				// Add by Ebatech CNBR 28/12/2001
#define DB_QCTRLMOUSE	         56				// Add by SystemMetrix(Maeda) 2001.09.03
#define DB_QCURSORSIZE	         57
#define DB_QCVPORT		         58
#define DB_QCYCLECURR	         59
#define DB_QCYCLEKEEP	         60
#define DB_QDATE		         61
#define DB_QDBMOD		         62
#define DB_QDCTCUST 	         63
#define DB_QDCTMAIN 	         64
#define DB_QDEFLPLSTYLE          65				// Add by Ebatech CNBR 28/12/2001
#define DB_QDEFPLSTYLE	         66				// Add by Ebatech CNBR 28/12/2001
#define DB_QDELOBJ		         67
#define DB_QDEMANDLOAD	         68				// Add by Ebatech CNBR 28/12/2001
#define DB_QDIASTAT 	         69
#define DB_QDIMADEC		         70				// Add by Ebatech CNBR 7/2/2003
#define DB_QDIMALT		         71
#define DB_QDIMALTD 	         72
#define DB_QDIMALTF 	         73
#define DB_QDIMALTRND	         74				// Add by Ebatech CNBR 28/12/2001
#define DB_QDIMALTTD	         75
#define DB_QDIMALTTZ	         76
#define DB_QDIMALTU 	         77
#define DB_QDIMALTZ 	         78
#define DB_QDIMAPOST	         79
#define DB_QDIMASO		         80
#define DB_QDIMASSOC			 81				// Add by Ebatech CNBR 28/12/2001 - read from object dictionary
#define DB_QDIMASZ		         82
#define DB_QDIMATFIT	         83				// EBATECH(watanabe) dimatfit
#define DB_QDIMAUNIT	         84
#define DB_QDIMAZIN 	         85				// Add by Ebatech CNBR 28/12/2001
#define DB_QDIMBLK		         86
#define DB_QDIMBLK1 	         87
#define DB_QDIMBLK2 	         88
#define DB_QDIMCEN		         89
#define DB_QDIMCLRD 	         90
#define DB_QDIMCLRE 	         91
#define DB_QDIMCLRT 	         92
#define DB_QDIMDEC		         93
#define DB_QDIMDLE		         94
#define DB_QDIMDLI		         95
#define DB_QDIMDSEP		         96				// Add by Ebatech CNBR 28/12/2001 -> 28/3/2003
#define DB_QDIMEXE		         97
#define DB_QDIMEXO		         98
#define DB_QDIMFIT		         99
#define DB_QDIMFRAC 	        100				// EBATECH(watanabe)-dimfrac
#define DB_QDIMGAP		        101
#define DB_QDIMJUST             102
#define DB_QDIMLDRBLK	        103				// EBATECH(watanabe)-dimldrblk
#define DB_QDIMLFAC 	        104
#define DB_QDIMLIM		        105
#define DB_QDIMLUNIT	        106				// EBATECH(watanabe)-dimlunit
#define DB_QDIMLWD		        107				// Add by Ebatech CNBR 28/12/2001
#define DB_QDIMLWE		        108				// Add by Ebatech CNBR 28/12/2001
#define DB_QDIMPOST 	        109
#define DB_QDIMRND		        110
#define DB_QDIMSAH		        111
#define DB_QDIMSCALE	        112
#define DB_QDIMSD1		        113
#define DB_QDIMSD2		        114
#define DB_QDIMSE1		        115
#define DB_QDIMSE2		        116
#define DB_QDIMSHO		        117
#define DB_QDIMSOXD 	        118
#define DB_QDIMSS		        119
#define DB_QDIMSSPOS	        120
#define DB_QDIMSTYLE	        121
#define DB_QDIMTAD		        122
#define DB_QDIMTDEC 	        123
#define DB_QDIMTFAC 	        124
#define DB_QDIMTIH		        125
#define DB_QDIMTIX		        126
#define DB_QDIMTM		        127
#define DB_QDIMTMOVE	        128			// EBATECH(watanabe)dimtmove
#define DB_QDIMTOFL 	        129
#define DB_QDIMTOH		        130
#define DB_QDIMTOL		        131
#define DB_QDIMTOLJ 	        132
#define DB_QDIMTP		        133
#define DB_QDIMTSZ		        134
#define DB_QDIMTVP		        135
#define DB_QDIMTXSTY	        136
#define DB_QDIMTXT		        137
#define DB_QDIMTZIN 	        138
#define DB_QDIMUNIT 	        139
#define DB_QDIMUPT		        140
#define DB_QDIMZIN		        141
#define DB_QDISPSILH	        142
#define DB_QDISTANCE	        143
#define DB_QDONUTID 	        144
#define DB_QDONUTOD 	        145
#define DB_QDRAGMODE	        146
#define DB_QDRAGOPEN	        147
#define DB_QDRAGP1		        148
#define DB_QDRAGP2		        149
#define DB_QDWGCHECK	        150		// Add by Ebatech CNBR 28/12/2001
#define DB_QDWGCODEPAGE         151		// Swap order Ebatech CNBR 06/15/2002
#define DB_QDWGNAME 	        152
#define DB_QDWGPREFIX	        153
#define DB_QDWGTITLED	        154
#define DB_QEDGEMODE	        155
#define DB_QELEVATION	        156
#define DB_QERRNO		        157
#define DB_QEXEDIR		        158			  // Add by Ebatech CNBR 28/12/2001
#define DB_QEXPERT		        159
#define DB_QEXPLEVEL	        160
#define DB_QEXPLMODE	        161
#define DB_QEXPLSTBLK	        162
#define DB_QEXPLSTDS	        163
#define DB_QEXPLSTLAY	        164
#define DB_QEXPLSTLT	        165
#define DB_QEXPLSTTS	        166
#define DB_QEXPLSTUCS	        167
#define DB_QEXPLSTVW	        168
#define DB_QEXTMAX		        169
#define DB_QEXTMIN		        170
#define DB_QEXTNAMES	        171			  // Add by Ebatech CNBR 28/12/2001
#define DB_QFACETRATIO	        172			  // Add by Ebatech CNBR 28/12/2001
#define DB_QFACETRES	        173
#define DB_QFASTZOOM	        174
#define DB_QFFLIMIT 	        175
#define DB_QFIELDDISPLAY        176
#define DB_QFILEDIA 	        177
#define DB_QFILLETRAD	        178
#define DB_QFILLMODE	        179
#define DB_QFITTYPE 	        180
#define DB_QFLATLAND	        181
#define DB_QFONTALT 	        182
#define DB_QFONTMAP 	        183
#define DB_QFRONTZ		        184
#define DB_QFULLOPEN	        185			  // Add by Ebatech CNBR 28/12/2001
#define DB_QGLOBCHECK	        186
#define DB_QGPSLASTPT	        187
#define DB_QGPSMODE 	        188
#define DB_QGPSPAUSED	        189
#define DB_QGPSPLAYED	        190
#define DB_QGRIDMODE	        191
#define DB_QGRIDUNIT	        192
#define DB_QGRIPBLOCK	        193
#define DB_QGRIPCOLOR	        194
#define DB_QGRIPHOT 	        195
#define DB_QGRIPHOVER 	        196
#define DB_QGRIPOBJLIMIT        197
#define DB_QGRIPS		        198
#define DB_QGRIPSIZE	        199
#define DB_QGRIPTIPS	        200
#define DB_QHALOGAP				201				// Add by Ebatech CNBR 28/12/2001 - read from object dictionary
#define DB_QHANDLES 	        202
#define DB_QHANDSEED	        203
#define DB_QHANDUDCOPY	        204
#define DB_QHIDEPRECISION       205				// Add by Ebatech CNBR 28/12/2001 - read from object dictionary
#define DB_QHIDETEXT			206				// Add by Ebatech CNBR 28/12/2001
#define DB_QHIGHLIGHT	        207
#define DB_QHPANG		        208
#define DB_QHPASSOC		        209
#define DB_QHPBOUND 	        210
#define DB_QHPDOUBLE	        211
#define DB_QHPDRAWORDER	        212
#define DB_QHPGAPTOL	        213
#define DB_QHPNAME		        214
#define DB_QHPSCALE 	        215
#define DB_QHPSPACE 	        216
#define DB_QHPSTYLE 	        217
#define DB_QHYPERLINKBASE       218
#define DB_QICADVER 	        219
#define DB_QIMAGEHLT	        220			  // Add by Ebatech CNBR 28/12/2001
#define DB_QINDEXCTL			221			  // Read from object dictionary
#define DB_QINETLOCATION        222			  // Add by Ebatech CNBR 28/12/2001
#define DB_QINSBASE 	        223
#define DB_QINSNAME 	        224
#define DB_QINSUNITS	        225			  // Add by Ebatech CNBR 28/12/2001
#define DB_QINSUNITSDEFSOURCE	226		  // Add by Ebatech CNBR 28/12/2001
#define DB_QINSUNITSDEFTARGET	227		  // Add by Ebatech CNBR 28/12/2001
#define DB_QINTERSECTIONCOLOR	228		  
#define DB_QISAVEBAK	   		229			  // Add by Ebatech CNBR 28/12/2001
#define DB_QISAVEPERCENT        230
#define DB_QISLASTCMDPT         231
#define DB_QISOLINES	        232
#define DB_QLASTANGLE	        233
#define DB_QLASTCMDANG	        234
#define DB_QLASTCMDPT	        235
#define DB_QLASTPOINT	        236
#define DB_QLASTPROMPT	        237
#define DB_QLAYOUTREGENCTRL		238	  // Add by Ebatech CNBR 28/12/2001
#define DB_QLENSLENGTH	        239
#define DB_QLIMCHECK	        240
#define DB_QLIMMAX		        241
#define DB_QLIMMIN		        242
#define DB_QLISPINIT	   		243	  // Add by Ebatech CNBR 28/12/2001
#define DB_QLOCALE		        244
#define DB_QLOCALROOTPREFIX	    245
#define DB_QLOGFILEMODE         246
#define DB_QLOGFILENAME         247
#define DB_QLOGFILEPATH    		248	  // Add by Ebatech CNBR 28/12/2001
#define DB_QLOGINNAME	        249
#define DB_QLONGFNAME	        250
#define DB_QLTSCALE 	        251
#define DB_QLTSCLWBLK	        252
#define DB_QLUNITS		        253
#define DB_QLUPREC		        254
#define DB_QLWDEFAULT	   		255	  // Add by Ebatech CNBR 28/12/2001
#define DB_QLWDISPLAY	   		256	  // Add by Ebatech CNBR 28/12/2001
#define DB_QLWPOLYOUT	        257
#define DB_QLWUNITS 	   		258	  // Add by Ebatech CNBR 28/12/2001
#define DB_QMACROREC	        259
#define DB_QMACROTRACE	        260
#define DB_QMAKEBAK 	        261
#define DB_QMAXACTVP	        262
#define DB_QMAXOBJMEM	        263
#define DB_QMAXSORT 	        264
#define DB_QMBUTTONPAN	        265
#define DB_QMEASUREINIT    		266		// Add by Ebatech CNBR 28/12/2001
#define DB_QMEASUREMENT         267
#define DB_QMENUCTL 	        268
#define DB_QMENUECHO	        269
#define DB_QMENUNAME	        270
#define DB_QMIRRTEXT	        271
#define DB_QMODEMACRO	        272
#define DB_QMTEXTED 	        273
#define DB_QMTEXTFIXED 	        274
#define DB_QMTJIGSTRING	        275
#define DB_QMYDOCUMENTSPREFIX   276
#define DB_QNFILELIST	        277
#define DB_QNOMUTT		   		278		// Add by Ebatech CNBR 28/12/2001
#define DB_QOBSCUREDCOLOR		279		// Read from object dictionary
#define DB_QOBSCUREDLTYPE		280		// Read from object dictionary
#define DB_QOFFSETDIST	        281
#define DB_QOFFSETGAPTYPE  		282		// Add by Ebatech CNBR 28/12/2001
#define DB_QOLEHIDE 	   		283		// Add by Ebatech CNBR 28/12/2001
#define DB_QOLEQUALITY	   		284		// Add by Ebatech CNBR 28/12/2001
#define DB_QOLESTARTUP	   		285		// Add by Ebatech CNBR 28/12/2001
#define DB_QORTHOMODE	        286
#define DB_QOSMODE		        287
#define DB_QOSNAPCOORD	   		288		// Add by Ebatech CNBR 28/12/2001
#define DB_QOSNAPHATCH	   		289	 
#define DB_QPALETTEOPAQUE  		290	 
#define DB_QPAPERUPDATE    		291		// Add by Ebatech CNBR 28/12/2001
#define DB_QPDMODE		        292
#define DB_QPDSIZE		        293
#define DB_QPEDITACCEPT	        294
#define DB_QPELLIPSE	        295
#define DB_QPERIMETER	        296
#define DB_QPFACEVMAX	        297
#define DB_QPHANDLE 	        298
#define DB_QPICKADD 	        299
#define DB_QPICKAUTO	        300
#define DB_QPICKBOX 	        301
#define DB_QPICKDRAG	        302
#define DB_QPICKFIRST	        303
#define DB_QPICKSTYLE	        304
#define DB_QPLATFORM	        305
#define DB_QPLINEGEN	        306
#define DB_QPLINETYPE	        307
#define DB_QPLINEWID	        308
#define DB_QPLOTID		        309
#define DB_QPLOTOFFSET	        310
#define DB_QPLOTROTMODE         311
#define DB_QPLOTTER 	        312
#define DB_QPLQUIET 	   		313	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPOLARADDANG    		314	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPOLARANG	   		315	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPOLARDIST	   		316	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPOLARMODE	   		317	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPOLYSIDES	        318
#define DB_QPOPUPS		        319
#define DB_QPRODUCT 	   		320	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPROGBAR 	        321
#define DB_QPROGRAM 	   		322	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPROJECTNAME			323  // Read from object dictionary
#define DB_QPROJMODE	        324
#define DB_QPROMPTMENU	        325
#define DB_QPROXYGRAPHICS       326
#define DB_QPROXYNOTICE    		327	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPROXYSHOW	   		328	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPROXYWEBSEARCH 		329	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPSLTSCALE	        330
#define DB_QPSPROLOG	        331
#define DB_QPSQUALITY	        332
#define DB_QPSTYLEMODE	   		333	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPSTYLEPOLICY   		334	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPSVPSCALE	   		335	 // Add by Ebatech CNBR 28/12/2001
#define DB_QPUCSBASE	   		336	 // Add by Ebatech CNBR 28/12/2001
#define DB_QP_ELEVATION         337
#define DB_QP_EXTMAX	        338
#define DB_QP_EXTMIN	        339
#define DB_QP_INSBASE	        340
#define DB_QP_LIMCHECK	        341
#define DB_QP_LIMMAX	        342
#define DB_QP_LIMMIN	        343
#define DB_QP_UCSNAME	        344
#define DB_QP_UCSORG	        345
#define DB_QP_UCSXDIR	        346
#define DB_QP_UCSYDIR	        347
#define DB_QQAFLAGS 	        348
#define DB_QQTEXTMODE	        349
#define DB_QRASTERDPI	        350
#define DB_QRASTERPREVIEW       351
#define DB_QREFEDITNAME    		352	 // Add by Ebatech CNBR 28/12/2001
#define DB_QREGENMODE	   		353
#define DB_QREMEMBERFOLDERS		354	 // Add by Ebatech CNBR 28/12/2001
#define DB_QREPORTERROR			355	 
#define DB_QRE_INIT 	   		356
#define DB_QROAMABLEROOTPREFIX 	357
#define DB_QRTDISPLAY	   		358	 // Add by Ebatech CNBR 28/12/2001
#define DB_QSAVEFILE	        359
#define DB_QSAVEFILEPATH   		360	 // Add by Ebatech CNBR 20/06/2002
#define DB_QSAVEIMAGES	        361
#define DB_QSAVENAME	        362
#define DB_QSAVEROUNDTRIP       363
#define DB_QSAVETIME	        364
#define DB_QSCREENBOXES         365
#define DB_QSCREENMODE	        366
#define DB_QSCREENPOS	        367
#define DB_QSCREENSIZE	        368
#define DB_QSCRLHIST	        369
#define DB_QSDI 		   		370	 // Add by Ebatech CNBR 28/12/2001
#define DB_QSHADEDGE	        371
#define DB_QSHADEDIF	        372
#define DB_QSHORTCUTMENU   		373	 // Add by Ebatech CNBR 28/12/2001
#define DB_QSHPNAME 	        374
#define DB_QSIGWARN 	        375
#define DB_QSKETCHINC	        376
#define DB_QSKPOLY		   		377
#define DB_QSNAPALLVIEWS   		378	// Modified CyberAge VSB 12/05/2001 / Implementation of Flyover Snapping.
#define DB_QSNAPANG 	        379
#define DB_QSNAPBASE	        380
#define DB_QSNAPCOLOR	        381
#define DB_QSNAPISOPAIR         382
#define DB_QSNAPMODE	        383
#define DB_QSNAPSIZE	        384
#define DB_QSNAPSTYL	        385
#define DB_QSNAPTHICKNESS       386
#define DB_QSNAPTYPE	   		387			// Add by Ebatech CNBR 28/12/2001
#define DB_QSNAPUNIT	        388
#define DB_QSOLIDCHECK	   		389			// Add by Ebatech CNBR 28/12/2001
#define DB_QSORTENTS			390			// Add by Ebatech 30/11/2002 #SORTENTSISDICVAR#  - read from object dictionary
#define DB_QSPLFRAME	   		391
#define DB_QSPLINESEGS	        392
#define DB_QSPLINETYPE	        393
#define DB_QSRCHPATH	        394
#define DB_QSTANDARDSVIOLATION	395	 
#define DB_QSTARTUP				396
#define DB_QSTARTUPTODAY   		397			// Add by Ebatech CNBR 28/12/2001
#define DB_QSURFTAB1	        398
#define DB_QSURFTAB2	        399
#define DB_QSURFTYPE	        400
#define DB_QSURFU		        401
#define DB_QSURFV		        402
#define DB_QSYSCODEPAGE         403
#define DB_QTABMODE 	        404
#define DB_QTARGET		        405
#define DB_QTBCOLOR 	        406
#define DB_QTBSIZE		        407
#define DB_QTDCREATE	        408
#define DB_QTDINDWG 	        409
#define DB_QTDUPDATE	        410
#define DB_QTDUSRTIMER	        411
#define DB_QTDUUPDATE	   		412	 // Add by Ebatech CNBR 28/12/2001
#define DB_QTEMPPREFIX	        413
#define DB_QTEXTANGLE	        414
#define DB_QTEXTEVAL	        415
#define DB_QTEXTFILL	        416
#define DB_QTEXTQLTY	        417
#define DB_QTEXTSIZE	        418
#define DB_QTEXTSTYLE	        419
#define DB_QTHICKNESS	        420
#define DB_QTILEMODE	        421
#define DB_QTIPOFDAY	        422
#define DB_QTOOLTIPS	        423
#define DB_QTRACEWID	        424
#define DB_QTRACKPATH	   		425	 // Add by Ebatech CNBR 28/12/2001
#define DB_QTRAYICONS	   		426
#define DB_QTRAYNOTIFY	   		427
#define DB_QTRAYTIMEOUT	   		428
#define DB_QTREEDEPTH	        429
#define DB_QTREEMAX 	        430
#define DB_QTRIMMODE	        431
#define DB_QTSPACEFAC	   		432	 // Add by Ebatech CNBR 28/12/2001
#define DB_QTSPACETYPE	   		433	 // Add by Ebatech CNBR 28/12/2001
#define DB_QTSTACKALIGN    		434	 // Add by Ebatech CNBR 28/12/2001
#define DB_QTSTACKSIZE	   		435	 // Add by Ebatech CNBR 28/12/2001
#define DB_QUCSAXISANG	   		436	 // Add by Ebatech CNBR 28/12/2001
#define DB_QUCSBASE 	   		437	 // Add by Ebatech CNBR 28/12/2001
#define DB_QUCSFOLLOW	        438
#define DB_QUCSICON 	        439
#define DB_QUCSICONPOS	        440
#define DB_QUCSNAME 	        441
#define DB_QUCSORG		        442
#define DB_QUCSORTHO	        443
#define DB_QUCSVIEW				444
#define DB_QUCSVP		   		445	 // Add by Ebatech CNBR 28/12/2001
#define DB_QUCSXDIR 	        446
#define DB_QUCSYDIR 	        447
#define DB_QUNDOCTL 	        448
#define DB_QUNDOMARKS	        449
#define DB_QUNITMODE	        450
#define DB_QUSERI1		        451
#define DB_QUSERI2		        452
#define DB_QUSERI3		        453
#define DB_QUSERI4		        454
#define DB_QUSERI5		        455
#define DB_QUSERR1		        456
#define DB_QUSERR2		        457
#define DB_QUSERR3		        458
#define DB_QUSERR4		        459
#define DB_QUSERR5		        460
#define DB_QUSERS1		        461
#define DB_QUSERS2		        462
#define DB_QUSERS3		        463
#define DB_QUSERS4		        464
#define DB_QUSERS5		        465
#define DB_QUSRTIMER	        466
#define DB_QVIEWASPECT	        467
#define DB_QVIEWCTR 	        468
#define DB_QVIEWDIR 	        469
#define DB_QVIEWMODE	        470
#define DB_QVIEWSIZE	        471
#define DB_QVIEWTWIST	        472
#define DB_QVISRETAIN	        473
#define DB_QVSMAX		        474
#define DB_QVSMIN		        475
#define DB_QWHIPARC 	   		476	 // Add by Ebatech CNBR 28/12/2001
#define DB_QWHIPTHREAD	   		477	 // Add by Ebatech CNBR 28/12/2001
#define DB_QWMFBKGND	   		478	 // Add by Ebatech CNBR 28/12/2001
#define DB_QWMFFOREGND	   		479	 // Add by Ebatech CNBR 28/12/2001
#define DB_QWNDBTEXP	        480
#define DB_QWNDLCMD 	        481
#define DB_QWNDLMAIN	        482
#define DB_QWNDLMDI 	        483
#define DB_QWNDLSCRL	        484
#define DB_QWNDLSTAT	        485
#define DB_QWNDLTEXP	        486
#define DB_QWNDLTEXT	        487
#define DB_QWNDPCMD 	        488
#define DB_QWNDPFORM	        489
#define DB_QWNDPMAIN	        490
#define DB_QWNDPPMENU	        491
#define DB_QWNDPTEXP	        492
#define DB_QWNDPTEXT	        493
#define DB_QWNDSCMD 	        494
#define DB_QWNDSFORM	        495
#define DB_QWNDSMAIN	        496
#define DB_QWNDSTEXP	        497
#define DB_QWNDSTEXT	        498
#define DB_QWORLDUCS	        499
#define DB_QWORLDVIEW	        500
#define DB_QWRITESTAT	        501
#define DB_QXCLIPFRAME 			502			// Read from object dictionary
#define DB_QXEDIT		   		503			// Add by Ebatech CNBR 28/12/2001
#define DB_QXFADECTL	   		504			// Add by Ebatech CNBR 28/12/2001
#define DB_QXLOADCTL	        505
#define DB_QXLOADPATH	        506
#define DB_QXREFCTL 	        507
#define DB_QXREFNOTIFY 	        508
#define DB_QXREFTYPE 	        509
#define DB_QZOOMFACTOR	        510
#define DB_QZOOMPERCENT         511
#define DB_Q_LINFO		        512
#define DB_Q_PKSER		        513
#define DB_Q_SERVER 	   		514
#define DB_Q_VENDORNAME    		515		// Added Cybage VSB 08/08/2001 Reason: Fix for Bug No: 77822
#define DB_Q_VERNUM 	   		516

// flags for db_setfontusingmap
enum db_fontsubstmode {
	IC_ALLOW_ICAD_FNT,
	IC_NO_ALLOW_ICAD_FNT
} ;
#endif

// Moved or Removed variables...
//#define DB_QODIMADEC      999	// Move to HEADER    Ebatech CNBR 7/2/2003
//#define DB_QODIMDSEP		999 // Move to HEADER    Ebatech CNBR 28/3/2003
//#define DB_QSORTENTS      999	// Move to OBJECTVAR CNBR 30/11/2002 #SORTENTSISDICVAR#

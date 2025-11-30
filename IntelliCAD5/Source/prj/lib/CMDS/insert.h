#ifndef _INSERT_HEADER
#define _INSERT_HEADER


#define FLAGS_BYTE unsigned char

#define INT2BOOL(X) (0!=(X))
/*=============================================================================
	Meanings of flags for inquiry parameters at an insertion objects   	
           7 6 5 4 3 2 1 0
          |_|_|_|_|_|_|_|_|
             | \_____/ | |____ to request an angle             
             |    |    |______ to request an insertion point   
             |    |___________ to request a scales               
             |                   [0001] for sX [0010] for sY [0100] for sZ
             |                   [1000] for equal scaling (sx=sy=sz)
             |________________ to request a multiple

  ============================================================================*/
#define REQ_ANGLE		0x0001 // To request rotation angle
#define REQ_POINT		0x0002 // To request insertion point

#define REQ_SCALE_X		0x0004 // To request X scale
#define REQ_SCALE_Y		0x0008 // To request Y scale
#define REQ_SCALE_Z		0x0010 // To request Z scale

#define REQ_SCALES		0x001C // To request XYZ scale

//It's while not used:

#define REQ_EQUAL_SCALE 	0x0020 // To request sX=sY=sZ scale

#define REQ_MULTIPLE		0x0040 // To request multiple

/*========================================================================*/


short	xref_add_new( bool isOverlay, sds_resbuf *param=NULL, bool isCropPath=false);
int		xref_reload(resbuf *param);
int		xref_unload(resbuf *param);
int	  cmd_xref_editpath(char *xrefPattern,char *new_path=NULL,bool isCropPath=false, bool isExactName=false);

#endif _INSERT_HEADER



                                               

/* D:\ICADDEV\PRJ\LIB\CMDS\SHAPE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "commandqueue.h" /*DNT*/
#include "styletabrec.h" /*DNT*/

extern char *cmd_FontSavePath;

/* ===========================================================
	LOAD COMMAND : Load a shape file(shx) to the drawing.
	LOAD /? : List loaded shape files to Screen window.
=========================================================== */
/* -----------------------------------------------------------
	short cmd_loadshape( char* testname )

	loading shape file method.
	It calls from LOAD command and LoadShapeFile() method.

	char* testname (i) testing and loading shape file
						(with/without path).

	return
		RTNORM	shx file loaded/already loaded(= ready to use).
		RTERROR	some kinds of error detected.

----------------------------------------------------------- */
short cmd_loadshape(char* testname ) {

	// 1) Search SHX file and Remove Drive & Path(if you can).
	char filename[IC_ACADBUF], fullname[IC_ACADBUF];
	char* ptr;

	ChangeSlashToBackslashAndRemovePipe(testname);
rescan:
	if( sds_findfile( testname, fullname ) == RTNORM ){
		if(( ptr = _tcsrchr( (TCHAR*)fullname, IC_SLASH )) == NULL ){
			strcpy( filename, fullname );
		}else if( strisame(testname, fullname )){
			strcpy( filename, ptr+1 );
		}else{
			strcpy( filename, fullname );
		}
	}else{
		if(( ptr = _tcsrchr( (TCHAR*)testname, IC_SLASH )) == NULL ){
			sds_printf(ResourceString(IDC_LOAD_NOT_FOUND,"\nFile \"%s\" not found."),testname);
			return(RTERROR);
		}else{
			strcpy( testname, ptr+1 );	// filename has no path
			goto rescan;
		}
	}
	// 2) Search style record
	db_styletabrec* pShapeStyle;
	int  flag;

	for( pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,1));
		pShapeStyle !=NULL ; pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,0)))
	{
		pShapeStyle->get_70(&flag);
		if(( flag & IC_SHAPEFILE_SHAPES ) && !( flag & IC_ENTRY_XREF_DEPENDENT ))
		{
			if( strisame( filename, pShapeStyle->retp_fontpn()))
			{
				sds_printf(ResourceString(IDC_LOAD_ALREADY_LOADED,"\nFile \"%s\" already loaded."), filename);
				return(RTNORM);
			}
		}
	}
	// 3) Create Style Record
	db_fontlink* pShapeFont;
	// try to load shape font
	// 1 = Resource Font (ICAD.FNT)
	// 0 = Okay (in fontlink Do not free!)
	// <0  Error
	// 2002/5/3 EBATECH(CNBR) pShapeFont leave away if it isn't Shape font.
	if(db_setfont(filename, NULL, &pShapeFont, SDS_CURDWG) != 0)
	{
		sds_printf(ResourceString(IDC_LOAD_FAILED,"\nFile \"%s\" failed to load."),filename);
		return(RTERROR);
	}
	if( strcmp( pShapeFont->desc,"Shape Definition"/*DNT*/))
	{
		sds_printf(ResourceString(IDC_LOAD_NOT_SHAPE,"\nFile \"%s\" is not shape file."),filename);
		return(RTERROR);
	}

	// try to create style table record for this shape
	pShapeStyle = new db_styletabrec();
	pShapeStyle->set_70(IC_SHAPEFILE_SHAPES);
	pShapeStyle->set_3(filename, SDS_CURDWG);
	pShapeStyle->set_font(pShapeFont);
	if(SDS_CURDWG->addhanditem((db_handitem*)pShapeStyle))
	{
		sds_printf(ResourceString(IDC_LOAD_NOMEM,"\nFile \"%s\" can not add."),filename);
		delete pShapeStyle;
		return(RTERROR);
	}
	sds_printf(ResourceString(IDC_LOAD_SHAPE_LOADED,"\nFile \"%s\" loaded.\n"), filename);
	return(RTNORM);
}

/* -----------------------------------------------------------
	short cmd_shapelister(void)

	listing loaded shape file.

	return
		RTNORM	shx file loaded/already loaded(= ready to use).
		RTCAN	user break detected.

----------------------------------------------------------- */
short cmd_shapelister(void) {
	RT	ret;
	char scrollines[IC_ACADBUF];
	int scroll=0,numlines=0;

	db_styletabrec* pShapeStyle;
	int  flag;

	sds_textscr();
	for( pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,1));
		pShapeStyle != NULL ; pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,0)))
	{
		if( sds_usrbrk()) return(RTCAN);

		pShapeStyle->get_70(&flag);
		if(( flag & IC_SHAPEFILE_SHAPES ) && !( flag & IC_ENTRY_XREF_DEPENDENT ))
		{
			sds_printf("\n\t%s", pShapeStyle->retp_fontpn());
			++numlines;
			if (!scroll && numlines % 20 == 0) {
				sds_initget(0, ResourceString(IDC_LOAD_LIST_INIT1, "Scroll ~_Scroll"));
				ret = sds_getkword(ResourceString(IDC_LOAD_LIST_PROMPT, "\nScroll/<ENTER to view next screen>:" ),scrollines);
				switch(ret) {
				case RTNORM:
					scroll=1; break;
				case RTNONE:
					break;
				default:
					return ret;
				}
			}
		}
	}
	if( numlines == 0 )
		sds_printf(ResourceString(IDC_LOAD_LIST_NOFILE, "\n* no file *"));
	return(RTNORM);
}
/* -----------------------------------------------------------
	short cmd_load(void)

	IntelliCAD LOAD command implementation.
	User interface part.

	return
		RTNORM	shx file loaded/already loaded(= ready to use).
		RTCAN	user break detected.
		RTERROR	some kinds of error detected.

----------------------------------------------------------- */
short cmd_load(void) {

    char fs1[IC_ACADBUF];
    struct resbuf rb;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    *fs1=0;
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

    if( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia: ;
		// Display OpenFile Dialog box
        if( sds_getfiled(
        	ResourceString(IDC_LOAD_SHAPE_TITLE, "Load Shape File" ),
        	cmd_FontSavePath,
        	ResourceString(IDC_LOAD_SHAPE_FILTER, "Shape File|shx" ),0,&rb) != RTNORM ){
			return(RTCAN);
		} else if(rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        } else { goto typeit; }
    } else {
		typeit: ;
        if(RTNORM!=sds_getstring(1,ResourceString(IDC_LOAD_SHAPE_PRMPT, "\nShape file to load(? to list): " ),fs1)) {
			return(RTCAN);
		}
        if(*fs1=='~'/*DNT*/) goto dia;
		//check for "?" if so then list loaded shape and return
		if (strisame(fs1,"?"/*DNT*/) || strisame(fs1,"_?"/*DNT*/)) {
			return cmd_shapelister();
		}
	}
    if(*fs1) {
        return cmd_loadshape(fs1);
	}
	return(RTNORM);
}

/* ===========================================================
	SHAPE COMMAND : Insert SHAPE entity into the drawing.
	LOAD /? : List loaded shape files to Screen window.
=========================================================== */
/* -----------------------------------------------------------
	short cmd_shapeobjlister(void)

	listing loaded shape file and shape names.

	return
		RTNORM	shx file loaded/already loaded(= ready to use).
		RTCAN	user break detected.

----------------------------------------------------------- */
short cmd_shapeobjlister(void) {
	RT	ret;
	char scrollines[IC_ACADBUF];
	int idx, cols, numlines;
	db_styletabrec* pShapeStyle;
	db_fontlink* pShapeFile;
	char* pShapeName;
	int  flag;

	sds_textscr();
	numlines = 0;
	for( pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,1));
		pShapeStyle != NULL ; pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,0)))
	{
		if( sds_usrbrk()) return(RTCAN);

		pShapeStyle->get_70(&flag);
		if(( flag & IC_SHAPEFILE_SHAPES ) && !( flag & IC_ENTRY_XREF_DEPENDENT ))
		{
			++numlines;
			sds_printf("\n\t%s :"/*DNT*/, pShapeStyle->retp_fontpn());
			// Display shape names
			pShapeFile = pShapeStyle->ret_font();
			scrollines[0] = '\0';
			for(idx = cols = 0 ; idx < pShapeFile->nchrs ; idx++ )
			{
				pShapeName = pShapeFile->chr[idx].symbolName;
				if( pShapeFile->chr[idx].code != 0 &&
					pShapeName != NULL &&
					strlen(pShapeName) > 0 )
				{
					if(cols % 5 == 0 ){
						if( *scrollines ){
							sds_printf( scrollines );
							scrollines[0] = '\0';
						}
						strcpy(scrollines, "\n\t\t"/*DNT*/);
					}
					strcat(scrollines, pShapeFile->chr[idx].symbolName );
					strcat(scrollines, "\t");
					++cols;
				}
			}
			if( *scrollines ){
				sds_printf( scrollines );
			}
		}
	}
	if( numlines == 0 )
		sds_printf(ResourceString(IDC_LOAD_LIST_NOFILE, "\n* no file *"));
	return(RTNORM);
}
/* -----------------------------------------------------------
	GLOBALS for SHAPE command
----------------------------------------------------------- */
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;

/* -----------------------------------------------------------
	short MakeDragParlink()

	create a temporary shape object to define insert point and
	rotation using user input(dragging).

------------------------------------------------------------ */
short MakeDragParlink(
	char *name,			// (i) shape name             DXF=2
	sds_real *extru,	// (i) extruion vector        DXF=210
	sds_real rotang,	// (i) Initial Rotation Angle DXF=50
	db_handitem* hip,	// (i) Shape STYLE record     DXF=341
	db_shape **shape)	// (o) Temporary SHAPE object
{
	db_shape *elp;
	elp=new db_shape(SDS_CURDWG);
	if (!elp)
		return RTERROR;

	elp->set_tship(hip);
	elp->set_2(name);
	elp->set_210(extru);
	elp->set_50(rotang);
	*shape=elp;
	return RTNORM;
}
/* -----------------------------------------------------------
	short GetInsertionPoint()

	Get Point, Height and Rotation using user input(dragging).

------------------------------------------------------------ */
short GetInsertionPoint(
	sds_point pt1,		// (o) Insertion point in UCS(on screen)
	sds_name ename,		// (i) temporary SHAPE object
	db_shape *elp,		// (i) temporary SHAPE object
	bool draginspt,		// (i) $DRAGMODE
	bool *needscales,	// (o) Height/Width just stored!
	bool *needrotation,	// (o) Rotation just stored!
	sds_real *height,	// (o) Height
	sds_real *width,	// (o) Width
	sds_real *rotang 	// (o) Rotation
){

	char initgetstr[IC_ACADBUF];
	char promptstr[IC_ACADBUF];
	struct resbuf rb;
	short holdorthomode;
	RT ret;
	char kword[IC_ACADBUF];

	do{
		// Restore Prompt and initget parameter
		strcpy(promptstr,ResourceString(IDC_SHAPE_INSERTIONPOINT, "\nInsertion point for shape: " ));
		strcpy(initgetstr,ResourceString(IDC_SHAPE_INITEGET,"Height Width Rotate"));

		if(sds_initget(RSG_NONULL+SDS_RSG_NOCANCEL, initgetstr) != RTNORM ){
			return RTERROR;
		}
		if(!draginspt){
			ret = sds_getpoint( NULL, promptstr, pt1 ); // need to look at this
			return ret;
		}else{
			SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);   // turn ortho off, save what it was
			holdorthomode=rb.resval.rint;
			if ( holdorthomode != 0) {
				rb.resval.rint=0;
				sds_setvar("ORTHOMODE"/*DNT*/,&rb);
			}
			SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			extern bool SDS_ApplyNoOrtho;
			SDS_ApplyNoOrtho=TRUE;

			ret = DoDrag( ename,(double *)elp->retp_10(), promptstr, pt1, rb.resval.rint ); // do the drag

			SDS_ApplyNoOrtho=FALSE;
			if (holdorthomode) { // if ortho was on, turn it back on
				rb.resval.rint=1;
				sds_setvar("ORTHOMODE"/*DNT*/,&rb);
			}

			if (ret==RTKWORD) {  // process the hidden keywords
				if (sds_getinput(kword)!=RTNORM) {
					return RTERROR;
				}
				strupr(kword);
				if (sds_initget(RSG_NONULL,NULL)!=RTNORM) {
					return RTERROR;
				}
				if (strsame(kword,"HEIGHT"/*DNT*/)) {
					if (sds_getdist(NULL,ResourceString(IDC_SHAPE_HIGHT_FACTOR, "\nShape height factor: " ),height)==RTERROR) {
						return RTERROR;
					}
					*needscales=false;
					elp->set_40(*height);
				} else if (strsame(kword,"WIDTH"/*DNT*/)) {
					if (sds_getdist(NULL,ResourceString(IDC_SHAPE_WIDTH_FACTOR, "\nShape width factor: " ),width)==RTERROR) {
						return RTERROR;
					}
					*needscales=false;
					elp->set_41(*width);
				} else if (strsame(kword,"ROTATE"/*DNT*/)) {
					if (sds_getangle(NULL,ResourceString(IDC_SHAPE_DRAG_ROTATIONANGLE, "\nRotation angle for shape: " ),rotang)==RTERROR) {
						return RTERROR;
					}
					*needrotation=false;
					elp->set_50(*rotang);
				} // End of OPTION
			} // End of RTKWORD
		} // End of DRAGMODE
	}while (ret==RTKWORD);
	return ret;
}

/* -----------------------------------------------------------
	short GetShapeParameters()

	Get Point, Height and Rotation using user input(dragging).

------------------------------------------------------------ */
short GetShapeParameters(
	db_shape *elp,		// (io) temporary SHAPE object
	bool	draginspt,	// (i)  $DRAGMODE
	sds_real rotbase,	// (i)  Rotation base angle
	sds_real *curelev,	// (o) Elevation
	sds_real rotang,	// (i)  Rotaition angle
	sds_point retinspt  // (o)  Insertion Point
	) {

	sds_name ename;
	sds_point pt1;			// insertion point in UCS
	RT ret;
	sds_real height;		// SHAPE's height
	sds_real width;			// SHAPE's width
	sds_point tpt;			// insertion point in ECS
	struct resbuf rb;
	struct resbuf setgetrb;	// Current Elevation
	struct resbuf rbent;	// Translation Parameter ECS(for DB)
	struct resbuf rbucs;	// Translation Parameter UCS(on Screen)
	struct resbuf rbwcs;	// Translation Parameter WCS
	bool needscales;
	bool needrotation;

	// Coordinate conversion to/from current UCS
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	// Coordinate conversion to/from WCS
	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;
	// Coordinate conversion to/from ECS
	rbent.restype=RT3DPOINT;
	elp->get_210(rbent.resval.rpoint);

	needscales = true;
	needrotation = true;
	pt1[0]=pt1[1]=pt1[2]=0.0;   // Working point
	ename[0]=(long)elp;			// Working ename
	ename[1]=(long)SDS_CURDWG;
	elp->get_40(&height);		// current height
	elp->get_41(&width);		// current width
	elp->get_50(&rotang);		// current rotation

	// Get parameters on screen
	for (ret=RTERROR;ret!=RTNORM;){
		ret = GetInsertionPoint( pt1, ename, elp, draginspt, &needscales, &needrotation, &height, &width, &rotang );
		if(ret == RTERROR ){
			return ret;
		}else if( ret == RTCAN ){
			SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			return ret;
		}
	}
	ic_ptcpy(retinspt, pt1);
	extern sds_point SDS_PointDisplay;
	ic_ptcpy(SDS_PointDisplay, pt1);

	// Update the parlink so the dragging is displayed correctly.
	sds_trans(pt1, &rbucs, &rbent, 0, tpt);	// UCS->ECS as Point
	// pt1 will be converted on dragging loop below.
	elp->set_10(tpt);

	//get current elevation
	SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	*curelev=setgetrb.resval.rreal;

	if (pt1[2]!=*curelev){
		//temporarily set elevation so angle/scale dragging is correct
		SDS_CorrectElevation=*curelev;
		SDS_ElevationChanged=TRUE;
		setgetrb.resval.rreal=pt1[2];
		setgetrb.restype=RTREAL;
		sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
	}
	// Set LASTPOINT system variable, even if inserting from dialog
	setgetrb.restype=RT3DPOINT;
	ic_ptcpy(setgetrb.resval.rpoint,pt1);
	SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	// get current height
	if (needscales)	{
		if ((ret = GetOneScale(elp, &height, false))!=RTNORM){
			return ret;
		}
		elp->set_40(height);
	}
	// get current rotation
	if (needrotation) {
		if ((ret = GetRotation(ename, elp, pt1, rotbase, false, rotang)) !=RTNORM){
			return ret;
		}
	}
	return RTNORM;
}
/* -----------------------------------------------------------
	short cmd_shape(void)

	IntelliCAD SHAPE command implementation.
	User interface part.

	return
		RTNORM	shx file loaded/already loaded(= ready to use).
		RTCAN	user break detected.
		RTERROR	some kinds of error detected.

----------------------------------------------------------- */
short cmd_shape(void)	{

	char defaultShape[IC_ACADBUF];			// $SHPNAME
	char shapeName[IC_ACADBUF];				// Inserting SHAPE name
	int	shapeCode = 0;						// Inserting SHAPE code
	db_styletabrec* pShapeStyle = NULL;		// STYLE for SHAPE
	db_shape *pShapeObj = NULL;				// SHAPE object
	sds_name ename;
	struct resbuf *elist = NULL;			// SHAPE object
	struct resbuf rb;						// used for $SHPNAME, $DRAGMODE,
	RT ret;									// return status
	bool printit;
	bool dragmode;							// short? 0-2
	sds_real curelev;						// Current Elevation
	sds_real rotang;						// Rotation angle
	sds_real rotbase;						// Rotation base
	sds_point extru;						// Extrusion vector
	sds_point retinspt;						// Insertion Point

	rotang=0.0;

	// 1) Check command status and Get last shape name.
	{
		if (cmd_istransparent() == RTERROR) return RTERROR;

	    if (cmd_iswindowopen() == RTERROR) return RTERROR;

		ret = SDS_getvar(NULL,DB_QSHPNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES); // get default shape name
		if( ret != RTNORM ){
			return ret;
		}
		strncpy(defaultShape,rb.resval.rstring,IC_ACADBUF);
		IC_FREE( rb.resval.rstring );
	}
	// 2) Get Shape name
	{
		char prompt[IC_ACADBUF];

		if( *defaultShape ) {
			sprintf(prompt,ResourceString(IDC_SHAPE_PROMPT_DEFAULT, "\n? to list/Shape to insert <%s>: " ), defaultShape);
		} else {
			strcpy(prompt,ResourceString(IDC_SHAPE_PROMPT_NODEFAULT, "\n? to list/Shape to insert: " ));
		}
		//Get the shape name.
        ret = sds_getstring(0, prompt, shapeName);
        if( ret != RTNORM ){
			return( RTCAN );
		}
		if( *shapeName == '\0' ){
			if( *defaultShape ){
				strcpy( shapeName, defaultShape );
			}else{
				return( RTCAN );
			}
		}
		//check for "?" if so then list loaded shape and return
		if (strisame(shapeName,"?"/*DNT*/) || strisame(shapeName,"_?"/*DNT*/)) {
			return cmd_shapeobjlister();
		}
	}
	// 3) Seek SHAPE definition from STYLE table
	shapeCode = 0;
	for( pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,1));
		pShapeStyle != NULL ; pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,0)))
	{
		int  flag;
		db_fontlink* pShapeFile;

		pShapeStyle->get_70(&flag);
		if(( flag & IC_SHAPEFILE_SHAPES ) && !( flag & IC_ENTRY_XREF_DEPENDENT ))
		{
			pShapeFile = pShapeStyle->ret_font();
			if( shapeCode = db_findCodeByName(pShapeFile, shapeName) != 0 ){
				break;
			}
		}
	}
	if( shapeCode == 0 ) {
		sds_printf( ResourceString(IDC_SHAPE_NOSHAPE_IN_DRAWING, "\nShape %s not found."), shapeName );
		return RTERROR;
	}
	// 4) Prepare temporaly object.
	printit = PrintMode();
	if ((ret = GetExtrusionAndRotation(extru, &rotang, &rotbase)) != RTNORM){
		return ret;
	}

	if((ret = MakeDragParlink(shapeName, extru, rotang, (db_handitem*)pShapeStyle, &pShapeObj )) != RTNORM ){
		return ret;
	}
	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if( rb.resval.rint )
		dragmode=true;
	else
		dragmode=false;

	// 5) Get insertion point, height and rotation of the shpae
	ret = GetShapeParameters(  pShapeObj, dragmode, rotbase, &curelev, rotang, retinspt);
	if( ret != RTNORM ){
		delete pShapeObj;
		return ret;
	}
	// 6) Create actual SHAPE object
	ename[0] = (long)pShapeObj;
	ename[1] = (long)SDS_CURDWG;
	if(( elist = sds_entget( ename )) == NULL ){
		delete pShapeObj;
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		return RTERROR;
	}
	ret = sds_entmake( elist );
	IC_RELRB(elist);
	delete pShapeObj;
	if( ret != RTNORM ){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		return ret;
	}
	// Set ELEVATION system variable.
	if (SDS_ElevationChanged){
		rb.restype=RTREAL;
		rb.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&rb);
		SDS_ElevationChanged=FALSE;
	}

	// Set SHPNAME system variable.
	rb.restype=RTSTR;
	rb.resval.rstring = shapeName;
	if (sds_setvar("SHPNAME"/*DNT*/,&rb)!=RTNORM){
		return RTERROR;
	}
	return RTNORM;
}

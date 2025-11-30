/*  OURBASE.C


.............................................................................

 Copyright (c) 1998 by Visio Corporation.  All rights reserved.

 The Software is subject to the license agreement that accompanies
 or is included with the Software, which specifies the permitted
 and prohibited uses of the Software. Any unauthorized duplication
 or use of Visio Corporation Software, in whole or in part, in print,
 or in any other storage and retrieval system is prohibited.

 To the maximum extent permitted by applicable law, Visio Corporation
 and its suppliers disclaim any and all warranties and conditions,
 either express or implied, including, without limitation, implied
 warranties of merchantability, fitness for a particular purpose,
 title, and non-infringement, and those arising out of usage of trade
 or course of dealing, concerning these materials.  These materials
 are provided "as is" without warranty of any kind.
 .............................................................................
*/


#define DEFINEVARS

#include "ourbase.h"
#include "string.h"


int main(int argc, char **argv) {
    short linkstat;

    sds_init(argc,argv);
    sds_printf("\nLoading OURBASE application.");
	sds_printf("\nType \"INDEX\" to see a list of available tests.");
    ob_init();

    for(;;) {
        if ((linkstat=sds_link(RSRSLT))<0) {
             sds_printf("\nERROR: Unable to link with IntelliCAD - %d", linkstat);
             sds_exit(-1);
        }

        sds_printf("\nLink Status = %d",linkstat);

        switch(linkstat) {
            case RQXLOAD: ob_loadfuncs();  break;
            case RQXUNLD: ob_cleanup();    break;
            case RQSUBR : ob_dofuncs();    break;
            case RQEND  : ob_cleanup();    break;
            case RQQUIT : ob_cleanup();    break;
            case RQSAVE :
            default     : break;
        }
    }
    return(0);
}


/***************************************************************************
Initalizes function names.

RETURNS:(-1) for error.
        ( 0) for success.
****************************************************************************/
short ob_loadfuncs(void) {

#if defined(DEBUG)
   if (!sds_defun("C:DT"          ,1))  return(-1);
   if (!sds_defun("C:T"           ,2))  return(-1);
#endif

   if (!sds_defun("C:OB_BASEABOUT"   ,3))  return(-1);
   if (!sds_defun("C:OB_GETUSER"     ,4))  return(-1);
   if (!sds_defun("C:OB_GEOMETRY"    ,5))  return(-1);
   if (!sds_defun("C:OB_CHARACTER"   ,6))  return(-1);
   if (!sds_defun("C:OB_CONVERT"     ,7))  return(-1);
   if (!sds_defun("C:OB_DISPLAY"     ,8))  return(-1);
   if (!sds_defun("C:OB_GRAPHICS"    ,9))  return(-1);
   if (!sds_defun("C:OB_INITGET"     ,10)) return(-1);
   if (!sds_defun("C:OB_LISP"        ,11)) return(-1);
   if (!sds_defun("C:OB_EXTFUNCS"    ,12)) return(-1);
   if (!sds_defun("C:OB_APPS"        ,13)) return(-1);
   if (!sds_defun("C:OB_ERRORS"      ,14)) return(-1);
   if (!sds_defun("C:OB_MEMORY"      ,15)) return(-1);
   if (!sds_defun("C:OB_QUERYCOM"    ,16)) return(-1);
   if (!sds_defun("C:OB_GETSETVAR"   ,17)) return(-1);
   if (!sds_defun("C:OB_PASSTOLISP"  ,18)) return(-1);
   if (!sds_defun("C:OB_WILDCARD"    ,19)) return(-1);
   if (!sds_defun("C:OB_SELECTION"   ,20)) return(-1);
   if (!sds_defun("C:OB_ENTITIES"    ,21)) return(-1);
   if (!sds_defun("C:OB_EED"         ,22)) return(-1);
   if (!sds_defun("C:OB_SYMBOLTABLE" ,23)) return(-1);
   if (!sds_defun("C:OB_ABORTNOW"	 ,24)) return(-1);
   if (!sds_defun("C:OB_EXITNOW"     ,25)) return(-1);
   if (!sds_defun("C:OB_COMMANDLIST" ,26)) return(-1);
// The "30" functions are tools, not tests.
   if (!sds_defun("C:OB_INDEX"       ,30)) return(-1);
   if (!sds_defun("C:OB_DRAWING"     ,31)) return(-1);
   if (!sds_defun("C:OB_TEXTWIN"     ,32)) return(-1);
// The "40" functions are LISP tests.
   if (!sds_defun("OB_LISPRETINT"    ,40)) return(-1);
   if (!sds_defun("OB_LISPRETREAL"   ,41)) return(-1);
   if (!sds_defun("OB_LISPRETSTR"    ,42)) return(-1);
   if (!sds_defun("OB_LISPRETT"      ,43)) return(-1);
   if (!sds_defun("OB_LISPGETARGS"   ,44)) return(-1);
   if (!sds_defun("OB_LISPRETNIL"    ,45)) return(-1);
   if (!sds_defun("OB_LISPRETPOINT"  ,46)) return(-1);
   if (!sds_defun("OB_LISPRETVOID"   ,47)) return(-1);
   if (!sds_defun("OB_LISPRETVAL"    ,48)) return(-1);
   if (!sds_defun("OB_LISPRETNAME"   ,49)) return(-1);
   if (!sds_defun("OB_LISPRETLIST"   ,50)) return(-1);

//
//   if (!sds_regfunc(ob_func3  ,6)) return(-1);
//
   return(0);
}


/**************************************************************************
This function does the actual calls to the functions that the user types
at command:, This function also reads the the argument buffer by calling
sds_getargs() then passes the arguments to the called functions.

RETURNS:( 0) for success.
        (-1) for error.
        (-2) if user cancels.
**************************************************************************/
short ob_dofuncs(void) {

short         fcode;

    if ((fcode=sds_getfuncode())<0) return(0);

    switch(fcode) {
#if defined(DEBUG)
        case 1 : ob_dt();     sds_retvoid();  break;
        case 2 : ob_test();   sds_retvoid();  break;
		#endif											/* What to enter to use each function: */
        case 3 : ob_about();  sds_retvoid(); break;
        case 4 : ob_func1();  sds_retvoid(); break;		/* OB_USERINPUT	    */
        case 5 : ob_func2();  sds_retvoid(); break;		/* OB_GEOMETRY	    */
        case 6 : ob_func3();  sds_retvoid(); break;		/* OB_CHARACTER	    */
        case 7 : ob_func4();  sds_retvoid(); break;		/* OB_CONVERT 	    */
        case 8 : ob_func5();  sds_retvoid(); break;		/* OB_DISPLAY 	    */
        case 9 : ob_func6();  sds_retvoid(); break;		/* OB_GRAPHICS 	    */
        case 10: ob_func7();  sds_retvoid(); break;		/* OB_INITGET   	*/
        case 11: ob_func8();  sds_retvoid(); break;		/* OB_LISP 	    	*/
        case 12: ob_func9();  sds_retvoid(); break;		/* OB_EXTFUNCS  	*/
        case 13: ob_func10(); sds_retvoid(); break;		/* OB_APPS      	*/
        case 14: ob_func11(); sds_retvoid(); break;		/* OB_ERRORS	 	*/
        case 15: ob_func12(); sds_retvoid(); break;		/* OB_MEMORY	 	*/
        case 16: ob_func13(); sds_retvoid(); break;		/* OB_QUERYCOM	 	*/
        case 17: ob_func14(); sds_retvoid(); break;		/* OB_GETSETVAR 	*/
        case 18: ob_func15(); sds_retvoid(); break;		/* OB_PASSTOLISP 	*/
        case 19: ob_func16(); sds_retvoid(); break;		/* OB_WILDCARD 	    */
        case 20: ob_func17(); sds_retvoid(); break;		/* OB_SELECTION 	*/
        case 21: ob_func18(); sds_retvoid(); break;		/* OB_ENTITIES 	    */
        case 22: ob_func19(); sds_retvoid(); break;		/* OB_EED		 	*/
        case 23: ob_func20(); sds_retvoid(); break;		/* OB_SYMBOLTABLE 	*/
        case 24: ob_func21(); sds_retvoid(); break;		/* OB_ABORTNOW 	    */
        case 25: ob_func22(); sds_retvoid(); break;		/* OB_EXITNOW 		*/
        case 26: ob_func23(); sds_retvoid(); break;		/* OB_COMMANDLIST 	*/
/* The 30's are functions that don't test anything, they're just useful.    */
        case 30: ob_func30(); sds_retvoid(); break;		/* OB_INDEX 		*/
		case 31: ob_func31(); sds_retvoid(); break;		/* OB_DRAWING		*/
		case 32: ob_func32(); sds_retvoid(); break;		/* OB_TEXTWIN		*/
/* The 40-50's are functions for testing functions that deal with LISP.     */
        case 40: ob_func40();                break;		/* OB_LISPRETINT	*/
		case 41: ob_func41();                break;		/* OB_LISPRETREAL	*/
		case 42: ob_func42();                break;		/* OB_LISPRETSTR	*/
		case 43: ob_func43();                break;     /* OB_LISPRETT      */
		case 44: ob_func44();                break;     /* OB_LISPGETARGS   */
		case 45: ob_func45();                break;     /* OB_LISPRETNIL    */
		case 46: ob_func46();                break;     /* OB_LISPRETPOINT  */
		case 47: ob_func47();                break;     /* OB_LISPRETVOID   */
		case 48: ob_func48();                break;     /* OB_LISPRETVAL    */
		case 49: ob_func49();                break;     /* OB_LISPRETNAME   */
		case 50: ob_func50();                break;     /* OB_LISPRETLIST   */
    }
}

/**************************************************************************
Initalization function.  This loads the .dcl file.

RETURNS:(void)
***************************************************************************/
void ob_init(void) {

    char fs1[512];

    if (!ob_dclloaded) {  /* Don't load more than once */
        if (dlg_load_dialog(ob_DCLFILE,&ob_dclid)!=RTNORM) {
            sprintf(fs1,"\nERROR: Can't find/load %s.",ob_DCLFILE);
            sds_abort(fs1);
        }
        ob_dclloaded=1;
    }


    return;
}


/***************************************************************************
Cleans up any alloced memory before leaving

RETURNS:(void)
***************************************************************************/
void ob_cleanup(void) {

    if (ob_dclloaded) { dlg_unload_dialog(ob_dclid); ob_dclloaded=0; }

    return;
}

#if defined(DEBUG)

void ob_dt(void) {
    char fs1[512];
    sds_hdlg      hdlg;
    int           dlgstatus;

    sds_unload_dialog(ob_dclid);
    sds_load_dialog(ob_DCLFILE,&ob_dclid);
    if (RTCAN==sds_getstring(1,"\nEnter dialog name:", fs1)) {
        return;
    }
    if ((sds_new_dialog(fs1,ob_dclid,(CLIENTFUNC)0,&hdlg))!=RTNORM) return;
    sds_start_dialog(hdlg,&dlgstatus);
}

void ob_test(void) {

    return;
}

#endif

void ob_about(void) {
    sds_hdlg hdlg;
    int dlg_status;

    if(dlg_new_dialog("ob_about",ob_dclid,(CLIENTFUNC)0,&hdlg)!=RTNORM) {
        sds_printf("\nERROR: Reading %s dialog.","ob_about");
        return;
    }
    dlg_start_dialog(hdlg, &dlg_status);
    return;
}


/***************************************************************************
	SDS_*** Function tests start here.
	Each of these is prototyped in ourbase.h.
***************************************************************************/

/***************************************************************************
	This function tests each of the SDS_GET*** functions that request
	input directly from the user:

		SDS_GETINT		SDS_GETREAL		SDS_GETPOINT
		SDS_GETSTRING	SDS_GETCORNER	SDS_GETDIST
		SDS_GETANGLE	SDS_GETORIENT	SDS_KEWORD
		SDS_GETINPUT

	SDS_INITGET is separate 'cause it has too many options by itself.

    Each of these is documented in the Word file sds_list.doc.
	Type OB_GETUSER at the command line to run this.

***************************************************************************/
int ob_func1(void) {

	int int1,returni,controlc;
	sds_real real1=0.0;
    sds_point point1,point2;
    char string1[512];


    sds_printf("\n\nTesting sds_getint...");
    returni=sds_getint("\nPlease type an integer: ",&int1);
    sds_printf("\nThe integer you entered is %i.   sds_getint returned=%i.",int1,returni);

    sds_printf("\nTesting sds_getreal...");
    returni=sds_getreal("\nPlease type any real number: ",&real1);
    sds_printf("\nThe real number you entered is %f.   sds_getreal returned %i",real1,returni);

    sds_printf("\nTesting sds_getpoint...");
    returni=sds_getpoint(NULL,"\nPlease pick or specify a point: ",point1);
    sds_printf("\nThe point is (%f,%f,%f).   sds_getpoint returned %i",point1[0],point1[1],point1[2],returni);

    sds_printf("\nTesting sds_getstring...");
    returni=sds_getstring(1,"\nType a brief line of text: ",string1);
    sds_printf("\nYou entered: %s   sds_getstring returned %i",string1,returni);

    sds_printf("\nTesting sds_getcorner...");
    sds_getpoint(NULL,"\nPlease pick or specify the first corner for a rectangle: ",point1);
	returni=sds_getcorner(point1,"\nNow pick or specify the second corner: ",point2);
    sds_printf("\nThe points are (%f,%f,%f) and (%f,%f,%f).   sds_getcorner returned %i",point1[0],point1[1],point1[2],point2[0],point2[1],point2[2],returni);

    sds_printf("\nTesting sds_getdist...");
    sds_getpoint(NULL,"\nPlease pick or specify the first point for a distance: ",point1);
    returni=sds_getdist(point1,"\nNow pick the second point for a distance: ",&real1);
    sds_printf("\nThe distance between the points you entered is %f.   sds_getdist returned %i",real1,returni);

    sds_printf("\nTesting sds_getangle (starts wherever ANGBASE says to start)...");
    sds_getpoint(NULL,"\nPlease pick or specify the first point for an angle: ",point1);
    returni=sds_getangle(point1,"\nNow pick or specify the second point for an angle: ",&real1);
    sds_printf("\nThe resulting angle is %f radians.   sds_getangle returned %i",real1,returni);

    sds_printf("\nTesting sds_getorient (starts from the right)...");
    sds_getpoint(NULL,"\nPlease pick or specify the first point for an angle: ",point1);
    returni=sds_getorient(point1,"\nNow pick or specify the second point for an angle: ",&real1);
    sds_printf("\nThe resulting angle is %f radians.   sds_getorient returned %i",real1,returni);

    sds_printf("\nTesting sds_getkword...");
    sds_initget(0,"Cat Dog Horse coW");
    returni=sds_getkword("\nPlease enter one of the following: Cat/Dog/Horse/coW ",string1);
    sds_printf("\nThe keyword you entered is %s.   sds_getkword returned %i",string1,returni);

    sds_printf("\nTesting sds_getinput...");
    sds_initget(0,"Cat Dog Horse Bird");
    sds_getint("\nPlease enter one of the following: Cat/Dog/Horse/Bird ",&int1);
    returni=sds_getinput(string1);
    sds_printf("\nYou entered %s   sds_getinput returned %i",string1,returni);

    sds_printf("\n\nTesting sds_usrbrk...");
    sds_printf("\nWhen ready, press Enter to start this test,  ");
    sds_getstring(1,"\nthen press escape to stop the text from printing",string1);
	controlc=int1=0;
	while ((!controlc)&&(int1<=100)){
		if (int1<100)
			sds_printf("\nStill thinking...      Hmmm Hmmmmmm....     ");
		if (int1%6==5)
			sds_printf("\nHit escape already!!!");
		if (int1==100)
			sds_printf("\nDone.  You missed your chance. ");
   		controlc=sds_usrbrk();
		int1++;
	}


//	  Here is a blank format for coming up with new tests:
//    sds_printf("\nThis tests sds_getxxx.");
//    sds_getxxx(NULL,"\nPlease enter...: ",abc1);
//    sds_printf("\nThe ... you entered is %f.",abc1);

    sds_printf("\nDone with the OB_GETUSER test.");
    return(0);
}

/***************************************************************************
	This function tests each of the SDS_*** Geometry functions:

		SDS_ANGLE		SDS_DISTANCE	SDS_INTERS
		SDS_POLAR		SDS_TEXTBOX

    Each of these is documented in the Word file sds_list.doc.
	Type OB_GEOMETRY at the command line to run this.

***************************************************************************/
int ob_func2(void) {

	int returni;
	sds_real angle,dist,returnr;
    sds_point firstpt,secondpt,thirdpt,fourthpt,interspt,returnp;


    sds_printf("\nTesting sds_angle...");
    sds_getpoint(NULL,"\nFirst point for angle: ",firstpt);
    sds_getpoint(firstpt,"\nSecond point: ",secondpt);
	returnr=sds_angle(firstpt,secondpt);
    sds_printf("\nThe angle is %f radians",returnr);

    sds_printf("\nTesting sds_distance...");
    sds_getpoint(NULL,"\nFirst point for distance: ",firstpt);
    sds_getpoint(firstpt,"\nSecond point: ",secondpt);
	returnr=sds_distance(firstpt,secondpt);
    sds_printf("\nThe distance is %f",returnr);

    sds_printf("\nTesting sds_inters...");
    sds_getpoint(NULL,"\nFirst point for line 1: ",firstpt);
    sds_getpoint(firstpt,"\nSecond point: ",secondpt);
    sds_getpoint(NULL,"\nFirst point for line 2: ",thirdpt);
    sds_getpoint(thirdpt,"\nSecond point: ",fourthpt);
	returni=sds_inters(firstpt,secondpt,thirdpt,fourthpt,1,interspt);
		if (returni != RTNORM)
             sds_printf("\nThese two line segments do not intersect");
		if (returni == RTNORM)
             sds_printf("\nThe intersection point is (%f,%f,%f)",interspt[0],interspt[1],interspt[2]);
//    sds_printf("\nThe intersection point is (%f,%f,%f)",interspt[0],interspt[1],interspt[2]);

    sds_printf("\nTesting sds_polar...");
    sds_getpoint(NULL,"\nPick the center pt for the polar function: ",firstpt);
    sds_getangle(NULL,"\nEnter an angle: ",&angle);
    sds_getdist(NULL,"\nEnter a distance: ",&dist);
	sds_polar(firstpt,angle,dist,returnp);
    sds_printf("\nThe resulting point is (%f,%f,%f)",returnp[0],returnp[1],returnp[2]);


/*    sds_printf("\nTesting sds_textbox...");
	ret=sds_textbox();
    sds_printf("\nsds_textbox returned %x",xxx);
*/

    sds_printf("\nDone with the OB_GEOMETRY test.");
    return(0);
}

/***************************************************************************
	This function tests each of the SDS_IS*** and SDS_TO*** functions:

		SDS_ISALNUM		SDS_ISALPHA		SDS_ISCNTRL
		SDS_ISDIGIT		SDS_ISGRAPH		SDS_ISLOWER
		SDS_ISPRINT		SDS_ISPUNCT		SDS_ISSPACE
		SDS_ISUPPER		SDS_ISXDIGIT

		SDS_TOLOWER		SDS_TOUPPER

    Each of these is documented in the Word file sds_list.doc.
	Type OB_CHARACTER at the command line to run this.

***************************************************************************/
int ob_func3(void) {

	int returni;
	char string1[512];

    sds_printf("\nTesting sds_isalnum...");
	sds_getstring(1,"\nEnter an alphanumeric character (or not): ",string1);
    returni=sds_isalnum(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT alphanumeric.  sds_isalnum returned %i",returni);
		else
			sds_printf("\nThe character IS alphanumeric.  sds_isalnum returned %i",returni);

    sds_printf("\nTesting sds_isalpha...");
	sds_getstring(1,"\nEnter a alphabetical character (or not): ",string1);
    returni=sds_isalpha(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT alphabetical.  sds_isalpha returned %i",returni);
		else
			sds_printf("\nThe character IS alphabetical.  sds_isalpha returned %i",returni);

    sds_printf("\nTesting sds_iscntrl...");
	sds_getstring(1,"\nEnter a control character (or not): ",string1);
    returni=sds_iscntrl(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT a control character.  sds_iscntrl returned %i",returni);
		else
			sds_printf("\nThe character IS a control character.  sds_iscntrl returned %i",returni);

    sds_printf("\nTesting sds_isdigit...");
	sds_getstring(1,"\nEnter a digit (0-9) (or not): ",string1);
    returni=sds_isdigit(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT a digit (0-9).  sds_isdigit returned %i",returni);
		else
			sds_printf("\nThe character IS a digit (0-9).  sds_isdigit returned %i",returni);

    sds_printf("\nTesting sds_isgraph...");
	sds_getstring(1,"\nEnter a graphical character (or not) - try © (ALT-0169): ",string1);
    returni=sds_isgraph(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT graphical.  sds_isgraph returned %i",returni);
		else
			sds_printf("\nThe character IS graphical.  sds_isgraph returned %i",returni);

    sds_printf("\nTesting sds_islower...");
	sds_getstring(1,"\nEnter a lowercase character (or not): ",string1);
    returni=sds_islower(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT lowercase.  sds_islower returned %i",returni);
		else
			sds_printf("\nThe character IS lowercase.  sds_islower returned %i",returni);

    sds_printf("\nTesting sds_isprint...");
	sds_getstring(1,"\nEnter a printable character (or not): ",string1);
    returni=sds_isprint(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT printable.  sds_isprint returned %i",returni);
		else
			sds_printf("\nThe character IS printable.  sds_isprint returned %i",returni);

    sds_printf("\nTesting sds_ispunct...");
	sds_getstring(1,"\nEnter a punctuation character (or not): ",string1);
    returni=sds_ispunct(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT a punctuation mark.  sds_ispunct returned %i",returni);
		else
			sds_printf("\nThe character IS a punctuation mark.  sds_ispunct returned %i",returni);

    sds_printf("\nTesting sds_isspace...");
	sds_getstring(1,"\nHit space, tab, or enter (or type something that is NOT blank space): ",string1);
    returni=sds_isspace(string1[0]);
		if (returni == 0)
			sds_printf("\nYou typed something that is NOT blank space.  sds_isspace returned %i",returni);
		else
			sds_printf("\nYou entered some blank space.  sds_isspace returned %i",returni);

    sds_printf("\nTesting sds_isupper...");
	sds_getstring(1,"\nEnter an uppercase character (or not): ",string1);
    returni=sds_isupper(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT upper case.  sds_isupper returned %i",returni);
		else
			sds_printf("\nThe character IS upper case.  sds_isupper returned %i",returni);

    sds_printf("\nTesting sds_isxdigit...");
	sds_getstring(1,"\nEnter a hex digit (0-9 or A-F) (or not): ",string1);
    returni=sds_isxdigit(string1[0]);
		if (returni == 0)
			sds_printf("\nThe character is NOT a hex digit.  sds_isxdigit returned %i",returni);
		else
			sds_printf("\nThe character IS a hex digit.  sds_isxdigit returned %i",returni);

    sds_printf("\nTesting sds_tolower...");
	sds_getstring(1,"\nEnter an upper case character (or not).  Typing X should return 120 (x): ",string1);
    returni=sds_tolower(string1[0]);
	sds_printf("\nsds_tolower returned %i",returni);

    sds_printf("\nTesting sds_toupper...");
	sds_getstring(1,"\nEnter a lower case character (or not)  Typing x should return 88 (X): ",string1);
    returni=sds_toupper(string1[0]);
	sds_printf("\nsds_toupper returned %i",returni);

    sds_printf("\nDone with the OB_CHARACTER test.");
    return(0);
}


/***************************************************************************
	This function tests each of the SDS_*** conversion functions:

		SDS_ANGTOF		SDS_ANGTOS		SDS_CVUNIT
		SDS_DISTOF		SDS_RTOS

    Each of these is documented in the Word file sds_list.doc.
	Type OB_CONVERT at the command line to run this.

***************************************************************************/
int ob_func4(void) {

	int returni;
	sds_real real1=0.0;
	sds_real real2=0.0;
	char string1[512];

	char *angtof = "\nThat's equal to %f radians.  sds_angtof returned %i";
	char *angtos = "\nsds_angtos returned %i";
	char *cvunit = "\nsds_cvunit returned %i";
	char *distof = "\nsds_distof returned %i";

	int format;
	char mode[512];

    sds_printf("\nTesting sds_angtof...");
	sds_getstring(0,"\nEnter an angle in decimal degrees (eg. 47.5): ",string1);
    returni=sds_angtof(string1,0,&real1);
    sds_printf(angtof,real1,returni);
	sds_getstring(0,"\nEnter an angle in degrees/minutes/seconds (eg. 45d3'35\"): ",string1);
    returni=sds_angtof(string1,1,&real1);
    sds_printf(angtof,real1,returni);
	sds_getstring(0,"\nEnter an angle in grads (?): ",string1);
    returni=sds_angtof(string1,2,&real1);
    sds_printf(angtof,real1,returni);
	sds_getstring(0,"\nEnter an angle in radians (eg. 1.72): ",string1);
    returni=sds_angtof(string1,3,&real1);
    sds_printf(angtof,real1,returni);
	sds_getstring(0,"\nEnter an angle in surveyor's units (eg. S, or N30d23'57\"E): ",string1);
    returni=sds_angtof(string1,4,&real1);
    sds_printf(angtof,real1,returni);
/*
	Come back here later and do an sds_getvar to find the current value of AUNITS
	and report it back in the "Enter an angle in..." string.
*/
	sds_getstring(0,"\nEnter an angle in current angle units (AUNITS): ",string1);
    returni=sds_angtof(string1,-1,&real1);
    sds_printf(angtof,real1,returni);

    sds_printf("\nTesting sds_angtos (precision=AUPREC)...");
	sds_getreal("\nEnter radians to be converted to degrees: ",&real1);
    returni=sds_angtos(real1,0,-1,string1);
    sds_printf("\n%f radians equals %s degrees. ",real1,string1);
    sds_printf(angtos,returni);
	sds_getreal("\nEnter radians to be converted to degrees, minutes, and seconds: ",&real1);
    returni=sds_angtos(real1,1,-1,string1);
    sds_printf("\n%f radians equals %s degrees, minutes, and seconds. ",real1,string1);
    sds_printf(angtos,returni);
	sds_getreal("\nEnter radians to be converted to grads: ",&real1);
    returni=sds_angtos(real1,2,-1,string1);
    sds_printf("\n%f radians equals %s grads. ",real1,string1);
    sds_printf(angtos,returni);
	sds_getreal("\nEnter radians to be converted to radians: ",&real1);
    returni=sds_angtos(real1,3,-1,string1);
    sds_printf("\n%f radians equals %s radians. ",real1,string1);
    sds_printf(angtos,returni);
	sds_getreal("\nEnter radians to be converted to surveyor's units: ",&real1);
    returni=sds_angtos(real1,4,-1,string1);
    sds_printf("\n%f radians equals %s surveyor's units. ",real1,string1);
    sds_printf(angtos,returni);
/*
	Come back here later and do an sds_getvar to find the current value of AUNITS
	and report it back in the "radians equals..." string.
*/
	sds_getreal("\nEnter radians to be converted to current AUNITS: ",&real1);
    returni=sds_angtos(real1,-1,-1,string1);
    sds_printf("\n%f radians equals %s AUNITS. ",real1,string1);
    sds_printf(angtos,returni);


	sds_printf("\nWe'll skip sds_cvunit because ICAD.UNT doesn't seem to work.");
/*	Come back to this one when the ICAD.UNT file is working.
	sds_printf("\nTesting sds_cvunit...");
	sds_getreal("\nEnter XXX to convert to YYY: ",&real1);
    returni=sds_cvunit(real1,"XXX","YYY",&real2);
	sds_printf("\n%f XXX equals %f YYY. ",real1,real2);
    sds_printf(cvunit,returni);
	sds_getreal("\nEnter XXX to convert to YYY: ",&real1);
    returni=sds_cvunit(real1,"XXX","YYY",&real2);
	sds_printf("\n%f XXX equals %f YYY. ",real1,real2);
    sds_printf(cvunit,returni);
	sds_getreal("\nEnter XXX to convert to YYY: ",&real1);
    returni=sds_cvunit(real1,"XXX","YYY",&real2);
	sds_printf("\n%f XXX equals %f YYY. ",real1,real2);
    sds_printf(cvunit,returni);
*/

    sds_printf("\nTesting sds_distof...");
	format=1;
	while (format<=5){
		switch (format){
			case 1: strcpy(mode,"\nEnter a number using scientific notation (eg. 35e10):  ");    break;
			case 2: strcpy(mode,"\nEnter a number using decimal notation (5.7945):  ");          break;
			case 3: strcpy(mode,"\nEnter a number using engineering notation (3'8.125\"):  ");   break;
			case 4: strcpy(mode,"\nEnter a number using architectural notation (3'7-1/4\"):  "); break;
			case 5: strcpy(mode,"\nEnter a number using fractional notation (1/2):  ");          break;
		}
		sds_getstring(1,mode,string1);
		returni=sds_distof(string1,format,&real1);
		sds_printf("\n%s is equal to %f. ",string1,real1);
		sds_printf(distof,returni);
   		format++;
	}

    sds_printf("\nTesting sds_rtos with precision = LUPREC)...");
	format=1;
	while (format<=5){
		switch (format){
			case 1: strcpy(mode,"\nThat's %s, in scientific notation.");    break;
			case 2: strcpy(mode,"\nThat's %s, in decimal notation.");       break;
			case 3: strcpy(mode,"\nThat's %s, in engineering notation.");   break;
			case 4: strcpy(mode,"\nThat's %s, in architectural notation."); break;
			case 5: strcpy(mode,"\nThat's %s, in fractional notation.");    break;
		}
		sds_getreal("\nEnter any real number: ",&real1);
		returni=sds_rtos(real1,format,-1,string1);
		sds_printf(mode,string1);
		sds_printf("\nsds_rtos returned %i",returni);
   		format++;
	}

	sds_printf("\nSame thing, but precision to 1 decimal place...");
	format=1;
	while (format<=5){
		switch (format){
			case 1: strcpy(mode,"\nThat's %s, in scientific notation. ");    break;
			case 2: strcpy(mode,"\nThat's %s, in decimal notation. ");       break;
			case 3: strcpy(mode,"\nThat's %s, in engineering notation. ");   break;
			case 4: strcpy(mode,"\nThat's %s, in architectural notation. "); break;
			case 5: strcpy(mode,"\nThat's %s, in fractional notation. ");    break;
		}
		sds_getreal("\nEnter any real number: ",&real1);
		returni=sds_rtos(real1,format,1,string1);
		sds_printf(mode,string1);
		sds_printf("\nsds_rtos returned %i",returni);
   		format++;
	}

    sds_printf("\nDone with the OB_CONVERT test.");
    return(0);
}

/***************************************************************************
	This function tests each of the SDS_*** functions that deal with
	displaying information:

		SDS_GRAPHSCR	SDS_MENUCMD		SDS_PRINTF
		SDS_PROMPT		SDS_REDRAW		SDS_TEXTPAGE
		SDS_TEXTSCR

    Each of these is documented in the Word file sds_list.doc.
	Type OB_DISPLAY at the command line to run this.

***************************************************************************/
int ob_func5(void) {

	int returni;
	char string1[512];
	sds_point point1,point2;
	sds_name entity1;

//    sds_printf("\nTesting sds_menucmd...");
//    returni=sds_menucmd();
//    sds_printf("\nsds_menucmd returned %x",returni);

    sds_printf("\nTesting sds_printf...");
	sds_getstring(1,"\nEnter a brief line of text. ",string1);
    returni=sds_printf("\nYou entered:  %s",string1);
    sds_printf("\nsds_printf returned %i",returni);

//    sds_printf("\nTesting sds_prompt...");
//    returni=sds_prompt();
//    sds_printf("\nsds_prompt returned %i",returni);

    sds_printf("\nTesting sds_redraw...");
	sds_getpoint(NULL,"Pick a first point for a line: ",point1);
	sds_getpoint(point1,"\nSecond point: ",point2);
    sds_command(RTSTR,"line",RT3DPOINT,point1,RT3DPOINT,point2,RTSTR,"",RTNONE);
	sds_getstring(0,"\nTo clear the graphics window, press Enter. ",string1);
    sds_grclear();
	sds_getstring(0,"\nNow press Enter to redraw the graphics window. ",string1);
	returni=sds_redraw(NULL,0);
    sds_printf("\nsds_redraw returned %i",returni);
	returni=sds_entsel("\nPick an entity: ",entity1,point1);
	sds_redraw(NULL,4);
    returni=sds_redraw(entity1,3);
    sds_printf("\nsds_redraw returned %i",returni);
	returni=sds_entsel("\nPick another entity: ",entity1,point1);
	sds_redraw(NULL,4);
    returni=sds_redraw(entity1,2);
    sds_printf("\nsds_redraw returned %i",returni);
	sds_getstring(0,"\nPress enter to bring the entity back. ",string1);
	returni=sds_redraw(entity1,1);
	sds_printf("\nsds_redraw returned %i",returni);

    sds_printf("\nTesting sds_textscr...");
	sds_getstring(0,"\nTo switch to the text window, press Enter. ",string1);
    returni=sds_textscr();
    sds_printf("\nsds_textscr returned %i",returni);

    sds_printf("\nTesting sds_graphscr...");
	sds_getstring(0,"\nTo switch back to the graphics window, press Enter again. ",string1);
    returni=sds_graphscr();
    sds_printf("\nsds_graphscr returned %i",returni);

    sds_printf("\nTesting sds_textpage...");
	sds_getstring(0,"\nTo switch to the cleared text window, press Enter. ",string1);
    returni=sds_textpage();
    sds_printf("\nsds_textpage returned %i",returni);

    sds_printf("\nTesting sds_graphscr...");
	sds_getstring(0,"\nTo switch back to the graphics window, press Enter again. ",string1);
    returni=sds_graphscr();
    sds_printf("\nsds_graphscr returned %i",returni);

    sds_printf("\nDone with the OB_DISPLAY test.");
    return(0);
}

/***************************************************************************
	This function tests each of the SDS_*** graphics functions:

		SDS_GRCLEAR		SDS_GRDRAW		SDS_GRREAD
		SDS_GRTEXT		SDS_GRVECS

    Each of these is documented in the Word file sds_list.doc.
	Type OB_GRAPHICS at the command line to run this.

***************************************************************************/

int ob_func6(void) {

	int returni;
	struct resbuf *vectorbuf;
	sds_point vpt1 =  {0.0,0.0,0.0};
	sds_point vpt2 =  {5.0,2.0,0.0};
	sds_point vpt3 =  {1.0,4.0,0.0};
	sds_point vpt4 =  {4.0,6.0,0.0};
	sds_point vpt5 =  {8.0,3.0,0.0};
	sds_point vpt6 = {10.0,5.0,0.0};
	short white = 7;
	short green = 3;
	short redhlt = -1;
	sds_matrix vecident = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};


	int int1,int2;
//	sds_real real1=0.0;
	sds_point firstpt,secondpt;
	char string1[512];
	struct sds_resbuf readinput;

    sds_printf("\nTesting sds_grclear...");
	sds_getstring(0,"\nTo clear the graphics window, press Enter. ",string1);
    returni=sds_grclear();
    sds_printf("\nsds_grclear returned %i",returni);
	sds_getstring(0,"\nNow press Enter to redraw the graphics window. ",string1);
	sds_redraw(NULL,0);

    sds_printf("\nTesting sds_grdraw...");
	sds_getpoint(NULL,"\nFirst point for vector. ",firstpt);
	sds_getpoint(NULL,"\nSecond point for vector. ",secondpt);
	sds_getint("\nEnter a color number (0-255): ",&int1);
	sds_getint("\nEnter 1 for highlighted, 0 for normal display. ",&int2);
    returni=sds_grdraw(firstpt,secondpt,int1,int2);
    sds_printf("\nsds_grdraw returned %i",returni);

    sds_printf("\nTesting sds_grread...");
	int1=2; // int1 is the bitset argument controlling what gets returned.
    // int2 is the device type.  2 = Keyboard, 3 = Point, 4 = Menu item, etc..
	returni=sds_grread(int1,&int2,&readinput);
	sds_printf("\nThe type of input it %i. ",int2);
	switch(int2){
		case  2:		sds_printf("\nYou used the keyboard.  You will see the ACSI value for the character you typed.");	break;
		case  3:		sds_printf("\nYou selected a point.");	break;
		case  4:		sds_printf("\nYou selected something from a screen or pull-down menu.");	break;
		case  5:		sds_printf("\nYou moved the mouse.");	break;
		case  6:		sds_printf("\nYou selected something from a button menu.");	break;
		case  7:		sds_printf("\nYou selected a menu item from tablet1.");	break;
		case  8:		sds_printf("\nYou selected a menu item from tablet2.");	break;
		case  9:		sds_printf("\nYou selected a menu item from tablet3.");	break;
		case 10:		sds_printf("\nYou selected a menu item from tablet4.");	break;
		case 11:		sds_printf("\nYou selected a menu item from the aux menu.");	break;
		case 12:		sds_printf("\nYou drug the mouse around.");	break;
		default:		sds_printf("\nYou did something unusual.");	break;
	}
 	switch(readinput.restype){
		case RTREAL:    sds_printf("\nThe input is a real. Its value is %f. ",readinput.resval.rreal);
						break;
		case RTPOINT:   sds_printf("\nThe input is a 2D point. Its x and y values are %f,%f. ",readinput.resval.rpoint[0],readinput.resval.rpoint[1]);
						break;
		case RTSHORT:   sds_printf("\nThe input is a short. Its value is %i. ",readinput.resval.rint);
						break;
		case RTSTR:     sds_printf("\nThe input is a string.  The string is \"%s\".",readinput.resval.rstring);
//							if (valueout->resval.rstring != NULL)
//								free (readinput.resval.rstring);
						break;
		case RT3DPOINT:	sds_printf("\nThe input is a 3D point.  Its x, y, and z values are %f,%f,%f. ",readinput.resval.rpoint[0],readinput.resval.rpoint[1],readinput.resval.rpoint[2]);
						break;
		case RTLONG:	sds_printf("\nThe input is a long.  It's value is %ld. ",readinput.resval.rlong);
						break;
		default:		sds_printf("\nTThe input is not one of the usual types.");
	}
   sds_printf("\nsds_grread returned %i",returni);


    sds_printf("\nTesting sds_grtext...");
    returni=sds_grtext(-1,"Hey!  -1 is over here!",1);
    sds_printf("\nsds_grtext returned %i",returni);
    returni=sds_grtext(-2,"And -2 is right here",1);
    sds_printf("\nsds_grtext returned %i",returni);

/*  This will need to be tested with a transformation matrix, too.
    This is just the simplest test.  (And on top of that, it doesn't work (or grvecs doesn't work.)
*/
    sds_printf("\nTesting sds_grcvecs...");
	vectorbuf = sds_buildlist(RTSHORT,white, RT3DPOINT,vpt1, RT3DPOINT,vpt2, RT3DPOINT,vpt4, RT3DPOINT,vpt3,
		                      RTSHORT,redhlt,RT3DPOINT,vpt5, RT3DPOINT,vpt6, RT3DPOINT,vpt6, RT3DPOINT,vpt4,
							  RTSHORT,green, RT3DPOINT,vpt3, RT3DPOINT,vpt5, RT3DPOINT,vpt3, RT3DPOINT,vpt2, 0);
    returni=sds_grvecs(vectorbuf,vecident);
	sds_relrb(vectorbuf);
    sds_printf("\nsds_grvecs returned %i",returni);

    sds_printf("\nDone with the OB_GRAPHICS test. ");
    return(0);
}

/***************************************************************************
	This function tests the SDS_INITGET function.

    This is documented in the Word file sds_list.doc.
	Type OB_INITGET at the command line to run this.

***************************************************************************/
int ob_func7(void) {

	int int1,returni;
	sds_real real1=0.0;
    sds_point point1;
    char string1[512];

	sds_printf("\nThe first set uses the bitset numbers.  The second set uses the names. ");
    sds_printf("\nTesting sds_initget using keywords...");
    returni=sds_initget(0,"Cat Dog Horse Bird");
    sds_getint("\nPlease enter one of the following: Cat/Dog/Horse/Bird ",&int1);
    sds_getinput(string1);
    sds_printf("\nYou entered %s   sds_initget returned %i",string1,returni);

    sds_printf("\nTesting sds_initget using nonull (1)...");
    returni=sds_initget(RSG_NONULL,"");
    sds_getint("\nPress Enter-it shouldn't let you (then any integer): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);
    returni=sds_initget(0,"");
    sds_getint("\nPress Enter (now it should let you): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);

    sds_printf("\nTesting sds_initget using nozero (2)...");
    returni=sds_initget(RSG_NOZERO,"");
    sds_getint("\nEnter zero-it shouldn't let you (then any integer): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);
    returni=sds_initget(0,"");
    sds_getint("\nEnter zero (now it should let you): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);

    sds_printf("\nTesting sds_initget using noneg (4)...");
    returni=sds_initget(RSG_NONEG,"");
    sds_getint("\nEnter a negative integer-it shouldn't let you (then a positive integer): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);
    returni=sds_initget(0,"");
    sds_getint("\nEnter a negative integer (now it should let you): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);

    sds_printf("\nTesting sds_initget using nolim (8)...");
    returni=sds_initget(0,"");
    sds_getpoint(NULL,"\nEnter a point outside the drawing limits (it shouldn't let you) ",point1);
    sds_printf("\nThe point is (%f,%f,%f).   sds_initget returned %i",point1[0],point1[1],point1[2],returni);
    returni=sds_initget(RSG_NOLIM,"");
    sds_getpoint(NULL,"\nEnter a point outside the drawing limits (now it should let you) ",point1);
    sds_printf("\nThe point is (%f,%f,%f).   sds_initget returned %i",point1[0],point1[1],point1[2],returni);

    sds_printf("\nTesting sds_initget using dash (32)...");
    sds_getpoint(NULL,"\nPick the first point for an angle: ",point1);
    returni=sds_initget(0,"");
    sds_getangle(point1,"\nSecond point (rubber-band line should be solid): ",&real1);
    sds_printf("\nsds_initget returned %i",returni);
    sds_getpoint(NULL,"\nPick the first point for an angle: ",point1);
    returni=sds_initget(RSG_DASH,"");
    sds_getangle(point1,"\nSecond point (rubber-band line should be dashed): ",&real1);
    sds_printf("\nsds_initget returned %i",returni);

    sds_printf("\nTesting sds_initget using 2D (64)...");
    sds_printf("\nFirst, it will give you a 3D distance...");
    returni=sds_initget(0,"");
    sds_getpoint(NULL,"\nEnter a 3D first point: ",point1);
    sds_printf("\nsds_initget returned %i",returni);
    returni=sds_initget(0,"");
    sds_getdist(point1,"\nEnter a 3D second point: ",&real1);
    sds_printf("\nThe 3D distance between the points you entered is %f.   sds_initget returned %i",real1,returni);
    sds_printf("\nNow, it will give you a 2D distance...");
    returni=sds_initget(RSG_2D,"");
    sds_getpoint(NULL,"\nEnter a 3D first point: ",point1);
    sds_printf("\nsds_initget returned %i",returni);
    returni=sds_initget(RSG_2D,"");
    sds_getdist(point1,"\nEnter a 3D second point: ",&real1);
    sds_printf("\nThe 2D distance between the points you entered is %f.   sds_initget returned %i",real1,returni);

    sds_printf("\nTesting sds_initget allowing any input...");
    returni=sds_initget(0,"Cat Dog Horse Bird");
    sds_getint("\nPlease type something other than Cat/Dog/Horse/Bird (it shouldn't let you): ",&int1);
    sds_getinput(string1);
    sds_printf("\nYou entered %s   sds_initget returned %i",string1,returni);
    returni=sds_initget(RSG_OTHER,"Cat Dog Horse Bird");
    sds_getint("\nPlease type something other than Cat/Dog/Horse/Bird (now it should let you): ",&int1);
    sds_getinput(string1);
    sds_printf("\nYou entered %s   sds_initget returned %i",string1,returni);


	sds_printf("\nThis is the same set, but with the rsg_*** codes instead of bit values: ");

    sds_printf("\nTesting sds_initget using keywords...");
    returni=sds_initget(0,"Cat Dog Horse Bird");
    sds_getint("\nPlease enter one of the following: Cat/Dog/Horse/Bird ",&int1);
    sds_getinput(string1);
    sds_printf("\nYou entered %s   sds_initget returned %i",string1,returni);

    sds_printf("\nTesting sds_initget using nonull (1)...");
    returni=sds_initget(1,"");
    sds_getint("\nPress Enter-it shouldn't let you (then any integer): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);
    returni=sds_initget(0,"");
    sds_getint("\nPress Enter (now it should let you): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);

    sds_printf("\nTesting sds_initget using nozero (2)...");
    returni=sds_initget(2,"");
    sds_getint("\nEnter zero-it shouldn't let you (then any integer): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);
    returni=sds_initget(0,"");
    sds_getint("\nEnter zero (now it should let you): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);

    sds_printf("\nTesting sds_initget using noneg (4)...");
    returni=sds_initget(4,"");
    sds_getint("\nEnter a negative integer-it shouldn't let you (then a positive integer): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);
    returni=sds_initget(0,"");
    sds_getint("\nEnter a negative integer (now it should let you): ",&int1);
    sds_printf("\nYou entered %i   sds_initget returned %i",int1,returni);

    sds_printf("\nTesting sds_initget using nolim (8)...");
    returni=sds_initget(0,"");
    sds_getpoint(NULL,"\nEnter a point outside the drawing limits (it shouldn't let you) ",point1);
    sds_printf("\nThe point is (%f,%f,%f).   sds_initget returned %i",point1[0],point1[1],point1[2],returni);
    returni=sds_initget(8,"");
    sds_getpoint(NULL,"\nEnter a point outside the drawing limits (now it should let you) ",point1);
    sds_printf("\nThe point is (%f,%f,%f).   sds_initget returned %i",point1[0],point1[1],point1[2],returni);

    sds_printf("\nTesting sds_initget using dash (32)...");
    sds_getpoint(NULL,"\nPick the first point for an angle: ",point1);
    returni=sds_initget(0,"");
    sds_getangle(point1,"\nSecond point (rubber-band line should be solid): ",&real1);
    sds_printf("\nsds_initget returned %i",returni);
    sds_getpoint(NULL,"\nPick the first point for an angle: ",point1);
    returni=sds_initget(32,"");
    sds_getangle(point1,"\nSecond point (rubber-band line should be dashed): ",&real1);
    sds_printf("\nsds_initget returned %i",returni);

    sds_printf("\nTesting sds_initget using 2D (64)...");
    sds_printf("\nFirst, it will give you a 3D distance...");
    returni=sds_initget(0,"");
    sds_getpoint(NULL,"\nEnter a 3D first point: ",point1);
    sds_printf("\nsds_initget returned %i",returni);
    returni=sds_initget(0,"");
    sds_getdist(point1,"\nEnter a 3D second point: ",&real1);
    sds_printf("\nThe 3D distance between the points you entered is %f.   sds_initget returned %i",real1,returni);
    sds_printf("\nNow, it will give you a 2D distance...");
    returni=sds_initget(64,"");
    sds_getpoint(NULL,"\nEnter a 3D first point: ",point1);
    sds_printf("\nsds_initget returned %i",returni);
    returni=sds_initget(64,"");
    sds_getdist(point1,"\nEnter a 3D second point: ",&real1);
    sds_printf("\nThe 2D distance between the points you entered is %f.   sds_initget returned %i",real1,returni);

    sds_printf("\nTesting sds_initget allowing any input...");
    returni=sds_initget(0,"Cat Dog Horse Bird");
    sds_getint("\nPlease type something other than Cat/Dog/Horse/Bird (it shouldn't let you): ",&int1);
    sds_getinput(string1);
    sds_printf("\nYou entered %s   sds_initget returned %i",string1,returni);
    returni=sds_initget(128,"Cat Dog Horse Bird");
    sds_getint("\nPlease type something other than Cat/Dog/Horse/Bird (now it should let you): ",&int1);
    sds_getinput(string1);
    sds_printf("\nYou entered %s   sds_initget returned %i",string1,returni);

    sds_printf("\nDone with the OB_INITGET test.");
    return(0);
}


/***************************************************************************
	This function tests the functions that deal with the interface to LISP:

		SDS_INIT		SDS_LINK

    Each of these is documented in the Word file sds_list.doc.
	Type OB_LISP at the command line to run this.

***************************************************************************/
int ob_func8(void) {

//	int returni;
//
//    sds_printf("\nTesting sds_init...");
//    returni=sds_init();
//    sds_printf("\nsds_init returned %i",returni);
//
//	  sds_printf("\nTesting sds_link...");
//    returni=sds_link();
//    sds_printf("\nsds_link returned %i",returni);
//
    sds_printf("\nDone with the OB_LISP test.");
	sds_printf("\nNothing happened.  The OB_LISP test is not complete.");
    return(0);
}

/***************************************************************************
	This function tests the sds_ functions for external functions:

		SDS_DEFUN		SDS_UNDEF		SDS_GETFUNCODE
		SDS_GETARGS		SDS_REGFUNC		SDS_INVOKE

    Each of these is documented in the Word file sds_list.doc.
	Type OB_EXTFUNCS at the command line to run this.

***************************************************************************/
int ob_func9(void) {

//	int returni;
//	sds_real real1=0.0;
//	sds_point point1,point2;
//	char string1[512];
//
//  sds_printf("\nTesting sds_defun...");
//  returni=sds_defun();
//  sds_printf("\nsds_defun returned %i",returni);
//
//  sds_printf("\nTesting sds_getfuncode...");
//	returni=sds_getfuncode();
//	sds_printf("\nsds_getfuncode returned %i",returni);
//
//  sds_printf("\nTesting sds_undef...");
//	returni=sds_undef();
//  sds_printf("\nsds_undef returned %i",returni);
//
//    sds_printf("\nTesting sds_getargs...");
//    testbuf=sds_getargs();
//    sds_printf("\nsds_getargs returned %X",returnX);
//
//    sds_printf("\nTesting sds_regfunc...");
//    returni=sds_regfunc();
//    sds_printf("\nsds_regfunc returned %i",returni);
//
//    sds_printf("\nTesting sds_invoke...");
//    returni=sds_invoke();
//    sds_printf("\nsds_invoke returned %i",returni);
//
    sds_printf("\nDone with the OB_EXTFUNCS test.");
    sds_printf("\nNothing happened.  The OB_EXTFUNCS test is not complete.");
    return(0);
}

/***************************************************************************
	This function tests the functions for external applications:

		SDS_XLOAD		SDS_XUNLOAD		SDS_LOADED

    Each of these is documented in the Word file sds_list.doc.
	Type OB_APPS at the command line to run this.

***************************************************************************/
int ob_func10(void) {

struct sds_resbuf *loadlist;
struct sds_resbuf *templist;
char string1[512];
int howmany = 0;

    sds_printf("\nTesting sds_loaded...");
    loadlist=sds_loaded();
	for (templist=loadlist;templist->rbnext!=NULL;templist=templist->rbnext)
	{
		if (templist->restype!=RTSTR)	// It *should* be a string, but if not...
			sds_getstring(0,"\nThis result buffer contains the wrong type.  Press Enter.",string1);
		else {
			sds_printf("\nThe application %s is loaded.",templist->resval.rstring);
			howmany++;
		}
	}
	sds_printf("\nThere are %i applications loaded at this time.",howmany);
	if (howmany == 0)
		sds_getstring(0,"\nThis should have returned at least 1 application loaded, since"
						"\nOURBASE is running now.  Press Enter to continue. ",string1);
	sds_relrb(loadlist);
	loadlist=NULL;
	templist=NULL;
    sds_printf("\nDone testing sds_loaded. ");

    sds_printf("\nDone with the OB_APPS test.");
    return(0);
}

//    sds_printf("\nTesting sds_xload...");
//    returni=sds_xload();
//    sds_printf("\nsds_xload returned %i",returni);
//
//    sds_printf("\nTesting sds_xunload...");
//    returni=sds_xunload();
//    sds_printf("\nsds_xunload returned %i",returni);



/***************************************************************************
	This function tests the error handling functions:

		SDS_FAIL	SDS_ALERT

		But not:	SDS_ABORT		SDS_EXIT
		These are tested separately in the ABORTNOW and EXITNOW
		functions because they end the program.

    Each of these is documented in the Word file sds_list.doc.
	Type OB_ERRORS at the command line to run this.

***************************************************************************/
int ob_func11(void) {

    char string1[5001];

    sds_printf("\nTesting sds_fail...");
	strncpy(string1,"\nThis is an error message.",sizeof(string1)-1);
    sds_fail(string1);


	sds_printf("\nTesting sds_alert...");
	sds_getstring(1,"\nError message to display (up to 511 characters): ",string1);

	sds_alert(string1);

    sds_printf("\nTesting sds_alert, again...");
	sds_getstring(0,"\nPress Enter to display a message box.",string1);
	strncpy(string1,"This is the message box\ndisplayed by sds_alert. ",sizeof(string1)-1);
	sds_alert(string1);


    sds_printf("\nDone with the OB_ERRORS test.");
    return(0);
}

/***************************************************************************
	This function tests the memory management functions:

		SDS_NEWRB		SDS_BUILDLIST	SDS_RELRB

    Each of these is documented in the Word file sds_list.doc.
	Type OB_MEMORY at the command line to run this.

***************************************************************************/
int ob_func12(void) {

	struct sds_resbuf *newbuffer;

    sds_printf("\nTesting sds_newrb...");
    newbuffer=sds_newrb(RTREAL);
	newbuffer->resval.rreal=121.50;
	sds_printf("\nThe Real in the new buffer is %f.",newbuffer->resval.rreal);
    sds_printf("\nDone testing sds_newrb.");

    sds_printf("\nDone with the OB_MEMORY test.");
    return(0);
}


/***************************************************************************
	This function tests query and command functions:

		SDS_COMMAND		SDS_CMD
		SDS_SETVAR		SDS_FINDFILE	SDS_GETFILED
		SDS_GETSYM		SDS_PUTSYM		SDS_OSNAP
		SDS_VPORTS

    Except for SDS_GETVAR, which is next.  (Type GETSETVAR)

    Each of these is documented in the Word file sds_list.doc.
	Type OB_QUERYCOM at the command line to run this.

***************************************************************************/
int ob_func13(void) {
	char *tstr1="This is a test string";

	int returni,count;
	sds_point point1,point2;
	char string1[512],string2[512];
	struct sds_resbuf filename;
	struct sds_resbuf valuein;
	struct sds_resbuf *valueout;


    sds_printf("\nTesting sds_command...");
	sds_getpoint(NULL,"Pick a first point for a line: ",point1);
	sds_getpoint(point1,"\nSecond point: ",point2);
    returni=sds_command(RTSTR,"line",RT3DPOINT,point1,RT3DPOINT,point2,RTSTR,"",RTNONE);
    sds_printf("\nsds_command returned %i",returni);

    sds_printf("\nTesting sds_findfile...");
	sds_getstring(1,"\nEnter the name of a file to find: ",string1);
    returni=sds_findfile(string1,string2);
	sds_printf("\nThe path for that file is %s: ",string2);
    sds_printf("\nsds_findfile returned %i",returni);

    sds_printf("\nTesting sds_getfiled...");
    returni=sds_getfiled("sds_getfiled Test Dialog",NULL,"DWG",4,&filename);
	if (returni==RTNORM)
		sds_printf("\nThe name of the file is %s. ",filename.resval.rstring);
	else
		sds_printf("\nNo file was selected.");
//	if(filename.resval.rstring!=NULL)
//		free (filename.resval.rstring);
    sds_printf("\nsds_getfiled returned %i",returni);

    sds_printf("\nTesting sds_putsym and sds_getsym...");
	count=0;
	while (count < 10)
	{
		switch (count){
			case 0:		sds_printf("\nTesting with a real number...");
						valuein.restype=RTREAL;
						valuein.resval.rreal=121.5432100;
						break;
			case 1:		sds_printf("\nTesting with a 3D point...");
						valuein.restype=RT3DPOINT;
						valuein.resval.rpoint[0]=1.25;
						valuein.resval.rpoint[1]=3.75;
						valuein.resval.rpoint[2]=0.0;
						break;
			case 2:		sds_printf("\nTesting with a bigger 3D point...");
						valuein.restype=RT3DPOINT;
						valuein.resval.rpoint[0]=999999999.99;
						valuein.resval.rpoint[1]=888888888.88;
						valuein.resval.rpoint[2]=777777777.77;
						break;
			case 3:		sds_printf("\nTesting with a string...");
						valuein.restype=RTSTR;
						valuein.resval.rstring=(char *)malloc(strlen(tstr1)+1);
						strcpy(valuein.resval.rstring,tstr1);
						break;
			case 4:		sds_printf("\nTesting with a string that's 1000 characters long...");
						//valuein.restype=RTSTR;
						//valuein.resval.rstring=(char *)malloc(strlen(tstr2)+1);
						//strcpy(valuein.resval.rstring,tstr2);
						sds_printf("\nSkipping this test until \"free\" works right.");
						sds_printf("\nIgnore the string this returns.");
						break;
			case 5:		sds_printf("\nTesting with a real number...");
						valuein.restype=RTREAL;
						valuein.resval.rreal=7.654321;
						break;
			case 6:		sds_printf("\nTesting with biggest positive integer...");
						valuein.restype=RTSHORT;
						valuein.resval.rint=+32767;
						break;
			case 7:		sds_printf("\nTesting with biggest negative integer...");
						valuein.restype=RTSHORT;
						valuein.resval.rint=-32767;
						break;
			case 8:		sds_printf("\nTesting with biggest positive long...");
						valuein.restype=RTLONG;
						valuein.resval.rlong=+2147483647;
						break;
			case 9:		sds_printf("\nTesting with biggest negative long...");
						valuein.restype=RTLONG;
						valuein.resval.rlong=-2147483647;
						break;
			default:	sds_printf("\nA problem has occurred.");
		}
		returni=sds_putsym("LISPsym",&valuein);
		sds_printf("\nsds_putsym returned %i",returni);

		returni=sds_getsym("LISPsym",&valueout);
		switch(valueout->restype){
			case RTREAL:    sds_printf("\nLISPsym is a real. Its value is %f. ",valueout->resval.rreal);
							break;
			case RTPOINT:   sds_printf("\nLISPsym is a 2D point. Its x and y values are %f,%f. ",valueout->resval.rpoint[0],valueout->resval.rpoint[1]);
							break;
			case RTSHORT:   sds_printf("\nLISPsym is a short. Its value is %i. ",valueout->resval.rint);
							break;
			case RTSTR:     sds_printf("\nLISPsym is a string.  The string is \"%s\".",valueout->resval.rstring);
//							if (valueout->resval.rstring != NULL)
//								free (valueout->resval.rstring);
							break;
			case RT3DPOINT:	sds_printf("\nLISPsym is a 3D point.  Its x, y, and z values are %f,%f,%f. ",valueout->resval.rpoint[0],valueout->resval.rpoint[1],valueout->resval.rpoint[2]);
							break;
			case RTLONG:	sds_printf("\nLISPsym is a long.  It's value is %ld. ",valueout->resval.rlong);
							break;
			default:		sds_printf("\nThe value is not one of the usual types.");
		}
		sds_printf("\nsds_getsym returned %i",returni);
		count++;
	}  // End of While loop

    sds_printf("\nTesting sds_osnap...");
// Ideally, this needs a way to set the aperture.  Later.
	sds_getstring(0,"\nEnter a snap mode: ",string1);
	sds_getpoint(NULL,"\nPick a line to snap to:  ",point1);
	returni=sds_osnap(point1,string1,point2);
	sds_getpoint(point2,"\nPick a second point for a line: ",point1);
	sds_command(RTSTR,"line",RTPOINT,point2,RTPOINT,point1,RTSTR,"",RTNONE);
    sds_printf("\nsds_osnap returned %i",returni);
//
//    sds_printf("\nTesting sds_vports...");
//    returni=sds_vports();
//    sds_printf("\nsds_vports returned %i",returni);
//
    sds_printf("\nDone with the OB_QUERYCOM test.");
    return(0);
}

/***************************************************************************
    This function tests SDS_GETVAR and SDS_SETVAR

    Type OB_GETSETVAR at the command line to run this.
***************************************************************************/
int ob_func14(void) {

	int int1,returni;
	char string1[512];
	struct resbuf rb1;
//	sds_name entity1,sset1;
//	sds_point point1,point2;

    sds_printf("\nTesting sds_getvar...");
	sds_getstring(0,"\nEnter a variable name:  ",string1);
    returni=sds_getvar(string1,&rb1);
	switch(rb1.restype){
//		case RTNONE:    sds_printf("\nThe result type for %s is RTNONE:  No result. ",string1);
//						sds_printf("\nThere is no value for this return type. ");
//						break;
		case RTREAL:    sds_printf("\nThe result type for %s is RTREAL:  A Real - 8 Bytes. ",string1);
						sds_printf("\nIts value is %f. ",rb1.resval.rreal);
						break;
		case RTPOINT:   sds_printf("\nThe result type for %s is RTPOINT:  A 2D Point - 3 Reals @ 8 Bytes each.  Last is ignored ",string1);
						sds_printf("\nIts x,y value is %f,%f. ",rb1.resval.rpoint[0],rb1.resval.rpoint[1]);
						break;
		case RTSHORT:   sds_printf("\nThe result type for %s is RTSHORT:  A Short - 2 Bytes. ",string1);
						sds_printf("\nIts value is %i. ",rb1.resval.rint);
						break;
//		case RTANG:     sds_printf("\nThe result type for %s is RTANG:  An Angle in radians - a Real - 8 Bytes. ",string1);
//						sds_printf("\nIts value is %f. ",rb1.resval.rreal);
//						break;
		case RTSTR:     sds_printf("\nThe result type for %s is RTSTR:  An 4 Byte pointer to a string. ",string1);
						sds_printf("\nThe string is \"%s\".",rb1.resval.rstring);
//						THIS IS BROKEN.  IT SHOULD WORK, BUT DOESN'T.
//						if (rb1.resval.rstring != NULL)
//							free (rb1.resval.rstring);
						break;
//		case RTENAME:   sds_printf("\nThe result type for %s is RTENAME:  An Entity Name - 2 Longs @ 4 Bytes each. ",string1);
//						sds_printf("\nIts value is %ld,%ld. ",rb1.resval.rlname[0],rb1.resval.rlname[1]);
//						break;
//		case RTPICKS:   sds_printf("\nThe result type for %s is RTPICKS:  A Selection Set Name - 2 Longs @ 4 Bytes each. ",string1);
//						sds_printf("\nIts value is %ld,%ld. ",rb1.resval.rlname[0],rb1.resval.rlname[1]);
//						break;
//		case RTORINT:	sds_printf("\nThe result type for %s is RTORIENT:  An Orientation - ???. ",string1);
//						break;
		case RT3DPOINT: sds_printf("\nThe result type for %s is RT3DPOINT:  A 3D Point - 3 Reals @ 8 Bytes each. ",string1);
						sds_printf("\nIts x,y,z value is %f,%f,%f. ",rb1.resval.rpoint[0],rb1.resval.rpoint[1],rb1.resval.rpoint[2]);
						break;
//		case RTLONG:    sds_printf("\nThe result type for %s is RTLONG:  A Long - 4 Bytes. ",string1);
//						break;
//						sds_printf("\nIts value is %ld. ",rb1.resval.rlong);
//						break;
//		case RTVOID:    sds_printf("\nThe result type for %s is RTVOID:  Void - ???. ",string1);
//						break;
//		case RTLB:      sds_printf("\nThe result type for %s is RTLB:  A List Begin - ???. ",string1);
//						break;
//		case RTLE:      sds_printf("\nThe result type for %s is RTLE:  A List End - ???. ",string1);
//						break;
//		case RTDOTE:    sds_printf("\nThe result type for %s is RTDOTE:  Dot, for dotted pairs - ???. ",string1);
//						break;
//		case RTT:       sds_printf("\nThe result type for %s is RTT:  True, for AutoLISP - ???. ",string1);
//						break;
//		case RTNIL:     sds_printf("\nThe result type for %s is RTNIL:  Nil, for AutoLISP - ???. ",string1);
//						break;
//		case RTDXF0:    sds_printf("\nThe result type for %s is RTDXF0:  The Zero Group Code for DXF Lists - ???. ",string1);
//						break;
		default:		sds_printf("\nThe variable type for %s is not handled by this function. ",string1);
	}
    sds_printf("    sds_getvar returned %i",returni);

    sds_printf("\nTesting sds_setvar...");
	sds_getstring(0,"\nEnter a variable name:  ",string1);
    returni=sds_getvar(string1,&rb1);
	switch(rb1.restype){
//		case RTNONE:    sds_printf("\nThe result type for %s is RTNONE:  No result. ",string1);
//						sds_printf("\nThere is no value to change for this result type. ");
//						break;
		case RTREAL:    sds_printf("\nThe real value for %s is currently %f. ",string1,rb1.resval.rreal);
						sds_getreal("\nEnter a new real number: ",&rb1.resval.rreal);
						sds_setvar(string1,&rb1);
						sds_printf("\n%s is now set at %f.  ",string1,rb1.resval.rreal);
						break;
		case RTPOINT:   sds_printf("\nThe x and y values for the %s point are currently %f,%f. ",string1,rb1.resval.rpoint[0],rb1.resval.rpoint[1]);
						sds_getpoint(NULL,"\nPick a new point:  ",rb1.resval.rpoint);
						sds_setvar(string1,&rb1);
						sds_printf("\n%s is now set at %f,%f.  ",string1,rb1.resval.rpoint[0],rb1.resval.rpoint[1]);
						break;
		case RTSHORT:   sds_printf("\nThe short value for %s is currently %i. ",string1,rb1.resval.rint);
						sds_getint("\nEnter a new integer: ",&int1);
						rb1.resval.rint=int1;
						sds_setvar(string1,&rb1);
						sds_printf("\n%s is now set at %i.  ",string1,rb1.resval.rint);
						break;
//		case RTANG:     sds_printf("\nThe real number angle for %s is currently %ld. ",string1,rb1.resval.rreal);
//						sds_getreal("\nEnter a new angle in radians: ",&rb1.resval.rreal);
//						sds_setvar(string1,&rb1);
//						sds_printf("\nThe angle for %s is now set at %ld radians.  ",string1,rb1.resval.rreal);
//						break;
		case RTSTR:     sds_printf("\nThe string in %s is currently %s. ",string1,rb1.resval.rstring);
						sds_getstring(0,"\nEnter a new string: ",rb1.resval.rstring);
						sds_setvar(string1,&rb1);
						sds_printf("\nThe new string for %s is now set at %s.  ",string1,rb1.resval.rstring);
//						THIS IS BROKEN.  IT SHOULD WORK, BUT DOESN'T.
//						if (rb1.resval.rstring != NULL)
//							free (rb1.resval.rstring);
						break;
//		case RTENAME:   sds_printf("\nThe entity name for %s is currently %ld,%ld. ",string1,rb1.resval.rlname[0],rb1.resval.rlname[1]);
//						sds_entsel("\nPick a new entity: ",rb1.resval.rlname,point1);
//						sds_setvar(string1,&rb1);
//						sds_printf("\nThe new entity you chose for %s is now %ld,%ld.  ",string1,rb1.resval.rlname[0],rb1.resval.rlname[1]);
//						break;
//		case RTPICKS:   sds_printf("\nThe selection set name for %s is currently %ld,%ld. ",string1,rb1.resval.rlname[0],rb1.resval.rlname[1]);
//						rb1.resval.rlname[0]=0;
//						rb1.resval.rlname[1]=0;
//						sds_ssget(NULL,point1,point2,NULL,&rb1.resval.rlname);
//						sds_setvar(string1,&rb1);
//						sds_printf("\nThe new selection set you chose for %s is now %ld,%ld.  ",string1,rb1.resval.rlname[0],rb1.resval.rlname[1]);
//						break;
//		case RTORINT:	sds_printf("\nThe result type for %s is RTORIENT:  An Orientation - ???. ",string1);
//						break;
		case RT3DPOINT: sds_printf("\nThe result type for %s is RT3DPOINT:  A 3D Point - 3 Reals @ 8 Bytes each. ",string1);
						sds_printf("\nIts x,y,z value is %f,%f,%f. ",rb1.resval.rpoint[0],rb1.resval.rpoint[1],rb1.resval.rpoint[2]);
						break;
//		case RTLONG:    sds_printf("\nThe result type for %s is RTLONG:  A Long - 4 Bytes. ",string1);
//						sds_printf("\nIts value is %f. ",rb1.resval.rlong);
//						break;
//		case RTVOID:    sds_printf("\nThe result type for %s is RTVOID:  Void - ???. ",string1);	break;
//		case RTLB:      sds_printf("\nThe result type for %s is RTLB:  A List Begin - ???. ",string1);	break;
//		case RTLE:      sds_printf("\nThe result type for %s is RTLE:  A List End - ???. ",string1);	break;
//		case RTDOTE:    sds_printf("\nThe result type for %s is RTDOTE:  Dot, for dotted pairs - ???. ",string1);	break;
//		case RTT:       sds_printf("\nThe result type for %s is RTT:  True, for AutoLISP - ???. ",string1);	break;
//		case RTNIL:     sds_printf("\nThe result type for %s is RTNIL:  Nil, for AutoLISP - ???. ",string1);	break;
//		case RTDXF0:    sds_printf("\nThe result type for %s is RTDXF0:  The Zero Group Code for DXF Lists - ???. ",string1);	break;
		default:		sds_printf("\nThe variable type for %s is not handled by this function. ",string1);
	}
    sds_printf("\nsds_setvar returned %i",returni);

	sds_printf("\nDone with the OB_GETSETVAR test.");
	return 0;
}



/***************************************************************************
	This function tests each of the SDS_RET*** functions for passing
	values to LISP functions:

		SDS_RETINT		SDS_RETREAL		SDS_RETPOINT
		SDS_RETSTR		SDS_RETNAME		SDS_RETVAL
		SDS_RETNIL		SDS_RETT		SDS_RETVOID
		SDS_RETLIST

    Each of these is documented in the Word file sds_list.doc.
	Type OB_PASSTOLISP at the command line to run this.

***************************************************************************/
int ob_func15(void) {

    sds_printf("\nThis was done using a set of LISP functions.  Each name is");
	sds_printf("\nLISP followed by the function, like OB_LISPRETVAL.");
    return(0);
}


/***************************************************************************
	This function tests the SDS_WCMATCH function:

		SDS_WCMATCH

    Each of these is documented in the Word file sds_list.doc.
	Type OB_WILDCARD at the command line to run this.

***************************************************************************/
int ob_func16(void) {

	int returni;
	int testnum=0;
	char doesthis[512],matchthis[512],string1[512];

    sds_printf("\nTesting sds_wcmatch...");
	sds_textscr();

	while (testnum<20){
		switch (testnum){    //These are all cases that *should* match:
			case 0:		strncpy(doesthis,"ABCDEFG",sizeof(doesthis)-1);
						strncpy(matchthis,"ABCDEFG",sizeof(matchthis)-1);		break;
			case 1:		strncpy(matchthis,"@@@@@@@",sizeof(matchthis)-1);		break;
			case 2:		strncpy(matchthis,"???????",sizeof(matchthis)-1);		break;
			case 3:		strncpy(matchthis,"*",sizeof(matchthis)-1);				break;
			case 4:		strncpy(matchthis,"~GFEDCBA",sizeof(matchthis)-1);		break;
			case 5:		strncpy(matchthis,"~#######",sizeof(matchthis)-1);		break;
			case 6:		strncpy(matchthis,"~.......",sizeof(matchthis)-1);		break;
			case 7:		strncpy(matchthis,"ABCDEFG,abcdefg",sizeof(matchthis)-1);break;
			case 8:		strncpy(matchthis,"ABC??@@",sizeof(matchthis)-1);		break;
			case 9:		strncpy(matchthis,"ABC*",sizeof(matchthis)-1);			break;

			case 10:	strncpy(doesthis,"B",sizeof(doesthis)-1);
						strncpy(matchthis,"[BxY56&]",sizeof(matchthis)-1);		break;
			case 11:	strncpy(matchthis,"[~0-9]",sizeof(matchthis)-1);		break;
			case 12:	strncpy(matchthis,"[~XYZ]",sizeof(matchthis)-1);		break;
			case 13:	strncpy(matchthis,"~b",sizeof(matchthis)-1);			break;
			case 14:	strncpy(matchthis,"@",sizeof(matchthis)-1);				break;
			case 15:	strncpy(matchthis,"~#",sizeof(matchthis)-1);			break;
			case 16:	strncpy(matchthis,"[A-L]",sizeof(matchthis)-1);			break;
			case 17:	strncpy(matchthis,"[~M-Z]",sizeof(matchthis)-1);		break;
			case 18:	strncpy(matchthis,"~.",sizeof(matchthis)-1);			break;
			case 19:	strncpy(matchthis,"[B]",sizeof(matchthis)-1);			break;
			default:	sds_printf("\nAn error has occurred.");
		}	// End of first SWITCH.
		returni=sds_wcmatch(doesthis,matchthis);
			if (returni==RTNORM)
				sds_printf("\nCorrectly matches \"%s\" with \"%s\".",doesthis,matchthis);
		else{
			sds_printf("\nFailed to match \"%s\" with \"%s\".  ",doesthis,matchthis);
			sds_getstring(0,"Press Enter to continue",string1);
		}	// End of ELSE.
    sds_printf("\nsds_wcmatch returned %i",returni);
	testnum++;
	}	//End of first WHILE.

	testnum=0;

	while (testnum<10){
		switch (testnum){    //These are all cases that should *not* match:
			case 0:		strncpy(doesthis,"ABCDEFG",sizeof(doesthis)-1);
						strncpy(matchthis,"abcdefg",sizeof(matchthis)-1);		break;
			case 1:		strncpy(matchthis,"#######",sizeof(matchthis)-1);		break;
			case 2:		strncpy(matchthis,"1234567",sizeof(matchthis)-1);		break;
			case 3:		strncpy(matchthis,"~*",sizeof(matchthis)-1);			break;
			case 4:		strncpy(matchthis,"[Z]",sizeof(matchthis)-1);			break;
			case 5:		strncpy(matchthis,"[0-9]",sizeof(matchthis)-1);			break;
			case 6:		strncpy(matchthis,"[~DOG]",sizeof(matchthis)-1);		break;
			case 7:		strncpy(matchthis,"[~ABCDEFG]",sizeof(matchthis)-1);	break;
			case 8:		strncpy(matchthis,"~GFEDCBA",sizeof(matchthis)-1);		break;
			case 9:		strncpy(matchthis,"~ABC...",sizeof(matchthis)-1);		break;
			default:	sds_printf("\nAn error has occurred.");
		}	// End of second SWITCH.

		returni=sds_wcmatch(doesthis,matchthis);
		if (returni==RTNORM){
			sds_printf("\nFailed to reject \"%s\" versus \"%s\".  ",doesthis,matchthis);
			sds_getstring(0,"Press Enter to continue",string1);
			}	// End of IF.
		else
			sds_printf("\nCorrectly rejected \"%s\" versus \"%s\".",doesthis,matchthis);
    sds_printf("\nsds_wcmatch returned %i",returni);
	testnum++;
	}	//End of second WHILE.

    sds_printf("\nDone with the OB_WILDCARD test.");
	sds_printf("\nType OB_DRAWING if you wish to return to the drawing window.");
    return(0);
}


/***************************************************************************
	This function tests each of the SDS_SS*** functions:

		SDS_SSGET		SDS_SSADD		SDS_SSDEL
		SDS_SSFREE		SDS_SSLENGTH	SDS_SSNAME
		SDS_SSMEMB		SDS_XFORMSS

    Each of these is documented in the Word file sds_list.doc.
	Type OB_SELECTION at the command line to run this.

***************************************************************************/
int ob_func17(void) {
//
	int returni,count;
	long setlen,index;
	char string1[512];
	sds_name entname1, entname2, entname3, entname4, entnamex, setname1, setname2,setname3;
//	struct sds_resbuf filter;
//	struct sds_resbuf *polypts;
//	void *voidptr1;
//	void *voidptr2;
	sds_point getpt1 = {2.0,2.0,0.0};
	sds_point getpt2 = {10.0,10.0,0.0};
//	sds_point getpt3,getpt4;

    sds_point point1 = {0.0,0.0,0.0};
	sds_point point2 = {17.0,10.0,0.0};
	sds_point rect1s = {1,1,0};
	sds_point rect1e = {2,2,0};
	sds_point circle1ctr = {1.5,3.5,0.0};
	sds_point circle1edge = {1,3.5,0.0};
	sds_point line1s = {1,5,0};
	sds_point line1e = {10,5,0};

	count=0;
	index=0L;

	sds_graphscr();
	sds_printf("\nFirst, we will draw a few entities to use in these tests.");
    sds_command(RTSTR,"erase",RTSTR,"all",RTSTR,"");
    sds_command(RTSTR,"zoom",RTSTR,"W",RTPOINT,point1,RTPOINT,point2,RTNONE);
    sds_command(RTSTR,"rectang",RTPOINT,rect1s,RTPOINT,rect1e,RTNONE);
    sds_command(RTSTR,"array",RT3DPOINT,rect1s,RTSTR,"",RTSTR,"R",RTSTR,"1",RTSTR,"10",RTSTR,"2",RTNONE);
	sds_command(RTSTR,"circle",RT3DPOINT,circle1ctr,RTREAL,0.5,RTNONE);
    sds_command(RTSTR,"array",RT3DPOINT,circle1edge,RTSTR,"",RTSTR,"R",RTSTR,"1",RTSTR,"10",RTSTR,"2",RTNONE);
    sds_command(RTSTR,"line",RT3DPOINT,line1s,RT3DPOINT,line1e,RTSTR,"",RTNONE);
    sds_command(RTSTR,"array",RT3DPOINT,line1s,RTSTR,"",RTSTR,"R",RTSTR,"10",RTSTR,"1",RTSTR,"1",RTNONE);


//	sds_printf("\n You need to already have some entities drawn on the screen to use this.");
//	sds_getstring(0,"\nPress Enter to continue.  ",string1);

    sds_printf("\nTesting sds_ssadd...");
	sds_entsel("\nPick an entity: ",entname1,point1);	// Get the name of an entity.
    returni=sds_ssadd(entname1,NULL,setname1);			// Make a new set containing entname1.
	sds_printf("The first entity created the selection set \"%ld\".",setname1);
    sds_printf("\nsds_ssadd returned %i",returni);
	sds_entsel("\nPick another entity to add to the set: ",entname2,point1);	// Get the name of a second entity.
	returni=sds_ssadd(entname2,setname1,setname2);		// Add entname2 to the current set (setname2 should == setname1 (?) )
	sds_printf("\nThe selection set should now contain both entities you selected.");
    sds_printf("\nsds_ssadd returned %i",returni);
	sds_printf("\nHighlighting entities in the current set... ");
	sds_sslength(setname2,&setlen);
	while (index < setlen){
		returni=sds_ssname(setname2,index,entnamex);
		if (returni != RTNORM)
			sds_getstring(0,"\nsds_ssname was not able to find the entity to highlight. ",string1);
		else{
		sds_redraw(entnamex,3);
		index++;
		}
	}
	index=0;
	sds_getstring(0,"\nHit Enter to unhighlight everything...",string1);
	sds_redraw(NULL,4);

	sds_entsel("\nPick one more entity: ",entname3,point1);
	sds_ssadd(entname3,setname2,setname3);				// The new current set is setname3
	sds_sslength(setname3,&setlen);						// Get the number of entities in the set.
	sds_printf("\nThere are %li entities in the set \"%ld\". ",setlen,setname3);
	sds_entsel("\nPick any entity:  ",entname4,point1); // Pick any entity.
	returni=sds_ssmemb(entname4,setname3);				// See if it's in the current set.
	if (returni==RTNORM)
		sds_printf("\nThat entity is in the existing set. ");
	else
		sds_printf("\nThat entity is not in the existing set. ");

	sds_printf("\nHighlighting entities in the current set... ");
	while (index < setlen){
		returni=sds_ssname(setname3,index,entnamex);
		if (returni != RTNORM)
			sds_getstring(0,"\nsds_ssname was not able to find the entity to highlight. ",string1);
		else{
			sds_redraw(entnamex,3);
			index++;
		}
	}
	index=0;
	sds_getstring(0,"\nHit Enter to unhighlight everything...",string1);
	sds_redraw(NULL,4);

	sds_printf("\nNow removing the first entity from the set...");
	sds_ssdel(entname1,setname3);						// Delete the first entity.
	returni=sds_ssmemb(entname1,setname3);				// Is the first entity there, or gone?
	if (returni==RTNORM)
		sds_printf("\nAn error has occured...the first entity was not deleted. ");
	else
		sds_printf("\nThe first entity was successfully deleted from the set. ");
	sds_sslength(setname3,&setlen);						// How many entities now?
	sds_printf("\nThe set now contains %li entities. ",setlen);
	sds_printf("\nHighlighting entities in the current set... ");
	while (index < setlen){
		returni=sds_ssname(setname3,index,entnamex);
		if (returni != RTNORM)
			sds_getstring(0,"\nsds_ssname was not able to find the entity to hightlight. ",string1);
		else{
		sds_redraw(entnamex,3);
		index++;
		}
	}
	index=0;
	sds_getstring(0,"\nHit Enter to unhighlight everything...",string1);
	sds_redraw(NULL,4);

	sds_printf("\nNow freeing all the sets used above... ");
	returni = sds_ssfree(setname1);
	if (returni != RTNORM)
		sds_printf("\nAn error has occured... setname1 was not freed. ");
	returni = sds_ssfree(setname2);
	if (returni != RTNORM)
		sds_printf("\nAn error has occured... setname2 was not freed. ");
	returni = sds_ssfree(setname3);
	if (returni != RTNORM)
		sds_printf("\nAn error has occured... setname3 was not freed. ");
/*  THIS DOESN'T WORK RIGHT YET.  COULD BE ME, COULD BE SSGET.
	sds_printf("\nTesting sds_ssget...");
	while(count<10){
		sds_redraw(NULL,4);
		switch(count){
			case 0:		sds_printf("\nGet current set, or ask for a new one...");
						sds_ssget(NULL,NULL,NULL,NULL,setname1);		break;
			case 1:		sds_printf("\nGet current set if there is one...");
						sds_ssget("I",NULL,NULL,NULL,setname1);			break;
			case 2:		sds_printf("\nGet the previous selection set...");
						sds_ssget("P",NULL,NULL,NULL,setname1);			break;
			case 3:		sds_printf("\nGet the last entity added to the drawing...");
						sds_ssget("L",NULL,NULL,NULL,setname1);			break;
			case 4:		sds_printf("\nGet the entity that passes through (2,2)...");
						sds_ssget(NULL,getpt1,NULL,NULL,setname1);		break;
			case 5:		sds_printf("\nGet all the entities in the window (2,2)(10,10)...");
						sds_ssget("W",getpt1,getpt2,NULL,setname1);		break;
			case 6:		sds_printf("\nGet the entities within this polygon...");
						getpt3[0]=4.0; getpt3[1]=9.0; getpt3[2]=0.0;
						getpt4[0]=8.0; getpt4[1]=1.0; getpt4[2]=0.0;
						polypts=sds_buildlist(RTPOINT,getpt1,RTPOINT,getpt3,
							                  RTPOINT,getpt2,RTPOINT,getpt4);
						sds_ssget("WP",polypts,NULL,NULL,setname1);		break;
			case 7:		sds_printf("\nGet entities crossing the above polygon...");
						sds_ssget("CP",polypts,NULL,NULL,setname1);
						sds_relrb(polypts);								break;
			case 8:		sds_printf("\nGet entities crossing rectangle (2,2)(10,10)...");
						sds_ssget("C",getpt1,getpt2,NULL,setname1);	break;
			case 9:		sds_printf("\nGet current set, or ask for a new one...");
						sds_ssget(NULL,NULL,NULL,NULL,setname1);	break;
			default:	sds_printf("\nAn error occurred.");
		}
		sds_sslength(setname1,&setlen);   // How many items in the set?
		sds_printf("\nThe set now contains %li entities. ",setlen);
		sds_printf("\nHighlighting entities in the current set... ");
		while (index < setlen){		// Go through the set & get each entity.
			returni=sds_ssname(setname1,index,entnamex);
			if (returni != RTNORM)
				sds_getstring(0,"\nsds_ssname was not able to find the entity to highlight. ",string1);
			else
				sds_redraw(entnamex,3);  // Highlights each entity in the set.
			index++;	// Go on to get the next entity, if there is one
		}				// End of while loop for redrawing each member of set.
		sds_getstring(0,"\nPress Enter to continue with the next test.",string1);  // Pause
		sds_redraw(NULL,4);	// Unhighlight everything.
		count++;    // Procede with the next case until done.
		index=0;	// Reset index for the next set.
	}				// End of while loop for testing different ssgets.
	index=0;		// Reset index for the next function that uses it.
	sds_free(setname1);	// Free the selection set.
*/

	sds_sslength(setname1,&setlen);						// How many entities?
	sds_printf("\nThe set now contains %li entities. ",setlen);
	sds_printf("\nHighlighting entities in the current set... ");
	while (index < setlen){
		returni=sds_ssname(setname3,index,entnamex);
		if (returni != RTNORM)
			sds_getstring(0,"\nsds_ssname was not able to find the entity to highlight. ",string1);
		else{
		sds_redraw(entnamex,3);
		index++;
		}
	}
	index=0;
	sds_getstring(0,"\nHit Enter to unhighlight everything...",string1);
	sds_redraw(NULL,4);
	sds_printf("\nNow freeing the set used above... ");
	returni = sds_ssfree(setname1);
	if (returni != RTNORM)
		sds_printf("\nAn error has occured... setname1 was not freed. ");




	//    sds_printf("\nTesting sds_xformss...");
//    returni=sds_xformss();
//    sds_printf("\nsds_xformss returned %i",returni);
//

    sds_printf("\nDone with the OB_SELECTION test.");
    return(0);
}


/***************************************************************************
	This function tests each of the SDS_ENT*** entity functions:

		SDS_ENTGET		SDS_ENTMOD		SDS_ENTMAKE
		SDS_ENTDEL		SDS_ENTNEXT		SDS_ENTLAST
		SDS_HANDENT		SDS_ENTSEL		SDS_NENTSELP
		SDS_NENTSEL		SDS_ENTUPD

    Each of these is documented in the Word file sds_list.doc.
	Type OB_ENTITIES at the command line to run this.

***************************************************************************/
int ob_func18(void) {

	int returni,after;
//	sds_real real1=0.0;
	sds_name entity1;
	sds_name entity2;
	sds_point point1;
	sds_point point2;
    char string1[512];
	char enthandle[512];
	struct resbuf *entlist=NULL;
	struct resbuf *templist=NULL;
	struct resbuf *applist=NULL;
	sds_name entname;

	//variables n'stuff for testing sds_nentselp:
//	sds_point pickpt = {0.0,0.0,0.0};
//	sds_name pickedent;
//	short pickornot;
//	sds_matrix ecs2wcs;
//	struct sds_resbuf **nestblks;

/*	THIS FUNCTION IS NOT COMPLETE.
    sds_printf("\nTesting sds_nentselp...");
	pickornot=0;
    returni=sds_nentselp("Pick an entity for testing sds_nentselp",
		                  pickedent, pickpt, pickornot,
						  ecs2wcs, nestblks);
    sds_printf("\nsds_nentselp returned %i",returni);
*/

/*
    sds_printf("\nTesting sds_entsel...");
	sds_getpoint(NULL,"Draw a line - First point: ",point1);
	sds_getpoint(point1,"\nSecond point: ",point2);
    sds_command(RTSTR,"line",RT3DPOINT,point1,RT3DPOINT,point2,RTSTR,"",RTNONE);
	sds_getpoint(NULL,"Draw another line - First point: ",point1);
	sds_getpoint(point1,"\nSecond point: ",point2);
    sds_command(RTSTR,"line",RT3DPOINT,point1,RT3DPOINT,point2,RTSTR,"",RTNONE);
	returni=sds_entsel(NULL,entity1,point1);
	sds_redraw(NULL,4);
    sds_redraw(entity1,3);
	sds_printf("\nThe point on the entity you chose is %f, %f, %f. ",point1[0],point1[1],point1[2]);
    sds_printf("\nsds_entsel returned %i",returni);
	returni=sds_entsel("\nPick another entity: ",entity1,point1);
	sds_redraw(NULL,4);
    sds_redraw(entity1,3);
	sds_printf("\nThe point on the entity you chose is %f, %f, %f. ",point1[0],point1[1],point1[2]);
    sds_printf("\nsds_entsel returned %i",returni);

    sds_printf("\nTesting sds_entnext...");
	sds_getpoint(NULL,"Draw a line - First point: ",point1);
	sds_getpoint(point1,"\nSecond point: ",point2);
    sds_command(RTSTR,"line",RT3DPOINT,point1,RT3DPOINT,point2,RTSTR,"",RTNONE);
	sds_getpoint(NULL,"Draw another line - First point: ",point1);
	sds_getpoint(point1,"\nSecond point: ",point2);
    sds_command(RTSTR,"line",RT3DPOINT,point1,RT3DPOINT,point2,RTSTR,"",RTNONE);
	sds_entsel("\nPick the first line: ",entity1,point1);
	sds_redraw(NULL,4);
    sds_redraw(entity1,3);
	sds_getstring(0,"\nPress enter to highlight the second line: ",string1);
    returni=sds_entnext(entity1,entity2);
	sds_redraw(NULL,4);
    sds_redraw(entity2,3);
    sds_printf("\nsds_entnext returned %i",returni);

    sds_printf("\nTesting sds_entdel...");
	sds_getstring(0,"\nPress enter to delete the highlighted line: ",string1);
    returni=sds_entdel(entity2);
    sds_printf("\nsds_entdel returned %i",returni);
	sds_entsel("\nPick another entity to delete: ",entity1,point1);
    returni=sds_entdel(entity1);
    sds_printf("\nsds_entdel returned %i",returni);
	sds_getstring(0,"\nPress enter to undelete the first line: ",string1);
    returni=sds_entdel(entity2);
    sds_printf("\nsds_entdel returned %i",returni);
	sds_getstring(0,"\nPress enter to undelete the entity you chose: ",string1);
    returni=sds_entdel(entity1);
    sds_printf("\nsds_entdel returned %i",returni);

    sds_printf("\nTesting sds_entlast...");
	sds_getstring(0,"\nPress enter to highlight the last entity in the drawing: ",string1);
    returni=sds_entlast(entity1);
	sds_redraw(NULL,4);
    sds_redraw(entity1,3);
    sds_printf("\nsds_entlast returned %i",returni);
*/

    sds_printf("\nTesting sds_entget...");
	sds_entsel("\nPick an entity: ",entname,point1);
    entlist=sds_entget(entname);
	ob_listdxf(entlist);

	if (entlist!=NULL){
		sds_relrb(entlist);
		entlist=NULL;
	}
    sds_printf("\nDone testing sds_entget. ");

    sds_printf("\nTesting sds_entmake...");
	point1[0]=1.0;
	point1[1]=1.0;
	point1[2]=0.0;
	point2[0]=3.0;
	point2[1]=3.0;
	point2[2]=0.0;
	entlist=sds_buildlist(RTDXF0, "line",	// Entity type		Required
							   8, "0",		// Layer name		Required
	//						   5, xxx,		// Handle name		Optional, no default
	//						   6, abc,		// Linetype name	Defaults to BYLAYER
	//						  39, 123,		// Thickness		Defaults to zero
							  62, 35,		// Color number		Defaults to BYLAYER (256)
	//						  67, 0 or 1,   // Model/Paper Sp.	Defaults to Model (0)
							  10, point1,	// Starting point for the line
							  11, point2,	// Ending point for the line
								  RTNONE);  // List always ends with RTNONE or 0
    returni=sds_entmake(entlist);
	if (entlist!=NULL){
		sds_relrb(entlist);
		entlist=NULL;
    }
    sds_printf("\nsds_entmake returned %i",returni);

	sds_entlast(entname);			// Get the name of the last entity drawn.
	entlist=sds_entget(entname);	// Get the resbuf for that entity.
									// Go through the entity's resbuf...
	for (templist=entlist;templist->rbnext!=NULL;templist=templist->rbnext)
	{
		if (templist->restype==62){			// When it gets to the color
			templist->resval.rint=240;		// group, change it to 240.
			after=templist->resval.rint;	// Make a variable to check later.
		}
	}
	returni=sds_entmod(entlist);    // Put the (modified) resbuf back
    entlist=sds_entget(entname);	// Get the newly modified entity info...
	ob_listdxf(entlist);			// Print it out at the command line
	if (entlist!=NULL)				// If there's anything in the entlist...
		sds_relrb(entlist);				// Release it
	entlist=NULL;					// Make it a NULL pointer, just in case
	templist=NULL;					// Make templist NULL, too.
	if (after==240)					// See if the color really got changed.
		sds_printf("\nEntmod succeeded in changing the color number.");
	else
		sds_getstring(0,"\nEntmod failed to change the color number. Press Enter.",string1);
    sds_printf("\nsds_entmod returned %i",returni);

	sds_regapp("ourbase");		// Register the application "ourbase"

    sds_printf("\nTesting sds_entgetx...");
	point1[0]=1.0;
	point1[1]=3.0;
	point1[2]=0.0;
	point2[0]=7.0;
	point2[1]=3.0;
	point2[2]=0.0;
	entlist=sds_buildlist(RTDXF0, "line",	// Entity type		Required
							   8, "0",		// Layer name		Required
	//						   5, xxx,		// Handle name		Optional, no default
	//						   6, abc,		// Linetype name	Defaults to BYLAYER
	//						  39, 123,		// Thickness		Defaults to zero
							  62, 70,		// Color number		Defaults to BYLAYER (256)
	//						  67, 0 or 1,   // Model/Paper Sp.	Defaults to Model (0)
							  10, point1,	// Starting point for the line
							  11, point2,	// Ending point for the line
								  RTNONE);  // List always ends with RTNONE or 0
    sds_entmake(entlist);
	sds_entlast(entname);			// Get the name of the last entity drawn.
	sds_redraw(entname,3);          // Highlight the last entity drawn.
	sds_getstring(0,"\nPress Enter to update this entity in the database. ",string1);
	sds_entupd(entname);
	sds_getstring(0,"Press Enter to unhighlight the entity that was just updated.",string1);
	sds_redraw(entname,4);
	applist=sds_buildlist(RTSTR,"ourbase",RTNONE);
		/* This test should include the appname for an application running
		   under ICAD.  The list above will specify that appname, and the
		   resbuf returned by sds_entgetx will include the EED for the entity
		   that belongs to that application.  For now, it just returns any
		   EED for ourbase, which should be nothing.
        */
    entlist=sds_entgetx(entname,applist);  // Get its entity and app info
	ob_listdxf(entlist);			// Print entity info out at the command line

	if (entlist!=NULL)				// If there's anything in the entlist...
		sds_relrb(entlist);				// Release it
	if (applist!=NULL)				// If there's anything in the applist...
		sds_relrb(applist);				// Release it
	entlist=NULL;					// Make it a NULL pointer
	applist=NULL;					// Make applist NULL, too
    sds_printf("\nDone testing sds_entgetx. ");


//    sds_printf("\nTesting sds_handent...");
	sds_entlast(entity1);			// Get the name of the last entity drawn.
	entlist=sds_entget(entity1);	// Get the resbuf for that entity.
									// Go through the entity's resbuf to the handle...
	/* For loop explanation... Make a new linked list called templist, so we
	   don't lose the address of the start of the original list (entlist).
	   Then, as long as we're still within the list, look for when the restype
	   is 5 - that's the handle.  Get the entity's name by using sds_handent.
	*/
	for (templist=entlist;templist->rbnext!=NULL;templist=templist->rbnext){
		if (templist->restype==5){
			sds_handent(templist->resval.rstring,entity2);
		}
	}
	returni=sds_handent(enthandle,entity2);
	sds_getstring(0,"Press Enter to highlight the entity from sds_handent",string1);
	sds_redraw(entity2,3);
	sds_getstring(0,"Press Enter to unhighlight the entity from sds_handent",string1);
	sds_redraw(entity2,4);
	if (entlist!=NULL)				// If there's anything in the entlist...
		sds_relrb(entlist);				// Release it
	entlist=NULL;					// Make it a NULL pointer, just in case
	templist=NULL;					// Make templist NULL, too.
    sds_printf("\nsds_handent returned %i",returni);


//    sds_printf("\nTesting sds_nentsel...");
//    returni=sds_nentsel();
//    sds_printf("\nsds_nentsel returned %i",returni);


	sds_printf("\nDone with the OB_ENTITIES test. ");
    return(0);
}


/***************************************************************************
	This function tests the EED functions:

		SDS_REGAPP		SDS_ENTGETX		SDS_XDSIZE
		SDS_XDROOM

    Each of these is documented in the Word file sds_list.doc.
	Type OB_EED at the command line to run this.

***************************************************************************/
int ob_func19(void) {
//
//	int int1,returni;
//	sds_real real1=0.0;
//  sds_point point1,point2;
//  char string1[512];
//
//    sds_printf("\nTesting sds_regapp...");
//    returni=sds_regapp();
//    sds_printf("\nsds_regapp returned %i",returni);
//
//    sds_printf("\nTesting sds_entgetx...;
//    THIS RETURNS A STRUCT RESBUF=sds_entgetx();
//    sds_printf("\nsds_entgetx returned %x",returnx);
//
//    sds_printf("\nTesting sds_xdsize...");
//    returni=sds_xdsize();
//    sds_printf("\nsds_xdsize returned %i",returni);
//
//    sds_printf("\nTesting sds_xdroom...");
//    returni=sds_xdroom();
//    sds_printf("\nsds_xdroom returned %i",returni);
//
    sds_printf("\nDone with the OB_EED test.");
	sds_printf("\nNothing happened.  The OB_EED test is not complete.");
    return(0);
}


/***************************************************************************
	This function tests each of the SDS_TBL*** functions:

		SDS_TBLNEXT		SDS_TBLSEARCH

    Each of these is documented in the Word file sds_list.doc.
	Type OB_SYMBOLTABLE at the command line to run this.

***************************************************************************/
int ob_func20(void) {

//	int int1,returni;
//	sds_real real1=0.0;
//  sds_point point1,point2;
//	char string2[512];
	char tablename[512];
	char lookfor[512];
	int types;
    char string1[512];
	struct sds_resbuf *nextbuf;
	struct sds_resbuf *searchbuf=NULL;
	int first = 1;

    sds_printf("\nTesting sds_tblnext...");
	for (types=0; types<9; types++){
		switch(types){
			case 0:    strncpy(tablename,"APPID",   sizeof(tablename)-1);	break;
			case 1:    strncpy(tablename,"BLOCK",   sizeof(tablename)-1);	break;
			case 2:    strncpy(tablename,"DIMSTYLE",sizeof(tablename)-1);	break;
			case 3:    strncpy(tablename,"STYLE",   sizeof(tablename)-1);	break;
			case 4:    strncpy(tablename,"LAYER",   sizeof(tablename)-1);	break;
			case 5:    strncpy(tablename,"LTYPE",   sizeof(tablename)-1);	break;
			case 6:    strncpy(tablename,"UCS",     sizeof(tablename)-1);	break;
			case 7:    strncpy(tablename,"VIEW",    sizeof(tablename)-1);	break;
			case 8:    strncpy(tablename,"VPORT",   sizeof(tablename)-1);	break;
			default:   sds_getstring(0,"\nThe counter is broken, and generated an incorrect value. Press Enter to continue.",string1);
		}
		nextbuf = sds_tblnext(tablename,1);
		for (first=0;nextbuf!=NULL;first=0){
			nextbuf = sds_tblnext(tablename,first);
			ob_listdxf(nextbuf);
		}
		sds_printf("\nTested %s.",tablename);
	}
	sds_relrb(nextbuf);
	sds_printf("\nDone testing sds_tblnext.");


    sds_printf("\nTesting sds_tblsearch...");
	sds_getstring(1,"\nWhat string do you want to search for?",lookfor);
	for (types=0; types<9; types++){
		switch(types){
			case 0:    strncpy(tablename,"APPID",   sizeof(tablename)-1);	break;
			case 1:    strncpy(tablename,"BLOCK",   sizeof(tablename)-1);	break;
			case 2:    strncpy(tablename,"DIMSTYLE",sizeof(tablename)-1);	break;
			case 3:    strncpy(tablename,"STYLE",   sizeof(tablename)-1);	break;
			case 4:    strncpy(tablename,"LAYER",   sizeof(tablename)-1);	break;
			case 5:    strncpy(tablename,"LTYPE",   sizeof(tablename)-1);	break;
			case 6:    strncpy(tablename,"UCS",     sizeof(tablename)-1);	break;
			case 7:    strncpy(tablename,"VIEW",    sizeof(tablename)-1);	break;
			case 8:    strncpy(tablename,"VPORT",   sizeof(tablename)-1);	break;
			default:   sds_getstring(0,"\nThe counter is broken, and generated an incorrect value. Press Enter to continue.",string1);
		}
		for (first=1;nextbuf!=NULL;first=0){
			searchbuf = sds_tblsearch(tablename,lookfor,0);
			ob_listdxf(searchbuf);
		    if(searchbuf!=NULL) {sds_relrb(searchbuf);searchbuf=NULL;}
		}
		sds_printf("\nTested %s.",tablename);
	}
	sds_relrb(nextbuf);
    sds_printf("\nDome testing sds_tblsearch.");

    sds_printf("\nDone with the OB_SYMBOLTABLE test.");
    return(0);
}



/***************************************************************************
	This function tests the SDS_ABORT function:

	Type OB_ABORTNOW at the command line to run this.

***************************************************************************/
int ob_func21(void) {

    sds_printf("\nTesting sds_abort...");
    sds_abort(" is aborting now.\nGoodbye!");

    return(0);
}

/***************************************************************************
	This function tests the SDS_EXIT function:

	Type OB_EXITNOW at the command line to run this.

***************************************************************************/
int ob_func22(void) {

    sds_printf("\nTesting sds_exit...\nOURBASE is shutting down now.");
    sds_exit(0);

    return(0);
}

/***************************************************************************
	This function tests the SDS_CMD function:

	Type OB_COMMANDLIST at the command line to run this.

***************************************************************************/
int ob_func23(void){
	struct sds_resbuf *listbuf1;
	struct sds_resbuf *listbuf2;
	sds_point point1 = {2.0, 2.0};
	sds_point point2 = {3.0, 3.0};

	listbuf1=sds_buildlist(RTSTR,"circle",RTPOINT,point1,RTPOINT,point2,RTNONE);
	sds_cmd(listbuf1);
//	sds_relrb(listbuf1);

	listbuf2=sds_buildlist(RTSTR,"circle",RTSTR,"5,2",RTSTR,"6,3",RTNONE);
	sds_cmd(listbuf2);
//	sds_relrb(listbuf2);

return(0);
}


/***************************************************************************
	THE FOLLOWING IS A BLANK FOR CREATING EACH SET OF COMMAND TESTS.
	DON'T USE IT...JUST COPY IT AND RE-USE IT.

***************************************************************************/


/***************************************************************************
	This function tests each of the SDS_*** functions:

		SDS_			SDS_			SDS_
		SDS_			SDS_			SDS_
		SDS_			SDS_			SDS_

    Each of these is documented in the Word file sds_list.doc.
	Type OB_XXXXX at the command line to run this.

***************************************************************************/
//int ob_funcX(void) {
//
//	int int1,returni;
//	sds_real real1=0.0;
//  sds_point point1,point2;
//  char string1[512];
//
//    sds_printf("\nTesting sds_xxx...");
//    returni=sds_xxx();
//    sds_printf("\nsds_xxx returned %i",returni);
//
//    sds_printf("\nThank you.  Please report any bugs right away.");
//    return(0);
//}


/***************************************************************************

	End of sds_*** function tests

***************************************************************************/

/***************************************************************************

	The function below acts as an index for all of the above.
	Add to it as appropriate.

	Type OB_INDEX at the command line to see this.

***************************************************************************/
int ob_func30(void) {

    sds_textpage();
    sds_printf("\nThis is an index for testing our sds_*** functions.");
    sds_printf("\nType the group name at the command line to test each of the");
    sds_printf("\nsds_*** functions listed below it.");
	sds_printf("\n");
	sds_printf("\nOB_INITGET		All the options of the sds_initget function. ");
	sds_printf("\n	SDS_INITGET ");
	sds_printf("\n");
	sds_printf("\nOB_GETUSER		Functions that request input directly from the user: ");
	sds_printf("\n	SDS_GETINT		SDS_GETREAL		SDS_GETPOINT ");
	sds_printf("\n	SDS_GETSTRING		SDS_GETCORNER		SDS_GETDIST ");
	sds_printf("\n	SDS_GETANGLE		SDS_GETORIENT		SDS_KEYWORD ");
	sds_printf("\n	SDS_GETINPUT");
	sds_printf("\n");
	sds_printf("\nOB_GEOMETRY		Geometry functions: ");
	sds_printf("\n	SDS_ANGLE		SDS_DISTANCE		SDS_INTERS ");
	sds_printf("\n	SDS_POLAR		SDS_TEXTBOX ");
	sds_printf("\n");
	sds_printf("\nOB_CHARACTER		Character type checking and transformation:");
	sds_printf("\n	SDS_ISALNUM		SDS_ISALPHA		SDS_ISCNTRL	");
	sds_printf("\n	SDS_ISDIGIT		SDS_ISGRAPH		SDS_ISLOWER	");
	sds_printf("\n	SDS_ISPRINT		SDS_ISPUNCT		SDS_ISSPACE	");
	sds_printf("\n	SDS_ISUPPER		SDS_ISXDIGIT ");
	sds_printf("\n	SDS_TOLOWER		SDS_TOUPPER ");
	sds_printf("\n");
	sds_printf("\nOB_CONVERT		Conversion functions: ");
	sds_printf("\n	SDS_ANGTOF		SDS_ANGTOS		SDS_CVUNIT ");
	sds_printf("\n	SDS_DISTOF		SDS_RTOS ");
	sds_printf("\n");
	sds_printf("\nOB_DISPLAY		Displaying information: ");
	sds_printf("\n	SDS_GRAPHSCR		SDS_MENUCMD		SDS_PRINTF ");
	sds_printf("\n	SDS_PROMPT		SDS_REDRAW		SDS_TEXTPAGE ");
	sds_printf("\n	SDS_TEXTSCR ");
	sds_printf("\n");
	sds_printf("\nOB_GRAPHICS		Graphics functions: ");
	sds_printf("\n	SDS_GRCLEAR		SDS_GRDRAW		SDS_GRREAD ");
	sds_printf("\n	SDS_GRTEXT		SDS_GRVECS ");
	sds_printf("\n");
	sds_printf("\nOB_LISP			LISP interface functions: ");
	sds_printf("\n	SDS_INIT		SDS_LINK: ");
	sds_printf("\n");
	sds_printf("\nOB_EXTFUNCS		External functions: ");
	sds_printf("\n	SDS_DEFUN		SDS_UNDEF		SDS_GETFUNCODE");
	sds_printf("\n	SDS_GETARGS		SDS_REGFUNC		SDS_INVOKE");
	sds_printf("\n");
	sds_printf("\nOB_APPS			External applications: ");
	sds_printf("\n	SDS_XLOAD		SDS_XUNLOAD		SDS_LOADED");
	sds_printf("\n");
	sds_printf("\nOB_ERROR			Error handling functions: ");
	sds_printf("\n	SDS_FAIL		SDS_ALERT  ");
	sds_printf("\n");
	sds_printf("\nOB_MEMORY			Memory management functions: ");
	sds_printf("\n	SDS_NEWRB		SDS_BUILDLIST		SDS_RELRB");
	sds_printf("\n");
	sds_printf("\nOB_QUERYCOM		Query and command functions: ");
	sds_printf("\n	SDS_COMMAND		SDS_CMD			SDS_GETVAR");
	sds_printf("\n	SDS_SETVAR		SDS_FINDFILE		SDS_GETFILED");
	sds_printf("\n	SDS_GETSYM		SDS_PUTSYM		SDS_OSNAP");
	sds_printf("\n	SDS_VPORTS");
	sds_printf("\n");
	sds_printf("\nOB_PASSTOLISP		Functions for passing values to LISP functions: ");
	sds_printf("\n	SDS_RETINT		SDS_RETREAL		SDS_RETPOINT");
	sds_printf("\n	SDS_RETSTR		SDS_RETNAME		SDS_RETVAL");
	sds_printf("\n	SDS_RETNIL		SDS_RETT		SDS_RETVOID");
	sds_printf("\n	SDS_RETLIST");
	sds_printf("\n");
	sds_printf("\nOB_WILDCARD		The sds_wcmatch wildcard function: ");
	sds_printf("\n	SDS_WCMATCH");
	sds_printf("\n");
	sds_printf("\nOB_SELECTION		Selection Set functions: ");
	sds_printf("\n	SDS_SSGET		SDS_SSADD		SDS_SSDEL");
	sds_printf("\n	SDS_SSFREE		SDS_SSLENGTH		SDS_SSNAME");
	sds_printf("\n	SDS_SSMEMB		SDS_XFORMSS");
	sds_printf("\n");
	sds_printf("\nOB_ENTITIES		Entity functions: ");
	sds_printf("\n	SDS_ENTGET		SDS_ENTMOD		SDS_ENTMAKE");
	sds_printf("\n	SDS_ENTDEL		SDS_ENTNEXT		SDS_ENTLAST");
	sds_printf("\n	SDS_HANDENT		SDS_ENTSEL		SDS_NENTSELP");
	sds_printf("\n	SDS_NENTSEL		SDS_ENTUPD");
	sds_printf("\n");
	sds_printf("\nOB_EED			Extended Entity Data functions: ");
	sds_printf("\n	SDS_REGAPP		SDS_ENTGETX		SDS_XDSIZE");
	sds_printf("\n	SDS_XDROOM");
	sds_printf("\n");
	sds_printf("\nOB_SYMBOLTABLE	Next and Search functions for symbol tables: ");
	sds_printf("\n	SDS_TBLNEXT		SDS_TBLSEARCH");
	sds_printf("\n");
	sds_printf("\nOB_EXITNOW		The sds_exit function: ");
	sds_printf("\nThis actually exits.  Don't use it until you are done. ");
	sds_printf("\n	SDS_EXIT		");
	sds_printf("\n");
	sds_printf("\nOB_ABORTNOW		The sds_abort function: ");
	sds_printf("\nThis actually aborts.  Don't use it until you are done. ");
	sds_printf("\n	SDS_ABORT		");
	sds_printf("\n");
	sds_printf("\n");
	sds_printf("\nYou may find it easiest to continue to run this from this ");
	sds_printf("\ntext window because you can see more information at at time. ");
	sds_printf("\n");
	sds_printf("\nTo switch back to the graphics window at any time between ");
	sds_printf("\nthe test groups, type OB_DRAWING at the command line. ");
	sds_printf("\nTo switch back to the text window at any time between ");
	sds_printf("\nthe test groups, type OB_TEXTWIN at the command line. ");

    return(0);
}

/*This returns the user to the graphics window when they type "OB_DRAWING". */
int ob_func31(void) {
    sds_graphscr();
    return(0);
}

/*This switches the user to the text window when they type "OB_TEXTWIN". */
int ob_func32(void) {
    sds_textscr();
    return(0);
}

/* This is just pasted from the \icadlib\listdxf file for use with the
   entity functions.                                               */

void ob_listdxf(struct resbuf *elist) {
    char fs1[41],fc1;
    short dxfmod100,lc;
    unsigned short lnbrk,fu1,fu2;
    struct resbuf *tb;

    lnbrk=60;

    for (tb=elist,lc=0; tb!=NULL; tb=tb->rbnext,lc++) {
        if (lc>20) {
            if (sds_getstring(0,"\nPress ENTER to continue...",fs1)!=RTNORM)
                goto out;
            lc=0;
        }

        dxfmod100=tb->restype%100;
        sds_printf("\n%10d",tb->restype);
        if (tb->restype<0) {
            if (tb->restype==-1 || tb->restype==-2) sds_printf("  (0x%lX,0x%lX)",
                tb->resval.rlname[0],tb->resval.rlname[1]);
            continue;
        } else if (dxfmod100<10) {
            if (tb->restype==1004) sds_printf("  Binary chunk");
            else {
                if ((fu1=strlen(tb->resval.rstring))<=lnbrk) {
                    sds_printf("  |%s|",tb->resval.rstring);
                } else {  /* Too long for 1 sds_printf() */
                    for (fu2=lnbrk; fu2<fu1; fu2+=lnbrk) {
                        fc1=tb->resval.rstring[fu2];
                        tb->resval.rstring[fu2]=0;
                        if (fu2>lnbrk) sds_printf("\n          ");
                        sds_printf("  |%s|",
                            tb->resval.rstring+fu2-lnbrk);
                        tb->resval.rstring[fu2]=fc1;
                    }
                    sds_printf("\n            |%s|",
                        tb->resval.rstring+fu2-lnbrk);
                    lc+=fu1/lnbrk;
                }
            }
        } else if (dxfmod100<20)
            sds_printf("  (%0.15G,%0.15G,%0.15G)",tb->resval.rpoint[0],
                tb->resval.rpoint[1],tb->resval.rpoint[2]);
        else if (dxfmod100<38)
            sds_printf("  Other coords?");
        else if (dxfmod100<60)
            sds_printf("  %0.15G",tb->resval.rreal);
        else if (dxfmod100<80) {
            if (tb->restype==1071)  sds_printf("  %ld (long)",tb->resval.rlong);
            else sds_printf("  %d",tb->resval.rint);
        } else sds_printf("  Unknown");
    }

out: ;
//    IC_FREEIT
}


// The switch below is here just as a template for future work.
/*
	switch(rb1.restype){
		case RTNONE:    sds_printf("\nThe result type for %s is RTNONE:  No result. ",string1);
						sds_printf("\nTheir is no value for this return type. ");	break;
		case RTREAL:    sds_printf("\nThe result type for %s is RTREAL:  A Real - 8 Bytes. ",string1);
						sds_printf("\nIts value is %f. ",rb1.resval.rreal);	break;
		case RTPOINT:   sds_printf("\nThe result type for %s is RTPOINT:  A 2D Point - 3 Reals @ 8 Bytes each.  Last is ignored ",string1);
						sds_printf("\nIts x,y value is %f,%f. ",rb1.resval.rpoint[0],rb1.resval.rpoint[1]);	break;
		case RTSHORT:   sds_printf("\nThe result type for %s is RTSHORT:  A Short - 2 Bytes. ",string1);
						sds_printf("\nIts value is %i. ",rb1.resval.rint);	break;
		case RTANG:     sds_printf("\nThe result type for %s is RTANG:  An Angle in radians - a Real - 8 Bytes. ",string1);
						sds_printf("\nIts value is %f. ",rb1.resval.rreal);	break;
		case RTSTR:     sds_printf("\nThe result type for %s is RTSTR:  An 8 Byte pointer to a string. ",string1);
						sds_printf("\nThe string is \"%s\".",rb1.resval.rstring);
						free(rb1.resval.rstring); 	break;
		case RTENAME:   sds_printf("\nThe result type for %s is RTENAME:  An Entity Name - 2 Longs @ 4 Bytes each. ",string1);
						sds_printf("\nIts value is %f,%f. ",rb1.resval.rlname[0],rb1.resval.rlname[1]);	break;
		case RTPICKS:   sds_printf("\nThe result type for %s is RTPICKS:  A Selection Set Name - 2 Longs @ 4 Bytes each. ",string1);
						sds_printf("\nIts value is %f,%f. ",rb1.resval.rlname[0],rb1.resval.rlname[1]);	break;
		case RTORINT:	sds_printf("\nThe result type for %s is RTORIENT:  An Orientation - ???. ",string1);
		case RT3DPOINT: sds_printf("\nThe result type for %s is RT3DPOINT:  A 3D Point - 3 Reals @ 8 Bytes each. ",string1);
						sds_printf("\nIts x,y,z value is %f,%f,%f. ",rb1.resval.rpoint[0],rb1.resval.rpoint[1],rb1.resval.rpoint[2]);	break;
		case RTLONG:    sds_printf("\nThe result type for %s is RTLONG:  A Long - 4 Bytes. ",string1);
						sds_printf("\nIts value is %f. ",rb1.resval.rlong);	break;
//		case RTVOID:    sds_printf("\nThe result type for %s is RTVOID:  Void - ???. ",string1);
//		case RTLB:      sds_printf("\nThe result type for %s is RTLB:  A List Begin - ???. ",string1);
//		case RTLE:      sds_printf("\nThe result type for %s is RTLE:  A List End - ???. ",string1);
//		case RTDOTE:    sds_printf("\nThe result type for %s is RTDOTE:  Dot, for dotted pairs - ???. ",string1);
//		case RTT:       sds_printf("\nThe result type for %s is RTT:  True, for AutoLISP - ???. ",string1);
//		case RTNIL:     sds_printf("\nThe result type for %s is RTNIL:  Nil, for AutoLISP - ???. ",string1);
//		case RTDXF0:    sds_printf("\nThe result type for %s is RTDXF0:  The Zero Group Code for DXF Lists - ???. ",string1);
	}

	*/


/**************

Here is a general function for making entities.  Delete the stuff you don't want.

entlist=sds_buildlist(RTDXF0, "line",	// Entity type		Required
		                   8, abc,		// Layer name		Required
						   5, xxx,		// Handle name		Optional, no default
						   6, abc,		// Linetype name	Defaults to BYLAYER
						  39, 123,		// Thickness		Defaults to zero
						  62, 123,		// Color number		Defaults to BYLAYER (256)
						  67, 0 or 1,   // Model/Paper Sp.	Defaults to Model (0)
					      XX, abc,		// Others are different for each ent type
						 210, (0,0,1)   // Extrusion dir.   Defaults to (0,0,1)
						      RTNONE);  // List always ends with RTNONE or 0
sds_entmake(entlist);


***************/



/* This tests the sds_retint function, called from LISP -  "(OB_LISPRETINT)". */
int ob_func40(void){
		sds_printf("\nsds_retint(5) returns:  ");
		sds_retint(5);
    return(0);
}

/* This tests the sds_retreal function, called from LISP -  "(OB_LISPRETREAL)". */
int ob_func41(void){
		sds_printf("\nsds_retreal(12.3456789) returns:  ");
		sds_retreal(12.3456789);
    return(0);
}

/* This tests the sds_retstr function, called from LISP -  "(OB_LISPRETSTR)". */
int ob_func42(void){
		sds_printf("\nsds_retstr(\"A string\") returns:  ");
		sds_retstr("A string");
    return(0);
}

/* This tests the sds_rett function, called from LISP -  "(OB_LISPRETT)". */
int ob_func43(void){
		sds_printf("\nTesting sds_rett.");
		sds_printf("\nsds_rett() returns:  ");
		sds_rett();
    return(0);
}

/* This tests the sds_getargs function, called from LISP - "(OB_GETARGS argument)". */
int ob_func44(void){

	struct sds_resbuf *argbuf;

	sds_printf("\nTesting sds_getargs...  ");
		argbuf=sds_getargs();
		if(argbuf==NULL) return(RTERROR);
		sds_printf("\nThe type is %i.",argbuf->restype);
		switch(argbuf->restype){
			case RTREAL:    sds_printf("\nThe argument is a real. Its value is %f. ",argbuf->resval.rreal);
							break;
			case RTPOINT:   sds_printf("\nThe argument is a 2D point. Its x and y values are %f,%f. ",argbuf->resval.rpoint[0],argbuf->resval.rpoint[1]);
							break;
			case RTSHORT:   sds_printf("\nThe argument is a short. Its value is %i. ",argbuf->resval.rint);
							break;
			case RTSTR:     sds_printf("\nThe argument is a string.  The string is \"%s\".",argbuf->resval.rstring);
//							if (valueout->resval.rstring != NULL)
//								free (valueout->resval.rstring);
							break;
			case RT3DPOINT:	sds_printf("\nThe argument is a 3D point.  Its x, y, and z values are %f,%f,%f. ",argbuf->resval.rpoint[0],argbuf->resval.rpoint[1],argbuf->resval.rpoint[2]);
							break;
			case RTLONG:	sds_printf("\nThe argument is a long.  It's value is %ld. ",argbuf->resval.rlong);
							break;
			default:		sds_printf("\nThe argument is not one of the usual types.");
		}
		sds_printf("\nDone testing sds_getargs.");
    return(0);
}

/* This tests the sds_retnil function, called from LISP -  "(OB_LISPRETNIL)". */
int ob_func45(void){
		sds_printf("\nsds_retnil() returns:  ");
		sds_retnil();
    return(0);
}

/* This tests the sds_retpoint function, called from LISP -  "(OB_LISPRETPOINT)"  */
int ob_func46(void){
	sds_point point1;
	point1[0]=11.1;
	point1[1]=22.2;
	point1[2]=33.3;
		sds_printf("\nsds_retpoint(point1) returns:  ");
		sds_retpoint(point1);
    return(0);
}

/* This tests the sds_retvoid function, called from LISP -  "(OB_LISPRETVOID)". */
int ob_func47(void){
		sds_printf("\nsds_retvoid() returns:  ");
		sds_retvoid();
    return(0);
}

/* This tests the sds_retval function, called from LISP -  "(OB_LISPRETVAL)". */
int ob_func48(void){
	struct sds_resbuf *argbuf;
		sds_printf("\nTesting sds_retval...  ");
		sds_getsym("argument",&argbuf);
		if(argbuf==NULL) return(RTERROR);
		sds_printf("\nThe type is %i.",argbuf->restype);
		switch(argbuf->restype){
			case RTREAL:    sds_printf("\nThe argument is a real. Its value is %f. ",argbuf->resval.rreal);
							break;
			case RTPOINT:   sds_printf("\nThe argument is a 2D point. Its x and y values are %f,%f. ",argbuf->resval.rpoint[0],argbuf->resval.rpoint[1]);
							break;
			case RTSHORT:   sds_printf("\nThe argument is a short. Its value is %i. ",argbuf->resval.rint);
							break;
			case RTSTR:     sds_printf("\nThe argument is a string.  The string is \"%s\".",argbuf->resval.rstring);
//							if (argbuf->resval.rstring != NULL)
//								free (argbuf->resval.rstring);
							break;
			case RT3DPOINT:	sds_printf("\nThe argument is a 3D point.  Its x, y, and z values are %f,%f,%f. ",argbuf->resval.rpoint[0],argbuf->resval.rpoint[1],argbuf->resval.rpoint[2]);
							break;
			case RTLONG:	sds_printf("\nThe argument is a long.  It's value is %ld. ",argbuf->resval.rlong);
							break;
			default:		sds_printf("\nThe argument is not one of the usual types.");
		}
		sds_printf("\nsds_retval() returns:  ");
		sds_retval(argbuf);
    return(0);
}


/* This tests the sds_retname function, called from LISP -  "(OB_LISPRETNAME)"   */
int ob_func49(void){
	sds_name itsname;
	int itstype;
	sds_point point1;
	sds_point point2;
	point1[0]=1.0;
	point1[1]=1.0;
	point1[2]=0.0;
	point2[0]=3.0;
	point2[1]=3.0;
	point2[2]=0.0;

    sds_command(RTSTR,"line",RT3DPOINT,point1,RT3DPOINT,point2,RTSTR,"",RTNONE);
	sds_entlast(itsname);
	sds_printf("\nsds_retname(itsname,itstype) returns:  ");
	sds_retname(itsname,itstype);
    return(0);
}

/* This tests the sds_retlist and buildlist function, called from LISP -  "(OB_LISPRETLIST)". */
int ob_func50(void){
	struct sds_resbuf *listbuf;

		listbuf=sds_buildlist(RTREAL,3.57,RTSHORT,12,RTSTR,"Test String",RTNONE);
		sds_printf("\nsds_retlist() returns:  ");
		sds_retlist(listbuf);
    return(0);
}




/*  THAT'S ALL FOLKS!  */

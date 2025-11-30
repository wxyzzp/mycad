/* DWGCODEPAGE.CPP
 * Copyright (C) 2000 EBATECH. All rights reserved.
 */
#include "cmds.h" /*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h" /*DNT*/
/*----------------------------------------------------------------------------*/
static	short	list_dwgcodepage( void )
{
	char fs1[IC_ACADBUF], fs2[IC_ACADBUF];

	sds_textscr();
    sprintf(fs1,ResourceString(IDC_DWGCODEPAGE_8, "Hit [Enter] to continue."));
	sds_prompt(ResourceString(IDC_DWGCODEPAGE_5, "\n--- AutoCAD for Unix ---\n"));
    if (sds_getstring(0,fs1,fs2)!=RTNORM) {
		return RTCAN;
	}
    sprintf(fs1,ResourceString(IDC_DWGCODEPAGE_8, "Hit [Enter] to continue."));
	sds_prompt(ResourceString(IDC_DWGCODEPAGE_6, "\n--- AutoCAD for DOS/Mac ---\n"));
    if (sds_getstring(0,fs1,fs2)!=RTNORM) {
		return RTCAN;
	}
    sprintf(fs1,ResourceString(IDC_DWGCODEPAGE_8, "Hit [Enter] to continue."));
	sds_prompt(ResourceString(IDC_DWGCODEPAGE_7, "\n--- AutoCAD for Windows ---\n"));
    if (sds_getstring(0,fs1,fs2)!=RTNORM) {
		return RTCAN;
	}
	return RTNORM;
}

/*----------------------------------------------------------------------------*/
short	cmd_dwgcodepage
(
void
)
{
	short ret;
	char fs1[IC_ACADBUF], nCur[IC_ACADBUF];
	struct resbuf rb1, rb2;
	rb1.resval.rstring = rb2.resval.rstring = NULL;
	// Error: No Drawings
	if ((ret = cmd_iswindowopen()) != RTNORM){
		return ret;
	}
	// Display a note at once.
    sds_printf(ResourceString(IDC_DWGCODEPAGE_1, "\nThis command sets DWGCODEPAGE forcely.\nIt prevents unfortunately charactor code conversion when this drawing loads in other CAD." ));
	// Get DWGCODEPAGE as String.
    if(( ret = SDS_getvar(NULL,DB_QDWGCODEPAGE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)) != RTNORM){
		return ret;
	}
	// Compose prompt.
    sprintf(fs1,ResourceString(IDC_DWGCODEPAGE_2, "\n?=List/New code page<%s> : "),rb1.resval.rstring);
	// User Input Loop
	for(;;){
		// User Input:
        if ((ret=sds_getstring(0,fs1,nCur))!=RTNORM) {
			ret = RTCAN;	// Hit [ESC]
			goto out;
		} else if( strlen( nCur ) == 0 ){
			ret = RTNORM;	// Hit [Enter]
			goto out;
		} else if( stricmp( rb1.resval.rstring, nCur ) == 0 ){
			ret = RTNORM;	// Same Value
			goto out;
		} else if( *nCur == '?' || stricmp( nCur, "_?"/*DNT*/) == 0 ){
			ret = list_dwgcodepage();	// ?:List
			if( ret != RTNORM ){
				goto out;
			}
		} else if(( rb2.resval.rstring = new char [strlen( nCur ) + 1 ]) == NULL ){
			ret = RTERROR; // memmory error!
			goto out;
		} else {
			rb2.restype = RTSTR;
			strcpy( rb2.resval.rstring, nCur );
			if( SDS_setvar(NULL,DB_QDWGCODEPAGE,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0) != RTNORM ){
				ret = RTERROR;
				goto out;
			}
			// Set new codepage to System Variable (for Test).
			IC_FREE( rb2.resval.rstring );
			if(( ret = SDS_getvar(NULL,DB_QDWGCODEPAGE,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)) != RTNORM){
				goto out;
			}else{
				// It is not acceptable.
				if( stricmp( "UNDEFINED", rb2.resval.rstring ) == 0 ){
					IC_FREE( rb2.resval.rstring );
					sds_printf(ResourceString(IDC_DWGCODEPAGE_4, "\nWrong codepage"));
					SDS_setvar(NULL,DB_QDWGCODEPAGE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				// It is acceptable.
				}else{
					ret = RTNORM;
					goto out;
				}
			}
		}
	}
out:
	IC_FREE( rb2.resval.rstring );
	IC_FREE( rb1.resval.rstring );
	return ret;
}

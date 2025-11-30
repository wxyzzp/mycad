/* G:\ICADDEV\PRJ\LIB\CMDS\RENDER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "configure.h"
#include "IcadApp.h"
#include "LicensedComponents.h"

// Defines for cmd_InitRender & cmd_InitQRender
#define SDS_LW_INIT		"lw_initialise"/*DNT*/
#define SDS_LW_TERM		"lw_terminate"/*DNT*/
#define SDS_LW_WIRE		"lw_render_wireframe"/*DNT*/
#define SDS_LW_HIDN		"lw_render_hidden"/*DNT*/
#define SDS_LW_PREV		"lw_render_preview"/*DNT*/
#define SDS_LW_FULL		"lw_render_full"/*DNT*/
#define SDS_LW_MATL		"lw_edit_mtl"/*DNT*/
#define SDS_LW_LITE		"lw_edit_lights"/*DNT*/
#define SDS_LW_BKGD		"lw_edit_bg"/*DNT*/
#define SDS_LW_OPTS		"lw_options"/*DNT*/

// Rendering related stuff
int *(*lw_initialise)(void* data);
int *(*lw_terminate)(void* data);
int *(*lw_render_wireframe)(void* data);
int *(*lw_render_hidden)(void* data);
int *(*lw_render_preview)(void* data);
int *(*lw_render_full)(void* data);
int *(*lw_edit_mtl)(void* data);
int *(*lw_edit_lights)(void* data);
int *(*lw_edit_bg)(void* data);
int *(*lw_options)(void* data);

// for Quick Render
int *(*qr_initialise)(void* data);
int *(*qr_terminate)(void* data);
int *(*qr_render)(void* data);
int *(*qr_options)(void* data);

#ifdef BUILD_WITH_RENDERING_SUPPORT

int cmd_InitRender(int initmode) {
	static HINSTANCE        hDLL = NULL;
	static bool             isInitialised = false;

	if(!initmode) {
		if(lw_terminate) (*lw_terminate)(NULL);
		isInitialised = false;
		return(RTNORM);
	}

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if(hDLL == NULL) {
		hDLL= LoadLibrary(SDS_RENDERDLL);
		if(hDLL == NULL) {
			cmd_ErrorPrompt(CMD_ERR_BADRENDER,0,SDS_RENDERDLL);
			return(RTERROR);
		}

		// Get a pointer to the functions.
		lw_initialise = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_INIT);
		if(lw_initialise == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_INIT);

		lw_terminate = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_TERM);
		if(lw_initialise == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_TERM);

		lw_render_wireframe = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_WIRE);
		if(lw_render_wireframe == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_WIRE);

		lw_render_hidden = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_HIDN);
		if(lw_render_hidden == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_HIDN);

		lw_render_preview = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_PREV);
		if(lw_render_preview == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_PREV);

		lw_render_full = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_FULL);
		if(lw_render_full == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_FULL);

		lw_edit_mtl = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_MATL);
		if(lw_edit_mtl == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_MATL);

		lw_edit_lights = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_LITE);
		if(lw_edit_lights == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_LITE);

		lw_edit_bg = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_BKGD);
		if(lw_edit_bg == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_BKGD);

		lw_options = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_OPTS);
		if(lw_options == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_OPTS);
	}

	if(!isInitialised && lw_initialise)	{

		cmd_InitQRender(0);

		struct resbuf rb;
		SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int highlight = rb.resval.rint;
	
		rb.resval.rint = 0;
		SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

		(*lw_initialise)(NULL);
		isInitialised = true;

		rb.resval.rint = highlight;
		SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}

	return(RTNORM);	
}

short cmd_light(void)     {			// Lighting settings:  "Lighting..."

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	ExecuteUIAction( ICAD_WNDACTION_RND_LIGHT );
	return(RTNORM); 
}  

short cmd_render(void)    {			// Basic rendering:  "Render"

//Autodsys 102603{{
#ifdef CRIPPLE_TESTDRIVE
	if (CIcadApp::IsTestDrive())
	{
		MessageBox(IcadSharedGlobals::GetMainHWND(),
				   ResourceString(IDS_TESTDRIVE_RENDERING, "Rendering has been disabled.  Please register to enable the full version.",),
				   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
				   MB_OK | MB_ICONEXCLAMATION);
		return RTERROR;
	}
#endif
//Autodsys 102603}}


	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	struct resbuf rb;

	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==0) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
	}

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	SDS_SetUndo(IC_UNDO_REDRAW_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
	ExecuteUIAction( ICAD_WNDACTION_RND_RENDERPRE );
	return(RTNORM); 
} 

short cmd_rmat(void)    {			// Materials settings:  "Materials..."

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	ExecuteUIAction( ICAD_WNDACTION_RND_EDITMAT );
	return(RTNORM); 
} 

short cmd_renderfull(void)    {		// Killer rendering:  "Full Render"	 
//Autodsys 102603{{
#ifdef CRIPPLE_TESTDRIVE
	if (CIcadApp::IsTestDrive())
	{
		MessageBox(IcadSharedGlobals::GetMainHWND(),
				   ResourceString(IDS_TESTDRIVE_RENDERING, "Rendering has been disabled.  Please register to enable the full version.",),
				   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
				   MB_OK | MB_ICONEXCLAMATION);
		return RTERROR;
	}
#endif
//Autodsys 102603}}

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	struct resbuf rb;

	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==0) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
	}

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	SDS_SetUndo(IC_UNDO_REDRAW_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
	ExecuteUIAction( ICAD_WNDACTION_RND_RENDERFL );
	return(RTNORM); 
} 

short cmd_renderwf(void)    {		// Wireframe view (not in menus/toolbars)

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	struct resbuf rb;

	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==0) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
	}

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	SDS_SetUndo(IC_UNDO_REDRAW_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
	ExecuteUIAction( ICAD_WNDACTION_RND_RENDERWF );
	return(RTNORM); 
} 

short cmd_renderhl(void)    {		// Hidden line removal (not in menus/toolbars)

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	struct resbuf rb;

	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==0) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
	}

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	SDS_SetUndo(IC_UNDO_REDRAW_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
	ExecuteUIAction( ICAD_WNDACTION_RND_RENDERHD );
	return(RTNORM); 
}
 
short cmd_editgb(void)    {			// Background settings:  "Backgrounds..."

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	ExecuteUIAction( ICAD_WNDACTION_RND_EDITBG );
	return(RTNORM); 
} 

short cmd_roptions(void)    {		// Render settings:  "Render Settings..."

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	if(RTERROR==cmd_InitRender(1)) return(RTERROR);
	ExecuteUIAction( ICAD_WNDACTION_RND_ROPTIONS );
	return(RTNORM); 
} 

#else

int cmd_InitRender(int initmode) { return RTERROR; }
short cmd_light(void)		{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_render(void)		{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_rmat(void)		{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_renderfull(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_renderwf(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_renderhl(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_editgb(void)		{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_roptions(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  

#endif //BUILD_WITH_RENDERING_SUPPORT



// ****************************************************
//    Quick Render functions
// ****************************************************

#ifdef BUILD_WITH_QRENDERING_SUPPORT

int cmd_InitQRender(int initmode) {
	static HINSTANCE        hDLL = NULL;
	static bool             isInitialised = false;

	if(!initmode) {
		if(qr_terminate) (*qr_terminate)(NULL);
		isInitialised = false;
		return(RTNORM);
	}

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::QRENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if(hDLL == NULL) {
		hDLL= LoadLibrary(SDS_QRENDERDLL);
		if(hDLL == NULL) {
			cmd_ErrorPrompt(CMD_ERR_BADRENDER,0,SDS_QRENDERDLL);
			return(RTERROR);
		}

		// Get a pointer to the functions.
		qr_initialise = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_INIT);
		if(qr_initialise == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_INIT);

		qr_terminate = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_TERM);
		if(qr_initialise == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_TERM);

		qr_render = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_FULL);
		if(qr_render == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_FULL);

		qr_options = (int *(*)(void* data))GetProcAddress(hDLL, SDS_LW_OPTS);
		if(qr_options == NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTLOADREN,0,SDS_LW_OPTS);
	}

	if(!isInitialised && qr_initialise)	{

		cmd_InitRender(0);

		struct resbuf rb;
		SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int highlight = rb.resval.rint;
	
		rb.resval.rint = 0;
		SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

		(*qr_initialise)(NULL);
		isInitialised = true;

		rb.resval.rint = highlight;
		SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}

	return(RTNORM);	
}

short cmd_qrender(void)    {

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::QRENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	struct resbuf rb;

	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==0) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
	}

	if(RTERROR==cmd_InitQRender(1)) return(RTERROR);
	SDS_SetUndo(IC_UNDO_REDRAW_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
	ExecuteUIAction( ICAD_WNDACTION_RND_QRENDER );
	return(RTNORM); 
} 

short cmd_qroptions(void)    {

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::QRENDER))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		SDS_FreeEventList(0);
		return(RTNORM);
	}

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	if(RTERROR==cmd_InitQRender(1)) return(RTERROR);
	ExecuteUIAction( ICAD_WNDACTION_RND_QROPTIONS );
	return(RTNORM); 
} 

#else

int cmd_InitQRender(int initmode) { return RTERROR; }
short cmd_qrender(void)		{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_qroptions(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }

#endif //BUILD_WITH_QRENDERING_SUPPORT


//short cmd_background(void){ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_scene(void)     { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_vlconv(void)    { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); } 
short cmd_replay(void)    { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_rendscr(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_lsedit(void)    { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_lslib(void)     { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_lsnew(void)     { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_setuv(void)     { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_transparency(void){ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }  
short cmd_fog(void)       { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); } 

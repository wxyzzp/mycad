// File  : <DevDir>\source\prj\lib\cmds\acisInOut.cpp
// Author: Dmitry Gavrilov

#pragma warning (disable : 4786)	// for calm using of std containers templates

#include "icad.h"
#include "IcadDoc.h"
#include "cmds.h"
#include "icadapi.h"
#include "Modeler.h"
#include "DbAcisEntity.h"
#include "commandqueue.h" /*DNT*/
#include "CmdQueryTools.h"

using namespace std;

short cmd_acisin()
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    if(cmd_iswindowopen() == RTERROR)
		return RTERROR;

    char fs1[IC_ACADBUF];
    *fs1 = 0;

    resbuf rb, fileName;
    if (SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
        return(RTERROR);
	
    int filedia = rb.resval.rint;

    if( filedia && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore) {
        dia: ;
        // EBATECH(CNBR) 2002/4/30 disable type button( option +2)
	    if (SDS_GetFiled(ResourceString(ACIS_MSG_OPEN_TITLE, "Open a SAT file"), NULL, "sat"/*DNT*/, 2, &fileName, NULL, NULL) != RTNORM ||
		    fileName.restype != RTSTR || !fileName.resval.rstring || !*fileName.resval.rstring)
		    return RTERROR;
    }
    else {
        if (RTNORM!=sds_getstring(1,ResourceString(IDC_CMDS9_ACIS_SAT__381, "\nACIS SAT file to load: " ),fs1)) 
            return(RTCAN);

        if (*fs1=='~'/*DNT*/) 
            goto dia;

        if (!strchr(fs1,'.'/*DNT*/)) 
            ic_setext(fs1,"sat"/*DNT*/);

	    ChangeSlashToBackslashAndRemovePipe(fs1);
        fileName.resval.rstring = fs1;
    }

	xstd<CDbAcisEntityData>::vector objs;
    RT rc = CModeler::get()->loadSatFile(fileName.resval.rstring, getCurrColorIndex(), SDS_CURPALLET, objs);
    IC_FREE(fileName.resval.rstring);
    
	if (!rc)
    {
		sds_prompt(ResourceString(ACIS_MSG_LOAD_SAT_FAILED, "ERROR: Failed to load the file."));
        return RTERROR;
    }

	sds_name ename;
	for (int i = 0, n = objs.size(); i < n; i++)
		create(objs[i], ename);

    return RTNORM;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Process ACISOUT command.
 * Returns:	SDS RTxxx return codes.
 ********************************************************************************/
short cmd_acisout()
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    if(cmd_iswindowopen() == RTERROR)
		return RTERROR;

    do
    {
        resbuf filter;
	    filter.restype = 0;
	    filter.resval.rstring = "REGION,3DSOLID,BODY"/*DNT*/;
	    filter.rbnext = 0;

        sds_name ss1;
        int ret;
	    if ((ret = sds_pmtssget("\nSelect ACIS entities to save: ",
	                     (GetActiveCommandQueue()->IsEmpty() && lsp_cmdhasmore==0) ? NULL : (LPCTSTR)""/*DNT*/,
		                  NULL,NULL,&filter,ss1,1))==RTERROR) 
        {
		    cmd_ErrorPrompt(CMD_ERR_NOENT,0); 
		    break;
	    }
	    else if (ret == RTCAN) 
		    break;

        sds_name ename;
        resbuf app;
        app.restype=RTSTR; app.resval.rstring="*"/*DNT*/ ; app.rbnext=NULL;
        std::vector<CDbAcisEntityData > objs;
	    for (long ssct=0l; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) 
	    {
            sds_resbuf* elist = sds_entgetx(ename, &app);
            do
            {
                if (ic_assoc(elist, 0))
                    break;
                if (sds_wcmatch(ic_rbassoc->resval.rstring, "BODY,3DSOLID,REGION") != RTNORM)
                    break;
                if (ic_assoc(elist, 1))
                    break;

				CDbAcisEntity* dbitem = reinterpret_cast<CDbAcisEntity*>(ename[0]);

                objs.push_back(dbitem->getData());
            }
            while (false);

            sds_relrb(elist);
        }
        if (!objs.size())
            break;

        resbuf fileName;
        int opt = 3; // EBATECH(CNBR) 2002/4/30 disable type button( option +2)
	    if (SDS_GetFiled("Create a SAT file", NULL, "sat"/*DNT*/, opt, &fileName, NULL, NULL) != RTNORM ||
		    fileName.restype != RTSTR || !fileName.resval.rstring || !*fileName.resval.rstring)
		    return RTERROR;
        
        resbuf rb;
		SDS_getvar(NULL,DB_QACISOUTVER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int version = rb.resval.rint;

        RT rc = CModeler::get()->saveSatFile(fileName.resval.rstring, objs, SDS_CURPALLET, version);

        IC_FREE(fileName.resval.rstring);

        // unhighlight selected entities
        //
	    for (ssct=0l; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) 
	        sds_redraw(ename, IC_REDRAW_UNHILITE);

        if (!rc)
        {
		    sds_prompt("ERROR: Failed to save the file.");
            return RTERROR;
        }
    }
    while (false);

    return RTNORM;
}

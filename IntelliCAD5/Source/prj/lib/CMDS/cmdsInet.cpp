#include "cmds.h"
#include "cmdsInet.h"
#include "cmdsGroup.h"
#include "IcadApi.h"
#include "IcadDoc.h"
#include "Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT OpenWebDoc(LPCTSTR pURL);

short cmd_browser()
{ 
	if(cmd_istransparent() == RTERROR)
		return RTERROR;

	return cmd_url();
}  

short cmd_url()
{
	short status = RTNORM;
	char url[IC_ACADBUF] = "";
	char label[IC_ACADBUF] = "";
	char ref[IC_ACADBUF] = "";
	long l;
	sds_name entity;
	sds_sslength(SDS_CURDOC->m_pGripSelection, &l);
	if(l != 1 || 
		sds_ssname(SDS_CURDOC->m_pGripSelection, 0, entity) != RTNORM ||
		cmd_getEntityHL(entity, url, NULL, label) == 0)
	{
		struct sds_resbuf inetLoc;
		SDS_getvar(NULL, DB_QINETLOCATION, &inetLoc, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(inetLoc.resval.rstring)
		{
			strcpy(url, inetLoc.resval.rstring);
			IC_FREE(inetLoc.resval.rstring);
		}
		if(url[0] == '\0')
			strcpy(url, ResourceString(IDC_WEB_VISIO,"http://www.intellicad.org"));
		sprintf(label, ResourceString(IDC_URL_ENTERURL, "\nEnter URL<%s>: "), url);
		status = sds_getstring(0, label, ref);
		if(status != RTNORM && status != RTNONE)
			return status;
		if(ref[0] != '\0')
			strcpy(url, ref);
		label[0] = '\0';
	}
	else
	{
		if(cmd_isRelativeHyperlink(url))
		{
			char hlbase[IC_ACADBUF];
			cmd_getHyperlinkBase(hlbase);
			sprintf(ref, "%s/%s", hlbase, url); 
			strcpy(url, ref);
		}
	}

	if(label[0] == '\0')
	{
		strcpy(ref, url);
		ShellExecute(0, "open", (LPCTSTR)ref, NULL, NULL, SW_NORMAL);
	}
	else
	{
		sprintf(ref, "%s#%s", url, label);
		OpenWebDoc((LPCTSTR)ref);
	}
	sds_printf(" %s", ref);
	return status;
}

short cmd_projectCenter()
{
	if(cmd_istransparent() == RTERROR)
		return RTERROR;

	if (!SDS_OurWinExec(NULL, "ProjectCenterUpload.exe"))
		sds_printf(ResourceString(IDC_PROJECTCENTER_UPLOAD_ERROR, "\nError running '%s'."), "ProjectCenterUpload.exe");
		
	return RTNORM;
}

short cmd_hyperlink()
{
#ifndef _SUPPORT_HYPERLINKS_
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); 
	SDS_FreeEventList(0);
	return RTNORM;
#endif
	if(cmd_iswindowopen() == RTERROR)
		return RTERROR;

	if(cmd_istransparent() == RTERROR)
		return RTERROR;

	struct resbuf cd;
	sds_getvar("CMDDIA", &cd);
	if(cd.resval.rint == 0)
		return cmd_hyperlink_();

	ExecuteUIAction(ICAD_WNDACTION_HYPERLINK);
	return RTNORM;
}

short cmd_hyperlink_()
{
#ifndef _SUPPORT_HYPERLINKS_
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); 
	SDS_FreeEventList(0);
	return RTNORM;
#endif

	char url[IC_ACADBUF] = "";
	char desc[IC_ACADBUF] = "";
	char location[IC_ACADBUF] = "";
	char answer[IC_ACADBUF] = "";
	char input[IC_ACADBUF] = "";
	int result = RTNORM;
	bool bRemove = false;
	long i;
	sds_name entities;
	sds_name entity;

	while(true)
	{
		if(sds_initget(0, ResourceString(IDC_HYPERLINK_OPTION, "Remove Insert")) != RTNORM)
			return RTERROR;

		result = sds_getkword(ResourceString(IDC_HYPERLINK_OPTIONPROMPT, "\nEnter an option [Remove/Insert] <Insert>: "), answer);
		if(result != RTNORM)
		{
			if(result == RTNONE)
				strcpy(answer, ResourceString(IDC_HYPERLINK_OPTIONINSERT, "Insert"));
			else
				break;
		}

		if(stricmp(answer, ResourceString(IDC_HYPERLINK_OPTIONINSERT, "Insert")))
			bRemove = true;

		// TODO: addition of URL to area

		result = sds_pmtssget("", NULL, NULL, NULL, NULL, entities, 1);
		if(result != RTNORM)
			break;

		sds_sslength(entities, &i);
		if(i == 0)
			break;

		if(bRemove)
		{
			while(--i >= 0)
			{
				sds_ssname(entities, i, entity);
				cmd_removeEntityHL(entity);
			}
			break;
		}

		cmd_getEntitiesHL(entities, url, desc, location);

		sprintf(answer, ResourceString(IDC_HYPERLINK_ENTERURL, "\nEnter URL <%s> :"), url);
		result = sds_getstring(1, answer, input);
		if(result == RTNORM && input[0] != 0)
			strcpy(url, input);
		else
			if(result == RTERROR || result == RTCAN)
				break;

		sprintf(answer, ResourceString(IDC_HYPERLINK_ENTERLOC, "\nEnter named location <%s> :"), location);
		result = sds_getstring(1, answer, input);
		if(result == RTNORM && input[0] != 0)
			strcpy(location, input);
		else
			if(result == RTERROR || result == RTCAN)
				break;

		sprintf(answer, ResourceString(IDC_HYPERLINK_ENTERDESC, "\nEnter description <%s> :"), desc);
		result = sds_getstring(1, answer, input);
		if(result == RTNORM && input[0] != 0)
			strcpy(desc, input);
		else
			if(result == RTERROR || result == RTCAN)
				break;

		while(--i >= 0)
		{
			sds_ssname(entities, i, entity);
			cmd_modifyEntityHL(entity, url, desc, location);
		}
		break;
	}

	cmd_redrawSS(entities, IC_REDRAW_UNHILITE);
	sds_ssfree(entities);
	return RTNORM;
}

short cmd_hyperlinkoptions()
{
#ifndef _SUPPORT_HYPERLINKS_
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); 
	SDS_FreeEventList(0);
	return RTNORM;
#endif

	char answer[IC_ACADBUF];
	char prompt[IC_ACADBUF];
	if(sds_initget(0, ResourceString(IDC_HYPERLINK_OPTIONYESNO, "Yes No")) != RTNORM)
		return RTERROR;
	
	sprintf(prompt, ResourceString(IDC_HYPERLINK_DISPLAYMENU, "\nDisplay hyperlink cursor and shortcut menu? [Yes/No] <%s>:"), 
		(bool)UserPreference::s_bEnableHyperlinkMenu ? ResourceString(IDC_HYPERLINK_OPTIONYES, "Yes") : ResourceString(IDC_HYPERLINK_OPTIONNO, "No"));
	int result = sds_getkword(prompt, answer);
	if(result != RTNORM)
	{
		if(result == RTNONE)
			strcpy(answer, (bool)UserPreference::s_bEnableHyperlinkMenu ? ResourceString(IDC_HYPERLINK_OPTIONYES, "Yes") : ResourceString(IDC_HYPERLINK_OPTIONNO, "No"));
		else
			return RTNORM;
	}

	if(stricmp(answer, ResourceString(IDC_HYPERLINK_OPTIONYES, "Yes")) == 0)
	{
		UserPreference::s_bEnableHyperlinkMenu = true;
		if(sds_initget(0, ResourceString(IDC_HYPERLINK_OPTIONYESNO, "Yes No")) != RTNORM)
			return RTERROR;
		
		sprintf(prompt, ResourceString(IDC_HYPERLINK_DISPLAYTOOLTIPS, "\nDisplay hyperlink tooltips? [Yes/No] <%s>:"),
			(bool)UserPreference::s_bEnableHyperlinkTooltip ? ResourceString(IDC_HYPERLINK_OPTIONYES, "Yes") : ResourceString(IDC_HYPERLINK_OPTIONNO, "No"));
		int result = sds_getkword(prompt, answer);
		if(result != RTNORM)
		{
			if(result == RTNONE)
				strcpy(answer, (bool)UserPreference::s_bEnableHyperlinkTooltip ? ResourceString(IDC_HYPERLINK_OPTIONYES, "Yes") : ResourceString(IDC_HYPERLINK_OPTIONNO, "No"));
			else
				return RTNORM;
		}

		if(stricmp(answer, ResourceString(IDC_HYPERLINK_OPTIONYES, "Yes")) == 0)
			UserPreference::s_bEnableHyperlinkTooltip = true;
		else
			UserPreference::s_bEnableHyperlinkTooltip = false;
	}
	else
	{
		UserPreference::s_bEnableHyperlinkMenu = false;
		UserPreference::s_bEnableHyperlinkTooltip = false;
	}

	return RTNORM;
}

int cmd_getEntitiesHL(sds_name entities, char* pURL, char* pDescription, char* pLocation)
{
	long l;
	sds_sslength(entities, &l);
	char curURL[IC_ACADBUF];
	char curDesc[IC_ACADBUF];
	char curLoc[IC_ACADBUF];
	sds_name entity;
	if(l == 0)
		return 0;

	long i;
	for(i = 0; i < l; ++i)
	{
		sds_ssname(entities, i, entity);
		if(cmd_getEntityHL(entity, pURL, pDescription, pLocation) != 0)
			break;
	}
	if(i == l)
		return 0;
	
	for(++i; i < l; ++i)
	{
		sds_ssname(entities, i, entity);
		if(!cmd_getEntityHL(entity, curURL, curDesc, curLoc))
		{
			strcpy(curURL, "");
			strcpy(curDesc, "");
			strcpy(curLoc, "");
		}
		if(pURL && strcmp(pURL, curURL) != 0)
			strcpy(pURL, "Varies");
		if(pDescription && strcmp(pDescription, curDesc) != 0)
			strcpy(pDescription, "Varies");
		if(pLocation && strcmp(pLocation, curLoc) != 0)
			strcpy(pLocation, "Varies");
	}
	return 1;
}

int cmd_getEntityHL(sds_name entity, char* pURL, char* pDescription, char* pLocation)
{
	if(pURL != NULL)
		pURL[0] = 0;
	if(pDescription != NULL)
		pDescription[0] = 0;
	if(pLocation != NULL)
		pLocation[0] = 0;

	if(!entity || !entity[0] || !entity[1])
		return 0;

	struct resbuf* pApp = sds_buildlist(RTSTR, "PE_URL", RTNONE);
	/*DG - 7.4.2003*/// Get only EED, not the whole entity resbuf list: performance.
#if 1
	struct resbuf* pEntity = reinterpret_cast<db_handitem*>(entity[0])->get_eed(pApp, NULL);
#else
	struct resbuf* pEntity = sds_entgetx(entity, pApp);
#endif
	sds_relrb(pApp);
	pApp = pEntity;
	while(pEntity != NULL && pEntity->restype != 1000)
		pEntity = pEntity->rbnext;
	if(pEntity == NULL)
	{
		sds_relrb(pApp);
		return 0;
	}

	if(pURL != NULL)
		strcpy(pURL, pEntity->resval.rstring);
	pEntity = pEntity->rbnext;
	if(pEntity != NULL && pEntity->restype == 1002 && pEntity->resval.rstring[0] == '{')
	{
		pEntity = pEntity->rbnext;
		ASSERT(pEntity != NULL && pEntity->restype == 1000);

		if(pDescription != NULL)
			strcpy(pDescription, pEntity->resval.rstring);
		pEntity = pEntity->rbnext;
		if(pEntity != NULL && pEntity->restype == 1000 && pLocation != NULL)
			strcpy(pLocation, pEntity->resval.rstring);
	}
	sds_relrb(pApp);
	return 1;
}

int cmd_modifyEntityHL(sds_name entity, const char* pURL, const char* pDescription, const char* pLocation)
{
	struct resbuf* pApp = sds_buildlist(RTSTR, "PE_URL", RTNONE);
	struct resbuf* pEntity = sds_entgetx(entity, pApp);
	sds_relrb(pApp);

	struct resbuf* pCurPair = pEntity;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 1000)
		pCurPair = pCurPair->rbnext;

	sds_regapp("PE_URL");
	if(pCurPair->rbnext == NULL)
	{
		if(pURL == NULL)
		{
			sds_relrb(pEntity);
			return 0;
		}
		if(pLocation != NULL)
			pCurPair->rbnext = sds_buildlist(-3, 1001, "PE_URL", 
				1000, pURL, 
				1002, "{", 
				1000, pDescription != NULL ? pDescription : "",
				1000, pLocation,
				1002, "}", RTNONE);
		else
			if(pDescription != NULL)
				pCurPair->rbnext = sds_buildlist(-3, 1001, "PE_URL", 
					1000, pURL, 
					1002, "{", 
					1000, pDescription,
					1002, "}", RTNONE);
		sds_entmod(pEntity);
		return 1;
	}

	pApp = pCurPair->rbnext;
	if(pURL != NULL)
	{
		pCurPair->rbnext = sds_buildlist(1000, pURL, RTNONE);
		pCurPair->rbnext->rbnext = pApp->rbnext;
		pApp->rbnext = NULL;
		sds_relrb(pApp);
	}
	pCurPair = pCurPair->rbnext;
	if(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 1002 && pCurPair->rbnext->resval.rstring[0] == '{')
	{
		pCurPair = pCurPair->rbnext;
		ASSERT(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 1000);

		pApp = pCurPair->rbnext;
		if(pDescription != NULL)
		{
			pCurPair->rbnext = sds_buildlist(1000, pDescription, RTNONE);
			pCurPair->rbnext->rbnext = pApp->rbnext;
			pApp->rbnext = NULL;
			sds_relrb(pApp);
		}

		if(pLocation != NULL)
		{
			pCurPair = pCurPair->rbnext;

			pApp = pCurPair->rbnext;
			pCurPair->rbnext = sds_buildlist(1000, pLocation, RTNONE);
			if(pApp->restype == 1000)
			{
				pCurPair->rbnext->rbnext = pApp->rbnext;
				pApp->rbnext = NULL;
				sds_relrb(pApp);
			}
			else
				pCurPair->rbnext->rbnext = pApp;
		}
	}
	else
	{
		pApp = pCurPair->rbnext;
		if(pLocation != NULL)
		{
			pCurPair->rbnext = sds_buildlist(1002, "{", 
				1000, pDescription != NULL ? pDescription : "",
				1000, pLocation,
				1002, "}", RTNONE);
			pCurPair->rbnext->rbnext->rbnext->rbnext->rbnext = pApp;
		}
		else
		{
			if(pDescription != NULL)
				pCurPair->rbnext = sds_buildlist(1002, "{", 
					1000, pDescription,
					1002, "}", RTNONE);
			pCurPair->rbnext->rbnext->rbnext->rbnext = pApp;
		}
	}

	sds_entmod(pEntity);
	sds_relrb(pEntity);
	return 1;
}

int cmd_removeEntityHL(sds_name entity)
{
	struct resbuf* pApp = sds_buildlist(RTSTR, "PE_URL", RTNONE);
	if(pApp == NULL)
		return 0;

	struct resbuf* pEntity = sds_entgetx(entity, pApp);
	sds_relrb(pApp);
	if(pEntity == NULL)
		return 0;

	struct resbuf* pEED = NULL;
	struct resbuf* pCurPair = pEntity;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != -3)
		pCurPair = pCurPair->rbnext;
	if(pCurPair->rbnext == NULL)
	{
		sds_relrb(pEntity);
		return 1;
	}
	pCurPair = pCurPair->rbnext;
	pCurPair = pCurPair->rbnext;
	pApp = pCurPair;
	pCurPair = pCurPair->rbnext;
	pEED = pCurPair;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype >= 1000)
		pCurPair = pCurPair->rbnext;

	pApp->rbnext = pCurPair->rbnext;
	pCurPair->rbnext = NULL;
	sds_relrb(pEED);

	if(sds_entmod(pEntity) != RTNORM)
	{
		sds_relrb(pEntity);
		return 0;
	}
	sds_relrb(pEntity);
	return 1;
}

int cmd_isRelativeHyperlink(const char* pURL)
{
	if(strcmp(pURL, "Varies") == 0)
		return 0;
	if(PathIsURL(pURL))
		return 0;
	if(PathIsRelative(pURL))
		return 1;
	return 0;
}

int cmd_getHyperlinkBase(char* pBase)
{
	struct resbuf hlbase;
	SDS_getvar(NULL, DB_QHYPERLINKBASE, &hlbase, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(hlbase.resval.rstring[0] == '\0')
		GetCurrentDirectory(IC_ACADBUF, pBase);
	else
		strcpy(pBase, hlbase.resval.rstring);
	IC_FREE(hlbase.resval.rstring);
	return 1;
}

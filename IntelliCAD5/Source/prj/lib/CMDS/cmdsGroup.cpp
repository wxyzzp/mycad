#include "cmds.h"
#include "cmdsGroup.h"
#include "IcadApi.h"
#include "objects.h"
#include "DbGroup.h"

/*
	DP: Sample for command realization from old times.
	char			szPrompt[IC_ACADBUF],	// For making up complicated prompt strings with %s
					szKeyWord[IC_ACADBUF];	// Keyword from command line
	int				nRet=RTNORM;			// This function's return value.
    long			lErrorLine=0L;			// Gives the line number if there is an error.


	for (;;) {
		if (){
			if (sds_initget(0,ResourceString(IDC_CMDS3__68, "" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_get(ResourceString(IDC_CMDS3__N________599, "\n/<>: " )))==RTCAN || nRet==RTERROR) goto bail;
		}else{
			if (sds_initget(0,ResourceString(IDC_CMDS3__68, "" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_get(ResourceString(IDC_CMDS3__N________599, "\n/<>: " )))==RTCAN || nRet==RTERROR) goto bail;
		}
		if (nRet==RTNORM) break;
		if (nRet==RTERROR)
			goto bail;
		if (nRet==RTKWORD) {
            nRet=sds_getinput(szKeyWord);
			if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0) || (stricmp(szKeyWord,ResourceString(IDC_CMDS3_JUSTIFICATION_442, "Justification" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			}

end:
    // put end stuff here if needed.

bail:

    if (lErrorLine==0L) return(RTNORM);
    if (lErrorLine==(-1L)) return(RTCAN);
    if (lErrorLine<(-1L)) CMD_INTERNAL_MSG(-lErrorLine);
    return(nRet);
}
*/
#define _SUPPORT_GROUPS_
short cmd_group_()
{
#ifndef _SUPPORT_GROUPS_
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
	SDS_FreeEventList(0);
	return RTNORM;
#endif

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	int result = RTNORM;
	char answer[IC_ACADBUF];
	char name[IC_ACADBUF];
	sds_name entities;
	sds_name group;

	db_drawing* pDrawing = SDS_CURDWG;
	db_dictionary* pDictGroup = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(), "ACAD_GROUP", 0);
	ASSERT(pDictGroup != NULL);

	if(sds_initget(0, ResourceString(IDC_GROUP_OPTIONS, "? Order Add Remove Un-group reName Selectable Create")) != RTNORM)
		return RTERROR;

	result = sds_getkword(ResourceString(IDC_GROUP_OPTIONSPROMPT, "\nEnter group option [? to list/Order/Add/Remove/Un-group/REName/Selectable/Create] <Create>: "), answer);
	if(result != RTNORM)
	{
		if(result == RTNONE)
			strcpy(answer, ResourceString(IDC_GROUP_OPTIONCREATE, "Create"));
		else
			return RTNORM;
	}

	if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONLIST, "?")) == 0)
	{
		cmd_listGroups();
		return RTNORM;
	}

	// loop for group name
	while(true)
	{
		// 2003/4/12 Ebatech(cnbr) long name accept
		if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONCREATE, "Create")) == 0)
			result = sds_getstring(1, ResourceString(IDC_GROUP_ENTERNAME2, "\nEnter a group name or ? to list [* to create unnamed]: "), name);
		else
			result = sds_getstring(1, ResourceString(IDC_GROUP_ENTERNAME, "\nEnter a group name or ? to list: "), name);
		
		if(result != RTNORM)
			return RTNORM;

		ic_trim(name, "be");	// 2003/4/12 Ebatech(cnbr) long name accept

		if(stricmp(name, ResourceString(IDC_GROUP_OPTIONLIST, "?")) == 0)
			cmd_listGroups();
		else
			break;
	}

	if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONCREATE, "Create")) == 0)
	{
		if(strcmp(name, "*"/*DNT*/) != 0)
		{
			if(!ic_isvalidname(name))
			{
				sds_printf("\n%s", ResourceString(IDC_GROUP_NOTVALIDNAME, "This is not valid name for group."));
				return RTNORM;
			}
			if(cmd_findGroup(name))
			{
				sds_printf("\n%s", ResourceString(IDC_GROUP_ALREADYEXIST, "Group with such name already exist."));
				return RTNORM;
			}
		}
		
		if(sds_getstring(1, ResourceString(IDC_GROUP_ENTERDESC, "\nEnter description: "), answer) != RTNORM)
			return RTNORM;
		
		cmd_createGroup(name, answer, 1,
			strcmp(name, "*" /*DNT*/) == 0 ? 1 : 0,
			group);
		sds_pmtssget("", NULL, NULL, NULL, NULL, entities, 1);
		cmd_addSSToGroup(entities, group);
	}
	else
	{
		if(!cmd_findGroup(name, group))
		{
			sds_printf("\n%s", ResourceString(IDC_GROUP_NOTFOUND, "Group not found"));
			return RTNORM;
		}
		if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONORDER, "Order")) == 0)
		{
			if(sds_initget(0, ResourceString(IDC_GROUP_OPTIONYESNO, "Yes No")) != RTNORM)
				return RTERROR;
			
			result = sds_getkword(ResourceString(IDC_GROUP_REVERSEORDER, "\nReverse order [Yes/No] <Yes>: "), answer);
			if(result != RTNORM)
			{
				if(result == RTNONE)
					strcpy(answer, ResourceString(IDC_GROUP_OPTIONYES, "Yes"));
				else
					return RTNORM;
			}
			
			if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONNO, "No")) == 0)
			{
				int fromPos, toPos, count;
				int n = ((CDbGroup*)group[0])->nEntities();
				
				sprintf(answer, "\n%s (0-%d):", ResourceString(IDC_GROUP_REMOVEFROM, "Remove from position"), n - 1);
				if(sds_getint(answer, &fromPos) != RTNORM)
					return RTNORM;
				sprintf(answer, "\n%s (0-%d):", ResourceString(IDC_GROUP_PLACETO, "Place to Position"), n - 1);
				if(sds_getint(answer, &toPos) != RTNORM)
					return RTNORM;
				sprintf(answer, "\n%s (1-%d):", ResourceString(IDC_GROUP_NUMBER, "Number of entities"), n);
				if(sds_getint(answer, &count) != RTNORM)
					return RTNORM;
				
				if(!cmd_reorderGroupEntities(group, fromPos, toPos, count))
					sds_printf("\n%s", ResourceString(IDC_GROUP_CANTREORDER, "Can't reorder entities in group"));
			}
			else
				if(!cmd_reverseGroupOrder(group))
					sds_printf("\n%s", ResourceString(IDC_GROUP_CANTREORDER, "Can't reorder entities in group"));
		}
		else
		{
			if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONADD, "Add")) == 0)
			{
				cmd_getSSFromGroup(entities, group);
				cmd_redrawSS(entities, IC_REDRAW_HILITE);
				sds_ssfree(entities);
				
				sds_pmtssget("", NULL, NULL, NULL, NULL, entities, 1);
				cmd_addSSToGroup(entities, group);
				
				sds_ssfree(entities);
				cmd_getSSFromGroup(entities, group);
				cmd_redrawSS(entities, IC_REDRAW_UNHILITE);
			}
			else
			{
				if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONREMOVE, "Remove")) == 0)
				{
					cmd_getSSFromGroup(entities, group);
					cmd_redrawSS(entities, IC_REDRAW_HILITE);
					sds_ssfree(entities);
					
					// PICKSTYLE should be cleared before
					int pickstyle;
					struct resbuf ps;
					//sds_getvar("PICKSTYLE", &ps);
					SDS_getvar(NULL, DB_QPICKSTYLE, &ps, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
					
					pickstyle = ps.resval.rint;
					ps.resval.rint = 0;
					//sds_setvar("PICKSTYLE", &ps);
					SDS_setvar(NULL, DB_QPICKSTYLE, &ps, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
					
					sds_pmtssget("", NULL, NULL, NULL, NULL, entities, 1);
					cmd_removeSSFromGroup(entities, group);
					
					sds_ssfree(entities);
					cmd_getSSFromGroup(entities, group);
					cmd_redrawSS(entities, IC_REDRAW_UNHILITE);
					
					// restore PICKSTYLE
					ps.resval.rint = pickstyle;
					//sds_setvar("PICKSTYLE", &ps);
					SDS_setvar(NULL, DB_QPICKSTYLE, &ps, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
				}
				else
				{
					if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONUNGROUP, "Un-group")) == 0)
					{
						sds_entdel(group);
					}
					else
					{
						if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONRENAME, "reName")) == 0)
						{
							if(sds_getstring(1, ResourceString(IDC_GROUP_ENTERNEWNAME, "\nEnter new name: "), answer) != RTNORM)
								return RTNORM;

							ic_trim(name, "be");	// 2003/4/12 Ebatech(cnbr) long name accept

							if(!ic_isvalidname(answer))
							{
								sds_printf("\n%s", ResourceString(IDC_GROUP_NOTVALIDNAME, "This is not valid name for group."));
								return RTNORM;
							}
							cmd_modifyGroup(answer, NULL, -1, group);
						}
						else
						{
							if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONSELECTABLE, "Selectable")) == 0)
							{
								if(sds_initget(0, ResourceString(IDC_GROUP_OPTIONYESNO, "Yes No")) != RTNORM)
									return RTERROR;
								
								sprintf(name, "\n%s <%s>: ",
									ResourceString(IDC_GROUP_CHANGEDESC, "Change group selection flag [Yes/No]"),
									((CDbGroup*)group[0])->isSelectable() ? ResourceString(IDC_GROUP_OPTIONYES, "Yes") : ResourceString(IDC_GROUP_OPTIONNO, "No"));
								result = sds_getkword(name, answer);
								if(result != RTNORM)
								{
									if(result == RTNONE)
										strcpy(name, ResourceString(IDC_GROUP_OPTIONYES, "Yes"));
									else
										return RTNORM;
								}
								
								if(stricmp(answer, ResourceString(IDC_GROUP_OPTIONYES, "Yes")) == 0)
									cmd_modifyGroup(NULL, NULL, 1, group);
								else
									cmd_modifyGroup(NULL, NULL, 0, group);
							}
							else
							{
								ASSERT(FALSE);
								return RTERROR;
							}
						} // rename
					} // ungroup
				} // remove
			} // add
		} // order
	} // create
	sds_ssfree(entities);

	return RTNORM;
}

short cmd_group()
{
#ifndef _SUPPORT_GROUPS_
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
	SDS_FreeEventList(0);

//	Reinstate this first commented part if you want to play with the dialog box.
//	sds_printf(ResourceString(IDC_CMDS3__NTHE_GROUP_FUNC_629, "\nThe Group function is under construction.  It displays a dialog box, but has no effect." ));

#else
	struct resbuf cd;
	sds_getvar("CMDDIA", &cd);
	if(cd.resval.rint == 0)
		return cmd_group_();

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction(ICAD_WNDACTION_GROUP);
#endif
    return(RTNORM);
}

int cmd_listGroups()
{
    sds_textscr();

	db_drawing* pDrawing = SDS_CURDWG;
	db_dictionary* pDictGroup = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(), "ACAD_GROUP", 0);
	db_dictionaryrec* pCurRec;
	pDictGroup->get_recllends(&pCurRec, NULL);
	int scroll = 0;
	char scrollAnswer[IC_ACADBUF];
	while(pCurRec != NULL)
	{

		if(!((CDbGroup*)pCurRec->hiref->ret_hip(pDrawing))->ret_deleted())
		{
			sds_printf("\n%s: %s", ResourceString(IDC_GROUP_LISTNAME, "Group name"), pCurRec->name);
			sds_printf("\n%s: %s", ResourceString(IDC_GROUP_LISTDESC, "Group description"),
				((CDbGroup*)pCurRec->hiref->ret_hip(pDrawing))->description());
			sds_printf("\n%s: %s", ResourceString(IDC_GROUP_LISTSELECTABLE, "Selectable"),
				((CDbGroup*)pCurRec->hiref->ret_hip(pDrawing))->isSelectable() ? ResourceString(IDC_GROUP_OPTIONYES, "Yes") : ResourceString(IDC_GROUP_OPTIONNO, "No"));

			if(!scroll)
			{
				sds_initget(0, ResourceString(IDC_GROUP_SCROLL, "Scroll"));
				scroll = sds_getkword(ResourceString(IDC_GROUP_SCROLLPROMPT, "\nScroll/<ENTER to show next group>: "), scrollAnswer);
				if(scroll == RTNORM)
					scroll = 1;
				else
					if(scroll == RTNONE)
						scroll = 0;
					else
						break;
			}
		}
		pCurRec = pCurRec->next;
	}
	return 1;
}

int cmd_redrawSS(sds_name ss, int mode)
{
	long l;
	sds_name entity;
	sds_sslength(ss, &l);
	for(long i = 0; i < l; ++i)
	{
		sds_ssname(ss, i, entity);
		sds_redraw(entity, mode);
	}
	return 1;
}

int cmd_reverseSSOrder(sds_name ssOld, sds_name ssNew)
{
	long l;
	sds_name entity;
	sds_sslength(ssOld, &l);
	for(long i = 0; i < l; ++i)
	{
		sds_ssname(ssOld, i, entity);
		sds_ssadd(entity, ssNew, ssNew);
	}
	return 1;
}

int cmd_addSSToGroup(sds_name ss, sds_name sdsGroupName)
{
	long ssLength;
	sds_sslength(ss, &ssLength);
	if(ssLength <= 0)
	{
		ASSERT(FALSE);
		return 0;
	}

	sds_name sdsName;
	struct resbuf* pEntities[2] = {NULL, NULL};
	struct resbuf* pCurPair;
	//struct resbuf* pCurEntity;
	//struct resbuf* pCurEntityPair;
	for(int i = 0; i < ssLength; ++i)
	{
		sds_ssname(ss, i, sdsName);
		pCurPair = sds_buildlist(340, sdsName, RTNONE);
		if(pEntities[0] == NULL)
			pEntities[0] = pCurPair;
		else
			pEntities[1]->rbnext = pCurPair;
		pEntities[1] = pCurPair;

		/* DP: uncomment this when things with reactors became better
		pCurEntity = sds_entget(sdsName);
		if(pCurEntity == NULL)
			break;

		pCurEntityPair = pCurEntity;
		while(pCurEntityPair->rbnext != NULL && pCurEntity->rbnext->restype != 102)
			pCurEntityPair = pCurEntityPair->rbnext;
		if(pCurEntityPair->rbnext == NULL)
			pCurEntityPair->rbnext = sds_buildlist(102, "{ACAD_REACTORS",
				330, sdsGroupName,
				102, "}",
				RTNONE);
		else
		{
			ASSERT(pCurEntityPair->rbnext->resval.rstring[0] == '{');
			pCurPair = pCurEntityPair->rbnext->rbnext;
			pCurEntityPair->rbnext = sds_buildlist(330, sdsGroupName, RTNONE);
			pCurEntityPair->rbnext->rbnext = pCurPair;
		}
		if(sds_entmod(pCurEntity) != RTNORM)
			break;
		sds_relrb(pCurEntity);
		*/
	}
	if(i != ssLength)
	{
		ASSERT(FALSE);
		sds_relrb(pEntities[0]);
		//sds_relrb(pCurEntity);
		return 0;
	}

	struct resbuf* pGroup = sds_entget(sdsGroupName);
	if(pGroup == NULL)
	{
		ASSERT(FALSE);
		return 0;
	}
	pCurPair = pGroup;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 340)
		pCurPair = pCurPair->rbnext;
	if(pCurPair->rbnext != NULL)
		while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 340)
			pCurPair = pCurPair->rbnext;
	pEntities[1]->rbnext = pCurPair->rbnext;
	pCurPair->rbnext = pEntities[0];

	if(sds_entmod(pGroup) != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pGroup);
		return 0;
	}

	sds_relrb(pGroup);
	return 1;
}

int cmd_removeSSFromGroup(sds_name ss, sds_name sdsGroupName)
{
	long ssLength;
	sds_sslength(ss, &ssLength);
	if(ssLength <= 0)
	{
		ASSERT(FALSE);
		return 0;
	}

	sds_name sdsName;
	struct resbuf* pRemovedPair = NULL;
	struct resbuf* pCurPair;
	//struct resbuf* pCurEntity;
	//struct resbuf* pCurEntityPair;

	struct resbuf* pGroup = sds_entget(sdsGroupName);
	if(pGroup == NULL)
	{
		ASSERT(FALSE);
		return 0;
	}
	for(int i = 0; i < ssLength; ++i)
	{
		sds_ssname(ss, i, sdsName);
		pCurPair = pGroup;
		while(pCurPair->rbnext != NULL)
		{
			if(pCurPair->rbnext->restype == 340 &&
				pCurPair->rbnext->resval.rlname[0] == sdsName[0] &&
				pCurPair->rbnext->resval.rlname[1] == sdsName[1])
			{
				pRemovedPair = pCurPair->rbnext;
				pCurPair->rbnext = pRemovedPair->rbnext;
				pRemovedPair->rbnext = NULL;
				sds_relrb(pRemovedPair);

				/* DP: uncomment this when things with reactors became better
				pCurEntity = sds_entget(sdsName);
				if(pCurEntity == NULL)
				{
					sds_relrb(pGroup);
					return 0;
				}
				pCurEntityPair = pCurEntity;
				while(pCurEntityPair->rbnext != NULL && pCurEntityPair->rbnext->restype != 102)
					pCurEntityPair = pCurEntityPair->rbnext;

				if(pCurEntityPair->rbnext != NULL)
				{
					pCurEntityPair = pCurEntityPair->rbnext;
					ASSERT(pCurEntityPair->resval.rstring[0] == '{');
					while(pCurEntityPair->rbnext->restype == 330 &&
						!(pCurEntityPair->rbnext->resval.rlname[0] == sdsGroupName[0] &&
						pCurEntityPair->rbnext->resval.rlname[1] == sdsGroupName[1]))
						pCurEntityPair = pCurEntityPair->rbnext;

					if(pCurEntityPair->rbnext->restype == 330)
					{
						pRemovedPair = pCurEntityPair->rbnext;
						pCurEntityPair->rbnext = pRemovedPair->rbnext;
						pRemovedPair->rbnext = NULL;
						sds_relrb(pRemovedPair);
						if(sds_entmod(pCurEntity) != RTNORM)
						{
							sds_relrb(pCurEntity);
							sds_relrb(pGroup);
							return 0;
						}
					}
				}
				sds_relrb(pCurEntity);
				*/
				break;
			}
			pCurPair = pCurPair->rbnext;
		}
	}

	if(sds_entmod(pGroup) != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pGroup);
		return 0;
	}

	sds_relrb(pGroup);
	return 1;
}

int cmd_getSSFromGroup(sds_name ss, sds_name sdsGroupName)
{
	struct resbuf* pCurPair;
	struct resbuf* pGroup = sds_entget(sdsGroupName);
	if(pGroup == NULL)
	{
		ASSERT(FALSE);
		return 0;
	}
	pCurPair = pGroup;
	while(pCurPair != NULL)
	{
		if(pCurPair->restype == 340)
			sds_ssadd(pCurPair->resval.rlname, ss, ss);
		pCurPair = pCurPair->rbnext;
	}
	sds_relrb(pGroup);
	return 1;
}

int cmd_findGroup(const char* pName, sds_name sdsGroupName)
{
	db_dictionary* pDictGroup = (db_dictionary*)SDS_CURDWG->dictsearch(SDS_CURDWG->namedobjdict(), "ACAD_GROUP", 0);
	ASSERT(pDictGroup != NULL);
	db_handitem* pGroup = SDS_CURDWG->dictsearch(pDictGroup, const_cast<char*>(pName), 0);
	if(pGroup == NULL)
		return 0;

	ASSERT(pGroup->ret_type() == DB_GROUP);
	if(sdsGroupName != NULL)
	{
		sdsGroupName[0] = (long)pGroup;
		sdsGroupName[1] = (long)SDS_CURDWG;
	}
	return 1;
}

int cmd_findGroup(sds_name ssGroups, sds_name entity)
{
	if(entity[0] == 0 || entity[1] == 0)
		return 0;

	db_dictionary* pDictGroup = (db_dictionary*)((db_drawing*)entity[1])->dictsearch(((db_drawing*)entity[1])->namedobjdict(),
		"ACAD_GROUP", 0);
	ASSERT(pDictGroup != NULL);

	long l = 0;
	db_dictionaryrec* pCurRec;
	CDbGroup* pGroup;
	sds_name sdsGroupName;
	pDictGroup->get_recllends(&pCurRec, NULL);
	while(pCurRec != NULL)
	{
		pGroup = (CDbGroup*)pCurRec->hiref->ret_hip((db_drawing*)entity[1]);
		if(!pGroup->ret_deleted() &&
			pGroup->findEntity(((db_handitem*)entity[0])->RetHandlePtr(), NULL))
		{
			sdsGroupName[0] = (long)pGroup;
			sdsGroupName[1] = entity[1];
			sds_ssadd(sdsGroupName, ssGroups, ssGroups);
			++l;
		}
		pCurRec = pCurRec->next;
	}
	return l;
}

int cmd_createGroup(const char* pName, const char* pDescription, int selectable, int unnamed, sds_name sdsGroupName)
{
	db_drawing* pDrawing = SDS_CURDWG;
	db_dictionary* pDict = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(), "ACAD_GROUP", 0);
	ASSERT(pDict != NULL);

	sds_name sdsDictName;
	sdsDictName[0] = (long)pDict;
	sdsDictName[1] = (long)pDrawing;

	// create group object
	struct resbuf* pGroup = sds_buildlist(RTDXF0, "GROUP",
		102, "{ACAD_REACTORS",
		330, sdsDictName,
		102, "}",
		300, pDescription,
		70, unnamed,
		71, selectable,
		RTNONE);
	int result = sds_entmakex(pGroup, sdsGroupName);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pGroup);
		return 0;
	}
	sds_relrb(pGroup);

	// add object to group dictionary
	struct resbuf* pDictGroup = sds_entget(sdsDictName);
	if(pDictGroup == NULL)
	{
		ASSERT(FALSE);
		sds_relrb(pDictGroup);
		return 0;
	}

	char name[IC_ACADBUF];
	if(unnamed)
		sprintf(name, "*A%d", pDict->freeIndex());
	pGroup = sds_buildlist(3, unnamed ? name : pName,
		350, sdsGroupName, RTNONE);
	struct resbuf* pCurPair = pDictGroup;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 350)
		pCurPair = pCurPair->rbnext;

	if(pCurPair->rbnext == NULL)
		pCurPair->rbnext = pGroup;
	else
	{
		ASSERT(pCurPair->restype == 3);
		pGroup->rbnext->rbnext = pCurPair->rbnext->rbnext;
		pCurPair->rbnext->rbnext = pGroup;
	}
	pGroup = NULL;
	result = sds_entmod(pDictGroup);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pDictGroup);
		return 0;
	}
	sds_relrb(pDictGroup);
	return 1;
}

int cmd_modifyGroup(const char* pNewName, const char* pNewDescription, int selectable, sds_name sdsGroupName)
{
	struct resbuf* pCurPair = NULL;
	char* pOld = NULL;
	// change name in dictionary
	if(pNewName != NULL)
	{
		db_drawing* pDrawing = (db_drawing*)sdsGroupName[1];
		db_dictionary* pDictGroup = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(),
			"ACAD_GROUP", 0);
		sds_name sdsDictName;
		sdsDictName[0] = (long)pDictGroup;
		sdsDictName[1] = sdsGroupName[1];

		struct resbuf* pDict = sds_entget(sdsDictName);
		pCurPair = pDict;
		while(pCurPair->rbnext != NULL)
		{
			if(pCurPair->rbnext->restype == 350 &&
				pCurPair->rbnext->resval.rlname[0] == sdsGroupName[0] &&
				pCurPair->restype == 3)
			{
				pOld = pCurPair->resval.rstring;
				pCurPair->resval.rstring = const_cast<char*>(pNewName);
				break;
			}
			pCurPair = pCurPair->rbnext;
		}
		if(pCurPair == NULL || sds_entmod(pDict) != RTNORM)
		{
			ASSERT(FALSE);
			if(pCurPair != NULL)
				pCurPair->resval.rstring = pOld;
			sds_relrb(pDict);
			return 0;
		}
		pCurPair->resval.rstring = pOld;
		sds_relrb(pDict);
	}

	// change description
	pCurPair = NULL;
	pOld = NULL;
	struct resbuf* pGroup = sds_entget(sdsGroupName);
	struct resbuf* pDescPair = NULL;
	if(pNewDescription != NULL)
	{
		pCurPair = pGroup;
		while(pCurPair != NULL)
		{
			if(pCurPair->restype == 300)
			{
				pDescPair = pCurPair;
				pOld = pDescPair->resval.rstring;
				pDescPair->resval.rstring = const_cast<char*>(pNewDescription);
				break;
			}
			pCurPair = pCurPair->rbnext;
		}
		if(pDescPair == NULL)
		{
			sds_relrb(pGroup);
			return 0;
		}
	}

	pCurPair = NULL;
	if(selectable != -1)
	{
		pCurPair = pGroup;
		while(pCurPair != NULL)
		{
			if(pCurPair->restype == 71)
			{
				pCurPair->resval.rint = selectable;
				break;
			}
			pCurPair = pCurPair->rbnext;
		}
		if(pCurPair == NULL)
		{
			sds_relrb(pGroup);
			return 0;
		}
	}

	if(pDescPair != NULL || pCurPair != NULL)
	{
		if(sds_entmod(pGroup) != RTNORM)
		{
			ASSERT(FALSE);
			if(pOld != NULL)
				pDescPair->resval.rstring = pOld;
			sds_relrb(pGroup);
			return 0;
		}
		if(pOld != NULL)
			pDescPair->resval.rstring = pOld;
	}

	sds_relrb(pGroup);
	return 1;
}

int cmd_reverseGroupOrder(sds_name sdsGroupName)
{
	struct resbuf* pGroup = sds_entget(sdsGroupName);
	if(pGroup == NULL)
	{
		ASSERT(FALSE);
		return 0;
	}

	int result = 1;
	struct resbuf* pCurPair = pGroup;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 340)
		pCurPair = pCurPair->rbnext;

	if(pCurPair->rbnext == NULL)
	{
		ASSERT(FALSE);
		sds_relrb(pGroup);
		return result;
	}

	struct resbuf* pEntities = pCurPair;
	struct resbuf* pEntity;
	pCurPair = pCurPair->rbnext;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 340)
	{
		pEntity = pEntities->rbnext;
		pEntities->rbnext = pCurPair->rbnext;
		pCurPair->rbnext = pEntities->rbnext->rbnext;
		pEntities->rbnext->rbnext = pEntity;
	}

	if(sds_entmod(pGroup) != RTNORM)
		result = 0;

	ASSERT(result == 1);
	sds_relrb(pGroup);
	return result;
}

int cmd_reorderGroupEntities(sds_name sdsGroupName, int oldPos, int newPos, int count)
{
	struct resbuf* pGroup = sds_entget(sdsGroupName);
	if(pGroup == NULL || (oldPos >= newPos && oldPos + count - 1 <= newPos))
	{
		ASSERT(FALSE);
		return 0;
	}

	int result = 1;
	int index = 0;
	struct resbuf* pCurPair = pGroup;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 340)
		pCurPair = pCurPair->rbnext;

	struct resbuf* pMoveTo = pCurPair;
	struct resbuf* pListToMove[2] = {NULL, NULL};
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 340)
	{
		if(index == oldPos)
			pListToMove[0] = pCurPair;
		if(index == oldPos + count - 1)
			pListToMove[1] = pCurPair->rbnext;
		++index;
		pCurPair = pCurPair->rbnext;
	}
	if(pListToMove[0] == NULL || pListToMove[1] == NULL || pMoveTo == NULL)
	{
		ASSERT(FALSE);
		sds_relrb(pGroup);
		return 0;
	}
	pCurPair = pListToMove[0]->rbnext;
	pListToMove[0]->rbnext = pListToMove[1]->rbnext;
	pListToMove[0] = pCurPair;
	pListToMove[1]->rbnext = NULL;

	index = 0;
	pCurPair = pMoveTo;
	pMoveTo = NULL;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 340)
	{
		if(index == newPos)
			pMoveTo = pCurPair;
		++index;
		pCurPair = pCurPair->rbnext;
	}
	if(pMoveTo == NULL)
	{
		ASSERT(index == newPos - 1);
		pMoveTo = pCurPair;
	}

	pListToMove[1]->rbnext = pMoveTo->rbnext;
	pMoveTo->rbnext = pListToMove[0];

	if(sds_entmod(pGroup) != RTNORM)
		result = 0;

	ASSERT(result == 1);
	sds_relrb(pGroup);
	return result;
}




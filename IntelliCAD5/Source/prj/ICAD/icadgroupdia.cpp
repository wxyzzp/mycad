/* D:\DEV\PRJ\ICAD\ICADGROUPDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


// This is just the skeleton for bringing up the main Group dialog.
// This dialog brings up another one (for ordering entities within groups,
// and that one brings one up that lets you scroll through them by
// highlighting them in the drawing.

// The re-ordering dialog is IDD_GROUP_ORDER
// The highlightling one is	IDD_GROUP_HILITE




//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "paths.h"
#include "IcadDialog.h"
#include "IcadHelp.h"
#include "DialogGroup.h"
#include "DialogGroupOrder.h"
#include "DialogGroupHLight.h"
#include "cmdsGroup.h"
#include "cmds.h"

/*
class grp_CGroup:public IcadDialog {
    public:
        grp_CGroup();
        BOOL OnInitDialog();
    protected:
        afx_msg void OnHelp();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(grp_CGroup,CDialog)
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

void grp_CGroup::OnHelp() {
    char fs1[IC_ACADBUF];

    if( !SearchHelpFile( SDS_COMMANDHELP, fs1) ) {
		MessageBox(ResourceString(IDC_ICADMLEDITDIA_UNABLE_TO__0, "Unable to find the requested help file." ),NULL,MB_OK|MB_ICONEXCLAMATION);
        return;
    }
	CMainWindow::IcadHelp( fs1,ICADHELP_CONTENTS,0);
}

grp_CGroup::OnInitDialog() {

    BOOL ret=IcadDialog::OnInitDialog();

    return(ret);
}

void grp_CGroup::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);


}

grp_CGroup::grp_CGroup():IcadDialog(IDD_GROUP) {

}
*/

//***This is the main window
void CMainWindow::IcadGroupDia(void) 
{
    int action, result;
	CDialogGroup dg(this, SDS_CURDWG);
	char groupCommand[8] = "GROUP";
	int nChanges = 2;

	while(true)
	{
		cmd_redraw();
		action = dg.DoModal();
		if(action == IDOK) 
		{
			// apply all
			break;
		}
		else
		{
			if(action == IDCANCEL)
			{
				// suppress messages
				bool bFromPipeline = SDS_WasFromPipeLine;
				SDS_WasFromPipeLine = true;

				// "trick" undo command for cancel
				SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
				while(--nChanges)
					cmd_u();
				// this also clears redo stack
				SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
				cmd_redraw();

				SDS_WasFromPipeLine = bFromPipeline;
				break;
			}
			else
			{
				// perform action and return into dialog
				sds_name entities;
				sds_name group;
				switch(action)
				{
				case CDialogGroup::eACTION_HIGHLIGHT:
					{
						dg.getCurrentGroup(group);

						cmd_getSSFromGroup(entities, group);
						cmd_redrawSS(entities, IC_REDRAW_HILITE); 
						MessageBox(ResourceString(IDC_GROUP_PRESSOK, "Press Ok to continue."));
						cmd_redrawSS(entities, IC_REDRAW_UNHILITE); 
						break;
					}
				case CDialogGroup::eACTION_ADD:
					{
						dg.getCurrentGroup(group);
						cmd_getSSFromGroup(entities, group);
						cmd_redrawSS(entities, IC_REDRAW_HILITE);
						sds_ssfree(entities);
						sds_pmtssget("", NULL, NULL, NULL, NULL, entities, 1);

						/*
						SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						if(cmd_addSSToGroup(entities, group))
							++nChanges;
						SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						*/
						cmd_addSSToGroup(entities, group);

						sds_ssfree(entities);
						cmd_getSSFromGroup(entities, group);
						cmd_redrawSS(entities, IC_REDRAW_UNHILITE); 
						break;
					}
				case CDialogGroup::eACTION_REMOVE:
					{
						dg.getCurrentGroup(group);
						cmd_getSSFromGroup(entities, group);
						cmd_redrawSS(entities, IC_REDRAW_HILITE);
						sds_ssfree(entities);

						// PICKSTYLE should be cleared before
						int pickstyle;
						struct resbuf ps;
						sds_getvar("PICKSTYLE", &ps);
						pickstyle = ps.resval.rint;
						ps.resval.rint = 0;
						sds_setvar("PICKSTYLE", &ps);
						
						sds_pmtssget("", NULL, NULL, NULL, NULL, entities, 1);

						/*
						SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						if(cmd_removeSSFromGroup(entities, group))
							++nChanges;
						SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						*/
						cmd_removeSSFromGroup(entities, group);
						
						sds_ssfree(entities);
						cmd_getSSFromGroup(entities, group);
						cmd_redrawSS(entities, IC_REDRAW_UNHILITE);
						
						// restore PICKSTYLE
						ps.resval.rint = pickstyle;
						sds_setvar("PICKSTYLE", &ps);
						break;
					}
				case CDialogGroup::eACTION_FINDBYENTITY:
					{
						// PICKSTYLE should be cleared before
						int pickstyle;
						struct resbuf ps;
						sds_getvar("PICKSTYLE", &ps);
						pickstyle = ps.resval.rint;
						ps.resval.rint = 0;
						sds_setvar("PICKSTYLE", &ps);

						sds_name entity;
						result = sds_pmtssget("", "", NULL, NULL, NULL, entities, 1);
						sds_ssname(entities, 0, entity);
						cmd_findGroup(group, entity);
						CDialogGroupOrder dgo(this, SDS_CURDWG, group);
						// show groups to which entity belongs
						dgo.DoModal();
						sds_redraw(entity, IC_REDRAW_UNHILITE);
						sds_ssfree(group);

						// restore PICKSTYLE
						ps.resval.rint = pickstyle;
						sds_setvar("PICKSTYLE", &ps);
						break;
					}
				case CDialogGroup::eACTION_REORDER:
					{
						// copy groups
						++nChanges;
						SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						CDialogGroupOrder dgo(this, SDS_CURDWG);
						while(true)
						{
							sds_ssfree(entities);
							action = dgo.DoModal();
							if(action == IDOK)
							{
								// apply all
								SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
								if(dgo.isDidReorder())
									++nChanges;
								break;
							}
							else
							{
								if(action == IDCANCEL)
								{
									bool bFromPipeline = SDS_WasFromPipeLine;
									SDS_WasFromPipeLine = true;

									SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
									if(dgo.isDidReorder())
										cmd_u();

									SDS_WasFromPipeLine = bFromPipeline;
									break;
								}
								else
									if(action == CDialogGroupOrder::eACTION_HIGHLIGHT)
									{
										dgo.getCurrentGroup(group);

										sds_name ss;
										cmd_getSSFromGroup(ss, group);
										cmd_reverseSSOrder(ss, entities);
										sds_ssfree(ss);

										CDialogGroupHLight dghl(this, entities);
										dghl.DoModal();
										cmd_redrawSS(entities, IC_REDRAW_UNHILITE);
									}
							}
						}
						SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)groupCommand, NULL, NULL, SDS_CURDWG);
						break;
					}
				default:
					ASSERT(FALSE);
				}
				sds_ssfree(entities);
			}
		}
	}
}






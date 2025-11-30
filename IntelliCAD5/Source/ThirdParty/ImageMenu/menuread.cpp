#include "stdafx.h"
#include "sds.h"
#include "menuitem.h"
#include "menupage.h"
#include "menuread.h"

extern char g_Cmd[];
CMenuPage g_Page;

CReadMenu::CReadMenu()
{
	m_page = 0;
}

CReadMenu::~CReadMenu()
{
}

short CReadMenu::Do(const char *psMenu, const char *psSection)
{
	// Find the ***IMAGE section of the menu
	char sBuffer[256];
	strcpy(sBuffer, "");
	CStdioFile fmenu;
	TRY {
		fmenu.Open(psMenu, CFile::modeRead);
	} CATCH(CFileException, e) {
		sds_printf("\nError opening file %s\n", psMenu);
		return FALSE;
	} END_CATCH
	while(strnicmp(sBuffer, "***IMAGE", 8)) {
		TRY {
			if (fmenu.ReadString(sBuffer, 255) == NULL) {
				fmenu.Close();
				sds_printf("\n***IMAGE Section not found\n");
				return FALSE;
			}
		} CATCH(CFileException, e) {
			fmenu.Close();
			sds_printf("\nFile Read Error\n");
			return FALSE;
		} END_CATCH
	}

	g_Page.Reset();

	// Find the image section
	strcpy(sBuffer, "");
	while(strcmp(sBuffer, psSection)) {
		TRY {
			if (fmenu.ReadString(sBuffer, 255) == NULL) {
				fmenu.Close();
				sds_printf("\nSection %s not found\n", psSection);
				return FALSE;
			}
		} CATCH(CFileException, e) {
			sds_printf("\nFile ReadError\n");
			fmenu.Close();
			return FALSE;
		} END_CATCH
	}

	int dkey;  // dialog key
	sds_hdlg handle; // dialog handle
	if (dlg_load_dialog("image.dcl", &dkey) != RTNORM) {
		sds_printf("\nimage.dcl was not found\n");
		return FALSE;
	}
	if (dlg_new_dialog("imageDlg", dkey, dlg_callback, &handle) != RTNORM) {
		dlg_unload_dialog(dkey);
		sds_printf("\nDialog creation failed\n");
		return FALSE;
	}

	short rslt = InitDialog(&fmenu, handle);
	fmenu.Close();
	if (rslt == FALSE) {
		sds_printf("\nError Initializing Dialog\n");
		return FALSE;
	}

	int status;
	dlg_start_dialog(handle, &status);
	dlg_unload_dialog(dkey);
	if (status == DLGOK) {
		m_page = 0;
		return TRUE;
	}
	if (status == DLGSTATUS) { // Image Double Click
		m_page = 0;
		return TRUE;
	}

	if (status == DLGSTATUS + 1) { // Next Button
		m_page++;
		strcpy(g_Cmd, "");
		return TRUE;
	}
	if (status == DLGSTATUS + 2) { // Previous Button
		m_page--;
		strcpy(g_Cmd, "");
		return TRUE;
	}
	return FALSE;
}


short CReadMenu::InitDialog(CStdioFile *pfmenu, sds_hdlg handle)
{
	// Set Action Callbacks
	dlg_action_tile(handle, "accept", acceptCB);
	dlg_action_tile(handle, "cancel", rejectCB);
	dlg_action_tile(handle, "next", nextCB);
	dlg_action_tile(handle, "prev", prevCB);
	dlg_action_tile(handle, "list", listCB);

	char sBuffer[256];
	char sLine[256];
	// Get the Title of this Image page
	TRY {
		if (pfmenu->ReadString(sBuffer, 255) == NULL)
			return FALSE;
	} CATCH(CFileException, e) {
		return FALSE;
	} END_CATCH

	char sSection[256];
	strcpy(sSection, sBuffer + 1);
	sSection[strlen(sSection) - 2] = '\0';


	// Set Dialog Title
	SetWindowText((HWND)handle, sSection);

	char *psTemp;
	char sTile[10];
	strcpy(sTile, "img1");
	short width, height, test;
	short slides = 0;
	strcpy(sBuffer, "");
	CMenuItem item;


	// Move up pages
	for (short idx = 0; idx < m_page * 20; idx++) {
		TRY {
			if (pfmenu->ReadString(sLine, 255) == NULL)
				return FALSE;
		} CATCH(CFileException, e) {
			return FALSE;
		} END_CATCH
	}

	// Start List
	dlg_start_list(handle, "list", LIST_APPEND, 0);

	if (m_page) // Enable Previous Button
		dlg_mode_tile(handle, "prev", MODE_ENABLE);
	else
		dlg_mode_tile(handle, "prev", MODE_DISABLE);

	// Loop through image definitions
	for (idx = 0; ; idx++) {
		if (idx > 19) {
			dlg_mode_tile(handle, "next", 0);
			break;
		}
		// Get Library Slide Name
		TRY {
			if (pfmenu->ReadString(sLine, 255) == NULL)
				break;
		} CATCH(CFileException, e) {
			break;
		} END_CATCH
		if ((sLine[0] == '*' && sLine[1] == '*') || strlen(sLine) < 3)
			break;
		item.Parse(sLine);
		g_Page.Set(idx, &item);

		// Add Item
		psTemp = item.Item();
		if (psTemp)
			dlg_add_list(psTemp);

		// Add slide
		psTemp = item.Slide();
		if (psTemp) {
			slides++;
			itoa(slides, sTile + 3, 10);
			// have to do dlg_dimensions_tile twice in release mode to get succes!
			dlg_dimensions_tile(handle, sTile, &test, &height);
			dlg_dimensions_tile(handle, sTile, &width, &height);
			dlg_start_image(handle, sTile);
			dlg_slide_image(0, 0, width, height, psTemp);
			dlg_end_image();
		}
		item.Reset();
	}

	if (idx < 19) // No other page
		dlg_mode_tile(handle, "next", MODE_DISABLE);

	// End List
	dlg_end_list();

	return TRUE;
}

void dlg_callback(sds_callback_packet *cpkt)
{
	char sBuffer[256];
	CMenuItem *pItem;
	short idx = 1;
	strcpy(sBuffer, "");
	dlg_get_attr_string(cpkt->tile, "key", sBuffer, 255);
	if (strnicmp(sBuffer, "img", 3))
		return;
	if (cpkt->reason == CBR_DOUBLE_CLICK) {
		idx = atoi(sBuffer + 3);
		pItem = g_Page.Get(idx - 1);
		if (pItem)
			strcpy(g_Cmd, pItem->Cmd());
		else
			strcpy(g_Cmd, "");
		dlg_done_dialog(cpkt->dialog, DLGOK);
	}
	//Modified SAU	28/6/2000	   [
	//Reason:select an image tile, the corresponding item in the list must become highlighted.
	if (cpkt->reason == CBR_SELECT) {
		idx = atoi(sBuffer + 3);
		pItem = g_Page.Get(idx - 1);
		if (pItem) {
			itoa(--idx,sBuffer,10);
			dlg_set_tile(cpkt->dialog,"list",sBuffer);	 
		}	
	}
	//Modified SAU	28/6/2000	 ]
}

void acceptCB(sds_callback_packet *cpkt)
{
	char sBuffer[10];
	dlg_get_tile(cpkt->dialog, "list", sBuffer, 9);
	short idx = 0;
	CMenuItem *pItem;
	if (strlen(sBuffer)) {
		idx = atoi(sBuffer);
		pItem = g_Page.Get(idx);
		if (pItem)
			strcpy(g_Cmd, pItem->Cmd());
		else
			strcpy(g_Cmd, "");
		dlg_done_dialog(cpkt->dialog, DLGOK);
	}
}

void rejectCB(sds_callback_packet *cpkt)
{
	dlg_done_dialog(cpkt->dialog, DLGCANCEL);
}

void nextCB(sds_callback_packet *cpkt)
{
	dlg_done_dialog(cpkt->dialog, DLGSTATUS + 1);
}

void prevCB(sds_callback_packet *cpkt)
{
	dlg_done_dialog(cpkt->dialog, DLGSTATUS + 2);
}

void listCB(sds_callback_packet *cpkt)
{
	CMenuItem *pItem;
	char sBuffer[10];
	short idx = 0;
	if(cpkt->reason == CBR_DOUBLE_CLICK) {
		idx = atoi(cpkt->value);
		pItem = g_Page.Get(idx);
		if (pItem)
			strcpy(g_Cmd, pItem->Cmd());
		else
			strcpy(g_Cmd, "");
		dlg_done_dialog(cpkt->dialog, DLGOK);
	}
	//Modified SAU	28/6/2000		[
	//Reason:select an item in the list, the correspondig image tile  must become highlighted.
	if(cpkt->reason == CBR_SELECT) {
		idx = atoi(cpkt->value);
		pItem = g_Page.Get(idx);
		if (pItem) {
			strcpy(sBuffer,"img");
			itoa(++idx,sBuffer+3,10);
			dlg_mode_tile(cpkt->dialog,sBuffer,2); 
		}
	}
	//Modified SAU	28/6/2000		]
	
}

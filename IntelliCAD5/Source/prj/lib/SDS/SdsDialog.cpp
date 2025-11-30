/* F:\RC98C\PRJ\API\SDS\SDSDIALOG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 *	Class to facilitate creation of modal dialogs from SDS applications
 *	written as MFC Extension DLLs.
 *
 *	$Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:40 $
 *
 */

#include <afxdlgs.h>
#include "sds.h"
#include "SdsDialog.h"

UINT SdsDialog:: m_doModalMsg = 0;

UINT 
SdsDialog:: GetDoModalMsg()
	{
	if ( ! m_doModalMsg )
		m_doModalMsg = RegisterWindowMessage( "VisioSDSDoModal");

	return m_doModalMsg;
	}


int
SdsDialog:: SendDoModalMsg( CDialog *dialog)
	{
	return ::SendMessage( sds_getmainhwnd(), GetDoModalMsg(), (WPARAM)dialog, TRUE);
	}


int
SdsDialog:: SendDoModalMsg( CPropertySheet *sheet)
	{
	return ::SendMessage( sds_getmainhwnd(), GetDoModalMsg(), (WPARAM)sheet, FALSE);
	}


int
SdsDialog:: DoModal()
	{
	return SendDoModalMsg( this);
	}


int
SdsPropertyPage:: DoModal()
	{
	return SdsDialog::SendDoModalMsg( this);
	}


int
SdsPropertySheet:: DoModal()
	{
	return SdsDialog::SendDoModalMsg( this);
	}



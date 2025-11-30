/* LICENSING.CPP
 * Copyright (C) 2001 Bricsnet. All rights reserved.
 */

#include "cmds.h" /*DNT*/
#include "cmdsLicence.h"
#include "IcadApp.h" /*DNT*/

/*----------------------------------------------------------------------------*/
short	cmd_licEnterKey
(
void
)
{
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short status = RTNORM;

#ifdef _DEBUG
	sds_printf("\nEnter license key");
#endif

	return status;
}

/*----------------------------------------------------------------------------*/
short	cmd_licProperties
(
void
)
{
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short status = RTNORM;

#ifdef _DEBUG
	sds_printf("\nLicense properties");
#endif

	return status;
}
/*----------------------------------------------------------------------------*/
short	cmd_licExportKey
(
void
)
{
	short status = RTNORM;

#ifdef _DEBUG
	sds_printf("\nExport license key");
#endif

	return status;
}

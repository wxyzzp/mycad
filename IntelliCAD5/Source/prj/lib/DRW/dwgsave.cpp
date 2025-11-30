/* C:\ICAD\PRJ\LIB\DB\DWGSAVE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

/*DG - 3.2002*/// MFC's map introduced by 4M's change "157" is replaced with STL's one.
// Reason - removing MFC usage from icadlib/db.
#pragma warning (disable : 4786)

#include <string>
#include <map>

#include "db.h"
#include "dimstyletabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "ucstabrec.h"
#include "viewport.h"
#include "objects.h"
#include "drw.h"

#include "drwprotos.h"
#include "DbAcisEntity.h"
#include "DbGroup.h"
#include "DbPlotSettings.h"
#include "DbLayout.h"
#include "DbSortentsTable.h"
#include "Modeler.h"

#include "DWGdirectWriter.h"

char *drw_str_0 		 ="0";
char *drw_str_acad		 ="ACAD";
char *drw_str_continuous ="CONTINUOUS";
char *drw_str_msblockname="*MODEL_SPACE";
char *drw_str_psblockname="*PAPER_SPACE";
char *drw_str_standard	 ="STANDARD";
char *drw_str_acadgroup  ="ACAD_GROUP";
char *drw_str_acadmlsty  ="ACAD_MLINESTYLE";
char *drw_BadAcisMsg	 ="WARNING: This drawing contains ACIS entities without image data. \n" \
						  "This drawing was most likely created from a DXF file via DXFIN. \n" \
						  "The ACIS entities will not be saved in the new drawing (DWG) file. \n" \
						  "To preserve the ACIS entities, save the drawing back to a DXF file.";
char *drw_str_byblock	 ="BYBLOCK";
char *drw_str_bylayer	 ="BYLAYER";
bool drw_GaveAcisDiscardMsg;
int  drw_savetype;
db_handitem *saveltypecachehip=NULL;
db_handitem *savelayercachehip=NULL;

DB_API int drw_readwrite::drw_DWGdirect_Save(db_drawing *dp,long arghipct,db_handitem **arghipp,char *savename,char filetype,int version,short dxfnegz,short dxfdecprec,short dxfwritezeroes,char r12dxfvbls,int argmakebak,db_charbuflink *cfgbuf, bool bUsePasswd, bool bSavePreview) 
{
	int rc = TRUE;

	if (argmakebak) 
		ic_makebak(savename);

	OdRxObjectImpl<DWGdirectWriterServices> exServices;

	int saveroundtrip;
	db_qgetvar(DB_QSAVEROUNDTRIP,cfgbuf->buf,&saveroundtrip,DB_INT,0);
	exServices.setSAVEROUNDTRIP((bool)(saveroundtrip == 1));

	DWGdirectWriter DWGWriter(exServices);
	bool result = DWGWriter.writeDiskFile(savename, dp, filetype, version, bUsePasswd, bSavePreview);
	ASSERT(result == true);
	if (result == false)
	{
		return FALSE;
	}

	return rc;
}

// This function returns the same drawing that was passed in if there were no errors.
db_drawing* drw_readwrite::drw_DWGdirect_Audit(db_drawing *dp, bool fixErrors, short writeLogFile) 
{
	int rc = TRUE;

	OdRxObjectImpl<DWGdirectWriterServices> exServices;

	DWGdirectWriter DWGWriter(exServices);
	db_drawing* retDWG = DWGWriter.auditFile(dp, fixErrors, writeLogFile);

	return retDWG;
}
